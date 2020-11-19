//=====================================================================//
/*! @file
    @brief  RX65N/RX72N 計算機サンプル
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
#include "graphics/simple_dialog.hpp"
#include "graphics/widget_director.hpp"
#include "graphics/scaling.hpp"

#include "common/basic_arith.hpp"
#include "common/fixed_string.hpp"

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
	typedef gui::widget_director<RENDER, TOUCH, 40> WIDD;
	WIDD		widd_(render_, touch_);

	static const int16_t BTN_W = 38;
	static const int16_t BTN_H = 38;
	static const int16_t ORG_X = 10;
	static const int16_t ORG_Y = 94;
	static const int16_t SPC_X = 44;
	static const int16_t SPC_Y = 44;
	constexpr int16_t LOC_X(int16_t x)
	{
		return ORG_X + SPC_X * x;
	}
	constexpr int16_t LOC_Y(int16_t y)
	{
		return ORG_Y + SPC_Y * y;
	}
	typedef gui::button BUTTON;
	BUTTON		no0_  (vtx::srect(LOC_X(0), LOC_Y(3), BTN_W, BTN_H), "０");
	BUTTON		no1_  (vtx::srect(LOC_X(0), LOC_Y(2), BTN_W, BTN_H), "１");
	BUTTON		no2_  (vtx::srect(LOC_X(1), LOC_Y(2), BTN_W, BTN_H), "２");
	BUTTON		no3_  (vtx::srect(LOC_X(2), LOC_Y(2), BTN_W, BTN_H), "３");
	BUTTON		no4_  (vtx::srect(LOC_X(0), LOC_Y(1), BTN_W, BTN_H), "４");
	BUTTON		no5_  (vtx::srect(LOC_X(1), LOC_Y(1), BTN_W, BTN_H), "５");
	BUTTON		no6_  (vtx::srect(LOC_X(2), LOC_Y(1), BTN_W, BTN_H), "６");
	BUTTON		no7_  (vtx::srect(LOC_X(0), LOC_Y(0), BTN_W, BTN_H), "７");
	BUTTON		no8_  (vtx::srect(LOC_X(1), LOC_Y(0), BTN_W, BTN_H), "８");
	BUTTON		no9_  (vtx::srect(LOC_X(2), LOC_Y(0), BTN_W, BTN_H), "９");

	BUTTON		del_  (vtx::srect(LOC_X(3), LOC_Y(0), BTN_W, BTN_H), "DEL");
	BUTTON		ac_   (vtx::srect(LOC_X(4), LOC_Y(0), BTN_W, BTN_H), "AC");

	BUTTON		mul_  (vtx::srect(LOC_X(3), LOC_Y(1), BTN_W, BTN_H), "×");
	BUTTON		div_  (vtx::srect(LOC_X(4), LOC_Y(1), BTN_W, BTN_H), "÷");
	BUTTON		add_  (vtx::srect(LOC_X(3), LOC_Y(2), BTN_W, BTN_H), "＋");
	BUTTON		sub_  (vtx::srect(LOC_X(4), LOC_Y(2), BTN_W, BTN_H), "－");

	BUTTON		poi_  (vtx::srect(LOC_X(1), LOC_Y(3), BTN_W, BTN_H), "・");
	BUTTON		pin_  (vtx::srect(LOC_X(2), LOC_Y(3), BTN_W, BTN_H), "（");
	BUTTON		pot_  (vtx::srect(LOC_X(3), LOC_Y(3), BTN_W, BTN_H), "）");
	BUTTON		equ_  (vtx::srect(LOC_X(4), LOC_Y(3), BTN_W, BTN_H), "＝");

	typedef utils::basic_arith<float> ARITH;
	ARITH		arith_;

	typedef utils::fixed_string<128> STR;
	STR			cbuff_;

	static const int16_t limit_ = 3;
	vtx::spos	cur_pos_;

	void clear_win_()
	{
		cbuff_.clear();
		cur_pos_.set(0);
		render_.set_fore_color(DEF_COLOR::Darkgray);
		render_.round_box(vtx::srect(0, 0, 480, 16 * 5 + 6), 8);
	}

	typedef utils::fixed_string<512> OUTSTR;


	void conv_cha_(char ch, OUTSTR& out)
	{
		switch(ch) {
		case '0': out += "０"; break;
		case '1': out += "１"; break;
		case '2': out += "２"; break;
		case '3': out += "３"; break;
		case '4': out += "４"; break;
		case '5': out += "５"; break;
		case '6': out += "６"; break;
		case '7': out += "７"; break;
		case '8': out += "８"; break;
		case '9': out += "９"; break;
		case '+': out += "＋"; break;
		case '-': out += "－"; break;
		case '/': out += "÷"; break;
		case '*': out += "×"; break;
		default:
			out += ch; break;
		}
	}


	OUTSTR conv_str_(const char* str)
	{
		OUTSTR out;
		char ch;
		while((ch = *str++) != 0) {
			conv_cha_(ch, out);
		}
		return out;
	}


	void update_calc_()
	{
		if(cur_pos_.x != cbuff_.size()) {
			if(cur_pos_.x > cbuff_.size()) {
				render_.set_fore_color(DEF_COLOR::Darkgray);
				auto x = cur_pos_.x;
				if(x > 0) --x;
				render_.fill_box(vtx::srect(6 + x * 16, 6 + cur_pos_.y * 20, 16, 16));
			} else {
				OUTSTR tmp;
				conv_cha_(cbuff_.back(), tmp);
				render_.set_fore_color(DEF_COLOR::White);
				render_.draw_text(vtx::spos(6 + cur_pos_.x * 16, 6 + cur_pos_.y * 20),
					tmp.c_str());
			}
			cur_pos_.x = cbuff_.size();
		}
	}


	void update_ans_()
	{
		arith_.analize(cbuff_.c_str());
		auto ans = arith_.get();
		char tmp[20];
		utils::sformat("%7.6f\n", tmp, sizeof(tmp)) % ans;
		auto out = conv_str_(tmp);
		render_.set_back_color(DEF_COLOR::Darkgray);
		render_.set_fore_color(DEF_COLOR::White);
		render_.draw_text(vtx::spos(6, 6 + 20 * 3), out.c_str(), false, true);
		cbuff_.clear();
		cur_pos_.y++;
		if(cur_pos_.y >= limit_) {
			render_.move(vtx::srect(6, 6 + 20, 480 - 12, 20 * 2), vtx::spos(6, 6));
			render_.set_fore_color(DEF_COLOR::Darkgray);
			render_.fill_box(vtx::srect(6, 6 + 20 * 2, 480 - 12, 20));
			cur_pos_.y = limit_ - 1;
		}
	}
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
		no0_.enable();
		no0_.at_select_func() = [=](uint32_t id) { cbuff_ += '0'; };
		no1_.enable();
		no1_.at_select_func() = [=](uint32_t id) { cbuff_ += '1'; };
		no2_.enable();
		no2_.at_select_func() = [=](uint32_t id) { cbuff_ += '2'; };
		no3_.enable();
		no3_.at_select_func() = [=](uint32_t id) { cbuff_ += '3'; };
		no4_.enable();
		no4_.at_select_func() = [=](uint32_t id) { cbuff_ += '4'; };
		no5_.enable();
		no5_.at_select_func() = [=](uint32_t id) { cbuff_ += '5'; };
		no6_.enable();
		no6_.at_select_func() = [=](uint32_t id) { cbuff_ += '6'; };
		no7_.enable();
		no7_.at_select_func() = [=](uint32_t id) { cbuff_ += '7'; };
		no8_.enable();
		no8_.at_select_func() = [=](uint32_t id) { cbuff_ += '8'; };
		no9_.enable();
		no9_.at_select_func() = [=](uint32_t id) { cbuff_ += '9'; };

		del_.enable();
		del_.at_select_func() = [=](uint32_t id) { cbuff_.pop_back(); };
		ac_.enable();
		ac_.at_select_func() = [=](uint32_t id) { clear_win_(); };

		mul_.enable();
		mul_.at_select_func() = [=](uint32_t id) { cbuff_ += '*'; };
		div_.enable();
		div_.at_select_func() = [=](uint32_t id) { cbuff_ += '/'; };
		add_.enable();
		add_.at_select_func() = [=](uint32_t id) { cbuff_ += '+'; };
		sub_.enable();
		sub_.at_select_func() = [=](uint32_t id) { cbuff_ += '-'; };

		poi_.enable();
		poi_.at_select_func() = [=](uint32_t id) { cbuff_ += '.'; };
		pin_.enable();
		pin_.at_select_func() = [=](uint32_t id) { cbuff_ += '('; };
		pot_.enable();
		pot_.at_select_func() = [=](uint32_t id) { cbuff_ += ')'; };

		equ_.enable();
		equ_.at_select_func() = [=](uint32_t id) { update_ans_(); };

		clear_win_();
	}


	void update_gui()
	{
		render_.sync_frame();
		touch_.update();
		widd_.update();

		touch_.reset();

		update_calc_();
	}
}
