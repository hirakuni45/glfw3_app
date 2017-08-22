#pragma once
//=====================================================================//
/*! @file
	@brief  main ヘッダー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "snd_io/sound.hpp"
#include "widgets/widget_director.hpp"
#include "utils/preference.hpp"
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "gl_fw/glutils.hpp"

int main(int argc, char** argv);

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  アプリケーション・共有リソース
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct core {

		al::sound	sound_;

		gui::widget_director	widget_director_;

		sys::preference		preference_;

		core() { }
	};
}
