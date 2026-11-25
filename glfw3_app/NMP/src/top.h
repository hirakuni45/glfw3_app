#pragma once
//=========================================================================//
/*! @file
	@brief  SEGA SATURN ハード依存関係・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>
#include "saturn.h"

void SetupScreenStyle(int type);
void ClearBackGroundScreen(void);

void ToBlackScreen(void);
void ClearCLUT(void);
void SetCLUT(int, int, int, int);
void SetCLUTi(int, int);
void SetCLUTs(int, int, const uint8_t *src);
void BrightColorTables(int top, int end, int lvl);

void SyncBlanking(void);

void GetDateTime(DateTime *);
void SetCommandSMPC(int);
int TakePeriData(unsigned char *, int, unsigned char *);

int TakeDirection(void);
void PresetBGposition(int, int);

void InstallSlaveFunction(void (*)(void));

void LaunchSound(const char *, const char *);
void SetSoundGroup(const char *, int);
void RingSfx(int no);

void ServiceCloseInt(void);
void SetSpeedLevel(int x, int c);

void EntryVsync(uint32_t padbits);
void ExitVsync(void);

#define CLUT_MAX	512

extern volatile uint32_t clutTransBit;
extern volatile uint32_t clutTransMask;
extern volatile unsigned short padLevel[2], padTriggerP[2], padTriggerN[2], padStatus[2];
extern volatile unsigned short padOnlyLevel[2], padOnlyTriggerP[2], padOnlyTriggerN[2];
extern volatile unsigned short mouseLevel, mouseTriggerP, mouseTriggerN;
extern volatile unsigned short analogEna[2], analogX[2], analogY[2], analogL[2], analogR[2];
extern volatile short mouseDx, mouseDy, mouseBl, mouseEna;
extern volatile short mouseX, mouseY, mouseB, mouseT;
extern volatile uint32_t vsyncCounter;
extern volatile char slaveSync;

extern char soundDrive;
extern char soundSeq;
// short mxFlag;
extern short tvFrame;

extern uint32_t clutBuff[CLUT_MAX];
