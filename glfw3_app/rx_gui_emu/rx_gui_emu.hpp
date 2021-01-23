#pragma once
//=====================================================================//
/*! @file
	@brief  RX GUI Emulation クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_fb.hpp"

#include "utils/format.hpp"

#include "utils/sjis_utf16.hpp"

namespace gui_sim {

	const void* get_fbp();
	void set_pos(const vtx::spos& pos, bool touch);
	void setup_gui();
	void update_gui();
	void injection_capture(uint32_t freq, float ppvolt);

}

namespace {

	std::string rx_stdout_;

}


void rx_putchar(char ch)
{
	rx_stdout_ += ch;
}


extern "C" {

	WCHAR ff_uni2oem (DWORD uni, WORD cp)
	{
		return utils::utf16_to_sjis(uni);
	}

}


namespace app {

	class rx_gui_emu : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		lcd_frame_;
		gui::widget_fb*			lcd_core_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		rx_gui_emu(utils::director<core>& d) : director_(d),
			lcd_frame_(nullptr), lcd_core_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~rx_gui_emu() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
//			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{  // LCD
				{
					widget::param wp(vtx::irect(50, 50, 480, 272));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					lcd_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), lcd_frame_);
					widget_fb::param wp_;
					lcd_core_ = wd.add_widget<widget_fb>(wp, wp_);
				}
				lcd_core_->at_fb().initialize(480, 272, 32);
			}

			{  // ターミナル
				{
					widget::param wp(vtx::irect(10, 10, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
				}
			}

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;

			if(lcd_frame_ != nullptr) {
				lcd_frame_->load(pre);
			}

			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}

			gui_sim::setup_gui();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			// 疑似的に波形を生成
			int32_t freq = 10'000;
			float ppvolt = 3.0f;
			gui_sim::injection_capture(freq, ppvolt);

			// マウスの操作を、ファーストタッチに似せる～
			if(lcd_core_->get_local_param().ms_positive_ || lcd_core_->get_local_param().ms_level_) {
				const auto& p = lcd_core_->get_local_param().ms_pos_;
				gui_sim::set_pos(vtx::spos(p.x, p.y), true);
			}
			if(lcd_core_->get_local_param().ms_negative_) {
				const auto& p = lcd_core_->get_local_param().ms_pos_;
				gui_sim::set_pos(vtx::spos(p.x, p.y), false);
			}

			gui_sim::update_gui();

			{
				const void* p = gui_sim::get_fbp();
				if(p != nullptr) {
					lcd_core_->at_fb().rendering(gl::texfb::IMAGE::RGB565, p);
					lcd_core_->at_fb().flip();
				}
			}

			terminal_core_->output(rx_stdout_);
			rx_stdout_.clear();

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
			sys::preference& pre = director_.at().preference_;

			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}

			if(lcd_frame_) {
				lcd_frame_->save(pre);
			}
		}
	};

}
