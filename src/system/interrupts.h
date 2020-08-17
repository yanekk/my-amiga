#include <exec/types.h>
#include <proto/exec.h>
#include <exec/execbase.h>

#define INT_3 0x6c
#define __interrupt __attribute__((interrupt))

volatile static APTR VBR=0;

void SetInterruptHandler(APTR interrupt);
APTR GetInterruptHandler();