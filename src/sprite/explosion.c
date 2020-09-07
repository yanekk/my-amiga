#include <proto/exec.h>
#include <proto/dos.h>

#include "explosion.h"
#include "common.h"
#include "../support/gcc8_c_support.h"
#include "../common/utils.h"
#include "../common/custom_chip.h"

SHORT frameHeights[EXPLOSION_FRAMES] =  { 4, 6, 8, 12, 14, 16, 18, 18, 18, 18, 18 };

INCBIN_CHIP(spriteData, ".\\\\assets\\\\explosion.16x16.spr");

struct ExplosionFrame* Explosion_CurrentFrame(struct Explosion* explosion) 
{
    return explosion->frames[explosion->currentFrame];
}

static inline UBYTE Explosion_CalculateFlags(UWORD spriteVStart, UWORD spriteVStop) {
    return (((spriteVStart & 0xF00) >> 8) << 2) | (((spriteVStop & 0xF00) >> 8) << 1);
}

struct Explosion* Explosion_Create(SHORT hStart, SHORT vStart) 
{
    struct Explosion* explosion = AllocMem(sizeof(struct Explosion), MEMF_CLEAR);
    explosion->currentFrame = 0;
    for(SHORT i = 0; i < EXPLOSION_FRAMES; i++) 
    {
        struct ExplosionFrame* frame = AllocMem(sizeof(struct ExplosionFrame), MEMF_CLEAR);
        frame->height = frameHeights[i];
        frame->hOffset = EXPLOSION_SPRITE_HEIGHT - (frame->height / 2);
        
        SHORT offset = 0;
        for(SHORT j = 0; j < i; j++) {
            offset += frameHeights[j] * EXPLOSION_BITPLANES * 2;
        }
        UWORD spriteVStart = vStart + frame->hOffset;
        UWORD spriteVStop = vStart + frame->height + frame->hOffset;

        ULONG currentSpriteData = ((ULONG)spriteData) + offset;
        struct MySprite *currentSprite = AllocMySprite((struct MySprite) {
            .vStart = (UBYTE)spriteVStart,
            .vStop  = (UBYTE)spriteVStop,
            .hStart = hStart,
            .flags  = Explosion_CalculateFlags(spriteVStart, spriteVStop)
        }, currentSpriteData, frame->height * EXPLOSION_BITPLANES * 2);

        frame->sprite = currentSprite;

        explosion->frames[i] = frame;
    }
    return explosion;
}

void Explosion_NextFrame(struct Explosion* explosion) 
{
    explosion->currentStep++;
    if(explosion->currentStep == EXPLOSION_ANIM_SPEED) 
    {
        struct ExplosionFrame* currentFrame = Explosion_CurrentFrame(explosion);
        struct MySprite* currentSprite = currentFrame->sprite;
        
        explosion->currentStep = 0;
        explosion->currentFrame++;

        if(explosion->currentFrame >= EXPLOSION_FRAMES)
            explosion->currentFrame = 0;

        struct ExplosionFrame* nextFrame = Explosion_CurrentFrame(explosion);
        struct MySprite* nextSprite = nextFrame->sprite;

        nextSprite->hStart = currentSprite->hStart;

        UWORD spriteVStart = VSTART_W(currentSprite)
            - currentFrame->hOffset + nextFrame->hOffset;
        UWORD spriteVStop = VSTOP_W(currentSprite)
            - currentFrame->height - currentFrame->hOffset + nextFrame->height + nextFrame->hOffset;

        nextSprite->vStart = spriteVStart;
        nextSprite->vStop = spriteVStop;
        nextSprite->flags = Explosion_CalculateFlags(spriteVStart, spriteVStop);
    }
}

void Explosion_Move(struct Explosion* explosion, SHORT x, SHORT y) 
{
    struct ExplosionFrame* currentFrame = Explosion_CurrentFrame(explosion);
    currentFrame->sprite->hStart += x;

    UWORD spriteVStart = VSTART_W(currentFrame->sprite) + y;
    UWORD spriteVStop = VSTOP_W(currentFrame->sprite) + y;

    currentFrame->sprite->vStart = spriteVStart;
    currentFrame->sprite->vStop = spriteVStop;
    currentFrame->sprite->flags = Explosion_CalculateFlags(spriteVStart, spriteVStop);
}

void Explosion_Paint(struct Explosion* explosion) {
    UWORD* ptr = explosion->copperListSpritePointer;
    CPMOVE_L(ptr, SPR0PTH, (ULONG)Explosion_CurrentFrame(explosion)->sprite);
}

void Explosion_Free(struct Explosion* explosion) 
{

}