//=========================================================================//
/*! @file
	@brief  Necronomicon BLOODY
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
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
#include "etc.h"
#include "bloody.h"

static void ServiceMultiBall(PLAYER *ply, char *sct);
static void ServiceJackpot(PLAYER *ply);
static void ServiceRound(PLAYER *ply);

static char saucerServiceEnable;
static char eclipseCount;
static char machineCount;
static char angelsFlag[2];
static char roundAward[16];
static char jackpotFlag[3];
static char jackpotSpell;
static short riseDelay;
static short multiBallTimer;
static short multiBallCharge;
static short eclipseTimer;
static short switchAll, switchLimit;
static short distraCompCount;
static short saucerDelay1, saucerDelay2;
static short angelsDelay;

static short lightPosTables[] = {
	-56,136,56,-92,
/* POS */
	0,0,
	-10,50,
	-35,40,
	16,48,
	-21,40,
	28,12,
	-22,12,
	-19,16,
	-15,20,
	-10,22,
	-5,24,
	-4,59,
	0,55,
	5,51,
	13,33,
	14,26,
	15,20,
	-13,63,
	-35,6,
	-15,-5,
	0,-16,
	15,-5,
	9,12,
	-9,12,
	-8,40,
	-27,-4,
	-20,-13,
	8,129,
	19,129,
	29,129,
	-15,-21,
	-10,-23,
	-5,-25,
	0,-26,
	5,-25,
	10,-23,
	15,-21,
	-39,-21,
	-31,-21,
	31,-21,
	39,-21,
	0,-59,
	39,-31,
	-39,-35,
/* JETS */
	4,94,
	19,77,
	31,95,
/* SLING */
	23,-34
	-23,-34,
};
static char jackpotLiteIndex[] = {
	LITE_CLT_RIGHTRAMP_JACKPOT,
	LITE_CLT_LEFTRAMP_JACKPOT,
	LITE_CLT_RIGHTLANE_JACKPOT,
	LITE_CLT_LEFTLANE_JACKPOT,
	LITE_CLT_LEFT_SAUCER_JACKPOT,
	LITE_CLT_CENTER_SAUCER_JACKPOT
};
static char roundAudioTables[] = {
	-1, -1,
	CDA_BLD_SOUL_START, CDA_BLD_SOUL_PLAY,
	CDA_BLD_ECLIPSE_START, CDA_BLD_ECLIPSE_PLAY,
	CDA_BLD_ANGEL_START, CDA_BLD_ANGEL_PLAY,
	CDA_BLD_MACHINE_START, CDA_BLD_MACHINE_PLAY,
	CDA_BLD_GOD_START, CDA_BLD_GOD_PLAY
};

void InitBloody(PLAYER *ply)
{
	lightPosPtr = &lightPosTables[0];
	ply->vukEnable = 1;
	ply->gateEnable[DIVERTER1_TRUE] = 1;
	ply->gateEnable[DIVERTER1_FALSE] = 0;
	ply->multiBallSpell = 5;
	PresetScore(ply->jackpot, scoreConst[SCORE_010M]);
}

void InitBloodyPlay(PLAYER *ply)
{
	ply->saucerEnable[0] = 0;
	ply->saucerEnable[1] = 0;
	saucerServiceEnable = 1;
	ply->multiBallEnable = 1;
	riseDelay = 0;
}

