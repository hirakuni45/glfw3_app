//=========================================================================//
/*! @file
	@brief  Necronomicon ARKHAM
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
#include "fnc.h"
#include "cdfs.h"
#include "sim.h"
#include "etc.h"
#include "arkham.h"

static void ServiceMultiBall(PLAYER *ply, char *sct);
static void ServiceRound(PLAYER *ply);
static void ServiceRoundStart(PLAYER *ply);
static void ServiceDeathWalker(PLAYER *ply);
static void ServiceCeme(PLAYER *ply);

static short bingoHole, cemetDelay, cemetNo;
static short spellCount, spellTimer;
static short jackpotCountDown;
static short jackpotRiseTimer1, jackpotRiseTimer2;
static char divCon;
static char jackpotRise1, jackpotRise2;
static char bingoFlag[5];
static char doubleJackpotEnable;
static char jackpotFlag[4];
static short creationCount, creationComp;
static short deathDelay[2];
static short diverterDelay;
static char multiInfoSel;
static char soulFlag[3];

static char jackpotLightIndex[] = {
	LITE_ARK_RIGHTRAMP_JACKPOT,
	LITE_ARK_LEFTRAMP_JACKPOT,
	LITE_ARK_RIGHTLANE_JACKPOT,
	LITE_ARK_LEFTLANE_JACKPOT,
};
static short lightPosTables[] = {
	-56,156,56,-81,
/* POS */
	-27,67,
	35,30,
	15,67,
	27,44,
	-14,21,
	-12,26,
	-8,30,
	-4,34,
	1,36,
	7,38,
	12,38,
	-30,32,
	-28,41,
	-26,50,
	21,49,
	35,44,
	-12,62,
	-4,65,
	4,67,
	20,-2,
	10,27,
	1,-2,
	-5,16,
	26,16,
	23,33,
	14,56,
	13,48,
	21,152,
	32,154,
	44,156,
	-5,-10,
	0,-12,
	5,-14,
	10,-15,
	16,-14,
	21,-12,
	26,-10,
	-31,2,
	-23,2,
	44,2,
	52,2,
	10,-38,
	52,-8,
	-31,-12,
/* JETS */
	22,113,
	36,97,
	43,117,
/* SLING */
	36,-8
	-15,-8,
};
static char arrowInfoTables[] = { 6, 5, 7 };
static char roundTaskTables[] = { 1, 33, 17, 9, 25, 41 };
static char roundAudioTables[] = {
	CDA_ARK_DEATH_START, CDA_ARK_DEATH_PLAY,
	-1, -1,
	CDA_ARK_ASYLUM_START, CDA_ARK_ASYLUM_PLAY,
	CDA_ARK_CEMETERY_START, CDA_ARK_CEMETERY_PLAY,
	CDA_ARK_SOUL_START, CDA_ARK_SOUL_PLAY,
	CDA_ARK_CREATION_START, CDA_ARK_CREATION_PLAY
};

void InitArkham(PLAYER *ply)
{
	lightPosPtr = &lightPosTables[0];
	ply->vukEnable = 1;
	ply->gateEnable[DIVERTER1_TRUE] = 1;
	ply->gateEnable[DIVERTER1_FALSE] = 0;
	ply->gateEnable[DIVERTER2_TRUE] = 1;
	ply->gateEnable[DIVERTER2_FALSE] = 0;
	ply->gateEnable[DIVERTER3_TRUE] = 1;
	ply->gateEnable[DIVERTER3_FALSE] = 0;
	ply->gateEnable[SUSPENSION1] = 0;
	ply->gateEnable[SUSPENSION2] = 0;
	PresetScore(ply->jackpot, scoreConst[SCORE_010M]);
	ply->multiBallDepth = 0;
	ply->madnessSpell = 0;		/* MADNESS SPELL COUNTER */
	ply->madnessSpellTimer = 0;
	ply->roundStartCount = 0;
	ply->roundStartDepth = 1;
}

void InitArkhamPlay(PLAYER *ply)
{
	ply->gateEnable[DIVERTER1_TRUE] = 1;
	ply->gateEnable[DIVERTER1_FALSE] = 0;
	ply->gateEnable[DIVERTER2_TRUE] = 1;
	ply->gateEnable[DIVERTER2_FALSE] = 0;
	ply->gateEnable[DIVERTER3_TRUE] = 1;
	ply->gateEnable[DIVERTER3_FALSE] = 0;
	ply->gateEnable[SUSPENSION1] = 0;
	ply->gateEnable[SUSPENSION2] = 0;
	roundDelay = 0;
	diverterDelay = 0;
	divCon = 0;
}

