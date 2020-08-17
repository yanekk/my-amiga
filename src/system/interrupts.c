#include "interrupts.h"

static APTR GetVBR(void) {
	APTR vbr = 0;
	UWORD getvbr[] = { 0x4e7a, 0x0801, 0x4e73 }; // MOVEC.L VBR,D0 RTE

	if (SysBase->AttnFlags & AFF_68010) 
		vbr = (APTR)Supervisor((ULONG (*)())getvbr);

	return vbr;
}

void SetInterruptHandler(APTR interrupt) {
    if(!VBR) VBR = GetVBR();
	*(volatile APTR*)(((UBYTE*)VBR) + INT_3) = interrupt;
}

APTR GetInterruptHandler() {
    if(!VBR) VBR = GetVBR();
	return *(volatile APTR*)(((UBYTE*)VBR) + INT_3);
}
