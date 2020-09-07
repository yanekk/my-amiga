#pragma once;
#include "system.h"
#include <proto/graphics.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

#include "interrupts.h"

struct GfxBase *GfxBase = NULL;

void System_Kill(struct SystemData* systemData) {
    systemData->custom = (struct Custom *)0xdff000;
    GfxBase = (struct GfxBase *)OpenLibrary(GRAPHICSNAME, 0);
    systemData->copinit = GfxBase->copinit;

    CloseLibrary((struct Library*)GfxBase);
    systemData->SystemInts = systemData->custom->intenar;
    systemData->SystemDMA = systemData->custom->dmaconr;
    systemData->SystemIrq = GetInterruptHandler(); //store interrupt register
}
void System_Restore(struct SystemData* systemData) {
    systemData->custom->cop1lc = (ULONG)systemData->copinit;
    systemData->custom->intreq = 0x7fff;
    systemData->custom->intena = systemData->SystemInts | 0xc000;
    systemData->custom->dmacon = systemData->SystemDMA | 0x8000;
    SetInterruptHandler(systemData->SystemIrq);
}