#pragma once
//=====================================================================//
/*! @file
	@brief  fnc.h
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <stdint.h>

void InitFunctions(void);
void LineToSinCos(short *xyxy, short *sincos);
void TurnCwXY(int *in, int *sc, int *out);
void TurnCcwXY(int *in, int *sc, int *out);
void BoundVectorS(short *inxy, short iv, short *outxy);
void BoundVectorP(short *inxy, short *sincos, short *outxy);

void SinCos(short s, short *sincos);

unsigned int Isqrt16(unsigned int a);
unsigned int Isqrt(unsigned int a);
int Mrand(int v, short *p);
int Rand(void);
