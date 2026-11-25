#pragma once
//=========================================================================//
/*! @file
	@brief  ボールシュミレーター NRCRONOMICON・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

/* Gladiators: 63956 / 1120[mm] */
/* Necronomicon: 63956 / 1260[mm] */
/* 1ST:Difarential count loop = 4, break value = 8 */
/* 2ND:Difarential count loop = 4, break value = 33 */

#define DIF_COUNT_LOOP		4
/* 8.27 break: 44 */
#define DIF_BREAK_VALUE		52
#define BOUND_LIST_MAX		150

#define PLAYER_MAX		4
#define MULTI_BALL_MAX		8
#define SCORE_MAX		12

#define FLIPPER_SPEED_FRD	(2048)
#define FLIPPER_SPEED_BWD	(-1024)
#define DEMO_FLIPPER_TIME	25

/* Hits factor (Max:1024) */
#define METAL_HIT_FACTOR	290
#define PLASTIC_HIT_FACTOR	200
#define RUBBER_BAND_HIT_FACTOR	470
#define RUBBER_POST_HIT_FACTOR	490
#define RAMP_HIT_FACTOR		180
#define HOLE_GUIDE_HIT_FACTOR	25
#define FLIPPER_HIT_FACTOR	510

/* DXF attribute names */
#define ATTR_MASK		0x0F00
#define BALLS			0x0000
#define POLYLINE		0x0100
#define DROP_TARGET		0x0200
#define JET_BUMPER		0x0300
#define STRUCT_RAMP		0x0400
#define SLING_SHOT		0x0500
#define STAND_UP		0x0600
#define LR_FLIPPER		0x0700
#define OTHER_BALL		0x0800
#define HOPPER			0x0900
#define HOLE_ZONE		0x0A00
#define MOVE_GATE		0x0B00
#define MAGNA_LOCK		0x0C00
#define SAUCER_ZONE		0x0D00
#define SWITCH_GATE		0x0F00

#define _METAL			0x0000
#define _PLASTIC		0x1000
#define _RUBBER_BAND		0x2000
#define _RUBBER_POST		0x3000
#define _RAMP			0x4000
#define _HOLE_GUIDE		0x5000
#define _FLIPPER		0x6000

#define LAYER_MASK		0x00C0
#define _LAYER_0		0x0000
#define _LAYER_1		0x0040
#define _LAYER_2		0x0080
#define _LAYER_ALL		0x00C0

#define FLIPPER_MAX		4
#define FLIPPER_STEP		6

#define POSITION_SHIFT		4
#define SINCOS_SHIFT		14
#define BOUND_SHIFT		10

#define VUK_MAX			4

#define ZONE_DIVIDE		16
#define ZONE_SIZE		(65536 / ZONE_DIVIDE)
#define AREA_GAP		30

#define SPEED_MAX_X		((31000 / ZONE_DIVIDE) << POSITION_SHIFT)
#define SPEED_MAX_Y		((31000 / ZONE_DIVIDE) << POSITION_SHIFT)
#define SPEED_MAX_Z		((31000 / ZONE_DIVIDE) << POSITION_SHIFT)
/*
/#define SPEED_MAX_X		((29000 / ZONE_DIVIDE) << POSITION_SHIFT)
/#define SPEED_MAX_Y		((29000 / ZONE_DIVIDE) << POSITION_SHIFT)
/#define SPEED_MAX_Z		((29000 / ZONE_DIVIDE) << POSITION_SHIFT)
*/
#define CONECT_LINE_GAP		6
#define CONECT_CIRCLE_GAP	9

