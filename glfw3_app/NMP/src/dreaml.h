#pragma once
//=========================================================================//
/*! @file
	@brief  Necronomicon DREAML・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

void InitDreaml(PLAYER *ply);
void InitDreamlPlay(PLAYER *ply);
void ServiceDreaml(PLAYER *ply, char *sct);

void DoDreamMultiBallReady(PLAYER *ply);
