#include "support/gcc8_c_support.h"

#include <hardware/cia.h>
#include <hardware/custom.h>

#include "common/structs.h"
#include "common/utils.h"

#include "support/gcc8_c_support.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/dos.h>

#include <graphics/gfxmacros.h>

#include <intuition/intuitionbase.h>
#include <hardware/adkbits.h>
#include <hardware/intbits.h>

#include <exec/execbase.h>
#include <graphics/gfxbase.h>

#include "common/screen.h"
#include "drawing/example.h"
#include "intuition/example.h"
#include "sprite/example.h"
#include "bob/example.h"
#include "common/custom_chip.h"
#include "system/interrupts.h"

struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
extern struct DosLibrary *DOSBase;

struct View* oldView;

static void StoreCurrentView() {        
    oldView = GfxBase->ActiView;
}

static void RevertOldView() {
    LoadView(oldView);
} 

static void HaveFunWithGraphics() 
{
    struct ViewInfo* vi = CreateView();
    //RunDrawExample(vi);

    RunVSpriteExample(vi);
    //RunBobExample(vi);
    FreeView(vi);
}

#define SCREEN_BPL_W 320
#define SCREEN_BPL_H 256
#define SCREEN_BITPLANES 3
#define SCREEN_BPL_SIZE (SCREEN_BPL_W/8)
#define SCREEN_BYTE_WIDTH (SCREEN_BPL_SIZE * SCREEN_BITPLANES)
#define SCREEN_SIZE SCREEN_BPL_W*SCREEN_BPL_H/8

#define IMG_W 176
#define IMG_H 72
#define IMG_BITPLANES 3
#define IMG_MARGIN ((SCREEN_BPL_W - IMG_W)/2)
#define IMG_BPL_SIZE (IMG_W / 8)
#define IMG_BYTE_WIDTH (IMG_BPL_SIZE * IMG_BITPLANES)
#define IMG_SIZE IMG_BYTE_WIDTH * (IMG_H)
#define IMG_SIZE_WITH_MARGIN (IMG_SIZE + IMG_BYTE_WIDTH * 10)

#define FONT_W 288
#define FONT_H 82
#define FONT_BPL_SIZE (FONT_W / 8)
#define FONT_COLORS 8

#define LINE_TOP (0x4c-6)
#define LINE_BOTTOM (0x4c+IMG_H+1)

#define LINE_START 0x07
#define LINE_END 0xdf

#define LINES 8

#define COPPERLIST_SIZE 512

INCBIN(imageData, ".\\\\assets\\\\image.166x72.bltraw")
INCBIN(imageColors, ".\\\\assets\\\\palette.raw");

INCBIN_CHIP(fontData, ".\\\\assets\\\\font.288x82.bltraw")
INCBIN(fontColors, ".\\\\assets\\\\font-palette.raw");

struct copinit *oldcopinit;

typedef struct CopperLine {
    WORD vhpos;
    WORD _mask;
    WORD _colorRegister;
    WORD colorValue;
} CopperLine;

struct Sprite {
    UBYTE vStart;
    UBYTE hStart;
    UBYTE vStop;
    UBYTE flags;
    WORD data[];
};

CopperLine* lines[LINES];
LONG line_colors[LINES] = { 0x444, 0x777, 0xbbb, 0xfff, 0xbbb, 0x777, 0x444, 0x000 };

UWORD SystemInts;
UWORD SystemDMA;

static APTR SystemIrq;

USHORT line = (LINE_TOP + LINE_BOTTOM)/2;
WORD line_direction = 1;

static inline ULONG GetVPos() {
    return (*(volatile ULONG*)0xDFF004) & 0x1ff00;
}

static void WaitVbl() {
	while (GetVPos() == (311<<8)) { }
    while (GetVPos() != (311<<8)) { }
}

const UWORD __chip mySpriteData[] = {
    0b0000011111100000, 0b0000000000000000,
    0b0001100000011000, 0b0000011111100000,
    0b0010001111000100, 0b0001111111111000,
    0b0100111001110010, 0b0011111001111100,
    0b0101100000011010, 0b0011100000011100,
    0b1001000000001001, 0b0111000000001110,  
    0b1010000000000101, 0b0110000000000110,  
    0b1010000000000101, 0b0110000000000110,  
    0b1010000000000101, 0b0110000000000110,  
    0b1010000000000101, 0b0110000000000110,    
    0b1001000000001001, 0b0111000000001110, 
    0b0101100000011010, 0b0011100000011100,
    0b0100111001110010, 0b0011111001111100,
    0b0010001111000100, 0b0001111111111000,
    0b0001100000011000, 0b0000011111100000,
    0b0000011111100000, 0b0000000000000000,
    0, 0
};

