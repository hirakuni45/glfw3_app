#pragma once
//=========================================================================//
/*! @file
	@brief  pinball general for Necronomicon
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>
#include "ballsim.h"

int ChooseMysteryItem(void);
void CreatePowerSelectList(PLAYER *, int);
void ServiceMysteryItem(PLAYER *ply, int no);
void TakeMystery(PLAYER *ply, int);
void ResetScore(char *dst);
void PresetScore(char *dst, char *src);
int AddScore(char *dst, char *src);
int SubScore(char *dst, char *src);
int CmpScore(char *dst, char *src);
unsigned int ScoreToInterger(char *src);
void IntergerToScore(char *dst, unsigned int src);
void StartMusic(int);
void SetupSoundGroup(int no);
void InitGeneralRule(int);
void InitConstScore(void);

int ServiceGeneralRule(void);

void ScanNextRound(PLAYER *ply);

int ServiceWisdom(PLAYER *ply);

void LaunchBallSaver(PLAYER *ply, int);
void AddBallSaver(PLAYER *ply);
void ServiceExitMulti(PLAYER *ply, int exn);
void ServiceExitRound(PLAYER *ply, int exn);
void RoundClose(PLAYER *ply, int);

void InitLightServer(void);
void ServiceLightServer(void);

void ResetDropTarget(PLAYER *ply);
void SetRoundTimer(PLAYER *ply, int);
void ServiceMultiScore(PLAYER *ply, int s, int d);

void ServiceLoopBonus(PLAYER *ply);
void ServiceSoul(PLAYER *, int);
void ServiceRoundEnd(PLAYER *ply);

void LaunchRound(PLAYER *ply);
void ServiceMysteryRound(PLAYER *ply);
void ServiceStatus(void);

void ServicePlayerInfo(PLAYER *);
void ServicePlayerSelector(void);

extern short trialTimer;
extern short soulTask;
extern short diagInfoTab[4];
extern short roundTimer, roundStartDelay, roundDelay;
extern char tiltGo;
extern char gameMode;
extern char nextStageEnable;
extern char roundValue[12];
extern char *scoreConst[50];

extern char lightCommand[50];
extern short *lightPosPtr;
extern const uint8_t *lightAnimePtr;

extern const uint8_t liteBLKALL[];
extern const uint8_t liteWBT[];
extern const uint8_t liteWBT2[];
extern const uint8_t liteWBT3[];
extern const uint8_t liteWTB[];
extern const uint8_t liteWTB2[];
extern const uint8_t liteTCW[];
extern const uint8_t liteTCC[];
extern const uint8_t liteBLK[];
extern const uint8_t liteBLK2[];
extern const uint8_t liteBLK3[];
extern const uint8_t liteBLK4[];
extern const uint8_t liteBLK8[];
extern const uint8_t liteBLK16[];
extern const uint8_t liteLTA[];
extern const uint8_t liteWBT_BLK_WBT_BLK[];
extern const uint8_t liteWBT_WTB_TCW_TCC[];
extern const uint8_t liteTCW_TCC[];
extern const uint8_t liteTCW_TCC_WBT[];
extern const uint8_t liteTCW_TCC_WBT_WTB[];
extern const uint8_t liteTCW_TCC_WBT_WBT[];
extern const uint8_t liteNightMare[];
