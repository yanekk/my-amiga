#include <hardware/cia.h>
#include <proto/graphics.h>

#include <hardware/intbits.h>
#include <hardware/dmabits.h>
#include <string.h>

#include "common/structs.h"
#include "common/utils.h"
#include "common/custom_chip.h"
#include "sprite/common.h"
#include "sprite/explosion.h"
#include "support/gcc8_c_support.h"
#include "system/interrupts.h"
#include "system/system.h"
#include "system/screen.h"
#include "copper/screen.h"
#include "copper/display.h"
#include "effects/bounce.h"
#include "blitter/copy.h"
#include "music/p61.h"

#define IMG_H 72

#define FONT_LETTER_WIDTH 16
#define FONT_LETTER_HEIGHT 16
#define FONT_LETTERS_PER_LINE 18 * FONT_LETTER_WIDTH

#define SCROLL_SPEED 2

#define LINE_TOP (0x4c-6)
#define LINE_BOTTOM (0x4c+IMG_H+1)

#define LINE_START 0x07
#define LINE_END 0xdf

#define LINES 8

#define COPPERLIST_SIZE 512
#define MUSIC 0

INCBIN(imageData, ".\\\\assets\\\\image.164x72.raw");
INCBIN(imageColors, ".\\\\assets\\\\palette.raw");

INCBIN_CHIP(fontData, ".\\\\assets\\\\font.288x96.raw");
INCBIN(fontColors, ".\\\\assets\\\\font-palette.raw");

INCBIN_CHIP(music, ".\\\\assets\\\\P61.mod.my-1st-module.1")

typedef struct CopperLine {
    WORD vhpos;
    WORD _mask;
    WORD _colorRegister;
    WORD colorValue;
} CopperLine;

CopperLine* lines[LINES];
LONG line_colors[LINES] = { 0x444, 0x777, 0xbbb, 0xfff, 0xbbb, 0x777, 0x444, 0x000 };

USHORT line = (LINE_TOP + LINE_BOTTOM)/2;
WORD line_direction = 1;

const UWORD __chip nullSpriteData[] = {
    0, 0,
    0, 0
};

struct Explosion *explosion;
struct MySprite *nullSprite;

struct SystemData systemData;
struct Display display;
struct NewScreen logoScreen;
struct NewScreen textScreen;
struct NewScreen fontScreen;

struct Bounce bounce;

UWORD scrollCounter = 0;
UWORD *bottomScreen;

const char* letters = "GREETINGS FROM THE OLD AMIGA COMPUTER!   ";
USHORT letterIndex = 0;
SHORT letterCount;
SHORT adj = 0;

UWORD plotY = 100;
UWORD plotX;


static void Scroll() {
    Screen_SetY(&textScreen, bounce.y);
    Bounce_Update(&bounce);

    if(scrollCounter == 0) {
        USHORT letterNumber = letters[letterIndex] - 0x1e;

        Blitter_CopyAtoB(&fontScreen,
            letterNumber % FONT_LETTERS_PER_LINE,
            letterNumber / FONT_LETTERS_PER_LINE,
            FONT_LETTER_WIDTH, FONT_LETTER_HEIGHT,
            &textScreen, plotX, plotY);

        letterIndex++;
        if(letterIndex >= letterCount) letterIndex = 0;
    }
    
    scrollCounter += SCROLL_SPEED;
    if(scrollCounter == FONT_LETTER_WIDTH)
        scrollCounter = 0;
    Blitter_ShiftALeft(&textScreen, plotX, plotY, 
        textScreen.Width, FONT_LETTER_HEIGHT, SCROLL_SPEED);
}

static void MoveLine() 
{
    if(line == LINE_TOP || line == LINE_BOTTOM)
        line_direction = -line_direction;
    line += line_direction;

    for(SHORT i = 0; i < LINES; i++) {
        lines[i]->vhpos = CPLINE(line+i, LINE_START);
    }
}

static __interrupt void OnVBlank() {
    custom->intreq = INTF_VERTB;
    if(MUSIC)
        p61Music();
}

UWORD imageSizeWithMargin;

static UWORD* CreateDisplay(UWORD* copPtr) {
    // display settings
    display.Width = 320;
    display.Height = 256;
    display.TopMargin = 76;
    display.LeftMargin = 110;
    return Display_Create(copPtr, &display);
}

static UWORD* CreateLogoScreen(UWORD* copPtr) {
    // logo
    logoScreen.Width = 176;
    logoScreen.Height = 86;
    logoScreen.Bitplanes = 3;
    logoScreen.Palette = (UWORD*)imageColors;
    logoScreen.Display = &display;

    UWORD imageSize = Screen_Size(&logoScreen);

    imageSizeWithMargin = imageSize + Screen_RowWidth(&logoScreen) * logoScreen.Bitplanes * 10;
    logoScreen.Data = AllocMem(imageSizeWithMargin, MEMF_CHIP | MEMF_CLEAR);
    CopyMem(imageData, logoScreen.Data, imageSize);    

    return Screen_Create(copPtr, &logoScreen);
}

