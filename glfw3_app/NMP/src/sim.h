#pragma once
//=========================================================================//
/*! @file
	@brief  コリジョン関係 Necronomicon・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include "ballsim.h"

int PrintAverage(char *, int n);
void InitVibration(void);
void ServiceVibration(void);
void InitBallSimulator(FIELD *para, const short *vect, int vnum, const char *msg);
void MainBallSimulator(void);
void BallSimulatorMotionObjects(void);
void Perspective(const int *xyz, int *xys);
void AutoFlipper(void);
void LocationBallPosition(void);
int AddPlayerBall(void);
int AddPsychoSave(void);
int AddLoopShooter(void);

int MoveTestBall(int d);

int ScanVUK(void);

extern short totalPlays;
extern short dropTargetSw[8], dropTargetTotal[8];
extern short standUpSw[16], standUpTotal[16];
extern short holeSw[8], holeTotal[8];
extern short rampSw[4], rampTotal[4];
extern short jetBumperSw, jetBumperTotal;
extern short slingShotSw, slingShotTotal;
extern short topLaneSw, topLaneTotal;
extern short returnLaneSw, returnLaneTotal;
extern unsigned int playTime, playTimeTotal;
extern short exitBallCount;

extern PLAYER playerTab[PLAYER_MAX];
extern short shoottingCounter;
extern unsigned short shooterSw;
extern int frameRate;
extern int screenWidth, screenHeight;
extern short slingShotTurnOn[2];
extern short jetBumperTurnOn[8];
extern int listMax;
extern char dangerFlag, tiltFlag, dangerCount, dangerWait, flipperMask;
extern char magnaLockEnable, ballHitSfxEnable;
