#pragma once
//=====================================================================//
/*! @file
    @brief  CRM Test・アプリケーション・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "main.hpp"
#include "core/glcore.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_utils.hpp"

#include "utils/serial_win32.hpp"

#include "crm.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  CRM テスト・アプリケーション・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class crm_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		menu_;
		gui::widget_list*		ports_;
		gui::widget_button*		connect_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		crm						crm_;

		typedef device::serial_win32 SERIAL;
		SERIAL					serial_;
		SERIAL::name_list		serial_list_;

		// ターミナル、行入力
		void term_enter_(const utils::lstring& text) {
			auto s = utils::utf32_to_utf8(text);
//			project_.logic_edit_.command(s);
///			std::cout << s << std::endl;
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		crm_test(utils::director<core>& d) : director_(d),
			menu_(nullptr),
			ports_(nullptr), connect_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			crm_(d, serial_),
			serial_(), serial_list_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			gl::core& core = gl::core::get_instance();

			int menu_width  = 950;
			int menu_height = 320;
			{	// メニューパレット
				widget::param wp(vtx::irect(10, 10, menu_width, menu_height));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				menu_ = wd.add_widget<widget_frame>(wp, wp_);
				menu_->set_state(gui::widget::state::SIZE_LOCK);
			}

			{ // Serial PORT select
				widget::param wp(vtx::irect(10, 20, 150, 40), menu_);
				widget_list::param wp_;
				ports_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{ // コネクションボタン
				widget::param wp(vtx::irect(10 + 160, 20, 150, 40), menu_);
				widget_button::param wp_("connect");
				connect_ = wd.add_widget<widget_button>(wp, wp_);
				connect_->at_local_param().select_func_ = [=](int id) {
					const auto& port = ports_->get_select_text();
					if(serial_.probe()) {
						ports_->set_stall(false);
						serial_.close();
						terminal_core_->output("Close Serialport: '" + port + "'\n");
						connect_->set_text("connect");
					} else {
						if(!port.empty()) {
							if(serial_.open(port, 115200)) {
								connect_->set_text("close");
								terminal_core_->output("Open Serialport: '" + port + "'\n");
								ports_->set_stall();
							} else {
								terminal_core_->output("Can't open Serialport: '" + port + "'\n");
							}
						}
					}
				};
			}

			crm_.init(menu_, menu_width, 80, 70);

			{	// ターミナル
				{
					widget::param wp(vtx::irect(20, 100, 100, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [=](const utils::lstring& text) {
						term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					term_chaout::set_output(terminal_core_);
				}
			}
			crm_.set_termcore(terminal_core_);

			// プリファレンスのロード
			sys::preference& pre = director_.at().preference_;

			crm_.load(pre);

			if(menu_ != nullptr) {
				menu_->load(pre, false, false);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(ports_ != nullptr) ports_->load(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;

			// シリアルポートの更新
			if(serial_list_.empty() || !serial_.compare(serial_list_)) {
				serial_.create_list();
				serial_list_ = serial_.get_list();
				utils::strings list;
				for(const auto& t : serial_list_) {
					list.push_back(t.port);
					terminal_core_->output(t.port + " (" + t.info + ")\n");
				}
				ports_->get_menu()->build(list);
				ports_->select(0);
			}

			crm_.update();

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

			crm_.save(pre);

			if(ports_ != nullptr) ports_->save(pre);

			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}

			if(menu_ != nullptr) {
				menu_->save(pre);
			}
		}
	};
}
