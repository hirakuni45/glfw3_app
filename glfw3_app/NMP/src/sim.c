//=========================================================================//
/*! @file
	@brief  コリジョン関係 Necronomicon
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdio.h>
#include "ballsim.h"
#include "saturn.h"
#include "sound.h"
#include "option.h"
#include "top.h"
#include "fnc.h"
#include "genr.h"
#include "vect.h"
#include "pinball.h"
#include "flags.h"
#include "etc.h"
#include "sim.h"

#define MODE_NORMAL		0
#define MODE_RAMP_EXIT		1
#define MODE_RAMP_ENTER		2
#define MODE_FALL		3
#define MODE_HOLE		4
#define MODE_VUK		5
#define MODE_OUT_HOLE		6
#define MODE_BALL_READY		7
#define MODE_SHOOT_READY	8
#define MODE_LOOP_SHOOTER	9
#define MODE_SAUCER		10
#define MODE_BALL_HOLD		11

#define MOVE_TEST_START_SPD	3000
#define	MOVE_TEST_LIMIT_SPD	16000
#define	MOVE_TEST_DELTA_SPD	20

typedef struct {
	int xt, yt;
	int xb, yb;
	int sin, cos, svs;
	int vp;
} MYXY;

static void SetupOtherBallHit(int);

static void Registance(int *, int);
static void LimitValue(int *, int);
static void ScanHit(BALL *);
static void AttributeReactions(BALL *, int);
static int CrossGateCheck(MYXY *a, HIT *b);
static int HitCheckLineToLine(MYXY *, HIT *);
static int HitCheckLineToCircle(MYXY *, HIT *);
static int ScanHitMap(BALL *, MYXY *);
static void ScanSwitch(BALL *);
static int ScanZoneArea(int, int, short *);
static void ServiceSwitch(BALL *);
static void ServiceMode(BALL *);
static int SumDelayVUK(BALL *);
static void ServiceMagnaLock(BALL *);

static int persL, persH, persV, persSin, persCos, persTan;
static int persOfsY, persScreen, persWidth, persBall;
static int vrmaxX, vrmaxY;
static int ball2Radius, ball2RadiusSquare;
static int magnaDx, magnaDy, magnaSq;
static int gravityTable, gravityRamp, gravityFall;
static int vibrPosX, vibrPosY;
static int tiltX[4], tiltY[4];
static short analogXold, analogYold;
static unsigned int swNeg, swPos;
static HIT ballHit[8];
static int b2bSpdX[8], b2bSpdY[8];
static const int boundPara[] = {
	METAL_HIT_FACTOR,
	PLASTIC_HIT_FACTOR,
	RUBBER_BAND_HIT_FACTOR,
	RUBBER_POST_HIT_FACTOR,
	RAMP_HIT_FACTOR,
	HOLE_GUIDE_HIT_FACTOR,
	FLIPPER_HIT_FACTOR,
	METAL_HIT_FACTOR };
static short ballHitCnt;
static short demoBallDelay;
static short flipperOnTime[2];
static short list[BOUND_LIST_MAX];
static unsigned short vibrSwLeft, vibrSwRight;
static int shooterSpd, kickbackSpd, vukSpd, slingshotSpd, jetbumperSpd;
static int shooterRans, kickbackRans, vukRans, slingshotRans, jetbumperRans;
static int jetbumperTrgSpd, slingshotTrgSpd, droptargetTrgSpd, standupTrgSpd;
static int loopSpd, loopRans;
static char magnaZone;
static FIELD *prp;

short totalPlays;
short dropTargetSw[8], dropTargetTotal[8];
short standUpSw[16], standUpTotal[16];
short holeSw[8], holeTotal[8];
short rampSw[4], rampTotal[4];
short jetBumperSw, jetBumperTotal;
short slingShotSw, slingShotTotal;
short topLaneSw, topLaneTotal;
short returnLaneSw, returnLaneTotal;
unsigned int playTime, playTimeTotal;
short exitBallCount;

PLAYER playerTab[PLAYER_MAX];
short shoottingCounter;
unsigned short shooterSw;
int frameRate;
int screenWidth, screenHeight;
short slingShotTurnOn[2];
short jetBumperTurnOn[8];
int listMax = 0;
char dangerFlag, tiltFlag, dangerCount, dangerWait, flipperMask;
char magnaLockEnable, ballHitSfxEnable;

/*----------------------------------------------/
/						/
/	Debug Monitor				/
/						/
/----------------------------------------------*/
void ResetReportMessage(void)
{
	int	i;

	for(i=0; i<8; ++i) dropTargetSw[i] = 0;
	for(i=0; i<16; ++i) standUpSw[i] = 0;
	for(i=0; i<8; ++i) holeSw[i] = 0;
	for(i=0; i<4; ++i) rampSw[i] = 0;
	jetBumperSw = 0;
	slingShotSw = 0;
	topLaneSw = 0;
	returnLaneSw = 0;

	playTime = 0;
	exitBallCount = 0;
}
/*
void CreateReportMessage(char *text)
{
	int	i, j;

	text += sprintf(text, "Play Counts = %d\n", totalPlays);

	if(totalPlays==0) return;

	text += sprintf(text, "\n");
	text += sprintf(text, "Play Time = %d, Average = %d [sec]\n",
		playTime / frameRate, playTimeTotal / frameRate / totalPlays);

	text += sprintf(text, "\n");
	if(jetBumperMax) {
	text += sprintf(text, "Jet Bumper = %d", jetBumperSw);
	text += PrintAverage(text, jetBumperTotal); }
	if(slingShotMax) {
	text += sprintf(text, "Sling Shot = %d", slingShotSw);
	text += PrintAverage(text, slingShotTotal); }

	j = 0;
	i = dropTargetMax;
	while(i) {
	text += sprintf(text, "Drop Target #%d = %d", j+1, dropTargetSw[j]);
	text += PrintAverage(text, dropTargetTotal[j]);
		--i; ++j; }
	j = 0;
	i = standUpMax;
	while(i) {
	text += sprintf(text, "Stand Up #%d = %d", j+1, standUpSw[j]);
	text += PrintAverage(text, standUpTotal[j]);
		--i; ++j; }
	j = 0;
	i = holeMax;
	while(i) {
	text += sprintf(text, "Hole #%d = %d", j+1, holeSw[j]);
	text += PrintAverage(text, holeTotal[j]);
		--i; ++j; }
	j = 0;
	i = rampMax;
	while(i) {
	text += sprintf(text, "Ramp #%d = %d", j+1, rampSw[j]);
	text += PrintAverage(text, rampTotal[j]);
		--i; ++j; }

	text += sprintf(text, "\n");
	text += sprintf(text, "Out Of Field = %d\n", exitBallCount);

	text += sprintf(text, "\n[EOF]\n");
	*text = 0;
}
*/
/*
int PrintAverage(char *text, int n)
{
	int	i, j;

	i = n / totalPlays;
	j = (n * 10 / totalPlays) % 10;
	return sprintf(text, ", Average = %d.%d\n", i, j);
}
*/
/*----------------------------------------------/
/						/
/	Initialization Ball Simulator		/
/						/
/----------------------------------------------*/
void InitBallSimulator(FIELD *para, const short *vect, int vnum, const char *msg)
{
	unsigned int u, v;

	prp = para;

	frameRate = para->frameRate;
	screenWidth = para->screenWidth;
	screenHeight = para->screenHeight;

	int i = 63956 * 16 / para->fieldHeight;
	int j = 18;		/* [%] */
	shooterSpd = para->shooterSpd * i / frameRate;
	shooterRans = shooterSpd / j;
	kickbackSpd = para->kickbackSpd * i / frameRate;
	kickbackRans = kickbackSpd / j;
	vukSpd = para->vukSpd * i / frameRate;
	vukRans = vukSpd / j;
	loopSpd = para->loopSpd * i / frameRate;
	loopRans = loopSpd / j;
	slingshotSpd = para->slingshotSpd * i / frameRate;
	slingshotRans = slingshotSpd / j;
	jetbumperSpd = para->jetbumperSpd * i / frameRate;
	jetbumperRans = jetbumperSpd / j;

	jetbumperTrgSpd = para->jetbumperTrgSpd * i / frameRate;
	slingshotTrgSpd = para->slingshotTrgSpd * i / frameRate;
	droptargetTrgSpd = para->droptargetTrgSpd * i / frameRate;
	standupTrgSpd = para->standupTrgSpd * i / frameRate;

	PickUpFirst(vect, vnum);

	ball2Radius = ballRadius * 2;
	ball2RadiusSquare = ball2Radius * ball2Radius;

	vrmaxX = para->fieldWidth / 2 * 63956 / para->fieldHeight;
	vrmaxY = 63956 / 2;

	persV = para->camZ * 63956 / para->fieldHeight;
	persH = (para->refY - para->camY) * 63956 / para->fieldHeight;
	u = (unsigned int)persV >> 1;
	v = (unsigned int)persH >> 1;
	persL = Isqrt(u * u + v * v) << 1;
	persSin = (persV << SINCOS_SHIFT) / persL;
	persCos = (persH << SINCOS_SHIFT) / persL;
	persTan = (persV << SINCOS_SHIFT) / persH;
	persOfsY = (para->fieldHeight / 2 - para->refY) * 63956 / para->fieldHeight;
	persScreen = ((para->screenHeight / 2) << 18) / para->tanHeight;
	persWidth = ((para->screenWidth / 2) << 18) / para->tanWidth;
	persBall = ball2Radius * persWidth;

	MakeupVectors(para, vect, vnum, msg);

	gravityTable = 240;	/* 220, 200(new), 240, 270, 325 */
	gravityRamp = 510;	/* 480, 580, 530(new), 810, 910, 1441 */
	gravityFall = 1500;	/* 1600(new) */

	BALL *blp = &ball[0];
	for(int i = 0; i < ballMax; ++i) {
		blp->mode = MODE_BALL_READY;
		blp->shooterFlag = 0;
		blp->vukDelay = 0;
		blp->gravity = gravityTable;
		blp->layer = _LAYER_0;
		blp->xb = blp->xp = startBallX << POSITION_SHIFT;
		blp->yb = blp->yp = startBallY << POSITION_SHIFT;
		blp->zb = blp->zp = startBallZ << POSITION_SHIFT;
		blp->xs = blp->ys = blp->zs = 0;
		blp->xm = blp->ym = blp->zm = 0;
		blp->psychoDelay = 0;
		blp->holeNo = 0;
		blp->vl = 0;
		blp->test = 0;
		blp++;
	}
	ballHitCnt = 0;

	slingShotTurnOn[0] = slingShotTurnOn[1] = 0;
	shoottingCounter = 0;
	exitBallCount = 0;
	swNeg = swPos = 0;
	demoBallDelay = frameRate * 3;

	for(int i = 0; i < FLIPPER_MAX; ++i) {
		if(flipper[i].hl > 0) {
			flipper[i].new = flipper[i].old = 0;	
			flipper[i].spd = 0;
			flipper[i].an = (i << 4);
			flipper[i].sound = 0;
		}
	}

	flipperMask = 1;
	flipperOnTime[0] = flipperOnTime[1] = 0;

	magnaLockEnable = 0;
	ballHitSfxEnable = 1;
}

