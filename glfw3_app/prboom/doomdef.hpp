#pragma once
//=====================================================================//
/*! @file
	@brief  DOOM インターフェース・定義 @n
			Copyright 2019 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//

#ifdef __cplusplus
extern "C" {
	int doom_main(int argc, char* argv[]);
	void doom_keybits(uint32_t bits);
	void doom_frame(int width, int height, uint8_t* rgba);
}
#endif

static const uint32_t KEY_BITS_UP    = 0x00000001;
static const uint32_t KEY_BITS_DOWN  = 0x00000002;
static const uint32_t KEY_BITS_RIGHT = 0x00000004;
static const uint32_t KEY_BITS_LEFT  = 0x00000008;

static const uint32_t KEY_BITS_ENTER = 0x00000010;
static const uint32_t KEY_BITS_ESC   = 0x00000020;
static const uint32_t KEY_BITS_DEL   = 0x00000040;
static const uint32_t KEY_BITS_TAB   = 0x00000080;
