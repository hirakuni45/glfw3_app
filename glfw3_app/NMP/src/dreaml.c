#include <stdio.h>
#include "ballsim.h"
#include "score.h"
#include "lite.h"
#include "info.h"
#include "sound.h"
#include "option.h"
#include "genr.h"
#include "top.h"
#include "cdfs.h"
#include "sim.h"
#include "dreaml.h"

extern volatile short slaveTaskNo;
extern volatile unsigned int vsyncCounter;
extern short infoNo, infoNumber, infoTimer;
extern unsigned short gamePadLevel, gamePadTrigger, gamePadRelese;
extern short roundTimer;
extern char roundValue[], *infoText[];
extern char nextStageEnable;
extern short diagInfoTab[];
extern short soulTask;
extern char optionMenuPad[];

extern char *scoreConst[];
extern int frameRate;
extern char lightCommand[];
extern short *lightPosPtr;
extern unsigned char *lightAnimePtr;
extern unsigned char liteWBT[], liteWBT_BLK_WBT_BLK[];
extern unsigned char liteBLK[], liteBLKALL[], liteBLK4[];
extern unsigned char liteTCW_TCC_WBT_WBT[];
extern unsigned char liteWBT2[], liteWTB2[];
extern unsigned char liteWBT_WTB_TCW_TCC[];
extern unsigned char liteTCW_TCC_WBT[];
extern unsigned char liteWBT_BLK_WBT_BLK[];
extern unsigned char liteNightMare[];
extern short infoAnimeNo[];
extern short roundStartDelay;
extern char magnaLockEnable, ballHitSfxEnable;

static void ServiceMultiBall(PLAYER *ply, char *sct);
static void ServiceRound(PLAYER *ply);
static void ServiceRoundStart(PLAYER *ply);

static short lightPosTables[] = {
/* AREA */
	-57,154,57,-85,
/* POS */
	0,0,
	0,0,
	24,43,
	-31,54,
	27,52,
	-33,37,
	21,75,
	37,16,
	-9,37,
	-26,65,
	-42,25,
	-42,25,
	-40,32,
	-18,62,
	-14,56,
	-10,52,
	30,40,
	32,32,
	35,25,
	-30,29,
	-19,-14,
	0,-23,
	19,-13,
	22,7,
	28,31,
	-22,88,
	-20,76,
	-7,139,
	3,141,
	14,142,
	-12,-6,
	-9,-10,
	-5,-12,
	0,-13,
	4,-12,
	9,-10,
	12,-7,
	-42,-11,
	-34,-11,
	33,-11,
	41,-11,
	0,-34,
	41,-20,
	-42,-24,
/* JETS */
	-10,9,
	7,78,
	13,98,
/* SLING */
	25,-22,
	-25,-22
};
static short ballLockDelay;
static short multiBallDelay;
static short dreamMulti;
static short woodCount;
static short magnaTimer, vukTimer;
static short standUpComp, jackpotNo, jackpotSpell;
static short necroMoonCount, necroMoonLimit;
static char necroFlag[6];
static char magnaFlag;
static char soulFlag[3];
static char jackpotLiteTables[] = {
	LITE_DRM_LEFTRAMP_JACKPOT,
	LITE_DRM_LEFTLANE_JACKPOT,
	LITE_DRM_RIGHTRAMP_JACKPOT,
	LITE_DRM_RIGHTLANE_JACKPOT
};
static char roundAudioTables[] = {
	-1, -1,
	CDA_DRM_SOUL_START, CDA_DRM_SOUL_PLAY,
	CDA_DRM_GATE_START, CDA_DRM_GATE_PLAY,
	CDA_DRM_WOOD_START, CDA_DRM_WOOD_PLAY,
	CDA_DRM_MOON_START, CDA_DRM_MOON_PLAY,
	CDA_DRM_NECRO_START, CDA_DRM_NECRO_PLAY
};
static char necroLightIndex[] = {
	LITE_DRM_RIGHTRAMP_JACKPOT,
	LITE_DRM_LEFTRAMP_JACKPOT,
	LITE_DRM_RIGHTLANE_JACKPOT,
	LITE_DRM_LEFTLANE_JACKPOT,
	LITE_DRM_CENTER_HOLE_JACKPOT,
	LITE_DRM_MOON_HOLE_JACKPOT
};
static char readyCountTables[] = { 5, 7, 10 };

