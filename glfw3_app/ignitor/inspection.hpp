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
			C_MES,		///< 容量計測（CRM/静特性）
			R_MES,		///< 抵抗計測（CRM/静特性）
			V_MES,		///< 電圧計測（DC2/静特性）
			I_MES,		///< 電流計測（DC2/静特性）
			THR,		///< 熱抵抗計測（静特性）
			WD,			///< 波形計測（動特性）
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
		thr						thr_;

		// 測定用件
		test_param				test_param_;

		gui::widget_chip*		chip_;			///< help chip

		bool					startup_init_;
		bool					dc1_all_ena_;
		uint32_t				d2md_id_;

		uint32_t				wdm_exec_id_;

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

			wdm,		///< WDM 開始
			wdm0,
			wdm1,
			wdm2,

			dc2,		///< DC2 開始

			off_all,	///< 全てのスイッチ、オフ
			off_all0,
			off_all1,
			off_all2,
			off_all3,
			off_all4,

			off_crm,	///< CRM スイッチ、オフ (CRM)

			off_dc2,	///< DC2 スイッチ、オフ (DC2)

			off_wdm,	///< WDM スイッチ、オフ (DC1, WDM, WGM, ICM)
			off_wdm0,
			off_wdm1,
			off_wdm2,

			off_thr,	///< THR 関係 OFF (DC1, WDM)
			off_thr0,
			off_thr1,

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
				crm_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;


			case cmd_task::dc2:
				dc2_.exec_->exec();
				cmd_task_ = cmd_task::idle;
				break;


			case cmd_task::wdm:
				icm_.exec_->exec();
				cmd_task_ = cmd_task::wdm0;
				break;
			case cmd_task::wdm0:
				dc1_.exec_->exec();
				cmd_task_ = cmd_task::wdm1;
				break;
			case cmd_task::wdm1:
				wgm_.exec_->exec();
				cmd_task_ = cmd_task::wdm2;
				break;
			case cmd_task::wdm2:
				wdm_.exec_->exec();
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


			// WDM のオフ（ICM はそのまま）
			case cmd_task::off_wdm:
				wgm_.startup();
				cmd_task_ = cmd_task::off_wdm0;
				break;
			case cmd_task::off_wdm0:
				dc1_.startup();
				cmd_task_ = cmd_task::off_wdm1;
				break;
			case cmd_task::off_wdm1:
				wdm_.startup();
				cmd_task_ = cmd_task::off_wdm2;
				break;
			case cmd_task::off_wdm2:
				icm_.startup();
				cmd_task_ = cmd_task::idle;
				break;

			case cmd_task::off_thr:
				dc1_.startup();
				cmd_task_ = cmd_task::off_thr0;
				break;
			case cmd_task::off_thr0:
				wgm_.startup();
				cmd_task_ = cmd_task::off_thr1;
				break;
			case cmd_task::off_thr1:
				wdm_.startup();
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
			thr_(d, client_, interlock_, kikusui_, wave_cap_),
			test_param_(d),

			chip_(nullptr),

			startup_init_(false), dc1_all_ena_(false),
			d2md_id_(0),
			wdm_exec_id_(0),
			test_mode_(test_mode::NONE), cmd_task_(cmd_task::idle)
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

			case test_mode::WD:
				cmd_task_ = cmd_task::off_wdm;
				break;

			case test_mode::THR:
				cmd_task_ = cmd_task::off_thr;
				break;

			default:
				cmd_task_ = cmd_task::off_all;
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計測モードの取得
			@return 計測モード
		*/
		//-----------------------------------------------------------------//
		test_mode get_test_mode() const {
			if(sheet_->get_select_pos() == 0) {  // CR (静特性）
				if(crm_.mode_->get_select_pos() == 0) {
					return test_mode::R_MES;
				} else {
					return test_mode::C_MES;
				}
			} else if(sheet_->get_select_pos() == 1) {  // DC2 (静特性）
				if(dc2_.mode_->get_select_pos() == 0) {
					return test_mode::I_MES;
				} else {
					return test_mode::V_MES;
				}
			} else if(sheet_->get_select_pos() == 2) {  // WDM (動特性）
				return test_mode::WD;
			} else {  // THR (熱抵抗)
				return test_mode::THR;
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
			} else if(sheet_->get_select_pos() == 2) {  // WDM
				return wave_cap_.get_unit_str();
			} else {  // THR 熱抵抗
				return thr_.get_unit_str();
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


		uint32_t get_dc2_id() const { return dc2_.id_; }
		bool get_dc2_mode() const { return dc2_.mode_->get_select_pos(); }
		double get_volt_value() const { return dc2_.volt_value_; }
		double get_curr_value() const { return dc2_.curr_value_; }


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

			case test_mode::WD:
				cmd_task_ = cmd_task::wdm;
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
			ofsx = 90 - 5;
			icm_.init(style_[2], d_w, ofsx,  30);
			crm_.init(style_[0], d_w, ofsx,  30);
			dc2_.init(style_[1], d_w, ofsx,  30);
			wdm_.init(style_[2], d_w, ofsx,  40 + 40);
			dc1_.init(style_[2], d_w, ofsx, 140 + 40);
			wgm_.init(style_[2], d_w, ofsx, 240 + 40);
			thr_.init(style_[3], d_w, ofsx,  40);
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

			if(wdm_exec_id_ != wave_cap_.get_exec_button()->get_local_param().id_) {
				wdm_exec_id_ = wave_cap_.get_exec_button()->get_local_param().id_;
				cmd_task_ = cmd_task::wdm;				
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
