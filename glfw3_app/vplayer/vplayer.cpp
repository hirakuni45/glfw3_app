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

		camera_.set_eye(vtx::fvtx(-100.0f, 0.0f, 200.0f));
		camera_.set_target(vtx::fvtx(0.0f, 0.0f, 200.0f));
		camera_.set_up(vtx::fvtx(0.0f, 0.0f, 1.0f));

		// 半球ドーム作成
		surface_.create_dome(vtx::fvtx(500.0f, 500.0f, 500.0f), 64);

		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{ // ツールパレット
			widget::param wp(vtx::irect(10, 10, 200, 350));
			widget_frame::param wp_;
			tools_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			tools_frame_->set_state(widget::state::SIZE_LOCK);
		}

		{ // オープンファイルボタン
			widget::param wp(vtx::irect(10, 10, 180, 40), tools_frame_);
			widget_button::param wp_("load");
			wp_.select_func_ = [this] (int id) {
				if(load_ctx_) {
					bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
					load_ctx_->enable(!f);
				}
			};
			open_file_ = wd.add_widget<widget_button>(wp, wp_);
		}

		{ // ボリューム・スライダー
			widget::param wp(vtx::irect(10, 60, 180, 20), tools_frame_);
			widget_slider::param wp_;
			wp_.select_func_ = [this] (float value) {
				auto& s = director_.at().sound_;
				s.set_gain_audio(value);
			};
			volume_ = wd.add_widget<widget_slider>(wp, wp_);
		}

		{ // Play/Pause ボタン
			widget::param wp(vtx::irect( 10, 90, 85, 40), tools_frame_);
			widget_button::param wp_("Play");
			wp_.select_func_ = [this] (int id) {
				if(decode_open_) {
					bool f = director_.at().sound_.status_audio();
					if(f) {
						decode_pause_ = true;
						director_.at().sound_.pause_audio();
					} else {
						decode_pause_ = false;
						director_.at().sound_.play_audio();
					}
				}
			};
			play_pause_ = wd.add_widget<widget_button>(wp, wp_);
		}

		{ // Stop ボタン
			widget::param wp(vtx::irect(100, 90, 85, 40), tools_frame_);
			widget_button::param wp_("Stop");
			wp_.select_func_ = [this] (int id) {
				decoder_.close();
				decode_open_ = false;
				decode_pause_ = false;
				director_.at().sound_.stop_audio();
			};
			stop_ = wd.add_widget<widget_button>(wp, wp_);
		}

		{ // チェックボックスのテスト
			widget::param wp(vtx::irect(10, 130, 180, 40), tools_frame_);
			widget_check::param wp_("Dome Map");
			dome_ = wd.add_widget<widget_check>(wp, wp_);
		}

#if 0
		if(1) {	// ラジオボタンのテスト
			widget::param wpr(vtx::irect(20, 20, 130, 130), 0);
			widget_null::param wpr_;
			widget* root = wd.add_widget<widget_null>(wpr, wpr_);
			root->set_state(widget::state::POSITION_LOCK);

			widget::param wp(vtx::irect(0, 0, 130, 30), root);
			widget_radio::param wp_("Enable");
			for(int i = 0; i < 3; ++i) {
				if(i == 2) wp_.check_ = true;
				widget_radio* w = wd.add_widget<widget_radio>(wp, wp_);
				w->at_local_param().select_func_ = [this](bool f, int n) {
					std::cout << "Radio button: " << static_cast<int>(f) << " (" << n << ")" << std::endl;
				};
				wp.rect_.org.y += 40;
			}
		}
