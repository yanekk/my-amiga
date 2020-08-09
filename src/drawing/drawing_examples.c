#include "drawing_examples.h"
#include "drawing.h"
#include "../common/screen.h"
#include <proto/graphics.h>
#include <graphics/gfxmacros.h>

void DrawBoxes(struct ViewInfo * vi) {
    static struct Box boxes[3] = {
        {.x = 25, .y = 25, .width = 75, .height = 75, .color = 1 },
        {.x = 50, .y = 50, .width = 75, .height = 75, .color = 2 },
        {.x = 75, .y = 75, .width = 75, .height = 75, .color = 3 },
    };
    
    for (int box = 0; box <= 2; box++) 
        DrawBox(vi, boxes[box]);
}
void DrawArea(struct  ViewInfo * vi) 
{
    struct RastPort * rp = &vi->rastPort;

    USHORT areaPattern[3][8] =
    {
        {
            0x0000, 0x0000,
            0xffff, 0xffff,
            0x0000, 0x0000,
            0xffff, 0xffff
        },
        {
            0x0000, 0x0000,
            0x0000, 0x0000,
            0xffff, 0xffff,
            0xffff, 0xffff
        },
        {
            0xff00, 0xff00,
            0xff00, 0xff00,
            0xff00, 0xff00,
            0xff00, 0xff00
        }
    };
    
    SetAfPt(rp, (UWORD*)&areaPattern, -3);
    
    SetAPen(rp, -1);
    SetBPen(rp, 0);
    SetDrMd(rp, JAM2);
    SetAPen(rp, 2);
    AreaMove(rp, 0, 0);
    AreaDraw(rp, 30, 0);
    AreaDraw(rp, 40, 40);
    AreaDraw(rp, 0, 40);
    AreaEnd(rp);
}

void DrawPoly(struct ViewInfo * vi) {
    struct RastPort * rp = &vi->rastPort;
    SHORT linearray[] = {
        10, 10,
        60, 15,
        15, 15,
        30, 10,
        10, 10
    };
    SetDrPt(&vi->rastPort, 0xAAAA);
    //Move(&vi->rastPort, 100, 100);
    //Draw(&vi->rastPort, 120, 130);

    PolyDraw(rp, 4, linearray);
}

void FillScreen(struct ViewInfo * vi) 
{
    SetRast(&vi->rastPort, 0);
}

void FloodExample(struct ViewInfo * vi) 
{
    struct RastPort * rp = &vi->rastPort;
    SetBPen(rp, 2);
    SetDrMd(rp, JAM2);
    SetAfPt(rp, NULL, 0);
    Flood(rp, 1, 100, 100);
}