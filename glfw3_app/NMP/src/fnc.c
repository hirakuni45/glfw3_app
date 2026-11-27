//=========================================================================//
/*! @file
	@brief  Functions engine. Version 1.55b
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 1994, 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include "fnc.h"

#define PAI_32		0xC90FDAA2	/* Pai (3.141592654 * 2^30) */
#define SINCOS_SHIFT	14
#define ANGLE_SIZE	1024

static void MakeupTriFuncTbl(short *p, short count);
static void ShiftRight(int *a, int *c);
static void Mulu32x32(int *a, int *b, int *c);
static void Mul64(int *a, int *b, int *c);
static void Not64(int *a, int *c);
static void Neg64(int *a, int *c);
static void Sub64(int *a, int *b, int *c);
static void Add64(int *a, int *b, int *c);

int rootMod;
int randSeed;
int randBase1, randBase2;
static short randTapTables[] = {
	0x0001,0x0040,	0x0004,0x0040,	/*  7 bits */
	0x0008,0x0100,			/*  9 bits */
	0x0004,0x0200,			/* 10 bits */
	0x0002,0x0400,			/* 11 bits */
	0x0001,0x4000,			/* 15 bits */
	0x0008,0x4000,	0x0004,0x4000	/* 15 bits */
};
static int C1[2] = { 1, 0 };
static short *tblSinTop;
static short sicoTbl[1024];

/*--------------------------------------------------------------/
/	#Initialization functions. #Setup tables and others.	/
/--------------------------------------------------------------*/
void InitFunctions(void)
{
	MakeupTriFuncTbl(sicoTbl, ANGLE_SIZE);
	tblSinTop = sicoTbl;

/* 0 to MAX-1 (Do not initial 'MAX(0xffffffff'.) */
	randBase1 = randBase2 = 0x9218;
	randBase1 ^= 0xffff;
	randSeed = 0;
	rootMod = 0;
}

/*--------------------------------------------------------------/
/	(X0, Y0) to (X1, Y1)					/
/--------------------------------------------------------------*/
void LineToSinCos(short *xyxy, short *sincos)
{
	int	x0, y0, x1, y1, dx, dy, a;

	x0 = *xyxy++;
	y0 = *xyxy++;
	x1 = *xyxy++;
	y1 = *xyxy;
	dx = x1 - x0;
	dy = y1 - y0;
	a = Isqrt(dx * dx + dy * dy);
	if(a==0)
	{
	*sincos++ = 0;
	*sincos = 0;
	}
	else
	{
	*sincos++ = (dy<<14) / a;
	*sincos   = (dx<<14) / a;
	}
}

/*--------------------------------------------------------------/
/	Turn CW or CCW by X, Y (Rotation X, Y --> X', Y')	/
/	CW:							/
/		X' =  X COS(s) + Y SIN(s)			/
/		Y' =  Y COS(s) - X SIN(s)			/
/	CCW:							/
/		X' =  X COS(s) - Y SIN(s)			/
/		Y' =  Y COS(s) + X SIN(s)			/
/--------------------------------------------------------------*/
void TurnCwXY(int *in, int *sc, int *out)
{
	out[0] = (in[0] * sc[1] + in[1] * sc[0]) >> SINCOS_SHIFT;
	out[1] = (in[1] * sc[1] - in[0] * sc[0]) >> SINCOS_SHIFT;
}

void TurnCcwXY(int *in, int *sc, int *out)
{
	out[0] = (in[0] * sc[1] - in[1] * sc[0]) >> SINCOS_SHIFT;
	out[1] = (in[1] * sc[1] + in[0] * sc[0]) >> SINCOS_SHIFT;
}

/*--------------------------------------------------------------/
/	Bound Vector (reflection vector)			/
/	X' = -X COS(2s) - Y SIN(2s)				/
/	Y' = -X SIN(2s) + Y COS(2s)				/
/--------------------------------------------------------------*/
void BoundVectorS(short *inxy, short iv, short *outxy)
{
	short	sc[2];
	int	x, y, sin, cos;

	x = *inxy++;
	y = *inxy;
	SinCos((iv+0x4000)<<1, sc);
	sin = sc[0];
	cos = sc[1];
	*outxy++ = (-(x * cos)-(y * sin))>>15;
	*outxy   = ( (y * cos)-(x * sin))>>15; 
}

void BoundVectorP(short *inxy, short *sincos, short *outxy)
{
	short	sc[2];
	int	x, y, sin, cos;

	x = *inxy++;
	y = *inxy;
	sin = *sincos++;
	cos = *sincos;
	*outxy++ = (-(x * cos)-(y * sin))>>15;
	*outxy   = ( (y * cos)-(x * sin))>>15; 
}

/*--------------------------------------------------------------/
/	Get SIN(s), COS(s) value.				/
/	s = 0 to 65535 (2 pai Raidian)				/
/	Exit value 0x4000 -- 0 -- -0x4000			/
/--------------------------------------------------------------*/
void SinCos(short s, short *sincos)
{
	short sa, sb;

	if(s & 0x8000) sa=-1; else sa=0;
	if(s & 0x4000) sb=-1; else sb=0;
	s = ((s >> 4) ^ sb) & ANGLE_SIZE-1;
	*sincos++ = (tblSinTop[s] ^ sa) + sa;
	s ^= ANGLE_SIZE-1;
	sa ^= sb;
	*sincos = (tblSinTop[s] ^ sa) + sa;
}