/*----------------------------------------------/
/						/
/	Vibration (TILT / VIBRATE / Etc.)	/
/	ピンボール台の揺れに関する		/
/	シュミレーション・モデル		/
/						/
/----------------------------------------------*/
void InitVibration(void)
{
	int	i;

	for(i=0; i<4; ++i) tiltX[i] = tiltY[i] = 0;
	vibrPosX = vibrPosY = 0;
	dangerWait = 0;
	dangerCount = 0;
	dangerFlag = 0;
	tiltFlag = 0;
}

void ServiceVibration(void)
{
	int	i, j, k, l;

	l = k = 0;
	if(gameAnalogEna) {
	i = analogYold - gameAnalogY;
	if(i > 20 || i < -20) k -= i * 600;
	i = analogXold - gameAnalogX;
	if(i > 20 || i < -20) l -= i * 450;
	analogXold = gameAnalogX;
	analogYold = gameAnalogY;
	}
	else if(vibrSwLeft==vibrSwRight && vibrSwLeft==SATURN_DIR_ALL) {
	if(gamePadTrigger & SATURN_DIR_LEFT) l = (210 << 8);
	if(gamePadTrigger & SATURN_DIR_RIGHT) l = -(210 << 8);
	if((gamePadLevel & SATURN_DIR_UP)!=0 && l!=0) k = 445 << 8;
	}
	else {
	if(gamePadTrigger & vibrSwLeft) { k = 445 << 8; l = 210 << 8; }
	if(gamePadTrigger & vibrSwRight) { k = 445 << 8; l = -(210 << 8); }
		}

	if(k) {
		i = tiltY[0]; j = tiltX[0];
		tiltY[0] += k;
		if((tiltY[0] ^ i) < 0) i = -i;
		if(i!=0 && j!=0) tiltX[0] = j * tiltY[0] / i;

		i = tiltY[2]; j = tiltX[2];
		tiltY[2] += k;
		if((tiltY[2] ^ i) < 0) i = -i;
		if(i!=0 && j!=0) tiltX[2] = j * tiltY[2] / i; }
	if(l) {
		i = tiltY[1]; j = tiltX[1];
		tiltY[1] += l;
		if((tiltY[1] ^ i) < 0) i = -i;
		if(i!=0 && j!=0) tiltX[1] = j * tiltY[1] / i;

		i = tiltY[3]; j = tiltX[3];
		tiltY[3] += l;
		if((tiltY[3] ^ i) < 0) i = -i;
		if(i!=0 && j!=0) tiltX[3] = j * tiltY[3] / i; }

/*	振幅の減衰率	*/
	j = 28;
/*	振動周波数	*/
	k = 4;
	for(i=0; i<4; ++i)
	{
	if(i==2) { j += j; k += k << 2; }
	tiltX[i] = tiltX[i] - tiltY[i] / k;
	tiltY[i] = tiltY[i] + tiltX[i] / k;
	l = tiltX[i] / j;
	if(l>j || l<(-j)) tiltX[i] -= l; else tiltX[i] = tiltY[i] = 0;
	l = tiltY[i] / j;
	if(l>j || l<(-j)) tiltY[i] -= l; else tiltX[i] = tiltY[i] = 0;
	}

	i = tiltY[3] >> 16;
	j = tiltY[2] >> 16;
	if(i > 7 || j > 7 || i < -7 || j < -7) tiltFlag = 1;
	if(tiltFlag==0 && tiltGo==0)
	{
	if(i > 4 || j > 4 || i < -4 || j < -4) { dangerFlag = 1; dangerWait = 100; }
	}
	vibrPosX = tiltY[1];
	vibrPosY = tiltY[0];
	vibrX = vibrPosX >> 16;
	vibrY = vibrPosY >> 16;
/*	MobjBall(tiltY[1] >> 12, tiltY[0] >> 12, 16, 3); */
/*	MobjBall(0, 0, 16, 3); */

	if(dangerWait) {
	dangerWait--;
	if(dangerWait==0) dangerCount++;
	if(dangerCount >= 2 && dangerFlag!=0) { dangerFlag = 0; tiltFlag = 1; }
	}
}

int AddPlayerBall(void)
{
	int	i;
	BALL	*blp;

	if(shoottingCounter) return 0;

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_SHOOT_READY) return 0;
	blp++;
	}

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_BALL_READY)
		{
		RingSfx(SFX_BALL_LOAD);
		blp->vukDelay = 0;
		blp->mode = MODE_SHOOT_READY;
		blp->layer = _LAYER_0;
		blp->xp = blp->xb = startBallX << POSITION_SHIFT;
		blp->yp = blp->yb = startBallY << POSITION_SHIFT;
		blp->zp = blp->zb = startBallZ << POSITION_SHIFT;
		blp->xm = blp->xs = 11000;
		blp->ym = blp->ys = 0;
		blp->zm = blp->zs = 0;
		shoottingCounter = -1;
		totalPlays++;
		return 1;
		}
	blp++;
	}
	return 0;
}

int AddPsychoSave(void)
{
	int	i, j;
	BALL	*blp;

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_BALL_READY)
		{
		blp->vukDelay = 0;
		blp->mode = MODE_NORMAL;
		blp->layer = _LAYER_0;
		blp->xp = blp->xb = psychoSaveX << POSITION_SHIFT;
		blp->yp = blp->yb = psychoSaveY << POSITION_SHIFT;
		blp->zp = blp->zb = psychoSaveZ << POSITION_SHIFT;
		blp->xm = blp->xs = 0;
		blp->ym = blp->ys = 14000;
		blp->zm = blp->zs = 0;
		blp->psychoDelay = frameRate;
		return 1;
		}
	blp++;
	}
	return 0;
}

int AddLoopShooter(void)
{
	int	i;
	BALL	*blp;
	SWITCH	*sw;

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_LOOP_SHOOTER) return 0;
	blp++;
	}

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_BALL_READY)
		{
		RingSfx(SFX_PLUNGER_SHOT);
		blp->vukDelay = 0;
		blp->mode = MODE_LOOP_SHOOTER;
		blp->layer = _LAYER_1;
		blp->xp = blp->xb = vukPosX[1] << POSITION_SHIFT;
		blp->yp = blp->yb = vukPosY[1] << POSITION_SHIFT;
/*		sw = &swPara[SW_RAMP1_ENTER];
		blp->zp = blp->zb = (sw->zw << POSITION_SHIFT); */
		blp->zp = blp->yb = vukPosZ[1] << POSITION_SHIFT;
		blp->xm = blp->xs = 0;
/* blp->ys = -(loopSpd + (Rand() % loopRans) - (loopRans >> 1)); */
		blp->ym = blp->ys = -62000;
		blp->zm = blp->zs = 0;
		return 1;
		}
	blp++;
	}
	return 0;
}

