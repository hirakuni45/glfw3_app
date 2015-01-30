//=====================================================================//
/*! @file
	@brief  player クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "core/glcore.hpp"
#include "gl_fw/glutils.hpp"
#include "effv_main.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void effv_main::initialize()
	{
		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{	// ファイラー・リソース
			widget::param wp(vtx::srect(30, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}
		{	// ツールパレット
			widget::param wp(vtx::srect(20, 20, 150, 350));
			widget_frame::param wp_;
			wp_.plate_param_.set_caption(12);
			tools_ = wd.add_widget<widget_frame>(wp, wp_);
			tools_->set_state(gui::widget::state::SIZE_LOCK);
		}
		short h = 12 + 10;
		{	// ファイラー起動ボタン
			widget::param wp(vtx::srect(10, h, 100, 36), tools_);
			h += 40;
			widget_button::param wp_("開く");
			fopen_ = wd.add_widget<widget_button>(wp, wp_);
		}
		{	// Grid、On/Off
			widget::param wp(vtx::srect(10, h, 150, 30), tools_);
			h += 30;
			widget_check::param wp_("Grid", true);
			grid_ = wd.add_widget<widget_check>(wp, wp_);
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void effv_main::update()
	{
		gui::widget_director& wd = director_.at().widget_director_;

		if(fopen_->get_selected()) {
			bool f = filer_->get_state(gui::widget::state::ENABLE);
			filer_->enable(!f);
		}

		/// ファイラー、ファイル選択
		if(filer_id_ != filer_->get_select_file_id()) {
			filer_id_ = filer_->get_select_file_id();


		}

		if(!wd.update()) {
			camera_.update();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void effv_main::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::glColor(img::rgbaf(1.0f));

		camera_.service();

		if(grid_->get_check()) {
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			gl::draw_grid(vtx::fpos(-10.0f), vtx::fpos(10.0f), vtx::fpos(1.0f));
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_BLEND);
		}

///		light_.enable(false);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void effv_main::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->save(pre);
		}
	}
}
