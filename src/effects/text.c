#include <string.h>
#include <exec/types.h>
#include "../copper/screen.h"
#include "../blitter/copy.h"
#include "text.h"

#define FONT_LETTER_WIDTH 16
#define FONT_LETTER_HEIGHT 16
#define FONT_LETTERS_PER_LINE 18 * FONT_LETTER_WIDTH
#define FONT_PLOT_Y 100

#define SCROLL_SPEED 1

static UWORD plotX;

static struct NewScreen* sScreen;
static struct NewScreen* dScreen;

static UWORD scrollCounter = 0;
static USHORT letterIndex = 0;
static USHORT letterCount;
static char* letters;

void TextPlotting_Initialize(struct NewScreen* sourceScreen, struct NewScreen* destScreen, char* text) {
    plotX = destScreen->Width - FONT_LETTER_WIDTH;
    dScreen = destScreen;
    sScreen = sourceScreen;
    
    letters = text;
    letterCount = strlen(letters);
}

void TextPlotting_Scroll() {

    if(scrollCounter == 0) {
        USHORT letterNumber = letters[letterIndex] - 0x1e;

        Blitter_CopyAtoB(sScreen,
            letterNumber % FONT_LETTERS_PER_LINE,
            letterNumber / FONT_LETTERS_PER_LINE,
            FONT_LETTER_WIDTH, FONT_LETTER_HEIGHT,
            dScreen, plotX, FONT_PLOT_Y);

        letterIndex++;
        if(letterIndex >= letterCount) letterIndex = 0;
    }
    
    scrollCounter += SCROLL_SPEED;
    if(scrollCounter == FONT_LETTER_WIDTH)
        scrollCounter = 0;
    Blitter_ShiftALeft(dScreen, plotX, FONT_PLOT_Y, 
        dScreen->Width, FONT_LETTER_HEIGHT, SCROLL_SPEED);
}

