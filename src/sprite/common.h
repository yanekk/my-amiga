#pragma once
#include <exec/types.h>
#define VSTARTH_MASK 1 << 2
#define VSTOPH_MASK 1 << 1

#define VSTART_W(sprite) ((UWORD)sprite->vStart | ((((UWORD)sprite->flags) & VSTARTH_MASK) << 6))
#define VSTOP_W(sprite) ((UWORD)sprite->vStop | ((((UWORD)sprite->flags) & VSTOPH_MASK) << 7)) 

struct MySprite {
    UBYTE vStart;
    UBYTE hStart;
    UBYTE vStop;
    UBYTE flags;
    WORD data[];
};

struct MySprite *AllocMySprite(struct MySprite newSprite, const UWORD *spriteData, SHORT spriteDataSize);