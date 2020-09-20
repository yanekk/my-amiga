#pragma once
#include <exec/types.h>

struct Display {
    UWORD Width;
    UWORD Height;
    USHORT TopMargin;
    USHORT LeftMargin;

    // private
    USHORT HStart;
    USHORT HStop;
};
UWORD* Display_Create(UWORD* copPtr, struct Display* display);