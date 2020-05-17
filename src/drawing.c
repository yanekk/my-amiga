#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include "drawing.h"
#include "screen.h"

VOID DrawBox(struct Box rectangle, struct BitMap* bitMap)
{
    for (int depth = 0; depth < DEPTH; depth++)
    {
        UBYTE * displaymem = bitMap->Planes[depth] + rectangle.offset;

        WORD boxWidth = (rectangle.width)/4;
        WORD boxHeight = rectangle.height;

        UBYTE value = ((rectangle.color & (1 << depth)) != 0) ? 0xFF : 0x00;

        for( ; boxHeight; boxHeight--)
        {
            for(WORD width = 0; width < boxWidth; width++)
                *displaymem++ = value;

            displaymem += (bitMap->BytesPerRow - boxWidth);
        }
    }
}