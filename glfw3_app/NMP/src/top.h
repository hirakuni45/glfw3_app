#pragma once
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
