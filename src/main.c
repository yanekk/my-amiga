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

#include "common/screen.h"
#include "drawing/example.h"
#include "intuition/example.h"
#include "sprite/example.h"
#include "bob/example.h"
#include "common/utils.h"

struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct ExecBase *SysBase;
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

int main() 
{
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
}