void ServiceArkham(PLAYER *ply, char *sct)
{
	int	f;

/*--------------------------------------------------------------/
/	Diverter, Suspension Controle (Extra Ball / Mystery)	/
/--------------------------------------------------------------*/
	if(divCon==0) {

	if(diverterDelay) {
		diverterDelay--;
		if(diverterDelay==0) ply->gateEnable[SUSPENSION1] = 0;
		}

	if(ply->extraBallReady | ply->mysteryReady) {
		ply->gateEnable[DIVERTER1_TRUE] = 0;
		ply->gateEnable[DIVERTER1_FALSE] = 1;
		}
	else 	{
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		}
	}

	if(ply->rampRollOverOn[1] | ply->suspensionRollOverOn[0]) {
	if(ply->extraBallReady) {
		RingSfx(SFX_EXTRABALL_AWARD);
		if(ply->extraBallCount < 3) {
			ply->extraBallCount++;
			ply->extraBallCycle++;
			if(divCon==0) {
				ply->gateEnable[SUSPENSION1] = 1;
				SetMultiInfo(INFO_ANIME_EXTRA_BALL, 5);
				diverterDelay = 2 * frameRate;
				}
			}
		else	{
			AddScore(ply->score, scoreConst[SCORE_030M]);
			if(divCon==0) {
				ply->gateEnable[SUSPENSION1] = 1;
				SetMultiInfo(INFO_ANIME_30M, 5);
				diverterDelay = 2 * frameRate;
				}
			}
		ply->extraBallReady--;
		}
	if(ply->mysteryReady) {
		if(divCon==0) {
			ply->gateEnable[SUSPENSION1] = 1;
			diverterDelay += 2 * frameRate;
			f = 1;
			}
		else f = 0;
		TakeMystery(ply, f);
		ply->mysteryReady--;
		}
	}

	ServiceMultiBall(ply, sct);
	ServiceRound(ply);
}

void DoArkhamMultiBallReady(PLAYER *ply)
{
	ply->multiBallReady = 1;
	ply->madnessSpell = 7;
	ply->madnessSpellTimer = 0;
}

