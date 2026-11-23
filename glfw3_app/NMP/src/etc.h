#pragma once

#include <stdint.h>

void EntrySoundTester(void);
void EntryInfoTester(void);
void InitNECRONOMICON(void);
void EntryOptions(void);
void MainOptions(void);

void ServiceBrightClut(int top, int cnt);
void EntryManual(void);
void MainManual(void);
void InitPlayingManual(int);
void MainPlayingManual(void);

void EntryStaffRoll(void);

void FreezeScoreCode(uint32_t *dst, const char *src);
void ExpandScoreCode(char *dst, const int *src);
