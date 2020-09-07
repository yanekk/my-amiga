#include <exec/types.h>
#include <hardware/custom.h>
#include <stddef.h>

#include "screen.h"
#include "display.h"

#include "../common/custom_chip.h"
#include "../common/utils.h"

UWORD Screen_BitplaneSize(struct NewScreen* newScreen) {
    return newScreen->Width / 8;
}

UWORD Screen_ByteWidth(struct NewScreen* newScreen) {
    return Screen_BitplaneSize(newScreen) * newScreen->Bitplanes;
}

UWORD* Screen_Create(UWORD* copPtr, struct NewScreen* newScreen) 
{
    SHORT screenMargin = (newScreen->Display->Width - newScreen->Width) / 2;

    CPMOVE(copPtr, DDFSTRT, 0x38 + screenMargin / 2);
    CPMOVE(copPtr, DDFSTOP, 0xd0 - screenMargin / 2);
    
    CPMOVE(copPtr, BPLCON0, 0x1000 * newScreen->Bitplanes + 0x0200);
    CPMOVE(copPtr, BPLCON1, 0x0000);
    CPMOVE(copPtr, BPLCON2, 0x0000);

    UWORD screenBitplaneSize = Screen_BitplaneSize(newScreen);
    UWORD screenModulo = Screen_ByteWidth(newScreen) - screenBitplaneSize;
    CPMOVE(copPtr, BPL1MOD, screenModulo);
    CPMOVE(copPtr, BPL2MOD, screenModulo);

    for(SHORT i = 0; i < newScreen->Bitplanes; i++) {
        ULONG bpl = ((ULONG)newScreen->Data) + i * screenBitplaneSize;
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    for(SHORT i = 0; i < (1 << newScreen->Bitplanes); i++) {
        UWORD color = ((UWORD*)newScreen->Palette)[i];
        CPMOVE(copPtr, offsetof(struct Custom, color[i]), color);
    }
    return copPtr;
}