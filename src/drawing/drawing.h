#pragma once
#include <clib/graphics_protos.h>
#include "../common/screen.h"

struct Box {
    WORD x;
    WORD y;
    WORD width;
    WORD height;
    WORD color;
};

VOID DrawBox(struct ViewInfo* viewInfo, struct Box rectangle);
VOID DrawPixel(struct ViewInfo* viewInfo, LONG x, LONG y);