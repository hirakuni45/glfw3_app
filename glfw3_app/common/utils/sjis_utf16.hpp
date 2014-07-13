#pragma once
//=====================================================================//
/*!	@file
	@brief	SJIS, UTF16 変換
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>

namespace utils {

	void init_utf16_to_sjis();

	uint16_t sjis_to_utf16(uint16_t sjis);

	uint16_t utf16_to_sjis(uint16_t sjis);

};
