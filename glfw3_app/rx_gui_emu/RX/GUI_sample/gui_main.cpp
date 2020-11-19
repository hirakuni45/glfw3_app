//=====================================================================//
/*! @file
    @brief  RX65N/RX72N GUI サンプル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/format.hpp"

#include "graphics/font8x16.hpp"
#include "graphics/kfont.hpp"
#include "graphics/graphics.hpp"
// #include "graphics/filer.hpp"
#include "graphics/simple_dialog.hpp"
// #include "graphics/img_in.hpp"
#include "graphics/widget_director.hpp"
#include "graphics/scaling.hpp"

namespace {

	// LCD 定義
	static const int16_t LCD_X = 480;
	static const int16_t LCD_Y = 272;
//	static const auto PIX = graphics::pixel::TYPE::RGB565;

	// GLCDC 関係リソース
//	typedef device::glcdc_mgr<device::GLCDC, LCD_X, LCD_Y, PIX> GLCDC;

	template <uint32_t LCDX, uint32_t LCDY>
	class glcdc_emu {
	public:
		static const uint32_t	width  = LCDX;
		static const uint32_t	height = LCDY;

	private:
		uint16_t fb_[LCDX * LCDY];

	public:

		void sync_vpos() { }

		void* get_fbp() { return fb_; }
	};
	typedef glcdc_emu<LCD_X, LCD_Y> GLCDC;

	// フォントの定義
	typedef graphics::font8x16 AFONT;
// 	for cash into SD card /kfont16.bin
//	typedef graphics::kfont<16, 16, 64> KFONT;
	typedef graphics::kfont<16, 16> KFONT;
	typedef graphics::font<AFONT, KFONT> FONT;

	// ソフトウェアーレンダラー
	typedef graphics::render<GLCDC, FONT> RENDER;
	// 標準カラーインスタンス
	typedef graphics::def_color DEF_COLOR;

	GLCDC		glcdc_;
	AFONT		afont_;
	KFONT		kfont_;
	FONT		font_(afont_, kfont_);
	RENDER		render_(glcdc_, font_);

//	typedef chip::FT5206<FT5206_I2C> TOUCH;
	class touch_emu {
	public:

		struct touch_t {
			vtx::spos	pos;
		};

	private:
		touch_t	touch_[4];
		uint32_t	num_;

	public:
		touch_emu() : num_(0) { }

		uint32_t get_touch_num() const { return num_; }

		const auto& get_touch_pos(uint32_t idx) const {
			if(idx >= 4) idx = 0;
			return touch_[idx];
		}

		void update()
		{
		}

		void set_pos(const vtx::spos& pos)
		{
			touch_[0].pos = pos;
			num_ = 1;
		}

		void reset() { num_ = 0; }
	};
	typedef touch_emu TOUCH;
	TOUCH		touch_;

	typedef gui::simple_dialog<RENDER, TOUCH> DIALOG;
	DIALOG		dialog_(render_, touch_);

	// 最大３２個の Widget 管理
	typedef gui::widget_director<RENDER, TOUCH, 32> WIDD;
	WIDD		widd_(render_, touch_);

	typedef gui::button BUTTON;
	BUTTON		button_  (vtx::srect(   10, 10+50*0, 80, 32), "Button");
	typedef gui::check CHECK;
	CHECK		check_(vtx::srect(   10, 10+50*1, 0, 0), "Check");  // サイズ０指定で標準サイズ
	typedef gui::group<3> GROUP3;
	GROUP3		group_(vtx::srect(   10, 10+50*2, 0, 0));
	typedef gui::radio RADIO;
	RADIO		radioR_(vtx::srect(   0, 50*0, 0, 0), "Red");
	RADIO		radioG_(vtx::srect(   0, 50*1, 0, 0), "Green");
	RADIO		radioB_(vtx::srect(   0, 50*2, 0, 0), "Blue");
	typedef gui::slider SLIDER;
	SLIDER		sliderh_(vtx::srect(200, 20, 200, 0), 0.5f);
	SLIDER		sliderv_(vtx::srect(440, 20, 0, 200), 0.0f);
	typedef gui::menu MENU;
	MENU		menu_(vtx::srect(120, 70, 100, 0), "ItemA,ItemB,ItemC,ItemD");
}


/// widget の登録・グローバル関数
bool insert_widget(gui::widget* w)
{
    return widd_.insert(w);
}

/// widget の解除・グローバル関数
void remove_widget(gui::widget* w)
{
    widd_.remove(w);
}

namespace gui_emu {

	const void* get_fbp() { return glcdc_.get_fbp(); }

	void set_pos(const vtx::spos& pos)
	{
		touch_.set_pos(pos);
	}

	void setup_gui()
	{
		button_.enable();
		button_.at_select_func() = [=](uint32_t id) {
			utils::format("Select Button: %d\n") % id;
		};

		check_.enable();
		check_.at_select_func() = [=](bool ena) {
			utils::format("Select Check: %s\n") % (ena ? "On" : "Off");
		};

		// グループにラジオボタンを登録
		group_ + radioR_ + radioG_ + radioB_;
		group_.enable();  // グループ登録された物が全て有効になる。
		radioR_.at_select_func() = [=](bool ena) {
			utils::format("Select Red: %s\n") % (ena ? "On" : "Off");
		};
		radioG_.at_select_func() = [=](bool ena) {
			utils::format("Select Green: %s\n") % (ena ? "On" : "Off");
		};
		radioB_.at_select_func() = [=](bool ena) {
			utils::format("Select Blue: %s\n") % (ena ? "On" : "Off");
		};
		radioG_.exec_select();  // 最初に選択されるラジオボタン

		sliderh_.enable();
		sliderh_.at_select_func() = [=](float val) {
			utils::format("Slider H: %3.2f\n") % val;
		};
		sliderv_.enable();
		sliderv_.at_select_func() = [=](float val) {
			utils::format("Slider V: %3.2f\n") % val;
		};

		menu_.enable();
		menu_.at_select_func() = [=](uint32_t pos, uint32_t num) {
			char tmp[32];
			menu_.get_select_text(tmp, sizeof(tmp));
			utils::format("Menu: '%s', %u/%u\n") % tmp % pos % num;
		};
	}


	void update_gui()
	{
		render_.sync_frame();
		touch_.update();
		widd_.update();

		touch_.reset();
	}
}
