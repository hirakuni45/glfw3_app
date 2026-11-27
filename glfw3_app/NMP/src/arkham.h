#pragma once
//=========================================================================//
/*! @file
	@brief  Necronomicon ARKHAM・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

void InitArkham(PLAYER *ply);
void InitArkhamPlay(PLAYER *ply);
void ServiceArkham(PLAYER *ply, char *sct);

void DoArkhamMultiBallReady(PLAYER *ply);
