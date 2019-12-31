#pragma once
//=====================================================================//
/*! @file
	@brief  DOOM メイン・ラップ・クラス @n
			Copyright 2019 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_slider.hpp"
#include "gl_fw/gltexfb.hpp"
#include "snd_io/pcm.hpp"
#include "utils/fifo.hpp"
#include "utils/format.hpp"
#include "utils/input.hpp"

#include "doomdef.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	DOOM シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class doom : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;
		bool					terminal_;

		gui::widget_filer*		filer_;

		gui::widget_frame*		menu_frame_;
		bool					menu_;
		gui::widget_spinbox*	state_slot_;
		gui::widget_button*		state_save_;
		gui::widget_button*		state_load_;
		gui::widget_button*		nes_reset_;
		gui::widget_slider*		volume_;

		gui::widget_dialog*		dialog_;

		gl::texfb				texfb_;

		static const int sys_width_  = 320;
		static const int sys_height_ = 240;
		static const int sample_rate_ = 44100;
		static const int audio_len_ = sample_rate_ / 60;

		uint8_t			fb_[sys_width_ * sys_height_ * 4];

///		emu::tools		tools_;


		void pad_()
		{
	   		gl::core& core = gl::core::get_instance();

			const auto& dev = core.get_device();

			// A
			if(dev.get_level(gl::device::key::Z)) {
///				inp_[0].data |= INP_PAD_A;
			}
			if(dev.get_level(gl::device::key::GAME_0)) {
///				inp_[0].data |= INP_PAD_A;
			}

			// B
			if(dev.get_level(gl::device::key::X)) {
///				inp_[0].data |= INP_PAD_B;
			}
			if(dev.get_level(gl::device::key::GAME_1)) {
///				inp_[0].data |= INP_PAD_B;
			}

			// SELECT
			if(dev.get_level(gl::device::key::_1)) {
///				inp_[0].data |= INP_PAD_SELECT;
			}
			if(dev.get_level(gl::device::key::GAME_2)) {
///				inp_[0].data |= INP_PAD_SELECT;
			}
			// START
			if(dev.get_level(gl::device::key::_2)) {
///				inp_[0].data |= INP_PAD_START;
			}
			if(dev.get_level(gl::device::key::GAME_3)) {
///				inp_[0].data |= INP_PAD_START;
			}

			if(dev.get_level(gl::device::key::LEFT)) {
///				inp_[0].data |= INP_PAD_LEFT;
			}
			if(dev.get_level(gl::device::key::GAME_LEFT)) {
///				inp_[0].data |= INP_PAD_LEFT;
			}
			if(dev.get_level(gl::device::key::RIGHT)) {
///				inp_[0].data |= INP_PAD_RIGHT;
			}
			if(dev.get_level(gl::device::key::GAME_RIGHT)) {
///				inp_[0].data |= INP_PAD_RIGHT;
			}
			if(dev.get_level(gl::device::key::UP)) {
///				inp_[0].data |= INP_PAD_UP;
			}
			if(dev.get_level(gl::device::key::GAME_UP)) {
///				inp_[0].data |= INP_PAD_UP;
			}
			if(dev.get_level(gl::device::key::DOWN)) {
///				inp_[0].data |= INP_PAD_DOWN;
			}
			if(dev.get_level(gl::device::key::GAME_DOWN)) {
///				inp_[0].data |= INP_PAD_DOWN;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		doom(utils::director<core>& d) : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr), terminal_(false),
			menu_frame_(nullptr), menu_(false),
			volume_(nullptr),
			dialog_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~doom() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
	   		gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			{  	// ターミナル
				{
					widget::param wp(vtx::irect(10, 10, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
					terminal_frame_->enable(terminal_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [=] (const utils::lstring& inp) {
						auto s = utils::utf32_to_utf8(inp);
///						tools_.command(s);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);

///					emu::tools::set_terminal(terminal_core_);
				}
			}

			{  	// ファイラー
				widget::param wp(vtx::irect(10, 10, 200, 200));
				widget_filer::param wp_(core.get_current_path(), "");
				wp_.select_file_func_ = [=](const std::string& fn) {

///					if(nes_insertcart(fn.c_str()) == 0) {
///						nes_file_ = fn;
///						nes_play_ = true;
///						tools_.enable();
///					} else {
///						nes_file_ = "";
///						nes_play_ = false;
///						dialog_->enable();
///						dialog_->set_text("NES file false:\n'" + fn + "'");
///						tools_.enable(false);
///					}

				};
				filer_ = wd.add_widget<widget_filer>(wp, wp_);
				filer_->enable(false);
			}

			{
				{   // メニュー
					widget::param wp(vtx::irect(20, 20, 210, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					menu_frame_ = wd.add_widget<widget_frame>(wp, wp_);
					menu_frame_->enable(false);
					menu_frame_->at_param().state_.set(widget::state::SIZE_LOCK);
				}
				{   // スピン・ボックス
					widget::param wp(vtx::irect(10, 35, 90, 30), menu_frame_);
					widget_spinbox::param wp_(0, 0, 9);
					wp_.select_func_ = [=] (widget_spinbox::state st, int before, int newpos) {
						return (boost::format("%d") % newpos).str();
					};
					state_slot_ = wd.add_widget<widget_spinbox>(wp, wp_);
				}
				{ // ステート・セーブ
					widget::param wp(vtx::irect(110, 35+40*0, 90, 30), menu_frame_);
					widget_button::param wp_("Save");
					state_save_ = wd.add_widget<widget_button>(wp, wp_);
					state_save_->at_local_param().select_func_ = [=](int id) {
///						state_setslot(get_state_no_());
///						if(state_save() != 0) {
///							dialog_->set_text("Save state error");
///							dialog_->enable();
///						} else {
///							if(filer_ != nullptr) filer_->rescan_center();
///						}
					};

				}
				{ // ステート・ロード
					widget::param wp(vtx::irect(110, 35+40*1, 90, 30), menu_frame_);
					widget_button::param wp_("Load");
					state_load_ = wd.add_widget<widget_button>(wp, wp_);
					state_load_->at_local_param().select_func_ = [=](int id) {
///						state_setslot(get_state_no_());
///						if(!nes_play_) {
///							dialog_->set_text("Load state error: 'NES file not load'");
///							dialog_->enable();
///						} else if(state_load() != 0) {
///							dialog_->set_text("Load state error");
///							dialog_->enable();
///						}
					};
				}
				{ // NES リセット
					widget::param wp(vtx::irect(10, 35+40*2, 90, 30), menu_frame_);
					widget_button::param wp_("Reset");
					nes_reset_ = wd.add_widget<widget_button>(wp, wp_);
					nes_reset_->at_local_param().select_func_ = [=](int id) {
///						nes_reset(HARD_RESET);
					};
				}
				{   // ボリューム
					widget::param wp(vtx::irect(10, 35+40*3, 190, 20), menu_frame_);
					widget_slider::param wp_(0.5f);
					volume_ = wd.add_widget<widget_slider>(wp, wp_);
				}
			}

			{   // Daialog
				widget::param wp(vtx::irect(70, 70, 300, 150));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
			}

			texfb_.initialize(sys_width_, sys_height_, 32);

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(filer_ != nullptr) {
				filer_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(menu_frame_ != nullptr) {
				menu_frame_->load(pre, false, false);
			}
			if(volume_ != nullptr) {
				volume_->load(pre);
			}


			static char* argv[3];
			static char tmp[256];
			argv[0] = &tmp[0];
			strcpy(argv[0], "doom");
			argv[1] = &tmp[16];
			strcpy(argv[1], "-quiet");
			argv[2] = &tmp[16];
			strcpy(argv[2], "DOOM1.WAD");
			doom_main(3, argv);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
	   		gl::core& core = gl::core::get_instance();
			const gl::device& dev = core.get_device();

			if(dev.get_negative(gl::device::key::F1)) {
				filer_->enable(!filer_->get_enable());
			}
			if(dev.get_negative(gl::device::key::F5)) {
				terminal_ = !terminal_;
				terminal_frame_->enable(terminal_);
			}
			if(dev.get_positive(gl::device::key::F2)) {
				menu_ = !menu_;
				menu_frame_->enable(menu_);
			}

			uint32_t keybits = 0;
			if(dev.get_level(gl::device::key::UP)) {
				keybits |= KEY_BITS_UP;
			}
			if(dev.get_level(gl::device::key::DOWN)) {
				keybits |= KEY_BITS_DOWN;
			}
			if(dev.get_level(gl::device::key::RIGHT)) {
				keybits |= KEY_BITS_RIGHT;
			}
			if(dev.get_level(gl::device::key::LEFT)) {
				keybits |= KEY_BITS_LEFT;
			}
			if(dev.get_level(gl::device::key::ENTER)) {
				keybits |= KEY_BITS_ENTER;
			}
			if(dev.get_level(gl::device::key::ESCAPE)) {
				keybits |= KEY_BITS_ESC;
			}
			if(dev.get_level(gl::device::key::TAB)) {
				keybits |= KEY_BITS_TAB;
			}

			doom_keybits(keybits);
			doom_frame(sys_width_, sys_height_, fb_);
			texfb_.rendering(gl::texfb::IMAGE::RGBA, (const char*)&fb_[0]);
			texfb_.flip();
#if 0
			if(nes_play_) {

				// copy sound emulation
				al::sound& sound = director_.at().sound_;
				uint32_t len = audio_len_;
				uint32_t mod = 16;
				if(sound.get_queue_audio_length() < mod) {
					len += mod;
				}
				al::sound::waves16 tmp;
				tmp.resize(len);
				apu_process(&tmp[0], len);
				sound.queue_audio(tmp);

				// copy video
				if(nes_play_) {
					auto nes = nes_getcontext();
					bitmap_t* v = nes->vidbuf;
					const rgb_t* lut = get_palette();
					if(v != nullptr && lut != nullptr) {
						for(int h = 0; h < nes_height_; ++h) {
							const uint8_t* src = &v->data[h * v->pitch];
							uint8_t* dst = &fb_[h * nes_width_ * 4];
							for(int w = 0; w < nes_width_; ++w) {
								auto idx = *src++;
								idx &= 63;
								*dst++ = lut[idx].r;  // R
								*dst++ = lut[idx].g;  // G
								*dst++ = lut[idx].b;  // B
								*dst++ = 255;  // alpha
							}
						}
						texfb_.rendering(gl::texfb::IMAGE::RGBA, (const char*)&fb_[0]);
					}
					texfb_.flip();
				}
			}
#endif
			// ストリームのゲイン(volume)を設定
			if(volume_ != nullptr) {
				auto vol = volume_->get_slider_param().position_;
				al::sound& sound = director_.at().sound_;
				sound.set_gain_stream(vol);
			}

        	gui::widget_director& wd = director_.at().widget_director_;
			wd.update();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render()
		{
	   		gl::core& core = gl::core::get_instance();

			const vtx::spos& siz = core.get_rect().size;

			texfb_.setup_matrix(0, 0, siz.x, siz.y);

			float scale = 1.0f;
			float ofsx = 0.0f;
			float ofsy = 0.0f;
			if(siz.x < siz.y) {
				scale = static_cast<float>(siz.x) / static_cast<float>(sys_width_);
				float h = static_cast<float>(sys_height_);
				ofsy = (static_cast<float>(siz.y) - h * scale) * 0.5f;
			} else {
				scale = static_cast<float>(siz.y) / static_cast<float>(sys_height_);
				float w = static_cast<float>(sys_width_);
				ofsx = (static_cast<float>(siz.x) - w * scale) * 0.5f;
			}
			gl::glTranslate(ofsx, ofsy);
			gl::glScale(scale);
			texfb_.draw();

			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
///			emu::tools::set_terminal(nullptr);
///			nes_destroy();

			sys::preference& pre = director_.at().preference_;
			if(filer_ != nullptr) {
				filer_->save(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(menu_frame_ != nullptr) {
				menu_frame_->save(pre);
			}
			if(volume_ != nullptr) {
				volume_->save(pre);
			}
		}
	};

}
