//=====================================================================//
/*! @file
	@brief  vplayer メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "vplayer.hpp"
#include "core/glcore.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void vplayer::initialize()
	{
		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{ // ツールパレット
			widget::param wp(vtx::srect(10, 10, 130, 350));
			widget_frame::param wp_;
			tools_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			tools_frame_->set_state(widget::state::SIZE_LOCK);
		}
		{ // オープンファイルボタン
			widget::param wp(vtx::srect(10, 10, 110, 40), tools_frame_);
			widget_button::param wp_("load");
			open_file_ = wd.add_widget<widget_button>(wp, wp_);
		}

		{ // load ファイラー本体
			widget::param wp(vtx::srect(10, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path());
			load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
			load_ctx_->enable(false);
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(load_ctx_) load_ctx_->load(pre);
		if(tools_frame_) tools_frame_->load(pre, false, false);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void vplayer::update()
	{
		gl::core& core = gl::core::get_instance();

		gui::widget_director& wd = director_.at().widget_director_;

		if(open_file_) {
			if(open_file_->get_selected()) {
				if(load_ctx_) {
					bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
					load_ctx_->enable(!f);
				}
			}
		}


		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void vplayer::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void vplayer::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(load_ctx_) load_ctx_->save(pre);
		if(tools_frame_) tools_frame_->save(pre);
	}
}