#define SW_SHOOTER_LANE		0
#define SW_OUT_HOLE		1
#define SW_KICK_BACK_RIGHT	2
#define SW_KICK_BACK_LEFT	3
#define SW_OUT_LANE_RIGHT	4
#define SW_OUT_LANE_LEFT	5
#define SW_RETURN_LANE_RIGHT	6
#define SW_RETURN_LANE_LEFT	7
#define SW_TOP_LANE_1		8
#define SW_TOP_LANE_2		9
#define SW_TOP_LANE_3		10
#define SW_TOP_LANE_4		11
#define SW_TOP_LANE_5		12
#define SW_TOP_LANE_6		13
#define SW_RAMP_BEGINE		14
#define SW_RAMP_EXIT		15
#define SW_RAMP1_ENTER		16
#define SW_RAMP2_ENTER		17
#define SW_RAMP3_ENTER		18
#define SW_RAMP4_ENTER		19
#define SW_NEXT_STAGE		20

#define SW_LANE_ROLLOVER_RIGHT	24
#define SW_LANE_ROLLOVER_LEFT	25
#define SW_LANE_ENTER_RIGHT	26
#define SW_LANE_ENTER_LEFT	27
#define SW_RAMP_ROLLOVER_RIGHT	28
#define SW_RAMP_ROLLOVER_LEFT	29
#define SW_SUSPENSION_1		30
#define SW_SUSPENSION_2		31

#define	DIVERTER1_TRUE		0
#define	DIVERTER1_FALSE		1
#define DIVERTER2_TRUE		2
#define DIVERTER2_FALSE		3
#define DIVERTER3_TRUE		4
#define DIVERTER3_FALSE		5
#define	SUSPENSION1		6
#define	SUSPENSION2		7

#define SIM_OPTION_VECT		0x0001
#define SIM_OPTION_ZONE		0x0002

#define OPTION_PLAYERS			0
#define OPTION_CONTROLLER		1
#define OPTION_BGM_DISABLE		2

typedef struct {
	unsigned int	vs;	/* Vector Size */
	unsigned int	vss;
	short	atr;		/* Attribute */
	short	xt, yt;		/* X, Y, Z Top */
	short	sin, cos;	/* Sin, Cos */
} HIT;

typedef struct {
	int	fieldWidth;	/* field width [mm] */
	int	fieldHeight;	/* field height [mm] */ 
	int	camX;		/* camera position X [mm] (no effect) */
	int	camY;		/* camera position Y [mm] */
	int	camZ;		/* camera position Z [mm] */
	int	refX;		/* reference position X [mm] (no effect) */
	int	refY;		/* reference position Y [mm] */
	int	refZ;		/* reference position Z [mm] (no effect) */
	int	screenWidth;	/* screen width [pixel] */
	int	screenHeight;	/* screen height [pixel] */
	int	tanWidth;	/* view corn width  [tan(@) * 2^18] */
	int	tanHeight;	/* view corn height [tan(@) * 2^18] */
	int	frameRate;	/* frame rate per secound */
	int	fpX[4];		/* flipper position X */
	int	fpY[4];		/* flipper position Y */
	int	dtpX[8];	/* drop target position X */
	int	dtpY[8];	/* drop target position Y */
	int	shooterSpd;	/* [mm/s] */
	int	kickbackSpd;	/* [mm/s] */
	int	vukSpd;		/* [mm/s] */
	int	loopSpd;	/* [mm/s] */
	int	slingshotSpd;	/* [mm/s] */
	int	jetbumperSpd;	/* [mm/s] */
	int	slingshotTrgSpd;	/* [mm/s] */
	int	jetbumperTrgSpd;	/* [mm/s] */
	int	droptargetTrgSpd;	/* [mm/s] */
	int	standupTrgSpd;		/* [mm/s] */
	int	flipperSpdFactor[4];	/* [???] */
	int	flipperRengeFactor[4];	/* [???] */
	int	flipperBlurFactor[4];	/* [???] */
	int	flipperOffsetFactor[4];	/* [???] */
	int	slingShotPosX[2];
	int	slingShotPosY[2];
	int	jetBumperPosX[4];
	int	jetBumperPosY[4];
} FIELD;

