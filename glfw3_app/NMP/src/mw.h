#pragma once

#include <stdint.h>

void MxBaseInit(void);
void MountKanjiFont(void);
void MxInit(void);
void MxRefreshWindow(void);
void ServiceMouse(void);
void ServiceScreenSaver(void);

void DrawImageForWindow(WINDOW *, char *, int, int);
