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

#define BACKGROUND_COLOR 0x113
#define LINE_TOP 0x40
#define LINE_BOTTOM 0xF0

#define LINE_START 0x07
#define LINE_END 0xdf

#define LINES 8
#define BPL_SIZE 320*256/8
#define COPPERLIST_SIZE 512

struct copinit *oldcopinit;

typedef struct CopperLine {
    WORD vhpos;
    WORD _mask;
    WORD _colorRegister;
    WORD colorValue;
} CopperLine;

CopperLine* lines[LINES];
LONG colors[LINES] = { 0x444, 0x777, 0xbbb, 0xfff, 0xbbb, 0x777, 0x444, BACKGROUND_COLOR };

UWORD SystemInts;
static APTR SystemIrq;

USHORT line = 0xac;
WORD line_direction = 1;

static __interrupt void MoveLine() 
{
    custom->intreq = INTF_VERTB;

    if(line == LINE_TOP || line == LINE_BOTTOM)
        line_direction = -line_direction;
    line += line_direction;

    for(SHORT i = 0; i < LINES; i++) {
        lines[i]->vhpos = CPLINE(line+i, LINE_START);
    }
}

int main() 
{
    SysBase = *((struct ExecBase**)4UL);

    ULONG* bitplane = AllocMem(BPL_SIZE, MEMF_CHIP | MEMF_CLEAR);
    bitplane[0] = 0xc0ffee;
    bitplane[1] = 0xc0ffee;
    bitplane[3] = 0xc0ffee;

    APTR copinit = AllocMem(COPPERLIST_SIZE, MEMF_CHIP);
    UWORD* copPtr = copinit;

    CPMOVE(copPtr, DDFSTRT, 0x38);
    CPMOVE(copPtr, DDFSTOP, 0xd0);
    CPMOVE(copPtr, DIWSTRT, 0x2c81);
    CPMOVE(copPtr, DIWSTOP, 0x2cc1);
    
    CPMOVE(copPtr, BPLCON0, 0x1200);
    CPMOVE(copPtr, BPLCON1, 0x0000);
    CPMOVE(copPtr, BPLCON2, 0x0000);

    CPMOVE(copPtr, BPL1MOD, 0);
    CPMOVE(copPtr, BPL2MOD, 0);
    CPMOVE(copPtr, BPL1PTH, (UWORD)(((ULONG)bitplane) >> 16));
    CPMOVE(copPtr, BPL1PTL, (UWORD)bitplane);
    
    CPMOVE(copPtr, COLOR01, 0xFFF);
    CPMOVE(copPtr, COLOR02, 0xFFF);
    CPMOVE(copPtr, COLOR03, 0xFFF);
    CPMOVE(copPtr, COLOR00, 0x349);
    CPWAIT(copPtr, CPLINE(0x2b, LINE_START), 0xfffe);

    CPMOVE(copPtr, COLOR00, 0x56b);
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START), 0xfffe);

    CPMOVE(copPtr, COLOR00, BACKGROUND_COLOR);

    for(SHORT i = 0; i < LINES; i++) {
        lines[i] = (CopperLine*)copPtr;
        CPWAIT(copPtr, CPLINE(0x80, LINE_START), 0xfffe);
        CPMOVE(copPtr, COLOR00, colors[i]);
    }

    CPWAIT(copPtr, CPLINE(0xff, LINE_END), 0xfffe);
    CPWAIT(copPtr, CPLINE(0x2c, LINE_START), 0xfffe);
    CPMOVE(copPtr, COLOR00, 0x56c);

    CPWAIT(copPtr, CPLINE(0x2d, LINE_START), 0xfffe);
    CPMOVE(copPtr, COLOR00, 0x349);
    CPEND(copPtr);

    GfxBase = (struct GfxBase *)OpenLibrary(GRAPHICSNAME, 0);
    oldcopinit = GfxBase->copinit;

    CloseLibrary((struct Library*)GfxBase);

    SystemInts = custom->intenar;
    SystemIrq = GetInterruptHandler(); //store interrupt register

    SetInterruptHandler((APTR)MoveLine);

    custom->intena = INTF_SETCLR | INTF_VERTB;
    custom->intreq = INTF_VERTB;
    custom->cop1lc = (ULONG)copinit;
	custom->dmacon = DMAF_SETCLR | DMAF_ALL;

	while(ciaa->ciapra & CIAF_GAMEPORT0) // active low
	{

	}
    
    custom->cop1lc = oldcopinit;
    FreeMem(bitplane, BPL_SIZE);
    FreeMem(copinit, COPPERLIST_SIZE);

    custom->intreq = 0x7fff;
    custom->intena = SystemInts | 0xc000;

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