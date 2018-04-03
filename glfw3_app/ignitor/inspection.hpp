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
#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "wave_cap.hpp"
#include "test.hpp"
#include "kikusui.hpp"

#include "icm.hpp"
#include "wdm.hpp"
#include "dc1.hpp"
#include "dc2.hpp"
#include "wgm.hpp"
#include "crm.hpp"
#include "thr.hpp"
#include "dif.hpp"
#include "test_param.hpp"

#include "project.hpp"

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
			C_MES,		///< 容量計測（CRM/静特性）
			R_MES,		///< 抵抗計測（CRM/静特性）
			V_MES,		///< 電圧計測（DC2/静特性）
			I_MES,		///< 電流計測（DC2/静特性）
			THR,		///< 熱抵抗計測（静特性）
			WD1,		///< 波形計測/DC1（動特性）
			DIF,		///< 差分計算
			WD2,		///< 波形計測/DC2（動特性）
		};

	private:
		utils::director<core>&	director_;

		net::ign_client_tcp&	client_;

		interlock&				interlock_;
		wave_cap&				wave_cap_;
		kikusui&				kikusui_;
		project&				project_;

		gui::widget_dialog*		dialog_;
		gui::widget_button*		load_file_;			///< load file
		gui::widget_button*		save_file_;			///< save file
		gui::widget_check*		ilock_enable_;		///< Interlock 許可／不許可
		gui::widget_sheet*		sheet_;
		gui::widget_null*		style_[6];
		utils::select_file		unit_load_filer_;
		utils::select_file		unit_save_filer_;

		dc1						dc1_;
		dc2						dc2_;
		wgm						wgm_;
		crm						crm_;
		icm						icm_;
		wdm						wdm_;
		thr						thr_;
		dif						dif_;

		// 測定用件
		test_param				test_param_;

		gui::widget_chip*		chip_;			///< help chip
		gui::widget_button*		all_off_;		///< All SW OFF

		bool					startup_init_;
		bool					dc1_all_ena_;
		uint32_t				d2md_id_;

		uint32_t				wdm_exec_id_;

		test_mode				test_mode_;

		icm						icm__;
		dc2						dc2__;
		wgm						wgm__;
		wdm						wdm__;

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

			wdm_dc1,	///< WDM/DC1 開始
			wdm_dc1_0,
			wdm_dc1_1,
			wdm_dc1_2,

			dc2,		///< DC2 開始

			thr,		///< THR 開始

			wdm_dc2,	///< WDM/DC2 開始
			wdm_dc2_0,
			wdm_dc2_1,
			wdm_dc2_2,

			off_all,	///< 全てのスイッチ、オフ
			off_all0,
			off_all1,
			off_all2,
			off_all3,
			off_all4,

			off_crm,	///< CRM スイッチ、オフ (CRM)

			off_dc2,	///< DC2 スイッチ、オフ (DC2)

			off_wdm_dc1,	///< WDM/DC1 スイッチ、オフ (DC1, WDM, WGM, ICM)
			off_wdm_dc1_0,
			off_wdm_dc1_1,
			off_wdm_dc1_2,

			off_thr,	///< THR 関係 OFF (DC1, WDM)

			off_wdm_dc2,	///< WDM/DC2 スイッチ、オフ (DC2, WDM, WGM, ICM)
			off_wdm_dc2_0,
			off_wdm_dc2_1,
			off_wdm_dc2_2,
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

			// CRM
			case cmd_task::crm:
				crm_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;

			// DC2
			case cmd_task::dc2:
				dc2_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;

			// WDM
			case cmd_task::wdm_dc1:
				icm_.exec_->exec();
				cmd_task_ = cmd_task::wdm_dc1_0;
				break;
			case cmd_task::wdm_dc1_0:
				dc1_.exec_->exec();
				cmd_task_ = cmd_task::wdm_dc1_1;
				break;
			case cmd_task::wdm_dc1_1:
				wgm_.exec_->exec();
				cmd_task_ = cmd_task::wdm_dc1_2;
				break;
			case cmd_task::wdm_dc1_2:
				wdm_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;

			// THR
			case cmd_task::thr:
				thr_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;


			case cmd_task::off_all:
				wdm_.startup();
				cmd_task_ = cmd_task::off_all0;
				break;
			case cmd_task::off_all0:
				wgm_.startup();
				cmd_task_ = cmd_task::off_all1;
				break;
			case cmd_task::off_all1:
				dc1_.startup();
				cmd_task_ = cmd_task::off_all2;
				break;
			case cmd_task::off_all2:
				dc2_.startup();
				cmd_task_ = cmd_task::off_all3;
				break;
			case cmd_task::off_all3:
				crm_.startup();
				cmd_task_ = cmd_task::off_all4;
				break;
			case cmd_task::off_all4:
				icm_.startup();
				cmd_task_ = cmd_task::idle;
				break;


			// CRM のオフ
			case cmd_task::off_crm:
				crm_.startup();
				cmd_task_ = cmd_task::idle;
				break;


			// DC2 のオフ
			case cmd_task::off_dc2:
				dc2_.startup();
				cmd_task_ = cmd_task::idle;
				break;

			// WDM/DC1 のオフ
			case cmd_task::off_wdm_dc1:
				wgm_.startup();
				cmd_task_ = cmd_task::off_wdm_dc1_0;
				break;
			case cmd_task::off_wdm_dc1_0:
				dc1_.startup();
				cmd_task_ = cmd_task::off_wdm_dc1_1;
				break;
			case cmd_task::off_wdm_dc1_1:
				wdm_.startup();
				cmd_task_ = cmd_task::off_wdm_dc1_2;
				break;
			case cmd_task::off_wdm_dc1_2:
				icm_.startup();
				cmd_task_ = cmd_task::idle;
				break;

			// WDM/DC2 のオフ
			case cmd_task::off_wdm_dc2:
				wgm_.startup();
				cmd_task_ = cmd_task::off_wdm_dc2_0;
				break;
			case cmd_task::off_wdm_dc2_0:
				dc2_.startup();
				cmd_task_ = cmd_task::off_wdm_dc2_1;
				break;
			case cmd_task::off_wdm_dc2_1:
				wdm_.startup();
				cmd_task_ = cmd_task::off_wdm_dc2_2;
				break;
			case cmd_task::off_wdm_dc2_2:
				icm_.startup();
				cmd_task_ = cmd_task::idle;
				break;

			case cmd_task::off_thr:
				thr_.startup();
				cmd_task_ = cmd_task::idle;
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
		inspection(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilock, wave_cap& wc, kikusui& kik, project& proj) :
			director_(d), client_(client), interlock_(ilock), wave_cap_(wc), kikusui_(kik),
			project_(proj),
			dialog_(nullptr),
			load_file_(nullptr), save_file_(nullptr), ilock_enable_(nullptr),
			sheet_(nullptr), style_{ nullptr },

			dc1_(d, client_, interlock_),
			dc2_(d, client_, interlock_, kikusui_),
			wgm_(d, client_, interlock_),
			crm_(d, client_, interlock_),
			icm_(d, client_, interlock_),
			wdm_(d, client_, interlock_),
			thr_(d, client_, interlock_, kikusui_, wave_cap_),
			dif_(d, project_.at_csv1()),
			test_param_(d),

			chip_(nullptr), all_off_(nullptr),

			startup_init_(false), dc1_all_ena_(false),
			d2md_id_(0),
			wdm_exec_id_(0),
			test_mode_(test_mode::NONE),
			icm__(d, client_, interlock_),
			dc2__(d, client_, interlock_, kikusui_),
			wgm__(d, client_, interlock_),
			wdm__(d, client_, interlock_),
			cmd_task_(cmd_task::idle)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  CRM の参照 (const)
			@return CRM
		*/
		//-----------------------------------------------------------------//
		const crm& get_crm() const { return crm_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief  DC2 の参照 (const)
			@return DC2
		*/
		//-----------------------------------------------------------------//
		const dc2& get_dc2() const { return dc2_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief  THR の参照 (const)
			@return THR
		*/
		//-----------------------------------------------------------------//
		const thr& get_thr() const { return thr_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief  DIF の参照 (const)
			@return DIF
		*/
		//-----------------------------------------------------------------//
		const dif& get_dif() const { return dif_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief  全オフライン
		*/
		//-----------------------------------------------------------------//
		void offline_all()
		{
			cmd_task_ = cmd_task::off_all;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  モード別、オフライン
		*/
		//-----------------------------------------------------------------//
		void offline()
		{
			switch(get_test_mode()) {
			case test_mode::R_MES:
			case test_mode::C_MES:
				cmd_task_ = cmd_task::off_crm;
				break;

			case test_mode::I_MES:
			case test_mode::V_MES:
				cmd_task_ = cmd_task::off_dc2;
				break;

			case test_mode::WD1:
				cmd_task_ = cmd_task::off_wdm_dc1;
				break;

			case test_mode::THR:
				cmd_task_ = cmd_task::off_thr;
				break;

			case test_mode::WD2:
				cmd_task_ = cmd_task::off_wdm_dc2;
				break;

			default:
				cmd_task_ = cmd_task::off_all;
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  タスク状態取得
			@return タスク動作中なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_task_busy() const {
			return cmd_task_ != cmd_task::idle;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計測モードの取得
			@return 計測モード
		*/
		//-----------------------------------------------------------------//
		test_mode get_test_mode() const
		{
			if(sheet_->get_select_pos() == 0) {  // CR (静特性）
				if(crm_.mode_->get_select_pos() == 0) {
					return test_mode::R_MES;
				} else {
					return test_mode::C_MES;
				}
			} else if(sheet_->get_select_pos() == 1) {  // DC2 (静特性）
				if(dc2_.mode_->get_select_pos() == 0) {  // 電流設定、電圧取得
					return test_mode::V_MES;
				} else {  // 電圧設定、電流取得
					return test_mode::I_MES;
				}
			} else if(sheet_->get_select_pos() == 2) {  // WDM/DC1 (動特性）
				return test_mode::WD1;
			} else if(sheet_->get_select_pos() == 3) {  // THR (熱抵抗)
				return test_mode::THR;
			} else if(sheet_->get_select_pos() == 4) {  // DIF（差分）
				return test_mode::DIF;
			} else if(sheet_->get_select_pos() == 5) {  // WDM/DC2 (動特性）
				return test_mode::WD2;
			}
			return test_mode::NONE;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計測単位の取得
			@return 計測単位
		*/
		//-----------------------------------------------------------------//
		std::string get_unit_str() const {
			if(sheet_->get_select_pos() == 0) {  // CRM
				return crm_.get_unit_str();
			} else if(sheet_->get_select_pos() == 1) {  // DC2
				return dc2_.get_unit_str();
			} else if(sheet_->get_select_pos() == 2) {  // WDM/DC1
				return wave_cap_.get_unit_str();
			} else if(sheet_->get_select_pos() == 3) {  // THR 熱抵抗
				return thr_.get_unit_str();
			} else if(sheet_->get_select_pos() == 4) {  // DIF
				return dif_.get_unit_str();
			} else if(sheet_->get_select_pos() == 5) {  // WDM/DC2
				return wave_cap_.get_unit_str();
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

			case test_mode::V_MES:
			case test_mode::I_MES:
				cmd_task_ = cmd_task::dc2;
				break;

			case test_mode::WD1:
				cmd_task_ = cmd_task::wdm_dc1;
				break;

			case test_mode::WD2:
				cmd_task_ = cmd_task::wdm_dc2;
				break;

			case test_mode::THR:
				cmd_task_ = cmd_task::thr;
				break;

			default:
				break;
			}
			return true;
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
				widget::param wp(vtx::irect(5, 10, d_w - 10, 400), dialog_);
				widget_sheet::param wp_;
				sheet_ = wd.add_widget<widget_sheet>(wp, wp_);
				sheet_->at_local_param().select_func_ = [=](uint32_t newidx, uint32_t id) {
					bool c0 = false;
					bool c1 = false;
					bool c2 = false;
					bool c3 = false;
					bool c5 = false;
					switch(newidx) {
					case 0:
						c0 = true;
						break;
					case 1:
						c1 = true;
						break;
					case 2:
						c2 = true;
						break;
					case 3:
						c3 = true;
						break;
					case 4:
						dif_.set_index(project_.get_csv_index());
						break;
					case 5:
						c5 = true;
						break;
					default:
						break;
					}
					if(!c0) {
						crm_.all_->set_check(c0);
						crm_.all_->exec();
					}
					if(!c1) {
						dc2_.all_->set_check(c1);
						dc2_.all_->exec();
					}
					if(!c2) {
						icm_.all_->set_check(c2);
						icm_.all_->exec();
						wgm_.all_->set_check(c2);
						wgm_.all_->exec();
						dc1_.all_->set_check(c2);
						dc1_.all_->exec();
					}
					if(!c3) {
						thr_.all_->set_check(c3);
						thr_.all_->exec();
					}
					if(!c5) {
						icm__.all_->set_check(c5);
						icm__.all_->exec();
						wgm__.all_->set_check(c5);
						wgm__.all_->exec();
						dc2__.all_->set_check(c5);
						dc2__.all_->exec();
					}
				};
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
			{
				widget::param wp(vtx::irect(0, 0, 0, 0), sheet_);
				widget_null::param wp_;
				style_[4] = wd.add_widget<widget_null>(wp, wp_);
				sheet_->add("検査差分", style_[4]);
			}
			{
				widget::param wp(vtx::irect(0, 0, 0, 0), sheet_);
				widget_null::param wp_;
				style_[5] = wd.add_widget<widget_null>(wp, wp_);
				sheet_->add("高動的検査", style_[5]);
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
			{  // All SW OFF
				widget::param wp(vtx::irect(d_w - 120, 480, 100, 40), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_3;
				widget_button::param wp_("All OFF");
				all_off_ = wd.add_widget<widget_button>(wp, wp_);
				all_off_->at_local_param().select_func_ = [=](bool f) {
					icm_.reset_sw();
					crm_.reset_sw();
					dc2_.reset_sw();
					dc1_.reset_sw();
					wgm_.reset_sw();
					crm_.reset_sw();
					icm__.reset_sw();
					dc2__.reset_sw();
					wgm__.reset_sw();
				};
			}
			ofsx = 90 - 5;
			icm_.init(style_[2], d_w, ofsx,  30);
			crm_.init(style_[0], d_w, ofsx,  30);
			dc2_.init(style_[1], d_w, ofsx,  30);
			wdm_.init(style_[2], d_w, ofsx,  40 + 40);
			dc1_.init(style_[2], d_w, ofsx, 140 + 40);
			wgm_.init(style_[2], d_w, ofsx, 240 + 40);
			thr_.init(style_[3], d_w, ofsx,  40);
			dif_.init(style_[4], d_w, ofsx,  40);

			icm__.init(style_[5], d_w, ofsx,  30,      gui::widget::PRE_GROUP::_5);
			wdm__.init(style_[5], d_w, ofsx,  40 + 40, gui::widget::PRE_GROUP::_5);
			dc2__.init(style_[5], d_w, ofsx, 140 + 40, gui::widget::PRE_GROUP::_5);
			wgm__.init(style_[5], d_w, ofsx, 240 + 40, gui::widget::PRE_GROUP::_5);

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

			// WAVE_CAP 側スイッチの転送ボタン検出
			if(wdm_exec_id_ != wave_cap_.get_exec_button()->get_local_param().id_) {
				wdm_exec_id_ = wave_cap_.get_exec_button()->get_local_param().id_;
				if(sheet_->get_select_pos() == 2) {
					cmd_task_ = cmd_task::wdm_dc1;
				} else if(sheet_->get_select_pos() == 5) {
					cmd_task_ = cmd_task::wdm_dc2;
				}
			}

			cmd_service_();

			// 転送スイッチの状態をネットワークの接続状態で設定
			icm_.update();
			dc1_.update();
			{  // DC1 が ALLOFF の場合、WGM も ALLOFF にする。
				bool all = dc1_.all_->get_check();
				if(dc1_all_ena_ && !all) {
					if(wgm_.all_->get_check()) {
						wgm_.all_->set_check(false);
					}
				}
				dc1_all_ena_ = all;
			}
			dc2_.update();
			wgm_.update();
			crm_.update();
			wdm_.update();
			thr_.update();

			icm__.update();
			dc2__.update();
			wgm__.update();
			wdm__.update();

			if(!dialog_->get_state(gui::widget::state::ENABLE)) return;

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
			else if(crm_.help(chip_)) { }
			else if(wgm_.help(chip_)) { }
			else if(thr_.help(chip_)) { }
			else if(dif_.help(chip_)) { }
			else if(wdm__.help(chip_)) { }
			else if(dc2__.help(chip_)) { }
			else if(wgm__.help(chip_)) { }
			else if(test_param_.help(chip_)) { }
			else { act = 0; }
			chip_->active(act);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save_sys(sys::preference& pre)
		{
			dialog_->save(pre);
			crm_.save_sys(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load_sys(sys::preference& pre)
		{
			dialog_->load(pre);
			crm_.load_sys(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
///			unit_name_->save(pre);

			icm_.save(pre);
			dc1_.save(pre);
			dc2_.save(pre);
			wgm_.save(pre);
			crm_.save(pre);
			wdm_.save(pre);
			thr_.save(pre);
			dif_.save(pre);
			test_param_.save(pre);

			icm__.save(pre);
			dc2__.save(pre);
			wgm__.save(pre);
			wdm__.save(pre);

			wave_cap_.save(pre);

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
			@param[in]	pre	プリファレンス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre)
		{
///			unit_name_->load(pre);

			icm_.load(pre);
			dc1_.load(pre);
			dc2_.load(pre);
			wgm_.load(pre);
			crm_.load(pre);
			wdm_.load(pre);
			thr_.load(pre);
			dif_.load(pre);
			test_param_.load(pre);

			icm__.load(pre);
			dc2__.load(pre);
			wgm__.load(pre);
			wdm__.load(pre);

			wave_cap_.load(pre);

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
