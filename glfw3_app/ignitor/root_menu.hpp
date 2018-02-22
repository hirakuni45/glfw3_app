#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・ルート・メニュー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/select_file.hpp"

#include "project.hpp"
#include "inspection.hpp"
#include "ign_client_tcp.hpp"
#include "interlock.hpp"

#define NATIVE_FILER

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ルート・メニュー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class root_menu {

		static constexpr const char* PROJECT_EXT_ = "ipr";

		static const int ofs_x_ = 10;
		static const int ofs_y_ = 10;
		static const int btn_w_ = 240;
		static const int btn_h_ = 60;

		utils::director<core>&	director_;

		net::ign_client_tcp&	client_;
		interlock&				interlock_;

		gui::widget_button*		new_project_;
		gui::widget_label*		proj_title_;

		gui::widget_button*		load_project_;
		gui::widget_label*		proj_path_;

		gui::widget_button*		edit_project_;
		gui::widget_button*		save_project_;
		gui::widget_button*		igni_settings_;
		gui::widget_button*		cont_settings_;
		gui::widget_check*		wave_edit_;
		gui::widget_button*		run_;

		gui::widget_dialog*		proj_name_dialog_;
		gui::widget_label*		proj_name_label_;

		project					project_;
		inspection				inspection_;

		gui::widget_dialog*		cont_setting_dialog_;
		gui::widget_check*		cont_connect_;
		gui::widget_label*		cont_setting_ip_[4];
		gui::widget_label*		cont_setting_cmds_;
		gui::widget_button*		cont_setting_exec_;

#ifdef NATIVE_FILER
		utils::select_file		proj_load_filer_;
		utils::select_file		proj_save_filer_;
#else
		gui::widget_filer*		proj_load_filer_;
		gui::widget_filer*		proj_save_filer_;
#endif
		std::string				root_name_;

		std::string				ip_str_[4];
		int						ip_[4];


		void stall_button_(bool f)
		{
return;
			new_project_->set_stall(f);
			proj_title_->set_stall(f);
			load_project_->set_stall(f);
//			edit_project_->set_stall(f);
//			save_project_->set_stall(f);
			cont_settings_->set_stall(f);
			igni_settings_->set_stall(f);
//			run_->set_stall(f);
		}


		void save_setting_value_()
		{
			for(int i = 0; i < 4; ++i) {
				ip_str_[i] = cont_setting_ip_[i]->get_text();
			}
		}


		void load_setting_value_()
		{
			for(int i = 0; i < 4; ++i) {
				cont_setting_ip_[i]->set_text(ip_str_[i]);
			}
		}


		void update_project_()
		{
return;
///			if(inspection_.get_title().empty()) {
				edit_project_->set_stall();
				save_project_->set_stall();
				run_->set_stall();
///			} else {
				edit_project_->set_stall(false);
				save_project_->set_stall(false);
				run_->set_stall(false);
///			}
		}


		bool save_project_file_(const std::string& path)
		{
			sys::preference pre;
			proj_title_->save(pre);
			project_.save(pre);
			auto ph = path;
			if(utils::get_file_ext(path).empty()) {
				ph += PROJECT_EXT_;
			}
			return pre.save(ph);
		}


		bool load_project_file_(const std::string& path)
		{
			sys::preference pre;
			auto ph = path;
			if(utils::get_file_ext(path).empty()) {
				ph += PROJECT_EXT_;
			}
			auto ret = pre.load(ph);
			if(ret) {
				proj_title_->load(pre);
				project_.load(pre);
			}
			return ret;
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		root_menu(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilock) :
			director_(d), client_(client), interlock_(ilock),
			new_project_(nullptr),  proj_title_(nullptr),
			load_project_(nullptr), proj_path_(nullptr),
			edit_project_(nullptr),
			save_project_(nullptr),
			igni_settings_(nullptr), cont_settings_(nullptr),
			wave_edit_(nullptr),
			run_(nullptr),
			proj_name_dialog_(nullptr), proj_name_label_(nullptr),
			project_(d),
			inspection_(d, client, ilock),
			cont_setting_dialog_(nullptr),
		   	cont_connect_(nullptr), cont_setting_ip_{ nullptr },
			cont_setting_cmds_(nullptr), cont_setting_exec_(nullptr),
#ifndef NATIVE_FILER
			proj_load_filer_(nullptr), proj_save_filer_(nullptr),
#endif
			ip_{ 0 }
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~root_menu()
		{
			destroy();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  inspection の参照を取得
			@return inspection の参照
		*/
		//-----------------------------------------------------------------//
		const inspection& get_inspection() const { return inspection_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief  ターゲット接続の取得
			@return	ターゲット接続
		*/
		//-----------------------------------------------------------------//
		bool get_target_connect() const
		{
			if(cont_connect_ == nullptr) {
				return false;
			}
			return cont_connect_->get_check();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ターゲット IP アドレスの取得
			@return	IP アドレス 
		*/
		//-----------------------------------------------------------------//
		std::string get_target_ip() const
		{
			std::string ips;
			ips.clear();
			if(!cont_connect_->get_check()) {
				return ips;
			}
			for(int i = 0; i < 4; ++i) {
				if(cont_setting_ip_[i] == nullptr) break;
				const std::string& ip = cont_setting_ip_[i]->get_text();
				ips += ip;
				if(i < 3) ips += ".";
			}
			return ips; 
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形編集の状態を取得
			@return	波形編集の状態
		*/
		//-----------------------------------------------------------------//
		bool get_wave_edit() const {
			if(wave_edit_ == nullptr) return false;
			return wave_edit_->get_check();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			auto& core = gl::core::get_instance();
			const auto& scs = core.get_rect().size;

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int sph = btn_h_ + 10;
			int scw = scs.x;
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 0, btn_w_, btn_h_));
				widget_button::param wp_("新規プロジェクト");
				new_project_ = wd.add_widget<widget_button>(wp, wp_);
				new_project_->at_local_param().select_func_ = [=](uint32_t id) {
					proj_name_dialog_->enable();
					root_name_ = proj_title_->get_text();
				};
				{
					widget::param wp(vtx::irect(ofs_x_ + btn_w_ + 50,
						ofs_y_ + sph * 0 + ((btn_h_ - 40) / 2), 400, 40));
					widget_label::param wp_("");
					proj_title_ = wd.add_widget<widget_label>(wp, wp_);
				}
			}

			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 1, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト・ロード");
				load_project_ = wd.add_widget<widget_button>(wp, wp_);
				load_project_->at_local_param().select_func_ = [=](uint32_t id) {
					stall_button_(true);
#ifdef NATIVE_FILER
					std::string filter = "プロジェクト(*.";
					filter += PROJECT_EXT_;
					filter += ")\t*.";
					filter += PROJECT_EXT_;
					filter += "\t";
					proj_load_filer_.open(filter);
#else
					proj_load_filer_->enable();
#endif
				};
				{
					widget::param wp(vtx::irect(ofs_x_ + btn_w_ + 50,
						ofs_y_ + sph * 1 + ((btn_h_ - 40) / 2), 600, 40));
					widget_label::param wp_("");
					proj_path_ = wd.add_widget<widget_label>(wp, wp_);
				}
			}
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 2, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト編集");
				edit_project_ = wd.add_widget<widget_button>(wp, wp_);
				edit_project_->at_local_param().select_func_ = [=](uint32_t id) {
					project_.get_dialog()->enable();
				};
			}
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 3, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト・セーブ");
				save_project_ = wd.add_widget<widget_button>(wp, wp_);
				save_project_->at_local_param().select_func_ = [=](uint32_t id) {
					stall_button_(true);
#ifdef NATIVE_FILER
					std::string filter = "プロジェクト(*.";
					filter += PROJECT_EXT_;
					filter += ")\t*.";
					filter += PROJECT_EXT_;
					filter += "\t";
					proj_save_filer_.open(filter, true);
#else
					proj_save_filer_->enable();
#endif
				};
			}

			{  // 単体試験編集ボタン
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 4, btn_w_, btn_h_));
				widget_button::param wp_("単体試験編集");
				igni_settings_ = wd.add_widget<widget_button>(wp, wp_);
				igni_settings_->at_local_param().select_func_ = [=](uint32_t id) {
					inspection_.get_dialog()->enable();
				};
			}

			{  // コントローラー設定ボタン
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 5, btn_w_, btn_h_));
				widget_button::param wp_("コントローラー設定");
				cont_settings_ = wd.add_widget<widget_button>(wp, wp_);
				cont_settings_->at_local_param().select_func_ = [=](uint32_t id) {
					save_setting_value_();
					cont_setting_dialog_->enable();
				};
			}
			{  // 波形編集機能
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 6, btn_w_, btn_h_));
				widget_check::param wp_("波形編集");
				wave_edit_ = wd.add_widget<widget_check>(wp, wp_);
				wave_edit_->at_local_param().select_func_ = [=](bool f) {

				};
			}
			{  // 検査開始ボタン
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 7, btn_w_, btn_h_));
				widget_button::param wp_("検査開始");
				run_ = wd.add_widget<widget_button>(wp, wp_);
				run_->at_local_param().select_func_ = [=](uint32_t id) {
				};
			}

			{  // プロジェクト名入力ダイアログ
				int w = 300;
				int h = 200;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				proj_name_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				proj_name_dialog_->enable(false);
				proj_name_dialog_->at_local_param().select_func_ = [=](bool ok) {
					if(!ok) {
						proj_title_->set_text(root_name_);
						return;
					}
					proj_title_->set_text(proj_name_label_->get_text());
					update_project_();
				};
				{
					widget::param wp(vtx::irect(10, 20, w - 10 * 2, 40), proj_name_dialog_);
					widget_text::param wp_("プロジェクト名：");
					wd.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 70, w - 10 * 2, 40), proj_name_dialog_);
					widget_label::param wp_("", false);
					proj_name_label_ = wd.add_widget<widget_label>(wp, wp_);
				}
			}

			project_.initialize();

			inspection_.initialize();

			{  // コントローラー設定ダイアログ
				int w = 450;
				int h = 310;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				cont_setting_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				cont_setting_dialog_->enable(false);
				cont_setting_dialog_->at_local_param().select_func_ = [=](bool ok) {
					if(!ok) {
						load_setting_value_();
						return;
					}
					for(int i = 0; i < 4; ++i) {
						const std::string& ip = cont_setting_ip_[i]->get_text();
						int v = 0;
						if((utils::input("%d", ip.c_str()) % v).status()) {
							ip_[i] = v;
						}
					}
//					utils::format("IP: %d, %d, %d, %d\n") % ip_[0] % ip_[1] % ip_[2] % ip_[3];
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
					cont_connect_->at_local_param().select_func_ = [=](bool f) {
///						client_.start();
					};
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
					widget_label::param wp_("20", false);
					cont_setting_ip_[3] = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // コントローラー・コマンド
					widget::param wp(vtx::irect(10, 100, w - 10 * 2, 40), root);
					widget_label::param wp_("", false);
					cont_setting_cmds_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // コントローラー・コマンド実行ボタン
					widget::param wp(vtx::irect(w - 110, 150, 100, 40), root);
					widget_button::param wp_("exec");
					cont_setting_exec_ = wd.add_widget<widget_button>(wp, wp_);
					cont_setting_exec_->at_local_param().select_func_ = [=](uint32_t id) {
						auto s = cont_setting_cmds_->get_text();
						s += '\n';
						client_.send_data(s);
					};
				}
			}

