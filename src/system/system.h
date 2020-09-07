#pragma once
#include <hardware/custom.h>
#include <exec/types.h>
#include <graphics/copper.h>

struct SystemData {
    struct Custom *custom;
    struct copinit *copinit;
    UWORD SystemInts;
    UWORD SystemDMA;
    APTR SystemIrq;
};

void System_Kill(struct SystemData* systemData);
void System_Restore(struct SystemData* system);