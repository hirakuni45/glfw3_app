#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター評価クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <string>

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  テスト・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class test {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  値、構造
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct value_t {
			std::string		symbol_;
			uint32_t		retry_;
			double			wait_;
			uint32_t		term_;
			double			delay_;
			uint32_t		filter_;
			double			width_;
			double			min_;
			double			max_;
			value_t() : symbol_(), retry_(0), wait_(0.0),
				term_(0), delay_(0.0), filter_(0), width_(0.0), min_(0.0), max_(0.0)
			{ }
		};
	};
}
