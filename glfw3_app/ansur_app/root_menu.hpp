#pragma once
//=====================================================================//
/*! @file
    @brief  ルート・メニュー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_label.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"

#include "ftpsub.h"
#include "ansur_tools.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ルート・メニュー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class root_menu {

		utils::director<core>&	director_;

		gui::widget_frame*		frame_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_button*		cont_run_;

		gui::widget_button*		list_;

		gui::widget_dialog*		cont_setting_dialog_;
		gui::widget_check*		cont_connect_;
		gui::widget_label*		cont_setting_ip_[4];

		gui::widget_dialog*		info_dialog_;

		gui::widget_dialog*		msg_dialog_;

		int						ip_[4];

		ftpsub_buf				ftpsub_;
		int						sockfd_;

		bool	connect_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		root_menu(utils::director<core>& d) :
			director_(d),
			frame_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			cont_run_(nullptr),
			cont_setting_dialog_(nullptr),
		   	cont_connect_(nullptr), cont_setting_ip_{ nullptr },

			ip_{ 0 },

			sockfd_(0),

			connect_(false)

			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~root_menu()
		{
			WSACleanup();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			auto& core = gl::core::get_instance();
//			const auto& scs = core.get_rect().size;

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int fw = 150;
			int fh = 400;
			{  // ツール・フレーム
				widget::param wp(vtx::irect(0, 0, fw, fw));
				widget_frame::param wp_;
				frame_ = wd.add_widget<widget_frame>(wp, wp_);				
			}
			{  // コントロール設定
				widget::param wp(vtx::irect(20, 20, 100, 40), frame_);
				widget_button::param wp_("設定");
				cont_run_ = wd.add_widget<widget_button>(wp, wp_);
				cont_run_->at_local_param().select_func_ = [=](uint32_t id) {
					cont_setting_dialog_->enable();
				};			
			}
			{  // コントロール設定
				widget::param wp(vtx::irect(20, 70, 100, 40), frame_);
				widget_button::param wp_("設定");
				list_ = wd.add_widget<widget_button>(wp, wp_);
				list_->at_local_param().select_func_ = [=](uint32_t id) {
				};			
			}


			{	// ターミナル
				{
					widget::param wp(vtx::irect(50, 50, 400, 300));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [=](const utils::lstring& text) {
						// term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					term_chaout::set_output(terminal_core_);
				}
			}


			{  // コントローラー設定ダイアログ
				int w = 520;
				int h = 290;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_(widget_dialog::style::OK);
				cont_setting_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				cont_setting_dialog_->enable(false);
				cont_setting_dialog_->at_local_param().select_func_ = [=](bool ok) {
					for(int i = 0; i < 4; ++i) {
						const std::string& ip = cont_setting_ip_[i]->get_text();
						int v = 0;
						if((utils::input("%d", ip.c_str()) % v).status()) {
							ip_[i] = v;
						}
					}
				};
				widget_dialog* root = cont_setting_dialog_;
				{
					widget::param wp(vtx::irect(10, 20, w - 10 * 2, 40), root);
					widget_text::param wp_("コントローラーＩＰ：");
					wd.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(20, 50, 100, 40), root);
					widget_check::param wp_("接続");
					cont_connect_ = wd.add_widget<widget_check>(wp, wp_);
				}
				int ipw = 60;  // IP 設定幅
				int ips = 20;  // IP 設定隙間
				{
					widget::param wp(vtx::irect(110 + (ipw + ips) * 0, 50, 60, 40), root);
					widget_label::param wp_("192", false);
					cont_setting_ip_[0] = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(110 + (ipw + ips) * 1, 50, 60, 40), root);
					widget_label::param wp_("168", false);
					cont_setting_ip_[1] = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(110 + (ipw + ips) * 2, 50, 60, 40), root);
					widget_label::param wp_("0", false);
					cont_setting_ip_[2] = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(110 + (ipw + ips) * 3, 50, 60, 40), root);
					widget_label::param wp_("11", false);
					cont_setting_ip_[3] = wd.add_widget<widget_label>(wp, wp_);
				}
			}

			{  // 情報ダイアログ
				int w = 750;
				int h = 210;
				widget::param wp(vtx::irect(50, 50, w, h));
				widget_dialog::param wp_(widget_dialog::style::OK);
				info_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
#if 0
				std::string s =	"イグナイター検査\n";
				uint32_t bid = B_ID;
				s += (boost::format("Build: %d\n") % bid).str();
				s += (boost::format("Version %d.%02d\n")
					% (app_version_ / 100) % (app_version_ % 100)).str();
				s += "Copyright (C) 2018 Hitachi Automotive Systems Hanshin,Ltd.\n";
				s += "All Rights Reserved.";
				info_dialog_->set_text(s);
#endif
				info_dialog_->enable(false);
			}

			{  // メッセージ・ダイアログ（ボタン無し）
				int w = 650;
				int h = 410;
				widget::param wp(vtx::irect(75, 75, w, h));
				widget_dialog::param wp_(widget_dialog::style::NONE);
				msg_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				msg_dialog_->enable(false);
			}

			::ftpsub_buf_init(&ftpsub_);
			::ftpiconvopen(&ftpsub_);

			{
				WSADATA wsad;
				int res = WSAStartup(MAKEWORD(2,0), &wsad);
				if(res != NO_ERROR) {
					std::cout << "WSAStartup function failed with error:" << std::endl;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			bool connect = cont_connect_->get_check();
			if(!connect_ && connect) {
				std::string host;
				for(int i = 0; i < 4; ++i) {
					host += cont_setting_ip_[i]->get_text();
					if(i < 3) { host += '.'; }
				}
				sockfd_ = CmdLOGIN(&ftpsub_, host.c_str(), 21, "SEEDA03", "SEEDA03");
				terminal_core_->output("Login: SEEDA03\n");
			}
			if(connect_ && !connect) {
				CmdQUIT(&ftpsub_, sockfd_);
				CmdCLOSE(&ftpsub_, &sockfd_);
				terminal_core_->output("Logout\n");
			}
			connect_ = connect;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プリファレンスのロード
		*/
		//-----------------------------------------------------------------//
		void load()
		{
			sys::preference& pre = director_.at().preference_;

			frame_->load(pre);

			info_dialog_->load(pre);
			msg_dialog_->load(pre);

			cont_setting_dialog_->load(pre);
///			cont_connect_->load(pre);
			cont_setting_ip_[0]->load(pre);
			cont_setting_ip_[1]->load(pre);
			cont_setting_ip_[2]->load(pre);
			cont_setting_ip_[3]->load(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
		*/
		//-----------------------------------------------------------------//
		void save()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			sys::preference& pre = director_.at().preference_;

			frame_->save(pre);

			info_dialog_->save(pre);
			msg_dialog_->save(pre);

			cont_setting_dialog_->save(pre);
///			cont_connect_->save(pre);
			cont_setting_ip_[0]->save(pre);
			cont_setting_ip_[1]->save(pre);
			cont_setting_ip_[2]->save(pre);
			cont_setting_ip_[3]->save(pre);
		}
	};
}
