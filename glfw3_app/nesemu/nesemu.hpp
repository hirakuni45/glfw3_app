#pragma once
//=====================================================================//
/*! @file
	@brief  NES Emulator クラス @n
			操作説明： @n
			「１」： SELECT @n
			「２」： START @n
			「Ｚ」： A-BUTTON @n
			「Ｘ」： B-BUTTON @n
			「↑」： UP-DIR @n
			「↓」： DOWN-DIR @n
			「→」： RIGHT-DIR @n
			「←」： LEFT-DIR @n
			「F1」： Filer @n
			「F4」： Log Terminal @n
			Copyright 2017 Kunihito Hiramatsu
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
#include "utils/input.hpp"
#include "tools.hpp"
#include "nsfplay.hpp"

extern "C" {
	#include "log.h"
	#include "nes.h"
	#include "nesinput.h"
	#include "nesstate.h"
	extern const rgb_t* get_palette();
};

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	nesemu シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class nesemu : public utils::i_scene {

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

		gl::texfb			texfb_;

		static const int nes_width_  = 256;
		static const int nes_height_ = 240;
		static const int sample_rate_ = 44100;
		static const int audio_len_ = sample_rate_ / 60;
		static const int audio_queue_ = 1024;

		nes_t*	nes_;

		bool	rom_active_;

		typedef utils::fifo<int16_t, audio_queue_ * 16> fifo;
		fifo		fifo_;
		al::audio	audio_;

		uint8_t	fb_[nes_width_ * nes_height_ * 4];

		nesinput_t	inp_[2];

		int		nes_pause_;

		emu::tools		tools_;

		emu::nsfplay	nsfplay_;

		void pad_()
		{
	   		gl::core& core = gl::core::get_instance();

			const gl::device& dev = core.get_device();

			inp_[0].data = 0;
			inp_[1].data = 0;

			// A
			if(dev.get_level(gl::device::key::Z)) {
				inp_[0].data |= INP_PAD_A;
			}
			if(dev.get_level(gl::device::key::GAME_0)) {
				inp_[0].data |= INP_PAD_A;
			}

			// B
			if(dev.get_level(gl::device::key::X)) {
				inp_[0].data |= INP_PAD_B;
			}
			if(dev.get_level(gl::device::key::GAME_1)) {
				inp_[0].data |= INP_PAD_B;
			}

			// SELECT
			if(dev.get_level(gl::device::key::_1)) {
				inp_[0].data |= INP_PAD_SELECT;
			}
			if(dev.get_level(gl::device::key::GAME_2)) {
				inp_[0].data |= INP_PAD_SELECT;
			}
			// START
			if(dev.get_level(gl::device::key::_2)) {
				inp_[0].data |= INP_PAD_START;
			}
			if(dev.get_level(gl::device::key::GAME_3)) {
				inp_[0].data |= INP_PAD_START;
			}

			if(dev.get_level(gl::device::key::LEFT)) {
   		 		inp_[0].data |= INP_PAD_LEFT;
			}
			if(dev.get_level(gl::device::key::GAME_LEFT)) {
   		 		inp_[0].data |= INP_PAD_LEFT;
			}
			if(dev.get_level(gl::device::key::RIGHT)) {
   		 		inp_[0].data |= INP_PAD_RIGHT;
			}
			if(dev.get_level(gl::device::key::GAME_RIGHT)) {
   		 		inp_[0].data |= INP_PAD_RIGHT;
			}
			if(dev.get_level(gl::device::key::UP)) {
   		 		inp_[0].data |= INP_PAD_UP;
			}
			if(dev.get_level(gl::device::key::GAME_UP)) {
   		 		inp_[0].data |= INP_PAD_UP;
			}
			if(dev.get_level(gl::device::key::DOWN)) {
   		 		inp_[0].data |= INP_PAD_DOWN;
			}
			if(dev.get_level(gl::device::key::GAME_DOWN)) {
   		 		inp_[0].data |= INP_PAD_DOWN;
			}
		}

		int get_state_no_() const {
			if(state_slot_ == nullptr) {
				return -1;
			}
			int n;
			if((utils::input("%d", state_slot_->get_select_text().c_str()) % n).status()) {
				return n;
			} else {
				return -1;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		nesemu(utils::director<core>& d) : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr), terminal_(false),
			menu_frame_(nullptr), menu_(false),
			state_slot_(nullptr), state_save_(nullptr), state_load_(nullptr), nes_reset_(nullptr),
			volume_(nullptr),
			dialog_(nullptr),
			nes_(nullptr), rom_active_(false), nes_pause_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~nesemu() { }


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
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);

					emu::tools::set_terminal(terminal_core_);
				}
			}

			{  	// ファイラー
				widget::param wp(vtx::irect(10, 10, 200, 200));
				widget_filer::param wp_(core.get_current_path(), "");
				wp_.select_file_func_ = [this](const std::string& fn) {
					if(nsfplay_.open(fn)) {

					} else if(nes_insertcart(fn.c_str(), nes_) == 0) {
						rom_active_ = true;
					} else {
						dialog_->enable();
						dialog_->set_text("NES file false:\n'" + fn + "'");
					}
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
					widget_spinbox::param wp_;
					wp_.text_list_.push_back("0");
					wp_.text_list_.push_back("1");
					wp_.text_list_.push_back("2");
					wp_.text_list_.push_back("3");
					wp_.text_list_.push_back("4");
					wp_.text_list_.push_back("5");
					wp_.text_list_.push_back("6");
					wp_.text_list_.push_back("7");
					wp_.text_list_.push_back("8");
					wp_.text_list_.push_back("9");
					state_slot_ = wd.add_widget<widget_spinbox>(wp, wp_);
				}
				{ // ステート・セーブ
					widget::param wp(vtx::irect(110, 35+40*0, 90, 30), menu_frame_);
					widget_button::param wp_("Save");
					state_save_ = wd.add_widget<widget_button>(wp, wp_);
					state_save_->at_local_param().select_func_ = [this](int id) {
						state_setslot(get_state_no_());
						if(state_save() != 0) {
							dialog_->set_text("Save state error");
							dialog_->enable();
						}
					};

				}
				{ // ステート・ロード
					widget::param wp(vtx::irect(110, 35+40*1, 90, 30), menu_frame_);
					widget_button::param wp_("Load");
					state_load_ = wd.add_widget<widget_button>(wp, wp_);
					state_load_->at_local_param().select_func_ = [this](int id) {
						state_setslot(get_state_no_());
						if(state_load() != 0) {
							dialog_->set_text("Load state error");
							dialog_->enable();
						}
					};
				}
				{ // NES リセット
					widget::param wp(vtx::irect(10, 35+40*2, 90, 30), menu_frame_);
					widget_button::param wp_("Reset");
					nes_reset_ = wd.add_widget<widget_button>(wp, wp_);
					nes_reset_->at_local_param().select_func_ = [this](int id) {
						nes_reset(HARD_RESET);
					};
				}
				{   // ボリューム
					widget::param wp(vtx::irect(10, 35+40*3, 190, 20), menu_frame_);
					widget_slider::param wp_;
					volume_ = wd.add_widget<widget_slider>(wp, wp_);
				}
			}

			{   // Daialog
				widget::param wp(vtx::irect(70, 70, 300, 150));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::param::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
			}

			texfb_.initialize(nes_width_, nes_height_, 32);

			log_init();
			nes_ = nes_create(sample_rate_, 16);

			// regist input
			inp_[0].type = INP_JOYPAD0;
			inp_[0].data = 0;
			input_register(&inp_[0]);
			inp_[1].type = INP_JOYPAD1;
			inp_[1].data = 0;
			input_register(&inp_[1]);

			audio_ = al::create_audio(al::audio_format::PCM16_MONO);
			audio_->create(sample_rate_, audio_queue_);

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
			if(dev.get_positive(gl::device::key::ESCAPE)) {
				nes_pause_ ^= 1;
				// nes_pause(nes_pause_);
			}
			if(dev.get_positive(gl::device::key::SPACE)) {
				menu_ = !menu_;
				menu_frame_->enable(menu_);
			}

        	gui::widget_director& wd = director_.at().widget_director_;

			if(nes_ != nullptr && rom_active_) {
				pad_();

				nes_emulate(1);

				// copy sound
				int16_t tmp[audio_len_];
				apu_process(tmp, audio_len_);
				for(int i = 0; i < audio_len_; ++i) {
					fifo_.put(tmp[i]);
				}

				if(fifo_.length() >= (audio_queue_ * 2) && audio_) {
					al::sound& sound = director_.at().sound_;
					for(int i = 0; i < audio_queue_; ++i) {
						al::pcm16_m w(fifo_.get());
						audio_->put(i, w);
					}
					sound.queue_audio(audio_);
				}

				// copy video
				bitmap_t* v = nes_->vidbuf;
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
					texfb_.rendering(gl::texfb::image::RGBA, (const char*)&fb_[0]);
				}

				// ストリームのゲイン(volume)を設定
				if(volume_ != nullptr) {
					auto vol = volume_->get_slider_param().position_;
					al::sound& sound = director_.at().sound_;
					sound.set_gain_stream(vol);
				}
			}

			texfb_.flip();

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
				scale = static_cast<float>(siz.x) / static_cast<float>(nes_width_);
				float h = static_cast<float>(nes_height_);
				ofsy = (static_cast<float>(siz.y) - h * scale) * 0.5f;
			} else {
				scale = static_cast<float>(siz.y) / static_cast<float>(nes_height_);
				float w = static_cast<float>(nes_width_);
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
			nes_destroy(nes_);

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
