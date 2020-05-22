#pragma once
#include <clib/graphics_protos.h>
#include "screen.h"

struct Box {
    WORD x;
    WORD y;
    WORD width;
    WORD height;
    WORD color;
};

VOID DrawBox(struct ViewInfo* viewInfo, struct Box rectangle);