//=====================================================================//
/*!	@file
	@brief	日本語 FEP をインターフェースするクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "core/jfep_io.hpp"

namespace input {


// ImmReleaseContext(HWND, himc_);


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	 */
	//-----------------------------------------------------------------//
	void jfep_io::initialize()
	{
		himc_ = ImmGetContext(0);


	}



}
