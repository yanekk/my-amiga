#include "screen.h"

static inline BOOL IsLine(USHORT line) {
    return ((*(volatile ULONG*)0xDFF004 >> 8) & 0x1ff) == line;
}

void WaitLine(USHORT line) {
	while (!IsLine(line)) { }
}

void WaitVbl() {
	while (IsLine(311)) { }
    while (!IsLine(311)) { }
}

void WaitForLine10() { WaitLine(0x10); }