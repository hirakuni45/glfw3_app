#pragma once

#include <stdint.h>

void EntryLogo(void);

void EntryTitle(void);
void EntryMainMenu(void);
void EntryGameMenu(void);

void MakeGameInput(int i);
void ServiceRealmsMode(int);
void TurnIntoDark(int);
int MobjScore(int, int, const char *);
void MobjBall(int x, int y, int size, int maskno, int no);
int MobjFont(int, int, int);
void MobjString(int, int, const char *);
void MobjStringC(int, int, const char *);
void MobjNumber(int x, int y, int no);

void ResetReportMessage(void);
