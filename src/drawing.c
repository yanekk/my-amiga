#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include "drawing.h"
#include "viewport.h"

VOID DrawFilledBox(SHORT offset, struct BitMap* bitMap, WORD fillcolor)
{
    for (int depth = 0; depth < DEPTH; depth++)
    {
        UBYTE * displaymem = bitMap->Planes[depth] + offset;

        WORD boxWidth = (WIDTH/2)/8;
        WORD boxHeight = HEIGHT/2;

        UBYTE value = ((fillcolor & (1 << depth)) != 0) ? 0xFF : 0x00;

        for( ; boxHeight; boxHeight--)
        {
            for(WORD width = 0; width < boxWidth; width++)
                *displaymem++ = value;

            displaymem += (bitMap->BytesPerRow - boxWidth);
        }
    }
}