const UWORD __chip nullSpriteData[] = {
    0, 0,
    0, 0
};

struct Sprite *sprite1;
struct Sprite *nullSprite;

UWORD *bottomScreen;
static void Scroll() {
    /*  #######
        blitter
        ####### */
    
    WORD scrollY = 100;

    WaitBlit();

    LONG_PTR(custom->bltcon0) = 0x09f00000;
    LONG_PTR(custom->bltafwm) = 0xffffffff;
   
    custom->bltapt = (APTR)fontData;
    custom->bltdpt = (APTR)((ULONG)bottomScreen + (SCREEN_BPL_SIZE*3) * scrollY);
    
    custom->bltamod = FONT_BPL_SIZE - 2; 
    custom->bltdmod = SCREEN_BPL_SIZE - 2;
    custom->bltsize = (UWORD)(16*3 * 64 + 1);

    WaitBlit();
    
    UWORD bltx = 48;
    UWORD blty = 30;
    UWORD offset = blty * (SCREEN_BPL_SIZE*3) + (bltx/8);

    UWORD blth = 50;
    UWORD bltw = 224 / 16;
    UWORD bltskip = (SCREEN_BPL_W - 224)/8;

    UWORD corner = (blth-1) * (SCREEN_BPL_SIZE*3) + (bltw*2) - 2;

    LONG_PTR(custom->bltcon0) = 0x19f00002;
    LONG_PTR(custom->bltafwm) = 0xffffffff;
   
    APTR bitplane_offset = (APTR)((ULONG)bottomScreen + offset + corner);

    custom->bltapt = bitplane_offset;
    custom->bltdpt = bitplane_offset;
    
    custom->bltdmod = bltskip;
    custom->bltamod = bltskip; 
    custom->bltsize = (UWORD)((blth*3) * 64 + bltw);
    
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
    sprite1->hStart++;
    Scroll();
    //sprite1->vStart++;
    //sprite1->vStop++;
}

static struct Sprite *AllocMySprite(struct Sprite newSprite, const UWORD *spriteData, SHORT spriteDataSize) {
    struct Sprite* spritePtr = (struct Sprite*) AllocMem(sizeof(struct Sprite) + spriteDataSize, MEMF_CHIP);
    spritePtr->vStart = newSprite.vStart;
    spritePtr->hStart = newSprite.hStart;
    spritePtr->vStop = newSprite.vStop;
    spritePtr->flags = newSprite.flags;
    CopyMem((UWORD*)spriteData, &spritePtr->data, spriteDataSize);
    return spritePtr;
}

static void FreeMySprite(struct Sprite *sprite, SHORT spriteDataSize) {
    FreeMem(sprite, sizeof(struct Sprite) + spriteDataSize);
}


