//=====================================================================//
/*! @file
	@brief  vplayer メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "vplayer.hpp"
#include "core/glcore.hpp"
#include <boost/format.hpp>

namespace app {

	void vplayer::output_term_(const std::string& text)
	{
		if(terminal_core_ == nullptr) return;
		if(text.empty()) return;

		terminal_core_->output(text);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void vplayer::initialize()
	{
		// AV Decoder 初期化
		decoder_.initialize();

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
			wp_.select_file_func_ = [this] (const std::string& path) {
				if(!decode_open_) {
					decode_open_ = decoder_.open(path);
					if(decode_open_) {
						output_term_("File: '" + path + "'\n");
						auto x = decoder_.get_frame_size().x;
						auto y = decoder_.get_frame_size().y;
						output_term_((boost::format("Screen: %d, %d\n") % x % y).str()); 
///						decoder_.info();
						int depth = 24;
						texfb_.initialize(x, y, depth);
					} else {
						output_term_("Can't open AV file: '" + path + "'\n");
					}
				}
			};
			load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
			load_ctx_->enable(false);
		}


		{	// ターミナル
			{
				widget::param wp(vtx::srect(300, 300, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(20);
				terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::srect(0), terminal_frame_);
				widget_terminal::param wp_;
				terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
			}
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(load_ctx_) load_ctx_->load(pre);
		if(tools_frame_) tools_frame_->load(pre, false, false);
		if(terminal_frame_) terminal_frame_->load(pre);
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

		if(open_file_ != nullptr && !decode_open_) {
			if(open_file_->get_selected()) {
				if(load_ctx_) {
					bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
					load_ctx_->enable(!f);
				}
			}
		}

		// AV デコーダー更新
		if(decode_open_) {
			static int f_ = 0;
			if(f_ & 1) {
				bool f = decoder_.update(director_.at().sound_);
				if(f) {
					std::cout << "Frames: " << static_cast<unsigned int>(decoder_.get_frame_no()) << std::endl;
					decoder_.close();
					decode_open_ = false;
				} else {
					const void* img = decoder_.get_frame();
					if(img) {
						texfb_.rendering(gl::texfb::image::RGB, img);
						texfb_.flip();
					}
				}
			}
			++f_;
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
		gl::core& core = gl::core::get_instance();
		texfb_.setup_matrix(0, 0, core.get_size().x, core.get_size().y);
		texfb_.set_disp_start(vtx::ipos(0, 0));
		texfb_.draw();

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
		if(terminal_frame_) terminal_frame_->save(pre);
		if(load_ctx_) load_ctx_->save(pre);
		if(tools_frame_) tools_frame_->save(pre);
	}
}
