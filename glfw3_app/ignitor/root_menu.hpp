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
#include "utils/serial_win32.hpp"

#include "project.hpp"
#include "inspection.hpp"
#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "csv.hpp"
#include "wave_cap.hpp"
#include "test.hpp"
#include "kikusui.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ルート・メニュー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class root_menu {

		static const int32_t  app_version_ = 90;

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
		gui::widget_list*		cont_setting_serial_;
		gui::widget_button*		cont_setting_exec_;

		gui::widget_dialog*		info_dialog_;

		gui::widget_dialog*		msg_dialog_;

		inspection				inspection_;
		project					project_;

		int						ip_[4];

		bool					init_client_;

		enum class task {
			idle,	///< アイドル状態
			start,	///< 開始
			loop,	///< ループ基点
			wait,	///< 測定遅延
			mctrl,	///< モジュール制御
			sence,	///< センシング
			sync,	///< 同期
			error,	///< エラー
			sync_error,   ///< エラー同期
			retry,	///< リトライ
			fin,	///< 終了
		};
		task		task_;

		uint32_t	unit_id_;
		uint32_t	wait_;
		uint32_t	retry_;

		test::value_t	value_;

		gui::widget_dialog*		err_dialog_;
		gui::widget_dialog*		okc_dialog_;


		enum class mctrl_task {
			idle,

			init_icm,
			init_dc2,
			init_crm,
			init_wgm,
			init_dc1,

			icm,
			dc2,
			crm,
			wgm,
			dc1,
			wdm,
		};
		mctrl_task		mctrl_task_;
		uint32_t		mctrl_delay_;
		uint32_t		mctrl_id_;
		uint32_t		dc2_id_;
		uint32_t		crm_id_;
		uint32_t		wdm_id_[4];
		uint32_t		time_out_;

		typedef device::serial_win32 SERIAL;
		SERIAL				serial_;
		SERIAL::name_list	serial_list_;
		kikusui				kikusui_;

		void mctrl_service()
		{
			switch(mctrl_task_) {
			case mctrl_task::idle:
				break;

			case mctrl_task::init_icm:
				inspection_.exec_icm();
				mctrl_task_ = mctrl_task::init_dc2;
				break;
			case mctrl_task::init_dc2:
				inspection_.exec_dc2();
				mctrl_task_ = mctrl_task::init_crm;
				break;
			case mctrl_task::init_crm:
				inspection_.exec_crm();
				mctrl_task_ = mctrl_task::init_wgm;
				break;
			case mctrl_task::init_wgm:
				inspection_.exec_gen();
				mctrl_task_ = mctrl_task::init_dc1;
				break;
			case mctrl_task::init_dc1:
				inspection_.exec_dc1();
				mctrl_task_ = mctrl_task::idle;
				break;

			case mctrl_task::icm:
				inspection_.exec_icm();
				// ※ ICM 設定が以前と異なる場合に安全時間待機
				mctrl_delay_ = 15;  // ICM リレー安全時間 0.25 sec
				mctrl_task_ = mctrl_task::dc2;
				break;

			case mctrl_task::dc2:
				inspection_.exec_dc2();
				mctrl_task_ = mctrl_task::crm;
				break;

			case mctrl_task::crm:
				inspection_.exec_crm();
				mctrl_task_ = mctrl_task::wgm;
				break;

			case mctrl_task::wgm:
				inspection_.exec_gen();
				mctrl_task_ = mctrl_task::dc1;
				break;

			case mctrl_task::dc1:
				inspection_.exec_dc1();
				mctrl_task_ = mctrl_task::wdm;
				break;

			case mctrl_task::wdm:
				inspection_.exec_wdm();
				mctrl_task_ = mctrl_task::idle;
				++mctrl_id_;
				break;

			default:
				break;
			}
		}

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
			cont_setting_cmds_(nullptr), cont_setting_serial_(nullptr),
			cont_setting_exec_(nullptr),
			info_dialog_(nullptr), msg_dialog_(nullptr),
			inspection_(d, client, ilock, wave_cap_, kikusui_),
			project_(d),

			ip_{ 0 }, init_client_(false),

			task_(task::idle), unit_id_(0), wait_(0), retry_(0),
			err_dialog_(nullptr), okc_dialog_(nullptr),
			mctrl_task_(mctrl_task::idle), mctrl_delay_(0), mctrl_id_(0),
			dc2_id_(0), crm_id_(0), wdm_id_{ 0 }, time_out_(0),

			serial_(), serial_list_(), kikusui_(serial_)
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
					project_.enable_name_dialog();
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
					project_.open_load_file();
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
					project_.enable_edit_dialog();
				};
			}
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 3, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト・セーブ");
				save_project_ = wd.add_widget<widget_button>(wp, wp_);
				save_project_->at_local_param().select_func_ = [=](uint32_t id) {
					project_.open_save_file();
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
				{  // コントローラー・シリアル・ポート
					widget::param wp(vtx::irect(10, 150, 260, 40), root);
					widget_list::param wp_;
					cont_setting_serial_ = wd.add_widget<widget_list>(wp, wp_);
					cont_setting_serial_->at_local_param().select_func_ =
						[=](const std::string& str, uint32_t pos) {
						if(serial_.probe()) {
							serial_.close();
						}
					}; 
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
				widget_dialog::param wp_(widget_dialog::style::OK);
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

			wave_cap_.initialize();

			{  // エラー・ダイアログ
				int w = 550;
				int h = 300;
				widget::param wp(vtx::irect(75, 75, w, h));
				widget_dialog::param wp_;
				err_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				err_dialog_->enable(false);
			}
			{  // 確認ダイアログ
				int w = 550;
				int h = 300;
				widget::param wp(vtx::irect(75, 75, w, h));
				widget_dialog::param wp_(widget_dialog::style::CANCEL_OK);
				okc_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				okc_dialog_->enable(false);
				okc_dialog_->at_local_param().select_func_ = [=](bool ok) {
					if(ok) {
						task_ = task::sync;
					} else {
						task_ = task::fin;
					}
				};
			}

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
			// ネットワーク接続時
			if(!init_client_ && client_.probe()) {
				mctrl_task_ = mctrl_task::init_icm;
				init_client_ = true;
			}

			project_.update();
			if(project_.get_project_title().empty() || project_.get_project_root().empty()) {
				edit_project_->set_stall();
				save_project_->set_stall();
				run_->set_stall();
				proj_title_->set_text("");
				proj_path_->set_text("");
			} else {
				edit_project_->set_stall(false);
				proj_title_->set_text(project_.get_project_title());
				save_project_->set_stall(false);
				auto path = project_.get_project_root();
				proj_path_->set_text(path);
				if(project_.get_unit_count() > 0) {
					// プロジェクトの設定が有効なら、ストールを解除
					if(task_ == task::idle) {
						run_->set_stall(false);
					}
				}
			}

			inspection_.update();
			inspection_.update_client();

			wave_cap_.set_sample_param(get_inspection().get_sample_param());

			wave_cap_.update();

			kikusui_.update();

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

			// シリアルポート
			if(serial_list_.empty() || !serial_.compare(serial_list_)) {
				serial_.create_list();
				serial_list_ = serial_.get_list();
				utils::strings list;
				for(const auto& t : serial_list_) {
					list.push_back(t.port);
				}
				cont_setting_serial_->get_menu()->build(list);
				auto sel = cont_setting_serial_->get_local_param().text_param_.get_text();
				if(sel.empty()) {
	                cont_setting_serial_->select(0);
				} else {
	                cont_setting_serial_->select(sel);
				}
			}
			if(!serial_list_.empty()) {
				if(!serial_.probe()) {
					serial_.open(cont_setting_serial_->get_select_text());
// std::cout << "Serial Open: " << cont_setting_serial_->get_select_text() << std::endl;
				}
			}

			if(cont_setting_exec_ != nullptr) {
				cont_setting_exec_->set_stall(!client_.probe());
			}

			// 検査ループ制御
			switch(task_) {
			case task::idle:
				break;

			// 検査開始
			case task::start:
				if(!client_.probe()) {
					err_dialog_->set_text("コントローラー設定、「接続」\nを確認下さい。");
					err_dialog_->enable();
					task_ = task::idle;
					break;
				}
				unit_id_ = 0;
				task_ = task::loop;
				msg_dialog_->set_text("検査開始");
				msg_dialog_->enable();
				retry_ = 0;
				
				break;

			case task::loop:
				{
					auto fp = project_.get_unit_name(unit_id_);
					auto s = utils::get_file_name(fp);
					if(!inspection_.load(fp)) {
						std::string str;
						str = (boost::format("ファイル「%s」\n") % s).str();
						str += "が読めません。";
						err_dialog_->set_text(str);
						err_dialog_->enable();
						task_ = task::idle;
						break;
					}
					std::string top = (boost::format("検査「%s」\n") % s).str();
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

					std::string unit;
					std::string min;
					std::string max;
					if(v.term_ == 0) {
						unit = "A";
					} else if(v.term_ == 1) {
						unit = "V";
					} else if(v.term_ == 2) {
						unit = "V";
					} else if(v.term_ == 3) {
						unit = "KV";
					} else if(v.term_ == 4) {

					} else if(v.term_ == 5) {
						min = (boost::format("%3.2f") % v.min_).str();
						max = (boost::format("%3.2f") % v.max_).str();
						if(inspection_.get_crm_mode() == 0) { // 抵抗
							unit = "mOHM";
						} else {
							unit = "uF";
						}
					}

					project_.at_csv1().set(unit_id_ + 1, 1, v.symbol_);
					project_.at_csv1().set(unit_id_ + 1, 2, max);
					project_.at_csv1().set(unit_id_ + 1, 3, min);
					project_.at_csv1().set(unit_id_ + 1, 4, unit);

					project_.at_csv2().set(unit_id_ + 1, 1, v.symbol_);
					project_.at_csv2().set(unit_id_ + 1, 2, max);
					project_.at_csv2().set(unit_id_ + 1, 3, min);
					project_.at_csv2().set(unit_id_ + 1, 4, unit);

					wait_ = static_cast<uint32_t>(value_.wait_ * 60.0);
					task_ = task::wait;
				}
				break;

			case task::wait:
				if(wait_ > 0) {
					--wait_;
					break;
				}

				mctrl_task_ = mctrl_task::icm;
				for(int i = 0; i < 4; ++i) {
					wdm_id_[i] = client_.get_mod_status().wdm_id_[i];
				}
				crm_id_ = client_.get_mod_status().crm_id_;

				time_out_ = 60 * 5;
				task_ = task::mctrl;
				break;

			case task::mctrl:
				mctrl_service();
				if(value_.term_ < 4) { // WDM CH1 to CH4
					uint32_t n = 0;
					for(int i = 0; i < 4; ++i) {
						if(wdm_id_[i] != client_.get_mod_status().wdm_id_[i]) {
							++n;
						}
					}
					if(n == 4) {
						for(int i = 0; i < 4; ++i) {
							wdm_id_[i] = client_.get_mod_status().wdm_id_[i];
						}
						task_ = task::sence;
					}
				} else if(value_.term_ == 4) {  // DC2

				} else if(value_.term_ == 5) {  // CRM
					if(crm_id_ != client_.get_mod_status().crm_id_) {
						task_ = task::sence;
					}
				} else if(value_.term_ == 6) {  // 熱抵抗検査

				}
				if(time_out_ > 0) {
					--time_out_;
				} else {
					std::string str;
					str = "コントローラーとの通信が\n";
					str += "タイムアウトしました。";
					err_dialog_->set_text(str);
					err_dialog_->enable();
					task_ = task::idle;					
				}
				break;

			case task::sence:
				{
					bool f = false;
					// 動特性検査時
					if(value_.term_ < 4) { // WDM CH1 to CH4
						f = wave_cap_.value_check(value_);
					} else if(value_.term_ == 4) {  // DC2

					} else if(value_.term_ == 5) {  // CRM
						double v = 0.0;
						if(inspection_.get_crm_mode() == 0) { // 抵抗
							v = inspection_.get_crrd_value();
						} else {
							v = inspection_.get_crcd_value();
						}
						if(value_.min_ <= v && v <= value_.max_) {
							f = true;
						}
						std::string st = (boost::format("%3.2f") % v).str();
						auto idx = project_.get_csv_index() - 1;
						project_.at_csv1().set(unit_id_ + 1, 9 + idx, st);
						project_.at_csv2().set(unit_id_ + 1, 5, st);
					} else if(value_.term_ == 6) {  // 熱抵抗検査

					}
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
				{
					std::string str;
					str = (boost::format("「%s」") % value_.symbol_).str();
					str += "検査エラー：\nデータ保存しますか？";
					okc_dialog_->set_text(str);
					okc_dialog_->enable();
					task_ = task::sync_error;
					break;
				}

			case task::sync_error:
				break;

			case task::sync:
				++unit_id_;
				if(unit_id_ >= project_.get_unit_count()) {
					project_.save_csv1();
					project_.save_csv2();
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
			err_dialog_->load(pre);
			okc_dialog_->load(pre);

			wave_edit_->load(pre);

			cont_setting_dialog_->load(pre);
			cont_connect_->load(pre);
			cont_setting_ip_[0]->load(pre);
			cont_setting_ip_[1]->load(pre);
			cont_setting_ip_[2]->load(pre);
			cont_setting_ip_[3]->load(pre);
			cont_setting_serial_->load(pre);

			project_.load_dialog(pre);	// ダイアログの位置復元
			inspection_.get_dialog()->load(pre);  	// ダイアログの位置復元

			wave_cap_.load(pre);
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
			err_dialog_->save(pre);
			okc_dialog_->save(pre);

			wave_edit_->save(pre);

			cont_setting_dialog_->save(pre);
			cont_connect_->save(pre);
			cont_setting_ip_[0]->save(pre);
			cont_setting_ip_[1]->save(pre);
			cont_setting_ip_[2]->save(pre);
			cont_setting_ip_[3]->save(pre);
			cont_setting_serial_->save(pre);

			project_.save_dialog(pre);	// ダイアログの位置セーブ
			inspection_.get_dialog()->save(pre);  	// ダイアログの位置セーブ

			wave_cap_.save(pre);
		}
	};
}