void InitDreaml(PLAYER *ply)
{
	lightPosPtr = &lightPosTables[0];
	ply->vukEnable = 1;
	ply->gateEnable[DIVERTER1_TRUE] = 1;
	ply->gateEnable[DIVERTER1_FALSE] = 0;
	ply->holeSelectorNo = 0;

	ply->countUpMulti = 0;
	ply->multiReadyCount = 0;
	ply->multiBallDepth = 0;
	ply->ballLockEnable = 0;

	PresetScore(ply->jackpot, scoreConst[SCORE_010M]);
}

void InitDreamlPlay(PLAYER *ply)
{
	magnaLockEnable = 0;
	magnaFlag = 0;
	magnaTimer = 0;
	standUpComp = 0;
	vukTimer = 0;
	dreamMulti = 0;
	ballLockDelay = 0;
	ply->vukEnable = 1;
}

void ServiceDreaml(PLAYER *ply, char *sct)
{
	int	i, j, f, z;

	if(ply->roundGoNo==0 && ply->multiBallGo==0) {
	if(ply->standUpOn[6]) {
		magnaFlag ^= 1;
		if(magnaFlag==0) magnaLockEnable = 0;
		}
	if(magnaFlag)
		lightCommand[LITE_DRM_STANDUP_TARGET_1] = LIGHT_TURN_ON;
	else	lightCommand[LITE_DRM_STANDUP_TARGET_1] = LIGHT_TURN_OFF;
	}
	if(magnaFlag) {
	magnaTimer++;
	i = magnaTimer / frameRate;
	if(i >= 0 && i < 4) magnaLockEnable = 1;
	else if(i >= 4 && i < 7) magnaLockEnable = 0;
	else if(i >= 7) magnaTimer = 0;
	}
	if(magnaLockEnable)
		lightCommand[LITE_DRM_MAGNA_LOCK_NONE] = LIGHT_TURN_BLINK;
	else	lightCommand[LITE_DRM_MAGNA_LOCK_NONE] = LIGHT_TURN_OFF;

	if(ServiceWisdom(ply)) ballLockDelay = frameRate * 3;
/*----------------------------------------------/
/	Mystery / Extraball			/
/----------------------------------------------*/
	if(ply->holeOn[0]) {
	j = ply->roundGoNo | ply->multiBallGo;
	if(ply->extraBallReady)	{
		RingSfx(SFX_EXTRABALL_AWARD);
		if(ply->extraBallCount < 3) {
		ply->extraBallCount++;
		ply->extraBallCycle++;
		if(j==0) {
			ply->vukEnable = 0;
			vukTimer += 2 * frameRate;
			SetMultiInfo(INFO_ANIME_EXTRA_BALL, 5);
			}
		}
		else	{
			ply->vukEnable = 0;
			vukTimer += 2 * frameRate;
			AddScore(ply->score, scoreConst[SCORE_030M]);
			if(j==0) SetMultiInfo(INFO_ANIME_30M, 5);
			}
		ply->extraBallReady--;
		}
	if(ply->mysteryReady) {
		if(j==0) {
			ply->vukEnable = 0;
			vukTimer += 2 * frameRate;
			f = 1;
			}
		else f = 0;
		TakeMystery(ply, f);
		ply->mysteryReady--;
		}
	}
	if(vukTimer) {
	vukTimer--;
	if(vukTimer==0) ply->vukEnable = 1;
	}

/*------------------------------------------------------/
/	StandUp #4 to #6 (Count Up To Multi Ball)	/
/------------------------------------------------------*/
	for(i=0; i<3; ++i) {
	if(standUpComp)
		lightCommand[LITE_DRM_STANDUP_TARGET_4 + i] = LIGHT_TURN_BLINK;
	else	{
		if(ply->standUpFlag[i + 3])
			lightCommand[LITE_DRM_STANDUP_TARGET_4 + i] = LIGHT_TURN_ON;
		else	lightCommand[LITE_DRM_STANDUP_TARGET_4 + i] = LIGHT_TURN_OFF;
		}
	}
	if(standUpComp) standUpComp--;
	j = 1;
	for(i=0; i<3; ++i) j &= ply->standUpFlag[i + 3];
	if(j) {
	for(i=0; i<3; ++i) ply->standUpFlag[i + 3] = 0;
	standUpComp = frameRate * 2;
	AddScore(ply->jackpot, scoreConst[SCORE_0025M]);
	if(ply->multiBallGo==0 && ply->countUpMulti < 4) {
///		RingSfx(SFX_DRM_NIGHT_LOCK_RDY, 0);
		RingSfx(SFX_DRM_NIGHT_LOCK_RDY);
		ply->ballLockEnable = 1;
		}
	}

	if(ballLockDelay) {
	ballLockDelay--;
	if(ballLockDelay==0) {
///		RingSfx(SFX_DRM_NIGHT_LOCK_RDY, 0);
		RingSfx(SFX_DRM_NIGHT_LOCK_RDY);
		ply->ballLockEnable = 1;
		}
	}

	ServiceLoopBonus(ply);
	ServiceMultiBall(ply, sct);
	ServiceRound(ply);

	if(ply->roundGoNo==0 && ply->multiBallGo==0) {
	if(ply->ballLockEnable) {
		ply->holeSelectorEnable = 1;
		lightCommand[LITE_DRM_CENTER_HOLE_JACKPOT] = LIGHT_TURN_BLINK;
		}
	else	{
		lightCommand[LITE_DRM_CENTER_HOLE_JACKPOT] = LIGHT_TURN_OFF;
		}
	}
}

