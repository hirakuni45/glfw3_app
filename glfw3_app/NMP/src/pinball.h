#pragma once
//=========================================================================//
/*! @file
	@brief  pinball 関係メイン処理 Necronomicon・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
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
void MobjFlipper(int x, int y, int no);
void MobjDropTarget(int x, int y, int no);
void MobjJetBumper(int x, int y, int no);
void MobjSlingShot(int x, int y, int no);
int MobjFont(int, int, int);
void MobjString(int, int, const char *);
void MobjStringC(int, int, const char *);
void MobjNumber(int x, int y, int no);

void ResetReportMessage(void);

extern uint32_t clutSave[256];
extern uint32_t clutLupe[16];
extern uint16_t gamePadLevel, gamePadTrigger, gamePadRelese;
extern short gameAnalogX, gameAnalogY, gameAnalogEna;
extern short vibrX, vibrY, vibrXo, vibrYo;
extern short outTimer;
extern short demoPlayEnable;
extern char debugFlag;
extern char gamePauseEnable;
extern char vibrEnable;
extern char debugPauseEnable;
