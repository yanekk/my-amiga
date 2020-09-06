#include <proto/exec.h>
#include "common.h"

struct MySprite* AllocMySprite(struct MySprite newSprite, const UWORD *spriteData, SHORT spriteDataSize) {
    struct MySprite* spritePtr = (struct MySprite*) AllocMem(sizeof(struct MySprite) + spriteDataSize, MEMF_CHIP);
    spritePtr->vStart = newSprite.vStart;
    spritePtr->hStart = newSprite.hStart;
    spritePtr->vStop = newSprite.vStop;
    spritePtr->flags = newSprite.flags;
    CopyMem((UWORD*)spriteData, &spritePtr->data, spriteDataSize);
    return spritePtr;
}

void FreeMySprite(struct MySprite *sprite, SHORT spriteDataSize) {
    FreeMem(sprite, sizeof(struct MySprite) + spriteDataSize);
}