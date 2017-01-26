//=====================================================================//
/*! @file
	@brief  cave メイン クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "core/glcore.hpp"
#include "gl_fw/glutils.hpp"
#include "logger_main.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void logger_main::initialize()
	{
		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{	// ターミナルのテスト
			{
				widget::param wp(vtx::irect(10, 10, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(30);
				terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0), terminal_frame_);
				widget_terminal::param wp_;
				terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
			}
		}


		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;

		if(terminal_frame_) {
			terminal_frame_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void logger_main::update()
	{
		gui::widget_director& wd = director_.at().widget_director_;



		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void logger_main::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void logger_main::destroy()
	{
		sys::preference& pre = director_.at().preference_;

		if(terminal_frame_) {
			terminal_frame_->save(pre);
		}
	}
}
