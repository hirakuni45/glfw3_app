//=====================================================================//
/*! @file
    @brief  DSOS simulator
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#define GLFW_SIM
#include "dso_gui.hpp"

#include "graphics/font8x16.hpp"
#include "graphics/kfont.hpp"
#include "graphics/graphics.hpp"
#include "graphics/simple_dialog.hpp"
#include "graphics/widget_director.hpp"
#include "graphics/scaling.hpp"

#include "touch_sim.hpp"

namespace {

	static const int16_t LCD_X = 480;
	static const int16_t LCD_Y = 272;

	template <uint32_t LCDX, uint32_t LCDY>
	class glcdc_emu {
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
	typedef glcdc_emu<LCD_X, LCD_Y> GLCDC;

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

	typedef utils::capture<1024> CAPTURE;
	CAPTURE	capture_;

	typedef utils::dso_gui<RENDER, TOUCH, CAPTURE> DSO_GUI;
	DSO_GUI	dso_gui_(render_, touch_, capture_);
}

namespace gui_emu {

    const void* get_fbp()
	{
		return glcdc_.get_fbp();
	}


    void set_pos(const vtx::spos& pos)
	{
		touch_.set_pos(pos);		
	}


    void setup_gui()
	{
		dso_gui_.start();
	}


    void update_gui()
	{
		dso_gui_.update();
		touch_.reset();
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
