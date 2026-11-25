#pragma once
//=========================================================================//
/*! @file
	@brief  NMP core
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//

extern "C" {
	void EntryVsync(uint32_t padbits);
	void ExitVsync(void);
	void main_nmp(int no);
	void ServiceTask(void);
	const uint32_t *GetCLUT();
	uint8_t *get_VDP1_VRAM_TOP();
	uint8_t *get_VDP2_VRAM_TOP();
};

struct MOBJ {
	short	mode;
	short	flip;
	short	x, y, h, v;
	short	color;
	short	chaptr;
	short	chasiz;
	short	tmp;
};
