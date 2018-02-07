#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・アプリケーション・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
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

#include "root_menu.hpp"
#include "ign_client.hpp"
#include "ign_server.hpp"
#include "wave_cap.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor アプリケーション・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ignitor : public utils::i_scene {

		utils::director<core>&	director_;

		root_menu				root_menu_;

		wave_cap				wave_cap_;

		gui::widget_button*		load_;
		gui::widget_list*		div_;

		gui::widget_filer*		load_ctx_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		asio::io_service		io_service_;
		net::ign_client			client_;
		uint32_t				delay_client_;
		bool					connect_client_;
		bool					start_client_;

///		net::ign_server			server_;

		std::string				ip_;

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
		ignitor(utils::director<core>& d) : director_(d),
			root_menu_(d, client_),
			wave_cap_(d),
			load_(nullptr), div_(nullptr), load_ctx_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			io_service_(),
			client_(io_service_),
			delay_client_(60), connect_client_(false), start_client_(false),
///			server_(io_service_),
			ip_()
		{
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			root_menu_.initialize();

			wave_cap_.initialize();

			using namespace gui;
//			widget_director& wd = director_.at().widget_director_;
//			gl::core& core = gl::core::get_instance();
#if 0
			{ // ロード起動ボタン
				widget::param wp(vtx::irect(10, 20 + 40 * 0, menu_width - 20, 30), menu_);
				widget_button::param wp_("load");
				wp_.select_func_ = [this](int id) {
//					gui::get_open_file_name();
					if(load_ctx_) {
//						script_on_ = false;
						bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
						load_ctx_->enable(!f);
//						save_->set_stall(!f);
//						export_->set_stall(!f);
//						script_->set_stall(!f);
					}
				};
				load_ = wd.add_widget<widget_button>(wp, wp_);
			}

			{ // load ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				wp_.select_file_func_ = [this](const std::string& path) {
#if 0
					bool f = false;
					if(script_on_) {
						f = project_.logic_edit_.injection(path);
					} else {
						f = project_.logic_.load(path);
					}
					load_->set_stall(false);
					save_->set_stall(false);
					export_->set_stall(false);
					script_->set_stall(false);
					if(!f) {  // load error
						if(script_on_) dialog_->set_text("Script error:\n" + path);
						else dialog_->set_text("Load error:\n" + path);
						dialog_->enable();
					}
#endif
				};
				load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				load_ctx_->enable(false);
			}
#endif

#if 0
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
					wp_.enter_func_ = [this](const utils::lstring& text) {
						term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					term_chaout::set_output(terminal_core_);
				}

				// ロジック編集クラスの出力先の設定
//				project_.logic_edit_.set_output([this](const std::string& s) {
//					terminal_core_->output(s);
//				}
//				);
			}
#endif

			wave_cap_.load();

			// プリファレンスのロード
//			sys::preference& pre = director_.at().preference_;

//			if(menu_ != nullptr) {
//				menu_->load(pre, false, false);
//			}

///			project_.load(pre);

//			if(terminal_frame_ != nullptr) {
//				terminal_frame_->load(pre);
//			}

//			if(argv_frame_ != nullptr) {
//				argv_frame_->load(pre);
//			}

//			if(load_ctx_ != nullptr) load_ctx_->load(pre);
///			if(save_ctx_ != nullptr) save_ctx_->load(pre);

///			if(edit_ != nullptr) edit_->load(pre);

//			if(div_ != nullptr) div_->load(pre);

			// テスト・サーバー起動
///			server_.start();
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
				if(client_.connect()) {
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
///			server_.service();

			io_service_.run();

			wave_cap_.update();

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
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			wave_cap_.save();

//			sys::preference& pre = director_.at().preference_;

///			if(edit_ != nullptr) edit_->save(pre);

///			if(load_ctx_ != nullptr) load_ctx_->save(pre);
///			if(save_ctx_ != nullptr) save_ctx_->save(pre);

//			if(argv_frame_ != nullptr) {
//				argv_frame_->save(pre);
//			}

///			if(terminal_frame_ != nullptr) {
///				terminal_frame_->save(pre);
///			}

//			project_.save(pre);

///			if(div_ != nullptr) {
///				div_->save(pre);
///			}
		}
	};
}
