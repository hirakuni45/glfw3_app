//=====================================================================//
/*! @file
	@brief  BASIC GUI クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "core/glcore.hpp"
#include "basic_gui.hpp"

static std::function< void(char) > output_ch_;
static std::function< void(const char*) > output_puts_;

extern "C" {

	void sci_putch(char ch)
	{
		output_ch_(ch);
	}

	void sci_puts(const char* str)
	{
		output_puts_(str);
	}

};


namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void basic_gui::initialize()
	{
		auto& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{
			widget::param wp(vtx::irect(10, 10, 300, 300));
			widget_frame::param wp_;
			wp_.plate_param_.set_caption(30);
			terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{
			widget::param wp(vtx::irect(0), terminal_frame_);
			widget_terminal::param wp_;
			terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
		}

		// プリファレンスの取得
		auto& pre = director_.at().preference_;
//		if(filer_) {
//			filer_->load(pre);
//		}
		if(terminal_frame_) {
			terminal_frame_->load(pre);
		}

		output_ch_ = [this](char ch) {
			if(terminal_core_ != nullptr) {
				terminal_core_->at_terminal().output(static_cast<uint32_t>(ch));
			}
		};

		output_puts_ = [this](const char* str) {
			if(terminal_core_ != nullptr) {
				terminal_core_->at_terminal().output(str);
			}
		};
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void basic_gui::update()
	{
		auto& wd = director_.at().widget_director_;

#if 0
		if(filer_open_) {
			if(filer_open_->get_selected()) {
				if(filer_) {
					filer_->enable(!filer_->get_state(gui::widget::state::ENABLE));
				}
			}
		}
		if(filer_) {
			if(filer_id_ != filer_->get_select_file_id()) {
				filer_id_ = filer_->get_select_file_id();
				std::cout << "Filer: '" << filer_->get_file() << "'" << std::endl;
				std::cout << std::flush;
			}
		}
#endif

		if(terminal_core_ != nullptr) {
			const auto ch = terminal_core_->at_terminal().get_last_char();
			if(ch != last_ && ch == '\r') {
				auto t = terminal_core_->at_terminal().get_last_text();

				basic_.service(t.c_str());

#if 0
				utils::basic_arith<uint32_t> arith;
				/// std::cout << t << std::endl;
				if(!arith.analize(t.c_str())) {
					std::cout << "Error: " << std::endl;
				}
				terminal_core_->at_terminal().output(
					(boost::format("%d\n") % arith()).str()
				);
#endif
			}
			last_ = ch;
		}		

		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void basic_gui::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void basic_gui::destroy()
	{
		auto& pre = director_.at().preference_;
#if 0
		if(filer_) {
			filer_->save(pre);
		}
#endif
		if(terminal_frame_) {
			terminal_frame_->save(pre);
		}
	}
}
