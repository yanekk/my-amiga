#include <stdlib.h>
#include <graphics/gfxmacros.h>
#include <clib/exec_protos.h>

#include "common/screen.h"
#include "drawing/example.h"
#include "intuition/example.h"
#include "sprite/example.h"
#include "bob/example.h"

struct GfxBase *GfxBase = NULL;
struct View* oldView;

static void StoreCurrentView() {        
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    oldView = GfxBase->ActiView;
}

static void RevertOldView() {
    LoadView(oldView);
    CloseLibrary((struct Library *)GfxBase);
}

static void HaveFunWithGraphics() 
{
    struct ViewInfo* vi = CreateView();
    //RunDrawExample(vi);
    //RunVSpriteExample(vi);
    RunBobExample(vi);
    FreeView(vi);
}


int main(void) 
{
    //RunIntuitionExample();

    StoreCurrentView();
    HaveFunWithGraphics();
    RevertOldView();

    return EXIT_SUCCESS;
}