#ifndef NATIVE_FILER
			{  // プロジェクト・ファイラー
				gl::core& core = gl::core::get_instance();
				{
					widget::param wp(vtx::irect(30, 30, 500, 400));
					widget_filer::param wp_(core.get_current_path(), "", false);
					proj_load_filer_ = wd.add_widget<widget_filer>(wp, wp_);
					proj_load_filer_->enable(false);
					proj_load_filer_->at_local_param().select_file_func_ = [=](const std::string& path) {

///						if(inspection_.load(path)) {
///							proj_title_->set_text(inspection_.get_title());
///							proj_path_->set_text(path);
///							update_project_();
///						}
						stall_button_(false);
					};
					proj_load_filer_->at_local_param().cancel_file_func_ = [=](void) {
						stall_button_(false);
					};
				}
				{
					widget::param wp(vtx::irect(30, 30, 500, 400));
					widget_filer::param wp_(core.get_current_path(), "", true);
					proj_save_filer_ = wd.add_widget<widget_filer>(wp, wp_);
					proj_save_filer_->enable(false);
					proj_save_filer_->at_local_param().select_file_func_ = [=](const std::string& path) {
///						inspection_.save(path);
						stall_button_(false);
					};
					proj_save_filer_->at_local_param().cancel_file_func_ = [=](void) {
						stall_button_(false);
					};
				}
			}
