#include <exec/types.h>

#include "../copper/screen.h"

void Blitter_CopyAtoB(struct NewScreen* source, UWORD sourceX, UWORD sourceY, UWORD sourceW, UWORD sourceH, 
    struct NewScreen* destination, UWORD destinationX, UWORD destinationY);
void Blitter_CookieCut(
    struct NewScreen* source, struct NewScreen* background, struct NewScreen* mask,
    UWORD sourceX, UWORD sourceY, UWORD width, UWORD height, 
    struct NewScreen* destination, UWORD destinationX, UWORD destinationY);
void Blitter_ShiftALeft(struct NewScreen* screen, SHORT x, SHORT y, SHORT width, SHORT height, SHORT scrollSpeed);