int MoveTestBall(int d)
{
	int	i, f;
	BALL	*blp;

	blp = &ball[0];
	f = 0;
	for(i=0; i<ballMax; ++i)
	{
	if(blp->test) f++;
	if(f) break;
	blp++;
	}

	if(f)
	{
	if(d & SATURN_PAD_Y)
		{
		blp->ys = 0;
		i = blp->test;
		if(d & SATURN_DIR_UP) blp->ys = i;
		if(d & SATURN_DIR_DOWN) blp->ys = -i;
		blp->xs = 0;
		if(d & SATURN_DIR_LEFT) blp->xs = -i;
		if(d & SATURN_DIR_RIGHT) blp->xs = i;
		if(d & SATURN_DIR_ALL) blp->test += MOVE_TEST_DELTA_SPD;
		else blp->test = MOVE_TEST_START_SPD;
		if(blp->test >= MOVE_TEST_LIMIT_SPD) blp->test = MOVE_TEST_LIMIT_SPD;
		}
	else blp->test = 0;
	}
	else if(d & SATURN_PAD_Y)
	{
	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_BALL_READY)
		{
		blp->vukDelay = 0;
		blp->mode = MODE_NORMAL;
		blp->xp = blp->xb = 0;
		blp->yp = blp->yb = 0;
		blp->zp = blp->zb = 0;
		blp->xm = blp->xs = 0;
		blp->ym = blp->ys = 0;
		blp->zm = blp->zs = 0;
		blp->test = MOVE_TEST_START_SPD;
		return 1;
		}
	blp++;
	}
	}
	return 0;
}

/*----------------------------------------------/
/						/
/	Main Task Ball Simulator		/
/						/
/----------------------------------------------*/
void MainBallSimulator(void)
{
	BALL	*blp;
	PLAYER	*ply;
	int	i, j, k, lvl, ptr, ntr;
	int	x, y, z, xx, yy, zz;
	int	ll, l, aa, bb, bs;
	int	xyz[3], xys[3], xss[8], yss[8];
	int	mm, kx;

	if(gameAnalogEna) {
	flipper[0].sw = flipper[2].sw = SATURN_PAD_LEFT | SATURN_DIR_ALL;
	flipper[3].sw = flipper[1].sw = SATURN_PAD_RIGHT | SATURN_PAD_ABCXYZ;
	shooterSw = SATURN_PAD_ABCXYZ;
	vibrSwLeft = 0;
	vibrSwRight = 0;
	}
	else if(optionMenuPad[OPTION_NO_CONTROLLER]==1) {
	flipper[0].sw = flipper[2].sw = SATURN_PAD_LEFT;
	flipper[3].sw = flipper[1].sw = SATURN_PAD_RIGHT;
	shooterSw = SATURN_PAD_RIGHT;
	vibrSwLeft = SATURN_DIR_ALL;
	vibrSwRight = SATURN_PAD_ABCXYZ;
	}
	else if(optionMenuPad[OPTION_NO_CONTROLLER]==2) {
	flipper[0].sw = flipper[2].sw = SATURN_PAD_LEFT;
	flipper[3].sw = flipper[1].sw = SATURN_PAD_RIGHT | SATURN_PAD_ABCXYZ;
	shooterSw = SATURN_PAD_ABCXYZ;
	vibrSwLeft = SATURN_DIR_ALL;
	vibrSwRight = SATURN_DIR_ALL;
	}
	else {
	flipper[0].sw = flipper[2].sw = SATURN_DIR_ALL;
	flipper[3].sw = flipper[1].sw = SATURN_PAD_ABC | SATURN_PAD_XYZ;
	shooterSw = SATURN_PAD_ABCXYZ;
	vibrSwLeft = SATURN_PAD_LEFT;
	vibrSwRight = SATURN_PAD_RIGHT;
	}

	playTime++;
	playTimeTotal++;

	ply = &playerTab[playerNo];

	if(flipperMask!=0 || tiltGo!=0) ntr = ptr = lvl = 0;
	else	{
		lvl = gamePadLevel;
		ptr = gamePadTrigger;
		ntr = gamePadRelese;
		}

	for(i=0; i<2; ++i)
	{
	if(flipperOnTime[i])
		{ if(flipperOnTime[i]==DEMO_FLIPPER_TIME) ptr = flipper[i].sw;
		flipperOnTime[i]--;
		lvl = flipper[i].sw; }
	}

	j = 0;
	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_BALL_READY) ;
	else if(blp->mode==MODE_OUT_HOLE) ;
	else j++;
	blp++;
	}
	ply->multiBalls = j;

/*----------------------------------------------/
/	Service Flippers.			/
/	Rendering Flipper Motion Object		/
/----------------------------------------------*/
	for(i=0; i<FLIPPER_MAX; i++)
	{
	if(flipper[i].hl > 0) {
		if(lvl & flipper[i].sw) flipper[i].spd = FLIPPER_SPEED_FRD;
		else	{
			flipper[i].spd = FLIPPER_SPEED_BWD; flipper[i].sound = 0;
			}
		if((ptr & flipper[i].sw)!=0 && flipper[i].sound==0) {
			RingSfx(SFX_FLIPPER);
			flipper[i].sound = 1;
			}
		if((ntr & flipper[i].sw)) {
			RingSfx(SFX_FLIPPER_DOWN);
			}
		j = flipper[i].new;
		flipper[i].old = j;
		j += flipper[i].spd;
		k = j >> 10;
		if(k >= FLIPPER_STEP) j = (FLIPPER_STEP << 10) - 1; 
		else if(k < 0) j = 0;
		flipper[i].new = j;
		flipper[i].an = (j >> 10) | (i << 4);
		}
	}

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	b2bSpdX[i] = b2bSpdY[i] = 0;
/* 0.985:4034 1ST */
/* 0.987:4042 2ND */
/* 0.988:4047 3RD */
/* 0.990:4055 4TH */
	blp->xs = (blp->xs * 4055) >> 12;
	blp->ys = (blp->ys * 4055) >> 12;
	if(blp->mode == MODE_LOOP_SHOOTER) l = 5;
	else if(blp->layer & 0xc0) l = 18; else l = 10;
	Registance(&blp->xs, l);
	Registance(&blp->ys, l);
	Registance(&blp->zs, 30);
	LimitValue(&blp->xs, SPEED_MAX_X);
	LimitValue(&blp->ys, SPEED_MAX_Y);
	LimitValue(&blp->zs, SPEED_MAX_Z);
	LimitValue(&blp->xm, SPEED_MAX_X);
	LimitValue(&blp->ym, SPEED_MAX_Y);
	blp++;
	}

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	SetupOtherBallHit(i);
	blp->vl = 0;
	blp->xm = blp->xs;
	blp->ym = blp->ys;
	blp->svs = 0;
	if(blp->mode==MODE_BALL_READY) l = 2; else l = DIF_COUNT_LOOP;
	for(j=0; j<l; j++)
		{
		xx = blp->xm >> 1;
		yy = blp->ym >> 1;
		aa = xx * xx;
		bb = yy * yy;
		aa = Isqrt(aa + bb);
		blp->vl += aa;
		if(aa < DIF_BREAK_VALUE) break;
		blp->cos = (xx << SINCOS_SHIFT) / aa;
		blp->sin = (yy << SINCOS_SHIFT) / aa; 
		blp->svs = aa >> (POSITION_SHIFT - 1);
		ScanHit(blp);
		ServiceSwitch(blp);
		}
	ServiceMagnaLock(blp);
	ServiceMode(blp);
	blp++;
	}

	if(shoottingCounter > 0) shoottingCounter--;

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	blp->xs += b2bSpdX[i];
	blp->ys += b2bSpdY[i];
	LimitValue(&b2bSpdX[i], SPEED_MAX_X);
	LimitValue(&b2bSpdY[i], SPEED_MAX_Y);

/*	そっと戻す	*/
	if(blp->yp >= (vrmaxY << POSITION_SHIFT) || blp->yp < (-vrmaxY << POSITION_SHIFT) ||
		blp->xp < (-vrmaxX << POSITION_SHIFT) || blp->xp > (vrmaxX << POSITION_SHIFT))
		{ 
		exitBallCount++;
		blp->mode = MODE_BALL_READY;
		blp->shooterFlag = 0;
		blp->vukDelay = 0;
		blp->gravity = gravityTable;
		blp->layer = _LAYER_0;
		blp->xb = blp->xp = startBallX << POSITION_SHIFT;
		blp->yb = blp->yp = startBallY << POSITION_SHIFT;
		blp->zb = blp->zp = startBallZ << POSITION_SHIFT;
		blp->xs = blp->ys = blp->zs = 0;
		blp->xm = blp->ym = blp->zm = 0;
		blp->holeNo = 0;
		}
	if(blp->psychoDelay) {
		blp->psychoDelay--;
		if(blp->psychoDelay==0) {
			RingSfx(SFX_PSYCHO_SAVE);
			if(Rand() & 0x400) j = 3400; else j = -3400;
			blp->ym = blp->ys = 7500;
			blp->xs += j;
			}
		}
	blp++;
	}
}

