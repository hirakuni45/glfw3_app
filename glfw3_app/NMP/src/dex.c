#include <stdio.h>
#include "dex.h"

static int Mrand(int v, const short *p);
static int Rand(void);

static int randSeed;
static int randBase1, randBase2;
static const short randTapTables[] = {
	0x0001,0x0040,	0x0004,0x0040,	/*  7 bits */
	0x0008,0x0100,			/*  9 bits */
	0x0004,0x0200,			/* 10 bits */
	0x0002,0x0400,			/* 11 bits */
	0x0001,0x4000,			/* 15 bits */
	0x0008,0x4000,	0x0004,0x4000	/* 15 bits */
};
static char ioBuf[16];

const char *fdex(const char *txt, int s)
{
	int	i, j, k;
	unsigned int a, b, c;

	randBase1 = randBase2 = 0;

	randSeed = s;

	for(i=0; i<4; ++i) {
	randBase1 <<= 8;
	a = *txt++;
	randBase1 |= a;
	randBase2 <<= 8;
	b = *txt++;
	randBase2 |= b;
	k = a ^ b;
	k &= 16;
	randBase1 ^= randSeed << k;
	randBase2 ^= randSeed << k;
	Rand();
	}

	j = (s + 1) * 5;
	for(i=0; i<j; ++i) Rand();

	a = randBase1;
	b = randBase2;
	c = 0;
	for(i=0; i<13; ++i) {
	ioBuf[i] = a & 31;
	a >>= 5;
	a |= b << (32 - 5);
	b >>= 5;
	}
	ioBuf[13] = 0;

	return ioBuf;
}

/*--------------------------------------------------------------/
/	Rand							/
/--------------------------------------------------------------*/
static int Rand(void)
{
	randBase1 = Mrand(randBase1, &randTapTables[6 * 2]);
	randBase1 += (randBase1 << 2) + 1;
	randBase2 = Mrand(randBase2, &randTapTables[7 * 2]);
	randBase2 += (randBase2 << 2) + 1;
	randSeed ^= randBase1 ^ (randBase2 << 16);
	return randSeed;
}

/*--------------------------------------------------------------/
/	mRand n Bits renge					/
/	Entry:							/
/		a = input value.				/
/		p = mRand type. (0 to 7)			/
/								/
/		0, 1    =  7 bits				/
/		2       =  9 bits				/
/		3       = 10 bits				/
/		4       = 11 bits				/
/		5, 6, 7 = 15 bits				/
/	Exit:							/
/		0x0000AAAA (0000AAAA := Answer)			/
/--------------------------------------------------------------*/

static int Mrand(int v, const short *p)
{
	int	s, c;

	c = *p++;
	if(v & c) s=1; else s=0;
	c = *p;
	if(v & c) return v + v + s;
	else return v + v + (s ^ 1);
}
