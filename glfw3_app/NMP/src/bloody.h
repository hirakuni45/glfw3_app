#pragma once
//=========================================================================//
/*! @file
	@brief  Necronomicon BLOODY・ヘッダー
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <stdint.h>

void InitBloody(PLAYER *ply);
void InitBloodyPlay(PLAYER *ply);
void ServiceBloody(PLAYER *ply, char *sct);

void DoBloodyMultiBallReady(PLAYER *ply);

void StartRoundBloody(PLAYER *ply);
