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
#include "gl_fw/gltexfb.hpp"

extern "C" {
	#include "nes.h"
	extern const rgb_t* get_palette();
};

namespace app {

	class nesemu : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gl::texfb			texfb_;

		static const int nes_width_  = 256;
		static const int nes_height_ = 224;

		nes_t*	nes_;

		uint8_t	fb_[nes_width_ * nes_height_ * 4];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		nesemu(utils::director<core>& d) : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			nes_(nullptr)
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

			texfb_.initialize(nes_width_, nes_height_, 32);

			nes_ = nes_create();

			auto path = core.get_current_path();

			path += "/GALAXIAN.NES";

			nes_insertcart(path.c_str(), nes_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
//	   		gl::core& core = gl::core::get_instance();
//			const gl::device& dev = core.get_device();
        	gui::widget_director& wd = director_.at().widget_director_;

			if(nes_ != nullptr) {
				nes_emulate(1);

				bitmap_t* v = nes_->vidbuf;
				const rgb_t* lut = get_palette();
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
//			nes_destroy(nes_);
		}

	};

}