void ServiceBloody(PLAYER *ply, char *sct)
{
	int	i, j, f;

	ServiceWisdom(ply);

/*------------------------------------------------------/
/	SAUCER - Controle				/
/------------------------------------------------------*/
	if(saucerServiceEnable) {
	i = 3 *frameRate;
	if(ply->mysteryReady!=0 && ply->extraBallReady!=0
		&& ply->multiBallGo==0 && ply->roundGoNo==0) i += 3 * frameRate;
	if(ply->saucerOn[0]) saucerDelay1 = i;
	if(ply->saucerOn[1]) saucerDelay2 = 3 *frameRate;

	if(saucerDelay1) {
		saucerDelay1--;
		if(saucerDelay1==0) ply->saucerEnable[0] = 1;
		}

	if(saucerDelay2) {
		saucerDelay2--;
		if(saucerDelay2==0) ply->saucerEnable[1] = 1;
		}
	}

/*------------------------------------------------------/
/	MYSTERY / EXTRA-BALL				/
/------------------------------------------------------*/
	i = ply->extraBallReady | ply->mysteryReady;
	j = ply->roundGoNo | ply->multiBallGo;

	if(i!=0 && ply->saucerOn[0]) {
	if(ply->extraBallReady) {
		RingSfx(SFX_EXTRABALL_AWARD);
		if(ply->extraBallCount < 3) {
			ply->extraBallCount++;
			ply->extraBallCycle++;
			if(j==0) SetMultiInfo(INFO_ANIME_EXTRA_BALL, 5);
			}
		else	{
			AddScore(ply->score, scoreConst[SCORE_030M]);
			if(j==0) SetMultiInfo(INFO_ANIME_30M, 5);
			}
		ply->extraBallReady--;
		}
	if(ply->mysteryReady) {
		if(j==0) f = 1; else f = 0;
		TakeMystery(ply, f);
		ply->mysteryReady--;
		}
	}

	ServiceLoopBonus(ply);
	ServiceMultiBall(ply, sct);
	ServiceRound(ply);
}

void DoBloodyMultiBallReady(PLAYER *ply)
{
	ply->multiBallReady = 1;
}

