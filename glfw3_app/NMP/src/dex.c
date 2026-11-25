//=========================================================================//
/*! @file
	@brief  スコア暗号化プロトコル
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdio.h>
#include "dex.h"

static uint32_t Mrand(uint32_t v, const uint16_t *p);
static uint32_t Rand(void);

static uint32_t randSeed;
static uint32_t randBase1, randBase2;
static const uint16_t randTapTables[] = {
	0x0001,0x0040,	0x0004,0x0040,	/*  7 bits */
	0x0008,0x0100,			/*  9 bits */
	0x0004,0x0200,			/* 10 bits */
	0x0002,0x0400,			/* 11 bits */
	0x0001,0x4000,			/* 15 bits */
	0x0008,0x4000,	0x0004,0x4000	/* 15 bits */
};
static char ioBuf[16];

const char *fdex(const char *txt, int seed)
{
	randBase1 = randBase2 = 0;
	randSeed = seed;

	for(int i = 0; i < 4; ++i) {
		randBase1 <<= 8;
		uint32_t a = *(unsigned char *)txt++;
		randBase1 |= a;
		randBase2 <<= 8;
		uint32_t b = *(unsigned char *)txt++;
		randBase2 |= b;
		uint32_t k = a ^ b;
		k &= 16;
		randBase1 ^= randSeed << k;
		randBase2 ^= randSeed << k;
		Rand();
	}

	int j = (seed + 1) * 5;
	for(int i = 0; i < j; ++i) Rand();

	uint32_t a = randBase1;
	uint32_t b = randBase2;
	for(int i = 0; i < 13; ++i) {
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
static uint32_t Rand(void)
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

static uint32_t Mrand(uint32_t v, const uint16_t *p)
{
	uint32_t	s, c;

	c = *p++;
	if(v & c) s=1; else s=0;
	c = *p;
	if(v & c) return v + v + s;
	else return v + v + (s ^ 1);
}