static void ServiceMultiBall(PLAYER *ply, char *sct)
{
	int	i, j, k;

	if(ply->multiBallEnable==0) return;

/*----------------------------------------------/
/	RISE JACKPOT				/
/----------------------------------------------*/
	if(ply->multiBallGo) {
	if(jackpotRiseTimer1) jackpotRiseTimer1--;
	if(jackpotRiseTimer2) jackpotRiseTimer2--;
	j = 0;

	for(i=0; i<3; ++i) {
	if(jackpotRiseTimer1)
		lightCommand[LITE_ARK_STANDUP_TARGET_1 + i] = LIGHT_TURN_BLINK;
	else if(ply->standUpFlag[i]) 
		lightCommand[LITE_ARK_STANDUP_TARGET_1 + i] = LIGHT_TURN_ON;
	else	lightCommand[LITE_ARK_STANDUP_TARGET_1 + i] = LIGHT_TURN_OFF;
	if(ply->standUpOn[i]) jackpotRise1++;
	}

	if(jackpotRise1 >= 3) {
	j++;
	jackpotRiseTimer1 = 2 * frameRate;
	for(i=0; i<3; ++i) ply->standUpFlag[i] = 0;
	AddScore(ply->jackpot, scoreConst[SCORE_001M]);
	jackpotRise1 = 0;
	}
	for(i=0; i<2; ++i) {
	if(jackpotRiseTimer2)
		lightCommand[LITE_ARK_STANDUP_TARGET_4 + i] = LIGHT_TURN_BLINK;
	else if(ply->standUpFlag[i+3]) 
		lightCommand[LITE_ARK_STANDUP_TARGET_4 + i] = LIGHT_TURN_ON;
	else	lightCommand[LITE_ARK_STANDUP_TARGET_4 + i] = LIGHT_TURN_OFF;
	if(ply->standUpOn[i+3]) jackpotRise2++;
	}
	if(jackpotRise2 >= 2) {
	j++;
	jackpotRiseTimer2 = 2 * frameRate;
	for(i=0; i<2; ++i) ply->standUpFlag[i+3] = 0;
	AddScore(ply->jackpot, scoreConst[SCORE_0025M]);
	jackpotRise2 = 0;
	}
	if(j) {
	if(CmpScore(ply->jackpot, scoreConst[SCORE_100M])==0)
			PresetScore(ply->jackpot, scoreConst[SCORE_100M]);
		}
	}


	switch(ply->multiBallGo)
	{
	case 0:
	jackpotRise1 = jackpotRise2 = 0;
	jackpotRiseTimer1 = jackpotRiseTimer2 = 0;
	if(ply->multiBallRestart) ply->multiBallRestart--;
	if(ply->multiBallReady!=0 || ply->multiBallRestart!=0) j = 0;
	else j = 1;
	for(i=0; i<3; ++i) {
	ply->dropTargetEnable[i] = j;
	}

	for(i=0; i<3; ++i) {
	if(ply->madnessSpellTimer!=0 || ply->multiBallReady!=0 || ply->multiBallRestart!=0)
		lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_BLINK;
	else	{
	if(ply->dropTargetFlag[i])
		lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_ON;
	else	lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_OFF;
		}
	}
	j = 0;
	if(ply->multiBallDepth==0)
		for(i=0; i<3; ++i) j |= ply->dropTargetFlag[i];
	else if(ply->multiBallDepth==1)
		j = ply->dropTargetFlag[0] | ply->dropTargetFlag[2];
	else if(ply->multiBallDepth==2) j = ply->dropTargetFlag[1];
	else	{
		ply->multiBallDepth = 3;
		j = 1;
		for(i=0; i<3; ++i) j &= ply->dropTargetFlag[i];
		}
	if(j) {
	AddScore(sct, scoreConst[SCORE_DROP_COMP]);
	ResetDropTarget(ply);
	ply->madnessSpellTimer = frameRate * 2;
	ply->madnessSpell++;
	if(ply->madnessSpell >= 7) {
		ply->multiBallReady = 1;
		RingSfx(SFX_ARK_GATE_OPEN);
		}
	}
	if(ply->madnessSpellTimer) ply->madnessSpellTimer--;

	if(ply->multiBallReady!=0 || ply->multiBallRestart!=0) {
	if(ply->holeOn[1] | ply->holeOn[2]) {
		doubleJackpotEnable = 0;
		for(i=0; i<4; ++i) jackpotFlag[i] = 0;
		ResetDropTarget(ply);
		LaunchBallSaver(ply, 3);
		ply->multiBallGo = 4;
		if(ply->roundGoNo) ply->roundGoNo = 37;
		}
	else if(ply->holeOn[0]) {
		doubleJackpotEnable = 1;
		spellCount = 0;
		ResetDropTarget(ply);
		LaunchBallSaver(ply, 7);
		ply->multiBallCompFlag = 1;
		ply->multiBallGo = 11;
		if(ply->roundGoNo) ply->roundGoNo = 37;
		}

	if(ply->multiBallGo) {
		divCon = 1;
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		ply->gateEnable[SUSPENSION1] = 0;
		ply->gateEnable[SUSPENSION2] = 0;
		slaveTaskNo = 24;
		ply->multiBallReady = 0;
		ply->multiBallRestart = 0;
		ply->multiBallDepth++;
		for(i=0; i<4; ++i)
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		for(i=0; i<5; ++i) ply->standUpFlag[i] = 0;
		for(i=0; i<4; ++i) jackpotFlag[i] = 0;
		}
	}

	for(i=0; i<7; ++i) {
	if(ply->madnessSpell > i)
		lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_ON;
	else lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_OFF;
	}
	break;
	case 1:
	case 2:
	case 3:
/*	3 Ball Multi Main	*/
	case 4:
		diagInfoTab[0] = INFO_DIAG_MB_DIRECT;
		for(i=0; i<3; ++i)
			lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_OFF;
		i = 0;
		if(ply->rampRollOverOn[0]!=0 && jackpotFlag[0]==0)
			{ jackpotFlag[0] = 1; i++; }
		if(ply->rampRollOverOn[1]!=0 && jackpotFlag[1]==0)
			{ jackpotFlag[1] = 1; i++; }
		if(ply->laneRollOverOn[0]!=0 && jackpotFlag[2]==0)
			{ jackpotFlag[2] = 1; i++; }
		if(ply->laneRollOverOn[1]!=0 && jackpotFlag[3]==0)
			{ jackpotFlag[3] = 1; i++; }
		if(i)	{
			lightAnimePtr = liteBLK4;
			ply->multiBallCompFlag = 1;
			RingSfx(SFX_ARK_JACKPOT);
			RingSfx(SFX_JACKPOT);
			SetInfomations(INFO_ANIME_JACKPOT, 5);
			if(doubleJackpotEnable) i += i;
			while(i) {
			AddScore(ply->score, ply->jackpot);
			i--;
				}
			}
		j = 1;
		for(i=0; i<4; ++i) {
		if(jackpotFlag[i])
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_ON;
		else	lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_BLINK;
		j &= jackpotFlag[i];
		}
		if(j) ply->multiBallGo = 6;
		ServiceExitMulti(ply, 20);
		break;
	case 6:
		diagInfoTab[0] = INFO_DIAG_MB_HOLE;
		for(i=0; i<4; ++i)
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_DROP_TARGET_1] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_DROP_TARGET_2] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_DROP_TARGET_3] = LIGHT_TURN_OFF;
		ply->dropTargetEnable[0] = 1;
		ply->dropTargetEnable[1] = 0;
		ply->dropTargetEnable[2] = 1;
		if(ply->holeOn[0]) {
		RingSfx(SFX_ARK_SUPER_JACKPOT);
		SetInfomations(INFO_ANIME_SP_JACKPOT, 6);
		AddScore(ply->score, ply->jackpot);
		AddScore(ply->score, ply->jackpot);
		AddScore(ply->score, ply->jackpot);
		LaunchBallSaver(ply, 5);
		spellCount = 0;
		lightAnimePtr = liteBLK8;
		for(i=0; i<7; ++i) {
		lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_OFF;
			}
		for(i=0; i<3; ++i) {
		lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_OFF;
		ply->dropTargetEnable[i] = 1;
			}
		ply->multiBallGo = 7;
		}
		ServiceExitMulti(ply, 20);
		break;
