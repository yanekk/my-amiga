#include <hardware/custom.h>
#include <proto/graphics.h>

#include "copy.h"
#include "../common/utils.h"
#include "../copper/screen.h"
#include "../support/gcc8_c_support.h"

void Blitter_CopyAtoB(struct NewScreen* source, UWORD sourceX, UWORD sourceY, UWORD sourceW, UWORD sourceH, struct NewScreen* destination, UWORD destinationX, UWORD destinationY) 
{
    volatile struct Custom *custom = (struct Custom *)0xdff000;
    LONG_PTR(custom->bltcon0) = 0x09f00000;
    LONG_PTR(custom->bltafwm) = 0xffffffff;

    for(SHORT bpl = 0; bpl < source->Bitplanes; bpl++) 
    {
        WaitBlit();
        UWORD sourceOffset = (bpl * source->BitplaneSize) 
            + (sourceX / 8) + (sourceY * source->RowWidth);

        custom->bltapt = (APTR)((ULONG)source->Data + sourceOffset);

        UWORD destinationOffset = (bpl * destination->BitplaneSize) 
            + (destinationX / 8) + (destinationY * destination->RowWidth);

        custom->bltdpt = (APTR)((ULONG)destination->Data + destinationOffset);

        custom->bltamod = source->RowWidth - sourceW/8; 
        custom->bltdmod = destination->RowWidth - sourceW/8;
        custom->bltsize = (UWORD)(sourceH * 64 + 1);  
    }
}

void Blitter_ShiftALeft(struct NewScreen* screen, SHORT x, SHORT y, SHORT width, SHORT height, SHORT scrollSpeed)
{
    volatile struct Custom *custom = (struct Custom *)0xdff000;
    LONG_PTR(custom->bltcon0) = 0x09f00002 | scrollSpeed << 28;
    LONG_PTR(custom->bltafwm) = 0xffffffff;

    for(SHORT bpl = 0; bpl < screen->Bitplanes; bpl++) 
    {
        WaitBlit();
        ULONG bltw = width / 16;
        ULONG offset = (bpl * screen->BitplaneSize) + ((y + height) * screen->RowWidth) + (x / 8);
        
        APTR bitplane_offset = (APTR)((ULONG)screen->Data + offset);

        custom->bltapt = bitplane_offset;
        custom->bltdpt = bitplane_offset;
        
        custom->bltdmod = screen->RowWidth - width/8;
        custom->bltamod = screen->RowWidth - width/8; 

        custom->bltsize = (height+1) * 64 + bltw;
    }
}