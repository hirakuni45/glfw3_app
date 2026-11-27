//=========================================================================//
/*! @file
	@brief  pinball general for Necronomicon
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdio.h>
#include <string.h>
#include "ballsim.h"
#include "saturn.h"
#include "info.h"
#include "score.h"
#include "lite.h"
#include "sound.h"
#include "option.h"
#include "fnc.h"
#include "top.h"
#include "cdfs.h"
#include "arkham.h"
#include "bloody.h"
#include "dreaml.h"
#include "pinball.h"
#include "sim.h"
#include "etc.h"
#include "flags.h"
#include "vect.h"
#include "genr.h"

#define FREEZE_SWITCH	15

typedef struct {
	char	*txt;
	char	rate;
	char	flag;
	char	m;
} POWER_SELECT;

#define	POWER_ITEM_MAX		14

#define GMODE_GAME_INIT		0
#define GMODE_PLAY_INIT		1
#define	GMODE_SHOOT_READY	2
#define GMODE_POWER_SELECT	3
#define	GMODE_PLAY_GAME		4
#define	GMODE_BALL_LOST		5
#define GMODE_EXTRA_BALL	6
#define GMODE_EXTRA_BALL2	7
#define GMODE_GAME_OVER		8
#define GMODE_SHOOTER_WAIT	9
#define GMODE_NEXT_STAGE	10
#define GMODE_COMING_SOON	11
#define GMODE_PLAYER_READY	12
#define GMODE_NAME_ENTRY	13
#define GMODE_NAME_DELAY	14
#define GMODE_GAME_REPLAY	15
#define GMODE_TURN_TILT		16

static void InitGameValues(void);
static void ServiceEtc2(PLAYER *ply, char *sct);
static void ServiceEtc(PLAYER *ply, char *sct);
static void ServiceRoundIndicator(PLAYER *);
static void ServiceKickback(PLAYER *ply, char *);
static void ServiceMazeSpell(PLAYER *ply, char *sct);
static void ServiceRampShot(PLAYER *ply, char *sct);
static void ServiceBallSaver(PLAYER *ply);
static void LightCircle(int r);
static void InitLightRoll(int xo, int yo, unsigned int *a, unsigned int *b);
static void LightRoll(int a, int b, unsigned int *ab, unsigned int *bb);
static void LightWidth(int yp, int w);
static void LightHeight(int yp, int w);
static void InitDiags(void);
static void ServiceDiag(PLAYER *ply);
static void AddKickback(PLAYER *);
static void ServiceTestRule(PLAYER *);
static void StatusSub(int i, int y, char *s);
static void ServiceBGM(PLAYER *ply);
static void ServiceJackpotArrow(PLAYER *ply);
static int InstallScore(PLAYER *ply);
static int ServiceNameEntry(PLAYER *ply);

short trialTimer;
short soulTask;
short diagInfoTab[4];
short roundTimer, roundStartDelay, roundDelay;
char tiltGo;
char gameMode;
char nextStageEnable;
char roundValue[12];
char *scoreConst[50];

static const char *scoreConstSrc[] = {
	"000000030200", /*  0:STAND UP */
	"000000300000", /*  1:STAND UP COMPLITE */
	"000000025010", /*  2:DROP TARGET */
	"000000250000", /*  3:DROP TARGET COMPLITE */
	"000000054320", /*  4:PIT HOLE */
	"000000018880", /*  5:VUK */
	"000000200000", /*  6:RAMP */
	"000000150150", /*  7:LANE */
	"000000010010", /*  8:RETURN LANE */
	"000000050050", /*  9:OUT LANE */
	"000000200200", /* 10:KICK BACK */
	"000000002490", /* 11:JET BUMPER */
	"000000002870",	/* 12:TOP LANE */
	"000000003230",	/* 13:SLING SHOT */
	"000000033330",	/* 14:SILVER COUNT UP */
	"000000010620",	/* 15:DEATH COUNT SOWN */
	"000000010000", /* 16:010K */
	"000000020000", /* 17:020K */
	"000000050000", /* 18:050K */
	"000000100000",	/* 19:100K */
	"000000200000",	/* 20:200K */
	"000000300000",	/* 21:300K */
	"000000400000",	/* 22:400K */
	"000000500000",	/* 23:500K */
	"000000600000",	/* 24:600K */
	"000001000000",	/* 25:001M */
	"000002000000",	/* 26:002M */
	"000003000000",	/* 27:003M */
	"000004000000",	/* 28:004M */
	"000005000000",	/* 29:005M */
	"000006000000",	/* 30:006M */
	"000007000000",	/* 31:007M */
	"000008000000",	/* 32:008M */
	"000009000000",	/* 33:009M */
	"000010000000",	/* 34:010M */
	"000015000000",	/* 35:015M */
	"000020000000",	/* 36:020M */
	"000030000000",	/* 37:030M */
	"000040000000",	/* 38:040M */
	"000050000000",	/* 39:050M */
	"000060000000",	/* 40:060M */
	"000080000000",	/* 41:080M */
	"000100000000",	/* 42:100M */
	"000002500000",	/* 43:0025M */
	"000200000000",	/* 44:200M */
	"000902010800", /* 45:INITIAL SCORE ARKHAM */
	"000009218000", /* 46:INITIAL ADD SCORE ARKHAM */
	"000706050000", /* 47:INITIAL SCORE BLOODY */
	"000005800000", /* 48:INITIAL ADD SCORE BLOODY */
	"000507030000", /* 49:INITIAL SCORE DREAML */
	"000005800000", /* 50:INITIAL ADD SCORE DREAML */
	NULL };
static char scoreConstPad[12 * 50];

char lightCommand[50];
short *lightPosPtr;
const uint8_t *lightAnimePtr;

