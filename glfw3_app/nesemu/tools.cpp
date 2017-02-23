//=====================================================================//
/*! @file
	@brief  Emulator Tools クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
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

