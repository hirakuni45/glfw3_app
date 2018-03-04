#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・アプリケーション・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
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
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_utils.hpp"

#include "interlock.hpp"
#include "root_menu.hpp"
#include "ign_client_tcp.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor アプリケーション・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ignitor : public utils::i_scene {

		utils::director<core>&	director_;

		interlock				interlock_;
		root_menu				root_menu_;

///		asio::io_service		io_service_;
		net::ign_client_tcp		client_;
		uint32_t				delay_client_;
		bool					connect_client_;
		bool					start_client_;

		std::string				ip_;

		uint32_t				exit_loop_;
		bool					root_menu_exit_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ignitor(utils::director<core>& d) : director_(d),
			interlock_(),
			root_menu_(d, client_, interlock_),
///			io_service_(),
///			client_(io_service_),
			client_(),
			delay_client_(60), connect_client_(false), start_client_(false),
			ip_(), exit_loop_(60), root_menu_exit_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			root_menu_.initialize();

			director_.at().exit_func_ = [=]() {
				if(!root_menu_exit_) {
					root_menu_.exit();
					root_menu_exit_ = true;
				}
//				std::cout << "Recive EXIT" << std::endl;
				if(exit_loop_ > 0) {
					--exit_loop_;
				}
				return exit_loop_ == 0;
			};
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			root_menu_.update();

			gui::widget_director& wd = director_.at().widget_director_;

			if(start_client_) {
				if(client_.probe()) {
					client_.service();
				}
			} else {
				bool cn = root_menu_.get_target_connect();
				if(!connect_client_ && cn) {  // 接続が有効になった
					connect_client_ = cn;
					delay_client_ = 60;
				} 
				if(delay_client_ > 0) {
					--delay_client_;
					if(delay_client_ == 0) {
						auto ip = root_menu_.get_target_ip();
						if(!ip.empty() && ip_ != ip) {
							ip_ = ip;
							client_.start(ip_, 23);
							start_client_ = true;
						}
					}
				}
			}

///			io_service_.run();

#if 0
#if 0
			// Drag & Drop されたファイル
			gl::core& core = gl::core::get_instance();
			int id = core.get_recv_files_id();
			if(drop_file_id_ != id) {
				drop_file_id_ = id;
				const utils::strings& ss = core.get_recv_files_path();
				if(!ss.empty()) {
					std::string path = ss[0];
					if(load_ctx_ != nullptr && load_ctx_->get_local_param().select_file_func_ != nullptr) {
						load_ctx_->get_local_param().select_file_func_(path);
					}
				}
			}
#endif
#endif
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
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() { }
	};
}