#endif

			// プリファレンスのロード
			sys::preference& pre = director_.at().preference_;

			wave_edit_->load(pre);

			cont_setting_dialog_->load(pre);
			cont_connect_->load(pre);
			cont_setting_ip_[0]->load(pre);
			cont_setting_ip_[1]->load(pre);
			cont_setting_ip_[2]->load(pre);
			cont_setting_ip_[3]->load(pre);

			proj_name_dialog_->load(pre);		  // ダイアログの位置復元
			inspection_.get_dialog()->load(pre);  // ダイアログの位置復元
			project_.get_dialog()->load(pre);	  // ダイアログの位置復元

#ifndef NATIVE_FILER
			proj_load_filer_->load(pre);
			proj_save_filer_->load(pre);
#endif
			update_project_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
//			auto& core = gl::core::get_instance();
//			const auto& scs = core.get_rect().size;
//			run_->at_rect().org.x = scs.x - btn_w_ - ofs_x_;

			inspection_.update();

			project_.update();

			if(cont_setting_exec_ != nullptr) {
				cont_setting_exec_->set_stall(!client_.probe());
			}

#ifdef NATIVE_FILER
			if(proj_load_filer_.state()) {
				auto path = proj_load_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += PROJECT_EXT_;
					}
					if(load_project_file_(path)) {
						proj_path_->set_text(path);
					} else {
						proj_title_->set_text("");
					}
				}
				update_project_();
				stall_button_(false);
			}
			if(proj_save_filer_.state()) {
				auto path = proj_save_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += PROJECT_EXT_;
					}
					if(!save_project_file_(path)) {
// save error
					}
					stall_button_(false);
				}
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			sys::preference& pre = director_.at().preference_;

			wave_edit_->save(pre);

			cont_setting_dialog_->save(pre);
			cont_connect_->save(pre);
			cont_setting_ip_[0]->save(pre);
			cont_setting_ip_[1]->save(pre);
			cont_setting_ip_[2]->save(pre);
			cont_setting_ip_[3]->save(pre);

			proj_name_dialog_->save(pre);		  // ダイアログの位置セーブ
			inspection_.get_dialog()->save(pre);  // ダイアログの位置セーブ
			project_.get_dialog()->save(pre);	  // ダイアログの位置セーブ

#ifndef NATIVE_FILER
			proj_load_filer_->save(pre);
			proj_save_filer_->save(pre);

			wd.del_widget(proj_save_filer_);
			proj_save_filer_ = nullptr;
			wd.del_widget(proj_load_filer_);
			proj_load_filer_ = nullptr;
#endif
			wd.del_widget(cont_setting_dialog_);
			cont_setting_dialog_ = nullptr;
			wd.del_widget(proj_name_dialog_);
			proj_name_dialog_ = nullptr;

			wd.del_widget(cont_settings_);
			cont_settings_ = nullptr;
			wd.del_widget(save_project_);
			save_project_ = nullptr;
			wd.del_widget(edit_project_);
			edit_project_ = nullptr;

			wd.del_widget(load_project_);
			load_project_ = nullptr;
			wd.del_widget(proj_path_);
			proj_path_ = nullptr;

			wd.del_widget(proj_title_);
			proj_title_ = nullptr;
			wd.del_widget(new_project_);
			new_project_ = nullptr;
		}
	};
}