static void ServiceMultiBall(PLAYER *ply, char *sct)
{
	int	i, j, k, l;

	if(ply->multiBallEnable==0) return;

	switch(ply->multiBallGo)
	{
	case 0:
		i = ply->rampRollOverOn[0] | ply->rampRollOverOn[1];
		if(ply->roundGoNo) i = 0;
		if(i!=0 && ply->multiBallReady==0) {
		ply->multiBallSpell++;
		diagInfoTab[2] = INFO_DIAG_MACHINE + ply->multiBallSpell;
		if(ply->multiBallSpell >= 15) ply->multiBallReady = 1;
		}
		if(ply->multiBallReady!=0 || ply->multiBallRestart!=0) {
		if(ply->saucerOn[1]) {
			slaveTaskNo = 22;
			for(i=0; i<3; ++i) jackpotFlag[i] = 0;
			multiBallCharge = 0;
			LaunchBallSaver(ply, 3);
			ply->multiBallRestart = 0;
			ply->multiBallReady = 0;
			ply->multiBallGo = 1;
			if(ply->roundGoNo) ply->roundGoNo = 5;
			}
		}
		break;
/*	3 Ball Multi	*/
	case 1:
		l = multiBallCharge;
		infoNumber = multiBallCharge;
		if(multiBallCharge >= 100) {
		diagInfoTab[0] = INFO_DIAG_MB_HOLE;
		diagInfoTab[1] = INFO_DIAG_ENERGY_FULL;
		lightCommand[LITE_CLT_CENTER_SAUCER_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->saucerOn[1]) {
		lightCommand[LITE_CLT_CENTER_SAUCER_JACKPOT] = LIGHT_TURN_OFF;
			RingSfx(SFX_CLT_SUPER_JACKPOT);
			SetInfomations(INFO_ANIME_SP_JACKPOT, 6);
			AddScore(ply->score, ply->jackpot);
			AddScore(ply->score, ply->jackpot);
			AddScore(ply->score, ply->jackpot);
			lightAnimePtr = liteBLK8;
			LaunchBallSaver(ply, 7);
			jackpotSpell = 0;
			multiBallCharge = 0;
			ply->multiBallGo = 2;
			}
		}
		else	{
			diagInfoTab[0] = INFO_DIAG_MB_DIRECT;
			diagInfoTab[1] = INFO_DIAG_ENERGY_DIRECT;
			}
		ServiceJackpot(ply);
		if(l < 100 && multiBallCharge >= 100)
			RingSfx(SFX_CLT_FULL_CHARGE);
		ServiceExitMulti(ply, 5);
		break;
/*	7 Ball Multi	*/
	case 2:
		if(multiBallCharge >= 100) ply->multiBallGo = 3;
		l = multiBallCharge;
		infoNumber = multiBallCharge;
		diagInfoTab[0] = INFO_DIAG_MB_DIRECT;
		diagInfoTab[1] = INFO_DIAG_ENERGY_DIRECT;
		lightCommand[LITE_CLT_CENTER_SAUCER_JACKPOT] = LIGHT_TURN_OFF;
		for(i=0; i<3; ++i) jackpotFlag[i] = 0;
		ServiceJackpot(ply);
		if(l < 100 && multiBallCharge >= 100)
			RingSfx(SFX_CLT_FULL_CHARGE);
		ServiceExitMulti(ply, 5);
		break;
/*	7 Ball Multi (AWAKE THE MACHINE)	*/
	case 3:
		infoNumber = multiBallCharge;
		diagInfoTab[0] = INFO_DIAG_MB_HOLE;
		diagInfoTab[1] = INFO_DIAG_ENERGY_FULL;
		for(i=0; i<3; ++i) {
		lightCommand[jackpotLiteIndex[i]] = LIGHT_TURN_OFF;
		}
		lightCommand[LITE_CLT_CENTER_SAUCER_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->saucerOn[1]) {
			RingSfx(SFX_CLT_POWER_JACKPOT);
			SetInfomations(INFO_ANIME_SPR_JACKPOT, 6);
			lightAnimePtr = liteBLK8;
			for(i=0; i<7; ++i)
				AddScore(ply->score, ply->jackpot);
			jackpotSpell++;
			if(jackpotSpell==1) i = 5;
			else if(jackpotSpell==2) i = 8;
			else if(jackpotSpell==3) i = 15;
			diagInfoTab[2] = INFO_DIAG_MACHINE + i;
			multiBallCharge = 0;
			if(jackpotSpell >= 3) {
				multiBallTimer = 30 *frameRate;
				ply->multiBallGo = 4;
				}
			else ply->multiBallGo = 2;
			}
		ServiceExitMulti(ply, 5);
		break;
/*	7 Ball Multi (AWAKE THE MACHINE)	*/
	case 4:
		infoTimer = multiBallTimer / frameRate;
		if(infoTimer < 10) {
		if((multiBallTimer & frameRate)==0) lightAnimePtr = liteWBT;
		}
		diagInfoTab[0] = INFO_DIAG_MACHINE_JAC;
		diagInfoTab[1] = -1;
		if(ply->saucerOn[1]) {
		nextStageEnable |= 1;
		SetInfomations(INFO_ANIME_DARK_JACKPOT, 6);
		RingSfx(SFX_CLT_MACHINE_JACKPOT);
		for(i=0; i<15; ++i)
			AddScore(ply->score, ply->jackpot);
		lightAnimePtr = liteTCW_TCC_WBT_WTB;
		multiBallTimer = 0;
		}
		if(multiBallTimer) multiBallTimer--;
		else {
		jackpotSpell = 0;
		ply->multiBallGo = 2;
		}
		ServiceExitMulti(ply, 5);
		break;
/*	End Of MultiBall	*/
	case 5:
		diagInfoTab[0] = -1;
		diagInfoTab[1] = -1;
		for(i=0; i<5; ++i) {
		lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_OFF;
		}
		for(i=0; i<6; ++i) {
		lightCommand[jackpotLiteIndex[i]] = LIGHT_TURN_OFF;
		}
		ply->multiBallSpell = 0;
		slaveTaskNo = 23;
		RingSfx(SFX_TIME_BACK);
		ply->multiBallGo = 0;
		break;
	default:
		break;
	}

/*----------------------------------------------/
/	RISE JACKPOT				/
/----------------------------------------------*/
	if(ply->multiBallGo) {
	j = 1;
	for(i=0; i<3; ++i) {
	j &= ply->standUpFlag[i + 3];
	}
	if(j)	{
		for(i=0; i<3; ++i) ply->standUpFlag[i + 3] = 0;
		riseDelay = 3 * frameRate;
		}

	if(j)	{
	AddScore(ply->jackpot, scoreConst[SCORE_0025M]);
	if(CmpScore(ply->jackpot, scoreConst[SCORE_100M])==0)
			PresetScore(ply->jackpot, scoreConst[SCORE_100M]);
		}
	for(i=0; i<3; ++i) {
		if(riseDelay) j = LIGHT_TURN_BLINK;
		else if(ply->standUpFlag[i + 3]) j = LIGHT_TURN_ON;
		else j = LIGHT_TURN_OFF;
		lightCommand[LITE_CLT_STANDUP_TARGET_1 + i] = j;
		}
	if(riseDelay) riseDelay--;
	}
}

