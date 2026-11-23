#pragma once
#include "ballsim.h"

int PrintAverage(char *, int n);
void InitVibration(void);
void ServiceVibration(void);
void InitBallSimulator(FIELD *, short *, char *);
void MainBallSimulator(void);
void BallSimulatorMotionObjects(void);
void Perspective(int *xyz, int *xys);
void AutoFlipper(void);
void LocationBallPosition(void);
int AddPlayerBall(void);
int AddPsychoSave(void);
int AddLoopShooter(void);

int MoveTestBall(int d);

int ScanVUK(void);