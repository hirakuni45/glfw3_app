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
#include <boost/lexical_cast.hpp>

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
		widget_director& wd = director_.at().widget_director_;

		{ // 画像ファイル表示用フレーム
			widget::param wp(vtx::srect(30, 30, 256, 256));
			widget_frame::param wp_;
			wp_.plate_param_.set_caption(24);
			wp_.text_param_.text_ = "元画像";
			src_frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{ // 画像ファイル表示イメージ
			widget::param wp(vtx::srect(0, 0, 256, 256), src_frame_);
			widget_image::param wp_;
			wp_.linear_ = false;
			src_image_ = wd.add_widget<widget_image>(wp, wp_);
			src_image_->set_state(widget::state::CLIP_PARENTS);
			src_image_->set_state(widget::state::RESIZE_ROOT);
			src_image_->set_state(widget::state::MOVE_ROOT, false);
		}

		{ // 画像ファイル表示用フレーム
			widget::param wp(vtx::srect(60, 60, 256, 256));
			widget_frame::param wp_;
			wp_.plate_param_.set_caption(24);
			wp_.text_param_.text_ = "変換後";
			dst_frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{ // 画像ファイル表示イメージ
			widget::param wp(vtx::srect(0, 0, 256, 256), dst_frame_);
			widget_image::param wp_;
			wp_.linear_ = false;
			dst_image_ = wd.add_widget<widget_image>(wp, wp_);
			dst_image_->set_state(widget::state::CLIP_PARENTS);
			dst_image_->set_state(widget::state::RESIZE_ROOT);
			dst_image_->set_state(widget::state::MOVE_ROOT, false);
		}


		{ // 機能ツールパレット
			widget::param wp(vtx::srect(10, 10, 150, 300));
			widget_frame::param wp_;
			tools_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{ // スケール
			widget::param wp(vtx::srect(10, 10, 130, 40), tools_);
			widget_check::param wp_("scale");
			scale_ = wd.add_widget<widget_check>(wp, wp_);
		}
		{ // BDF page list
			widget::param wp(vtx::srect(10, 60, 100, 40), tools_);
			widget_list::param wp_("0");
			const bmc_core& bmc = *director_.at().bmc_;
			for(uint32_t i = 0; i < bmc.get_bdf_pages(); ++i) {
				std::string s = boost::lexical_cast<std::string>(i);
				wp_.text_list_.push_back(s);
			}
			bdf_page_no_ = 0;
			bdf_page_ = wd.add_widget<widget_list>(wp, wp_);
		}
		{ // ファイラー起動ボタン
//			widget::param wp(vtx::srect(5, 5, 100, 40), tools_);
//			widget_button::param wp_("file");
//			open_ = wd.add_widget<widget_button>(wp, wp_);
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
			src_frame_->load(pre);
			dst_frame_->load(pre);
			tools_->load(pre);
		}

		// コアの画像を表示
		if(director_.at().bmc_) {
			bmc_core& bmc = *director_.at().bmc_;
			if(!bmc.get_src_image().empty()) {
				src_handle_ = mobj_.install(&bmc.get_src_image());
				src_image_->at_local_param().mobj_ = mobj_;
				src_image_->at_local_param().mobj_handle_ = src_handle_;
			}
			if(!bmc.get_dst_image().empty()) {
				dst_handle_ = mobj_.install(&bmc.get_dst_image());
				dst_image_->at_local_param().mobj_ = mobj_;
				dst_image_->at_local_param().mobj_handle_ = dst_handle_;
			}
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

		gui::widget_director& wd = director_.at().widget_director_;
#if 0
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
					dialog_->set_text("Can't decode image file:\n '"
						+ filer_->get_file() + "'");
					dialog_->enable();
				} else {
//					mobj_.destroy();
//					mobj_.initialize();
//					img_handle_ = mobj_.install(imf.get_image_if());
//					image_->at_local_param().mobj_ = mobj_;
//					image_->at_local_param().mobj_handle_ = img_handle_;
///					imf.set_image_if(imf.get_image_if());
///					imf.save("test.tga", "rle");
				}
			}
		}
#endif
		// frame 内 image のサイズを設定
		if(src_frame_ && src_image_) {
			src_frame_->create_draw_area(src_image_->at_rect());

			float s = 1.0f;
			if(scale_->get_check()) {
				vtx::fpos is = mobj_.get_size(src_handle_);
				vtx::fpos ss = src_image_->at_rect().size;
				vtx::fpos sc = ss / is;
				if(sc.x < sc.y) s = sc.x; else s = sc.y;
			}
			src_image_->at_local_param().scale_ = s;

			if(src_image_->get_select_in()) {
				wd.top_widget(src_frame_);
				src_image_offset_ = src_image_->get_local_param().offset_;
			}
			if(src_image_->get_select()) {
				vtx::spos d = src_image_->get_param().move_pos_ - src_image_->get_param().move_org_;
				src_image_->at_local_param().offset_ = src_image_offset_ + d / s;
			}
		}

		// BDF ファイルの複数ページ対応
		bmc_core& bmc = *director_.at().bmc_;
		if(bmc.get_bdf_pages()) {
			if(bdf_page_->get_local_param().select_pos_ != bdf_page_no_) {
				bdf_page_no_ = bdf_page_->get_local_param().select_pos_;
				bmc.create_bdf_image(bdf_page_no_);
				mobj_.destroy();
				mobj_.initialize();
				dst_handle_ = mobj_.install(&bmc.get_dst_image());
				dst_image_->at_local_param().mobj_ = mobj_;
				dst_image_->at_local_param().mobj_handle_ = dst_handle_;
			}
		}

		if(dst_frame_ && dst_image_) {
			dst_frame_->create_draw_area(dst_image_->at_rect());

			float s = 1.0f;
			if(scale_->get_check()) {
				vtx::fpos is = mobj_.get_size(dst_handle_);
				vtx::fpos ss = dst_image_->at_rect().size;
				vtx::fpos sc = ss / is;
				if(sc.x < sc.y) s = sc.x; else s = sc.y;
			}
			dst_image_->at_local_param().scale_ = s;

			if(dst_image_->get_select_in()) {
				wd.top_widget(dst_frame_);
				dst_image_offset_ = dst_image_->get_local_param().offset_;
			}
			if(dst_image_->get_select()) {
				vtx::spos d = dst_image_->get_param().move_pos_ - dst_image_->get_param().move_org_;
				dst_image_->at_local_param().offset_ = dst_image_offset_ + d / s;
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
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();

		gl::IGLcore* igl = gl::get_glcore();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void bmc_main::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->save(pre);
			src_frame_->save(pre);
			dst_frame_->save(pre);
			tools_->save(pre);
		}
	}
}
