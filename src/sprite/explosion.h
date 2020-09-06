#pragma once
#include <proto/exec.h>
#include "common.h"

#define EXPLOSION_FRAMES 11
#define EXPLOSION_BITPLANES 2
#define EXPLOSION_ANIM_SPEED 3
#define EXPLOSION_SPRITE_HEIGHT 18

struct ExplosionFrame {
    struct MySprite* sprite; 
    SHORT height;
    SHORT hOffset;
};

struct Explosion {
    SHORT currentFrame;
    SHORT currentStep;
    UWORD* copperListSpritePointer;
    struct ExplosionFrame* frames[EXPLOSION_FRAMES];
};

struct Explosion* Explosion_Create(SHORT vStart, SHORT hStart);
void Explosion_NextFrame(struct Explosion* explosion);
void Explosion_Move(struct Explosion* explosion, SHORT x, SHORT y);
void Explosion_Paint(struct Explosion* explosion);
struct ExplosionFrame* Explosion_CurrentFrame(struct Explosion* explosion);
void Explosion_Free(struct Explosion* explosion);
