#ifndef _TDRbits_included_
#define _TDRbits_included_


#define BIT(n)       (1 << (n))
#define SETBIT(n,i)  ((n) |= BIT(i))
#define CLRBIT(n,i)  ((n) &= ~BIT(i))
#define READBIT(n,i) ((int)((n) & BIT(i)) >> (i))
#define TESTBIT(n,i) ((bool)(((n) & BIT(i)) != 0))

#endif

