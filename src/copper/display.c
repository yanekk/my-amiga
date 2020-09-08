#include "display.h"
#include "../common/custom_chip.h"
#include "../common/utils.h"
#include "../support/gcc8_c_support.h"

UWORD* Display_Create(UWORD* copPtr, struct Display* display) 
{
    UWORD diwStart = (UWORD)display->LeftMargin | display->TopMargin << 8;
    UWORD diwStop = (((UWORD)display->LeftMargin + display->Width) & 0xFF)
        | (((UWORD)display->TopMargin + display->Height) & 0xFF) << 8;

    CPMOVE(copPtr, DIWSTRT, diwStart);
    CPMOVE(copPtr, DIWSTOP, diwStop);
    return copPtr;
}