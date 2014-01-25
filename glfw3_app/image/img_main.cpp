//=====================================================================//
/*! @file
	@brief  img メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "img_main.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_utils.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void img_main::initialize()
	{
		gl::IGLcore* igl = gl::get_glcore();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{ // 画像ファイル表示用フレーム
			widget::param wp(vtx::srect(30, 30, 256, 256));
			widget_frame::param wp_;
			frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{ // 画像ファイル表示イメージ
			widget::param wp(vtx::srect(0, 0, 256, 256), frame_);
			widget_image::param wp_;
			image_ = wd.add_widget<widget_image>(wp, wp_);
			image_->set_state(widget::state::CLIP_PARENTS);
			image_->set_state(widget::state::RESIZE_ROOT);
		}

		{ // 機能ツールパレット
			widget::param wp(vtx::srect(10, 10, 150, 300));
			widget_frame::param wp_;
			tools_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{ // ファイラー起動ボタン
			widget::param wp(vtx::srect(10, 10, 100, 40), tools_);
			widget_button::param wp_("file");
			open_ = wd.add_widget<widget_button>(wp, wp_);
		}
		{ // スケールチェックボックス
			widget::param wp(vtx::srect(10, 60, 100, 40), tools_);
			widget_check::param wp_("scale");
			scale_ = wd.add_widget<widget_check>(wp, wp_);	
		}

		{ // ファイラー本体
			widget::param wp(vtx::srect(10, 30, 300, 200));
			widget_filer::param wp_(igl->get_current_path());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}
		{ // ダイアログ
			widget::param wp(vtx::srect(10, 30, 450, 200));
			widget_dialog::param wp_;
			dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			dialog_->enable(false);
		}

		mobj_.initialize();

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->load(pre);
			frame_->load(pre);
			tools_->load(pre);
			scale_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void img_main::update()
	{
		gl::IGLcore* igl = gl::get_glcore();
		const vtx::spos& size = igl->get_size();

		gui::widget_director& wd = director_.at().widget_director_;

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

				img::img_files& imf = wd.at_img_files();
				if(!imf.load(filer_->get_file())) {
					dialog_->set_text("Can't decode image file:\n '"
						+ filer_->get_file() + "'");
					dialog_->enable();
				} else {
					mobj_.destroy();
					mobj_.initialize();
					img_handle_ = mobj_.install(imf.get_image_if());
					image_->at_local_param().mobj_ = mobj_;
					image_->at_local_param().mobj_handle_ = img_handle_;
// imf.set_image_if(imf.get_image_if());
// imf.save("test.jp2");
				}
			}
		}

		// frame 内 image のサイズを設定
		if(frame_ && image_) {
			vtx::spos ofs(frame_->get_local_param().plate_param_.frame_width_);
			image_->at_rect().org = ofs;
			image_->at_rect().size = frame_->get_rect().size - ofs * 2;

			float s = 1.0f;
			if(scale_->get_check()) {
				vtx::fpos is = mobj_.get_size(img_handle_);
				vtx::fpos ss = image_->at_rect().size;
				vtx::fpos sc = ss / is;
				if(sc.x < sc.y) s = sc.x; else s = sc.y;
			}
			image_->at_local_param().scale_ = s;
		}

		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void img_main::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void img_main::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->save(pre);
			frame_->save(pre);
			tools_->save(pre);
			scale_->save(pre);
		}
	}
}
