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
		static const uint32_t DUMMY_LOOP_COUNT = 45;  // ダミーループ

		static const uint32_t crm_count_limit_ = 10;  // メディアン測定回数

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

		double					crrd_value_;
		double					crcd_value_;

	private:
		std::vector<float>		crrd_vals_;
		std::vector<float>		crcd_vals_;

		uint32_t				crrd_id_;
		uint32_t				crcd_id_;

		enum class refs_task {
			idle,
			start,
			term1,
			term2,
			term3,
			term4,
			term5,
		};
		refs_task				refs_task_;
		uint32_t				refs_id_;

		uint32_t				dummy_count_;
		std::string				last_cmd_;

		uint32_t				crm_id_;

		struct crm_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			uint16_t	amps;	///< 0, 1, 2, 3
			uint16_t	freq;	///< 0, 1, 2
			uint16_t	mode;	///< 0, 1

			crm_t() : sw(0), ena(0), amps(0), freq(0), mode(0) { }

			std::string build(uint32_t delay) const
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
//					if(delay > 0) {
//						s += (boost::format("delay %d\n") % delay).str();
//					}
					if(mode > 0) {  // mode 1, 2
						s += (boost::format("crm CRC?1\n")).str();
					} else {  // mode 0
						s += (boost::format("crm CRR?1\n")).str();
					}
				}
				return s;
			}
		};


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
			t.mode = mode_->get_select_pos();
			return t.build(0);
		}


		float median_(std::vector<float>& ss)
		{
			std::sort(ss.begin(), ss.end());
			float v = ss[ss.size() / 2];
			if((ss.size() & 1) == 0) {
				v += ss[(ss.size() / 2) + 1];
				v *= 0.5f;
			}
			return v;
		}


		void refs_msg_(const std::string& ins = "")
		{
			int n = static_cast<int>(refs_task_) - static_cast<int>(refs_task::start);
			std::string str = "200 mA / 抵抗測定\n";
			if(n >= 1 && n <= 5) {
				str += (boost::format("Term1 - Term4 の校正値：%s\n") % ins).str();
			}
			if(n >= 0 && n <= 4) {
				str += (boost::format("Term1 - Term4 の校正を行います。")).str();
			}
			dialog_->set_text(str);
		}


		void refs_send_()
		{
			int n = static_cast<int>(refs_task_) - static_cast<int>(refs_task::start);
			crm_t t;
			switch(n) {
			case 1:
				t.sw = 0b10000000001000;  // T1 - T4
				break;
			case 2:
				t.sw = 0b01000000000010;  // T2 - T6
				break;
			case 3:
				t.sw = 0b00100000000010;  // T3 - T6
				break;
			case 4:
				t.sw = 0b00010000000010;  // T4 - T6
				break;
			case 5:
				t.sw = 0b00001000000010;  // T5 - T6
				break;
			default:
				return;
				break;
			}
			if(n >= 1 && n <= 5) {
				t.ena = true;
				t.amps = 3;  // 200 mA
				amps_->select(3);
				t.freq = 0;  // 100Hz
				freq_->select(0);
				t.mode = 0;  // 抵抗
				mode_->select(0);				
				refs_id_ = crm_id_;
				crrd_id_ = client_.get_mod_status().crrd_id_;
				crcd_id_ = client_.get_mod_status().crcd_id_;
				last_cmd_ = t.build(0);
				client_.send_data(last_cmd_);
				dummy_count_ = DUMMY_LOOP_COUNT;
			}
		}


		double get_bias_()
		{
			double ofs = 0.0;
			// T1 - T4
			if((utils::input("%f", reg_refs_[0]->get_text().c_str()) % ofs).status()) {
			}
#if 0
			if(sw_[0]->get_check()) {  // T1
				if((utils::input("%f", reg_refs_[0]->get_text().c_str()) % ofs).status()) {
				}
			} else if(sw_[1]->get_check()) {  // T2
				if((utils::input("%f", reg_refs_[1]->get_text().c_str()) % ofs).status()) {
				}
			} else if(sw_[2]->get_check()) {  // T3
				if((utils::input("%f", reg_refs_[2]->get_text().c_str()) % ofs).status()) {
				}
			} else if(sw_[3]->get_check()) {  // T4
				if((utils::input("%f", reg_refs_[3]->get_text().c_str()) % ofs).status()) {
				}
			} else if(sw_[4]->get_check()) {  // T5
				if((utils::input("%f", reg_refs_[4]->get_text().c_str()) % ofs).status()) {
				}
			}
#endif
			return ofs;
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
			crrd_value_(0.0), crcd_value_(0.0),
			crrd_vals_(), crcd_vals_(), crrd_id_(0), crcd_id_(0),
			refs_task_(refs_task::idle), refs_id_(0), dummy_count_(0), last_cmd_(),
			crm_id_(0)
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
				return "Ω";
			} else {  // 容量
				return "uF";
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
			tools::set_checks(sw_, false, 14);
			crm_t t;
			client_.send_data(t.build(0));
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
					if(last_cmd_ != s) {
						dummy_count_ = DUMMY_LOOP_COUNT;
					}
					last_cmd_ = s;
					crrd_id_ = client_.get_mod_status().crrd_id_;
					crcd_id_ = client_.get_mod_status().crcd_id_;
					client_.send_data(last_cmd_);
					crrd_vals_.clear();
					crcd_vals_.clear();
					ans_->set_text("");
				};
			}
			{
				widget::param wp(vtx::irect(d_w - 45, ofsy, 30, 30), root);
				widget_check::param wp_;
				all_ = wd.add_widget<widget_check>(wp, wp_);
				all_->at_local_param().select_func_ = [=](bool ena) {
					if(!ena) {
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
					ans_->set_text("");
					last_cmd_.clear();
				};
			}
			{  // 校正ボタン
				widget::param wp(vtx::irect(ofsx, ofsy + 50, 100, 40), root);
				wp.pre_group_ = 1;
				widget_button::param wp_("校正");
				run_refs_ = wd.add_widget<widget_button>(wp, wp_);
				run_refs_->at_local_param().select_func_ = [=](uint32_t id) {
					refs_task_ = refs_task::start;
					refs_msg_("");
					dialog_->enable();
					last_cmd_.clear();
				};
			}
			{  // 校正データ、有効／無効
				widget::param wp(vtx::irect(ofsx + 110, ofsy + 50, 90, 40), root);
				wp.pre_group_ = 1;
				widget_check::param wp_("有効");
				ena_refs_ = wd.add_widget<widget_check>(wp, wp_);
				ena_refs_->at_local_param().select_func_ = [=](bool ena) {
					for(uint32_t i = 0; i < 5; ++i) {
						reg_refs_[i]->set_stall(!ena);
					}
				};
			}
			for(uint32_t i = 0; i < 5; ++i) {  // 校正データ表示
				widget::param wp(vtx::irect(ofsx + i * 140, ofsy + 100, 120, 40), root);
				wp.pre_group_ = 1;
				widget_text::param wp_("*");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				reg_refs_[i] = wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 校正ダイアログ
				widget::param wp(vtx::irect(100, 100, 500, 300));
				wp.pre_group_ = 1;
				widget_dialog::param wp_(widget_dialog::style::CANCEL_OK);
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [=](bool ok) {
					if(!ok) {
						dialog_->enable(false);
						refs_task_ = refs_task::idle;
						return;
					}
					switch(refs_task_) {
					case refs_task::start:
						refs_task_ = refs_task::term5;
						refs_msg_("");
						refs_send_();
						break;
					case refs_task::term1:
						refs_task_ = refs_task::term2;
						refs_send_();
						break;
					case refs_task::term2:
						refs_task_ = refs_task::term3;
						refs_send_();
						break;
					case refs_task::term3:
						refs_task_ = refs_task::term4;
						refs_send_();
						break;
					case refs_task::term4:
						refs_task_ = refs_task::term5;
						refs_send_();
						break;
					case refs_task::term5:
						refs_task_ = refs_task::idle;
						break;
					default:
						break;
					}


				};
			}
			{  // 校正情報
				widget::param wp(vtx::irect(100, 100, 500, 300));
				wp.pre_group_ = 1;
				widget_dialog::param wp_(widget_dialog::style::NONE);
				info_ = wd.add_widget<widget_dialog>(wp, wp_);
				info_->enable(false);
			}
			{  // 合成回路、抵抗設定
				widget::param wp(vtx::irect(ofsx + 220, ofsy + 50, 110, 40), root);
				wp.pre_group_ = 1;
				widget_label::param wp_("390.0", false);
				net_regs_ = wd.add_widget<widget_label>(wp, wp_);
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
			if(mode_->get_select_pos() == 0) {  // CRRD
				if(crrd_id_ != client_.get_mod_status().crrd_id_) {
					crrd_id_ = client_.get_mod_status().crrd_id_;
					if(dummy_count_ > 0) {
						--dummy_count_;
						client_.send_data(last_cmd_);
						return;
					}
					int32_t v = client_.get_mod_status().crrd_;
					int32_t ofs = sample_num * 0x7FFFF;
					v -= ofs;
// std::cout << v << std::endl;
					double lim = static_cast<double>(ofs) / 1.570798233;
					if(v >= lim) {
						ans_->set_text("OVF");
						return;
					}
					double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
					a *= 778.2;  // 778.2 mV P-P
					static const double itbl[4] = {  // 電流テーブル
						0.2, 2.0, 20.0, 200.0
					};
					a /= itbl[amps_->get_select_pos()];
					crrd_vals_.push_back(a);
					if(crrd_vals_.size() >= crm_count_limit_) {
						crrd_value_ = median_(crrd_vals_);
						if(amps_->get_select_pos() == 3 && refs_task_ == refs_task::idle) {
							if(ena_refs_->get_check()) {
								crrd_value_ -= get_bias_();
							}
						}
						ans_->set_text((boost::format("%6.5f Ω") % crrd_value_).str());
						++crm_id_;
					} else {
						client_.send_data(last_cmd_);
					}
				}
			} else { // CRCD (1, 2)
				if(crcd_id_ != client_.get_mod_status().crcd_id_) {
					crcd_id_ = client_.get_mod_status().crcd_id_;
					if(dummy_count_ > 0) {
						--dummy_count_;
						client_.send_data(last_cmd_);
						return;
					}
					int32_t v = client_.get_mod_status().crcd_;
					int32_t ofs = sample_num * 0x7FFFF;
					v -= ofs;
// std::cout << v << std::endl;
					double lim = static_cast<double>(ofs) / 1.570798233;
					if(v >= lim) {
						ans_->set_text("OVF");
						return;
					}
					double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
					a *= 778.2;  // 778.2 mV P-P
					static const double itbl[4] = {  // 電流テーブル
						0.2, 2.0, 20.0, 200.0
					};
					double ib = itbl[amps_->get_select_pos()];
					static const double ftbl[4] = {  // 周波数テーブル
						100.0, 1000.0, 10000.0
					};
					double fq = ftbl[freq_->get_select_pos()];
					if(mode_->get_select_pos() == 1) {  // 容量
						a = ib / (2.0 * 3.141592654 * fq * a);
					} else {  // 合成
						float b = 390.0;
						if((utils::input("%f", net_regs_->get_text().c_str()) % b).status()) {
						}
						a = (ib * a * a) / ((2.0 * 3.141592654 * fq * (a * a + b * b)));
					}
					a *= 1e6;
					crcd_vals_.push_back(a);
					if(crcd_vals_.size() >= crm_count_limit_) {
						crcd_value_ = median_(crcd_vals_);
						ans_->set_text((boost::format("%6.5f uF") % crcd_value_).str());
						++crm_id_;
					} else {
						client_.send_data(last_cmd_);
					}
				}
			}

			switch(refs_task_) {
			case refs_task::term1:
			case refs_task::term2:
			case refs_task::term3:
			case refs_task::term4:
			case refs_task::term5:
				if(refs_id_ != crm_id_) {
					refs_id_ = crm_id_;
					auto s = (boost::format("%6.5f Ω") % crrd_value_).str();
///					int idx = static_cast<int>(refs_task_) - static_cast<int>(refs_task::term1);
///					reg_refs_[idx]->set_text(s);
					reg_refs_[0]->set_text(s);
					refs_msg_(s);
					dialog_->enable();
				}
				break;
			default:
				break;
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
//			for(uint32_t i = 0; i < 5; ++i) {
//				reg_refs_[i]->save(pre);
//			}
			reg_refs_[0]->save(pre);
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
//			for(uint32_t i = 0; i < 5; ++i) {
//				reg_refs_[i]->save(pre);
//			}
			reg_refs_[0]->save(pre);
			net_regs_->save(pre);
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
//			for(uint32_t i = 0; i < 5; ++i) {
//				reg_refs_[i]->load(pre);
//			}
			reg_refs_[0]->load(pre);
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
//			for(uint32_t i = 0; i < 5; ++i) {
//				reg_refs_[i]->load(pre);
//			}
			reg_refs_[0]->load(pre);
			ena_refs_->exec();
			net_regs_->load(pre);
		}
	};
}
