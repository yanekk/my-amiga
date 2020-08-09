#include <proto/exec.h>
#include <proto/graphics.h>
#include "drawing.h"
#include "../common/screen.h"

VOID DrawBox(struct ViewInfo* viewInfo, struct Box box)
{
    SetAPen(&viewInfo->rastPort, box.color);
    RectFill(&viewInfo->rastPort, box.x, box.y, box.x + box.width, box.y + box.height);
}

VOID DrawPixel(struct ViewInfo* viewInfo, LONG x, LONG y)
{
    WritePixel(&viewInfo->rastPort, x, y);
}