static void SetupOtherBallHit(int my)
{
	int	i, j, k;
	HIT	*hit;
	BALL	*blp;

	ballHitCnt = 0;
	hit = &ballHit[0];
	i = ball[my].mode;
	if(i==MODE_NORMAL || i==MODE_RAMP_ENTER || i==MODE_SAUCER) {
	blp = &ball[0];
	for(i=0; i<ballMax; ++i) {
	k = blp->mode;
	j = ball[my].zp - blp->zp;
	if(j < 0) j = -j;
	if(i!=my && j < (ballRadius >> 8) &&
		(k==MODE_NORMAL || k==MODE_RAMP_ENTER || k==MODE_SAUCER)) {
			hit->atr = (OTHER_BALL | 0x8000) + i;
			hit->xt = blp->xp >> POSITION_SHIFT;
			hit->yt = blp->yp >> POSITION_SHIFT;
			hit->vs = ball2Radius;
			hit->vss = ball2RadiusSquare;
			hit->sin = hit->cos = 0;
			hit++;
			ballHitCnt++;
			}
		blp++;
		}
	}
}

/*----------------------------------------------/
/						/
/	Rendering Motion Object			/
/	A Drop Target				/
/	A Ball					/
/	A Flipper				/
/	A Score					/
/						/
/----------------------------------------------*/
void BallSimulatorMotionObjects(void)
{
	PLAYER *ply = &playerTab[playerNo];

	if(dropTargetMax) {
		for(int i = 0; i < dropTargetMax; ++i) {
			if(ply->dropTargetEnable[i]) {
				MobjDropTarget(prp->dtpX[i], prp->dtpY[i], i);
			}
		}
	}

	int sort[8];
	for(int i = 0; i < ballMax; ++i) sort[i] = i;
	for(int j = 0; j < (ballMax - 1); ++j) {
		int y = ball[j].yp;
		int z = ball[j].zp;
		for(int i = (j + 1); i < ballMax; ++i) {
			if(z > ball[i].zp || y < ball[i].yp) {
				int l = sort[j];
				sort[j] = sort[i];
				sort[i] = l;
				y = ball[i].yp;
				z = ball[i].zp;
			}
		}
	}

	for(int i = 0; i < 2; ++i) {
		if(slingShotTurnOn[i]) {
			slingShotTurnOn[i]--;
			MobjSlingShot(prp->slingShotPosX[i], prp->slingShotPosY[i], i);
		}
	}

	for(int i = 0; i < 3; ++i) {
		if(jetBumperTurnOn[i]) {
			jetBumperTurnOn[i]--;
			MobjJetBumper(prp->jetBumperPosX[i], prp->jetBumperPosY[i], i);
		}
	}

	int	xyz[3], xys[3];
	for(int i = 0; i < ballMax; ++i) {
		BALL *blp = &ball[sort[i]];
		xyz[0] = blp->xp >> POSITION_SHIFT;
		xyz[1] = blp->yp >> POSITION_SHIFT;
		xyz[2] = (blp->zp >> POSITION_SHIFT) + ballRadius;
		if(xyz[2] > (-ballRadius * 2) && blp->mode != MODE_BALL_READY) {
			Perspective(&xyz[0], &xys[0]);
			int mask = (blp->layer >> 6) & 3;
			MobjBall(xys[0], xys[1], xys[2], mask, i);
		}
	}

	MobjFlipper(prp->fpX[0], prp->fpY[0], flipper[0].an);
	MobjFlipper(prp->fpX[1], prp->fpY[1], flipper[1].an);
	if(flipper[2].hl > 0) MobjFlipper(prp->fpX[2], prp->fpY[2], flipper[2].an + 6);
	if(flipper[3].hl > 0) MobjFlipper(prp->fpX[3], prp->fpY[3], flipper[3].an + 6);

	ServicePlayerInfo(&playerTab[0]);
}

/*----------------------------------------------/
/						/
/	Auto Flipper / Ball Simulator		/
/						/
/----------------------------------------------*/
void AutoFlipper(void)
{
	int	i;
	PLAYER	*ply;

	ply = &playerTab[0];

	if(demoBallDelay) demoBallDelay--;
	if(demoBallDelay==0)
	{
	ply->shooterEnable = 1;
	AddPlayerBall();
	demoBallDelay = frameRate * 3;
	}

	if((ply->flipperOn[0] | ply->flipperOn[2])!=0 && flipperOnTime[0]==0)
		flipperOnTime[0] = DEMO_FLIPPER_TIME + (Rand() & 15) - 8;
	if((ply->flipperOn[1] | ply->flipperOn[3])!=0 && flipperOnTime[1]==0)
		flipperOnTime[1] = DEMO_FLIPPER_TIME + (Rand() & 15) - 8;
	for(i=0; i<4; ++i) ply->flipperOn[i] = 0;
}

/*------------------------------------------------------/
/	aa = yy * Sin(ConectAngle)			/
/	bb = yy * Cos(ConectAngle)			/
/	SL = (WindowHeight / 2) / Tan(FOV / 2)		/
/	py = (aa * SL) / (ll + bb)			/
/	px = (x * SL) / (ll + bb)			/
/	bs = (BallRadius * 2 * SL) / (ll + bb)		/
/------------------------------------------------------*/
void Perspective(const int *xyz, int *xys)
{
	int	x, y, z, zz, yy, aa, bb, ll;
	unsigned int u, v;

	x = *xyz++;
	y = *xyz++;
	z = *xyz;
	zz = z << SINCOS_SHIFT;
	yy = y + persOfsY + (zz / persTan);
	aa = (yy * persSin) >> (SINCOS_SHIFT - 1);
	bb = (yy * persCos) >> SINCOS_SHIFT;
	yy = aa * persScreen / (persL - (zz / persSin) + bb);
	if(yy & 1) *(xys + 1) = (yy >> 1) + 1; else *(xys + 1) = yy >> 1;
	u = (unsigned int)(persV - z);
	v = (unsigned int)(persH + y + persOfsY);
	u >>= 1;
	v >>= 1;
	ll = Isqrt(u * u + v * v);
	x = (x * persWidth / ll);
	if(x & 1) *xys = (x >> 1) + 1; else *xys = x >> 1;
	bb = persBall / ll;
	if(bb & 1) *(xys + 2) = (bb >> 1) + 1; else *(xys + 2) = bb >> 1;
}

static void Registance(int *in, int reg)
{
	int	s;

	s = *in;
	if(s == 0) return;
	if(s < 0) *in += reg;
	else *in -= reg;
	if((s ^ *in) < 0) *in = 0;
}

static void LimitValue(int *in, int cmp)
{
	int	a, f;

	if(*in < 0) { a = -( *in ); f = -1; } else { a = *in; f = 0; }
	if(a > cmp) a = cmp;
	if( f ) *in = -a; else *in = a;
}

static void ScanHit(BALL *blp)
{
	HIT	*hit;
	int	i, j, k, f, g, a, c, s, x, y, xx, yy, aa;
	int	sin, cos;
	MYXY	myxy;

	blp->xb = blp->xp + blp->xm;
	blp->yb = blp->yp + blp->ym;
	blp->zb = blp->zp;
	myxy.xt = blp->xp >> POSITION_SHIFT;
	myxy.yt = blp->yp >> POSITION_SHIFT;
	myxy.xb = blp->xb >> POSITION_SHIFT;
	myxy.yb = blp->yb >> POSITION_SHIFT;
	myxy.sin = blp->sin;
	myxy.cos = blp->cos;
	myxy.svs = blp->svs;
	if((f=ScanHitMap(blp, &myxy))>=0)
	{
	hit = blp->hit;
	xx = hit->xt << POSITION_SHIFT;
	yy = hit->yt << POSITION_SHIFT;
	sin = hit->sin;
	cos = hit->cos;
	a = blp->vp;
	blp->xb = xx + ((a * cos) >> (SINCOS_SHIFT-POSITION_SHIFT));
	blp->yb = yy + ((a * sin) >> (SINCOS_SHIFT-POSITION_SHIFT));

	if(hit->atr & 0x8000) g = CONECT_CIRCLE_GAP;
	else	{
		g = CONECT_LINE_GAP;
		sin =  hit->cos;
		cos = -hit->sin;
		}

	s = blp->xm;
	blp->xm -= blp->xb - blp->xp;
	if((blp->xm ^ s) < 0) blp->xm = 0;
	s = blp->ym;
	blp->ym -= blp->yb - blp->yp;
	if((blp->ym ^ s) < 0) blp->ym = 0;

/*	ギャップ（クリアランス）の補正、法線ベクトルのコピー	*/
	blp->xb += (cos * g) >> (SINCOS_SHIFT-POSITION_SHIFT);
	blp->yb += (sin * g) >> (SINCOS_SHIFT-POSITION_SHIFT);
	blp->sin1 = sin;
	blp->cos1 = cos;

	AttributeReactions(blp, f);
	}
	else
	{
	blp->xm = 0;
	blp->ym = 0;
	f = 3;
	}
	ScanSwitch(blp);

	i = blp->xp;
	j = blp->yp;
	k = blp->zp;
	blp->xp = blp->xb;
	blp->yp = blp->yb;
	blp->zp = blp->zb;
	blp->xb = i;
	blp->yb = j;
	blp->zb = k;
}