const uint8_t liteBLKALL[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_JUMP, -2 * 2 };
const uint8_t liteWBT[] = {
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteWBT2[] = {
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteWBT3[] = {
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteWTB[] = {
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteWTB2[] = {
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteTCW[] = {
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_STOP };
const uint8_t liteTCC[] = {
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_STOP };
const uint8_t liteBLK[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteBLK2[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteBLK3[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteBLK4[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteBLK8[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteBLK16[] = {
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteLTA[] = {
	LIGHT_ANIME_ON, 60,
	LIGHT_ANIME_OFF, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteWBT_BLK_WBT_BLK[] = {
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_ON, 10,
	LIGHT_ANIME_OFF, 10,
	LIGHT_ANIME_STOP };
const uint8_t liteWBT_WTB_TCW_TCC[] = {
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_STOP };
const uint8_t liteTCW_TCC[] = {
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_STOP };
const uint8_t liteTCW_TCC_WBT[] = {
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteTCW_TCC_WBT_WTB[] = {
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteTCW_TCC_WBT_WBT[] = {
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_STOP };
const uint8_t liteNightMare[] = {
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_ROLL_CW, 90,
	LIGHT_ANIME_ROLL_CCW, 90,
	LIGHT_ANIME_WAVE_UP, 60,
	LIGHT_ANIME_WAVE_DOWN, 60,
	LIGHT_ANIME_STOP };

static const uint8_t liteTilt[] = {
	LIGHT_ANIME_OFF, 60,
	LIGHT_ANIME_JUMP, -1 * 2,
	LIGHT_ANIME_STOP };
static const uint8_t liteDemo[] = {
	LIGHT_ANIME_ROLL_CW, 180,
	LIGHT_ANIME_ROLL_CW, 180,
	LIGHT_ANIME_ROLL_CCW, 180,
	LIGHT_ANIME_ROLL_CCW, 180,
	LIGHT_ANIME_CIRCLE_IN, 90,
	LIGHT_ANIME_CIRCLE_OUT, 90,
	LIGHT_ANIME_CIRCLE_IN, 90,
	LIGHT_ANIME_CIRCLE_OUT, 90,
	LIGHT_ANIME_ROLL2_CW, 180,
	LIGHT_ANIME_ROLL2_CW, 180,
	LIGHT_ANIME_ROLL2_CCW, 180,
	LIGHT_ANIME_ROLL2_CCW, 180,
	LIGHT_ANIME_WAVE_UP, 90,
	LIGHT_ANIME_WAVE_DOWN, 90,
	LIGHT_ANIME_WAVE_UP, 90,
	LIGHT_ANIME_WAVE_DOWN, 90,
	LIGHT_ANIME_WAVE_RIGHT, 90,
	LIGHT_ANIME_WAVE_LEFT, 90,
	LIGHT_ANIME_WAVE_RIGHT, 90,
	LIGHT_ANIME_WAVE_LEFT, 90,
	LIGHT_ANIME_JUMP, -20 * 2,
	LIGHT_ANIME_RANDOM, 0,
	LIGHT_ANIME_STOP
};
static short loopBonusTimer;
static short mazeValueComp;
static short replayTimer;
static char playerSwap;
static char multiBallReadyOld, multiBallGoOld, roundGoNoOld;
static char loopBonusScore[16];
static const char gameSoundGroup[] = {
/* GENERAL */
	SG_A, SG_G, SG_G, SG_G, SG_G, SG_G, SG_D, SG_G,
	SG_H, SG_H, SG_H, SG_H, SG_C, SG_B, SG_G, SG_G,
	SG_G, SG_G, SG_F, SG_F, SG_E, SG_E, SG_C, SG_B,
	SG_E, SG_E, SG_D, SG_G, SG_F, SG_F, 0, 0,
/* ARKHAM */
	SG_D, 0, SG_C, SG_D, 0, SG_E, SG_E, SG_E,
	SG_H, 0, 0, SG_C, SG_B, SG_B, SG_B, SG_B,
	0, 0, 0, 0, SG_B, SG_B, SG_B, SG_B,
	SG_C, SG_B, SG_D, SG_E, 0, 0, 0, 0,
/* BLOODY */
	SG_D, 0, SG_C, SG_D, 0, SG_E, SG_E, SG_E,
	SG_H, 0, 0, SG_C, SG_B, SG_B, SG_B, SG_B,
	0, 0, 0, 0, SG_B, SG_C, SG_B ,SG_B,
	SG_C, SG_B, 0, 0, 0, 0, 0, 0,
/* DREAML */
	SG_D, 0, SG_C, SG_D, 0, SG_E, SG_E, SG_E,
	SG_H, 0, 0, SG_C, SG_B, SG_C, SG_B, SG_B,
	0, 0, 0, 0, SG_B, SG_B, SG_B, SG_C,
	SG_B, 0, SG_D, SG_E, 0, 0, 0, 0
};
static const char audioIndexTables[] = {
	CDA_ARK_SINGLE_START, CDA_ARK_SINGLE_PLAY,
	CDA_ARK_MULTI_READY, CDA_ARK_MULTI_START, CDA_ARK_MULTI_PLAY,

	CDA_BLD_SINGLE_START, CDA_BLD_SINGLE_PLAY,
	CDA_BLD_MULTI_READY, CDA_BLD_MULTI_START, CDA_BLD_MULTI_PLAY,

	CDA_DRM_SINGLE_START, CDA_DRM_SINGLE_PLAY,
	CDA_DRM_MULTI_READY, CDA_DRM_MULTI_START, CDA_DRM_MULTI_PLAY
};
static POWER_SELECT powerSelectTables[] = {
/*0*/	"45/SECOND/SHIELD",	5,	0,	1,
/*1*/	"LITE/DEATHWATCH",	5,	0,	0,
/*2*/	"5/MILLION",		10,	0,	0,
/*3*/	"20/MILLION",		10,	0,	0,
/*4*/	"RISE/JACKPOT",		10,	0,	0,
/*5*/	"LONGER/TIMERS",	5,	0,	1,
/*6*/	"2X/PLAYFIELD",		5,	0,	1,
/*7*/	"MAX/MULTIPLIER",	5,	0,	1,
/*8*/	"SUPER/JETS",		5,	0,	1,
/*9*/	"KICKBACK",		10,	0,	1,
/*10*/	"SUPER/KICKBACK",	3,	0,	1,
/*11*/	"START/LIT ROUND",	20,	0,	1,
/*12*/	"LITE/EXTRABALL",	2,	0,	0,
/*13*/	"BONUS/HOLD",		5,	0,	1
};
static const char diagMysteryTables[] = {
	INFO_DIAG_MYSTERY_READY,
	INFO_DIAG_MYSTERY_READY,
	INFO_DIAG_MYSTERY_READY
};
static const char diagDoubleTables[] = {
	INFO_DIAG_DOUBLE_ARK,
	INFO_DIAG_DOUBLE_CLT,
	INFO_DIAG_DOUBLE_DRM
};
static const char *mysteryLitText[] = {
	"45 SECOND SHIELD",
	"DEATHWATCH LIT",
	"5 MILLION",
	"20 MILLION",
	"JACKPOT RISEN",
	"LONGER TIMERS",
	"2X PLAYFIELD",
	"MAX MULTIPLIER",
	"SUPER JETS",
	"KICKBACK",
	"SUPER KICKBACK",
	"START ROUND",
	"EXTRABALL LIT",
	"BONUS HELD",
	"HANGMAN FEATURES"
};
static const char *mysteryLitText2[] = {
	"45 SECOND/SHIELD",
	"DEATHWATCH/LIT",
	"5 MILLION",
	"20 MILLION",
	"JACKPOT/RISEN",
	"LONGER/TIMERS",
	"2X/PLAYFIELD",
	"MAX/MULTIPLIER",
	"SUPER/JETS",
	"KICKBACK/LIT",
	"SUPER/KICKBACK",
	"START/ROUND",
	"EXTRABALL/LIT",
	"BONUS/HELD",
	"HANGMAN/FEATURES"
};
static const char infoDiagRounds[] = {
/* Arkham */
	INFO_DIAG_DEATH_READY,
	INFO_DIAG_WITCH_READY,
	INFO_DIAG_ASYLUM_READY,
	INFO_DIAG_CEME_READY,
	INFO_DIAG_SOUL_READY,
	INFO_DIAG_CREA_READY,
/* Bloody */
	INFO_DIAG_ANCIENT_READY,
	INFO_DIAG_SOUL_READY,
	INFO_DIAG_ECLIPS_READY,
	INFO_DIAG_ANGELS_READY,
	INFO_DIAG_MACNE_READY,
	INFO_DIAG_GOD_READY,
/* Dream Lands */
	INFO_DIAG_DREAMER_READY,
	INFO_DIAG_SOUL_READY,
	INFO_DIAG_SILVER_READY,
	INFO_DIAG_WOOD_READY,
	INFO_DIAG_MOON_READY,
	INFO_DIAG_NECRO_READY
};
static const uint8_t roundLiteIndex[] = {
	LITE_ARK_ROUND_DEATH_WALKER_READY,
	LITE_ARK_ROUND_LITE_WITCHCRAFT_READY,
	LITE_ARK_ROUND_ARKHAM_ASYLUM_READY,
	LITE_ARK_ROUND_CEMETERY_READY,
	LITE_ARK_ROUND_SOUL_READY,

	LITE_CLT_ROUND_LITE_ANCIENT_READY,
	LITE_CLT_ROUND_SOUL_READY,
	LITE_CLT_ROUND_ECLIPSE_READY,
	LITE_CLT_ROUND_DARK_ANGELS_READY,
	LITE_CLT_ROUND_MACHINE_READY,

	LITE_DRM_ROUND_LITE_DREAMER_S_READY,
	LITE_DRM_ROUND_SOUL_READY,
	LITE_DRM_ROUND_SILVER_GATE_READY,
	LITE_DRM_ROUND_ENCHANTED_WOOD_READY,
	LITE_DRM_ROUND_MOONBEAST_READY
};
static const char kickbackLiteIndex[] = {
	LITE_ARK_STANDUP_TARGET_1,
	LITE_ARK_STANDUP_TARGET_2,
	LITE_ARK_STANDUP_TARGET_3,

	LITE_CLT_STANDUP_TARGET_4,
	LITE_CLT_STANDUP_TARGET_5,
	LITE_CLT_STANDUP_TARGET_6,

	LITE_DRM_STANDUP_TARGET_9,
	LITE_DRM_STANDUP_TARGET_8,
	LITE_DRM_STANDUP_TARGET_7
};
static const char multiReadyLiteIndex[] = {
	0,
	LITE_CLT_MULTIBALL_READY,
	LITE_DRM_MULTIBALL_READY
};
static const char loopBonusTables[] = {
/* 4 */	SCORE_001M,
/* 5 */	SCORE_002M,
/* 6 */	SCORE_003M,
/* 7 */	SCORE_004M,
/* 8 */	SCORE_005M,
/* 9 */	SCORE_010M,
/*10 */	SCORE_020M,
/*11 */	SCORE_050M
};
static const char soulScoreTables[] = {
/* 0 */	SCORE_005M, SCORE_010M,
/* 1 */	SCORE_010M, SCORE_020M,
/* 3 */	SCORE_030M, SCORE_060M
};
static const char soulAnimeTables[] = {
/* 0 */	INFO_ANIME_5M,  INFO_ANIME_10M, 
/* 1 */	INFO_ANIME_10M, INFO_ANIME_20M,
/* 2 */	INFO_ANIME_30M, INFO_ANIME_60M
};
static const char roundEndLiteTables[] = {
	LITE_ARK_LEFTLANE_JACKPOT,
	LITE_ARK_RIGHTLANE_JACKPOT,
	LITE_ARK_LEFTRAMP_JACKPOT,
	LITE_ARK_RIGHTRAMP_JACKPOT,
	LITE_ARK_DROP_TARGET_1,
	LITE_ARK_DROP_TARGET_2,
	LITE_ARK_DROP_TARGET_3,
	-1,

	LITE_CLT_LEFTLANE_JACKPOT,
	LITE_CLT_RIGHTLANE_JACKPOT,
	LITE_CLT_LEFTRAMP_JACKPOT,
	LITE_CLT_RIGHTRAMP_JACKPOT,
	LITE_CLT_CENTER_SAUCER_JACKPOT,
	LITE_CLT_LEFT_SAUCER_JACKPOT,
	-1,
	-1,

	LITE_DRM_LEFTLANE_JACKPOT,
	LITE_DRM_RIGHTLANE_JACKPOT,
	LITE_DRM_LEFTRAMP_JACKPOT,
	LITE_DRM_RIGHTRAMP_JACKPOT,
	LITE_DRM_MOON_HOLE_JACKPOT,
	LITE_DRM_CENTER_HOLE_JACKPOT,
	LITE_DRM_FIRESPITTER_JACKPOT,
	-1
};
static const char *testPlayString[] = {
	"DEATHWALKER",
	"CEMETERY",
	"ARKHAM ASYLUMK",
	"SOUL",
	"LITE WITCHCRAFT",
	"CREATION",
	"3 MULTI BALL",
	"7 MULTI BALL",

	"LITE ANCIENT",
	"SOUL",
	"ECLIPSE",
	"DARK ANGELS",
	"MACHINE",
	"GOD OF DESTRUCTION",
	"3 MULTI BALL",
	"7 MULTI BALL",

	"LITE DREAMER",
	"SOUL",
	"SILVER GATE",
	"ENCHANTED WOOD",
	"MOONBEAST",
	"NECRONOMICON",
	"3 MULTI BALL",
	"7 MULTI BALL"
};
static const char roundInfoIndex[] = {
	2, 3, 1, 0, 4, 5,
	4, 3, 2, 1, 0, 5,
	3, 4, 2, 0, 1, 5
};
static char testPlayNo;
static const char bonusMultiTables[] = { 1, 2, 3, 4, 5, 6, 7, 10 };
static const char *bonusMultiTexts[] = {
	"1 X", "2 X", "3 X", "4 X", "5 X", "6 X", "7 X", "10 X"
};
static unsigned char lightRedOff[50], lightGreenOff[50], lightBlueOff[50];
static unsigned char lightRedDelta[50], lightGreenDelta[50], lightBlueDelta[50];
static unsigned char lightIntenVal[50], lightIntenMode[50];
static char lightCommandSlave[50];
static char *mazeDiagScorePtr;
static char rampScore[12], rampDiagScore[16], mazeDiagScore[16];
static const char *mysteryDiagTextPtr;
static char *rampDiagNextPtr, *rampDiagScorePtr;
static char rampDiagNextExtra, rampDiagNextMystery;
static short generalMode;
static short powerItemList[3];
static short lightAnimeTime, lightBlinkTime, lightRollTime;
static short psychoSaveDelay;
static short freezeTimer;
static short topLaneComp;
static short standUpComp;
static short deathWatchComp;
static short kickbackTimer;
static short jetAmbient;
static short diagTimer;
static short wisdomSpell, wisdomCount;
static short multiScoreTimer;
static short commingTimer;
static unsigned int lightRollFlagA[50], lightRollFlagB[50];
static unsigned int lightSearchFlagA[50], lightSearchFlagB[50];
static const char arrowJackpotLiteTables[] = {
	LITE_ARK_LEFTLANE_JACKPOT,	0,
	LITE_ARK_RIGHTLANE_JACKPOT,	1,
	LITE_ARK_LEFTRAMP_JACKPOT,	2,
	LITE_ARK_RIGHTRAMP_JACKPOT,	3,
	-1,				-1,
	-1,				-1,

	LITE_CLT_LEFTLANE_JACKPOT,	8,
	LITE_CLT_RIGHTLANE_JACKPOT,	9,
	LITE_CLT_LEFTRAMP_JACKPOT,	10,
	LITE_CLT_RIGHTRAMP_JACKPOT,	11,
	LITE_CLT_CENTER_SAUCER_JACKPOT,	13,
	LITE_CLT_LEFT_SAUCER_JACKPOT,	14,

	LITE_DRM_LEFTLANE_JACKPOT,	15,
	LITE_DRM_RIGHTLANE_JACKPOT,	16,
	LITE_DRM_LEFTRAMP_JACKPOT,	17,
	LITE_DRM_RIGHTRAMP_JACKPOT,	18,
	LITE_DRM_MOON_HOLE_JACKPOT,	20,
	LITE_DRM_CENTER_HOLE_JACKPOT,	21
};

int ChooseMysteryItem(void)
{
	int	i, j, k, m;

	j = 0;
	for(i=0; i<POWER_ITEM_MAX; ++i)
	{
	if(powerSelectTables[i].flag==0)
		j += powerSelectTables[i].rate;
	}
	if(j==0) return -1;
	m = Rand() % j;
	j = 0;
	for(i=0; i<POWER_ITEM_MAX; ++i)
	{
	if(powerSelectTables[i].flag==0) {
		k = j;
		j += powerSelectTables[i].rate;
		if(m >= k && m < j) return i;
		}
	}
	return -1;
}

void CreatePowerSelectList(PLAYER *ply, int count)
{
	int	i, j;

	if(ply->extraBallCount >= 3) powerSelectTables[12].flag = 1;
	if(ply->roundStartReady) powerSelectTables[11].flag = 1;
/*	if(ply->multiBallReady) powerSelectTables[11].flag = 1; */
	if(ply->kickbackReady) powerSelectTables[9].flag = 1;
	if(ply->superKickbackReady) {
	powerSelectTables[9].flag = 1;
	powerSelectTables[10].flag = 1;
	}		
	for(i=0; i<count; ++i)
	{
	j = ChooseMysteryItem();
	if(j < 0) j = 3;				/* Choose 5 Milion */
	else if(j==2) powerSelectTables[3].flag = 1;	/* Finded 5 Mil */
	else if(j==3) powerSelectTables[2].flag = 1;	/* Finded 20 Mil */
	else if(j==9) powerSelectTables[10].flag = 1;	/* Finded Super Kickback */
	else if(j==10) powerSelectTables[9].flag = 1;	/* Finded Unlimited Kickback */
	powerSelectTables[j].flag = 1;
	powerItemList[i] = j;
	}
	for(i=0; i<POWER_ITEM_MAX; ++i)
		if(powerSelectTables[i].flag > 0) powerSelectTables[i].flag = 0;
}

void ServiceMysteryItem(PLAYER *ply, int no)
{
	switch(no)
	{
	case 0:
		freezeTimer = frameRate * 45;
		break;
	case 1:
		ply->deathWatchReady++;
		break;
	case 2:
		AddScore(ply->score, scoreConst[SCORE_005M]);
		break;
	case 3:
		AddScore(ply->score, scoreConst[SCORE_020M]);
		break;
	case 4:
		AddScore(ply->jackpot, scoreConst[SCORE_0025M]);
		if(CmpScore(ply->jackpot, scoreConst[SCORE_100M])==0)
			PresetScore(ply->jackpot, scoreConst[SCORE_100M]);
		break;
	case 5:
		ply->longerTimerEnable = 1;
		break;
	case 6:
		ply->playField2xEnable = 1;
		break;
	case 7:
		ply->bonusMultiCount = 7;
		break;
	case 8:
		ply->superJetsEnable = 1;
		break;
	case 9:
		AddKickback(ply);
		break;
	case 10:
		ply->superKickbackReady = 1;
		break;
	case 11:
		LaunchRound(ply);
		break;
	case 12:
		ply->extraBallReady++;
		break;
	case 13:
		ply->bonusHoldEnable = 1;
		break;
	default:
		break;
	}
}

void TakeMystery(PLAYER *ply, int f)
{
	int	i, j;

	powerSelectTables[11].flag = 1;		/* Disable "ROUND START" */
	powerSelectTables[0].flag = 1;		/* Disable "45 Sec."     */
	CreatePowerSelectList(ply, 1);
	i = powerItemList[0];
	if(powerSelectTables[i].m) powerSelectTables[i].flag = -1;
	if(f)	{
		infoText[1] = mysteryLitText[i];	
		SetMultiInfo(INFO_ANIME_MYSTERY, 5);
		}
	else	mysteryDiagTextPtr = mysteryLitText2[i];	
	ServiceMysteryItem(ply, i);
	RingSfx(SFX_ARK_WITCHCRAFT_AWD);
}

void ResetScore(char *dst)
{
	int	i;

	for(i=0; i<SCORE_MAX; ++i) *dst++ = 0;
}

void PresetScore(char *dst, char *src)
{
	int	i;

	for(i=0; i<SCORE_MAX; ++i) *dst++ = *src++;
}

int AddScore(char *dst, char *src)
{
	int	i, c;

	c = 0;
	for(i=0; i<SCORE_MAX; ++i) {
	c += *dst + *src;
	if( c >= 10 ) { *dst = c - 10; c = 1; }
	else { *dst = c; c = 0; }
	src++;
	dst++;
	}
	if(c)	{
		dst -= SCORE_MAX;
		dst++;
		for(i=0; i<(SCORE_MAX-1); ++i) *dst++ = 9;
		}
	return c;
}

int SubScore(char *dst, char *src)
{
	int	i, c;

	c = 0;
	for(i=0; i<SCORE_MAX; ++i) {
	c += *dst - *src;
	if(c < 0) { *dst = c + 10; c = -1; }
	else { *dst = c; c = 0; }
	src++;
	dst++;
	}
	return c;
}

int CmpScore(char *dst, char *src)
{
	int	i, c;

	c = 0;
	for(i=0; i<SCORE_MAX; ++i) {
	c += *dst - *src;
	if(c < 0) { c = -1; }
	else { c = 0; }
	src++;
	dst++;
	}
	return c;
}
unsigned int ScoreToInterger(char *src)
{
	int	i;
	unsigned int	j, k;

	j = 0;
	k = 1;
	for(i=0; i<12; ++i) {
	j += ( *src++ ) * k;
	k *= 10;
	}
	return j;
}

void IntergerToScore(char *dst, unsigned int src)
{
	int	i;

	for(i=0; i<12; ++i)
	{
	*dst++ = src % 10;
	src /= 10;
	}
}

void StartMusic(int no)
{
	if(optionMenuPad[OPTION_NO_BGM]==0) return;
	PlayCD(audioIndexTables[no * 5 + 0], audioIndexTables[no * 5 + 1]);
}

void SetupSoundGroup(int no)
{
	SetSoundGroup(gameSoundGroup, no);
}

void InitGeneralRule(int no)
{
	PLAYER	*ply;
	int	i, j, k;
	char	*s, *d;

	playerNo = 0;
	for(i=0; i<2; ++i) {
	ply = &playerTab[i];
	ply->ballMax = 3;
	ply->tableNo = no;
	ply->holeSelectorEnable = 0;
	if(ply->tableNo==0) InitArkham(ply);
	else if(ply->tableNo==1) InitBloody(ply);
	else if(ply->tableNo==2) InitDreaml(ply);
	ply++;
	}
	InitLightServer();

	SetSoundGroup(gameSoundGroup, ply->tableNo);

	InitGameValues();

	trialTimer = 100 * frameRate;
	generalMode = GMODE_GAME_INIT;
}

void InitConstScore(void)
{
	int i, j, k;
	char *s, *d;

	i = 0;
	while(scoreConstSrc[i] != NULL) {
		memcpy(&scoreConstPad[i * 12], scoreConstSrc[i], 12);
		scoreConst[i] = &scoreConstPad[i * 12];
		++i;
	}
	i = 0;
	while(scoreConst[i] != NULL) {
	s = scoreConst[i];
//	if(s[12]==0) {
//		s[12] = 1;
		d = &s[11];
		for(j=0; j<6; ++j) {
			k = *s;
			*s = *d - '0';
			*d = k - '0';
			s++;
			d--;
			}
//		}
	i++;
	}
}

static void InitGameValues(void)
{
	int	i, j;
	PLAYER	*ply;

	TurnIntoDark(256);
	vibrEnable = 0;
	playerNo = 0;
	nextStageEnable = 0;
	commingTimer = 0;
	gameMode = 0;
	playerSwap = 0;

	ply = &playerTab[0];

	InitInfomations(ply->tableNo);

	for(j=0; j<2; ++j) {
	if(ply->tableNo==0) InitArkham(ply);
	else if(ply->tableNo==1) InitBloody(ply);
	else if(ply->tableNo==2) InitDreaml(ply);
	ply->holeSelectorEnable = 0;
	ResetScore( ply->score );
	PresetScore(rampScore, scoreConst[SCORE_001M]);
	PresetScore(ply->outHoleBonus, scoreConst[SCORE_001M]);
	for(i=0; i<3; ++i) ply->topLaneFlag[i] = 0;
	ply->bonusMultiCount = 0;
	if(trialEnable | geEnable) ply->ballInPlay = 3;
	else ply->ballInPlay = 1;
	ply->rampShotCount = 0;
	ply->extraBallCount = 0;
	ply->extraBallCycle = 0;
	ply->multiBallCompFlag = 0;
	ply->multiBallReady = 0;
	ply->superRoundEnable = 0;
	ply->multiBallRestart = 0;
	ply->extraBallReady = 0;
	ply->mysteryReady = 0;
	ply->roundStartReady = 0;
	ply->deathWatchReady = 0;
	ply->mazeValueCount = 0;
	ply->kickbackCompCount = 0;
	ply->roundGoNo = 0;
	ply->multiBallGo = 0;
	ply->vukModeDelay = 0;
	soulTask = 0;
	for(i=0; i<4; ++i) ply->mazeValueFlag[i] = 0;
	for(i=0; i<8; ++i) ply->roundCompFlag[i] = 0;
	ply->roundNumber = (unsigned int)Rand() % 5;
	ply++;
	}
	roundTimer = roundStartDelay = roundDelay = 0;
	multiBallReadyOld = 0;
	multiBallGoOld = 0;
	roundGoNoOld = 0;
	InitVibration();
}

int ServiceGeneralRule(void)
{
	int	i, j, k, f;
	char	*ptr, sct[12];
	PLAYER	*ply;

	Rand();

	f = 0;

	ply = &playerTab[playerNo];

	ServiceBGM(ply);

	switch(generalMode)
	{
	case GMODE_GAME_INIT:
		InitGameValues();
		generalMode = GMODE_PLAY_INIT;
		break;
	case GMODE_PLAY_INIT:
		InitVibration();
		InitDiags();
		vibrEnable = 0;
		dangerFlag = 0;
		dangerCount = 0;
		tiltFlag = 0;
		tiltGo = 0;
		j = ply->tableNo * 8;
		for(i=0; i<8; ++i) {
		k = roundEndLiteTables[j];
		if(k >= 0) lightCommand[k] = LIGHT_TURN_OFF;
		j++;
		}
		for(i=0; i<4; ++i) diagInfoTab[i] = -1;
		commingTimer = 0;
		loopBonusTimer = 0;
		soulTask = 0;
		multiScoreTimer = 0;
		psychoSaveDelay = 0;
		multiBallReadyOld = 0;
		multiBallGoOld = 0;
		roundGoNoOld = 0;
		wisdomCount = wisdomSpell = 0;
		ply->vukModeDelay = 0;
		ply->nameInPos = 0;
		ply->loopBonusCount = 0;
		ply->bonusHoldEnable = 0;
		ply->multiBallCount = 0;
		ply->ballSaveAdd = 0;
		ply->ballSaveLimit = 1;
		if(ply->tableNo==0) InitArkhamPlay(ply);
		else if(ply->tableNo==1) InitBloodyPlay(ply);
		else if(ply->tableNo==2) InitDreamlPlay(ply);
		for(i=0; i<POWER_ITEM_MAX; ++i) powerSelectTables[i].flag = 0;
		if(ply->ballInPlay==3 && ply->multiBallCompFlag==0) {
			if(ply->tableNo==0) DoArkhamMultiBallReady(ply);
			else if(ply->tableNo==1) DoBloodyMultiBallReady(ply);
			else if(ply->tableNo==2) DoDreamMultiBallReady(ply);
		}
		ply->multiBallEnable = 1;
		ply->ballSaveFlag = 0;
		ply->switchTotal = 0;
		ply->kickbackEnable = 0;
		ply->superJetsEnable = 0;
		ply->playField2xEnable = 0;
		ply->longerTimerEnable = 0;
		ply->kickbackReady = 1;
		ply->superKickbackReady = 0;
		ply->roundGoNo = 0;
		ply->multiBallGo = 0;
		for(i=0; i<8; ++i) ply->dropTargetEnable[i] = 1;
/*		for(i=0; i<8; ++i) ply->standUpFlag[i] = 0;
		for(i=0; i<8; ++i) ply->dropTargetFlag[i] = 0;
*/
		ResetScore(ply->roundScore);
		PresetScore(ply->deathWatchScore, scoreConst[SCORE_005M]);
		topLaneComp = 0;
		mazeValueComp = 0;
		standUpComp = 0;
		deathWatchComp = 0;
		freezeTimer = 0;
		kickbackTimer = 0;
		if(optionMenuPad[OPTION_NO_PLAYER]) {
			TurnIntoDark(256);
			SetInfomations(INFO_ANIME_PLAYER, 10);
			generalMode = GMODE_PLAYER_READY;
			}
		else	generalMode = GMODE_SHOOT_READY;
		break;
	case GMODE_SHOOT_READY:
		if(demoPlayEnable) ply->shooterEnable = 1;
		else ply->shooterEnable = 0;
		if(AddPlayerBall()) {
		shoottingCounter = 0;
		CreatePowerSelectList(ply, 2);
		if(ply->tableNo==0) i = INFO_ANIME_POWER_SELECT;
		else i = INFO_ANIME_POWER_SEL2; 
		k = powerItemList[0];
		infoText[0] = powerSelectTables[k].txt;
		k = powerItemList[1];
		infoText[1] = powerSelectTables[k].txt;
		SetInfomations(i, 10);
		generalMode = GMODE_POWER_SELECT;
		}
		break;
	case GMODE_POWER_SELECT:
		vibrEnable = 1;
		if(tiltFlag) {
			SetInfomations(INFO_ANIME_TILT, 11);
			generalMode = GMODE_TURN_TILT;
			tiltGo = 1;
			}
		if((gamePadTrigger & SATURN_PAD_Z)!=0 && debugEnable!=0)
			CreatePowerSelectList(ply, 1);
		k = powerItemList[0];
		infoText[0] = powerSelectTables[k].txt;
		k = powerItemList[1];
		infoText[1] = powerSelectTables[k].txt;

		if(gamePadTrigger & shooterSw) { 
		lightAnimePtr = liteBLK2;
		freezeTimer = frameRate * 20;
		i = powerItemList[infoValue];
		infoText[0] = mysteryLitText[i];	
		SetInfomations(INFO_ANIME_POWER_TITLE, 10);
		ServiceMysteryItem(ply, i);
		if(powerSelectTables[i].m) powerSelectTables[i].flag = -1;
		generalMode = GMODE_SHOOTER_WAIT;
		}
		else if(tiltFlag) {
		ply->shooterEnable = 1;
		generalMode = GMODE_PLAY_GAME;
		}
		break;
	case GMODE_PLAY_GAME:
		if((gamePadTrigger & SATURN_PAD_Z)!=0 && debugEnable!=0)
			nextStageEnable = 1;
		ServiceBallSaver(ply);
		if(tiltFlag) {
			ply->vukEnable = 1;
			SetInfomations(INFO_ANIME_TILT, 11);
			generalMode = GMODE_TURN_TILT;
			tiltGo = 1;
			}
		else if(dangerFlag) {
			SetInfomations(INFO_DIAG_DANGER, 1);
			dangerFlag = 0;
			}
		if(realmsEnable!=0 && nextStageEnable!=0
			&& ply->roundGoNo==0 && ply->multiBallGo==0) {
			if(ply->tableNo==0) i = DIVERTER2_TRUE;
			else i = DIVERTER1_TRUE;
			ply->gateEnable[i] = 0;
			ply->gateEnable[i+1] = 1;
			if(ply->nextStageOn) {
				if(ply->tableNo==0) i = INFO_ANIME_COMPLETED;
				else i = INFO_ANIME_COMPLETED2;
				commingTimer = 2976;
				SetInfomations(i, 5);
				generalMode = GMODE_NEXT_STAGE;
				}
			} else {
			if(ply->tableNo==0) i = DIVERTER2_TRUE;
			else i = DIVERTER1_TRUE;
			ply->gateEnable[i] = 1;
			ply->gateEnable[i+1] = 0;
			}
		if(trialEnable | geEnable) {
		if(trialTimer) trialTimer--;
		else	{
			commingTimer = 2976;
			SetInfomations(INFO_ANIME_SOON, 11);
			generalMode = GMODE_COMING_SOON;
			}
		}
		break;
	case GMODE_BALL_LOST:
		if(trialEnable | geEnable) {
		commingTimer = 2976;
		SetInfomations(INFO_ANIME_SOON, 11);
		generalMode = GMODE_COMING_SOON;
		}
		else if(infoNo < 0) {
		AddScore(ply->score, roundValue);
		if(ply->bonusHoldEnable==0) {
			ply->bonusMultiCount = 0;
			PresetScore(ply->outHoleBonus, scoreConst[SCORE_001M]);	
			}
		lightAnimePtr = liteLTA;
		if(ply->extraBallCycle) generalMode = GMODE_EXTRA_BALL;
		else if(ply->ballInPlay >= ply->ballMax) {
			if(realmsEnable) {
				SetInfomations(INFO_ANIME_GAMEOVER, 10);
				generalMode = GMODE_GAME_OVER;
				}
			else if((infoNumber = InstallScore(ply)) <= 5) { 
				nameTextPos = 0;
				nameText[0] = 'A';
				commingTimer = 2976;
				SetInfomations(INFO_ANIME_NAME_ENTRY, 10);
				generalMode = GMODE_NAME_ENTRY;
				}
			else	{
				replayTimer = 10 * frameRate;
				infoText[0] = "SEARCH AGAIN #";
				infoNumber = replayTimer / frameRate;
				SetInfomations(INFO_ANIME_REPLAY, 10);
				generalMode = GMODE_GAME_REPLAY;
				}
			}
		else	{
			ply->ballInPlay++;
			ply->extraBallCount = 0;
			ply->extraBallCycle = 0;
		if(optionMenuPad[OPTION_NO_PLAYER]!=0 && trialEnable==0 && geEnable==0)
				playerSwap = 1;
			generalMode = GMODE_PLAY_INIT;
			}
		}
		else {
		if(vsyncCounter & 8) RingSfx(SFX_SACRIFICIAL_DRUMS);
		}
		break;
	case GMODE_EXTRA_BALL:
		ply->extraBallCycle--;
		RingSfx(SFX_MILLION_GRAIL);
		SetInfomations(INFO_ANIME_AGAIN, 10);
		generalMode = GMODE_EXTRA_BALL2;
		break;
	case GMODE_EXTRA_BALL2:
		if(infoNo < 0) {
		generalMode = GMODE_PLAY_INIT;
		}
		break;
	case GMODE_GAME_OVER:
		if(infoNo < 0) f = -1;
		PlayCD(-1, -1);
		break;
	case GMODE_SHOOTER_WAIT:
		if(infoNo < 0) {
		ply->shooterEnable = 1;
		generalMode = GMODE_PLAY_GAME;
		}
		break;
	case GMODE_NEXT_STAGE:
		if(infoNo < 0) f = 1;
		PlayCD(-1, -1);
		break;
	case GMODE_COMING_SOON:
		if(geEnable) infoText[6] = "NOW ON SALE";
		else infoText[6] = "COMING SOON";
		infoText[7] = "NECRONOMICON";
		if(infoNo < 0) f = -1;
		PlayCD(-1, -1);
		break;
	case GMODE_PLAYER_READY:
		if(infoNo < 0) generalMode = GMODE_SHOOT_READY;
		break;
	case GMODE_NAME_ENTRY:
		infoText[7] = "TOP % EXPLORER";
		CreateInfoScore(ply->score);
		if(ServiceNameEntry(ply)) {
			SetInfomations(INFO_ANIME_NAME_ENTRY_F, 10);
			generalMode = GMODE_NAME_DELAY;
			}
		break;
	case GMODE_NAME_DELAY:
		if(infoNo < 0) {
		replayTimer = frameRate * 10;
		infoText[0] = "SEARCH AGAIN #";
		infoNumber = replayTimer / frameRate;
		SetInfomations(INFO_ANIME_REPLAY, 10);
		generalMode = GMODE_GAME_REPLAY;
		}
		break;
	case GMODE_GAME_REPLAY:
		if(playerNo==0 && optionMenuPad[OPTION_NO_PLAYER]!=0) {
		TurnIntoDark(256);
		playerSwap = 1;
		generalMode = GMODE_PLAY_INIT;
		break;
		}
		infoText[0] = "SEARCH AGAIN #";
		infoNumber = replayTimer / frameRate;
		if(replayTimer==0) {
		RingSfx(SFX_OUT_LANE);
		SetInfomations(INFO_ANIME_GAMEOVER, 10);
		generalMode = GMODE_GAME_OVER;
		}
		else if(gamePadTrigger & SATURN_PAD_START) {
		StartMusic(ply->tableNo);
		generalMode = GMODE_GAME_INIT;
		}
		if(gamePadTrigger & SATURN_PAD_ABCXYZ) {
		i = replayTimer / frameRate;
		replayTimer = i * frameRate;
		}
		if(replayTimer) replayTimer--;
		break;
	case GMODE_TURN_TILT:
		ply->vukEnable = 1;
		ResetScore(roundValue);
		ResetScore(ply->roundScore);
		ResetScore(ply->outHoleBonus);
		if(ply->multiBalls==0 && ply->roundGoNo==0 && ply->multiBallGo==0) {
			SetInfomations(INFO_ANIME_CLEAR, 11);
			generalMode = GMODE_BALL_LOST;
			}
		break;
	default:
		break;
	}

	ResetScore(sct);

	if(debugFlag) ServiceTestRule(ply);
	if(ply->tableNo==0) ServiceArkham(ply, sct);
	else if(ply->tableNo==1) ServiceBloody(ply, sct);
	else if(ply->tableNo==2) ServiceDreaml(ply, sct);

	if(ply->tableNo) {
	i = multiReadyLiteIndex[ply->tableNo];
	if(ply->multiBallEnable!=0
		&& (ply->multiBallReady | ply->multiBallRestart)!=0) 
		lightCommand[i] = LIGHT_TURN_BLINK;
	else	lightCommand[i] = LIGHT_TURN_OFF;
	}

	ServiceEtc(ply, sct);
	ServiceKickback(ply, sct);
	ServiceMazeSpell(ply, sct);
	ServiceRampShot(ply, sct);
	ServiceEtc2(ply, sct);

	ServiceRoundIndicator(ply);

/*---------------------------------------------------------*/
	AddScore(ply->score, sct);
	if(ply->playField2xEnable) AddScore(ply->score, sct);

	for(i=0; i<8; ++i) ply->topLaneOn[i] = 0;
	for(i=0; i<4; ++i) ply->holeOn[i] = 0;
	for(i=0; i<4; ++i) ply->saucerOn[i] = 0;
	for(i=0; i<2; ++i) ply->outLaneOn[i] = 0;
	for(i=0; i<2; ++i) ply->returnLaneOn[i] = 0;
	for(i=0; i<2; ++i) ply->laneEnterOn[i] = 0;
	for(i=0; i<2; ++i) ply->laneRollOverOn[i] = 0;
	for(i=0; i<4; ++i) ply->rampRollOverOn[i] = 0;
	for(i=0; i<8; ++i) ply->dropTargetOn[i] = 0;
	for(i=0; i<8; ++i) ply->standUpOn[i] = 0;
	for(i=0; i<2; ++i) ply->kickbackOn[i] = 0;
	for(i=0; i<2; ++i) ply->vukOn[i] = 0;
	for(i=0; i<2; ++i) ply->suspensionRollOverOn[i] = 0;
	ply->nextStageOn = 0;
	ply->switchFrame = 0;

	ServiceDiag(ply);
	ServiceJackpotArrow(ply);

	if(commingTimer) {
	if(realmsEnable!=0 && nextStageEnable!=0) {
	if((vsyncCounter & 7)==0 && cdFadeLevel!=0) cdFadeLevel--;
		}
	if((vsyncCounter % 3)==0) TurnIntoDark((commingTimer >> 4) + 70);
	commingTimer -= frameRate / 3;
	if(commingTimer < 1) commingTimer = 1;
	}
	else	{
		if(tiltGo) lightAnimePtr = liteTilt;
		ServiceLightServer();
		}

	if(generalMode==GMODE_PLAY_GAME) gameMode = 1; else gameMode = 0;

	return f;
}

static void ServiceEtc2(PLAYER *ply, char *sct)
{
	int	i, j, k;
	char	*ptr;

/*----------------------------------------------/
/	Out Hole Bonus / Death Watch Service	/
/----------------------------------------------*/
	j = ply->switchFrame;
	for(i=0; i<j; ++i) {
	AddScore(ply->outHoleBonus, scoreConst[SCORE_020K]);
	AddScore(ply->deathWatchScore, scoreConst[SCORE_050K]);
	}
	j = ply->rampRollOverOn[0] + ply->rampRollOverOn[1];
	j += ply->laneRollOverOn[0] + ply->laneRollOverOn[1];
	for(i=0; i<j; ++i) AddScore(ply->outHoleBonus, scoreConst[SCORE_100K]);	
	if(ply->outLaneOn[0]!=0 && ply->deathWatchReady!=0) {
		RingSfx(SFX_DEATHWATCH_AWARD);
		AddScore(sct, ply->deathWatchScore);
		deathWatchComp = frameRate * 2;
		ply->deathWatchReady--;
		}
	if(deathWatchComp) {
		lightCommand[LITE_DEATHWATCH_READY_RIGHT] = LIGHT_TURN_BLINK;
		deathWatchComp--;
	}
	else	{
	if(ply->deathWatchReady)
		lightCommand[LITE_DEATHWATCH_READY_RIGHT] = LIGHT_TURN_ON;
	else	lightCommand[LITE_DEATHWATCH_READY_RIGHT] = LIGHT_TURN_OFF;
	}

/*----------------------------------------------/
/	Top Lane / (Bonus Multiplier)		/
/----------------------------------------------*/
	if(gamePadTrigger & flipper[0].sw) {
		j = ply->topLaneFlag[0];
		ply->topLaneFlag[0] = ply->topLaneFlag[1];
		ply->topLaneFlag[1] = ply->topLaneFlag[2];
		ply->topLaneFlag[2] = j;
	}
	if(gamePadTrigger & flipper[3].sw) {
		j = ply->topLaneFlag[2];
		ply->topLaneFlag[2] = ply->topLaneFlag[1];
		ply->topLaneFlag[1] = ply->topLaneFlag[0];
		ply->topLaneFlag[0] = j;
	}
	j = 1;
	k = 0;
	for(i=0; i<3; ++i) {
	if(ply->topLaneOn[i]) {
		AddScore(sct, scoreConst[SCORE_TOP_LANE]);
		topLaneComp = 0;
		k++;
		}
	j &= ply->topLaneFlag[i];
	if(topLaneComp) {
		lightCommand[LITE_BONUS_MULTIPLIER_1+i] = LIGHT_TURN_BLINK;
		}
	else	{
		if(ply->topLaneFlag[i]) lightCommand[LITE_BONUS_MULTIPLIER_1+i] = LIGHT_TURN_ON;
		else lightCommand[LITE_BONUS_MULTIPLIER_1+i] = LIGHT_TURN_OFF;
		}
	}
	if(k) RingSfx(SFX_TOP_LANE);
	if(topLaneComp) topLaneComp--;
	if(j) {
	topLaneComp = frameRate * 2;
	for(i=0; i<3; ++i) ply->topLaneFlag[i] = 0;
	ply->bonusMultiCount++;
	if(ply->bonusMultiCount > 7) {
		ply->bonusMultiCount = 7;
		AddScore(ply->score, scoreConst[SCORE_005M]);
		}
	}
	for(i=0; i<7; ++i) {
	if(ply->bonusMultiCount > i)
		lightCommand[LITE_BONUS_MULTIPLIER_X2 + i] = LIGHT_TURN_ON;
	else lightCommand[LITE_BONUS_MULTIPLIER_X2 + i] = LIGHT_TURN_OFF;
	}

/*----------------------------------------------/
/		Jet Bumper			/
/----------------------------------------------*/
	j = 0;
	if(ply->superJetsEnable) ptr = scoreConst[SCORE_100K];
	else ptr = scoreConst[SCORE_JETS];
	for(i=0; i<4; ++i) { 
	if(ply->jetBumperOn[i]) {
		AddScore(sct, ptr);
		j++;
		}
	ply->jetBumperOn[i] = 0;
	}
	if(j)	{
		if(ply->superJetsEnable) RingSfx(SFX_SUPER_JET_BUMPER);
		else RingSfx(SFX_JET_BUMPER);
		if(jetAmbient==0) RingSfx(SFX_ARK_BUMPER);
		jetAmbient = frameRate * 2;
		}
	if(jetAmbient) {
		jetAmbient--;
		if(jetAmbient==0) RingSfx(SFX_JET_BUMPER_AMBIENT);
		}

/*----------------------------------------------/
/		Sling Shot			/
/----------------------------------------------*/
	j = 0;
	for(i=0; i<2; ++i) {
	if(ply->slingShotOn[i]) {
		AddScore(sct, scoreConst[SCORE_SLING]);
		j++;
		}
	ply->slingShotOn[i] = 0;
	}
	if(j)	{
		RingSfx(SFX_SLINGSHOT);
		RingSfx(SFX_ARK_SLINGSHOT);
		}
}

static void ServiceEtc(PLAYER *ply, char *sct)
{
	int	i;

/*----------------------------------------------/
/		Hole Service			/
/----------------------------------------------*/
	for(i=0; i<4; ++i)
	{
	if(ply->holeOn[i] | ply->saucerOn[i]) {
		AddScore(sct, scoreConst[SCORE_PIT_HOLE]);
		lightAnimePtr = liteWBT;
		}
	}

/*----------------------------------------------/
/		VUK Service			/
/----------------------------------------------*/
	if(ply->vukOn[0]) AddScore(sct, scoreConst[SCORE_VUK]);
	if(ply->vukOn[1]) AddScore(sct, scoreConst[SCORE_VUK]);

/*----------------------------------------------/
/		Drop Target Service		/
/----------------------------------------------*/
	for(i=0; i<8; ++i) 
		if(ply->dropTargetOn[i]) AddScore(sct, scoreConst[SCORE_DROP]);

/*----------------------------------------------/
/		Lane Rollover Service		/
/----------------------------------------------*/
	i = 0;
	if(ply->laneRollOverOn[0]) {
		AddScore(sct, scoreConst[SCORE_LANE]);
		i++;
		}
	if(ply->laneRollOverOn[1]) {
		AddScore(sct, scoreConst[SCORE_LANE]);
		i++;
		}
	if(i) RingSfx(SFX_ARK_LANE);

/*----------------------------------------------/
/		Stand Up Service		/
/----------------------------------------------*/
	for(i=0; i<8; ++i) 
		if(ply->standUpOn[i]) AddScore(sct, scoreConst[SCORE_STAND_UP]);
}

/*----------------------------------------------/
/	Round Indicator / Round Ready		/
/----------------------------------------------*/
static void ServiceRoundIndicator(PLAYER *ply)
{
	int	i, j, k;

	i = 0;
	if(ply->roundStartReady!=0 && ply->multiBallGo==0) i = 1;
	if(ply->tableNo!=0 && ply->multiBallReady!=0) i = 0;
	if(i) {
		if(ply->superRoundEnable)
			lightCommand[LITE_ROUND_START_READY] = LIGHT_TURN_BLINK;
		else	lightCommand[LITE_ROUND_START_READY] = LIGHT_TURN_ON;
		}
	else lightCommand[LITE_ROUND_START_READY] = LIGHT_TURN_OFF;

	j = 1;
	for(i=0; i<5; ++i) j &= ply->roundCompFlag[i];
	for(i=0; i<5; ++i) {
	k = roundLiteIndex[ply->tableNo * 5 + i];
	if(j!=0 && ply->roundGoNo==0)
			lightCommand[k] = LIGHT_TURN_BLINK;
	else if(ply->roundCompFlag[i])
			lightCommand[k] = LIGHT_TURN_ON;
	else	{
		if(ply->roundNumber==i && ply->roundCompFlag[i]==0)
			lightCommand[k] = LIGHT_TURN_BLINK;
		else	lightCommand[k] = LIGHT_TURN_OFF;
		}
	}

	if(ply->roundGoNo==0) {
	if(j) ply->roundNumber = 5;
	else	{
	if(ply->laneRollOverOn[0]) i = -1;
	else if(ply->laneRollOverOn[1]) i = 1;
	else i = 0;
	if(i)	{
	k = ply->roundNumber + i;
		for(j=0; j<5; ++j) {
			if(k >= 5) k = 0; else if(k < 0) k = 4;
			if(ply->roundCompFlag[k]==0) break;
			k += i;	}
			if(k >= 5) k = 0; else if(k < 0) k = 4;
			ply->roundNumber = k;
			}
		}
	}
}

void ScanNextRound(PLAYER *ply)
{
	int	i, j;

	j = ply->roundNumber;
	for(i=0; i<5; ++i) {
	j++;
	if(j >= 5) j = 0;
	if(ply->roundCompFlag[j]==0) {
		ply->roundNumber = j;
		return;
		}
	}
	if(ply->roundCompFlag[5]==0) {
	ply->roundNumber = 5;
	return;
	}
	for(i=0; i<6; ++i) ply->roundCompFlag[i] = 0;
	ply->roundNumber = (unsigned int)Rand() % 5;
}

/*----------------------------------------------/
/		Kickback Service		/
/----------------------------------------------*/
static void ServiceKickback(PLAYER *ply, char *sct)
{
	int	i, j;

	if(soulTask==0) {

	for(i=0; i<3; ++i) {
	j = kickbackLiteIndex[ply->tableNo * 3 + i];
	if(standUpComp)
		lightCommand[j] = LIGHT_TURN_BLINK;
	else	{
		if(ply->standUpFlag[i])
			lightCommand[j] = LIGHT_TURN_ON;
		else	lightCommand[j] = LIGHT_TURN_OFF;
		}
	}
	if(standUpComp) standUpComp--;

	for(i=0; i<3; ++i) if(ply->standUpOn[i]) ply->kickbackCompCount++;
	if(ply->kickbackCompCount >= 3) {
		ply->kickbackCompCount = 0;
		AddScore(sct, scoreConst[SCORE_SHOT_COMP]);
		standUpComp = frameRate * 2;
		AddKickback(ply);
		if(ply->multiBallGo) {
			if(ply->tableNo!=0)
				AddScore(ply->jackpot, scoreConst[SCORE_0025M]);
			if(CmpScore(ply->jackpot, scoreConst[SCORE_100M])==0)
				PresetScore(ply->jackpot, scoreConst[SCORE_100M]);
			}
		for(i=0; i<3; ++i) ply->standUpFlag[i] = 0;
		}
	}

	i = ply->kickbackReady | ply->superKickbackReady;
	if(i!=0 || kickbackTimer!=0) {
	ply->kickbackEnable = 1;
	}
	else ply->kickbackEnable = 0;

	if(kickbackTimer) kickbackTimer--;

	if(ply->superKickbackReady!=0 || kickbackTimer!=0) {
		lightCommand[LITE_KICKBACK_READY] = LIGHT_TURN_BLINK;
		}
	else if(ply->kickbackReady) {
		lightCommand[LITE_KICKBACK_READY] = LIGHT_TURN_ON;
		}
	else	{
		lightCommand[LITE_KICKBACK_READY] = LIGHT_TURN_OFF;
		}

	if(ply->kickbackOn[1]!=0 && ply->kickbackEnable!=0) {
	RingSfx(SFX_KICKBACK);
	lightAnimePtr = liteWBT;
	AddScore(sct, scoreConst[SCORE_KICKBACK]);
	if(ply->kickbackReady!=0 && ply->superKickbackReady==0)
		ply->kickbackReady--;
	kickbackTimer = frameRate * 5;
	}
}

/*----------------------------------------------/
/	Maze Spell (Value) Service		/
/----------------------------------------------*/
static void ServiceMazeSpell(PLAYER *ply, char *sct)
{
	int	i, j;

	if(gamePadTrigger & flipper[0].sw) {
		j = ply->mazeValueFlag[0];
		ply->mazeValueFlag[0] = ply->mazeValueFlag[1];
		ply->mazeValueFlag[1] = ply->mazeValueFlag[2];
		ply->mazeValueFlag[2] = ply->mazeValueFlag[3];
		ply->mazeValueFlag[3] = j;
	}
	if(gamePadTrigger & flipper[3].sw) {
		j = ply->mazeValueFlag[3];
		ply->mazeValueFlag[3] = ply->mazeValueFlag[2];
		ply->mazeValueFlag[2] = ply->mazeValueFlag[1];
		ply->mazeValueFlag[1] = ply->mazeValueFlag[0];
		ply->mazeValueFlag[0] = j;
	}
	if(ply->outLaneOn[1]) {
		RingSfx(SFX_OUT_LANE);
		AddScore(sct, scoreConst[SCORE_OUT_LANE]);
		ply->mazeValueFlag[0] = 1;
		}
	if(ply->returnLaneOn[1]) {
		AddScore(sct, scoreConst[SCORE_RETURN]);
		ply->mazeValueFlag[1] = 1;
		}
	if(ply->returnLaneOn[0]) {
		AddScore(sct, scoreConst[SCORE_RETURN]);
		ply->mazeValueFlag[2] = 1;
		}
	if(ply->outLaneOn[0]) {
		RingSfx(SFX_OUT_LANE);
		AddScore(sct, scoreConst[SCORE_OUT_LANE]);
		ply->mazeValueFlag[3] = 1;
		}
	j = 1;
	for(i=0; i<4; ++i)
	{
	j &= ply->mazeValueFlag[i];
	if(mazeValueComp) {
		lightCommand[LITE_M_MAZE_SPELL_1+i] = LIGHT_TURN_BLINK;
		}
	else	{
		if(ply->mazeValueFlag[i])
			lightCommand[LITE_M_MAZE_SPELL_1+i] = LIGHT_TURN_ON;
		else	lightCommand[LITE_M_MAZE_SPELL_1+i] = LIGHT_TURN_OFF;
		}
	}
	if(mazeValueComp) mazeValueComp--;
	if(j) {
	RingSfx(SFX_MAZE_AWARD);
	mazeValueComp = frameRate * 3;
	for(i=0; i<4; ++i) ply->mazeValueFlag[i] = 0;
	AddScore(sct, scoreConst[SCORE_001M + ply->mazeValueCount]);
	i = SCORE_001M + ply->mazeValueCount;
	mazeDiagScorePtr = CreateInfoScores(mazeDiagScore, scoreConst[i]);
	if(ply->mazeValueCount & 1) AddKickback(ply);
	ply->mazeValueCount++;
	if(ply->mazeValueCount >= 10) ply->mazeValueCount = 9;
	}
}

/*----------------------------------------------/
/		Ramp Shot Service		/
/----------------------------------------------*/
static void ServiceRampShot(PLAYER *ply, char *sct)
{
	int	i, j, f;

	i = ply->rampRollOverOn[0] + ply->rampRollOverOn[1];
	if(i) {
	RingSfx(SFX_RAMP);
	lightAnimePtr = liteBLK2;
	AddScore(sct, scoreConst[SCORE_RAMP]);
	j = ply->rampShotCount;
	ply->rampShotCount += i;

	f = 0;
	if(ply->rampShotCount == 29) { rampDiagNextExtra = 1; f = 1; }
	else if(j < 30 && ply->rampShotCount >= 30) {
	f = 1;
	rampDiagNextExtra = 0;
	RingSfx(SFX_EXTRABALL_LIT);
	ply->extraBallReady++;
	}

	j = ply->rampShotCount % 10;
	i = ply->rampShotCount / 10;
	if(j==0) {
	if(i & 1) {
		rampDiagNextPtr = NULL;
		if(f==0) {
		rampDiagScorePtr = CreateInfoScores(rampDiagScore, rampScore);
		AddScore(sct, rampScore);
			}
		AddScore(rampScore, scoreConst[SCORE_002M]);
		if(CmpScore(rampScore, scoreConst[SCORE_020M])==0)
				PresetScore(rampScore, scoreConst[SCORE_020M]);
		}
	else	{
		if((ply->rampShotCount % 100)==0) {
			rampDiagNextExtra = 0;
			RingSfx(SFX_EXTRABALL_LIT);
			ply->extraBallReady++;
			}
		else	{
			rampDiagNextMystery = 0;
			RingSfx(SFX_ARK_WITCHCRAFT_LIT);
			ply->mysteryReady++;
			}
		}
	}
	else if(j==9) {
	if(i & 1) {
		if((ply->rampShotCount % 100)==99) rampDiagNextExtra = 1;
		else rampDiagNextMystery = 1;
		}
		else	{
		if(f==0) rampDiagNextPtr = CreateInfoScores(rampDiagScore, rampScore);
			}
		}
	}

	if(ply->mysteryReady)
		lightCommand[LITE_MYSTERY_READY] = LIGHT_TURN_BLINK;
	else	lightCommand[LITE_MYSTERY_READY] = LIGHT_TURN_OFF;
	if(ply->extraBallReady)
		lightCommand[LITE_EXTRABALL_READY] = LIGHT_TURN_BLINK;
	else	lightCommand[LITE_EXTRABALL_READY] = LIGHT_TURN_OFF;
}

int ServiceWisdom(PLAYER *ply)
{
	int	j, f;

	f = 0;
	j = 0;
	if(ply->tableNo==2 && ply->multiBallGo==0 && ply->roundGoNo==0) {
	if(ply->holeOn[0]!=0 && ply->ballLockEnable==0
		&& ply->mysteryReady==0 && ply->extraBallReady==0) j++;
	if(ply->holeOn[3]!=0 && ply->multiBallReady==0
		&& ply->roundStartReady==0) j++;
	}
	if(ply->tableNo==1 && ply->multiBallGo==0 && ply->roundGoNo==0) {
	if(ply->saucerOn[0]!=0 && ply->mysteryReady==0
		&& ply->extraBallReady==0) j++;
	if(ply->saucerOn[1]!=0 && ply->multiBallReady==0
		&& ply->roundStartReady==0) j++;
	}

	if(j==0) return f;

	AddScore(ply->score, scoreConst[SCORE_001M + wisdomCount]);
	SetMultiInfo(INFO_ANIME_WISDOM_W + wisdomSpell, 4);
	wisdomSpell++;
	if(wisdomSpell >= 6) {
	wisdomSpell = 0;
	f = 1;
	wisdomCount++;
	if(wisdomCount > 9) wisdomCount = 9;
	}
	return f;
}

/*----------------------------------------------/
/	Ball Saver ( Freeze / Shoot again )	/
/----------------------------------------------*/
static void ServiceBallSaver(PLAYER *ply)
{
	int	i, j, f;

	f = ply->ballSaveFlag;
	ply->ballSaveFlag = 0;
	if(ply->switchTotal < (FREEZE_SWITCH * ply->ballSaveLimit))
		ply->ballSaveFlag = 1;
	if(freezeTimer) {
	if(psychoSaveDelay==0) freezeTimer--;
	ply->ballSaveFlag = 1;
	}

	if(ply->ballSaveFlag==0 && f!=0)
		ply->ballSaveAdd = ply->ballSaveLimit - ply->multiBalls;

	if(ply->ballSaveFlag) {
	lightCommand[LITE_SHOOT_AGAIN] = LIGHT_TURN_BLINK;
	if(ply->multiBalls==0 && ply->ballSaveLimit==1 && ply->ballSaveAdd==0
			&& infoNo!=INFO_ANIME_FREEZE && psychoSaveDelay==0
			&& ply->multiBallCount==0) {
		SetInfomations(INFO_ANIME_FREEZE, 9);
		psychoSaveDelay = 2 * frameRate;
		}
	else if(ply->multiBalls < ply->ballSaveLimit && ply->ballSaveLimit > 1
		&& ply->multiBallCount==0) AddPlayerBall();
	}
	else if(ply->extraBallCycle)
		lightCommand[LITE_SHOOT_AGAIN] = LIGHT_TURN_ON;
	else	lightCommand[LITE_SHOOT_AGAIN] = LIGHT_TURN_OFF;

	if(ply->ballSaveFlag==0 && psychoSaveDelay==0
		&& ply->multiBallCount==0) {
	if(ply->ballSaveAdd > 0) {
		if(AddPlayerBall()) ply->ballSaveAdd--;
		}
	else if(ply->multiBalls==0) {
		if(ply->roundGoNo==0 && ply->multiBallGo==0) {
			generalMode = GMODE_BALL_LOST;
			infoText[5] = bonusMultiTexts[ply->bonusMultiCount];
			CreateInfoScore(ply->outHoleBonus);

			ResetScore(roundValue);
			if(ply->bonusMultiCount >= 8) j = 7;
			else j = bonusMultiTables[ply->bonusMultiCount];
			for(i=0; i<j; ++i)
				AddScore(roundValue, ply->outHoleBonus);
		infoText[6] = CreateInfoScores(loopBonusScore, roundValue);
			SetInfomations(INFO_ANIME_BONUS_END, 9);
			}
		}
	}

	if(ply->multiBallCount) {
		if(ply->tableNo==0) {
			if(AddLoopShooter()) ply->multiBallCount--;
			}
		else	{
			if(AddPlayerBall()) ply->multiBallCount--;
			}
		}

	if(psychoSaveDelay) {
	psychoSaveDelay--;
	if(psychoSaveDelay==0) AddPsychoSave();
	}
}

void LaunchBallSaver(PLAYER *ply, int ball)
{
	int	i;

	i = ball - ply->multiBalls;
	if(i >= 0) ply->multiBallCount = i;
	ply->ballSaveLimit = ball;
	ply->ballSaveAdd = 0;
	ply->switchTotal = 0;
}

void AddBallSaver(PLAYER *ply)
{
	if(ply->ballSaveLimit < 7) {
	ply->ballSaveLimit++;
	}
	ply->multiBallCount++;
}

void ServiceExitMulti(PLAYER *ply, int exn)
{
	if(ply->multiBalls <= 1 && ply->multiBallCount==0
		&& ply->ballSaveFlag==0 && ply->ballSaveAdd==0) {
	ply->multiBallGo = exn;
	ply->ballSaveLimit = 1;
	}
	if(tiltFlag) {
	ply->vukEnable = 1;
	ply->multiBallGo = exn;
	ply->ballSaveLimit = 1;
	}
}

void ServiceExitRound(PLAYER *ply, int exn)
{
	if(ply->multiBalls <= 1 && ply->multiBallCount==0
		&& ply->ballSaveFlag==0 && ply->ballSaveAdd==0) {
	ply->roundGoNo = exn;
	ply->ballSaveLimit = 1;
	}
	if(tiltFlag) {
	ply->vukEnable = 1;
	ply->roundGoNo = exn;
	ply->ballSaveLimit = 1;
	}
}

void RoundClose(PLAYER *ply, int no)
{
	if(ply->multiBalls < 1 && ply->ballSaveFlag==0 && ply->ballSaveAdd==0) {
	ply->roundGoNo = no;
	ply->ballSaveLimit = 1;
	}
	if(tiltFlag) {
	ply->vukEnable = 1;
	ply->roundGoNo = no;
	ply->ballSaveLimit = 1;
	}
}

/*----------------------------------------------/
/	Light Indicators Server			/
/----------------------------------------------*/
void InitLightServer(void)
{
	int	i, j, g, b, r;

	j = 20000;	/* Turn Off : 20% (12000 / 65536) */
	for(i=0; i<LIGHT_CLUT_MAX; ++i)
	{
	g = (clutBuff[i+LIGHT_CLUT_TOP] >> 8) & 255;
	b = (clutBuff[i+LIGHT_CLUT_TOP] >> 16) & 255;
	r = (clutBuff[i+LIGHT_CLUT_TOP] & 255);
	lightGreenOff[i] = (g * j) >> 16;
	lightBlueOff[i] = (b * j) >> 16;
	lightRedOff[i] = (r * j) >> 16;
	lightGreenDelta[i] = g - lightGreenOff[i];
	lightBlueDelta[i] = b - lightBlueOff[i];
	lightRedDelta[i] = r - lightRedOff[i];
	lightIntenVal[i] = 255;
	lightCommand[i] = LIGHT_TURN_OFF;
	lightCommandSlave[i] = LIGHT_TURN_OFF;
	lightIntenMode[i] = 0;
	}
	lightAnimeTime = 0;
/*	lightAnimePtr = NULL; */
	lightAnimePtr = liteDemo;
	lightBlinkTime = 0;
	InitLightRoll(0, 0, lightRollFlagA, lightRollFlagB);
	InitLightRoll(0, 820, lightSearchFlagA, lightSearchFlagB);
}

void ServiceLightServer(void)
{
	int	i, g, b, r, f;

	if(lightAnimePtr!=NULL) {
	f = 1;
	g = *lightAnimePtr;
	switch(g)
		{
	case LIGHT_ANIME_STOP:
		lightAnimePtr = NULL;
		lightAnimeTime = 0;
		f = 0;
		break;
	case LIGHT_ANIME_OFF:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		break;
	case LIGHT_ANIME_ON:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_ON;
		break;
	case LIGHT_ANIME_HOLD:
		break;
	case LIGHT_ANIME_RANDOM:
		b = (unsigned int)Rand() % LIGHT_CLUT_MAX;
		for(i=0; i<2; ++i) {
		r = lightIntenVal[b];
		if(r==0 || r==255) lightCommandSlave[b] ^= 1;
		b += ((unsigned int)Rand()) & 0xfff;
		b %= (LIGHT_CLUT_MAX - 1);
		}
		break;
	case LIGHT_ANIME_JUMP:
		g = *(char *)(lightAnimePtr + 1);
		lightAnimePtr += g;
		lightAnimeTime = 0;
		f = 0;
		break;
	case LIGHT_ANIME_CIRCLE_IN:
		g = *(lightAnimePtr + 1);
		b = lightPosPtr[1] << 4;
		i = (b / g) * (g - lightAnimeTime);
		LightCircle(i >> 4);
		break;
	case LIGHT_ANIME_CIRCLE_OUT:
		g = *(lightAnimePtr + 1);
		b = lightPosPtr[1] << 4;
		i = (b / g) * lightAnimeTime;
		LightCircle(i >> 4);
		break;
	case LIGHT_ANIME_ROLL_CW:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		g = *(lightAnimePtr + 1);
		i = ((128 * 16) / g) * (g - lightAnimeTime);
		i >>= 4;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		i += 21;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		i += 21;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		break;
	case LIGHT_ANIME_ROLL_CCW:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		g = *(lightAnimePtr + 1);
		i = ((128 * 16) / g) * lightAnimeTime;
		i >>= 4;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		i += 21;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		i += 21;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		break;
	case LIGHT_ANIME_ROLL2_CW:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		g = *(lightAnimePtr + 1);
		i = ((128 * 16) / g) * (g - lightAnimeTime);
		i >>= 4;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		i += 32;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		break;
	case LIGHT_ANIME_ROLL2_CCW:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		g = *(lightAnimePtr + 1);
		i = ((128 * 16) / g) * lightAnimeTime;
		i >>= 4;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		i += 32;
		LightRoll(i, i+28, lightRollFlagA, lightRollFlagB);
		break;
	case LIGHT_ANIME_WAVE_UP:
		g = *(lightAnimePtr + 1);
		b = (lightPosPtr[1] - lightPosPtr[3]) << 4;
		i = (b / g) * (g - lightAnimeTime);
		i >>= 4;
		i -= lightPosPtr[1];
		LightWidth(i, 15);
		break;
	case LIGHT_ANIME_WAVE_DOWN:
		g = *(lightAnimePtr + 1);
		b = (lightPosPtr[1] - lightPosPtr[3]) << 4;
		i = (b / g) * lightAnimeTime;
		i >>= 4;
		i -= lightPosPtr[1];
		LightWidth(i, 15);
		break;
	case LIGHT_ANIME_WAVE_RIGHT:
		g = *(lightAnimePtr + 1);
		b = (lightPosPtr[0] - lightPosPtr[1]) << 4;
		i = (b / g) * (g - lightAnimeTime);
		i >>= 4;
		i -= lightPosPtr[0];
		LightHeight(i, 5);
		break;
	case LIGHT_ANIME_WAVE_LEFT:
		g = *(lightAnimePtr + 1);
		b = (lightPosPtr[0] - lightPosPtr[1]) << 4;
		i = (b / g) * lightAnimeTime;
		i >>= 4;
		i -= lightPosPtr[0];
		LightHeight(i, 5);
		break;
	case LIGHT_ANIME_SEARCH_CW:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		g = *(lightAnimePtr + 1);
		i = ((16 * 16) / g) * (g - lightAnimeTime);
		i >>= 4;
		i += 32 + 8;
		LightRoll(i, i+30, lightSearchFlagA, lightSearchFlagB);
		break;
	case LIGHT_ANIME_SEARCH_CCW:
		for(i=0; i<LIGHT_CLUT_MAX; ++i) lightCommandSlave[i] = LIGHT_TURN_OFF;
		g = *(lightAnimePtr + 1);
		i = ((16 * 16) / g) * lightAnimeTime;
		i >>= 4;
		i += 32 + 8;
		LightRoll(i, i+30, lightSearchFlagA, lightSearchFlagB);
		break;
	default:
		break;
		}
	if(f) {
	b = *(lightAnimePtr + 1);
		if(b)	{
		if(lightAnimeTime >= b) {
			lightAnimePtr += 2; lightAnimeTime = 0;
			}
			else lightAnimeTime++;
			}
		}
	}

	for(i=0; i<LIGHT_CLUT_MAX; ++i)
	{
	if(i <= LIGHT_CLUT_END) {
	r = lightRedDelta[i];
	b = lightBlueDelta[i];
	g = lightGreenDelta[i];
	r *= lightIntenVal[i] + 1;
	r >>= 8;
	b *= lightIntenVal[i] + 1;
	b >>= 8;
	g *= lightIntenVal[i] + 1;
	g >>= 8;
	r += lightRedOff[i];
	b += lightBlueOff[i];
	g += lightGreenOff[i];
	clutBuff[LIGHT_CLUT_TOP+i] = (b << 16) | (g << 8) | r;
	clutTransBit |= 1 << ((i+LIGHT_CLUT_TOP) >> 4);
		}
	else if(i>=LIGHT_JET1_IDX && i<=LIGHT_JET3_IDX) {
		if(lightIntenVal[i] >= 128) {
			jetBumperTurnOn[i - LIGHT_JET1_IDX] = frameRate * 16 / 80;
			}
		}
	else if(i==LIGHT_SLING_R_IDX || i==LIGHT_SLING_L_IDX) {
		if(lightIntenVal[i] >= 128) {
			slingShotTurnOn[i - LIGHT_SLING_L_IDX] = frameRate * 16 / 80;
			}
		}
	g = lightIntenVal[i];
	if(lightAnimePtr!=NULL) r = lightCommandSlave[i];
	else r = lightCommand[i];
	if(r==LIGHT_TURN_ON) { g += LIGHT_SPEED_ON; if(g >= 255) g = 255; }
	else if(r==LIGHT_TURN_OFF) { g -= LIGHT_SPEED_OFF; if(g < 0) g = 0; }
	else if(r==LIGHT_TURN_BLINK) {
		if(lightIntenMode[i]==0) {
			g -= LIGHT_SPEED_OFF;
			if(g < 0) { g = 0; lightIntenMode[i]++; }
			}
		else if(lightIntenMode[i]==1) {
			g += LIGHT_SPEED_ON;
			if(g >= 255) { g = 255; lightIntenMode[i] = 0; }
			}
		}
	lightIntenVal[i] = g;
	}
}

static void LightCircle(int r)
{
	int	i, x, y;
	short	*pos;

	r *= r;
	pos = lightPosPtr + 4;
	for(i=0; i<LIGHT_CLUT_MAX; ++i)
	{
	x = *pos++;
	y = *pos++;
	if(r > (x * x + y * y))
		lightCommandSlave[i] = LIGHT_TURN_ON;
	else	lightCommandSlave[i] = LIGHT_TURN_OFF;
	}
}

static void InitLightRoll(int xo, int yo, unsigned int *a, unsigned int *b)
{
	int	i, j, k, l;
	int	x, y, xx, yy, sin, cos;
	unsigned int	m;
	short	*pos;

	for(i=0; i<LIGHT_CLUT_MAX; ++i) a[i] = b[i] = 0;
	sin = 1606;	/* 5.625 */
	cos = 16305;	/* 5.625 */
	xx = 16384;
	yy = 0;
	for(k=0; k<2; ++k)
	{
	m = 1;
	for(i=0; i<32; ++i)
		{
		pos = lightPosPtr + 4;
		for(j=0; j<LIGHT_CLUT_MAX; ++j)
			{
			x = *pos++;
			y = *pos++;
			x += xo;
			y += yo;
			l = y * xx - x * yy;
			if(l >= 0 && k==0) a[j] |= m;
			else if(l >= 0 && k==1) b[j] |= m;
			}
		m <<= 1;
		j  = (xx * cos + yy * sin) >> 14;
		yy = (yy * cos - xx * sin) >> 14;
		xx = j;
		}
	}
	lightRollTime = 0;
}

static void LightRoll(int a, int b, unsigned int *ab, unsigned int *bb)
{
	int	i;
	unsigned int	j, k, m[2], n[2];

	a &= 63;
	b &= 63;
	m[0] = m[1] = n[0] = n[1] = 0;
	if(a < 32) m[0] = 1 << (a & 31);
	else m[1] = 1 << (a & 31);
	if(b < 32) n[0] = 1 << (b & 31);
	else n[1] = 1 << (b & 31);

	for(i=0; i<LIGHT_CLUT_MAX; ++i)
	{
	j = ab[i] & m[0];
	j |= bb[i] & m[1];
	k = ab[i] & n[0];
	k |= bb[i] & n[1];
	if(j!=0 && k!=0) lightCommandSlave[i] = LIGHT_TURN_ON;
	}
}

static void LightWidth(int yp, int w)
{
	int	i, y;
	short	*pos;

	pos = lightPosPtr + 4;
	for(i=0; i<LIGHT_CLUT_MAX; ++i)
	{
	pos++;
	y = *pos++;
	if(y >= (yp - w) && y < (yp + w))
		lightCommandSlave[i] = LIGHT_TURN_ON;
	else	lightCommandSlave[i] = LIGHT_TURN_OFF;
	}
}

static void LightHeight(int xp, int h)
{
	int	i, x;
	short	*pos;

	pos = lightPosPtr + 4;
	for(i=0; i<LIGHT_CLUT_MAX; ++i)
	{
	x = *pos++;
	pos++;
	if(x >= (xp - h) && x < (xp + h))
		lightCommandSlave[i] = LIGHT_TURN_ON;
	else	lightCommandSlave[i] = LIGHT_TURN_OFF;
	}
}

static void InitDiags(void)
{
	int	i;

	mysteryDiagTextPtr = NULL;
	rampDiagScorePtr = NULL;
	mazeDiagScorePtr = NULL;
	rampDiagNextExtra = 0;
	rampDiagNextMystery = 0;
	for(i=0; i<4; ++i) diagInfoTab[i] = -1;
	diagTimer = 0;
}

static void ServiceDiag(PLAYER *ply)
{
	int	i, j, k, f;
	char	idx[18];

	if(generalMode!=GMODE_PLAY_GAME) return;

	for(i=0; i<18; ++i) idx[i] = -1;
	i = 0;
	if(diagInfoTab[2] >= 0) { idx[0] = i; i+=2; }
	if(diagInfoTab[3] >= 0) { idx[1] = i; i+=2; }
	if(ply->roundStartReady!=0 && ply->multiBallGo==0
		&& ply->multiBallRestart==0) { idx[2] = i; i++; }
	if(ply->superRoundEnable!=0 && ply->multiBallGo==0
		&& ply->roundStartReady!=0
		&& ply->multiBallRestart==0) { idx[3] = i; i++; }
	if(ply->multiBallReady!=0 && ply->multiBallEnable!=0) { idx[4] = i; i++; }
	if(ply->extraBallReady) { idx[5] = i; i++; }
	if(ply->mysteryReady) { idx[6] = i; i++; }
	if(ply->multiBallRestart) { idx[7] = i; i++; }
	if(ply->holeSelectorEnable!=0 && ply->roundGoNo==0
		&& ply->multiBallGo==0) { idx[8] = i; i++; }
	if(mysteryDiagTextPtr!=NULL) { idx[9] = i; i+=2; }
	if(mazeDiagScorePtr!=NULL) { idx[10] = i; i+=2; }
	if(rampDiagNextPtr!=NULL) { idx[11] = i; i++; }
	if(rampDiagScorePtr!=NULL) { idx[12] = i; i+=2; }
	if(rampDiagNextExtra) { idx[13] = i; i++; }
	if(rampDiagNextMystery) { idx[14] = i; i++; }
	if(diagInfoTab[0] >= 0)  { idx[15] = i; i++; }
	if(diagInfoTab[1] >= 0)  { idx[16] = i; i++; }
	if(nextStageEnable!=0 && realmsEnable!=0
		&& ply->roundGoNo==0
		&& ply->multiBallGo==0) { idx[17] = i; i++; }

	if(i==0) return;

	if(diagTimer >= (i * frameRate)) diagTimer = 0;

	f = 0;
	if(infoNo < 0) f = 1;
	if(diagInfoTab[2] >= 0 && infoPriority==0)
				{ f = 1; diagTimer = 0; }
	if(diagInfoTab[3] >= 0 && infoPriority==0)
				{ f = 1; diagTimer = 0; }

	if((diagTimer % frameRate)==0 && f!=0) {
	j = diagTimer / frameRate;
	for(k=0; k<18; ++k) {
		if(idx[k]==j)	{
			if(k==0) {
			SetInfomations(diagInfoTab[2], 0);
			diagInfoTab[2] = -1;
				}
			if(k==1) {
			SetInfomations(diagInfoTab[3], 0);
			diagInfoTab[3] = -1;
				}
			else if(k==2) {
			f = ply->roundNumber + ply->tableNo * 6;
			SetInfomations(infoDiagRounds[f], 0);
				}
			else if(k==3) {
			SetInfomations(diagDoubleTables[ply->tableNo], 0);
			}
			else if(k==4) SetInfomations(INFO_DIAG_MB_READY, 0);
			else if(k==5) SetInfomations(INFO_DIAG_EXTRA_READY, 0);
			else if(k==6) {
			SetInfomations(diagMysteryTables[ply->tableNo] , 0);
				}
			else if(k==7) {
			infoTimer = ply->multiBallRestart / frameRate;
			SetInfomations(INFO_DIAG_MB_RESTART, 1);
				}
			else if(k==8) {
			SetInfomations(INFO_DIAG_LOCK_READY , 0);
				}
			else if(k==9) {
			infoText[1] = mysteryDiagTextPtr;
			SetInfomations(INFO_DIAG_MYSTERY_AWARD, 0);
			mysteryDiagTextPtr = NULL;
				}
			else if(k==10) {
			infoText[4] = mazeDiagScorePtr;
			SetInfomations(INFO_DIAG_MAZE_COMP, 0);
			mazeDiagScorePtr = NULL;
				}
			else if(k==11) {
			infoText[2] = rampDiagNextPtr;
			SetInfomations(INFO_DIAG_NRAMP_SCORE, 0);
				}
			else if(k==12) {
			infoText[2] = rampDiagScorePtr;
			SetInfomations(INFO_DIAG_RAMP_AWARD, 0);
			rampDiagScorePtr = NULL;
				}
			else if(k==13) {
			SetInfomations(INFO_DIAG_NRAMP_XBALL, 0);
				}
			else if(k==14) {
			SetInfomations(INFO_DIAG_NRAMP_MYSTERY, 0);
				}
			else if(k==15) {
			SetInfomations(diagInfoTab[0], 0);
				}
			else if(k==16) {
			SetInfomations(diagInfoTab[1], 0);
				}
			else if(k==17) {
			SetInfomations(INFO_DIAG_NEXT_STAGE, 0);
				}
			}
		}
	}
	diagTimer++;
}

void ResetDropTarget(PLAYER *ply)
{
	int	i;

	for(i=0; i<8; ++i) {
	ply->dropTargetEnable[i] = 1;
	ply->dropTargetFlag[i] = 0;
	}
	RingSfx(SFX_DROP_TARGET_RESET);
}

void SetRoundTimer(PLAYER *ply, int base)
{
	int	i;

	if(ply->longerTimerEnable) base += 20;
	roundTimer = base * frameRate;
}

void ServiceMultiScore(PLAYER *ply, int s, int d)
{
	int	i, j;
	char	v[12];

	PresetScore(v, scoreConst[s]);
	if(ply->superRoundEnable) AddScore(v, scoreConst[s]);
	ResetScore(roundValue);
	j = ply->multiBalls;
	while(j) {
		AddScore(roundValue, v);
		j--;
		}
	CreateInfoScore(roundValue);
	diagInfoTab[0] = d;

	if(lightAnimePtr==NULL) multiScoreTimer = frameRate + (frameRate >> 1);
	if(multiScoreTimer) {
	multiScoreTimer--;
	if(multiScoreTimer==0) lightAnimePtr = liteWBT;
	}
}

static void AddKickback(PLAYER *ply)
{
	if(ply->kickbackReady >= 100) {
		AddScore(ply->score, scoreConst[SCORE_050M]);
		if(ply->roundGoNo==0 && ply->multiBallGo==0)
			SetInfomations(INFO_ANIME_50M, 5);
		ply->kickbackReady = 0;
		}
	else ply->kickbackReady++;
}

/*----------------------------------------------/
/	Loop Bonus				/
/	１−３回　　なし			/
/	４回　　　　１Ｍ			/
/	５回　　　　２Ｍ			/
/	６回　　　　３Ｍ			/
/	７回　　　　４Ｍ			/
/	８回　　　　５Ｍ+エキストラボールリット	/
/	９回　　　　１０Ｍ			/
/	１０回　　　２０Ｍ			/
/	１１回以上　２０Ｍ			/
/----------------------------------------------*/
void ServiceLoopBonus(PLAYER *ply)
{
	int	i, f;
	char	*ptr;

	if(ply->roundGoNo | ply->multiBallGo) return;

	f = 0;
	if(ply->tableNo==1) {
	if(ply->laneRollOverOn[0]) f = 1;
	}
	else if(ply->tableNo==2) {
	if(ply->rampRollOverOn[1]) f = 1;
	}

	if(f) {
	if(loopBonusTimer) ply->loopBonusCount++;
	loopBonusTimer = 3 * frameRate;
	if(ply->loopBonusCount>=1 && ply->loopBonusCount <= 3
						&& ply->tableNo==1) {
		for(i=0; i<3; ++i) if(ply->standUpFlag[i + 3]==0) break;
		if(i < 3) ply->standUpFlag[i + 3] = 1;
		}
	if(ply->loopBonusCount >= 4) {
		RingSfx(SFX_MILLION_GRAIL);
		if(ply->loopBonusCount >= 11) ply->loopBonusCount = 11;
		ptr = scoreConst[loopBonusTables[ply->loopBonusCount - 4]];
		infoText[3] = CreateInfoScores(loopBonusScore, ptr);
		diagInfoTab[3] = INFO_DIAG_LOOP_BONUS;
		AddScore(ply->score, ptr);
		}
	}
	if(loopBonusTimer) loopBonusTimer--;
	else ply->loopBonusCount = 0;
}

/*------------------------------------------------------/
/	Soul ( EXTRA BALL )				/
/------------------------------------------------------*/
void ServiceSoul(PLAYER *ply, int task)
{
	int	i, j;

	switch(soulTask)
	{
	case 0:
		ply->multiBallEnable = 1;
		SetRoundTimer(ply, 30);
		soulTask++;
		break;
	case 1:
	case 2:
	case 3:
		for(i=0; i<3; ++i) {
		j = kickbackLiteIndex[ply->tableNo * 3 + i];
		lightCommand[j] = LIGHT_TURN_BLINK;
		}
		infoTimer = roundTimer / frameRate;
		j = (soulTask - 1) * 2;
		if(ply->superRoundEnable) j++;
		PresetScore(roundValue, scoreConst[soulScoreTables[j]]);
		CreateInfoScore(roundValue);
		if(ply->extraBallCount < 3 && soulTask==3)
			diagInfoTab[0] = INFO_DIAG_SOUL_XBALL;
		else	diagInfoTab[0] = INFO_DIAG_SOUL_SCORE;
		for(i=0; i<3; ++i) if(ply->standUpOn[i]) break;
		if(i < 3) {
		lightAnimePtr = liteWBT_WTB_TCW_TCC;
		j = 1;
		if(soulTask==3) {
		if(ply->extraBallCount < 3) {
			j = 0;
			RingSfx(SFX_EXTRABALL_AWARD);
			ply->extraBallCount++;
			ply->extraBallCycle++;
		if(ply->superRoundEnable) {
			if(ply->extraBallCount < 3) {
			ply->extraBallCount++;
			ply->extraBallCycle++;
					} else {
			RingSfx(SFX_SOUL_AWARD);
			RingSfx(SFX_ROUND_JACKPOT);
			SetInfomations(INFO_ANIME_30M, 5);
			AddScore(ply->roundScore, scoreConst[SCORE_030M]);
						}
					}
				}
			}
			if(j)	{
			RingSfx(SFX_SOUL_AWARD);
			RingSfx(SFX_ROUND_JACKPOT);
			j = (soulTask - 1) * 2;
			if(ply->superRoundEnable) j++;
			SetInfomations(soulAnimeTables[j], 5);
			AddScore(ply->roundScore, roundValue);
			}
		soulTask++;
		}
		roundTimer--;
		if(roundTimer <= 0) soulTask = 4;
	if(ply->multiBalls < 1 && ply->ballSaveFlag==0 && ply->ballSaveAdd==0)
			soulTask = 4;
		break;
	case 4:
		soulTask = 0;
		for(i=0; i<3; ++i) ply->standUpFlag[i] = 0;
		ply->roundGoNo = task;
		break;
	default:
		break;
	}
}

/*----------------------------------------------/
/	ラウンド終了				/
/----------------------------------------------*/
void ServiceRoundEnd(PLAYER *ply)
{
	int	i, j, k;

	for(i=0; i<2; ++i) diagInfoTab[i] = -1;

	j = ply->tableNo * 8;
	for(i=0; i<8; ++i) {
	k = roundEndLiteTables[j];
	if(k >= 0) lightCommand[k] = LIGHT_TURN_OFF;
	j++;
	}

	if(ply->multiBallGo==0) {
	CreateInfoScore(ply->roundScore);
	diagInfoTab[2] = INFO_DIAG_ROUND_OVER;
	}
	AddScore(ply->score, ply->roundScore);
	lightAnimePtr = NULL;
	ply->superRoundEnable = 0;
	ply->multiBallEnable = 1;
	ply->ballSaveLimit = 1;
	ScanNextRound(ply);
	ply->roundGoNo = 0;
}

static void ServiceTestRule(PLAYER *ply)
{
	int	i;

	if(gamePadTrigger & SATURN_DIR_UP) testPlayNo++;
	if(gamePadTrigger & SATURN_DIR_DOWN) testPlayNo--;
	testPlayNo &= 7;
	i = ply->tableNo * 8 + testPlayNo;
	MobjInfoTexts(320+160, 430, 2, testPlayString[i]);
	if(gamePadTrigger & SATURN_PAD_B) {
	if(testPlayNo < 6) {
		ply->roundNumber = testPlayNo;
		LaunchRound(ply);
		}
	else ply->multiBallGo = (testPlayNo - 6) + 1;
	ResetScore(ply->roundScore);
	}
}

void LaunchRound(PLAYER *ply)
{
	int	i;

	roundStartDelay = frameRate * 3;
	i = roundInfoIndex[ply->tableNo * 6 + ply->roundNumber];
	infoAnimeNo[0] = INFO_NO_ROUND_1 + i;
	if(ply->tableNo==1) i = INFO_ANIME_RO2;
	else i = INFO_ANIME_ROUND;
	if(ply->superRoundEnable!=0)
		SetInfomations(i + 1, 10);
	else	SetInfomations(i, 10);
	RingSfx(SFX_ARK_ROUND_START);
	ply->roundStartReady = 1;
	ply->roundGoNo = 0;
}

void ServiceMysteryRound(PLAYER *ply)
{
	lightAnimePtr = liteTCW_TCC;
	ply->multiBallEnable = 1;
	RingSfx(SFX_ARK_WITCHCRAFT_LIT);
	if(ply->superRoundEnable) {
	ply->mysteryReady++;
	ply->superRoundEnable = 0;
	}
	ply->mysteryReady++;
	ScanNextRound(ply);
	ply->roundGoNo = 0;
}


void ServiceStatus(void)
{
	int	i, j, f;
	PLAYER	*ply;

	ply = &playerTab[playerNo];

	if(slaveTaskNo >= 20 && slaveTaskNo <= 25) f = 0; else f = 1;

	if((gamePadTrigger & SATURN_PAD_A)!=0 && f==1) {
	InitPlayingManual(ply->tableNo);
	MainPlayingManual();
	if(ply->multiBallGo) SetDiffAnime(ply->tableNo);
	return;
	}

	j = infoTextNo;
	infoTextNo = 0;
	if((vsyncCounter & 31) < 27)
		MobjInfoTexts(320, 40, 1, "INTERMISSION");
	CreateInfoScore(ply->jackpot);
	MobjInfoTexts(320, 60+35*1, 3, "JACKPOT VALUE: @");
	infoNumber = ply->extraBallCycle;
	MobjInfoTexts(320, 60+35*2, 3, "EXTRABALL REMAIN: #");
	CreateInfoScore(ply->outHoleBonus);
	MobjInfoTexts(320, 60+35*3, 3, "OUT HOLE BONUS VALUE: @");
	infoNumber = bonusMultiTables[ply->bonusMultiCount];
	MobjInfoTexts(320, 60+35*4, 3, "BONUS MULTIPLIER: X #");
	infoNumber = ply->rampShotCount % 1000;
	MobjInfoTexts(320, 60+35*5, 3, "RAMP SHOT: #");

	MobjInfoTexts(320, 70+35*6, 3, "MYSTERY AWARDS:");
	i = 0;
	if(ply->playField2xEnable) {
	StatusSub(i, 70+35*7, "2X PLAYFIELD."); i++;
	}
	if(ply->longerTimerEnable) {
	StatusSub(i, 70+35*7, "LONGER TIMER."); i++;
	}
	if(ply->bonusHoldEnable) {
	StatusSub(i, 70+35*7, "BONUS HOLD."); i++;
	}
	if(ply->superKickbackReady) {
	StatusSub(i, 70+35*7, "SUPER KICKBACK."); i++;
	}
	if(ply->superJetsEnable) {
	StatusSub(i, 70+35*7, "SUPER JETS."); i++;
	}
	infoTextNo = j;

	if((vsyncCounter & 31) < 27 && f==1)
		MobjInfoTexts(320, 70+35*10, 3, "PRESS A BUTTON FOR INSTRUCTION");
}

static void StatusSub(int i, int y, char *s)
{
	int	x;

	if(i & 1) x = 320 + 160; else x = 160;
	MobjInfoTexts(x, y + (i >> 1) * 35, 3, s);
}

static void ServiceBGM(PLAYER *ply)
{
	int	i, j;

	if(optionMenuPad[OPTION_NO_BGM]==0) return; 
	if(trialEnable!=0 && cdTrackStyle!=0) return;

	if(multiBallGoOld!=0 && ply->multiBallGo==0) {
		PlayCD(audioIndexTables[ply->tableNo * 5 + 0],
		       audioIndexTables[ply->tableNo * 5 + 1]);
		}
	else if(roundGoNoOld!=0 && ply->roundGoNo==0) {
	if(ply->multiBallReady!=0 && ply->multiBallEnable!=0) {
		PlayCD(-1, audioIndexTables[ply->tableNo * 5 + 2]);
		}
	else	{
		PlayCD(audioIndexTables[ply->tableNo * 5 + 0],
		       audioIndexTables[ply->tableNo * 5 + 1]);
		}
	}
	else if(multiBallGoOld==0 && ply->multiBallGo!=0)
		PlayCD( audioIndexTables[ply->tableNo * 5 + 3],
			audioIndexTables[ply->tableNo * 5 + 4]);
	if(ply->multiBallEnable) {
	if(multiBallReadyOld==0 && ply->multiBallReady!=0)
		PlayCD(-1, audioIndexTables[ply->tableNo * 5 + 2]);
	multiBallReadyOld = ply->multiBallReady;
	}
	multiBallGoOld = ply->multiBallGo;
	roundGoNoOld = ply->roundGoNo;
}

static void ServiceJackpotArrow(PLAYER *ply)
{
	int	i, j, k;

	if(optionMenuPad[OPTION_NO_ARROW]==0) return;

	j = ply->tableNo * 12;
	for(i=0; i<6; ++i) {
	k = arrowJackpotLiteTables[j];
	if(lightCommand[k]==LIGHT_TURN_BLINK) {
	k = arrowJackpotLiteTables[j + 1];
	if((vsyncCounter & 31) < 11 && k >= 0) MobjArrow(k);
		}
	j += 2;
	}
}

void ServicePlayerInfo(PLAYER *ply)
{
	int	x, y, i;

	if(optionMenuPad[OPTION_NO_PLAYER]!=0 && trialEnable==0 && geEnable==0) {
	x = MobjScore(0, -1, ply->score);
	MobjScore(16 - x, 16, ply->score);
	MobjScore(640-16, 16, (ply+1)->score);
	if(playerNo) x = 640 - 32; else x = 16;
	y = 48;
	}
	else {
	MobjScore(640-16, 16, ply->score);
	x = 16;
	y = 16;
	}
	if(playerNo) ply++;
	if((vsyncCounter & 31) > 22)
		MobjInfoText(x, y, 2, ply->ballInPlay + 30);
}

void ServicePlayerSelector(void)
{
	if(playerSwap) {
	playerNo++;
	playerNo &= 1;
	playerSwap = 0;
	}
}

static int InstallScore(PLAYER *ply)
{
	int	i, j, k, l;
	char	*ptr, *a, *b;

	for(i=0; i<8; ++i) {
	l = 7 - i;
	ptr = &necroPad[ply->tableNo].topScore[l * 12];
	if(CmpScore(ptr, ply->score)) {
		if(l)	{
			a = &necroPad[ply->tableNo].topScore[0];
			b = a + 12;
			k = l * 12;
			for(j=0; j<k; ++j) *a++ = *b++;
			a = &necroPad[ply->tableNo].topName[0];
			b = a + 3;
			k = l * 3;
			for(j=0; j<k; ++j) *a++ = *b++;
			}
		PresetScore(ptr, ply->score);
		ply->nameInPos = l;
		return i + 1;
		}
	}
	return 8;
}

static int ServiceNameEntry(PLAYER *ply)
{
	int	i;
	char	*txt;

	if(infoValue==0) return 0;

	if(gamePadTrigger & SATURN_DIR_DOWN) nameText[nameTextPos]--;
	else if(gamePadTrigger & SATURN_DIR_UP) nameText[nameTextPos]++;
	if(nameText[nameTextPos] < 'A') nameText[nameTextPos] = 'A';
	else if(nameText[nameTextPos] > 'Z') nameText[nameTextPos] = 'Z';
	i = nameTextPos;
	if(gamePadTrigger & SATURN_PAD_A) {
		nameTextPos++;
		nameText[nameTextPos] = nameText[nameTextPos-1];
		}
	if(gamePadTrigger & SATURN_PAD_C) nameTextPos--;
	if(nameTextPos < 0) nameTextPos = 0;
	if(nameTextPos >= 3) nameTextPos = 3;
	if(nameTextPos!=i) {
		if(nameTextPos < i) RingSfx(SFX_OUT_LANE);
		else RingSfx(SFX_PLUNGER_SHOT);
		}
	if(nameTextPos==3) {
	nameText[3] = 0;
	txt = &necroPad[ply->tableNo].topName[ply->nameInPos * 3];
	for(i=0; i<3; ++i) *txt++ = nameText[i];
	return 1;
	}
	nameText[nameTextPos+1] = 0;
	return 0;
}