static UWORD* CreateTextScreen(UWORD* copPtr) {
    textScreen.Width = 352;
    textScreen.Height = display.Height;
    textScreen.Bitplanes = 3;
    textScreen.Size = Screen_Size(&textScreen);

    bottomScreen = AllocMem(textScreen.Size, MEMF_CHIP | MEMF_CLEAR);
    /*UWORD c = 0;
    for(ULONG i = 0; i < textScreen.Size; i++) {
        bottomScreen[i] = c;
        c++;
    }*/

    textScreen.Palette = fontColors;    
    textScreen.Display = &display;
    textScreen.Data = bottomScreen;

    return Screen_Create(copPtr, &textScreen);
}

static void CreateFontScreen() {
    fontScreen.Width = 288;
    fontScreen.Height = 96;
    fontScreen.Data = fontData;
    fontScreen.Bitplanes = 3;
    fontScreen.BitplaneSize = Screen_BitplaneSize(&fontScreen);
    fontScreen.Size = Screen_Size(&fontScreen);
    fontScreen.RowWidth = Screen_RowWidth(&fontScreen);
}

static void InitializeTextPlotting() {
    plotX = textScreen.Width - FONT_LETTER_WIDTH;
}

int main() 
{
    SysBase = *((struct ExecBase**)4UL);
    line_colors[LINES-1] = ((UWORD*)imageColors)[0];

    letterCount = strlen(letters);

    APTR copinit = AllocMem(COPPERLIST_SIZE, MEMF_CHIP);
    UWORD* copPtr = copinit;

    CreateFontScreen();

    copPtr = CreateDisplay(copPtr);
    copPtr = CreateLogoScreen(copPtr);

    // sprite
    CPMOVE(copPtr, COLOR17, 0x10C);
    CPMOVE(copPtr, COLOR18, 0x06C);
    CPMOVE(copPtr, COLOR19, 0x0AC);

    explosion = Explosion_Create(128, 220);
    explosion->copperListSpritePointer = copPtr;
    CPMOVE_L(copPtr, SPR0PTH, (ULONG)Explosion_CurrentFrame(explosion)->sprite);

    nullSprite = AllocMySprite((struct MySprite) {
        .vStart = 0x2a,
        .vStop  = 0x2b,
        .hStart = 0x20,
        .flags  = 0x0
    }, nullSpriteData, sizeof(nullSpriteData));
    CPMOVE_L(copPtr, SPR1PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR2PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR3PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR4PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR5PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR6PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR7PTH, (ULONG)nullSprite);

    // top line
    CPMOVE(copPtr, COLOR00, 0x349);
    CPWAIT(copPtr, CPLINE(0x2b, LINE_START));

    CPMOVE(copPtr, COLOR00, 0x56b);
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START));

    CPMOVE(copPtr, COLOR00, ((UWORD*)imageColors)[0]);

    // copper line
    for(SHORT i = 0; i < LINES; i++) {
        lines[i] = (CopperLine*)copPtr;
        CPWAIT(copPtr, CPLINE(0x80, LINE_START));
        CPMOVE(copPtr, COLOR00, line_colors[i]);
    }

    // second screen
    CPWAIT(copPtr, CPLINE(LINE_BOTTOM+LINES, LINE_END));

    copPtr = CreateTextScreen(copPtr);
    InitializeTextPlotting();
    
    // bottom line
    CPWAIT(copPtr, CPLINE(0xff, LINE_END));
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START));
    CPMOVE(copPtr, COLOR00, 0x56c);

    CPWAIT(copPtr, CPLINE(0x2d, LINE_START));
    CPMOVE(copPtr, COLOR00, 0x349);
    CPEND(copPtr);

    System_Kill(&systemData);

    WaitVbl();

    // initialize custom values
    SetInterruptHandler((APTR)OnVBlank);
    custom->intena = INTF_SETCLR | INTF_VERTB;
    custom->intreq = INTF_VERTB;
    custom->cop1lc = (ULONG)copinit;
    custom->dmacon = DMAF_SETCLR | DMAF_ALL;

    p61Init(music);

    // main loop
	while(1)
	{
        WaitForLine10();
        
        MoveLine();
        Scroll();

        Explosion_NextFrame(explosion);
        Explosion_Move(explosion, 1, 0);
        Explosion_Paint(explosion);
	}

    System_Restore(&systemData);
    
    FreeMem(copinit, COPPERLIST_SIZE);
    FreeMem(bottomScreen, Screen_ByteWidth(&textScreen) * textScreen.Height);
    FreeMem(logoScreen.Data, imageSizeWithMargin);
    
    Explosion_Free(explosion);
    FreeMySprite(nullSprite, sizeof(nullSpriteData));
    return 0;
}
