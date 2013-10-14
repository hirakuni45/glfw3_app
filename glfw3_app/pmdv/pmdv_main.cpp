//=====================================================================//
/*! @file
	@brief  player クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "core/glcore.hpp"
#include "gl_fw/glutils.hpp"
#include "pmdv_main.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void pmdv_main::initialize()
	{
		gl::IGLcore* igl = gl::get_glcore();

		using namespace gui;
		widget_director& wd = director_.at_core().widget_director_;

		{	// ファイラー・リソース
			widget::param wp(vtx::srect(30, 30, 300, 200));
			widget_filer::param wp_(igl->get_current_path());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}
		{	// ツールパレット
			widget::param wp(vtx::srect(20, 20, 150, 350));
			widget_frame::param wp_;
			tools_ = wd.add_widget<widget_frame>(wp, wp_);
			tools_->set_state(gui::widget::state::SIZE_LOCK);
		}
		{	// ファイラー起動ボタン
			widget::param wp(vtx::srect(10, 10, 100, 40), tools_);
			widget_button::param wp_("開く");
			fopen_ = wd.add_widget<widget_button>(wp, wp_);
		}
		{
			widget::param wp(vtx::srect(10, 60, 150, 40), tools_);
			widget_check::param wp_("ボーン");
			bone_ = wd.add_widget<widget_check>(wp, wp_);
		}
#if 0
		if(1) {	// ツリー
			widget::param wp(vtx::srect(200, 400, 100, 100));
			widget_tree::param wp_;
			tree_ = wd.add_widget<widget_tree>(wp, wp_);

			widget_tree::tree_unit& tu = tree_->at_tree_unit();
			tu.make_directory("/root");
			tu.set_current_path("root");
			{
				widget_tree::value v;
				v.data_ = "AAA";
				tu.install("sub0", v);
			}
			{
				widget_tree::value v;
				v.data_ = "BBB";
				tu.install("sub1", v);
			}
			tu.list_all();
		}
#endif

		// プリファレンスの取得
		sys::preference& pre = director_.at_core().preference_;
		if(filer_) {
			filer_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void pmdv_main::update()
	{
		gl::IGLcore* igl = gl::get_glcore();
		const vtx::spos& size = igl->get_size();

		gui::widget_director& wd = director_.at_core().widget_director_;

		if(fopen_->get_selected()) {
			filer_->enable(fopen_->get_counts() & 1);
		}

		if(filer_id_ != filer_->get_select_file_id()) {
			filer_id_ = filer_->get_select_file_id();
			pmd_io_.open(filer_->get_file());
			pmd_io_.render_setup();
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
	void pmdv_main::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::glColor(img::rgbaf(1.0f));

		director_.at_core().widget_director_.service();

		camera_.service();
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

		pmd_io_.render_surface();
		if(bone_->get_check()) {
			pmd_io_.render_bone();
		}

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		director_.at_core().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void pmdv_main::destroy()
	{
		sys::preference& pre = director_.at_core().preference_;
		if(filer_) {
			filer_->save(pre);
		}
	}
}
