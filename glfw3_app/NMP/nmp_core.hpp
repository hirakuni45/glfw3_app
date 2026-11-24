#pragma once
//=====================================================================//
/*! @file
	@brief  NMP core
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//

extern "C" {
	void EntryVsync(void);
	void ExitVsync(void);
	void main_nmp(int no);
	void ServiceTask(void);
	const uint32_t *GetCLUT();
	uint8_t *get_VDP2_VRAM_TOP();
};
