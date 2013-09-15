#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_frame.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI terminal クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class widget_terminal {

		widget_director&	wd_;

		widget_frame*		base_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_terminal(widget_director& wd) : wd_(wd),
			base_(0)
		{ }


	};
}

