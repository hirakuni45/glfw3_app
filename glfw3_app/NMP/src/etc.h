#pragma once
//=========================================================================//
/*! @file
	@brief  ETC Necronomicon・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>
#include "option.h"
#include "mobj.h"

void EntrySoundTester(void);
void EntryInfoTester(void);
void InitNECRONOMICON(void);
void EntryOptions(void);
void MainOptions(void);

void ServiceBrightClut(int top, int cnt);
void EntryManual(void);
void MainManual(void);
void InitPlayingManual(int);
void MainPlayingManual(void);

void EntryStaffRoll(void);

void FreezeScoreCode(uint32_t *dst, const char *src);
void ExpandScoreCode(char *dst, const int *src);

extern char optionMenuPad[OPTION_PAD_MAX];
extern NECRONOMICON necroPad[3];
extern ANIME lupeAnime;
