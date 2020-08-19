#pragma once
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#define UNUSED(x) (void)(x)

/* Helpers */
#define GETBYTE_HI(var) (*(volatile UBYTE*)&var)
#define GETBYTE(var) GETBYTE_HI(var)
#define GETBYTE_LO(var) (*((volatile UBYTE*)&var+1))
#define GETWORD(var) (*(volatile UWORD*)&var)
#define GETLONG(var) (*(volatile ULONG*)&var)

/* Copperlist helpers */
#define CPCOMMAND(cpr, var1, var2) *cpr++ = var1; *cpr++ = var2
#define CPMOVE(cpr, addr, data) CPCOMMAND(cpr, addr, data)

#define CPMOVE_L(cpr, addr, ldata) \
    CPMOVE(cpr, addr, ((UWORD)(ldata >> 16))); \
    CPMOVE(cpr, (addr)+2, ((UWORD)ldata)) 

#define CPWAIT(cpr, vhpos, flags) CPCOMMAND(cpr, vhpos, flags)
#define CPEND(cpr) CPCOMMAND(cpr, 0xffff, 0xfffe)
#define CPLINE(h, v) ((v) | (h) << 8)

#define __chip __attribute__((section (".MEMF_CHIP")))

#define LONG_PTR(ptr) *(LONG*)(&ptr)