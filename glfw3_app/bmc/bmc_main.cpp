//=====================================================================//
/*! @file
	@brief  BMC メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "bmc_main.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_utils.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void bmc_main::initialize()
	{
		gl::IGLcore* igl = gl::get_glcore();

		using namespace gui;
		widget_director& wd = director_.at_core().widget_director_;

		{ // 画像ファイル表示用フレーム
			widget::param wp(vtx::srect(30, 30, 256, 256));
			widget_frame::param wp_;
			frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		if(0) { // 画像イメージ（子）
			widget::param wp(vtx::srect(0, 0, 256, 256), frame_);
			widget_image::param wp_;
			image_ = wd.add_widget<widget_image>(wp, wp_);
		}

		{ // ファイラー起動ボタン
			widget::param wp(vtx::srect(10, 10, 150, 40));
			widget_button::param wp_("画像ファイル");
			open_ = wd.add_widget<widget_button>(wp, wp_);
		}

		{ // ファイラー本体
			widget::param wp(vtx::srect(10, 30, 300, 200));
			widget_filer::param wp_(igl->get_current_path());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}

		mobj_.initialize();

		// プリファレンスの取得
		sys::preference& pre = director_.at_core().preference_;
		if(filer_) {
			filer_->load(pre);
			frame_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void bmc_main::update()
	{
		gl::IGLcore* igl = gl::get_glcore();
		const vtx::spos& size = igl->get_size();

		gui::widget_director& wd = director_.at_core().widget_director_;

		if(open_) {
			if(open_->get_selected()) {
				if(filer_) {
					bool f = filer_->get_state(gui::widget::state::ENABLE);
					filer_->enable(!f);
				}
			}
		}
		if(filer_) {
			if(filer_id_ != filer_->get_select_file_id()) {
				filer_id_ = filer_->get_select_file_id();
///				std::cout << "Filer: '" << filer_->get_file() << "'" << std::endl;

				img::img_files& imf = wd.at_img_files();
				if(!imf.load(filer_->get_file())) {

				} else {
					img_handle_ = mobj_.install(imf.get_image_if());
					
///					imf.set_image_if(imf.get_image_if());
///					imf.save("test.tga", "rle");
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
	void bmc_main::render()
	{
		director_.at_core().widget_director_.service();
		director_.at_core().widget_director_.render();

		gl::IGLcore* igl = gl::get_glcore();

		if(img_handle_) {
			const vtx::spos& size = igl->get_size();
			mobj_.setup_matrix(size.x, size.y);
			mobj_.draw(img_handle_, gl::mobj::attribute::normal, 0, 0);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void bmc_main::destroy()
	{
		sys::preference& pre = director_.at_core().preference_;
		if(filer_) {
			filer_->save(pre);
			frame_->save(pre);
		}
	}
}
