#include <exec/interrupts.h>
#include <hardware/cia.h>
#include <hardware/intbits.h>
#include <hardware/dmabits.h>
#include <proto/graphics.h>
#include <proto/dos.h>

#include "blitter/copy.h"
#include "common/structs.h"
#include "common/utils.h"
#include "common/custom_chip.h"
#include "copper/screen.h"
#include "copper/display.h"
#include "copper/initialization.h"
#include "effects/bounce.h"
#include "effects/line.h"
#include "effects/text.h"
#include "music/P6110.h"
#include "sprite/common.h"
#include "sprite/explosion.h"
#include "support/gcc8_c_support.h"
#include "system/interrupts.h"
#include "system/system.h"
#include "system/screen.h"

#define IMG_H 72

#define LINE_TOP (0x4c-6)
#define LINE_BOTTOM (0x4c+IMG_H+1)

#define COPPERLIST_SIZE 512
#define MUSIC 0

INCBIN_CHIP(music, ".\\\\assets\\\\P61.mod.my-1st-module.1")

const UWORD __chip nullSpriteData[] = {
    0, 0,
    0, 0
};

struct Explosion *explosion_1;
struct Explosion *explosion_2;
struct MySprite *nullSprite;

struct SystemData systemData;
struct Display display;
struct NewScreen logoScreen, textScreen, fontScreen;
struct Bounce bounce;

UWORD *bottomScreenData;

static void OnVBlank() {
    custom->intreq = INTF_VERTB;

    Line_Move();
    
    Screen_SetY(&textScreen, bounce.y);
    Bounce_Update(&bounce);
    TextPlotting_Scroll();

    Explosion_NextFrame(explosion_1);
    Explosion_Move(explosion_1, 1, 0);
    Explosion_Paint(explosion_1);

    Explosion_NextFrame(explosion_2);
    Explosion_Move(explosion_2, 1, 0);
    Explosion_Paint(explosion_2);

    if(MUSIC) ThePlayer61_Play();
}

USHORT lines;

int main() 
{
    SysBase = *((struct ExecBase**)4UL);
    
    if(MUSIC) ThePlayer61_Initialize(music);

    APTR copinit = AllocMem(COPPERLIST_SIZE, MEMF_CHIP);
    UWORD* copPtr = copinit;

    Initialize_FontScreen(&fontScreen);

    copPtr = Initialize_Display(copPtr, &display);
    copPtr = Initialize_LogoScreen(copPtr, &logoScreen, &display);

    // sprite
    CPMOVE(copPtr, COLOR17, 0x10C);
    CPMOVE(copPtr, COLOR18, 0x06C);
    CPMOVE(copPtr, COLOR19, 0x0AC);

    // initialize sprites
    nullSprite = AllocMySprite((struct MySprite) {
        .vStart = 0x2a,
        .vStop  = 0x2b,
        .hStart = 0x20,
        .flags  = 0x0
    }, nullSpriteData, sizeof(nullSpriteData));

    explosion_1 = Explosion_Create(128, 230, SPR0PTH);
    explosion_1->copperListSpritePointer = copPtr;
    CPMOVE_L(copPtr, SPR0PTH, (ULONG)Explosion_CurrentFrame(explosion_1)->sprite);

    explosion_2 = Explosion_Create(128, 200, SPR1PTH);
    explosion_2->copperListSpritePointer = copPtr;
    CPMOVE_L(copPtr, SPR1PTH, (ULONG)Explosion_CurrentFrame(explosion_2)->sprite);
    
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

    CPMOVE(copPtr, COLOR00, logoScreen.Palette[0]);

    // copper line
    copPtr = Line_Initialize(copPtr, LINE_TOP, LINE_BOTTOM, logoScreen.Palette[0], &lines);

    // second screen
    CPWAIT(copPtr, CPLINE(LINE_BOTTOM+lines, LINE_END));

    copPtr = Initialize_TextScreen(copPtr, &textScreen, &display);
    
    TextPlotting_Initialize(&fontScreen, &textScreen, "GREETINGS FROM THE OLD AMIGA COMPUTER!   ");
    
    // bottom line
    CPWAIT(copPtr, CPLINE(0xff, LINE_END));
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START));
    CPMOVE(copPtr, COLOR00, 0x56c);

    CPWAIT(copPtr, CPLINE(0x2d, LINE_START));
    CPMOVE(copPtr, COLOR00, 0x349);
    CPEND(copPtr);

    System_Kill(&systemData);

    WaitVbl();

    SetIntVector(INTB_VERTB, &((struct Interrupt) {
        .is_Code = OnVBlank
    }));

    custom->intena = INTF_SETCLR | INTF_VERTB;
    custom->intreq = INTF_VERTB;
    custom->cop1lc = (ULONG)copinit;
    custom->dmacon = DMAF_SETCLR | DMAF_ALL;

    // main loop
	while(1)
	{
  
	}

    System_Restore(&systemData);
    FreeMem(copinit, COPPERLIST_SIZE);
    FreeMem(textScreen.Data, textScreen.Size);
    FreeMem(logoScreen.Data, logoScreen.Size);
    return 0;
}