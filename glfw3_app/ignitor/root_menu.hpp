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
#include "csv.hpp"
#include "wave_cap.hpp"
#include "test.hpp"

#define NATIVE_FILER

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ルート・メニュー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class root_menu {

		static const int32_t  app_version_ = 80;

		static constexpr const char* PROJECT_EXT_ = "ipr";

		static const int ofs_x_ = 10;
		static const int ofs_y_ = 10;
		static const int btn_w_ = 240;
		static const int btn_h_ = 60;

		utils::director<core>&	director_;

		net::ign_client_tcp&	client_;
		interlock&				interlock_;

		wave_cap				wave_cap_;
		uint32_t				info_id_;

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
		gui::widget_button*		info_;

		gui::widget_dialog*		cont_setting_dialog_;
		gui::widget_check*		cont_connect_;
		gui::widget_label*		cont_setting_ip_[4];
		gui::widget_label*		cont_setting_cmds_;
		gui::widget_button*		cont_setting_exec_;

		gui::widget_dialog*		info_dialog_;

		gui::widget_dialog*		msg_dialog_;

#ifdef NATIVE_FILER
		utils::select_file		proj_load_filer_;
		utils::select_file		proj_save_filer_;
#else
		gui::widget_filer*		proj_load_filer_;
		gui::widget_filer*		proj_save_filer_;
#endif
		inspection				inspection_;
		project					project_;

		int						ip_[4];


		bool save_project_file_(const std::string& path)
		{
			sys::preference pre;

			project_.save(pre);
			auto ph = path;
			if(utils::get_file_ext(path).empty()) {
				ph += '.';
				ph += PROJECT_EXT_;
			}
			return pre.save(ph);
		}


		bool load_project_file_(const std::string& path)
		{
			sys::preference pre;
			auto ph = path;
			if(utils::get_file_ext(path).empty()) {
				ph += '.';
				ph += PROJECT_EXT_;
			}
			auto ret = pre.load(ph);
			if(ret) {
				project_.load(pre);
			}
			return ret;
		}


		enum class task {
			idle,	///< アイドル状態
			start,	///< 開始
			loop,	///< ループ基点
			wait,	///< 測定遅延
			mctrl,	///< モジュール制御
			sence,	///< センシング
			sync,	///< 同期
			error,	///< エラー
			retry,	///< リトライ
			fin,	///< 終了
		};
		task		task_;

		uint32_t	unit_id_;
		uint32_t	wait_;
		uint32_t	retry_;

		test::value_t	value_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		root_menu(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilock) :
			director_(d), client_(client), interlock_(ilock),
			wave_cap_(d, client, interlock_), info_id_(0),
			new_project_(nullptr),  proj_title_(nullptr),
			load_project_(nullptr), proj_path_(nullptr),
			edit_project_(nullptr),
			save_project_(nullptr),
			igni_settings_(nullptr), cont_settings_(nullptr),
			wave_edit_(nullptr),
			run_(nullptr), info_(nullptr),
			cont_setting_dialog_(nullptr),
		   	cont_connect_(nullptr), cont_setting_ip_{ nullptr },
			cont_setting_cmds_(nullptr), cont_setting_exec_(nullptr),
			info_dialog_(nullptr), msg_dialog_(nullptr),
#ifndef NATIVE_FILER
			proj_load_filer_(nullptr), proj_save_filer_(nullptr),
#endif
			inspection_(d, client, ilock),
			project_(d),

			ip_{ 0 }, task_(task::idle), unit_id_(0), wait_(0), retry_(0)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~root_menu()
		{
			save();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  inspection を取得
			@return inspection
		*/
		//-----------------------------------------------------------------//
		const inspection& get_inspection() const { return inspection_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief  inspection の参照を取得
			@return inspection の参照
		*/
		//-----------------------------------------------------------------//
		inspection& at_inspection() { return inspection_; } 


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
					project_.get_name_dialog()->enable();
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
						ofs_y_ + sph * 1 + ((btn_h_ - 40) / 2), 900, 40));
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
					cont_setting_dialog_->enable();
				};
			}
			{  // 波形編集機能
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 6, btn_w_, btn_h_));
				widget_check::param wp_("波形編集");
				wave_edit_ = wd.add_widget<widget_check>(wp, wp_);
				wave_edit_->at_local_param().select_func_ = [=](bool f) {
					wave_cap_.enable(f);
				};
			}
			{  // 検査開始ボタン
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 7, btn_w_, btn_h_));
				widget_button::param wp_("検査開始");
				run_ = wd.add_widget<widget_button>(wp, wp_);
				run_->at_local_param().select_func_ = [=](uint32_t id) {
					task_ = task::start;
				};
			}
			{  // 検査開始ボタン
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 8, btn_w_, btn_h_));
				widget_button::param wp_("情報");
				info_ = wd.add_widget<widget_button>(wp, wp_);
				info_->at_local_param().select_func_ = [=](uint32_t id) {
					info_dialog_->enable();
				};
			}

			inspection_.initialize();
			project_.initialize();

			{  // コントローラー設定ダイアログ
				int w = 450;
				int h = 260;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
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
///					cont_connect_->at_local_param().select_func_ = [=](bool f) {
///						client_.start();
///					};
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

			{  // 情報ダイアログ
				int w = 450;
				int h = 210;
				widget::param wp(vtx::irect(50, 50, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				info_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				std::string s =	"イグナイター検査\n";
				uint32_t bid = B_ID;
				s += (boost::format("Build: %d\n") % bid).str();
				s += (boost::format("Version %d.%02d\n")
					% (app_version_ / 100) % (app_version_ % 100)).str();
				s += "Copyright (C) 2018 Graviton Inc.\n";
				s += "All Rights Reserved.";
				info_dialog_->set_text(s);
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

#ifndef NATIVE_FILER
			{  // プロジェクト・ファイラー
				gl::core& core = gl::core::get_instance();
				{
					widget::param wp(vtx::irect(30, 30, 500, 400));
					widget_filer::param wp_(core.get_current_path(), "", false);
					proj_load_filer_ = wd.add_widget<widget_filer>(wp, wp_);
					proj_load_filer_->enable(false);
					proj_load_filer_->at_local_param().select_file_func_ = [=]
						(const std::string& path) {
					};
					proj_load_filer_->at_local_param().cancel_file_func_ = [=](void) {
					};
				}
				{
					widget::param wp(vtx::irect(30, 30, 500, 400));
					widget_filer::param wp_(core.get_current_path(), "", true);
					proj_save_filer_ = wd.add_widget<widget_filer>(wp, wp_);
					proj_save_filer_->enable(false);
					proj_save_filer_->at_local_param().select_file_func_ = [=]
						(const std::string& path) {
					};
					proj_save_filer_->at_local_param().cancel_file_func_ = [=](void) {
					};
				}
			}
#endif
			wave_cap_.initialize();

			load();

			wave_cap_.enable(wave_edit_->get_check());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			project_.update();
			if(project_.get_project_title().empty() || project_.get_project_path().empty()) {
				edit_project_->set_stall();
				save_project_->set_stall();
				run_->set_stall();
				proj_title_->set_text("");
				proj_path_->set_text("");
			} else {
				edit_project_->set_stall(false);
				proj_title_->set_text(project_.get_project_title());
				save_project_->set_stall(false);
				auto path = project_.get_project_path();
				if(!path.empty()) {
					if(path[0] != '/') {
						gl::core& core = gl::core::get_instance();
						auto tmp = core.get_current_path();
						tmp += '/';
						tmp += path;
						path = tmp;
					}
				}
				proj_path_->set_text(path);
				if(project_.get_unit_count() > 0) {
					// プロジェクトの設定が有効なら、ストールを解除
					if(task_ == task::idle) {
						run_->set_stall(false);
					}
				}
			}

			inspection_.update();

			wave_cap_.set_sample_param(get_inspection().get_sample_param());

			wave_cap_.update();

			// 波形計測のバックアノテーション
			const auto& inf = wave_cap_.get_info();
			if(inf.id_ != info_id_) {
				if(inf.annotate_) {
					const auto& test = at_inspection().get_test_param();
					if(test.delay_ != nullptr) {
						test.delay_->set_text((boost::format("%e") % inf.sample_org_).str());
					}
					if(test.width_ != nullptr) {
						test.width_->set_text((boost::format("%e") % inf.sample_width_).str());
					}
					if(test.term_ != nullptr) {
						auto n = test.term_->get_select_pos();
						if(n <= 3 && test.min_ != nullptr) {
							test.min_->set_text((boost::format("%4.3f") % inf.min_[n]).str());
						}
						if(n <= 3 && test.max_ != nullptr) {
							test.max_->set_text((boost::format("%4.3f") % inf.max_[n]).str());
						}
					}
				}
				info_id_ = inf.id_;
			}

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

					} else {

					}
				}
			}
			if(proj_save_filer_.state()) {
				auto path = proj_save_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += PROJECT_EXT_;
					}
					if(!save_project_file_(path)) {

					} else {

					}
				}
			}