void DoDreamMultiBallReady(PLAYER *ply)
{
	ply->multiBallReady = 1;
/*	countUpMulti = 0; */
}

static void ServiceMultiBall(PLAYER *ply, char *sct)
{
	int	i, j, k;

	if(ply->multiBallEnable==0) return;

	switch(ply->multiBallGo)
	{
	case 0:
		if(ply->roundGoNo==0 && ply->multiBallReady==0) {
		if(ply->rampRollOverOn[1]) {
		ply->multiReadyCount++;
		infoNumber = readyCountTables[ply->multiBallDepth]
							 - ply->multiReadyCount;
		if(infoNumber==0) ;
		else if(infoNumber==1) diagInfoTab[2] = INFO_DIAG_MULTI_LOOP;
		else diagInfoTab[2] = INFO_DIAG_MULTI_LOOPS;
		if(ply->multiReadyCount >= 
					readyCountTables[ply->multiBallDepth]) {
				ply->multiBallReady = 1;
			if(ply->multiBallDepth < 2) ply->multiBallDepth++;
				}		
			}
		}
		if(ply->roundGoNo==0 && ply->multiBallGo==0) {
		if(ply->holeSelectorEnable!=0 && ply->holeOn[0]!=0) {
			ply->ballLockEnable = 0;
			RingSfx(SFX_DRM_NIGHT_LOCKED);
			if(ply->countUpMulti < 4) ply->countUpMulti++;
			infoNumber = ply->countUpMulti;
			if(ply->countUpMulti==1) diagInfoTab[2] = INFO_DIAG_LOCK_COUNT;
			else diagInfoTab[2] = INFO_DIAG_LOCK_COUNTS;
			lightAnimePtr = liteWBT2;
			dreamMulti++;
			}	
		if(dreamMulti) { if(AddPlayerBall()) dreamMulti--; }
		}
		if(ply->multiBallReady!=0 || ply->multiBallRestart!=0) {
		if(ply->multiBallRestart) ply->multiBallRestart--;
		if(ply->holeOn[3]) {
			lightCommand[LITE_DRM_CENTER_HOLE_JACKPOT] = LIGHT_TURN_OFF;
			if(ply->multiBallRestart) ply->multiBallCompFlag = 1;
			LaunchBallSaver(ply, ply->countUpMulti + 3);
			ply->vukEnable = 0;
			slaveTaskNo = 20;
			magnaFlag = 0;
			magnaLockEnable = 1;
			ply->multiBallReady = 0;
			ply->multiBallRestart = 0;
			roundTimer = 30 * frameRate;
			ply->multiBallGo = 2;
			multiBallDelay = 3 * frameRate;
			ballHitSfxEnable = 0;
			if(ply->roundGoNo) ply->roundGoNo = 5;
			}
		}
		break;
	case 1:
	case 2:
		if(vsyncCounter & 32) lightAnimePtr = liteBLK;
		if(multiBallDelay!=0 && ply->multiBallCount==0)
			multiBallDelay--;
		if(multiBallDelay==0) {
			ballHitSfxEnable = 1;
			magnaLockEnable = 0;
			ply->vukEnable = 1;
			}
		if(multiBallDelay==0 && ply->multiBallCount==0) {
		if(roundTimer) roundTimer--;
		}
		ResetScore(roundValue);
/*		j = countUpMulti + 3;
		while(j) { */
		AddScore(roundValue, scoreConst[SCORE_100K]);
/*			j--;
			} */
		infoTimer = roundTimer / frameRate;
		CreateInfoScore(ply->jackpot);
		diagInfoTab[0] = INFO_DIAG_MULTI2_DIRECT;
		i = ply->switchFrame;
		if(i) RingSfx(SFX_DRM_NIGHT_AWARD);
		while(i) {
		AddScore(ply->jackpot, roundValue);
		i--;
		}
		if(roundTimer==0) {
			jackpotNo = 0;
			lightAnimePtr = liteWTB2;
			jackpotSpell = 0;
			ply->multiBallGo = 3;
			}
		ServiceExitMulti(ply, 5);
		break;
	case 3:
		diagInfoTab[0] = INFO_DIAG_NIGHTMARE + jackpotSpell;
		diagInfoTab[1] = INFO_DIAG_MB_DIRECT;
		for(i=0; i<4; ++i) {
		j = jackpotLiteTables[i];
		if(i==jackpotNo)
			lightCommand[j] = LIGHT_TURN_BLINK;
		else	lightCommand[j] = LIGHT_TURN_OFF;
		}
		i = 0;
		if(jackpotNo==0 && ply->rampRollOverOn[1]!=0) i++;
		if(jackpotNo==1 && ply->laneRollOverOn[1]!=0) i++;
		if(jackpotNo==2 && ply->rampRollOverOn[0]!=0) i++;
		if(jackpotNo==3 && ply->laneRollOverOn[0]!=0) i++;
		if(i) {
		ply->multiBallCompFlag = 1;
		jackpotNo++;
		jackpotNo &= 3;
		RingSfx(SFX_DRM_JACKPOT);
		SetInfomations(INFO_ANIME_JACKPOT, 5);
		while(i) { AddScore(ply->score, ply->jackpot); i--; }
		jackpotSpell++;
		lightAnimePtr = liteBLK4;
		}
		if(jackpotSpell >= 9) {
		for(i=0; i<4; ++i) {
			j = jackpotLiteTables[i];
			lightCommand[j] = LIGHT_TURN_OFF;
			}
		diagInfoTab[0] = INFO_DIAG_MOON_MULTI;
		diagInfoTab[1] = -1;
		ply->multiBallGo = 4;
		}
		ServiceExitMulti(ply, 5);
		break;
	case 4:
		lightCommand[LITE_DRM_MOON_HOLE_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->holeOn[3]) {
		nextStageEnable |= 1;
		RingSfx(SFX_DRM_NIGHT_JACKPOT);
		lightCommand[LITE_DRM_MOON_HOLE_JACKPOT] = LIGHT_TURN_OFF;
		SetInfomations(INFO_ANIME_SP_JACKPOT, 6);
		lightAnimePtr = liteNightMare;
		i = 15;
		while(i) { AddScore(ply->score, ply->jackpot); i--; }
		roundTimer = 30 * frameRate;
		jackpotNo = 0;
		jackpotSpell = 0;
		ply->multiBallGo = 3;
		}
		ServiceExitMulti(ply, 5);
		break;
	case 5:
		if(ply->multiBallCompFlag==0) {
			ply->multiBallRestart = frameRate * 30;
/*			ply->multiBallReady = 1; */
			}
		else	ply->countUpMulti = 0;
		ply->ballSaveLimit = 1;
		lightCommand[LITE_DRM_MOON_HOLE_JACKPOT] = LIGHT_TURN_OFF;
		for(i=0; i<4; ++i) {
		j = jackpotLiteTables[i];
		lightCommand[j] = LIGHT_TURN_OFF;
		diagInfoTab[i] = -1;
		}
		magnaLockEnable = 0;
		ply->multiReadyCount = 0;
		lightAnimePtr = NULL;
		RingSfx(SFX_TIME_BACK);
		slaveTaskNo = 21;
		ply->multiBallGo = 0;
		ply->vukModeDelay = frameRate * 2;
		break;
	default:
		break;
	}

	if(ply->multiBallGo) {
		j = 1;
		for(i=0; i<3; ++i) j &= ply->standUpFlag[i+6];
		if(j)	{
			AddScore(ply->jackpot, scoreConst[SCORE_0025M]);
			for(i=0; i<3; ++i) ply->standUpFlag[i+6] = 0;
			}
		if(CmpScore(ply->jackpot, scoreConst[SCORE_100M])==0)
			PresetScore(ply->jackpot, scoreConst[SCORE_100M]);
	}
}

