#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include "drawing.h"
#include "viewport.h"

VOID DrawFilledBox(UBYTE *displaymem, struct BitMap* bitMap, WORD fillcolor, WORD plane)
{
    WORD boxWidth = (WIDTH/2)/8;
    WORD boxHeight = HEIGHT/2;
    UBYTE value = ((fillcolor & (1 << plane)) != 0) ? 0xff : 0x00;
    WORD width;
    for( ; boxHeight; boxHeight--)
    {
        for(width=0 ; width < boxWidth; width++)
            *displaymem++ = value;

        displaymem += (bitMap->BytesPerRow - boxWidth);
    }
}