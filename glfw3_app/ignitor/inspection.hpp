#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター単体検査クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "utils/select_file.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_chip.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_sheet.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "tools.hpp"
#include "relay_map.hpp"
#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "wave_cap.hpp"
#include "test.hpp"
#include "kikusui.hpp"

#include "wdm.hpp"
#include "dc1.hpp"
#include "dc2.hpp"
#include "wgm.hpp"
#include "crm.hpp"
#include "icm.hpp"
#include "test_param.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  単体検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection
	{
	public:
		static constexpr const char* UNIT_EXT_ = "unt";  ///< 単体検査ファイル、拡張子

		//=================================================================//
		/*!
			@brief  計測モード
		*/
		//=================================================================//
		enum class test_mode {
			NONE,		///< 無効
			C_MES,		///< 容量計測
			R_MES,		///< 抵抗計測
		};

	private:
		utils::director<core>&	director_;

		net::ign_client_tcp&	client_;

		interlock&				interlock_;
		wave_cap&				wave_cap_;
		kikusui&				kikusui_;

		gui::widget_dialog*		dialog_;
		gui::widget_button*		load_file_;			///< load file
		gui::widget_button*		save_file_;			///< save file
		gui::widget_check*		ilock_enable_;		///< Interlock 許可／不許可
		gui::widget_sheet*		sheet_;
		gui::widget_null*		style_[4];
		utils::select_file		unit_load_filer_;
		utils::select_file		unit_save_filer_;

		dc1						dc1_;
		dc2						dc2_;
		wgm						wgm_;
		crm						crm_;
		icm						icm_;
		wdm						wdm_;

		// 測定用件
		test_param				test_param_;

		gui::widget_chip*		chip_;			///< help chip

		bool					startup_init_;
		uint32_t				crrd_id_;
		uint32_t				crcd_id_;
		uint32_t				d2md_id_;

		double					crrd_value_;
		double					crcd_value_;

		test_mode				test_mode_;

		struct vc_t {
			float		volt_max_;	/// 0.1V step
			float		volt_;		/// 0.1V step
			float		curt_max_;	/// 0.1A/0.01mA step
			float		curt_;		/// 0.1A/0.01mA step
		};

		enum class cmd_task {
			idle,		///< アイドル

			init_all,	///< 全ての初期化
			init_all0,
			init_all1,
			init_all2,
			init_all3,
			init_all4,

			crm,		///< CRM 開始
			crm0,



		};
		cmd_task		cmd_task_;

		void cmd_service_()
		{
			if(!client_.probe()) return;

			switch(cmd_task_) {
			case cmd_task::idle:
				break;

			case cmd_task::init_all:
				icm_.startup();
				cmd_task_ = cmd_task::init_all0;
				break;
			case cmd_task::init_all0:
				dc1_.startup();
				cmd_task_ = cmd_task::init_all1;
				break;
			case cmd_task::init_all1:
				dc2_.startup();
				cmd_task_ = cmd_task::init_all2;
				break;
			case cmd_task::init_all2:
				crm_.startup();
				cmd_task_ = cmd_task::init_all3;
				break;
			case cmd_task::init_all3:
				wgm_.startup();
				cmd_task_ = cmd_task::init_all4;
				break;
			case cmd_task::init_all4:
				wdm_.startup();
				cmd_task_ = cmd_task::idle;
				break;

			case cmd_task::crm:
				icm_.exec_->exec();
				cmd_task_ = cmd_task::crm0;
				break;
			case cmd_task::crm0:
				crm_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;

#if 0
//				
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
#endif
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
		inspection(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilock, wave_cap& wc, kikusui& kik) :
			director_(d), client_(client), interlock_(ilock), wave_cap_(wc), kikusui_(kik),
			dialog_(nullptr),
			load_file_(nullptr), save_file_(nullptr), ilock_enable_(nullptr),
			sheet_(nullptr), style_{ nullptr },

			dc1_(d, client_, interlock_),
			dc2_(d, client_, interlock_, kikusui_),
			wgm_(d, client_, interlock_),
			crm_(d, client_, interlock_),
			icm_(d, client_, interlock_),
			wdm_(d, client_, interlock_),
			test_param_(d),

			chip_(nullptr),

			startup_init_(false),
			crrd_id_(0), crcd_id_(0), d2md_id_(0),
			crrd_value_(0.0), crcd_value_(0.0),
			test_mode_(test_mode::NONE), cmd_task_(cmd_task::idle)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  計測モードの取得
			@return 計測モード
		*/
		//-----------------------------------------------------------------//
		test_mode get_test_mode() const {
			if(sheet_->get_select_pos() == 0) {  // CR
				if(crm_.mode_->get_select_pos() == 0) {
					return test_mode::R_MES;
				} else {
					return test_mode::C_MES;
				}
			}			
			return test_mode::NONE;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計測単位の取得
			@return 計測単位
		*/
		//-----------------------------------------------------------------//
		std::string get_test_unit() const {
			if(sheet_->get_select_pos() == 0) {  // CR
				if(crm_.mode_->get_select_pos() == 0) {
					return "mOHM";
				} else {
					return "uF";
				}
			}			
			return "";
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト・パラメーターの取得
			@return テスト・パラメーター
		*/
		//-----------------------------------------------------------------//
		const test_param& get_test_param() const { return test_param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト・パラメーターの取得
			@return テスト・パラメーター
		*/
		//-----------------------------------------------------------------//
		test_param& at_test_param() { return test_param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  サンプリング・パラメーターの取得
			@return サンプリング・パラメーター
		*/
		//-----------------------------------------------------------------//
		const wave_cap::sample_param& get_sample_param() const { return wdm_.sample_param_; }

		bool get_crm_mode() const { return crm_.mode_->get_select_pos(); }
		double get_crrd_value() const { return crrd_value_; }
		double get_crcd_value() const { return crcd_value_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  計測リクエスト
			@param[in]	mode	計測モード
			@return 処理中なら「false」
		*/
		//-----------------------------------------------------------------//
		bool request_test(test_mode mode)
		{
			if(cmd_task_ != cmd_task::idle) return false;

			test_mode_ = mode;
			switch(mode) {
			case test_mode::R_MES:
			case test_mode::C_MES:
				cmd_task_ = cmd_task::crm;
				break;
			default:
				break;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  各モジュールからの受信(DC2, CRM)
		*/
		//-----------------------------------------------------------------//
		void update_client()
		{
			// モジュールから受け取ったパラメーターをＧＵＩに反映
			static const uint32_t sample_num = 50;
			if(crm_.mode_->get_select_pos() == 0) {  // CRRD
				if(crrd_id_ != client_.get_mod_status().crrd_id_) {
					crrd_id_ = client_.get_mod_status().crrd_id_;
					uint32_t v = client_.get_mod_status().crrd_;
					v -= sample_num * 0x7FFFF;
					double a = static_cast<double>(v) / 50.0 / static_cast<double>(0x7FFFF)
						* 1.570798233;
					a *= 778.2;  // 778.2 mV P-P
					static const double itbl[3] = {  // 電流テーブル
						2.0, 20.0, 200.0
					};
					a /= itbl[crm_.amps_->get_select_pos() % 3];
					crrd_value_ = a;
					crm_.ans_->set_text((boost::format("%5.4f Ω") % a).str());
				}
			} else { // CRCD
				if(crcd_id_ != client_.get_mod_status().crcd_id_) {
					crcd_id_ = client_.get_mod_status().crcd_id_;
					uint32_t v = client_.get_mod_status().crcd_;
					v -= sample_num * 0x7FFFF;
					double a = static_cast<double>(v) / 50.0 / static_cast<double>(0x7FFFF)
						* 1.570798233;
					a *= 778.2 * 2.0;  // 778.2 mV P-P
					static const double itbl[3] = {  // 電流テーブル
						2.0, 20.0, 200.0
					};
					a /= itbl[crm_.amps_->get_select_pos() % 3];

					a = 1.0 / (2.0 * 3.141592654 * 1000.0 * a);
					a *= 1e6;
					crcd_value_ = a;
					crm_.ans_->set_text((boost::format("%5.4f uF") % a).str());
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int d_w = 970;
			int d_h = 580;
			{
				widget::param wp(vtx::irect(100, 100, d_w, d_h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [=](bool ok) {
				};
			}
			{	// 共有フレーム（プロパティシート）
				widget::param wp(vtx::irect(5, 60, d_w - 10, 350), dialog_);
				widget_sheet::param wp_;
				sheet_ = wd.add_widget<widget_sheet>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 0, 0, 0), sheet_);
				widget_null::param wp_;
				style_[0] = wd.add_widget<widget_null>(wp, wp_);
				sheet_->add("静的検査 (容量、抵抗 測定)", style_[0]);
			}
			{
				widget::param wp(vtx::irect(0, 0, 0, 0), sheet_);
				widget_null::param wp_;
				style_[1] = wd.add_widget<widget_null>(wp, wp_);
				sheet_->add("静的検査 (電圧、電流 検査)", style_[1]);
			}
			{
				widget::param wp(vtx::irect(0, 0, 0, 0), sheet_);
				widget_null::param wp_;
				style_[2] = wd.add_widget<widget_null>(wp, wp_);
				sheet_->add("動的検査（特性 検査）", style_[2]);
			}
			{
				widget::param wp(vtx::irect(0, 0, 0, 0), sheet_);
				widget_null::param wp_;
				style_[3] = wd.add_widget<widget_null>(wp, wp_);
				sheet_->add("熱抵抗測定", style_[3]);
			}

			int w = 100;
			int h = 45;
			int ofsx = w + 10;
			{  // ロード・ファイル
				widget::param wp(vtx::irect(20, 480, 100, 40), dialog_);
				widget_button::param wp_("ロード");
				load_file_ = wd.add_widget<widget_button>(wp, wp_);
				load_file_->at_local_param().select_func_ = [=](bool f) {
					std::string filter = "単体テスト(*.";
					filter += UNIT_EXT_;
					filter += ")\t*.";
					filter += UNIT_EXT_;
					filter += "\t";
					unit_load_filer_.open(filter);
				};
			}
			{  // セーブ・ファイル
				widget::param wp(vtx::irect(130, 480, 100, 40), dialog_);
				widget_button::param wp_("セーブ");
				save_file_ = wd.add_widget<widget_button>(wp, wp_);
				save_file_->at_local_param().select_func_ = [=](bool f) {
					std::string filter = "単体テスト(*.";
					filter += UNIT_EXT_;
					filter += ")\t*.";
					filter += UNIT_EXT_;
					filter += "\t";
					unit_save_filer_.open(filter, true);
				};
			}
			{  // インターロック機構、On/Off
				widget::param wp(vtx::irect(240, 480, 180, 40), dialog_);
				widget_check::param wp_("Interlock", true);
				ilock_enable_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // help message (widget_chip)
				widget::param wp(vtx::irect(0, 0, 100, 40), dialog_);
				widget_chip::param wp_;
				chip_ = wd.add_widget<widget_chip>(wp, wp_);
				chip_->active(0);
			}
			ofsx = 90;
			icm_.init(dialog_, d_w, ofsx, 15);
			ofsx -= 5;
			crm_.init(style_[0], d_w, ofsx,  30);
			dc2_.init(style_[1], d_w, ofsx,  30);
			wdm_.init(style_[2], d_w, ofsx,  40);
			dc1_.init(style_[2], d_w, ofsx, 140);
			wgm_.init(style_[2], d_w, ofsx, 240);
			ofsx = 130;
			test_param_.init(dialog_, d_w, ofsx, 420);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(!startup_init_ && client_.probe()) {
				cmd_task_ = cmd_task::init_all;
				startup_init_ = true;
			} 

			interlock_.update(ilock_enable_->get_check());

			dc2_.update();

			cmd_service_();

			if(!dialog_->get_state(gui::widget::state::ENABLE)) return;

			// 転送スイッチの状態をネットワークの接続状態で設定
			dc1_.exec_->set_stall(!client_.probe());
			dc2_.exec_->set_stall(!client_.probe());
			wgm_.exec_->set_stall(!client_.probe());
			crm_.exec_->set_stall(!client_.probe());
			icm_.exec_->set_stall(!client_.probe());
			wdm_.exec_->set_stall(!client_.probe());

			if(unit_load_filer_.state()) {
				auto path = unit_load_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += '.';
						ph += UNIT_EXT_;
					}
					sys::preference pre;
					auto ret = pre.load(path);
					if(ret) {
						load(pre);
						wave_cap_.load(pre);
					}
				}
			}
			if(unit_save_filer_.state()) {
				auto path = unit_save_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += '.';
						ph += UNIT_EXT_;
					}
					sys::preference pre;
					save(pre);
					wave_cap_.save(pre);
					pre.save(ph);
				}
			}

			// ヘルプ機能
			uint32_t act = 60 * 3;
			if(wdm_.help(chip_)) { }
			else if(dc1_.help(chip_)) { }
			else if(dc2_.help(chip_)) { }
			else if(wgm_.help(chip_)) { }
			else if(test_param_.help(chip_)) { }
			else { act = 0; }
			chip_->active(act);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
///			unit_name_->save(pre);

			dc1_.save(pre);
			dc2_.save(pre);
			wgm_.save(pre);
			crm_.save(pre);
			wdm_.save(pre);
			icm_.save(pre);
			test_param_.save(pre);

			ilock_enable_->save(pre);
			sheet_->save(pre);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path)
		{
			sys::preference pre;
			save(pre);
			return pre.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre)
		{
///			unit_name_->load(pre);

			dc1_.load(pre);
			dc2_.load(pre);
			wgm_.load(pre);
			crm_.load(pre);
			wdm_.load(pre);
			icm_.load(pre);
			test_param_.load(pre);

			ilock_enable_->load(pre);
			sheet_->load(pre);

			return true;
		}


		bool load(const std::string& path)
		{
			sys::preference pre;
			auto ret = pre.load(path);
			if(ret) {
				ret = load(pre);
			}
			return ret;
		}
	};
}
