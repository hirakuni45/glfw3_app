#pragma once
//=====================================================================//
/*! @file
    @brief  CRM クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <algorithm>
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
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "tools.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  CRM クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class crm
	{
		static const uint32_t DUMMY_LOOP_COUNT = 4;  // ダミーループ

		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;

	public:
		// CRM 設定
		gui::widget_check*		sw_[14];	///< CRM 接続スイッチ
		gui::widget_check*		ena_;		///< CRM 有効、無効
		gui::widget_list*		amps_;		///< CRM 電流レンジ切り替え
		gui::widget_list*		freq_;		///< CRM 周波数（100Hz, 1KHz, 10KHz）
		gui::widget_list*		mode_;		///< CRM 抵抗測定、容量測定
		gui::widget_label*		ans_;		///< CRM 測定結果 
		gui::widget_button*		exec_;		///< CRM 設定転送
		gui::widget_check*		all_;		///< CRM 全体
		gui::widget_button*		run_refs_;	///< CRM 公正用ボタン
		gui::widget_check*		ena_refs_;
		gui::widget_text*		reg_refs_[5];

		gui::widget_dialog*		dialog_;
		gui::widget_dialog*		info_;

		gui::widget_label*		net_regs_;		///< CRM 合成抵抗設定

		gui::widget_list*		carib_type_;
		gui::widget_text*		carib_data_[4 * 3];

		double					crrd_value_;
		double					crcd_value_;

	private:
		uint32_t				crdd_id_;
		int32_t					crdd_val_;
		uint32_t				crrd_id_;
		int32_t					crrd_val_;
		uint32_t				crcd_id_;
		int32_t					crcd_val_;

		enum class carib_task {
			idle,
			C0,		///< 100Hz 0.2mA
			C1,		///< 100Hz 2.0mA
			C2,		///< 100Hz 20mA
			C3,		///< 100Hz 200mA
			C4,
			C5,
			C6,
			C7,
			C8,
			C9,
			C10,
			C11,
			REG,	///< 0 ohm caribrations
		};
		carib_task				carib_task_;

		std::string				last_cmd_;

		uint32_t				crm_id_;

		uint32_t				dummy_loop_;

		struct crm_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			uint16_t	amps;	///< 0, 1, 2, 3
			uint16_t	freq;	///< 0, 1, 2

			crm_t() : sw(0), ena(0), amps(0), freq(0) { }

			std::string build() const
			{
				std::string s;
				s = (boost::format("crm CRSW%04X\n") % sw).str();
				if(ena) {
					s += (boost::format("crm CRIS%d\n") % amps).str();
					static const char* frqtbl[3] = { "001", "010", "100" };
					s += (boost::format("crm CRFQ%s\n") % frqtbl[freq % 3]).str();
				}
				s += (boost::format("crm CROE%d\n") % ena).str();
				if(ena) {
					s += "delay 2\n";
					s += (boost::format("crm CRD?1\n")).str();
					s += "delay 1\n";
					s += (boost::format("crm CRR?1\n")).str();
					s += "delay 1\n";
					s += (boost::format("crm CRC?1\n")).str();
				}
				return s;
			}
		};


		std::string make_carib_param_(uint32_t amps)
		{
			crm_t t;
			uint16_t sw = 0;
			for(int i = 0; i < 14; ++i) {
				sw <<= 1;
				if(sw_[i]->get_check()) sw |= 1;
			}
			t.sw = sw;
			t.ena = 1;
			t.amps = amps;
			t.freq = freq_->get_select_pos();
			return t.build();
		}

		std::string make_crm_param_()
		{
			crm_t t;
			uint16_t sw = 0;
			for(int i = 0; i < 14; ++i) {
				sw <<= 1;
				if(sw_[i]->get_check()) sw |= 1;
			}
			t.sw = sw;
			t.ena = ena_->get_check();
			t.amps = amps_->get_select_pos();
			t.freq = freq_->get_select_pos();
			return t.build();
		}


		bool calc_regs_(double& ans)
		{
			int32_t v = crrd_val_;
			int32_t ofs = crdd_val_;
			v -= ofs;
			double lim = static_cast<double>(ofs) / 1.570798233;
			if(v >= lim) {
				ans = 10e6;
				return false;
			}
			double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
			a *= 778.2;  // 778.2 mV P-P
			static const double itbl[4] = {  // 電流テーブル
				0.2, 2.0, 20.0, 200.0
			};
			a /= itbl[amps_->get_select_pos()];
			ans = a;
			return true;
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		crm(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc) :
			director_(d), client_(client), interlock_(ilc),
			sw_{ nullptr },
			ena_(nullptr), amps_(nullptr), freq_(nullptr), mode_(nullptr),
			ans_(nullptr), exec_(nullptr), all_(nullptr),
			run_refs_(nullptr), ena_refs_(nullptr), reg_refs_{ nullptr },
			dialog_(nullptr), info_(nullptr), net_regs_(nullptr),
			carib_type_(nullptr), carib_data_{ nullptr },
			crrd_value_(0.0), crcd_value_(0.0),
			crdd_id_(0), crdd_val_(0), crrd_id_(0), crrd_val_(0), crcd_id_(0), crcd_val_(0),
			carib_task_(carib_task::idle), last_cmd_(),
			crm_id_(0), dummy_loop_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  値の更新 ID 取得
			@return 値の更新 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_id() const { return crm_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  測定単位文字列の取得
			@return 測定単位文字列
		*/
		//-----------------------------------------------------------------//
		std::string get_unit_str() const {
			if(mode_->get_select_pos() == 0) {  // 抵抗
				return "Ohm";
			} else {  // 容量
				return "uF";
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  リセット・スイッチ
		*/
		//-----------------------------------------------------------------//
		void reset_sw()
		{
			for(uint32_t i = 0; i < 14; ++i) {
				sw_[i]->set_check(false);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
			crm_t t;
			client_.send_data(t.build());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
			@param[in]	root	ルート
			@param[in]	d_w		横幅最大
			@param[in]	ofsx	オフセット X
			@param[in]	ofsy	オフセット Y
		*/
		//-----------------------------------------------------------------//
		void init(gui::widget* root, int d_w, int ofsx, int ofsy)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{
				widget::param wp(vtx::irect(15, ofsy, 60, 40), root);
				widget_text::param wp_("CRM:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			tools::init_sw(wd, root, interlock_, ofsx, ofsy, sw_, 14, 1);
			ofsy += 50;
			// ＣＲメジャー・モジュール
			{
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_check::param wp_("有効");
				ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 電流レンジ切り替え（2mA, 20mA, 200mA）
				widget::param wp(vtx::irect(ofsx + 90, ofsy, 110, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("0.2 mA");
				wp_.init_list_.push_back("  2 mA");
				wp_.init_list_.push_back(" 20 mA");
				wp_.init_list_.push_back("200 mA");
				amps_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(ofsx + 220, ofsy, 110, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back(" 1 KHz");
				wp_.init_list_.push_back("10 KHz");
				freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 抵抗値、容量値選択
				widget::param wp(vtx::irect(ofsx + 350, ofsy, 120, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("抵抗測定");
				wp_.init_list_.push_back("容量測定");
				wp_.init_list_.push_back("合成測定");
				mode_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 500, ofsy, 200, 40), root);
				widget_label::param wp_("");
				ans_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					auto s = make_crm_param_();
					crdd_id_ = client_.get_mod_status().crdd_id_;
					crrd_id_ = client_.get_mod_status().crrd_id_;
					crcd_id_ = client_.get_mod_status().crcd_id_;
					client_.send_data(s);
					last_cmd_ = s;
					dummy_loop_ = DUMMY_LOOP_COUNT;
					ans_->set_text("");
					net_regs_->set_text("");
				};
			}
			{
				widget::param wp(vtx::irect(d_w - 45, ofsy, 30, 30), root);
				widget_check::param wp_;
				all_ = wd.add_widget<widget_check>(wp, wp_);
				all_->at_local_param().select_func_ = [=](bool ena) {
					if(!ena) {
						tools::set_checks(sw_, false, 14);
						startup();						
					}
					for(uint32_t i = 0; i < 14; ++i) {
						sw_[i]->set_stall(!ena, widget::STALL_GROUP::_1);
					}
					ena_->set_stall(!ena, widget::STALL_GROUP::_1);
					amps_->set_stall(!ena, widget::STALL_GROUP::_1);
					freq_->set_stall(!ena, widget::STALL_GROUP::_1);
					mode_->set_stall(!ena, widget::STALL_GROUP::_1);
					exec_->set_stall(!ena, widget::STALL_GROUP::_1);
					run_refs_->set_stall(!ena, widget::STALL_GROUP::_1);
					ena_refs_->set_stall(!ena, widget::STALL_GROUP::_1);
					carib_type_->set_stall(!ena, widget::STALL_GROUP::_1);
					ans_->set_text("");
					net_regs_->set_text("");
					last_cmd_.clear();
				};
			}
			{  // 校正ボタン
				widget::param wp(vtx::irect(ofsx + 180, ofsy + 50, 100, 40), root);
				widget_button::param wp_("校正");
				run_refs_ = wd.add_widget<widget_button>(wp, wp_);
				run_refs_->at_local_param().select_func_ = [=](uint32_t id) {
					auto n = carib_type_->get_select_pos();
					if(n >= 4) {  // 200mA
						n = 3;
						carib_task_ = carib_task::REG;
					} else {
						uint32_t ct = freq_->get_select_pos() * 4 + carib_type_->get_select_pos();
						carib_task_ = static_cast<carib_task>(ct + 1);
					}
					auto s = make_carib_param_(n);
					crdd_id_ = client_.get_mod_status().crdd_id_;
					crrd_id_ = client_.get_mod_status().crrd_id_;
					crcd_id_ = client_.get_mod_status().crcd_id_;
					client_.send_data(s);
					last_cmd_ = s;
					dummy_loop_ = DUMMY_LOOP_COUNT;					
				};
			}
			{  // 校正データ、有効／無効
				widget::param wp(vtx::irect(ofsx + 290, ofsy + 50, 200, 40), root);
				widget_check::param wp_("200mA/0Ohm");
				ena_refs_ = wd.add_widget<widget_check>(wp, wp_);
			}
			for(uint32_t i = 0; i < 5; ++i) {  // 校正データ表示
				widget::param wp(vtx::irect(ofsx + i * 140, ofsy + 250, 120, 40), root);
				widget_text::param wp_;
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				reg_refs_[i] = wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 校正ダイアログ
				widget::param wp(vtx::irect(100, 100, 500, 300));
				widget_dialog::param wp_(widget_dialog::style::CANCEL_OK);
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
			}
			{  // 校正情報
				widget::param wp(vtx::irect(100, 100, 500, 300));
				widget_dialog::param wp_(widget_dialog::style::NONE);
				info_ = wd.add_widget<widget_dialog>(wp, wp_);
				info_->enable(false);
			}
			{  // 合成回路、抵抗設定
				widget::param wp(vtx::irect(ofsx + 500, ofsy + 50, 200, 40), root);
				widget_label::param wp_;
				net_regs_ = wd.add_widget<widget_label>(wp, wp_);
			}

			{  // キャリブレーション・リスト
				widget::param wp(vtx::irect(ofsx, ofsy + 50, 170, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_5;
				widget_list::param wp_;
				wp_.init_list_.push_back("0.2mA/3.3K");
				wp_.init_list_.push_back("2mA/330");
				wp_.init_list_.push_back("20mA/33");
				wp_.init_list_.push_back("200mA/3.3");
				wp_.init_list_.push_back("200mA/0");
				carib_type_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			// キャリブレーション・データ（進角）
			for(uint32_t i = 0; i < 12; ++i) {
				uint32_t x = i % 4;
				uint32_t y = i / 4;
				widget::param wp(vtx::irect(ofsx + x * 200, ofsy + 100 + y * 50, 190, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_5;
				widget_text::param wp_("0");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				carib_data_[i] = wd.add_widget<widget_text>(wp, wp_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			exec_->set_stall(!client_.probe());

			// モジュールから受け取ったパラメーターをＧＵＩに反映
			static const uint32_t sample_num = 50;

			bool trg = false;
			if(crdd_id_ != client_.get_mod_status().crdd_id_) {
				crdd_id_ = client_.get_mod_status().crdd_id_;
				crdd_val_ = client_.get_mod_status().crdd_;
// utils::format("CRDD: %08X\n") % crdd_val_;
			}
			if(crrd_id_ != client_.get_mod_status().crrd_id_) {
				crrd_id_ = client_.get_mod_status().crrd_id_;
				crrd_val_ = client_.get_mod_status().crrd_;
// utils::format("CRRD: %08X\n") % crrd_val_;
			}
			if(crcd_id_ != client_.get_mod_status().crcd_id_) {
				crcd_id_ = client_.get_mod_status().crcd_id_;
				crcd_val_ = client_.get_mod_status().crcd_;
// utils::format("CRCD: %08X\n\n") % crcd_val_;
				trg = true;
			}

			if(!trg) {
				return;
			} 
			if(dummy_loop_ > 0) {
				--dummy_loop_;
				client_.send_data(last_cmd_);				
				return;
			}

			// キャリブレーション設定
			if(carib_task_ != carib_task::idle) {
				uint32_t n = static_cast<uint32_t>(carib_task_) - 1;
				if(n < 12) {
					double CRP = static_cast<double>(crrd_val_ - crdd_val_);
					double SIP = static_cast<double>(crcd_val_ - crdd_val_);
					std::string s = (boost::format("%6.5f") % (SIP / CRP)).str();
					carib_data_[n]->set_text(s);
				} else {  // 0 ohm, internal register
					double a;
					if(calc_regs_(a)) {
						reg_refs_[0]->set_text((boost::format("%6.5f") % a).str());
					}
				}
				carib_task_ = carib_task::idle;
				return;
			}

			if(mode_->get_select_pos() == 0) {  // CRRD
				double a = 0.0;
				auto ret = calc_regs_(a);
				if(ena_refs_->get_check() && amps_->get_select_pos() == 3) {
					float ofs = 0.0f;
					utils::input("%f", reg_refs_[0]->get_text().c_str()) % ofs;
					a -= ofs;
				}
				crrd_value_ = a;
				if(ret) {
					ans_->set_text((boost::format("%6.5f Ω") % crrd_value_).str());
				} else {
					ans_->set_text("OVF");
				}
				++crm_id_;
			} else { // CRCD (1, 2)
				int32_t v = crcd_val_;
				int32_t ofs = sample_num * 0x7FFFF;
				v -= ofs;
				double lim = static_cast<double>(ofs) / 1.570798233;
				if(v >= lim) {
					ans_->set_text("OVF");
					crcd_value_ = 10e3;
					++crm_id_;
					return;
				}
				double SRP = static_cast<double>(crrd_val_ - crdd_val_);
//				termcore_->output((boost::format("SRP: %6.5f\n") % SRP).str());
				double SIP = static_cast<double>(crcd_val_ - crdd_val_);
//				termcore_->output((boost::format("SIP: %6.5f\n") % SIP).str());
//			CRP = SRP + A_CAL * SIP  //A_CALの値は、純抵抗を測定した時（位相校正操作時）の
//        	CIP = SIP - A_CAL * SRP  //(SIP/SRP)の値（位相補正係数）を使用します。（後述）
				double A_CAL = 0.0;
				{
					uint32_t n = freq_->get_select_pos() * 4 + amps_->get_select_pos();
					auto s = carib_data_[n]->get_text(); 
					float a;
					utils::input("%f", s.c_str()) % a;
					A_CAL = a;
				}
				double CRP = SRP + (A_CAL * SIP);
//				termcore_->output((boost::format("CRP: %6.5f\n") % CRP).str());
				double CIP = SIP - (A_CAL * SRP);
//				termcore_->output((boost::format("CIP: %6.5f\n") % CIP).str());
//			CORは抵抗値計算用の定数係数 = 4.6588E-8 @n
//			COCは容量値計算用の定数係数 = 21454732.4 @n
//			_iはユーザーが選択した測定電流値 [Ap-p] @n
//			_fはユーザーが選択した測定周波数 [Hz] @n
				double COR = 4.6588E-8;
				double COC = 21454732.4;
				static const double itbl[4] = {  // 電流テーブル [A]
					0.2 / 1000.0, 2.0 / 1000.0, 20.0 / 1000.0, 200.0 / 1000.0
				};
				double _i = itbl[amps_->get_select_pos()];
				static const double ftbl[3] = {  // 周波数テーブル [Hz]
					100.0, 1000.0, 10000.0
				};
				double _f = ftbl[freq_->get_select_pos()];
//        	R [ohm]= COR * (CRP^2 + CIP^2) / (_i * CRP) @n
//        	C [F] = (COC * _i * CIP) / {(2 * π * _f) * (CRP^2 + CIP^2) } @n
//			ここで：@n
				double R = COR * ((CRP * CRP) + (CIP * CIP)) / (_i * CRP);
//				termcore_->output((boost::format("R: %6.5f [ohm]\n") % R).str());
				double PAI = 3.141592654;
				double C = (COC * _i * CIP) / ((2 * PAI * _f) * ((CRP * CRP) + (CIP * CIP))); 
				C *= 1e6;
//				termcore_->output((boost::format("C: %6.5f [uF]\n") % C).str());
				ans_->set_text((boost::format("%6.5f uF") % C).str());
				net_regs_->set_text((boost::format("%6.5f Ω") % R).str());
				crcd_value_ = C;
				++crm_id_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ヘルプ機能
			@param[in]	chip	ヘルプ・チップ
		*/
		//-----------------------------------------------------------------//
		bool help(gui::widget_chip* chip)
		{
			bool ret = true;
			if(all_->get_focus()) {
				tools::set_help(chip, all_, "CRM ON/OFF");
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ（システム）
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save_sys(sys::preference& pre)
		{
			dialog_->save(pre);
			info_->save(pre);
			reg_refs_[0]->save(pre);
			for(uint32_t i = 0; i < 12; ++i) {
				carib_data_[i]->save(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
			tools::save_sw(pre, sw_, 14);
			ena_->save(pre);
			amps_->save(pre);
			freq_->save(pre);
			mode_->save(pre);
			all_->save(pre);
			ena_refs_->save(pre);
			reg_refs_[0]->save(pre);
			net_regs_->save(pre);
			for(uint32_t i = 0; i < 12; ++i) {
				carib_data_[i]->save(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ（システム）
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load_sys(sys::preference& pre)
		{
			dialog_->load(pre);
			info_->load(pre);
			reg_refs_[0]->load(pre);
			for(uint32_t i = 0; i < 12; ++i) {
				carib_data_[i]->load(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			tools::load_sw(pre, sw_, 14);
			ena_->load(pre);
			amps_->load(pre);
			freq_->load(pre);
			mode_->load(pre);
			all_->load(pre);
			all_->exec();
			ena_refs_->load(pre);
			reg_refs_[0]->load(pre);
			ena_refs_->exec();
			net_regs_->load(pre);
			for(uint32_t i = 0; i < 12; ++i) {
				carib_data_[i]->load(pre);
			}
		}
	};
}
