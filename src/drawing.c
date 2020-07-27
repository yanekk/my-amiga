#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include "drawing.h"
#include "screen.h"

VOID DrawBox(struct ViewInfo* viewInfo, struct Box box)
{
    SetAPen(&viewInfo->rastPort, box.color);
    RectFill(&viewInfo->rastPort, box.x, box.y, box.x + box.width, box.y + box.height);
}

VOID DrawPixel(struct ViewInfo* viewInfo, LONG x, LONG y)
{
    WritePixel(&viewInfo->rastPort, x, y);
}