#pragma once
//=====================================================================//
/*! @file
    @brief  ツールス・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include "utils/input.hpp"
#include "utils/format.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ツールス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct tools {

		static std::string limitf_(const std::string& str, float min, float max, const char* form)
		{
			std::string newtext;
			float v;
			if((utils::input("%f", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}


		static std::string limiti_(const std::string& str, int min, int max, const char* form)
		{
			std::string newtext;
			int v;
			if((utils::input("%d", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}

	};
}
