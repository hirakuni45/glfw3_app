#pragma once
//=========================================================================//
/*! @file
	@brief  コリジョン座標関係 Necronomicon・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>
#include "ballsim.h"

void PickUpFirst(const short *vect, int vnum);
void MakeupVectors(FIELD *para, const short *vect, int vnum, const char *);
void TestMap(int width, int height, int scale, int fff);
void DrawOutLine(short *po, int width, int height, int scale);

#define HIT_MAX		800
#define TAG_MAX		2200

extern HIT hitList[HIT_MAX];
extern short hitMax, tagMax;
extern short listTag[TAG_MAX];
extern short zoneMap[ZONE_DIVIDE][ZONE_DIVIDE];
extern BALL ball[MULTI_BALL_MAX];
extern SWITCH swPara[32];
extern FLIPPER flipper[4];
extern short fieldWidthMax;
extern short startBallX, startBallY, startBallZ;
extern short psychoSaveX, psychoSaveY, psychoSaveZ;
extern short vukPosX[VUK_MAX], vukPosY[VUK_MAX], vukPosZ[VUK_MAX];
extern short magnaPosX, magnaPosY, magnaRadius;
extern int magnaRadius2;
extern short ballRadius;
extern short flipperMax, ballMax, psychoSaveMax,  messengerBallMax;
extern short jetBumperMax, dropTargetMax, standUpMax, holeMax;
extern short vukMax, rampMax, flipperMax;
extern short slingShotMax;
extern short movegateMax, magnaLockMax;