int main() 
{
    SysBase = *((struct ExecBase**)4UL);
    line_colors[LINES-1] = ((UWORD*)imageColors)[0];
    //((UWORD*)fontColors)[0] = ((UWORD*)imageColors)[0];

    UWORD* image = AllocMem(IMG_SIZE_WITH_MARGIN, MEMF_CHIP | MEMF_CLEAR);
    CopyMem(imageData, image, IMG_SIZE);

    sprite1 = AllocMySprite((struct Sprite) {
        .vStart = 0xac,
        .hStart = 0x40,
        .vStop = 0xbc,
        .flags = 0x0
    }, mySpriteData, sizeof(mySpriteData));

    nullSprite = AllocMySprite((struct Sprite) {
        .vStart = 0x2a,
        .hStart = 0x20,
        .vStop = 0x2b,
        .flags = 0x0
    }, nullSpriteData, sizeof(nullSpriteData));

    APTR copinit = AllocMem(COPPERLIST_SIZE, MEMF_CHIP);
    UWORD* copPtr = copinit;

    // logo

    CPMOVE(copPtr, DDFSTRT, 0x38 + IMG_MARGIN/2);
    CPMOVE(copPtr, DDFSTOP, 0xd0 - IMG_MARGIN/2);

    CPMOVE(copPtr, DIWSTRT, 0x4c81);
    CPMOVE(copPtr, DIWSTOP, 0x2cc1);
    
    CPMOVE(copPtr, BPLCON0, 0x3200);    // three bitplanes
    CPMOVE(copPtr, BPLCON1, 0x0000);
    CPMOVE(copPtr, BPLCON2, 0x0000);

    CPMOVE(copPtr, BPL1MOD, IMG_BYTE_WIDTH - IMG_BPL_SIZE);
    CPMOVE(copPtr, BPL2MOD, IMG_BYTE_WIDTH - IMG_BPL_SIZE);

    for(SHORT i = 0; i < IMG_BITPLANES; i++) {
        ULONG bpl = ((ULONG)image) + i * IMG_BPL_SIZE;  
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    for(SHORT i = 0; i < 16;i++) {
        UWORD color = ((UWORD*)imageColors)[i];
        CPMOVE(copPtr, offsetof(struct Custom, color[i]), color);
    }

    // sprite
    CPMOVE(copPtr, COLOR17, 0x10C);
    CPMOVE(copPtr, COLOR18, 0x06C);
    CPMOVE(copPtr, COLOR19, 0x0AC);

    CPMOVE_L(copPtr, SPR1PTH, (ULONG)sprite1);
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

    bottomScreen = AllocMem(SCREEN_SIZE * SCREEN_BITPLANES, MEMF_CHIP | MEMF_CLEAR);

    UWORD c = 0;
    for(ULONG i = 0; i < SCREEN_SIZE/2; i++) {
        bottomScreen[i] = c;
        c++;
    }

    // second screen
    CPWAIT(copPtr, CPLINE(LINE_BOTTOM+LINES+1, LINE_START), 0xfffe);

    for(SHORT i = 0; i < FONT_COLORS;i++) {
        UWORD color = ((UWORD*)fontColors)[i];
        CPMOVE(copPtr, offsetof(struct Custom, color[i]), color);
    }

    for(SHORT i = 0; i < SCREEN_BITPLANES; i++) {
        ULONG bpl = ((ULONG)bottomScreen) + i * SCREEN_BPL_SIZE;  
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    CPMOVE(copPtr, BPL1MOD, SCREEN_BYTE_WIDTH - SCREEN_BPL_SIZE);
    CPMOVE(copPtr, BPL2MOD, SCREEN_BYTE_WIDTH - SCREEN_BPL_SIZE);
    
    CPMOVE(copPtr, DDFSTRT, 0x38);
    CPMOVE(copPtr, DDFSTOP, 0xd0);
    CPMOVE(copPtr, BPLCON0, 0x3200);    // three bitplanes

    // bottom line
    CPWAIT(copPtr, CPLINE(0xff, LINE_END), 0xfffe);
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START), 0xfffe);
    CPMOVE(copPtr, COLOR00, 0x56c);

    CPWAIT(copPtr, CPLINE(0x2d, LINE_START), 0xfffe);
    CPMOVE(copPtr, COLOR00, 0x349);
    CPEND(copPtr);

    // kill system
    GfxBase = (struct GfxBase *)OpenLibrary(GRAPHICSNAME, 0);
    oldcopinit = GfxBase->copinit;

    CloseLibrary((struct Library*)GfxBase);
    
    SystemInts = custom->intenar;
    SystemDMA = custom->dmaconr;
    SystemIrq = GetInterruptHandler(); //store interrupt register

    // initialize custom values
    WaitVbl();
    SetInterruptHandler((APTR)MoveLine);

    custom->intena = INTF_SETCLR | INTF_VERTB;
    custom->intreq = INTF_VERTB;
    custom->cop1lc = (ULONG)copinit;
	custom->dmacon = DMAF_SETCLR | DMAF_ALL;

    Scroll(bottomScreen);

	while(ciaa->ciapra & CIAF_GAMEPORT0) // active low
	{
        // interrupt handler moves the line on the screen
	}

    // restore system
    custom->cop1lc = oldcopinit;
    FreeMem(copinit, COPPERLIST_SIZE);
    FreeMem(bottomScreen, SCREEN_SIZE);
    FreeMem(image, IMG_SIZE_WITH_MARGIN);
    
    FreeMySprite(sprite1, sizeof(mySpriteData));
    FreeMySprite(nullSprite, sizeof(nullSpriteData));

    custom->intreq = 0x7fff;
    custom->intena = SystemInts | 0xc000;
    custom->dmacon = SystemDMA | 0x8000;
    SetInterruptHandler(SystemIrq);

    return 0;
    /*
    SysBase = *((struct ExecBase**)4UL);
    
    UWORD pens[] = { ~0 };
    struct Screen *my_screen;

    //RunIntuitionExample();
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 0);

    my_screen = OpenScreenTags(NULL,
                                SA_Pens, (ULONG)pens,
                                SA_Depth, 2,
                                TAG_DONE);


    HaveFunWithGraphics();
    CloseScreen(my_screen);

    //RevertOldView();
    CloseLibrary((struct Library *)IntuitionBase);
    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)DOSBase);

    return 0;
    */
}
