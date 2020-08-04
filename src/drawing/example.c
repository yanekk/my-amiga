#include <clib/dos_protos.h>
#include <graphics/gfxbase.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <graphics/gfxmacros.h>

#include "drawing_examples.h"
#include "example.h"
#include "drawing.h"

void RunDrawExample(struct ViewInfo* vi) {
    struct RastPort* rp = &vi->rastPort;

    ULONG rassize = RASSIZE(WIDTH, HEIGHT);
    UBYTE *tmpbuf = AllocVec (rassize, MEMF_CHIP|MEMF_CLEAR);

    struct TmpRas tmpRas = { 0 };
    
    InitTmpRas(&tmpRas, tmpbuf, rassize);
    rp->TmpRas = &tmpRas;

    struct AreaInfo areaInfo = { 0 };
    WORD* areaBuffer = AllocVec(AREA_SIZE * sizeof(WORD), MEMF_CLEAR);
    InitArea(&areaInfo, areaBuffer, (AREA_SIZE * 2) / 5);
    rp->AreaInfo = &areaInfo;

    LoadView(&vi->view);

    SetAPen(rp, 3);
    DrawPixel(vi, 150, 150);
    DrawArea(vi);
    DrawEllipse(rp, 100, 100, 20, 50);
    
    DrawCircle(rp, 150, 150, 20);

    DrawPoly(vi);
    FloodExample(vi);
    //FillScreen(vi);
    
    Delay(1 * TICKS_PER_SECOND);

    FreeVec(areaBuffer);
    FreeVec(tmpbuf);
}