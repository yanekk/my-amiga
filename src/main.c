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
#include "copper/screen.h"
#include "copper/display.h"

#define SCREEN_BPL_W 352
#define SCREEN_BPL_H 256
#define SCREEN_BITPLANES 3

#define SCREEN_BPL_SIZE (SCREEN_BPL_W/8)
#define SCREEN_BYTE_WIDTH (SCREEN_BPL_SIZE * SCREEN_BITPLANES)
#define SCREEN_SIZE SCREEN_BPL_W*SCREEN_BPL_H/8

#define IMG_H 72

#define VIEWPORT_W 320
#define VIEWPORT_H SCREEN_BPL_H

#define FONT_W 288
#define FONT_H 82
#define FONT_BPL_SIZE (FONT_W / 8)
#define FONT_BYTE_WIDTH (FONT_BPL_SIZE * SCREEN_BITPLANES)
#define FONT_COLORS 8
#define FONT_LETTER_WIDTH 16
#define FONT_LETTER_HEIGHT 16
#define FONT_LETTERS_PER_LINE 18

#define SCROLL_SPEED 2

#define LINE_TOP (0x4c-6)
#define LINE_BOTTOM (0x4c+IMG_H+1)

#define LINE_START 0x07
#define LINE_END 0xdf

#define LINES 8

#define COPPERLIST_SIZE 512

INCBIN(imageData, ".\\\\assets\\\\image.166x72.bltraw");
INCBIN(imageColors, ".\\\\assets\\\\palette.raw");

INCBIN_CHIP(fontData, ".\\\\assets\\\\font.288x82.bltraw");
INCBIN(fontColors, ".\\\\assets\\\\font-palette.raw");

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

static inline ULONG GetVPos() {
    return (*(volatile ULONG*)0xDFF004) & 0x1ff00;
}

static void WaitVbl() {
	while (GetVPos() == (311<<8)) { }
    while (GetVPos() != (311<<8)) { }
}

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

UWORD scrollCounter = 0;
UWORD *bottomScreen;

const char* letters = "GREETINGS FROM THE OLD AMIGA COMPUTER!   ";
USHORT letterIndex = 0;

UWORD* bottomScreenBplsPtr;

#define JUMP_STRENGHT 5
#define GRAVITY_FRAMES 5
SHORT y = 0,  jump = 0, gravityFrames = 0;