static void ServiceJackpot(PLAYER *ply)
{
	int	i, j, k;

	k = -1;
	for(i=0; i<3; ++i) {
	if(multiBallCharge >= 100) j = LIGHT_TURN_OFF;
	else if(jackpotFlag[i]) j = LIGHT_TURN_ON;
	else	{
		if(i==0 && ply->rampRollOverOn[0]!=0) k = 0;
		else if(i==1 && ply->rampRollOverOn[1]!=0) k = 1;
		else if(i==2 && ply->laneRollOverOn[0]!=0) k = 2;
		j = LIGHT_TURN_BLINK;
		}
	lightCommand[jackpotLiteIndex[i]] = j;
	}
	multiBallCharge += ply->switchFrame;
	j = 1;
	for(i=0; i<3; ++i) {
	multiBallCharge += ply->standUpOn[i + 3];
	j &= ply->standUpFlag[i + 3];
	}
	if(j) multiBallCharge += 5;
	if(k >= 0) {
	ply->multiBallCompFlag = 1;
	jackpotFlag[k] = 1;
	SetInfomations(INFO_ANIME_JACKPOT, 5);
	RingSfx(SFX_JACKPOT);
	RingSfx(SFX_CLT_JACKPOT);
	k++;
	while(k) {
		AddScore(ply->score, ply->jackpot);
		k--;
		}
	lightAnimePtr = liteBLK4;
	}
	for(i=0; i<5; ++i) {
	j = multiBallCharge / 20;
	if(j >= 5)
		lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_BLINK;
	else if(i < j)
		lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_ON;
	else	lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_OFF;
	}
	if(ply->rampRollOverOn[0]) multiBallCharge += 7;
	if(ply->rampRollOverOn[1]) multiBallCharge += 7;
	if(ply->laneRollOverOn[0]!=0) multiBallCharge += 15;
}

