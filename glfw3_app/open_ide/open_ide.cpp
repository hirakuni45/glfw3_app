//=====================================================================//
/*! @file
	@brief  Open IDE クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/lexical_cast.hpp>
#include "open_ide.hpp"
#include "core/glcore.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void open_ide::initialize()
	{
//		gl::core& core = gl::core::get_instance();

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
	void open_ide::update()
	{
		gui::widget_director& wd = director_.at().widget_director_;



		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void open_ide::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void open_ide::destroy()
	{
		sys::preference& pre = director_.at().preference_;

		if(terminal_frame_) {
			terminal_frame_->save(pre);
		}
	}

}
