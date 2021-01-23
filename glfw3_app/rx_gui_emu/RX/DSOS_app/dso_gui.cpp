//=====================================================================//
/*! @file
    @brief  DSOS simulator core
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>
#include <limits>

#define GLFW_SIM
#include "dso_gui.hpp"

#include "graphics/font8x16.hpp"
#include "graphics/kfont.hpp"
#include "graphics/graphics.hpp"
#include "graphics/simple_dialog.hpp"
#include "graphics/widget_director.hpp"
#include "graphics/scaling.hpp"

#include "capture.hpp"

#include "touch_sim.hpp"

namespace {

	static const int16_t LCD_X = 480;
	static const int16_t LCD_Y = 272;

	template <uint32_t LCDX, uint32_t LCDY>
	class glcdc_sim {
	public:
		static const uint32_t	width  = LCDX;
		static const uint32_t	height = LCDY;
		static const uint32_t	line_width = width;

	private:
		uint16_t fb_[LCDX * LCDY];

	public:
		void sync_vpos() { }

		void* get_fbp() { return fb_; }
	};
	typedef glcdc_sim<LCD_X, LCD_Y> GLCDC;

	typedef graphics::font8x16 AFONT;
	typedef graphics::kfont<16, 16> KFONT;
	typedef graphics::font<AFONT, KFONT> FONT;

	// ソフトウェアーレンダラー
	typedef graphics::render<GLCDC, FONT> RENDER;
	// 標準カラーインスタンス
	typedef graphics::def_color DEF_COLOR;

	GLCDC	glcdc_;
	AFONT	afont_;
	KFONT	kfont_;
	FONT	font_(afont_, kfont_);
	RENDER	render_(glcdc_, font_);

	typedef chip::touch_sim TOUCH;
	TOUCH	touch_;

	typedef dsos::capture<8192> CAPTURE;
	CAPTURE	capture_;

	typedef dsos::dso_gui<RENDER, TOUCH, CAPTURE> DSO_GUI;
	DSO_GUI	dso_gui_(render_, touch_, capture_);
}

namespace gui_sim {

    const void* get_fbp()
	{
		return glcdc_.get_fbp();
	}


    void set_pos(const vtx::spos& pos, bool touch)
	{
		touch_.set_pos(pos, touch);		
	}


    void setup_gui()
	{
		touch_.start();

		dso_gui_.start();
	}


    void update_gui()
	{
		touch_.update();

		dso_gui_.update();
	}


	void injection_capture(uint32_t freq, float pp)
	{
		static int32_t count = 0;
		auto smpl = capture_.get_samplerate();
		auto& task = capture_.at_cap_task();
		auto unit = static_cast<float>(smpl) / static_cast<float>(freq);
		float vgain0 = pp / capture_.get_voltage_gain(0) * 2048.0f;
		float vgain1 = pp / capture_.get_voltage_gain(1) * 2048.0f;
		for(uint32_t i = 0; i < 512; ++i) {
			auto a = static_cast<float>(count % static_cast<int32_t>(unit)) / unit;
			task.adv_.x = static_cast<int16_t>(sinf(a * vtx::radian_f_) * vgain0);
			task.adv_.y = static_cast<int16_t>(cosf(a * vtx::radian_f_) * vgain1);
			if(task.adv_.x < -2048) task.adv_.x = -2048;
			else if(task.adv_.x > 2047) task.adv_.x = 2047;
			if(task.adv_.y < -2048) task.adv_.y = -2048;
			else if(task.adv_.y > 2047) task.adv_.y = 2047;
			task();
			++count;
			if(count >= CAPTURE::CAP_NUM) {
				count = 0;
			}
		}
	}
}

/// widget の登録・グローバル関数
bool insert_widget(gui::widget* w)
{
    return dso_gui_.at_widd().insert(w);
}

/// widget の解除・グローバル関数
void remove_widget(gui::widget* w)
{
    dso_gui_.at_widd().remove(w);
}