/*------------------------------------------------------/
/							/
/	Attribute Reaction				/
/							/
/------------------------------------------------------*/
static void AttributeReactions(BALL *blp, int f)
{
	int	bp, i, j, k, a, s;
	int	sin1, cos1, sin2, cos2, sx, sy, sin, cos;
	int	xx, yy, xs, ys, xm, ym, x, y, dx, dy;
	short	sico[2];
	PLAYER	*ply;

	sin1 = blp->sin1;
	cos1 = blp->cos1;
	a = blp->hit->atr;
	ply = &playerTab[playerNo];
	bp = boundPara[(a >> 12) & 7];
	xs = (blp->xs * cos1 + blp->ys * sin1) >> SINCOS_SHIFT;
	ys = (blp->ys * cos1 - blp->xs * sin1) >> SINCOS_SHIFT;
	xm = (blp->xm * cos1 + blp->ym * sin1) >> SINCOS_SHIFT;
	ym = (blp->ym * cos1 - blp->xm * sin1) >> SINCOS_SHIFT;

	sx = sy = xx = yy = 0;
	switch((a >> 8) & 15)
	{
	case OTHER_BALL >> 8:
		j = a & 7;
		if(xs < 0)
		{
		if(xs < -670)
			{
			if(ballHitSfxEnable) RingSfx(SFX_BALL_HIT);
			i = xs - (xs >> 3);
			b2bSpdX[j] += (i * cos1) >> SINCOS_SHIFT;
			b2bSpdY[j] += (i * sin1) >> SINCOS_SHIFT;
			}
		bp = 0;
		}
		break;
	case JET_BUMPER >> 8:
		if(jetbumperTrgSpd < -xm && tiltGo==0)
		{
		xx = jetbumperSpd + ((unsigned int)Rand() % jetbumperRans) - (jetbumperRans >> 1);
		bp = 0;
		jetBumperTurnOn[a & 7] = frameRate * 16 / 80;
		ply->switchTotal++;
		ply->switchFrame++;
		if(ply->jetHits) ply->jetHits--;
		ply->jetBumperOn[a & 7] = 1;
		jetBumperTotal++;
		jetBumperSw++;
		}
		break;
	case SLING_SHOT >> 8:
		blp->gravity = gravityTable;
		if(slingshotTrgSpd < -xm && tiltGo==0)
		{
		RingSfx(SFX_SLINGSHOT);
		slingShotTurnOn[a & 1] = frameRate * 16 / 80;
		ply->switchTotal++;
		ply->switchFrame++;
		if(ply->slingHits) ply->slingHits--;
		ply->slingShotOn[a & 1] = 1;
		i = blp->hit->vs >> 1;
		j = blp->vp - i;
		if(j < 0) j = -j;
		s = (j << 8) / i;
		s = 256 - s;
		s *= slingshotSpd + ((unsigned int)Rand() % slingshotRans) - (slingshotRans >> 1);
		s >>= 8;
		xx = s;
		bp = 0;
		slingShotTotal++;
		slingShotSw++;
		}
		break;
	case LR_FLIPPER >> 8:
		blp->gravity = gravityTable;
		i = (a >> 4) & 3;
		ply->flipperOn[i] = 1;
		j = flipper[i].new - flipper[i].old;
		if(j < 0) j >>= 2;
		if(j)
		{
		k = a & 15;
		k++;
		if(k >= FLIPPER_STEP) k = FLIPPER_STEP - 1;
		sin2 = flipper[i].sin1[k];
		cos2 = flipper[i].cos1[k];
		x = (blp->xb >> POSITION_SHIFT) - flipper[i].cx;
		y = (blp->yb >> POSITION_SHIFT) - flipper[i].cy;
		dx = (x * cos2 + y * sin2) >> SINCOS_SHIFT;
		dy = (y * cos2 - x * sin2) >> SINCOS_SHIFT;
		if((j ^ dx) < 0)
			{ bp = 300;
			xs -= xs >> 3;
			xm -= xm >> 3; }
		else	{ bp = 0;
/*		j *= 430; */
/*		j *= 380; */
/*		j *= 290; */
		j *= prp->flipperSpdFactor[i];
		j >>= 8;
		a = dy;
		if(dy < 0) dy = -dy;
		k = flipper[i].hl / 3;
		if(dy < k) { j *= dy; j /= k; }
		x = (j * cos1) >> SINCOS_SHIFT;
		y = (j * sin1) >> SINCOS_SHIFT;
		dx = (x * cos2 + y * sin2) >> SINCOS_SHIFT;
		dy = (y * cos2 - x * sin2) >> SINCOS_SHIFT;
			if(f<=2)
				{
/* if(f==1) gameDebugPause = 1; */
				y = flipper[i].sin1[3];
				x = flipper[i].cos1[3];
				if(a < flipper[i].hl)
					{
			/*	j = -2950;	*/
			/*	j = -2300;	*/
			k = (a * prp->flipperRengeFactor[i]) / flipper[i].hl;
			j = prp->flipperBlurFactor[i];
			k += ((unsigned int)Rand() % j) - (j >> 1); /* オフセット */
			k += prp->flipperOffsetFactor[i];
			SinCos(k, &sico[0]);
			cos2 = (x * sico[1] + y * sico[0]) >> SINCOS_SHIFT;
			sin2 = (y * sico[1] - x * sico[0]) >> SINCOS_SHIFT;
					}
				else { cos2 = cos1; sin2 = sin1; }
				bp = 0;
				}
			else	{ cos2 = cos1; sin2 = sin1; }
			sx = (cos2 * dx) >> (SINCOS_SHIFT-POSITION_SHIFT);
			sy = (sin2 * dx) >> (SINCOS_SHIFT-POSITION_SHIFT);
			}
		}
		break;
	case DROP_TARGET >> 8:
		if(droptargetTrgSpd < -xm)
		{
		RingSfx(SFX_DROP_TARGET);
		ply->switchTotal++;
		ply->switchFrame++;
		i = a & 7;
		ply->dropTargetFlag[i] = 1;
		ply->dropTargetOn[i] = 1;
		bp = 300;
		dropTargetTotal[i]++;
		dropTargetSw[i]++;
		}
		break;
	case STAND_UP >> 8:
		if(standupTrgSpd < -xm)
		{
		RingSfx(SFX_SPOT_TARGET);
		ply->switchTotal++;
		ply->switchFrame++;
		i = a & 15;
		ply->standUpOn[i] = 1;
		ply->standUpFlag[i] = 1;
		standUpTotal[i]++;
		standUpSw[i]++;
		}
		bp = 470;
		break;
	case HOLE_ZONE >> 8:
		RingSfx(SFX_BALL_LOCK);
		ply->switchTotal++;
		ply->switchFrame++;
		i = a & 7;
		blp->holeNo = i;
		blp->mode = MODE_HOLE;
		ply->holeOn[i] = 1;
		bp = 0;
		holeTotal[i]++;
		holeSw[i]++;
		break;
	case SAUCER_ZONE >> 8:
		if(blp->mode!=MODE_SAUCER) {
		RingSfx(SFX_BALL_LOCK);
		i = a & 3;
		blp->holeNo = i;
		blp->mode = MODE_SAUCER;
		blp->xs =blp->ys = blp->xm = blp->ym = 0;
		ply->saucerOn[i] = 1;
		ply->switchTotal++;
		ply->switchFrame++;
		bp = 0;
		}
		break;
	default:
		break;
	}
	if(xm < 0) xm = -(xm * bp) >> BOUND_SHIFT;
	if(xs < 0) xs = -(xs * bp) >> BOUND_SHIFT;
	xs += xx; ys += yy;
	xm += xx; ym += yy;

	if(blp->mode!=MODE_BALL_READY)
	{
	i = vibrPosX >> 8;
	j = vibrPosY >> 8;
	xx = (i * cos1 + j * sin1) >> SINCOS_SHIFT;
	yy = (j * cos1 - i * sin1) >> SINCOS_SHIFT;
	if(xx >= 0) { xs += xx; ys += yy; }
	}

	blp->xm = (xm * cos1 - ym * sin1) >> SINCOS_SHIFT;
	blp->ym = (ym * cos1 + xm * sin1) >> SINCOS_SHIFT;
	blp->xs = (xs * cos1 - ys * sin1) >> SINCOS_SHIFT;
	blp->ys = (ys * cos1 + xs * sin1) >> SINCOS_SHIFT;
	blp->xs += sx;
	blp->ys += sy;
	blp->xm += sx;
	blp->ym += sy;
}

