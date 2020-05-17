#include <stdio.h>
#include <clib/graphics_protos.h>
#include <graphics/videocontrol.h>

#include "viewport.h"

#define BLACK 0x000
#define RED 0xf00
#define GREEN 0x0f0
#define BLUE 0x00f

void DrawFilledBox(UBYTE *displaymem, struct BitMap* bitMap, WORD fillcolor, WORD plane);

void CreateView(struct ViewInfo* viewInfo) {
    InitView(&viewInfo->view);  

    viewInfo->viewExtra = GfxNew(VIEW_EXTRA_TYPE);
    GfxAssociate(&viewInfo->view, viewInfo->viewExtra);
    viewInfo->view.Modes |= EXTEND_VSTRUCT;

    ULONG modeId = DEFAULT_MONITOR_ID | HIRES;
    viewInfo->viewExtra->Monitor = OpenMonitor(NULL, modeId);

    InitBitMap(&viewInfo->bitMap, DEPTH, WIDTH, HEIGHT);
    for(int i = 0; i < DEPTH; i++) {
        viewInfo->bitMap.Planes[i] = AllocRaster(WIDTH, HEIGHT);
    }

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
    static UWORD colortable[] = { BLACK, RED, GREEN, BLUE };
    
    viewInfo->colorMap = GetColorMap(4);
    VideoControl(viewInfo->colorMap, vcTags);
    LoadRGB4(&viewInfo->viewPort, colortable, 4);

    MakeVPort(&viewInfo->view, &viewInfo->viewPort);
    MrgCop(&viewInfo->view);

    UBYTE *displaymem = NULL;
    for(int depth = 0; depth < DEPTH; depth++)
    {
        displaymem = (UBYTE *)viewInfo->bitMap.Planes[depth];
        BltClear(displaymem, (viewInfo->bitMap.BytesPerRow * viewInfo->bitMap.Rows), 1L);
    }

    LoadView(&viewInfo->view);
}

void FreeView(struct ViewInfo* viewInfo) 
{
    WaitTOF();
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

    if(viewInfo->viewExtra->Monitor) 
        CloseMonitor(viewInfo->viewExtra->Monitor);
    if(viewInfo->viewExtra) 
        GfxFree(viewInfo->viewExtra); 
}