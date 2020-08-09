#include <proto/exec.h>
#include <graphics/collide.h>

#include "../vendor/animtools.h"
#include "../common/screen.h"
#include "../common/utils.h"

#include "example.h"
#define DEPTH 3

static struct Bob* InitializeMyBob(struct RastPort* rp, short id, SHORT x, SHORT y) 
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

    UWORD myImage_1collMask[] = {
        0b0000011111100000, 
        0b0001111111111000, 
        0b0011111111111100, 
        0b0111111111111110, 
        0b0111111111111110, 
        0b1111111111111111, 
        0b1111111111111111,
        0b1111111111111111,
        0b1111111111111111, 
        0b1111111111111111, 
        0b1111111111111111,
        0b0111111111111110,
        0b0111111111111110,
        0b0011111111111100, 
        0b0001111111111000,
        0b0000011111100000, 
    };


    WORD* myImage_1Chip = AllocVec(sizeof(myImage_1), MEMF_CHIP);
    CopyMem(myImage_1, myImage_1Chip, sizeof(myImage_1));

    WORD* myImage_1collMaskChip = AllocVec(sizeof(myImage_1collMask), MEMF_CHIP);
    CopyMem(myImage_1collMask, myImage_1collMaskChip, sizeof(myImage_1collMask));

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
        .nb_PlanePick = 0b111,
        .nb_HitMask =  0b11,
        .nb_MeMask = 0b10,
        .nb_ImageShadow = myImage_1collMaskChip,
        .nb_Identifier = id
    });
    AddBob(myBob, rp);    
    return myBob;
}

BOOL ballsCollided = FALSE;

static void BallsCollision() 
{
    ballsCollided = TRUE;
}

SHORT direction = 1;

static void BoundaryCollision(struct VSprite* sprite, LONG borderFlags) 
{
    UNUSED(sprite);

    if((borderFlags & LEFTHIT) != 0)
        direction = 1;

    if((borderFlags & RIGHTHIT) != 0) 
        direction = -1;
}

void RunBobExample(struct ViewInfo * vi) {
    SetCollision(1, BallsCollision, vi->rastPort.GelsInfo);
    SetCollision(BORDERHIT, (void (*)(struct VSprite*, struct VSprite *))BoundaryCollision, vi->rastPort.GelsInfo);

    struct Bob* sprites[BALL_COUNT] = { 0 };

    SHORT x = 10;
    for(SHORT i = 0; i < BALL_COUNT; i++) {
        x += 20 * i;
        sprites[i] = InitializeMyBob(&vi->rastPort, i, x, 20);
    }

    RefreshScreen(vi);
    LoadView(&vi->view);
    
    int currentBall = 0;
    while(TRUE) {
        sprites[currentBall]->BobVSprite->X += direction * 2;
        DoCollision(&vi->rastPort);
        RefreshScreen(vi);
        if(ballsCollided) {
            ballsCollided = FALSE;
            currentBall += direction;
        }
    }
    
    Delay(1 * TICKS_PER_SECOND);
    
    for(SHORT i = 0; i < BALL_COUNT; i++) {
        freeBob(sprites[i], DEPTH);
    }
}