/*	5 Balls Multi Main		*/
	case 7:
		for(i=0; i<4; ++i) jackpotFlag[i] = 0;
		jackpotFlag[(Rand() & 3)] = 1;
		spellTimer = 3 * frameRate;
		ply->multiBallGo++;
		break;
	case 8:
		diagInfoTab[0] = INFO_DIAG_MB_DIRECT;
		for(i=0; i<4; ++i) {
		if(jackpotFlag[i])
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_BLINK;
		else	lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		}
		if(spellTimer) spellTimer--;
		i = 0;
		if(ply->rampRollOverOn[0]!=0 && jackpotFlag[0]!=0) i++;
		if(ply->rampRollOverOn[1]!=0 && jackpotFlag[1]!=0) i++;
		if(ply->laneRollOverOn[0]!=0 && jackpotFlag[2]!=0) i++;
		if(ply->laneRollOverOn[1]!=0 && jackpotFlag[3]!=0) i++;
		if(i)	{
		RingSfx(SFX_ARK_JACKPOT);
		RingSfx(SFX_JACKPOT);
		SetInfomations(INFO_ANIME_JACKPOT, 5);
		if(doubleJackpotEnable) i += i;
		while(i) {
			AddScore(ply->score, ply->jackpot);
			i--;
			spellCount++;
			}
		lightAnimePtr = liteBLK4;
		ply->multiBallGo = 7;
		}
		for(i=0; i<7; ++i) {
		if(spellCount > i) {
		if(spellTimer)
			lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_BLINK;
		else	lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_ON;
			}
		else	lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_OFF;
		}
		if(spellCount >= 7) ply->multiBallGo = 9;
		ServiceExitMulti(ply, 20);
		break;
	case 9:
		diagInfoTab[0] = INFO_DIAG_MB_HOLE;
		if(spellTimer) spellTimer--;
		for(i=0; i<7; ++i) {
		if(spellCount > i) {
		if(spellTimer)
			lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_BLINK;
		else	lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_ON;
			}
		else	lightCommand[LITE_ARK_M_MADNESS_SPELL_1 + i] = LIGHT_TURN_OFF;
		}
		for(i=0; i<4; ++i)
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		ply->dropTargetEnable[0] = 1;
		ply->dropTargetEnable[1] = 0;
		ply->dropTargetEnable[2] = 1;
		lightCommand[LITE_ARK_DROP_TARGET_1] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_DROP_TARGET_2] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_DROP_TARGET_3] = LIGHT_TURN_OFF;
		if(ply->holeOn[0]) {
		RingSfx(SFX_ARK_SPIRITS_JACKPOT);
		SetInfomations(INFO_ANIME_SPR_JACKPOT, 6);
		for(i=0; i<7; ++i)
			AddScore(ply->score, ply->jackpot);
		LaunchBallSaver(ply, 7);
		lightAnimePtr = liteBLK8;
		spellCount = 0;
		ply->multiBallGo = 11;
		}
		ServiceExitMulti(ply, 20);
		break;
	case 10:
/*	7 Balls Multi Main	*/
	case 11:
		for(i=0; i<4; ++i) jackpotFlag[i] = 0;
		for(i=0; i<3; ++i) {
		lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_OFF;
		ply->dropTargetEnable[i] = 1;
		}
		i = Rand() & 3;
		jackpotFlag[i] = 1;
		ply->multiBallGo++;
		break;
	case 12:
		for(i=0; i<4; ++i) {
		if(jackpotFlag[i])
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_BLINK;
		else	lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		}
		i = 0;
		if(ply->rampRollOverOn[0]!=0 && jackpotFlag[0]!=0) i++;
		if(ply->rampRollOverOn[1]!=0 && jackpotFlag[1]!=0) i++;
		if(ply->laneRollOverOn[0]!=0 && jackpotFlag[2]!=0) i++;
		if(ply->laneRollOverOn[1]!=0 && jackpotFlag[3]!=0) i++;
		if(i) {
		RingSfx(SFX_ARK_JACKPOT);
		RingSfx(SFX_JACKPOT);
		SetInfomations(INFO_ANIME_JACKPOT, 5);
		spellCount += i;
		if(spellCount >= 13) spellCount = 13;
		if(doubleJackpotEnable) i += i;
		while(i) {
			AddScore(ply->score, ply->jackpot);
			i--;
			}
		lightAnimePtr = liteBLK4;
		ply->multiBallGo = 11;
		}
		diagInfoTab[0] = INFO_DIAG_MB_DIRECT;
		diagInfoTab[1] = INFO_DIAG_SPELL_0 + spellCount;
		if(spellCount >= 13) {
		jackpotCountDown = 30 * frameRate;
		spellCount = 0;
		ply->multiBallGo = 13;
		}
		ServiceExitMulti(ply, 20);
		break;
	case 13:
		infoTimer = jackpotCountDown / frameRate;
		diagInfoTab[0] = INFO_DIAG_MBT_HOLE;
		for(i=0; i<4; ++i) {
		lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		}
		ply->dropTargetEnable[0] = 1;
		ply->dropTargetEnable[1] = 0;
		ply->dropTargetEnable[2] = 1;
		lightCommand[LITE_ARK_DROP_TARGET_1] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_DROP_TARGET_2] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_DROP_TARGET_3] = LIGHT_TURN_OFF;
		if(ply->holeOn[0]!=0 && jackpotCountDown!=0) {
		nextStageEnable |= 1;
		RingSfx(SFX_ARK_DARK_JACKPOT);
		SetInfomations(INFO_ANIME_DARK_JACKPOT, 6);
		for(i=0; i<15; ++i)
			AddScore(ply->score, ply->jackpot);
		ply->dropTargetEnable[1] = 1;
		lightAnimePtr = liteBLK16;
		ply->multiBallGo = 11;
		}
		if(jackpotCountDown) jackpotCountDown--;
		if(jackpotCountDown < (10 * frameRate)) {
		if((jackpotCountDown % frameRate)==0) lightAnimePtr = liteWBT;
		}
		if(jackpotCountDown <= 0) {
			SetInfomations(INFO_DIAG_TIME_OUT, 2);
			ply->multiBallGo = 11;
			}
		ServiceExitMulti(ply, 20);
		break;
