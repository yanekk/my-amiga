#include <exec/types.h>
#include <proto/exec.h>
#include <hardware/custom.h>
#include <stddef.h>

#include "screen.h"
#include "display.h"

#include "../common/custom_chip.h"
#include "../common/utils.h"
#include "../support/gcc8_c_support.h"

UWORD inline Screen_RowWidth(struct NewScreen* newScreen) {
    return newScreen->Width / 8;
}

UWORD inline Screen_BitplaneSize(struct NewScreen* newScreen) {
    return Screen_RowWidth(newScreen) * newScreen->Height;
}

UWORD inline Screen_Size(struct NewScreen* newScreen) {
    return Screen_BitplaneSize(newScreen) * newScreen->Bitplanes;
}

UWORD* Screen_Create(UWORD* copPtr, struct NewScreen* newScreen) 
{
    newScreen->RowWidth = Screen_RowWidth(newScreen);
    newScreen->BitplaneSize = Screen_BitplaneSize(newScreen);
    newScreen->Size = Screen_Size(newScreen);

    SHORT screenMargin = 0;
    UWORD screenModulo = newScreen->RowWidth * newScreen->Bitplanes;
    if(newScreen->Width < newScreen->Display->Width) {
        screenMargin = (newScreen->Display->Width - newScreen->Width) / 2;
        screenModulo -= newScreen->RowWidth;
    } else {
        screenModulo -= newScreen->Display->Width / 8;
    }

    newScreen->BitplanesCopperPointer = copPtr;
    for(SHORT i = 0; i < newScreen->Bitplanes; i++) {
        ULONG bpl = ((ULONG)newScreen->Data) + i * newScreen->RowWidth;
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }

    for(SHORT i = 0; i < (1 << newScreen->Bitplanes); i++) {
        UWORD color = ((UWORD*)newScreen->Palette)[i];
        CPMOVE(copPtr, offsetof(struct Custom, color[i]), color);
    }

    CPMOVE(copPtr, BPL1MOD, screenModulo);
    CPMOVE(copPtr, BPL2MOD, screenModulo);

    CPMOVE(copPtr, BPLCON0, 0x1000 * newScreen->Bitplanes + 0x0200);
    CPMOVE(copPtr, BPLCON1, 0x0000);
    CPMOVE(copPtr, BPLCON2, 0x0000);

    CPMOVE(copPtr, DDFSTRT, newScreen->Display->HStart + screenMargin / 2);
    CPMOVE(copPtr, DDFSTOP, newScreen->Display->HStop - screenMargin / 2);
    
    return copPtr;
}

void Screen_SetY(struct NewScreen* newScreen, SHORT y) 
{
    UWORD* screenPtr = (UWORD*)((ULONG)newScreen->Data + (newScreen->RowWidth * newScreen->Bitplanes * y));
    UWORD* copPtr = newScreen->BitplanesCopperPointer;
    for(SHORT i = 0; i < newScreen->Bitplanes; i++) {
        ULONG bpl = ((ULONG)screenPtr) + i * newScreen->RowWidth;  
        CPMOVE_L(copPtr, offsetof(struct Custom, bplpt[i]), bpl);
    }
}

void Screen_CreateMask(struct NewScreen* sourceScreen, struct NewScreen* maskScreen) 
{
    CopyMem(sourceScreen, maskScreen, sizeof(struct NewScreen));
    maskScreen->Data = AllocMem(sourceScreen->Size, MEMF_CHIP | MEMF_CLEAR);

    UWORD value;
    USHORT rowWidth = sourceScreen->RowWidth / 2;

    for(UWORD y = 0; y < sourceScreen->Height; y++) {
        for(UWORD x = 0; x < rowWidth; x++) {
            value = 0;
            for(USHORT pl = 0; pl < sourceScreen->Bitplanes; pl++) {
                value |= sourceScreen->Data[y * rowWidth * sourceScreen->Bitplanes + x + pl * rowWidth];
            }
            for(USHORT pl = 0; pl < sourceScreen->Bitplanes; pl++) {
                maskScreen->Data[y * rowWidth * sourceScreen->Bitplanes + x + pl * rowWidth] = value;
            }
        }
    }  
}