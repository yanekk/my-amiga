#include "display.h"
#include "../common/custom_chip.h"
#include "../common/utils.h"
#include "../support/gcc8_c_support.h"

UWORD* Display_Create(UWORD* copPtr, struct Display* display) 
{
    UWORD diwStart = (UWORD)display->LeftMargin | display->TopMargin << 8;
    UWORD diwStop = ((((UWORD)display->LeftMargin + display->Width) & 0xFF)
        | (((UWORD)display->TopMargin + display->Height) & 0xFF) << 8) - 0x2000;
    CPMOVE(copPtr, DIWSTRT, diwStart);
    CPMOVE(copPtr, DIWSTOP, diwStop);
    
    display->HStart = display->LeftMargin / 2 - 8;
    display->HStop = display->HStart + (8*((display->Width / 16) - 1));    
    
    return copPtr;
}
