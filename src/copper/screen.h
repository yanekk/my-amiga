#pragma once
#include <exec/types.h>

struct NewScreen {
    SHORT Bitplanes;
    UWORD* Data;
    UWORD* Palette;
    UWORD Width;
    UWORD Height;
    struct Display* Display;

    // do not modify, set internally by Screen_Create
    UWORD* BitplanesCopperPointer;
    UWORD BitplaneSize;
    UWORD RowWidth;
    UWORD Size;
};

UWORD* Screen_Create(UWORD* copPtr, struct NewScreen* newScreen);
UWORD Screen_RowWidth(struct NewScreen* newScreen);
UWORD Screen_BitplaneSize(struct NewScreen* newScreen);
UWORD Screen_Size(struct NewScreen* newScreen);
void Screen_SetY(struct NewScreen* newScreen, SHORT y);