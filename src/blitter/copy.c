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
    for(SHORT bpl = 0; bpl < source->Bitplanes; bpl++) 
    {
        UWORD sourceOffset = (bpl * source->BitplaneSize) 
            + (sourceX / 8) + (sourceY * source->RowWidth);

        UWORD destinationOffset = (bpl * destination->BitplaneSize) 
            + (destinationX / 8) + (destinationY * destination->RowWidth);

        Blit(0x09f00000, 0xffffffff,
            (APTR)((ULONG)source->Data + sourceOffset),
            (APTR)((ULONG)destination->Data + destinationOffset),
            source->RowWidth - sourceW/8,
            destination->RowWidth - sourceW/8,
            sourceH, sourceW);
    }
}

void Blitter_ShiftALeft(struct NewScreen* screen, SHORT x, SHORT y, SHORT width, SHORT height, SHORT scrollSpeed)
{
    for(SHORT bpl = 0; bpl < screen->Bitplanes; bpl++) 
    {
        ULONG offset = (bpl * screen->BitplaneSize) + ((y + height-1) * screen->RowWidth) + (x / 8);
        APTR bitplane_offset = (APTR)((ULONG)screen->Data + offset);
        UWORD modulo = screen->RowWidth - width/8;

        Blit(0x09f00002 | scrollSpeed << 28, 0xffffffff,
            bitplane_offset, bitplane_offset,
            modulo, modulo, height, width);
    }
}