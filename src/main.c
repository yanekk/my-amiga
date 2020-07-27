#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/gfxmacros.h>

#include <proto/exec.h>
#include "screen.h"
#include "drawing.h"
#include "animtools.h"
#include "sprite_examples.h"
#include "intuition_example.h"
#include "draw_example.h"

#define BLACK 0x000
#define RED 0xf00
#define GREEN 0x0f0
#define BLUE 0x00f

#define COLOR_COUNT 32

struct GfxBase *GfxBase = NULL;
extern struct Custom custom;

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
    struct ViewInfo* vi = AllocVec(sizeof(struct ViewInfo) + COLOR_COUNT * sizeof(LONG), MEMF_CLEAR);

    vi->color_count = COLOR_COUNT;
    vi->colortable[0] = BLACK;
    vi->colortable[1] = RED;
    vi->colortable[2] = GREEN;
    vi->colortable[3] = BLUE;

    CreateView(vi);
    RunDrawExample(vi);

    //RunVSpriteExample(vi);

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
