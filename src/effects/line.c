#include "line.h"
#include "../common/utils.h"
#include "../common/custom_chip.h"

#define LINES 8

typedef struct CopperLine {
    WORD vhpos;
    WORD _mask;
    WORD _colorRegister;
    WORD colorValue;
} CopperLine;

static USHORT line, top, bottom;
WORD line_direction = 1;
static CopperLine* lines[LINES];
LONG line_colors[LINES] = { 0x444, 0x777, 0xbbb, 0xfff, 0xbbb, 0x777, 0x444, 0x000 };

UWORD* Line_Initialize(UWORD* copPtr, USHORT topY, USHORT bottomY, UWORD backgroundColor, USHORT* size) {
    *size = LINES;
    line = (topY+bottomY)/2;
    
    line_colors[LINES-1] = backgroundColor;

    top = topY;
    bottom = bottomY;

    for(SHORT i = 0; i < LINES; i++) {
        lines[i] = (CopperLine*)copPtr;
        CPWAIT(copPtr, CPLINE(0x80, LINE_START));
        CPMOVE(copPtr, COLOR00, line_colors[i]);
    }
    return copPtr;
}

void Line_Move() 
{
    if(line == top || line == bottom)
        line_direction = -line_direction;
    line += line_direction;

    for(SHORT i = 0; i < LINES; i++) {
        lines[i]->vhpos = CPLINE(line+i, LINE_START);
    }
}

void Line_SetBackgroundColor(UWORD color) {

}