/*----------------------------------------------/
/	Round Service				/
/----------------------------------------------*/
static void ServiceRound(PLAYER *ply)
{
	int	i, j;
	unsigned int	k;
	char	tmp[12];

	if(ply->multiBallGo) return;

	for(i=0; i<3; ++i) {
	if(roundDelay)
		lightCommand[LITE_CLT_STANDUP_TARGET_1 + i] = LIGHT_TURN_BLINK;
	else if(ply->standUpFlag[i + 3])
		lightCommand[LITE_CLT_STANDUP_TARGET_1 + i] = LIGHT_TURN_ON;
	else	lightCommand[LITE_CLT_STANDUP_TARGET_1 + i] = LIGHT_TURN_OFF;
	}
	if(roundDelay) roundDelay--;

	switch(ply->roundGoNo)
	{
	case 0:
		j = 1;
		for(i=0; i<3; ++i) j &= ply->standUpFlag[i + 3];
		if(j)	{
		for(i=0; i<3; ++i) ply->standUpFlag[i + 3] = 0;
		AddScore(ply->score, scoreConst[SCORE_300K]);
		roundDelay = frameRate * 2;
		if(ply->roundStartReady) ply->superRoundEnable = 1;
		ply->roundStartReady = 1;
		}

		if(ply->saucerOn[1]!=0 && ply->roundStartReady!=0) {
		LaunchRound(ply);
		}

		if(roundStartDelay) {
		lightCommand[LITE_CLT_STANDUP_TARGET_1] = LIGHT_TURN_OFF;
		lightCommand[LITE_CLT_STANDUP_TARGET_2] = LIGHT_TURN_OFF;
		lightCommand[LITE_CLT_STANDUP_TARGET_3] = LIGHT_TURN_OFF;

		roundStartDelay--;
		if(roundStartDelay == (2 * frameRate)) {
		i = roundAudioTables[ply->roundNumber * 2];
		j = roundAudioTables[ply->roundNumber * 2 + 1];
		if(i >= 0 && optionMenuPad[OPTION_NO_BGM]!=0) PlayCD(i, j);
		}
		if(roundStartDelay==0) {
			ply->roundGoNo = ply->roundNumber * 8 + 1;
			ply->roundCompFlag[ply->roundNumber] = 1;
			ply->roundStartReady = 0;
			ResetScore(ply->roundScore);
			}
		}
		break;
/*----------------------------------------------/
/	Lite Ancient				/
/----------------------------------------------*/
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
		ServiceRoundEnd(ply);
		break;
	case 6:
	case 7:
	case 8:
/*----------------------------------------------/
/	Soul					/
/----------------------------------------------*/
	case 9:
		soulTask = 0;
		ply->multiBallEnable = 1;
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
/*----------------------------------------------/
/	Eclipse					/
/----------------------------------------------*/
	case 17:
		ply->multiBallEnable = 1;
		eclipseCount = 0;
		eclipseTimer = 0;
		if(ply->superRoundEnable) i = SCORE_010M; else i = SCORE_005M;
		PresetScore(roundValue, scoreConst[i]);
		SetRoundTimer(ply, 40);
		ply->roundGoNo++;
		break;
	case 18:
		infoTimer = roundTimer / frameRate;
		j = 0;
		for(i=0; i<4; ++i) {
		lightCommand[jackpotLiteIndex[i]] = LIGHT_TURN_BLINK;
		if(ply->rampRollOverOn[0]) j++;
		if(ply->rampRollOverOn[1]) j++;
		if(ply->laneRollOverOn[0]) j++;
		if(ply->laneRollOverOn[1]) j++;
		}
		CreateInfoScore(roundValue);
		diagInfoTab[0] = INFO_DIAG_ECLIPS_DIRECT;

		if(j) {
		lightAnimePtr = liteTCW_TCC;
		RingSfx(SFX_ROUND_JACKPOT);
		AddScore(ply->roundScore, roundValue);
		if(ply->superRoundEnable) i = SCORE_010M; else i = SCORE_005M;
		AddScore(roundValue, scoreConst[i]);
		eclipseCount++;
		}

		if(eclipseCount > 5 && j!=0) eclipseTimer = 2 * frameRate;
		for(i=0; i<5; ++i) {
		if(eclipseTimer)
			lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_BLINK;
		else if(i < eclipseCount)
			lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_ON;
		else	lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_OFF;
		}
		if(eclipseTimer) eclipseTimer--;

		ResetScore(tmp);
		AddScore(tmp, roundValue);
		AddScore(tmp, roundValue);
		AddScore(tmp, roundValue);
		infoText[3] = CreateInfoScores(roundAward ,tmp);

		if(eclipseCount >= 5) {
		diagInfoTab[1] = INFO_DIAG_STONE_DIRECT;
		lightCommand[LITE_CLT_CENTER_SAUCER_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->saucerOn[1]) {
			lightAnimePtr = liteWBT_WTB_TCW_TCC;
			RingSfx(SFX_CLT_ECLIPSE_AWARD);
			AddScore(ply->roundScore, tmp);
			ply->roundGoNo = 5;
			}
		}

		if(roundTimer) roundTimer--;
		if(roundTimer==0) ply->roundGoNo = 19;
		RoundClose(ply, 19);
		break;
	case 19:
		for(i=0; i<5; ++i) {
		lightCommand[LITE_CLT_20_CHARGE + i] = LIGHT_TURN_OFF;
		}
		ply->roundGoNo = 5;
		break;
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
/*----------------------------------------------/
/	Dark Angels				/
/----------------------------------------------*/
	case 25:
		saucerServiceEnable = 0;
		ply->multiBallEnable = 0;
		LaunchBallSaver(ply, 2);
		if(ply->superRoundEnable) i = SCORE_020M; else i = SCORE_010M;
		PresetScore(roundValue, scoreConst[i]);
		angelsFlag[0] = angelsFlag[1] = 0;
		roundTimer = -1;
		angelsDelay = 0;
		ply->roundGoNo++;
		break;
	case 26:
		for(i=0; i<2; ++i) {
		if(angelsFlag[i]==0)
			lightCommand[jackpotLiteIndex[i+4]] = LIGHT_TURN_BLINK;
		else	lightCommand[jackpotLiteIndex[i+4]] = LIGHT_TURN_ON;
		}

		i = 0;
		if(ply->saucerOn[0]) { angelsFlag[0] = 1; i++; }
		if(ply->saucerOn[1]) { angelsFlag[1] = 1; i++; }
		if(i) {
		lightAnimePtr = liteWBT3;
		roundTimer = 20 * frameRate;
		}

		if(roundTimer > 0) roundTimer--;
		if(roundTimer==0) {
		for(i=0; i<2; ++i) {
			if(angelsFlag[i]) ply->saucerEnable[i] = 1;
			angelsFlag[i] = 0;
			}
		roundTimer = -1;
		}

		if(angelsFlag[0]!=0 && angelsFlag[1]!=0 && angelsDelay==0) {
		angelsDelay = 2 * frameRate;
		RingSfx(SFX_CLT_ANGELS_AWARD);
		AddScore(ply->roundScore, roundValue);
		if(ply->superRoundEnable) i = SCORE_020M; else i = SCORE_010M;
		AddScore(roundValue, scoreConst[i]);
		if(ply->superRoundEnable) i = SCORE_100M; else i = SCORE_050M;
		if(CmpScore(roundValue, scoreConst[i])==0)
					PresetScore(roundValue, scoreConst[i]);
		roundTimer = -1;
		}

		if(angelsDelay) {
		angelsDelay--;
		if(angelsDelay==0) {
		for(i=0; i<2; ++i) {
				if(angelsFlag[i]) ply->saucerEnable[i] = 1;
				angelsFlag[i] = 0;
				}
			}
		}
		CreateInfoScore(roundValue);
		if(angelsDelay) diagInfoTab[0] = -1;
		else if(angelsFlag[0] | angelsFlag[1]) {
		infoTimer = roundTimer / frameRate;
		diagInfoTab[0] = INFO_DIAG_ANGELS_AWARD;
		}
		else diagInfoTab[0] = INFO_DIAG_ANGELS_DIRECT;

		ServiceExitRound(ply, 27);
		break;
	case 27:
		saucerServiceEnable = 1;
		for(i=0; i<2; ++i) {
		if(angelsFlag[i]) ply->saucerEnable[i] = 1;
		}
		ply->roundGoNo = 5;
		break;
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
/*----------------------------------------------/
/	Machine					/
/----------------------------------------------*/
	case 33:
		ply->multiBallEnable = 0;
		LaunchBallSaver(ply, 3);
		if(ply->superRoundEnable) i = SCORE_020M; else i = SCORE_010M;
		PresetScore(roundValue, scoreConst[i]);
		machineCount = 0;
		ply->roundGoNo++;
		break;
	case 34:
		CreateInfoScore(roundValue);
		if(machineCount & 1) {
		diagInfoTab[0] = INFO_DIAG_SOLAR_AWARD;
		lightCommand[LITE_CLT_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_CLT_LEFTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
			}
		else	{
		diagInfoTab[0] = INFO_DIAG_LUNAR_AWARD;	
		lightCommand[LITE_CLT_RIGHTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
		lightCommand[LITE_CLT_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
			}
		for(i=0; i<2; ++i) {
		if((machineCount & 1)==i & ply->rampRollOverOn[i]!=0) {
			lightAnimePtr = liteWBT2;
			if(i==0) RingSfx(SFX_CLT_LUNAR_AWARD);
			else RingSfx(SFX_CLT_SOLAR_AWARD);
			AddScore(ply->roundScore, roundValue);
			machineCount++;
			}
		}
		j = 1;
		for(i=0; i<3; ++i) j &= ply->standUpFlag[i + 3];
		if(j) {
		roundDelay = frameRate * 2;
		for(i=0; i<3; ++i) ply->standUpFlag[i+3] = 0;		
		if(ply->superRoundEnable) j = SCORE_005M;
		else j = SCORE_0025M;
		AddScore(roundValue, scoreConst[j]);
		if(ply->superRoundEnable) j = SCORE_060M;
		else j = SCORE_030M;			
		if(CmpScore(roundValue, scoreConst[j])==0)
				PresetScore(roundValue, scoreConst[j]);
		}
		ServiceExitRound(ply, 5);
		break;
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
/*----------------------------------------------/
/	God Of Destruction			/
/----------------------------------------------*/
	case 41:
		ply->multiBallEnable = 0;
		SetRoundTimer(ply, 30);
		switchAll = 0;
		switchLimit = 20;
		distraCompCount = 0;
		LaunchBallSaver(ply, 2);
		ply->roundGoNo++;
		break;
	case 42:
		infoTimer = roundTimer / frameRate;
		if(roundTimer) {
			infoNumber = switchLimit - switchAll;
			if(infoNumber==1) j = INFO_DIAG_DIST_DIRECT;
			else j = INFO_DIAG_DISTS_DIRECT;
			}
		else	{
			j = INFO_DIAG_MULTI_DIRECT;
			}
		i = ply->multiBalls;
		ply->multiBalls += distraCompCount;
		ServiceMultiScore(ply, SCORE_300K, j);
		ply->multiBalls = i;

		if(ply->switchFrame) RingSfx(SFX_CLT_DESTR_AWARD);

		j = 1;
		for(i=0; i<3; ++i) j &= ply->standUpFlag[i + 3];
		if(j) {
		roundDelay = frameRate * 2;
		for(i=0; i<3; ++i) ply->standUpFlag[i+3] = 0;		
		distraCompCount++;
		}

		if(switchAll >= switchLimit && roundTimer!=0) {
		RingSfx(SFX_CLT_DESTR_ADD_AWARD);
		lightAnimePtr = liteTCW_TCC_WBT_WBT;
		AddBallSaver(ply);
		switchAll -= switchLimit;
		if(switchAll < 0) switchAll = 0;
		switchLimit += 10;
		SetRoundTimer(ply, 30);
		}
		switchAll += ply->switchFrame;
		if(roundTimer) roundTimer--;
		i = ply->switchFrame;
		while(i) { AddScore(ply->roundScore, roundValue); i--; }
		ServiceExitRound(ply, 43);
		break;
	case 43:
		nextStageEnable |= 2;
		ply->roundGoNo = 5;
		break;
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	default:
		break;
	}
}
