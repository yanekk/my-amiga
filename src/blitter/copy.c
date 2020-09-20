#include <hardware/custom.h>
#include <proto/graphics.h>

#include "copy.h"
#include "../common/utils.h"
#include "../copper/screen.h"
#include "../support/gcc8_c_support.h"

static volatile struct Custom *custom = (struct Custom *)0xdff000;

static inline void Blit(ULONG con0, ULONG afwm, APTR aPtr, APTR dPtr, UWORD aMod, UWORD dMod, UWORD height, UWORD width) 
{
    WaitBlit();
    LONG_PTR(custom->bltcon0) = con0;
    LONG_PTR(custom->bltafwm) = afwm;
    custom->bltapt = aPtr;
    custom->bltdpt = dPtr;
    custom->bltamod = aMod;
    custom->bltdmod = dMod;
    custom->bltsize = height * 64 + width/16;
}

void Blitter_CopyAtoB(struct NewScreen* source, UWORD sourceX, UWORD sourceY, UWORD sourceW, UWORD sourceH, struct NewScreen* destination, UWORD destinationX, UWORD destinationY) 
{
    UWORD sourceOffset = (sourceY * source->RowWidth * source->Bitplanes) + (sourceX / 8);
    UWORD destinationOffset = (destinationY * destination->RowWidth * destination->Bitplanes) + (destinationX / 8);

    Blit(0x09f00000, 0xffffffff,
        (APTR)((ULONG)source->Data + sourceOffset),
        (APTR)((ULONG)destination->Data + destinationOffset),
        source->RowWidth - sourceW/8,
        destination->RowWidth - sourceW/8,
        sourceH * destination->Bitplanes, sourceW);
}

void Blitter_CookieCut(
    struct NewScreen* source, struct NewScreen* background, struct NewScreen* mask,
    UWORD sourceX, UWORD sourceY, UWORD width, UWORD height, 
    struct NewScreen* destination, UWORD destinationX, UWORD destinationY) 
{
    UWORD sourceOffset = (sourceY * source->RowWidth * source->Bitplanes) + (sourceX / 8);
    UWORD destinationOffset = (destinationY * destination->RowWidth * destination->Bitplanes) + (destinationX / 8);

    UWORD sourceModulo = source->RowWidth - width/8;
    UWORD destinationModulo = destination->RowWidth - width/8;

    WaitBlit();
    LONG_PTR(custom->bltcon0) = 0x0fca0000;
    LONG_PTR(custom->bltafwm) = 0xffffffff;
    
    custom->bltapt = (APTR)((ULONG)mask->Data + sourceOffset);
    custom->bltamod = sourceModulo;

    custom->bltbpt = (APTR)((ULONG)source->Data + sourceOffset);
    custom->bltbmod = sourceModulo;

    custom->bltcpt = (APTR)((ULONG)background->Data + destinationOffset);
    custom->bltcmod = destinationModulo;

    custom->bltdpt = (APTR)((ULONG)destination->Data + destinationOffset);  
    custom->bltdmod = destinationModulo;

    custom->bltsize = height * 64 * destination->Bitplanes + width/16;
}

void Blitter_ShiftALeft(struct NewScreen* screen, SHORT x, SHORT y, SHORT width, SHORT height, SHORT scrollSpeed)
{
    ULONG offset = ((y + height) * screen->RowWidth * screen->Bitplanes) + (x / 8) - screen->RowWidth;
    APTR bitplane_offset = (APTR)((ULONG)screen->Data + offset);
    UWORD modulo = screen->RowWidth - width/8;

    Blit(0x09f00002 | scrollSpeed << 28, 0xffffffff,
        bitplane_offset, bitplane_offset,
        modulo, modulo, height * screen->Bitplanes, width);
}