/*------------------------------------------------------/
/							/
/	Scanning					/
/	Hit Map, Switch					/
/							/
/------------------------------------------------------*/
static int ScanHitMap(BALL *blp, MYXY *myxy)
{
	int	xyofs[8], *ofs;
	int	i, j, k, l, m, p;
	int	xt, yt, xb, yb;
	int	hl, sin, cos, lc;
	HIT	*hit;
	short	*ptr;
	PLAYER	*ply;

	ply = &playerTab[playerNo];

	xt = myxy->xt >> 12;
	yt = myxy->yt >> 12;
	xb = myxy->xb >> 12;
	yb = myxy->yb >> 12;
	if(xt < xb) i=1; else if(xt > xb) i=-1; else i=0;
	if(yt < yb) j=1; else if(yt > yb) j=-1; else j=0;
	xt += ZONE_DIVIDE >> 1;
	yt += ZONE_DIVIDE >> 1;
	xb += ZONE_DIVIDE >> 1;
	yb += ZONE_DIVIDE >> 1;

	ofs = &xyofs[0];
	*ofs++ = xt;	*ofs++ = yt; k = 1;
	if(i) { *ofs++ = xt+i;	*ofs++ = yt; k++; }
	if(j) { *ofs++ = xt;	*ofs++ = yt+j; k++; }
	if(i!=0 && j!=0) { *ofs++ = xt+i; *ofs++ = yt+j; k++; }

	lc = 0;
	ofs = &xyofs[0];
	for(i=0; i<k; ++i)
	{
	xt = *ofs++ & 15;
	yt = *ofs++ & 15;
	if((j = zoneMap[xt][yt])>=0)
		{
		ptr = &listTag[j];
		while((p = *ptr++)>=0)
			{
			hit = &hitList[p];
			j = hit->atr & ATTR_MASK;
			l = hit->atr & LAYER_MASK;
			if(l==blp->layer || l==_LAYER_ALL)
				{
				if(j==LR_FLIPPER)
					{
				j = hit->atr & 0x3f;
				if(j==flipper[0].an) list[lc++] = p;
				else if(j==flipper[1].an) list[lc++] = p;
				else if(flipper[2].hl > 0 && j==flipper[2].an) list[lc++] = p;
				else if(flipper[3].hl > 0 && j==flipper[3].an) list[lc++] = p;
				else {	m = flipper[0].an; if(m & 15) m--;
					if(j==m) list[lc++] = p;
					m = flipper[1].an; if(m & 15) m--;
					if(j==m) list[lc++] = p;
					m = flipper[2].an; if(m & 15) m--;
					if(flipper[2].hl > 0 && j==m) list[lc++] = p;
					m = flipper[3].an; if(m & 15) m--;
					if(flipper[3].hl > 0 && j==m) list[lc++] = p;
						}
					}
				else if(j==DROP_TARGET) {
				if(ply->dropTargetEnable[hit->atr & 7]) list[lc++] = p;
					}
				else if(j==MOVE_GATE) {
				if(ply->gateEnable[hit->atr & 15]) list[lc++] = p;
					}
				else if(j!=SWITCH_GATE) list[lc++] = p;
				}
			}
		}
	}

if(lc > listMax) listMax = lc;

	hl = 0x7fffffff;
	ptr = &list[0];
	while(lc)
	{
	hit = &hitList[ *ptr++ ];
	if(hit->atr & 0x8000) k = HitCheckLineToCircle(myxy, hit);
	else k = HitCheckLineToLine(myxy, hit);
	if(hl > k)
		{
		blp->hit = hit;
		hl = k;
		blp->vp = myxy->vp;
		}
	lc--;
	}

if(hl>=2) {
	hit = &ballHit[0];
	lc = ballHitCnt;
	while(lc)
	{
	k = HitCheckLineToCircle(myxy, hit);
	if(hl > k)
		{
		blp->hit = hit;
		hl = k;
		blp->vp = myxy->vp;
		}
	hit++;
	lc--;
	}
}
	if(hl < 0x7fffffff)
	{
	hit = blp->hit;
	if(hit->atr & 0x8000)
		{
		xt = hit->cos;
		yt = hit->sin;
		if(hl>=2){
			k = hit->vs;
			xt = (xt << SINCOS_SHIFT) / k;
			yt = (yt << SINCOS_SHIFT) / k;
			sin = blp->sin;
			cos = blp->cos;
			hit->cos = (xt * cos - yt * sin) >> SINCOS_SHIFT;
			hit->sin = (yt * cos + xt * sin) >> SINCOS_SHIFT;
			}
		else	{
			if(blp->vp==0) blp->vp=1;
			k = Isqrt(blp->vp);
			hit->cos = (xt << SINCOS_SHIFT) / k;
			hit->sin = (yt << SINCOS_SHIFT) / k;
			}
		blp->vp = hit->vs;
		return hl;
		}
	else return hl;
	}
	else return -1;
}

static void ScanSwitch(BALL *blp)
{
	int	xyofs[8], *ofs;
	int	i, j, k, l, m;
	int	xt, yt, xb, yb;
	HIT	*hit;
	MYXY	myxy;
	short	*ptr, p;

	myxy.xt = blp->xp >> POSITION_SHIFT;
	myxy.yt = blp->yp >> POSITION_SHIFT;
	myxy.xb = blp->xb >> POSITION_SHIFT;
	myxy.yb = blp->yb >> POSITION_SHIFT;
	xt = myxy.xt >> 12;
	yt = myxy.yt >> 12;
	xb = myxy.xb >> 12;
	yb = myxy.yb >> 12;
	if(xt < xb) i=1; else if(xt > xb) i=-1; else i=0;
	if(yt < yb) j=1; else if(yt > yb) j=-1; else j=0;
	xt += ZONE_DIVIDE >> 1;
	yt += ZONE_DIVIDE >> 1;
	xb += ZONE_DIVIDE >> 1;
	yb += ZONE_DIVIDE >> 1;

	ofs = &xyofs[0];
	*ofs++ = xt;	*ofs++ = yt; k = 1;
	if(i) { *ofs++ = xt+i;	*ofs++ = yt; k++; }
	if(j) { *ofs++ = xt;	*ofs++ = yt+j; k++; }
	if(i!=0 && j!=0) { *ofs++ = xt+i; *ofs++ = yt+j; k++; }

	ofs = &xyofs[0];
	for(i=0; i<k; ++i)
	{
	xt = *ofs++ & 15;
	yt = *ofs++ & 15;
	if((j = zoneMap[xt][yt])>=0)
		{
		ptr = &listTag[j];
		while((p = *ptr++)>=0)
			{
			hit = &hitList[p];
			j = hit->atr;
			l = j & LAYER_MASK;
			if(l==blp->layer || l==_LAYER_ALL)
				{
				if((j & ATTR_MASK)==SWITCH_GATE)
					{
					m = CrossGateCheck(&myxy, hit);
					l = 1 << (j & 0x1f);
					if(m > 0) swPos |= l;
					else if(m < 0) swNeg |= l;
					}
				}
			}
		}
	}
}

/*------------------------------------------------------/
/							/
/	A Cross Gate check.				/
/							/
/------------------------------------------------------*/
static int CrossGateCheck(MYXY *a, HIT *b)
{
	int	xt, xb, yt, yb, dx, tt;
	int	x0, x1, y0, y1, vs, m, sin, cos;

	x0 = a->xt;
	y0 = a->yt;
	x1 = a->xb;
	y1 = a->yb;
	x0 -= b->xt;
	y0 -= b->yt;
	x1 -= b->xt;
	y1 -= b->yt;
	sin = b->sin;
	cos = b->cos;
	yt = (y0 * cos - x0 * sin) >> SINCOS_SHIFT;
	yb = (y1 * cos - x1 * sin) >> SINCOS_SHIFT;

/*   Bound & Zone Comparae. Phase I   */
	if(yt >= 0 && yb < 0) m = 1;
	else if(yt < 0 && yb >= 0) m = -1;
	else return 0;

/*   Bound & Zone Comparae. Phase II  */
	xt = (x0 * cos + y0 * sin) >> SINCOS_SHIFT;
	xb = (x1 * cos + y1 * sin) >> SINCOS_SHIFT;
	if(xt < 0 && xb < 0) return 0;

/*   Bound & Zone Comparae. Phase III */
	vs = b->vs;
	a->vp = xt;
	if(xt > vs && xb > vs) return 0;

	if(yt>=0 && yb<0)
	{
	if(xt<xb) { dx = ((xb-xt) *  yt) / (yt-yb); tt = dx + xt; }
	else { dx = ((xt-xb) * -yb) / (yt-yb); tt = dx + xb; }
	}
	else
	{
	if(xt<xb) { dx = ((xb-xt) * -yt) / (yb-yt); tt = dx + xt; }
	else { dx = ((xt-xb) *  yb) / (yb-yt); tt = dx + xb; }
	}

/*   Bound & Zone Comparae. Phase IV   */
	if(tt > vs || tt < 0) return 0;
	return m;
}

