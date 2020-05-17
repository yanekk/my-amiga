#pragma once
#include <clib/graphics_protos.h>

struct Box {
    WORD offset;
    WORD width;
    WORD height;
    WORD color;
};

VOID DrawBox(struct Box rectangle, struct BitMap* bitMap);