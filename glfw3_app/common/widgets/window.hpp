#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget window クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widget.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI window クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class window : public widget {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	window パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			int			round_radius_;
			int			frame_width_;
			widget::color_param		color_param_;

			widget*		handle_;

			param() : round_radius_(2), frame_width_(4), color_param(),
				handle_(0),
		};

	public:
		window() { }



	};

}