/*------------------------------------------------------/
/							/
/	Line to Line check.				/
/							/
/------------------------------------------------------*/
static int HitCheckLineToLine(MYXY *a, HIT *b)
{
	int	xt, xb, yt, yb, dx, tt;
	int	x0, x1, y0, y1, vs, m, sin, cos;

	x0 = a->xt;
	y0 = a->yt;
	x1 = a->xb;
	y1 = a->yb;
	x0 -= b->xt;
	y0 -= b->yt;
	x1 -= b->xt;
	y1 -= b->yt;
	sin = b->sin;
	cos = b->cos;
	yt = (y0 * cos - x0 * sin) >> SINCOS_SHIFT;
	yb = (y1 * cos - x1 * sin) >> SINCOS_SHIFT;

/*   Bound & Zone Comparae. Phase I   */
	if(yt >= 0 && yb < 0) m = 0;
	else if(yt < 0 && yt >= -ballRadius) m = 1;
	else return 0x7fffffff;

/*   Bound & Zone Comparae. Phase II  */
	xt = (x0 * cos + y0 * sin) >> SINCOS_SHIFT;
	xb = (x1 * cos + y1 * sin) >> SINCOS_SHIFT;
	if(xt < 0 && xb < 0) return 0x7fffffff;

/*   Bound & Zone Comparae. Phase III */
	vs = b->vs;
	a->vp = xt;
	if(xt > vs && xb > vs) return 0x7fffffff;
	else if(m) {
		return 0;
		}

	if(yt>=0 && yb<0)
	{
	if(xt<xb) { dx = ((xb-xt) *  yt) / (yt-yb); tt = dx + xt; }
	else { dx = ((xt-xb) * -yb) / (yt-yb); tt = dx + xb; }
	}
	else
	{
	if(xt<xb) { dx = ((xb-xt) * -yt) / (yb-yt); tt = dx + xt; }
	else { dx = ((xt-xb) *  yb) / (yb-yt); tt = dx + xb; }
	}

/*   Bound & Zone Comparae. Phase IV   */
	if(tt > vs || tt < 0) return 0x7fffffff;
	a->vp = tt;
	dx = tt - xt;
	return (dx * dx + yt * yt + 2);
}

/*------------------------------------------------------/
/							/
/	Line to Circle test.				/
/							/
/------------------------------------------------------*/
static int HitCheckLineToCircle(MYXY *a, HIT *b)
{
	int	vs, v, x, y, dx, x0, y0;

/*   Bound & Zone Comparae. Phase I   */
	x0  = b->xt - a->xt;
	y0  = b->yt - a->yt;
	vs = b->vs;
	if(x0 <= vs && x0 >= -vs && y0 <= vs && y0 >= -vs)
	{
	v = x0 * x0 + y0 * y0;
	if(v <= b->vss)
		{
		b->cos = -x0;
		b->sin = -y0;
		a->vp = v;
		return 1;
		}
	}

/*   Bound & Zone Comparae. Phase II  */
	y = (y0 * a->cos - x0 * a->sin) >> SINCOS_SHIFT;
	if(y > vs || y < -vs) return 0x7fffffff;

/*   Bound & Zone Comparae. Phase III */
	x = (x0 * a->cos + y0 * a->sin) >> SINCOS_SHIFT;
	if(x < 0 || (x - vs) > a->svs) return 0x7fffffff;

/*   Bound & Zone Comparae. Phase IV  */
	v = b->vss - y * y;
	if(v < 0) v = 0;
	dx = Isqrt( v );
	if((x - dx) > a->svs) return 0x7fffffff;

/*   Bound & Zone Comparae. Phase V   */
	b->cos = -dx;
	b->sin = -y;
	v = x - dx;
	a->vp = v;
	return (v * v + 2);
}

/*------------------------------------------------------/
/							/
/	Zone Area Judge.				/
/							/
/------------------------------------------------------*/
static int ScanZoneArea(int x, int y, short *zone)
{
	int	i, j, xt, yt, sin, cos;

	j = *(unsigned short *)zone++;
	for(i=0; i<j; ++i)
	{
	xt = *zone++;
	yt = *zone++;
	sin = *zone++;
	cos = *zone++;
	if(((y - yt) * cos - (x - xt) * sin) >= 0) return 0;
	}
	return 1;
}

/*------------------------------------------------------/
/							/
/	Service Switch					/
/							/
/------------------------------------------------------*/
static void ServiceSwitch(BALL *blp)
{
	int	i, j, k, x, y;
	unsigned int	p, n;
	PLAYER	*ply;

	ply = &playerTab[playerNo];

	p = swPos;
	n = swNeg;
	swPos = swNeg = 0;
	if(tiltGo) j = 0; else j = 1;
	for(i=0; i<32; i++)
	{
	switch(i)
		{
	case SW_SHOOTER_LANE:
		if(p & 1) {
			blp->shooterFlag = 1;
			blp->zp = blp->zs = 0; 
			blp->gravity = gravityTable;
			blp->layer = _LAYER_0;
			if(blp->mode!=MODE_SHOOT_READY) {
				blp->mode = MODE_SHOOT_READY;
				blp->xs = blp->ys = 0;
				}
			}
		break;
	case SW_OUT_HOLE:
		if(p & 1) {
			blp->gravity = gravityTable;
			blp->mode = MODE_OUT_HOLE;
			}
		break;
	case SW_KICK_BACK_RIGHT:
		if((p & j)!=0 && ply->kickbackEnable!=0) {
			ply->kickbackOn[0] = 1;
/* y = kickbackSpd + ((unsigned int)Rand() % kickbackRans) - (kickbackRans >> 1);
*/
			blp->ys -= y; blp->ym -= 12000;
			}
		break;
	case SW_KICK_BACK_LEFT:
		if((p & j)!=0 && ply->kickbackEnable!=0) {
			ply->kickbackOn[1] = 1;
			y = kickbackSpd + ((unsigned int)Rand() % kickbackRans) - (kickbackRans >> 1);
			blp->ys += y; blp->ym += y;
			}
		break;
	case SW_OUT_LANE_RIGHT:
	case SW_OUT_LANE_LEFT:
		if(p & j) {
			ply->switchTotal++;
			ply->switchFrame++;
			ply->outLaneOn[i-SW_OUT_LANE_RIGHT] = 1;
			}
		break;
	case SW_RETURN_LANE_RIGHT:
	case SW_RETURN_LANE_LEFT:
		if((p | n) & j) {
			ply->switchTotal++;
			ply->switchFrame++;
			ply->returnLaneOn[i-SW_RETURN_LANE_RIGHT] = 1;
			returnLaneSw++;
			returnLaneTotal++;
			}
		break;
	case SW_TOP_LANE_1:
	case SW_TOP_LANE_2:
	case SW_TOP_LANE_3:
	case SW_TOP_LANE_4:
	case SW_TOP_LANE_5:
	case SW_TOP_LANE_6:
		if((p | n) & j) {
			ply->switchTotal++;
			ply->switchFrame++;
			ply->topLaneOn[i-SW_TOP_LANE_1] = 1;
			ply->topLaneFlag[i-SW_TOP_LANE_1] = 1;
			topLaneSw++;
			topLaneTotal++;
			}
		break;
	case SW_RAMP_BEGINE:
		if(n & 1) {
			blp->gravity = gravityTable;
			blp->zp = swPara[blp->gate].zw << POSITION_SHIFT;
			blp->mode = MODE_NORMAL;
			}
		if(p & 1) {
			blp->gravity = gravityRamp;
			blp->mode = MODE_RAMP_ENTER;
			}
		break;
	case SW_RAMP_EXIT:
		if(p & 1) {
			if(blp->mode==MODE_LOOP_SHOOTER) {
				blp->layer = _LAYER_0;
				blp->mode = MODE_FALL;
				}
			else blp->mode = MODE_RAMP_EXIT;
			}
		break;
	case SW_RAMP1_ENTER:
	case SW_RAMP2_ENTER:
	case SW_RAMP3_ENTER:
	case SW_RAMP4_ENTER:
		if(n & 1) {
			blp->mode = MODE_RAMP_ENTER;
			blp->gate = i;
			blp->gravity = gravityRamp;
			blp->layer = _LAYER_1;
			}
		if(p & 1) {
			blp->mode = MODE_NORMAL;
			blp->zp = blp->zs = 0; 
			blp->gravity = gravityTable;
			blp->layer = _LAYER_0;
			}
		break;
	case SW_NEXT_STAGE:
		k = p | n;
		if(k & j) {
		ply->nextStageOn = 1;
		ply->switchTotal++;
		ply->switchFrame++;
		blp->mode = MODE_BALL_HOLD;
		}
		break;
	case SW_LANE_ROLLOVER_RIGHT:
	case SW_LANE_ROLLOVER_LEFT:
		if(p & j) {
			ply->switchTotal++;
			ply->switchFrame++;
			ply->laneRollOverOn[i-SW_LANE_ROLLOVER_RIGHT] = 1;
			}
		break;
	case SW_LANE_ENTER_RIGHT:
	case SW_LANE_ENTER_LEFT:
		if(p & j) {
			ply->switchTotal++;
			ply->switchFrame++;
			ply->laneEnterOn[i-SW_LANE_ENTER_RIGHT] = 1;
			}
		break;
	case SW_RAMP_ROLLOVER_RIGHT:
	case SW_RAMP_ROLLOVER_LEFT:
		if(p & j) {
		ply->rampRollOverOn[i-SW_RAMP_ROLLOVER_RIGHT] = 1;
		ply->switchTotal++;
		ply->switchFrame++;
		ply->switchRamps++;
		rampTotal[i-SW_RAMP_ROLLOVER_RIGHT]++;
		rampSw[i-SW_RAMP_ROLLOVER_RIGHT]++;
		}
		break;
	case SW_SUSPENSION_1:
	case SW_SUSPENSION_2:
		if(p & j) {
		ply->suspensionRollOverOn[i-SW_SUSPENSION_1] = 1;
		ply->switchTotal++;
		ply->switchFrame++;
		}
		break;
	default:
		break;
		}
	p >>= 1;
	n >>= 1;
	}
}

