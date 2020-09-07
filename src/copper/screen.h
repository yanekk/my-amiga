#pragma once
#include <exec/types.h>

struct NewScreen {
    SHORT Bitplanes;
    UWORD* Data;
    UWORD* Palette;
    UWORD Width;
    UWORD Height;
    struct Display* Display;
};

UWORD* Screen_Create(UWORD* copPtr, struct NewScreen* newScreen);
UWORD Screen_BitplaneSize(struct NewScreen* newScreen);
UWORD Screen_ByteWidth(struct NewScreen* newScreen);