typedef struct {
	int	xp, yp, zp;	/* start to position X, Y, Z */
	int	xb, yb, zb;	/* terminated position X, Y, Z */
	int	xs, ys, zs;	/* speed at master */
	int	xm, ym, zm;	/* spped at one frame */
	int	sin, cos, svs;
	int	vp, vl;
	int	sin1, cos1;
	int	gate;
	int	layer;
	int	mode;
	HIT	*hit;
	int	gravity;
	int	vukDelay;
	int	test;
	char	shooterFlag;
	char	holeNo;
	short	psychoDelay;
} BALL;

typedef struct {
	unsigned short	sw;
	short	sound;
	int	cx, cy, cz, hl;
	short	prx, pry;
	int	spd, new, old, an;
	int	sin1[FLIPPER_STEP], cos1[FLIPPER_STEP];
} FLIPPER;

typedef struct {
	int yb, yt;
	int zw;
} SWITCH;

typedef struct {
  int sin, cos;
} SINCOS;

typedef struct {
	char score[12];
	char roundScore[12];
	char outHoleBonus[12];
	char deathWatchScore[12];
	char jackpot[12];

	short tiltWarnings;		/* 規定以上の揺れがあった場合警告する回数 */
	short freezeSwitch;
	short madnessSpellTimer;

	unsigned char ballMax;		/* １ゲームの最大プレイボール数 */
	unsigned char ballInPlay;	/* プレイ中のボールカウント */
	unsigned char multiBalls;	/* フィールドにある総ボール数 */
	unsigned char ballSaveLimit;

	unsigned short	multiBallRestart;
	unsigned short jetHits;
	unsigned short slingHits;
	unsigned short switchTotal;
	unsigned short switchRamps;
	unsigned short switchFrame;
	unsigned short vukModeDelay;

	unsigned int rampShotCount;
	unsigned int extraBallReady;
	unsigned int mysteryReady;
	unsigned int kickbackReady;
	unsigned int deathWatchReady;
	unsigned int mazeValueCount;
	unsigned int kickbackCompCount;

	char	tableNo;

	char	kickbackEnable;
	char	shooterEnable;
	char	holeSelectorEnable;
	char	vukEnable;
	char	loopEnable;
	char	gateEnable[16];
	char	saucerEnable[4];

	char	jetBumperOn[4];
	char	slingShotOn[2];
	char	topLaneOn[8];
	char	holeOn[4];
	char	outLaneOn[2];
	char	returnLaneOn[2];
	char	laneEnterOn[2];
	char	laneRollOverOn[2];
	char	rampRollOverOn[4];
	char	suspensionRollOverOn[2];
	char	dropTargetOn[8];
	char	standUpOn[16];
	char	flipperOn[4];
	char	kickbackOn[2];
	char	vukOn[2];
	char	saucerOn[4];
	char	nextStageOn;

	char	topLaneFlag[8];
	char	dropTargetFlag[8];
	char	dropTargetEnable[8];
	char	standUpFlag[16];
	char	roundCompFlag[8];
	char	mazeValueFlag[4];
	char	holeSelectorNo;
	char	multiBallCompFlag;
	char	bonusMultiCount;
	char	extraBallCount;
	char	extraBallCycle;

	char	superRoundEnable;
	char	bonusHoldEnable;
	char	superJetsEnable;
	char	playField2xEnable;
	char	longerTimerEnable;
	char	multiBallEnable;
	char	ballLockEnable;

	char	superKickbackReady;
	char	roundStartReady;
	char	roundNumber;
	char	roundGoNo;
	char	multiBallReady;
	char	multiBallGo;
	char	ballSaveFlag;
	char	ballSaveAdd;
	char	multiBallCount;
	char	loopBonusCount;
	char	multiBallDepth;
	char	madnessSpell;
	char	roundStartCount;
	char	roundStartDepth;
	char	multiBallSpell;
	char	countUpMulti;
	char	multiReadyCount;
	char	nameInPos;
} PLAYER;
