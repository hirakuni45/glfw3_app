//=====================================================================//
/*! @file
	@brief  ツール・メイン
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "tools.hpp"

gui::widget_terminal* emu::tools::terminal_;

extern "C" {

	int emu_log(const char* text)
	{
		emu::tools::put(text);
		return 0;
	}

};

