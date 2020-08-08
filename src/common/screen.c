#include <stdio.h>
#include <clib/graphics_protos.h>
#include <graphics/videocontrol.h>
#include <clib/exec_protos.h>

#include "screen.h"
#include "../vendor/animtools.h"

#define COLOR_COUNT 32

struct ViewInfo * CreateView() {
    struct ViewInfo* viewInfo = AllocVec(sizeof(struct ViewInfo) + COLOR_COUNT * sizeof(LONG), MEMF_CLEAR);

    viewInfo->color_count = COLOR_COUNT;
    viewInfo->colortable[0] = 0x000;
    viewInfo->colortable[1] = 0x10C;
    viewInfo->colortable[2] = 0x06C;
    viewInfo->colortable[3] = 0x0AC;
    viewInfo->colortable[4] = 0x0FF;
    viewInfo->colortable[5] = 0x0FF;
    viewInfo->colortable[6] = 0x0FF;

    InitView(&viewInfo->view);  

    viewInfo->viewExtra = GfxNew(VIEW_EXTRA_TYPE);
    GfxAssociate(&viewInfo->view, viewInfo->viewExtra);
    viewInfo->view.Modes |= EXTEND_VSTRUCT;

    ULONG modeId = DEFAULT_MONITOR_ID | LORES_KEY;
    viewInfo->viewExtra->Monitor = OpenMonitor(NULL, modeId);

    InitBitMap(&viewInfo->bitMap, DEPTH, WIDTH, HEIGHT);
    for(int i = 0; i < DEPTH; i++) {
        viewInfo->bitMap.Planes[i] = AllocRaster(WIDTH, HEIGHT);
    }

    InitRastPort(&viewInfo->rastPort);
    viewInfo->rastPort.BitMap = &viewInfo->bitMap;

    viewInfo->viewPortExtra = GfxNew(VIEWPORT_EXTRA_TYPE);

    struct DimensionInfo dimensionInfo = { 0 };    
    GetDisplayInfoData(NULL, &dimensionInfo, sizeof(dimensionInfo), DTAG_DIMS, modeId);
    viewInfo->viewPortExtra->DisplayClip = dimensionInfo.Nominal;

    struct RasInfo rasInfo = {
        .BitMap = &viewInfo->bitMap
    };
    
    InitVPort(&viewInfo->viewPort);
    viewInfo->view.ViewPort = &viewInfo->viewPort;
    viewInfo->viewPort.RasInfo = &rasInfo;
    viewInfo->viewPort.DWidth = WIDTH;
    viewInfo->viewPort.DHeight = HEIGHT;
    viewInfo->viewPort.Modes = HIRES | LACE;

    struct TagItem vcTags[] = 
    { 
        { VTAG_ATTACH_CM_SET, (ULONG)&viewInfo->viewPort },
        { VTAG_VIEWPORTEXTRA_SET, (ULONG)viewInfo->viewPortExtra }, 
        { VTAG_NORMAL_DISP_SET, (ULONG)FindDisplayInfo(modeId) },
        { VTAG_END_CM, 0 } 
    };    
    
    viewInfo->colorMap = GetColorMap(viewInfo->color_count);
    VideoControl(viewInfo->colorMap, vcTags);
    LoadRGB4(&viewInfo->viewPort, viewInfo->colortable, viewInfo->color_count);

    MakeVPort(&viewInfo->view, &viewInfo->viewPort);
    MrgCop(&viewInfo->view);

    for(int depth = 0; depth < DEPTH; depth++)
    {
        UBYTE * displaymem = (UBYTE *)viewInfo->bitMap.Planes[depth];
        BltClear(displaymem, (viewInfo->bitMap.BytesPerRow * viewInfo->bitMap.Rows), 1L);
    }
    setupGelSys(&viewInfo->rastPort, (BYTE)0b11111111);
    return viewInfo;
}

void FreeView(struct ViewInfo* viewInfo) 
{
    WaitTOF();
    
    cleanupGelSys(viewInfo->rastPort.GelsInfo, &viewInfo->rastPort);

    FreeCprList(viewInfo->view.LOFCprList); 
    if(viewInfo->view.SHFCprList)
        FreeCprList(viewInfo->view.SHFCprList);
    FreeVPortCopLists(&viewInfo->viewPort);

    if(viewInfo->colorMap) 
        FreeColorMap(viewInfo->colorMap);

    if(viewInfo->viewPortExtra) 
        GfxFree(viewInfo->viewPortExtra);

    for(int depth = 0; depth < DEPTH; depth++)
    {
        if (viewInfo->bitMap.Planes[depth])
            FreeRaster(viewInfo->bitMap.Planes[depth], WIDTH, HEIGHT);
    }

    if(viewInfo->viewExtra && viewInfo->viewExtra->Monitor) 
        CloseMonitor(viewInfo->viewExtra->Monitor);
    if(viewInfo->viewExtra) 
        GfxFree(viewInfo->viewExtra); 
    FreeVec(viewInfo);
}

void RefreshScreen(struct ViewInfo* viewInfo) {
    SortGList(&viewInfo->rastPort);
    DrawGList(&viewInfo->rastPort, &viewInfo->viewPort);
    MrgCop(&viewInfo->view);
    WaitTOF();
}