#include <proto/exec.h>

#include "initialization.h"
#include "../copper/screen.h"
#include "../copper/display.h"
#include "../support/gcc8_c_support.h"

INCBIN_CHIP(fontData, ".\\\\assets\\\\font.288x82.bltraw");
INCBIN(fontColors, ".\\\\assets\\\\font-palette.raw");

INCBIN(imageData, ".\\\\assets\\\\image.166x72.bltraw");
INCBIN(imageColors, ".\\\\assets\\\\palette.raw");

UWORD* Initialize_Display(UWORD* copPtr, struct Display* display) {
    display->Width = 320;
    display->Height = 256;
    display->TopMargin = 76;
    display->LeftMargin = 129;
    return Display_Create(copPtr, display);
}

UWORD* Initialize_LogoScreen(UWORD* copPtr, struct NewScreen* logoScreen, struct Display* display) {
    logoScreen->Width = 176;
    logoScreen->Height = 72;
    logoScreen->Bitplanes = 3;
    logoScreen->Palette = (UWORD*)imageColors;
    logoScreen->Display = display;

    struct NewScreen logoScreenWithMargin = {
        .Width = logoScreen->Width,
        .Height = logoScreen->Height + 10,
        .Bitplanes = logoScreen->Bitplanes
    };
    
    logoScreen->Data = AllocMem(Screen_Size(&logoScreenWithMargin), MEMF_CHIP | MEMF_CLEAR);
    CopyMem(imageData, logoScreen->Data, Screen_Size(logoScreen));    

    return Screen_Create(copPtr, logoScreen);
}

UWORD* Initialize_TextScreen(UWORD* copPtr, struct NewScreen* textScreen, struct Display* display) {
    textScreen->Width = 336;
    textScreen->Height = display->Height;
    textScreen->Bitplanes = 3;
    textScreen->Size = Screen_Size(textScreen);
    
    textScreen->Data = AllocMem(textScreen->Size, MEMF_CHIP | MEMF_CLEAR);
    
    /*UWORD c = 0xFFFF;
    
    for(UWORD i = 0; i < textScreen->Size/2; i++) {
        textScreen->Data[i] = c;
    }
    */

    textScreen->Palette = fontColors;    
    textScreen->Display = display;

    return Screen_Create(copPtr, textScreen);
}

void Initialize_FontScreen(struct NewScreen* fontScreen) {
    fontScreen->Width = 288;
    fontScreen->Height = 96;
    fontScreen->Data = fontData;
    fontScreen->Bitplanes = 3;
    fontScreen->RowWidth = Screen_RowWidth(fontScreen);
    fontScreen->BitplaneSize = Screen_BitplaneSize(fontScreen);
    fontScreen->Size = Screen_Size(fontScreen);
}