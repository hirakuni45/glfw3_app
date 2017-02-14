#pragma once
//=====================================================================//
/*! @file
	@brief  NES Emulator クラス
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
#include "gl_fw/gltexfb.hpp"
#include "snd_io/pcm.hpp"

extern "C" {
	#include "nes.h"
	#include "nesinput.h"
	extern const rgb_t* get_palette();
};

namespace app {

	class nesemu : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_filer*		filer_;

		gl::texfb			texfb_;

		static const int nes_width_  = 256;
		static const int nes_height_ = 240;
		static const int audio_len_ = 44100 / 60 * 2;

		nes_t*	nes_;

		uint8_t		sound_[audio_len_];
		al::audio	audio_;
		uint32_t	frame_;

		uint8_t	fb_[nes_width_ * nes_height_ * 4];

		nesinput_t	inp_[2];

		bool	rom_active_;

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

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		nesemu(utils::director<core>& d) : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			nes_(nullptr), frame_(0), rom_active_(false)
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
			widget::param wp(vtx::irect(10, 10, 200, 200));
			widget_filer::param wp_(core.get_current_path(), "", true);
			wp_.select_file_func_ = [this](const std::string& fn) {
				nes_insertcart(fn.c_str(), nes_);
				rom_active_ = true;
			};
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);

			texfb_.initialize(nes_width_, nes_height_, 32);

			nes_ = nes_create();

			// regist input
			inp_[0].type = INP_JOYPAD0;
			inp_[0].data = 0;
			input_register(&inp_[0]);
			inp_[1].type = INP_JOYPAD1;
			inp_[1].data = 0;
			input_register(&inp_[1]);

			audio_ = al::create_audio(al::audio_format::PCM8_MONO);
			audio_->create(44100, audio_len_);

//			auto path = core.get_current_path();

//			path += "/GALAXIAN.NES";
//			path += "/Zombie.nes";
//			path += "/DRAGONQ1.NES";
//			path += "/DRAGONW2.NES";
//			path += "/SOLSTICE.NES";
//			path += "/GRADIUS.NES";

//			nes_insertcart(path.c_str(), nes_);

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
		void update()
		{
	   		gl::core& core = gl::core::get_instance();
			const gl::device& dev = core.get_device();

			if(dev.get_positive(gl::device::key::ENTER)) {
				filer_->enable(!filer_->get_enable());
			}

        	gui::widget_director& wd = director_.at().widget_director_;

			if(nes_ != nullptr && rom_active_) {
				pad_();

				nes_emulate(1);

				// copy sound
				if(frame_ & 1) {
					apu_process(sound_, audio_len_);
					al::sound& sound = director_.at().sound_;
					for(int i = 0; i < audio_len_; ++i) {
						al::pcm8_m w(sound_[i]);
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

			}

			++frame_;

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
			if(filer_) {
				filer_->save(pre);
			}
		}

	};

}
