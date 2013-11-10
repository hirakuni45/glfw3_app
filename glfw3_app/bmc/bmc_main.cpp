//=====================================================================//
/*! @file
	@brief  BMC メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "bmc_main.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_filer.hpp"

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

//		if(1) { // フレームのテスト
//			widget::param wp(vtx::srect(200, 20, 100, 80));
//			widget_frame::param wp_;
//			wd.add_widget<widget_frame>(wp, wp_);
//		}

		{
			widget::param wp(vtx::srect(10, 10, 150, 40));
			widget_button::param wp_("画像ファイル");
			open_ = wd.add_widget<widget_button>(wp, wp_);
		}

		{
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
		}
	}
}
