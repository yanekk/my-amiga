#include <proto/exec.h>

#include "initialization.h"
#include "../copper/screen.h"
#include "../copper/display.h"
#include "../support/gcc8_c_support.h"

INCBIN_CHIP(fontData, ".\\\\assets\\\\font.288x96.raw");
INCBIN(imageData, ".\\\\assets\\\\image.164x72.raw");
INCBIN(imageColors, ".\\\\assets\\\\palette.raw");

INCBIN(fontColors, ".\\\\assets\\\\font-palette.raw");

UWORD* Initialize_Display(UWORD* copPtr, struct Display* display) {
    display->Width = 320;
    display->Height = 256;
    display->TopMargin = 76;
    display->LeftMargin = 110;
    return Display_Create(copPtr, display);
}

UWORD* Initialize_LogoScreen(UWORD* copPtr, struct NewScreen* logoScreen, struct Display* display) {
    logoScreen->Width = 176;
    logoScreen->Height = 86;
    logoScreen->Bitplanes = 3;
    logoScreen->Palette = (UWORD*)imageColors;
    logoScreen->Display = display;

    UWORD screenSize = Screen_Size(logoScreen);
    logoScreen->Data = AllocMem(screenSize, MEMF_CHIP | MEMF_CLEAR);
    CopyMem(imageData, logoScreen->Data, screenSize);    

    return Screen_Create(copPtr, logoScreen);
}

UWORD* Initialize_TextScreen(UWORD* copPtr, struct NewScreen* textScreen, struct Display* display) {
    textScreen->Width = 352;
    textScreen->Height = display->Height;
    textScreen->Bitplanes = 3;
    textScreen->Size = Screen_Size(textScreen);

    textScreen->Data = AllocMem(textScreen->Size, MEMF_CHIP | MEMF_CLEAR);
    /*UWORD c = 0;
    for(ULONG i = 0; i < textScreen.Size; i++) {
        bottomScreen[i] = c;
        c++;
    }*/

    textScreen->Palette = fontColors;    
    textScreen->Display = display;

    return Screen_Create(copPtr, textScreen);
}

void Initialize_FontScreen(struct NewScreen* fontScreen) {
    fontScreen->Width = 288;
    fontScreen->Height = 96;
    fontScreen->Data = fontData;
    fontScreen->Bitplanes = 3;
    fontScreen->BitplaneSize = Screen_BitplaneSize(fontScreen);
    fontScreen->Size = Screen_Size(fontScreen);
    fontScreen->RowWidth = Screen_RowWidth(fontScreen);
}