static void ServiceMode(BALL *blp)
{
	int	i, j, k;
	SWITCH	*sw;
	PLAYER	*ply;

	ply = &playerTab[playerNo];
	switch(blp->mode)
	{
	case MODE_RAMP_EXIT:
		if(blp->ys < 0) i = -blp->ys;
		else i = blp->ys;
		if(i <= 400) {
			blp->mode = MODE_FALL;
/*			blp->layer = _LAYER_0; */
			}
		blp->ys -= blp->gravity;
		break;
	case MODE_RAMP_ENTER:
		sw = &swPara[blp->gate];
		i = blp->yp >> POSITION_SHIFT;
		if(i > sw->yt) i = sw->yt;
		else if(i < sw->yb) i = sw->yb;
		blp->zp = (((i - sw->yb) * sw->zw) / (sw->yt - sw->yb)) << POSITION_SHIFT;
		blp->ys -= blp->gravity;
		break;
	case MODE_FALL:
		if(blp->zp==0) {
			blp->layer = _LAYER_0;
			blp->mode = MODE_NORMAL;
			}
		else
		{
		blp->zs -= gravityFall;
		blp->zp += blp->zs;
		if(blp->zp < 0) blp->zp = blp->zs = blp->zm = 0;
		}
		break;
	case MODE_NORMAL:
	case MODE_LOOP_SHOOTER:
		blp->zs = blp->zm = 0;
		if(magnaZone) {
		k = Isqrt(magnaSq);
		if(k < (ballRadius >> 2)) {
		blp->xs >>= 1;
		blp->ys >>= 1;
		}
		else	{	
			i = (magnaDx << 8) / k;
			j = (magnaDy << 8) / k;
			blp->xs += (i * gravityFall) >> 8;
			blp->ys += (j * gravityFall) >> 8;
			}
		}
		else blp->ys -= gravityTable;
		break;
	case MODE_OUT_HOLE:
		blp->gravity = gravityTable;
		blp->zs = blp->zm = 0;
		blp->ys -= blp->gravity;
		blp->layer = _LAYER_0;
		blp->mode = MODE_BALL_READY;
		break;
	case MODE_BALL_READY:
		blp->gravity = gravityTable;
		blp->zs = blp->zm = 0;
		blp->ys -= blp->gravity;
		blp->layer = _LAYER_0;
		blp->vukDelay = 0;
		break;
	case MODE_HOLE:
		blp->gravity = gravityTable;
		blp->xs = blp->ys = 0;
		i = (ballRadius << POSITION_SHIFT) * -4;
		if(blp->zp > i)
		{
		blp->zs -= gravityFall;
		blp->zp += blp->zs;
		}
		else
		{
		blp->zp = i;
		blp->vukDelay = (2 * frameRate) + SumDelayVUK(blp);
		blp->mode = MODE_VUK;
		}
		break;
	case MODE_VUK:
		blp->layer = _LAYER_1;
		if(ply->vukModeDelay) ply->vukModeDelay--;
		if(ply->holeSelectorEnable!=0 && ply->roundGoNo==0 &&
			ply->multiBallGo==0 && ply->vukModeDelay==0 &&
				blp->holeNo==ply->holeSelectorNo) {
			blp->mode = MODE_BALL_READY;
			blp->layer = _LAYER_0;
			blp->vukDelay = 0;
			blp->xb = blp->xp = startBallX << POSITION_SHIFT;
			blp->yb = blp->yp = startBallY << POSITION_SHIFT;
			blp->zb = blp->zp = startBallZ << POSITION_SHIFT;
			blp->xs = blp->ys = blp->zs = 0;
			blp->xm = blp->ym = blp->zm = 0;
			ply->holeSelectorEnable = 0;
			}
		else if(blp->vukDelay) blp->vukDelay--;
		else if(ply->vukEnable) {
			blp->xp = blp->xb = vukPosX[0] << POSITION_SHIFT;
			blp->yp = blp->yb = vukPosY[0] << POSITION_SHIFT;
			sw = &swPara[SW_RAMP1_ENTER];
			blp->zs -= gravityFall;
			blp->zp += blp->zs;
			if(blp->zp <= ((ballRadius << POSITION_SHIFT) * -3)) {
				RingSfx(SFX_VUK);
				blp->zs += vukSpd + ((unsigned int)Rand() % vukRans) - (vukRans >> 1);
				}
			else if(blp->zp >= (sw->zw << POSITION_SHIFT)) {
			blp->zp = (sw->zw << POSITION_SHIFT);
			blp->mode = MODE_NORMAL;
			ply->vukOn[0] = 1;
			ply->switchTotal++;
			}
		}
		break;
	case MODE_SHOOT_READY:
		blp->gravity = gravityTable;
		if(blp->shooterFlag!=0 && ply->shooterEnable!=0)
		{
		RingSfx(SFX_PLUNGER_SHOT);
		blp->ys = shooterSpd + ((unsigned int)Rand() % shooterRans) - (shooterRans >> 1);
		blp->ym = blp->ys;
		blp->shooterFlag = 0;
		blp->mode = MODE_NORMAL;
		blp->vukDelay = 0;
		shoottingCounter = frameRate * 2;
		}
		blp->ys -= blp->gravity;
		blp->zs = blp->zm = 0;
		break;
	case MODE_SAUCER:
		blp->xs = blp->ys = blp->xm = blp->ym = 0;
		if(ply->saucerEnable[blp->holeNo]) {
		ply->saucerEnable[blp->holeNo] = 0;
		RingSfx(SFX_BALL_EJECT);
		blp->ys -= 5000;
		blp->mode = MODE_NORMAL;
		}
		break;
	case MODE_BALL_HOLD:
		blp->xs = blp->ys = blp->xm = blp->ym = 0;
		break;
	default:
		blp->gravity = gravityTable;
		break;
	}
}

static int SumDelayVUK(BALL *my)
{
	int	i, s;
	BALL	*blp;

	s = 0;
	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp!=my && blp->mode==MODE_VUK)
		{ if(blp->vukDelay > s) s = blp->vukDelay; }
	blp++;
	}
	return s;
}

int ScanVUK(void)
{
	int	i;
	BALL	*blp;

	blp = &ball[0];
	for(i=0; i<ballMax; ++i)
	{
	if(blp->mode==MODE_VUK) return 1;
	blp++;
	}
	return 0;
}

static void ServiceMagnaLock(BALL *blp)
{
	int	i, k;

	magnaZone = 0;

	if(magnaLockMax==0 || magnaLockEnable==0) return;

	magnaDx = magnaPosX - (blp->xp >> POSITION_SHIFT);
	if(magnaDx < 0) k = -magnaDx; else k = magnaDx;
	if(k > magnaRadius) return;
	magnaDy = magnaPosY - (blp->yp >> POSITION_SHIFT);
	if(magnaDy < 0) k = -magnaDy; else k = magnaDy;
	if(k > magnaRadius) return;

	magnaSq = magnaDx * magnaDx + magnaDy * magnaDy;
	if(magnaSq > magnaRadius2) return;

	magnaZone = 1;
}
