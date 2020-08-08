#include <clib/exec_protos.h>
#include <stdlib.h>

#include "../vendor/animtools.h"
#include "../common/screen.h"

#include "example.h"
#define DEPTH 3

static struct Bob* InitializeMyBob(struct RastPort* rp, /*WORD* colors,*/ SHORT x, SHORT y) 
{
    UWORD myImage_1[] = {
        0b0000011111100000, 
        0b0001100000011000, 
        0b0010001111000100, 
        0b0100111001110010, 
        0b0101100000011010, 
        0b1001000000001001, 
        0b1010000000000101,
        0b1010000000000101,
        0b1010000000000101, 
        0b1010000000000101, 
        0b1001000000001001,
        0b0101100000011010,
        0b0100111001110010,
        0b0010001111000100, 
        0b0001100000011000,
        0b0000011111100000, 

        0b0000000000000000, 
        0b0000011111100000, 
        0b0001111111111000, 
        0b0011111001111100, 
        0b0011100000011100, 
        0b0111000000001110, 
        0b0110000000000110, 
        0b0110000000000110, 
        0b0110000000000110, 
        0b0110000000000110, 
        0b0111000000001110,  
        0b0011100000011100, 
        0b0011111001111100, 
        0b0001111111111000, 
        0b0000011111100000, 
        0b0000000000000000, 

        0b0000000000000000, 
        0b0000000000000000, 
        0b0000000000000000, 
        0b0000000110000000, 
        0b0000011111100000, 
        0b0000111111110000,
        0b0001111111111000, 
        0b0001111111111000, 
        0b0001111111111000, 
        0b0001111111111000, 
        0b0000111111110000, 
        0b0000011111100000, 
        0b0000000110000000, 
        0b0000000000000000,
        0b0000000000000000, 
        0b0000000000000000,
    };
    

    WORD* myImage_1Chip = AllocVec(sizeof(myImage_1), MEMF_CHIP);
    CopyMem(myImage_1, myImage_1Chip, sizeof(myImage_1));

    struct Bob* myBob = makeBob(&(NEWBOB) 
    {
        .nb_Image = myImage_1Chip,
        .nb_WordWidth = 1,
        .nb_LineHeight = 16,
        .nb_ImageDepth = DEPTH,
        .nb_X = x,
        .nb_Y = y,
        .nb_BFlags = OVERLAY | SAVEBACK,
        .nb_RasDepth = DEPTH,
        .nb_PlanePick = 0b111
    });

    AddBob(myBob, rp);    
    return myBob;
}
void RunBobExample(struct ViewInfo * vi) {
    struct Bob* sprites[BOB_COUNT] = { 0 };

    SHORT x = 0;
    for(size_t i = 0; i < BOB_COUNT; i++) {
        sprites[i] = InitializeMyBob(&vi->rastPort, x, 0);
        x += 20;
    }

    RefreshScreen(vi);
    LoadView(&vi->view);

    for(size_t i = 0; i < BOB_COUNT; i++) {
        for(int j = 0; j < 16; j++) {
            sprites[i]->BobVSprite->Y += 1;
            RefreshScreen(vi);
        }
    }
    Delay(1 * TICKS_PER_SECOND);

    for(size_t i = 0; i < BOB_COUNT; i++) {
        freeBob(sprites[i], DEPTH);
    }
}