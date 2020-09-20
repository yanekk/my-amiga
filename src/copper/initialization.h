#include <exec/types.h>
#include "../copper/screen.h"
#include "../copper/display.h"

UWORD* Initialize_Display(UWORD* copPtr, struct Display* display);
UWORD* Initialize_LogoScreen(UWORD* copPtr, struct NewScreen* logoScreen, struct Display* display);
void Initialize_TextScreen(struct NewScreen* textScreen, struct Display* display);
UWORD* Initialize_ScreenBuffer(UWORD* copPtr, struct NewScreen* textScreen, struct Display* display);
void Initialize_FontScreen(struct NewScreen* fontScreen);