#endif

		{ // load ファイラー本体
			widget::param wp(vtx::irect(10, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path());
			wp_.select_file_func_ = [this] (const std::string& path) {
				bool open = decoder_.open(path);
				if(open) {
					decode_open_ = true;
					decode_pause_ = false;
					frame_time_ = 0.0;
					output_term_("File: " + path + '\n');
					auto x = decoder_.get_frame_size().x;
					auto y = decoder_.get_frame_size().y;
					output_term_((boost::format("Frame size: %d, %d\n") % x % y).str());
					auto fr = decoder_.get_frame_rate();
					output_term_((boost::format("Frame rate: %2.2f [fps]\n") % fr).str());
					auto ar = decoder_.get_audio_rate();
					output_term_((boost::format("Audio sample rate: %d [Hz]\n") % ar).str());
					auto ac = decoder_.get_audio_chanel();
					output_term_((boost::format("Audio chanel%s: %d\n") % ((ac > 1) ? "s" : "") % ac).str());
					auto af = decoder_.get_audio_format();
					std::string s;
					if(af == av::decoder::audio_format::none) {
						s = "none";
					} else if(af == av::decoder::audio_format::u8) {
						s = "u8";
					} else if(af == av::decoder::audio_format::s16) {
						s = "s16";
					} else if(af == av::decoder::audio_format::f32) {
						s = "f32";
					} else if(af == av::decoder::audio_format::invalid) {
						s = "invalid";
					}
					output_term_("Audio format: " + s + '\n');
					int depth = 24;
					texfb_.initialize(x, y, depth);
				} else {
					if(dialog_) {
						dialog_->enable();
						dialog_->set_text("ファイル\n" + path + "\nは開けませんでした。");
					}
				}
			};
			load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
			load_ctx_->enable(false);
		}

		{  // ダイアログ本体
			widget::param wp(vtx::irect(300, 300, 400, 200));
			widget_dialog::param wp_;
//			wp_.style_ = widget_dialog::param::style::CANCEL_OK;
			dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			dialog_->enable(false);
		}

		{	// ターミナル
			{
				widget::param wp(vtx::irect(300, 300, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(20);
				terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0), terminal_frame_);
				widget_terminal::param wp_;
				wp_.echo_ = false;
				terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
			}
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(tools_frame_) tools_frame_->load(pre, false, false);
		if(volume_) volume_->load(pre);
		if(dome_) dome_->load(pre);
		if(load_ctx_) load_ctx_->load(pre);
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

		// AV デコーダー更新
		if(decode_open_ && !decode_pause_) {
			frame_time_ += 1.0 / 60.0;
			double vt = decoder_.get_video_time();
			if(vt < frame_time_) { 
				bool f = decoder_.update();
				if(f) {
					output_term_((boost::format("Total: %d Frames\n") % decoder_.get_frame_no()).str());
					decoder_.close();
					decode_open_ = false;
				} else {
					const void* img = decoder_.get_image();
					if(img) {
						texfb_.rendering(gl::texfb::IMAGE::RGB, img);
						texfb_.flip();
					}
				}
			}
			double at = decoder_.get_audio_time();
//			if(at < frame_time_) {
				av::decoder::audio_deque& a = decoder_.at_audio();
				for(int i = 0; i < a.size(); ++i) {
					al::audio ai = a.front();
					if(director_.at().sound_.queue_audio(ai)) {
						a.pop_front();
					}
				}
//			}
		}

		// ボタンの状態を設定
		if(play_pause_) {
			if(decode_pause_) {
				play_pause_->set_text("Play");
			} else {
				play_pause_->set_text("Pause");
			}
			play_pause_->set_stall(!decode_open_);
		}
		if(stop_) {
			stop_->set_stall(!decode_open_);
		}

		// GUI が操作されない場合、カメラ操作
		if(!wd.update()) {
			camera_.update();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void vplayer::render()
	{
		if(decode_open_) {
			if(dome_ != nullptr && dome_->get_check()) {
				camera_.service();

				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

				glBindTexture(GL_TEXTURE_2D, texfb_.get_texture_id());
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				surface_.draw(gl::surface::draw_type::fill);
//				surface_.draw(gl::surface::draw_type::line);
			} else {
				gl::core& core = gl::core::get_instance();
				texfb_.setup_matrix(0, 0, core.get_size().x, core.get_size().y);
				texfb_.set_disp_start(vtx::ipos(0, 0));
				texfb_.draw();
			}
		}

		glEnable(GL_TEXTURE_2D);
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
		if(volume_) volume_->save(pre);
		if(dome_) dome_->save(pre);
	}
}
