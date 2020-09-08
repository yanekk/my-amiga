#include <exec/types.h>
#include <hardware/custom.h>
#include <stddef.h>

#include "screen.h"
#include "display.h"

#include "../common/custom_chip.h"
#include "../common/utils.h"

UWORD inline Screen_BitplaneSize(struct NewScreen* newScreen) {
    return newScreen->Width / 8;
}

UWORD inline Screen_ByteWidth(struct NewScreen* newScreen) {
    return Screen_BitplaneSize(newScreen) * newScreen->Bitplanes;
}

UWORD* Screen_Create(UWORD* copPtr, struct NewScreen* newScreen) 
{
    newScreen->BitplaneSize = Screen_BitplaneSize(newScreen);
    newScreen->ByteWidth = Screen_ByteWidth(newScreen);

    SHORT screenMargin = 0;
    UWORD screenModuloDiff;
    if(newScreen->Width < newScreen->Display->Width) {
        screenMargin = (newScreen->Display->Width - newScreen->Width) / 2;
        screenModuloDiff = newScreen->BitplaneSize;
    } else {
        screenModuloDiff = newScreen->Display->Width / 8;
    }   

    newScreen->BitplanesCopperPointer = copPtr;
    for(SHORT i = 0; i < newScreen->Bitplanes; i++) {
        ULONG bpl = ((ULONG)newScreen->Data) + i * newScreen->BitplaneSize;
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    for(SHORT i = 0; i < (1 << newScreen->Bitplanes); i++) {
        UWORD color = ((UWORD*)newScreen->Palette)[i];
        CPMOVE(copPtr, offsetof(struct Custom, color[i]), color);
    }

    CPMOVE(copPtr, BPL1MOD, newScreen->ByteWidth - screenModuloDiff);
    CPMOVE(copPtr, BPL2MOD, newScreen->ByteWidth - screenModuloDiff);

    CPMOVE(copPtr, BPLCON0, 0x1000 * newScreen->Bitplanes + 0x0200);
    CPMOVE(copPtr, BPLCON1, 0x0000);
    CPMOVE(copPtr, BPLCON2, 0x0000);

    CPMOVE(copPtr, DDFSTRT, 0x38 + screenMargin / 2);
    CPMOVE(copPtr, DDFSTOP, 0xd0 - screenMargin / 2);
    
    return copPtr;
}

void Screen_SetY(struct NewScreen* newScreen, SHORT y) 
{
    UWORD* screenPtr = (UWORD*)((ULONG)newScreen->Data + (newScreen->BitplaneSize * newScreen->Bitplanes * y));
    UWORD* copPtr = newScreen->BitplanesCopperPointer;
    for(SHORT i = 0; i < newScreen->Bitplanes; i++) {
        ULONG bpl = ((ULONG)screenPtr) + i * newScreen->BitplaneSize;  
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }
}