static void Scroll() {

    UWORD * bottomScreenPtr = (UWORD*)((ULONG)bottomScreen + (SCREEN_BPL_SIZE*3 * y));
    UWORD* bplPtr = bottomScreenBplsPtr;
    for(SHORT i = 0; i < SCREEN_BITPLANES; i++) {
        ULONG bpl = ((ULONG)bottomScreenPtr) + i * SCREEN_BPL_SIZE;  
        CPMOVE_L(bplPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    if(y == 0)
        jump = JUMP_STRENGHT;
    y += jump;

    if(++gravityFrames == GRAVITY_FRAMES) {
        jump -= 1; 
        gravityFrames = 0;
    }

    WaitBlit();

    SHORT letterCount = strlen(letters);
    /*  #######
        blitter
        ####### */
    UWORD plotY = 100;
    if(scrollCounter == 0) {    
        UWORD plotX = SCREEN_BPL_W - FONT_LETTER_WIDTH;

        LONG_PTR(custom->bltcon0) = 0x09f00000;
        LONG_PTR(custom->bltafwm) = 0xffffffff;

        USHORT letterNumber = letters[letterIndex] - 0x1e;

        UWORD letterOffset = letterNumber / FONT_LETTERS_PER_LINE * (FONT_LETTER_HEIGHT * FONT_BYTE_WIDTH);
        letterNumber = letterNumber % FONT_LETTERS_PER_LINE;
        letterOffset += letterNumber * FONT_LETTER_WIDTH / 8;  

        custom->bltapt = (APTR)((ULONG)fontData + letterOffset);
        custom->bltdpt = (APTR)((ULONG)bottomScreen + (SCREEN_BPL_SIZE*3) * plotY) + plotX/8;
        
        custom->bltamod = FONT_BPL_SIZE - 2; 
        custom->bltdmod = SCREEN_BPL_SIZE - 2;
        custom->bltsize = (UWORD)(FONT_LETTER_WIDTH*3 * 64 + 1);
        WaitBlit();

        letterIndex++;
        if(letterIndex >= letterCount) letterIndex = 0;
    }

    scrollCounter += SCROLL_SPEED;
    if(scrollCounter == FONT_LETTER_WIDTH)
        scrollCounter = 0;

    UWORD offset = plotY * SCREEN_BYTE_WIDTH;

    UWORD blth = FONT_LETTER_HEIGHT;
    UWORD bltw = SCREEN_BPL_W / FONT_LETTER_WIDTH;

    UWORD bottomRightCorner = blth * SCREEN_BYTE_WIDTH - 2;

    LONG_PTR(custom->bltcon0) = 0x09f00002 | SCROLL_SPEED << 28;
    LONG_PTR(custom->bltafwm) = 0xffffffff;
   
    APTR bitplane_offset = (APTR)((ULONG)bottomScreen + offset + bottomRightCorner);

    custom->bltapt = bitplane_offset;
    custom->bltdpt = bitplane_offset;
    
    custom->bltdmod = 0;
    custom->bltamod = 0; 
    custom->bltsize = (blth*3) * 64 + bltw;
    WaitBlit();
    
    /* ###########
       end blitter
       ########### */
}

static __interrupt void MoveLine() 
{
    custom->intreq = INTF_VERTB;

    if(line == LINE_TOP || line == LINE_BOTTOM)
        line_direction = -line_direction;
    line += line_direction;

    for(SHORT i = 0; i < LINES; i++) {
        lines[i]->vhpos = CPLINE(line+i, LINE_START);
    }
    Scroll();

    Explosion_NextFrame(explosion);
    Explosion_Move(explosion, 1, 0);
    Explosion_Paint(explosion);
}

UWORD imageSizeWithMargin;

static UWORD* CreateLogoScreen(UWORD* copPtr) {
    // logo
    logoScreen.Width = 176;
    logoScreen.Height = 72;
    logoScreen.Bitplanes = 3;
    logoScreen.Palette = (UWORD*)imageColors;
    logoScreen.Display = &display;

    UWORD imageSize = Screen_ByteWidth(&logoScreen) * logoScreen.Height;

    imageSizeWithMargin = imageSize + Screen_ByteWidth(&logoScreen) * 10;
    logoScreen.Data = AllocMem(imageSizeWithMargin, MEMF_CHIP | MEMF_CLEAR);
    CopyMem(imageData, logoScreen.Data, imageSize);    

    return Screen_Create(copPtr, &logoScreen);
}

int main() 
{
    SysBase = *((struct ExecBase**)4UL);
    line_colors[LINES-1] = ((UWORD*)imageColors)[0];
    //((UWORD*)fontColors)[0] = ((UWORD*)imageColors)[0];

    explosion = Explosion_Create(0x80, 0xDC);
    
    nullSprite = AllocMySprite((struct MySprite) {
        .vStart = 0x2a,
        .vStop  = 0x2b,
        .hStart = 0x20,
        .flags  = 0x0
    }, nullSpriteData, sizeof(nullSpriteData));

    APTR copinit = AllocMem(COPPERLIST_SIZE, MEMF_CHIP);
    UWORD* copPtr = copinit;

    // view port
    display.Width = 320;
    display.Height = 256;
    display.TopMargin = 76;
    display.LeftMargin = 110;

    copPtr = Display_Create(copPtr, &display);
    copPtr = CreateLogoScreen(copPtr);

    // sprite
    CPMOVE(copPtr, COLOR17, 0x10C);
    CPMOVE(copPtr, COLOR18, 0x06C);
    CPMOVE(copPtr, COLOR19, 0x0AC);

    explosion->copperListSpritePointer = copPtr;
    CPMOVE_L(copPtr, SPR0PTH, (ULONG)Explosion_CurrentFrame(explosion)->sprite);
    CPMOVE_L(copPtr, SPR1PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR2PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR3PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR4PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR5PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR6PTH, (ULONG)nullSprite);
    CPMOVE_L(copPtr, SPR7PTH, (ULONG)nullSprite);

    // top line
    CPMOVE(copPtr, COLOR00, 0x349);
    CPWAIT(copPtr, CPLINE(0x2b, LINE_START), 0xfffe);

    CPMOVE(copPtr, COLOR00, 0x56b);
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START), 0xfffe);

    CPMOVE(copPtr, COLOR00, ((UWORD*)imageColors)[0]);

    // copper line
    for(SHORT i = 0; i < LINES; i++) {
        lines[i] = (CopperLine*)copPtr;
        CPWAIT(copPtr, CPLINE(0x80, LINE_START), 0xfffe);
        CPMOVE(copPtr, COLOR00, line_colors[i]);
    }

    // second screen
    CPWAIT(copPtr, CPLINE(LINE_BOTTOM+LINES+1, LINE_START), 0xfffe);

    /*textScreen.Palette = fontColors;
    textScreen.Width = 352;
    textScreen.Height = display.Height;*/

    for(SHORT i = 0; i < FONT_COLORS;i++) {
        UWORD color = ((UWORD*)fontColors)[i];
        CPMOVE(copPtr, offsetof(struct Custom, color[i]), color);
    }

    bottomScreen = AllocMem(SCREEN_SIZE * SCREEN_BITPLANES, MEMF_CHIP | MEMF_CLEAR);
    bottomScreenBplsPtr = copPtr;
    for(SHORT i = 0; i < SCREEN_BITPLANES; i++) {
        ULONG bpl = ((ULONG)bottomScreen) + i * SCREEN_BPL_SIZE;  
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    CPMOVE(copPtr, BPL1MOD, SCREEN_BYTE_WIDTH - VIEWPORT_W/8);
    CPMOVE(copPtr, BPL2MOD, SCREEN_BYTE_WIDTH - VIEWPORT_W/8);
    
    CPMOVE(copPtr, DDFSTRT, 0x38);
    CPMOVE(copPtr, DDFSTOP, 0xd0);
    CPMOVE(copPtr, BPLCON0, 0x1000 * SCREEN_BITPLANES + 0x0200);    // three bitplanes

    // bottom line
    CPWAIT(copPtr, CPLINE(0xff, LINE_END), 0xfffe);
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START), 0xfffe);
    CPMOVE(copPtr, COLOR00, 0x56c);

    CPWAIT(copPtr, CPLINE(0x2d, LINE_START), 0xfffe);
    CPMOVE(copPtr, COLOR00, 0x349);
    CPEND(copPtr);

    System_Kill(&systemData);

    WaitVbl();

    // initialize custom values
    SetInterruptHandler((APTR)MoveLine);
    custom->intena = INTF_SETCLR | INTF_VERTB;
    custom->intreq = INTF_VERTB;
    custom->cop1lc = (ULONG)copinit;
	custom->dmacon = DMAF_SETCLR | DMAF_ALL;

    // main loop
	while(ciaa->ciapra & CIAF_GAMEPORT0) // active low
	{
        // interrupt handler moves the line on the screen
	}

    System_Restore(&systemData);
    
    FreeMem(copinit, COPPERLIST_SIZE);
    FreeMem(bottomScreen, SCREEN_SIZE);
    FreeMem(logoScreen.Data, imageSizeWithMargin);
    
    Explosion_Free(explosion);
    FreeMySprite(nullSprite, sizeof(nullSpriteData));
    return 0;
}