#endif

			switch(task_) {
			case task::idle:
				break;
			case task::start:
				unit_id_ = 0;
				task_ = task::loop;
				msg_dialog_->set_text("検査開始");
				msg_dialog_->enable();
				retry_ = 0;
				break;

			case task::loop:
				{
					auto fp = project_.get_unit_name(unit_id_);
					bool ret = inspection_.load(fp);
					auto s = utils::get_file_name(fp);
					if(ret) {
						s += " (OK)";
					} else {
						s += " (NG)";
					}
					std::string top = (boost::format("Test: %s\n") % s).str();
					inspection_.at_test_param().build_value();
					const auto& v = inspection_.get_test_param().value_;
					top += (boost::format("Symbol: %s\n") % v.symbol_).str();
					top += (boost::format("Retry: %d/%d\n") % retry_ % v.retry_).str();
					top += (boost::format("Wait:  %2.1f [s]\n") % v.wait_).str();
					top += (boost::format("Term:  %d\n") % v.term_).str();
					top += (boost::format("Delay: %e [s]\n") % v.delay_).str();
					top += (boost::format("Filter: %d\n") % v.filter_).str();
					top += (boost::format("Width: %e [s]\n") % v.width_).str();
					top += (boost::format("Min: %e [s]\n") % v.min_).str();
					top += (boost::format("Max: %e [s]") % v.max_).str();
					value_ = v;

					msg_dialog_->set_text(top);

					wait_ = static_cast<uint32_t>(value_.wait_ * 60.0);
wait_ = 120;
					if(wait_ > 0) {
						task_ = task::wait;
					} else {
						task_ = task::sence;
					}
				}
				break;

			case task::wait:
				if(wait_ > 0) {
					--wait_;
					break;
				}
				task_ = task::mctrl;
				break;

			case task::mctrl:
//				inspection_.
				task_ = task::sence;
				break;

			case task::sence:
				{
					bool f = wave_cap_.value_check(value_);
					if(f) {
						task_ = task::sync;
					} else {
						task_ = task::retry;
					}
				}
				break;

			case task::retry:
				++retry_;
				if(retry_ >= value_.retry_) {
					task_ = task::error;
				} else {
					task_ = task::loop;
				}
				break;

			case task::error:
				msg_dialog_->set_text("error:");
				task_ = task::sync;
				break;

			case task::sync:
				++unit_id_;
				if(unit_id_ >= project_.get_unit_count()) {
					task_ = task::fin;
				} else {
					task_ = task::loop;
				}
				break;

			case task::fin:
				msg_dialog_->enable(false);
				task_ = task::idle;
				break;

			default:
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プリファレンスのロード
		*/
		//-----------------------------------------------------------------//
		void load()
		{
			sys::preference& pre = director_.at().preference_;

			info_dialog_->load(pre);
			msg_dialog_->load(pre);

			wave_edit_->load(pre);

			cont_setting_dialog_->load(pre);
			cont_connect_->load(pre);
			cont_setting_ip_[0]->load(pre);
			cont_setting_ip_[1]->load(pre);
			cont_setting_ip_[2]->load(pre);
			cont_setting_ip_[3]->load(pre);

			project_.get_name_dialog()->load(pre);	// ダイアログの位置復元
			project_.get_dialog()->load(pre);	  	// ダイアログの位置復元
			inspection_.get_dialog()->load(pre);  	// ダイアログの位置復元

#ifndef NATIVE_FILER
			proj_load_filer_->load(pre);
			proj_save_filer_->load(pre);
#endif
			wave_cap_.load();
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

			info_dialog_->save(pre);
			msg_dialog_->save(pre);

			wave_edit_->save(pre);

			cont_setting_dialog_->save(pre);
			cont_connect_->save(pre);
			cont_setting_ip_[0]->save(pre);
			cont_setting_ip_[1]->save(pre);
			cont_setting_ip_[2]->save(pre);
			cont_setting_ip_[3]->save(pre);

			project_.get_name_dialog()->save(pre);	// ダイアログの位置セーブ
			project_.get_dialog()->save(pre);	  	// ダイアログの位置セーブ
			inspection_.get_dialog()->save(pre);  	// ダイアログの位置セーブ

#ifndef NATIVE_FILER
			proj_load_filer_->save(pre);
			proj_save_filer_->save(pre);
#endif

			wave_cap_.save();
		}
	};
}
