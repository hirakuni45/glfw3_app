#pragma once
//=========================================================================//
/*! @file
	@brief  アプリケーション制御フラグ・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

void ReciveValues(int);
void TransmitValues(void);

void SYS_Exit(int);
int GFS_ConvTno(int);

void SYS_EXECDMP(void);

extern char startMaskEnable;
extern char demoPlayIndex;
extern char mainMenuPos;
extern char gameMenuPos;
extern char playerNo;
extern char realmsEnable;
extern char titleMovieIndex;

extern const char trialEnable;
extern const char geEnable;
extern const char demoMovieMax;
extern const char cdTrackStyle;
extern char staffRollDebug;
extern const char alertTestEnable;
extern const char blockTestEnable;
extern char debugEnable;
extern const char multiPlayerEnable;
extern const char flashSegaEnable;