/*	End of Multi Ball	*/
	case 20:
		divCon = 0;
		for(i=0; i<2; ++i) diagInfoTab[i] = -1;
		if(ply->multiBallCompFlag==0) {
			ply->multiBallRestart = frameRate * 30;
			}
		ResetDropTarget(ply);
		for(i=0; i<4; ++i)
			lightCommand[jackpotLightIndex[i]] = LIGHT_TURN_OFF;
		RingSfx(SFX_TIME_BACK);
	for(i=0; i<2; ++i) ply->standUpFlag[i+3] = 0;
		slaveTaskNo = 25;
		ply->multiBallGo = 0;
		ply->madnessSpell = 0;
		break;
	default:
		break;
	}
}

/*----------------------------------------------/
/	Round Service				/
/----------------------------------------------*/
static void ServiceRound(PLAYER *ply)
{
	int	i, j;

	if(ply->multiBallGo) return;

	switch(ply->roundGoNo)
	{
	case 0:
		ServiceRoundStart(ply);
		break;
/*----------------------------------------------/
/	1:DEATHWALKER（ノーマルラウンド）	/
/----------------------------------------------*/
	case 1:
		divCon = 1;
		ply->multiBallEnable = 0;
		ResetDropTarget(ply);
		ply->gateEnable[DIVERTER1_TRUE] = 0;
		ply->gateEnable[DIVERTER1_FALSE] = 1;
		ply->gateEnable[DIVERTER3_TRUE] = 0;
		ply->gateEnable[DIVERTER3_FALSE] = 1;
		if(ply->superRoundEnable)
			PresetScore(roundValue, scoreConst[SCORE_020M]);
		else	PresetScore(roundValue, scoreConst[SCORE_010M]);
		deathDelay[0] = deathDelay[1] = 0;
		LaunchBallSaver(ply, 3);
		ply->roundGoNo = 3;
		break;
	case 2:
	case 3:
		CreateInfoScore(roundValue);
		diagInfoTab[0] = INFO_DIAG_DEATH_DIRECT;
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->superRoundEnable)
			SubScore(roundValue, scoreConst[SCORE_DEATH]);
		SubScore(roundValue, scoreConst[SCORE_DEATH]);
		if(CmpScore(roundValue, scoreConst[SCORE_001M]))
			PresetScore(roundValue, scoreConst[SCORE_001M]);
		ServiceDeathWalker(ply);
		ServiceExitRound(ply, 4);
		break;
	case 4:
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		ply->gateEnable[SUSPENSION1] = 0;
		ply->gateEnable[SUSPENSION2] = 0;
		divCon = 0;
		ply->roundGoNo = 37;
		break;
	case 5:
	case 6:
	case 7:
	case 8:
		break;
/*----------------------------------------------/
/	2:CEMETERY				/
/----------------------------------------------*/
	case 9:
		divCon = 1;
		ply->multiBallEnable = 0;
		ply->gateEnable[DIVERTER1_TRUE] = 0;
		ply->gateEnable[DIVERTER1_FALSE] = 1;
		ply->gateEnable[DIVERTER3_TRUE] = 0;
		ply->gateEnable[DIVERTER3_FALSE] = 1;
		SetRoundTimer(ply, 30);
		bingoHole = (unsigned int)Rand() % 5;
		for(i=0; i<5; ++i) bingoFlag[i] = 0;
		cemetDelay = 0;
		for(i=0; i<3; ++i) ply->dropTargetEnable[i] = 0;
		ply->roundGoNo++;
		break;
	case 10:
		ServiceCeme(ply);
		RoundClose(ply, 11);
		break;
	case 11:
		if(infoNo < 0 || tiltFlag!=0) {
		ResetDropTarget(ply);
		RingSfx(SFX_DROP_TARGET_RESET);
		ply->vukEnable = 1;
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		ply->gateEnable[SUSPENSION1] = 0;
		ply->gateEnable[SUSPENSION2] = 0;
		divCon = 0;
		ply->roundGoNo = 37;
		}
		break;
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		break;
/*----------------------------------------------/
/	3:ARKHAM ASYLUM				/
/----------------------------------------------*/
	case 17:
		divCon = 1;
		ply->multiBallEnable = 1;
		ResetDropTarget(ply);
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		SetRoundTimer(ply, 30);
		ply->roundGoNo++;
		break;
	case 18:
		infoTimer = roundTimer / frameRate;
		if(ply->superRoundEnable)
			CreateInfoScore(scoreConst[SCORE_020M]);
		else	CreateInfoScore(scoreConst[SCORE_010M]);
		diagInfoTab[0] = INFO_DIAG_ASYLUM_DIRECT;
		lightCommand[LITE_ARK_LEFTLANE_JACKPOT] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_RIGHTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		if(ply->laneRollOverOn[1]) {
		RingSfx(SFX_ARK_ASYLUM_AWARD);
		lightAnimePtr = liteWBT;
		if(ply->superRoundEnable) {
			AddScore(ply->roundScore, scoreConst[SCORE_020M]);
			SetInfomations(INFO_ANIME_20M, 5);
			}
		else	{
			AddScore(ply->roundScore, scoreConst[SCORE_010M]);
			SetInfomations(INFO_ANIME_10M, 5);
			}
		ply->roundGoNo = 19;
		}
		roundTimer--;
		if(roundTimer <= 0) ply->roundGoNo = 22;
		RoundClose(ply, 22);
		break;
	case 19:
		infoTimer = roundTimer / frameRate;
		if(ply->superRoundEnable)
			CreateInfoScore(scoreConst[SCORE_040M]);
		else	CreateInfoScore(scoreConst[SCORE_020M]);
		diagInfoTab[0] = INFO_DIAG_ASYLUM_DIRECT;
		lightCommand[LITE_ARK_LEFTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		if(ply->rampRollOverOn[1]) {
		RingSfx(SFX_ARK_ASYLUM_AWARD);
		lightAnimePtr = liteWBT;
		if(ply->superRoundEnable) {
			AddScore(ply->roundScore, scoreConst[SCORE_040M]);
			SetInfomations(INFO_ANIME_40M, 5);
			}
		else	{
			AddScore(ply->roundScore, scoreConst[SCORE_020M]);
			SetInfomations(INFO_ANIME_20M, 5);
			}
		ply->roundGoNo = 20;
		}
		roundTimer--;
		if(roundTimer <= 0) ply->roundGoNo = 22;
		RoundClose(ply, 22);
		break;
	case 20:
		infoTimer = roundTimer / frameRate;
		if(ply->superRoundEnable)
			CreateInfoScore(scoreConst[SCORE_060M]);
		else	CreateInfoScore(scoreConst[SCORE_030M]);
		diagInfoTab[0] = INFO_DIAG_ASYLUM_DIRECT;
		lightCommand[LITE_ARK_LEFTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTLANE_JACKPOT] = LIGHT_TURN_BLINK;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		if(ply->laneRollOverOn[0]) {
		RingSfx(SFX_ARK_ASYLUM_AWARD);
		lightAnimePtr = liteWBT;
		if(ply->superRoundEnable) {
			AddScore(ply->roundScore, scoreConst[SCORE_060M]);
			SetInfomations(INFO_ANIME_60M, 5);
			}
		else	{
			AddScore(ply->roundScore, scoreConst[SCORE_030M]);
			SetInfomations(INFO_ANIME_30M, 5);
			}
		ply->roundGoNo = 21;
		}
		roundTimer--;
		if(roundTimer <= 0) ply->roundGoNo = 22;
		RoundClose(ply, 22);
		break;
	case 21:
		infoTimer = roundTimer / frameRate;
		if(ply->superRoundEnable)
			CreateInfoScore(scoreConst[SCORE_100M]);
		else	CreateInfoScore(scoreConst[SCORE_050M]);
		diagInfoTab[0] = INFO_DIAG_ASYLUM_DIRECT;
		lightCommand[LITE_ARK_LEFTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
		if(ply->rampRollOverOn[0]) {
		RingSfx(SFX_ARK_ASYLUM_AWARD);
		lightAnimePtr = liteWBT;
		if(ply->superRoundEnable) {
			AddScore(ply->roundScore, scoreConst[SCORE_100M]);
			SetInfomations(INFO_ANIME_100M, 5);
			}
		else	{
			AddScore(ply->roundScore, scoreConst[SCORE_050M]);
			SetInfomations(INFO_ANIME_50M, 5);
			}
		ply->roundGoNo = 22;
		}
		roundTimer--;
		if(roundTimer <= 0) ply->roundGoNo = 22;
		RoundClose(ply, 22);
		break;
	case 22:
		lightCommand[LITE_ARK_LEFTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTLANE_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
		divCon = 0;
		ply->roundGoNo = 37;
		break;
	case 23:
	case 24:
		break;
/*------------------------------------------------------/
/	4:SOUL ( EXTRA BALL )				/
/------------------------------------------------------*/
	case 25:
		divCon = 1;
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		soulTask = 0;
		ply->roundGoNo++;
		break;
	case 26:
		ServiceSoul(ply, 37);
		break;
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
/*------------------------------------------------------/
/	5:LITE WITCHCRAFT（ライトミステリーラウンド）	/
/------------------------------------------------------*/
	case 33:
		ServiceMysteryRound(ply);
		break;
	case 34:
	case 35:
	case 36:
/*----------------------------------------------/
/	ラウンド終了				/
/----------------------------------------------*/
	case 37:
		divCon = 0;
		for(i=0; i<2; ++i) ply->standUpFlag[i+3] = 0;
		ServiceRoundEnd(ply);
		break;
	case 38:
	case 39:
	case 40:
		break;
/*------------------------------------------------------/
/	6:CREATION（ファイナルラウンド）		/
/------------------------------------------------------*/
	case 41:
		divCon = 1;
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		ply->multiBallEnable = 0;
		ResetDropTarget(ply);
		LaunchBallSaver(ply, 7);
		creationComp = 1;
		ply->roundGoNo++;
		break;
	case 42:
		ServiceMultiScore(ply, SCORE_200K, INFO_DIAG_MULTI_DIRECT);
		if((ply->multiBalls + ply->multiBallCount + ply->ballSaveAdd)
			 > 3) {
			i = ply->switchFrame;
			if(i) RingSfx(SFX_ARK_CREATION_AWARD);
			while(i) {
			AddScore(ply->roundScore, roundValue);
			i--;
			}
		}
		else if(ply->multiBalls < 4 && ply->multiBallCount==0
			&& ply->ballSaveAdd==0) {
			ResetDropTarget(ply);
			creationCount = 0;
			ply->roundGoNo = 43;
			}
		ServiceExitRound(ply, 44);
		break;
	case 43:
		ply->roundStartDepth = 2;
		infoNumber = creationComp - creationCount;
		diagInfoTab[0] = INFO_DIAG_CREA_RESTART;
		j = 1;
		for(i=0; i<3; ++i) {
		j &= ply->dropTargetFlag[i];
		if(ply->dropTargetFlag[i])
			lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_ON;
		else	{
			lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_BLINK;
			if((vsyncCounter & 31) < 11)
				MobjArrow(arrowInfoTables[i]);
			}
		}
		if(j)	{
			creationCount++;
			ResetDropTarget(ply);
			}
		if(creationCount == creationComp) {
			RingSfx(SFX_ARK_CREATION_RESTA);
			creationComp++;
			LaunchBallSaver(ply, 7);
			ply->roundGoNo = 42;
			}
		ServiceExitRound(ply, 44);
		break;
	case 44:
		divCon = 0;
		nextStageEnable |= 2;
		ply->roundGoNo = 37;
		break;
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

	if(ply->roundGoNo==0 && ply->multiBallGo==0) {
	if(ply->standUpOn[3]) ply->roundStartCount++;
	if(ply->standUpOn[4]) ply->roundStartCount++;
	if(ply->roundStartCount >= ply->roundStartDepth) {
		ply->roundStartCount = 0;
		AddScore(ply->score, scoreConst[SCORE_300K]);
		ply->standUpFlag[3] = ply->standUpFlag[4] = 0;
		roundDelay = frameRate * 2;
		if(ply->roundStartReady) ply->superRoundEnable = 1;
		ply->roundStartReady = 1;
		}
	}

	for(i=0; i<2; ++i) {
	if(roundDelay)
		lightCommand[LITE_ARK_STANDUP_TARGET_4 + i] = LIGHT_TURN_BLINK;
	else if(ply->standUpFlag[3 + i])
		lightCommand[LITE_ARK_STANDUP_TARGET_4 + i] = LIGHT_TURN_ON;
	else	lightCommand[LITE_ARK_STANDUP_TARGET_4 + i] = LIGHT_TURN_OFF;
	}
	if(roundDelay) roundDelay--;

	if(ply->roundStartReady!=0) {
	ply->gateEnable[DIVERTER3_TRUE] = 0;
	ply->gateEnable[DIVERTER3_FALSE] = 1;
	ply->gateEnable[SUSPENSION2] = 1;
	if(ply->suspensionRollOverOn[1]) {
		LaunchRound(ply);
		}
	}
	else {
	ply->gateEnable[DIVERTER3_TRUE] = 1;
	ply->gateEnable[DIVERTER3_FALSE] = 0;
	}

	if(roundStartDelay) {
	lightCommand[LITE_ARK_DROP_TARGET_1] = LIGHT_TURN_OFF;
	lightCommand[LITE_ARK_DROP_TARGET_2] = LIGHT_TURN_OFF;
	lightCommand[LITE_ARK_DROP_TARGET_3] = LIGHT_TURN_OFF;
	lightCommand[LITE_ARK_STANDUP_TARGET_4] = LIGHT_TURN_OFF;
	lightCommand[LITE_ARK_STANDUP_TARGET_5] = LIGHT_TURN_OFF;
	roundStartDelay--;
	if(roundStartDelay == (2 * frameRate)) {
	i = roundAudioTables[ply->roundNumber * 2];
	j = roundAudioTables[ply->roundNumber * 2 + 1];
	if(i >= 0 && optionMenuPad[OPTION_NO_BGM]!=0) PlayCD(i, j);
	}
	if(roundStartDelay==0) {
		ply->gateEnable[SUSPENSION2] = 0;
		ply->roundGoNo = roundTaskTables[ply->roundNumber];
		ply->roundCompFlag[ply->roundNumber] = 1;
		ply->roundStartReady = 0;
		ResetScore(ply->roundScore);
		}
	}
}

static void ServiceDeathWalker(PLAYER *ply)
{
	int	i;

	for(i=0; i<2; ++i) {
	if(ply->suspensionRollOverOn[i]) {
		if(i==0) {
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		}
		else	{
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		}
		RingSfx(SFX_ARK_DEATH_AWARD);
		deathDelay[i] = 2 * frameRate;
		lightAnimePtr = liteWBT;
		AddScore(ply->roundScore, roundValue);
		if(ply->superRoundEnable)
			AddScore(roundValue, scoreConst[SCORE_010M]);
		else	AddScore(roundValue, scoreConst[SCORE_005M]);
		}
	if(deathDelay[i]) {
		deathDelay[i]--;
		ply->gateEnable[SUSPENSION1 + i] = 1;
		if(deathDelay[i]==0) {
			ply->gateEnable[SUSPENSION1 + i] = 0;
			RingSfx(SFX_BALL_EJECT);
			if(i==0) {
				ply->gateEnable[DIVERTER1_TRUE] = 0;
				ply->gateEnable[DIVERTER1_FALSE] = 1;
				}
			else	{
				ply->gateEnable[DIVERTER3_TRUE] = 0;
				ply->gateEnable[DIVERTER3_FALSE] = 1;
				}
			}
		}
	}
}

static void ServiceCeme(PLAYER *ply)
{
	int	i, j;

	infoTimer = roundTimer / frameRate;
	diagInfoTab[0] = INFO_DIAG_CEME_DIRECT;
	for(i=0; i<3; ++i) {
	if(bingoFlag[2 + i])
		lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_OFF;
	else	{
		lightCommand[LITE_ARK_DROP_TARGET_1 + i] = LIGHT_TURN_BLINK;
		if((vsyncCounter & 31) < 11) MobjArrow(arrowInfoTables[i]);
		}
	}
	if(bingoFlag[0])
		lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_OFF;
	else	lightCommand[LITE_ARK_LEFTRAMP_JACKPOT] = LIGHT_TURN_BLINK;
	if(bingoFlag[1])
		lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_OFF;
	else	lightCommand[LITE_ARK_RIGHTRAMP_JACKPOT] = LIGHT_TURN_BLINK;

	if(ply->suspensionRollOverOn[0]) {
		i = 0;
		ply->gateEnable[DIVERTER1_TRUE] = 1;
		ply->gateEnable[DIVERTER1_FALSE] = 0;
		ply->gateEnable[SUSPENSION1] = 1;
		}
	else if(ply->suspensionRollOverOn[1]) {
		i = 1;
		ply->gateEnable[DIVERTER3_TRUE] = 1;
		ply->gateEnable[DIVERTER3_FALSE] = 0;
		ply->gateEnable[SUSPENSION2] = 1;
		}
	else if(ply->holeOn[0]) { i = 3; ply->dropTargetEnable[1] = 1; }
	else if(ply->holeOn[1]) { i = 2; ply->dropTargetEnable[0] = 1; }
	else if(ply->holeOn[2]) { i = 4; ply->dropTargetEnable[2] = 1; }
	else i = -1;
	if(i >= 0) {
	bingoFlag[i] = 1;
	cemetNo = i;
	cemetDelay = frameRate * 3;
	if(i>=2 && i<=4) {
		RingSfx(SFX_DROP_TARGET_RESET);
		ply->vukEnable = 0;
		}
	if(i==bingoHole) {
		RingSfx(SFX_ARK_CEME_YES_AWARD);
		lightAnimePtr = liteWBT_BLK_WBT_BLK;
		if(ply->superRoundEnable) {
		SetInfomations(INFO_ANIME_100M, 5);
		AddScore(ply->roundScore, scoreConst[SCORE_100M]);
			}
		else	{
		SetInfomations(INFO_ANIME_50M, 5);
		AddScore(ply->roundScore, scoreConst[SCORE_050M]);
		}
		ply->roundGoNo = 11;
		}
	else 	{
		RingSfx(SFX_ARK_CEME_NO_AWARD);
		if(ply->superRoundEnable) {
			SetInfomations(INFO_ANIME_10M, 5);
			AddScore(ply->roundScore, scoreConst[SCORE_010M]);
			}
		else	{
			SetInfomations(INFO_ANIME_5M, 5);
			AddScore(ply->roundScore, scoreConst[SCORE_005M]);
			}
		}
	}

	if(cemetDelay) {
	cemetDelay--;
	if(cemetDelay==0) {
		if(cemetNo==0) {
			RingSfx(SFX_BALL_EJECT);
			ply->gateEnable[SUSPENSION1] = 0;
			}
		else if(cemetNo==1) {
			RingSfx(SFX_BALL_EJECT);
			ply->gateEnable[SUSPENSION2] = 0;
			}
		else ply->vukEnable = 1;
		}
	}
	else if(infoNo!=INFO_ANIME_5M && infoNo!=INFO_ANIME_10M) roundTimer--;
	if(roundTimer <= 0) ply->roundGoNo = 11;
}