/*--------------------------------------------------------------/
/	Makeup trigonometrical function tables. (Pai/4 radian)	/
/	Not use Flotting-Point.					/
/								/
/	X' = X COS(s) - Y SIN(S)				/
/	Y' = X SIN(s) + Y COS(S)				/
/								/
/	if .s<<0. then ..					/
/	COS(s) =. 1						/
/	SIN(s) =. s						/
/								/
/	X' = X   - Y s						/
/	Y' = X s - Y						/
/								/
/--------------------------------------------------------------*/
static void MakeupTriFuncTbl(short *p, short count)
{
	int	i, j, w, x[2], y[2], s[2], pi[2];

	pi[0]=PAI_32; pi[1]=0;
	x[0]=0x00000000; x[1]=0x40000000;
	y[0]=0x00000000; y[1]=0x00000000;
	for(i=0; i<count; ++i)
	{
	*p++ = y[1] >> 16;

	w = y[1];
	if(w & 0x80000000) { j=1; w *= -1; } else j=0;
	Mulu32x32(&w, pi, s);
	if(j) Neg64(s, s);
	for(j=0; j<9; ++j) ShiftRight(s, s);
	Sub64(x, s, x);

	w = x[1];
	if(w & 0x80000000) { j=1; w *= -1; } else j=0;
	Mulu32x32(&w, pi, s);
	if(j) Neg64(s, s);
	for(j=0; j<9; ++j) ShiftRight(s, s);
	Add64(y, s, y);
	}
}

static void ShiftRight(int *a, int *c)
{
	int	s;

	*c = *a;
	s = *(c+1) = *(a+1);
	*(c+1) >>= 1;
	*(unsigned int *)c >>= 1;
	if(s & 1) *c |= 0x80000000;
}

static void Mulu32x32(int *a, int *b, int *c)
{
	int	i, s[2];
	unsigned int al, ah, bl, bh;

	al=a[0] & 0xffff;
	ah=(unsigned int)a[0] >> 16;
	bl=b[0] & 0xffff;
	bh=(unsigned int)b[0] >> 16;

	c[1]=ah * bh;
	c[0]=al * bl;
	s[0]=ah * bl; s[1]=0;
	for(i=0; i<16; ++i) Add64(s, s, s);
	Add64(c, s, c);

	s[0]=al * bh; s[1]=0;
	for(i=0; i<16; ++i) Add64(s, s, s);
	Add64(c, s, c);
}

static void Mul64(int *a, int *b, int *c)
{
	int	s, wa, wb;

	wa = *a;
	wb = *b;
	if(wa & 0x80000000) { s=1; wa *= -1; } else s=0;
	if(wb & 0x80000000) { s^=1; wb *= -1; } else ;
	Mulu32x32(&wa, &wb, c);
	if(s) Neg64(c, c);
}

static void Not64(int *a, int *c)
{
	*c = *a ^ -1L;
	*(c+1) = *(a+1) ^ -1L;
}

static void Neg64(int *a, int *c)
{
	int	w[2];

	w[0] = a[0]; w[1] = a[1];
	Not64(w, w);
	Add64(w, C1, c);
}

static void Sub64(int *a, int *b, int *c)
{
	Neg64(b, b);
	Add64(a, b, c);
}

static void Add64(int *a, int *b, int *c)
{
	unsigned int	al, ah, bl, bh, s;

	al = *a & 0xffff;
	ah = *(unsigned int *)a >> 16;
	bl = *b & 0xffff;
	bh = *(unsigned int *)b >> 16;
	s = (al+bl) >> 16;
	s = (ah+bh+s) >> 16;
	*c = *a + *b;
	*(c+1) = *(a+1) + *(b+1);
	if(s) (*(c+1))++;
}

/*--------------------------------------------------------------/
/	Integer Square ROOT (16 Bits renge)			/
/	Exit:							/
/		0x0000AAAA (AAAA := Answer)			/
/		rootMod := Mod of sqrt_16.			/
/--------------------------------------------------------------*/
unsigned int Isqrt16(unsigned int a)
{
	int	i;
	unsigned int	b;

	b = 0x00004000;
	for(i=0; i<8; ++i)
	{
	if(a>=b) { a -= b;
		b = ((b + b) & 0xfffe0000) + 0x10000 + (b & 0xffff);
		}
	else b = ((b + b) & 0xfffe0000) + (b & 0xffff);
	a <<= 2;
	}
	rootMod = (a & 0xffff0000) >> 16;
	return (b >> 16);
}

/*--------------------------------------------------------------/
/	Integer Square ROOT (32 Bits renge)			/
/	Exit:							/
/		0xAAAAAAAA (AAAAAAAA := Answer)			/
/		rootMod := Mod of sqrt_32			/
/--------------------------------------------------------------*/
unsigned int Isqrt(unsigned int al)
{
	int	i;
	unsigned int	ah, bh, bl;

	ah = bh = 0;
	bl = 0x40000000;
	for(i=0; i<16; ++i)
	{
	if(al>=bl)
		{
		if(ah>=bh) { ah-=bh; al-=bl; bh+=bh+1; }
		else bh+=bh;
		}
	else	{
		if(ah>=(bh+1)) { ah-=bh+1; al-=bl; bh+=bh+1; }
		else bh+=bh;
		}
	ah <<= 2; ah += al >> 30; al <<= 2;
	}
	rootMod = ah;
	return bh;
}


/*--------------------------------------------------------------/
/	Rand							/
/--------------------------------------------------------------*/
int Rand(void)
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
int Mrand(int v, short *p)
{
	int	s, c;

	c = *p++;
	if(v & c) s=1; else s=0;
	c = *p;
	if(v & c) return v + v + s;
	else return v + v + (s ^ 1);
}
