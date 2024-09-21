//=====================================================================//
/*! @file
	@brief  calc main クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2024 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_fb.hpp"
#include "widgets/widget_terminal.hpp"
#include "utils/format.hpp"
#include <string>

#define EMU

#ifdef EMU
#include "calc_cmd.hpp"

namespace gui_sim {

	const void* get_fbp();
	void set_pos(const vtx::spos& pos, bool touch);
	void start_gui();
	void setup_gui();
	void update_gui();
	app::calc_cmd& at_calc_cmd();

}
#endif

static inline std::function<void (const char* str)> output_func_;

void sci_puts(const char* str)
{
	if(output_func_ != nullptr) {
		if(str != nullptr) {
			output_func_(str);
		}
	}
}


namespace app {

	class calc_main : public utils::i_scene {

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
		calc_main(utils::director<core>& d) :
			director_(d),
			lcd_frame_(nullptr), lcd_core_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~calc_main() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			auto& core = gl::core::get_instance();

			using namespace gui;
			auto& wd = director_.at().widget_director_;

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

			{	// ターミナルの作成
				{
					widget::param wp(vtx::irect(10, 10, 400, 300));
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
			auto& pre = director_.at().preference_;
			if(lcd_frame_ != nullptr) {
				lcd_frame_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}

			if(terminal_core_ != nullptr) {
				auto& t = terminal_core_->at_terminal();
				t.enable_crlf();
			}

			output_func_ = [=](const char* str) {
				if(terminal_core_ != nullptr) {
					terminal_core_->at_terminal().output(str);
				}
			};
#ifdef EMU
			gui_sim::start_gui();
			gui_sim::setup_gui();
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			auto& wd = director_.at().widget_director_;

#ifdef EMU
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
#endif

			if(terminal_core_ != nullptr) {
				auto& t = terminal_core_->at_terminal();

				auto ch = t.get_last_char();
				if(ch == 0x0d) {
					t.output('\n');

					auto s = t.get_last_text();
					if(!s.empty() && s[0] >= 0x20 && s[0] < 0x7f) {
						gui_sim::at_calc_cmd().set_out(sci_puts);
						gui_sim::at_calc_cmd()(s.c_str());
					}
				}
			}

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


		auto get_terminal_core() noexcept
		{
			return terminal_core_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			auto& pre = director_.at().preference_;

			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(lcd_frame_ != nullptr) {
				lcd_frame_->save(pre);
			}
		}
	};
}
