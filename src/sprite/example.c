#include <clib/exec_protos.h>
#include <stdlib.h>

#include "../vendor/animtools.h"
#include "../common/screen.h"

#include "example.h"

static struct VSprite* InitializeMySprite(struct RastPort* rp, WORD* colors, SHORT x, SHORT y) 
{
    UWORD myImage_1[] = {
        0b0000011111100000,
        0b0000000000000000,

        0b0001100000011000,
        0b0000011111100000,
        
        0b0010001111000100,
        0b0001111111111000,

        0b0100111001110010,
        0b0011111001111100,

        0b0101100000011010,
        0b0011100000011100,

        0b1001000000001001,
        0b0111000000001110,  

        0b1010000000000101,
        0b0110000000000110,  

        0b1010000000000101,
        0b0110000000000110,  

        0b1010000000000101,
        0b0110000000000110,  

        0b1010000000000101,
        0b0110000000000110,    

        0b1001000000001001,
        0b0111000000001110, 

        0b0101100000011010,
        0b0011100000011100,

        0b0100111001110010,
        0b0011111001111100,

        0b0010001111000100,
        0b0001111111111000,

        0b0001100000011000,
        0b0000011111100000,

        0b0000011111100000,
        0b0000000000000000,
    };

    WORD* myImage_1Chip = AllocVec(sizeof(myImage_1), MEMF_CHIP);
    CopyMem(myImage_1, myImage_1Chip, sizeof(myImage_1));
    
    struct VSprite* myVSprite = makeVSprite(&(NEWVSPRITE) 
    {
        .nvs_Image = myImage_1Chip,
        .nvs_ColorSet = colors,
        .nvs_WordWidth = 1,
        .nvs_LineHeight = 16,
        .nvs_ImageDepth = 2,
        .nvs_X = x,
        .nvs_Y = y,
        .nvs_Flags = VSPRITE,
    });
    
    AddVSprite(myVSprite, rp);
    
    return myVSprite;
}

static WORD* SpriteColorSet_Create(WORD colors[]) {
    WORD* ptr = AllocVec(sizeof(WORD) * 3, MEMF_CLEAR);
    CopyMem(colors, ptr, sizeof(WORD) * 3);
    return ptr;
}

void RunVSpriteExample(struct ViewInfo * vi) {
    WORD* colors = SpriteColorSet_Create((WORD[]) { 0x10C, 0x06C, 0x0AC });
    struct VSprite* sprites[SPRITES_COUNT] = {};

    SHORT x = 0;
    for(size_t i = 0; i < SPRITES_COUNT; i++) {
        sprites[i] = InitializeMySprite(&vi->rastPort, colors, x, 0);
        x += 20;
    }

    RefreshScreen(vi);
    LoadView(&vi->view);

    for(size_t i = 0; i < SPRITES_COUNT; i++) {
        for(int j = 0; j < 16; j++) {
            sprites[i]->Y += 1;
            RefreshScreen(vi);
        }
    }
    Delay(1 * TICKS_PER_SECOND);

    for(size_t i = 0; i < SPRITES_COUNT; i++) {
        freeVSprite(sprites[i]);
    }
    FreeVec(colors);
}