/*----------------------------------------------/
/	Round Service				/
/----------------------------------------------*/
static void ServiceRound(PLAYER *ply)
{
	int	i, j, k;

	if(ply->multiBallGo) return;

	switch(ply->roundGoNo)
	{
	case 0:
		ServiceRoundStart(ply);
		break;
/*--------------------------------------/
/	Lite Dreamer			/
/--------------------------------------*/
	case 1:
		ServiceMysteryRound(ply);
		break;
	case 2:
	case 3:
	case 4:
/*--------------------------------------/
/	End Of Round			/
/--------------------------------------*/
	case 5:
		ply->vukModeDelay = frameRate * 2;
		ServiceRoundEnd(ply);
		break;
	case 6:
	case 7:
	case 8:
/*--------------------------------------/
/	SOUL				/
/--------------------------------------*/
	case 9:
		lightCommand[LITE_DRM_STANDUP_TARGET_4] = LIGHT_TURN_OFF;
		lightCommand[LITE_DRM_STANDUP_TARGET_5] = LIGHT_TURN_OFF;
		lightCommand[LITE_DRM_STANDUP_TARGET_6] = LIGHT_TURN_OFF;
		soulTask = 0;
		ply->roundGoNo++;
		break;
	case 10:
		ServiceSoul(ply, 5);
		break;
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
/*--------------------------------------/
/	Silver Gate			/
/--------------------------------------*/
	case 17:
		ply->multiBallEnable = 0;
		if(ply->superRoundEnable) i = SCORE_100M;
		else i = SCORE_050M;
		PresetScore(roundValue, scoreConst[i]);
		roundTimer = -1;
		ply->roundGoNo++;
		break;
	case 18:
		CreateInfoScore(roundValue);
		diagInfoTab[0] = INFO_DIAG_SILVER_DIRECT;
		lightCommand[LITE_DRM_MOON_HOLE_JACKPOT] = LIGHT_TURN_BLINK;
		if(roundTimer < 0) {
		if(ply->superRoundEnable) {
			AddScore(roundValue, scoreConst[SCORE_SILVER]);
			i = SCORE_200M;
			}
		else i = SCORE_100M;
		AddScore(roundValue, scoreConst[SCORE_SILVER]);
		if(CmpScore(roundValue, scoreConst[i])==0) {
			PresetScore(roundValue, scoreConst[i]);
			roundTimer = 5 * frameRate;
			}
		}
		if(ply->holeOn[3]) {
			RingSfx(SFX_DRM_SILVER_AWARD);
			AddScore(ply->roundScore, roundValue);
			lightAnimePtr = liteWBT_BLK_WBT_BLK;
			}
		if(roundTimer > 0) roundTimer--;
		if(roundTimer==0) ply->roundGoNo = 20;
		RoundClose(ply, 20);
		break;
	case 19:
		infoTimer = roundTimer / frameRate;
		CreateInfoScore(roundValue);
		diagInfoTab[0] = INFO_DIAG_SIL_T_DIRECT;
		lightCommand[LITE_DRM_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_DRM_MOON_HOLE_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->holeOn[3]) {
			RingSfx(SFX_DRM_SILVER_AWARD);
			lightAnimePtr = liteWBT_BLK_WBT_BLK;
			if(ply->superRoundEnable)
				AddScore(ply->roundScore, roundValue);
			AddScore(ply->roundScore, roundValue);
			ply->roundGoNo = 20;
			}
		roundTimer--;
		if(roundTimer<=0) ply->roundGoNo = 20;
		RoundClose(ply, 20);
		break;
	case 20:
/*		lightCommand[LITE_DRM_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF; */
		lightCommand[LITE_DRM_MOON_HOLE_JACKPOT] = LIGHT_TURN_OFF;
		ply->roundGoNo = 5;
		break;
	case 21:
	case 22:
	case 23:
	case 24:
/*--------------------------------------/
/	Enchanted Wood			/
/--------------------------------------*/
	case 25:
		ply->multiBallEnable = 0;
		magnaLockEnable = 1;
		magnaFlag = 1;
		magnaTimer = 0;
		LaunchBallSaver(ply, 3);
		woodCount = 0;
		if(ply->superRoundEnable)
			PresetScore(roundValue, scoreConst[SCORE_200K]);
		else	PresetScore(roundValue, scoreConst[SCORE_100K]);
		ply->roundGoNo = 27;
		break;
	case 26:
	case 27:
		CreateInfoScore(roundValue);
		diagInfoTab[0] = INFO_DIAG_WOOD_DIRECT;
		j = 0;
		for(i=0; i<3; ++i) if(ply->jetBumperOn[i]) j++;
		while(j) {
		RingSfx(SFX_DRM_WOOD_AWARD);
		AddScore(ply->roundScore, roundValue);
		if(ply->superRoundEnable) {
			k = SCORE_010M;
			AddScore(roundValue, scoreConst[SCORE_200K]);
			}
		else	{
			k = SCORE_005M;
			AddScore(roundValue, scoreConst[SCORE_100K]);
			}
		if(CmpScore(roundValue, scoreConst[k])==0)
			PresetScore(roundValue, scoreConst[k]);
		woodCount++;
		j--;
		}
		if((woodCount % 10)==0) lightAnimePtr = liteTCW_TCC_WBT;
		ServiceExitRound(ply, 28);
		break;
	case 28:
		magnaFlag = 0;
		magnaLockEnable = 0;
		ply->roundGoNo = 5;
		break;
	case 29:
	case 30:
	case 31:
	case 32:
		break;
/*--------------------------------------/
/	Moon Beast			/
/--------------------------------------*/
	case 33:
		ply->multiBallEnable = 0;
		LaunchBallSaver(ply, 2);
		if(ply->superRoundEnable)
			PresetScore(roundValue, scoreConst[SCORE_020M]);
		else	PresetScore(roundValue, scoreConst[SCORE_010M]);
		ply->roundGoNo = 35;
		break;
	case 34:
	case 35:
		CreateInfoScore(roundValue);
		diagInfoTab[0] = INFO_DIAG_MOON_DIRECT;
		lightCommand[LITE_DRM_CENTER_HOLE_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->holeOn[0]) {
		RingSfx(SFX_DRM_MOON_AWARD);
		AddScore(ply->roundScore, roundValue);
		if(ply->superRoundEnable)
			AddScore(roundValue, scoreConst[SCORE_010M]);
		AddScore(roundValue, scoreConst[SCORE_010M]);
		if(ply->superRoundEnable) k = SCORE_100M;
		else k = SCORE_050M;
		if(CmpScore(roundValue, scoreConst[k])==0)
			PresetScore(roundValue, scoreConst[k]);
		lightAnimePtr = liteWBT2;
		}
		ServiceExitRound(ply, 36);
		break;
	case 36:
		lightCommand[LITE_DRM_CENTER_HOLE_JACKPOT] = LIGHT_TURN_OFF;
		ply->roundGoNo = 5;
		break;
	case 37:
	case 38:
	case 39:
	case 40:
		break;
/*--------------------------------------/
/	NECRONOMICON			/
/--------------------------------------*/
	case 41:
		ply->multiBallEnable = 0;
		ply->switchTotal = 0;
		for(i=0; i<6; ++i) necroFlag[i] = 0;
		LaunchBallSaver(ply, 2);
		necroMoonLimit = 1;
		necroMoonCount = 0;
		necroFlag[5] = 1;
		ply->roundGoNo++;
		break;
	case 42:
		ServiceMultiScore(ply, SCORE_100K, INFO_DIAG_MULTI_DIRECT);
		i = ply->switchFrame;
		if(i) RingSfx(SFX_DRM_NECRO_AWARD);
		while(i) {
		AddScore(ply->roundScore, roundValue);
		i--;
		}
		if(ply->superRoundEnable) {
			j = SCORE_020M;
			k = INFO_ANIME_20M;
			}
		else	{
			j = SCORE_010M;
			k = INFO_ANIME_10M;
			}
		i = 0;
		if(ply->rampRollOverOn[0]!=0 && necroFlag[0]==0) {
		necroFlag[0] = 1;
		AddScore(ply->roundScore, scoreConst[j]);
		SetInfomations(k, 5);
		RingSfx(SFX_DRM_NECRO_AWARD);
		i++;
		}
		if(ply->rampRollOverOn[1]!=0 && necroFlag[1]==0) {
		necroFlag[1] = 1;
		AddScore(ply->roundScore, scoreConst[j]);
		SetInfomations(k, 5);
		RingSfx(SFX_DRM_NECRO_AWARD);
		i++;
		}
		if(ply->laneRollOverOn[0]!=0 && necroFlag[2]==0) {
		necroFlag[2] = 1;
		AddScore(ply->roundScore, scoreConst[j]);
		SetInfomations(k, 5);
		RingSfx(SFX_DRM_NECRO_AWARD);
		i++;
		}
		if(ply->laneRollOverOn[1]!=0 && necroFlag[3]==0) {
		necroFlag[3] = 1;
		AddScore(ply->roundScore, scoreConst[j]);
		SetInfomations(k, 5);
		RingSfx(SFX_DRM_NECRO_AWARD);
		i++;
		}
		if(ply->holeOn[0]!=0 && necroFlag[4]==0) {
		necroFlag[4] = 1;
		AddScore(ply->roundScore, scoreConst[j]);
		SetInfomations(k, 5);
		RingSfx(SFX_DRM_NECRO_AWARD);
		i++;
		}

		if(i) {
		lightAnimePtr = liteTCW_TCC_WBT_WBT;
		RingSfx(SFX_DRM_NECRO_ADD_AWARD);
		}

		while(i) {
		AddBallSaver(ply);
		i--;
		}

		j = 1;
		for(i=0; i<5; ++i) j &= necroFlag[i];
		if(j)	{
			necroFlag[5] = 0;
			infoNumber = necroMoonLimit - necroMoonCount;
			diagInfoTab[1] = INFO_DIAG_MOON_RESTART;
			}
		if(ply->holeOn[3]!=0 && j!=0) {
		necroMoonCount++;
		if(necroMoonCount >= necroMoonLimit) {
			necroMoonLimit++;
			necroMoonCount = 0;
			for(i=0; i<5; ++i) necroFlag[i] = 0;
			necroFlag[5] = 1;
			AddScore(ply->roundScore, scoreConst[j]);
			if(ply->superRoundEnable) {
				i = INFO_ANIME_100M;
				j = SCORE_100M;
				}
			else	{
				i = INFO_ANIME_50M;
				j = SCORE_050M;
				}
			AddScore(ply->score, scoreConst[j]);
			SetInfomations(i, 5);
			RingSfx(SFX_DRM_NECRO_AWARD);
			}
		}

		for(i=0; i<6; ++i) {
		if(necroFlag[i])
			lightCommand[necroLightIndex[i]] = LIGHT_TURN_OFF;
		else	lightCommand[necroLightIndex[i]] = LIGHT_TURN_BLINK;
		}
		ServiceExitRound(ply, 43);
		break;
	case 43:
		nextStageEnable |= 2;
		for(i=0; i<6; ++i)
			lightCommand[necroLightIndex[i]] = LIGHT_TURN_OFF;
		ply->roundGoNo = 5;
		break;
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
		break;
	default:
		break;
	}
}

static void ServiceRoundStart(PLAYER *ply)
{
	int	i, j;

	if(ply->multiBallGo==0) {
	if(ply->laneRollOverOn[1]) {
		if(ply->roundStartReady) ply->superRoundEnable = 1;
		ply->roundStartReady = 1;
		}
	}

	if(ply->roundStartReady!=0 && ply->multiBallReady==0) {
	if(ply->holeOn[3]) LaunchRound(ply);
	}

	if(roundStartDelay) {
	roundStartDelay--;
	if(roundStartDelay == (2 * frameRate)) {
	i = roundAudioTables[ply->roundNumber * 2];
	j = roundAudioTables[ply->roundNumber * 2 + 1];
	if(i >= 0 && optionMenuPad[OPTION_NO_BGM]!=0) PlayCD(i, j);
	}
	if(roundStartDelay==0) {
		lightCommand[LITE_DRM_CENTER_HOLE_JACKPOT] = LIGHT_TURN_OFF;
		ply->roundGoNo = ply->roundNumber * 8 + 1;
		ply->roundCompFlag[ply->roundNumber] = 1;
		ply->roundStartReady = 0;
		ResetScore(ply->roundScore);
		}
	}
}
