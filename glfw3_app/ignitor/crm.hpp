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
		gui::widget_text*		reg_refs_[5];

		gui::widget_dialog*		dialog_;

		double					crrd_value_;
		double					crcd_value_;

	private:
		static const uint32_t crm_count_limit_ = 5;  // 測定回数

		std::vector<float>		crrd_vals_;
		std::vector<float>		crcd_vals_;

		uint32_t				crrd_id_;
		uint32_t				crcd_id_;

		enum class refs_task {
			none,
			term1,
			term2,
			term3,
			term4,
			term5,
			fin
		};
		refs_task				refs_task_;

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
				if(ena) {
					s += (boost::format("crm CRSW%04X\n") % sw).str();
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
		crm_t			last_crm_t_;


		std::string make_crm_param_(uint32_t delay)
		{
			crm_t& t = last_crm_t_;
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
			return t.build(delay);
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


		void refs_sub_(const std::string& ins = "")
		{
			int n = static_cast<int>(refs_task_) - static_cast<int>(refs_task::term1);
			std::string str;
			if(!ins.empty()) {
				str = (boost::format("Term%d の校正値：%s") % n % ins).str();
			}
			if(refs_task_ != refs_task::fin) {
				if(!str.empty()) str += '\n';	
				str += (boost::format("Term%d の校正を行います。") % (n + 1)).str();
			}
			dialog_->set_text(str);
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
			run_refs_(nullptr), reg_refs_{ nullptr }, dialog_(nullptr),
			crrd_value_(0.0), crcd_value_(0.0),
			crrd_vals_(), crcd_vals_(), crrd_id_(0), crcd_id_(0),
			refs_task_(refs_task::none), last_crm_t_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
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
				widget::param wp(vtx::irect(ofsx + 350, ofsy, 110, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("抵抗値");
				wp_.init_list_.push_back("容量値");
				wp_.init_list_.push_back("容量値（抵抗除外）");
				mode_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 490, ofsy, 200, 40), root);
				widget_label::param wp_("");
				ans_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					auto str = make_crm_param_(120);
					client_.send_data(str);
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
					ans_->set_text("");
				};
			}
			{  // 校正ボタン
				widget::param wp(vtx::irect(ofsx, ofsy + 50, 100, 40), root);
				wp.pre_group_ = 1000;
				widget_button::param wp_("校正");
				run_refs_ = wd.add_widget<widget_button>(wp, wp_);
				run_refs_->at_local_param().select_func_ = [=](uint32_t id) {
					refs_task_ = refs_task::term1;
					refs_sub_();
					dialog_->enable();
				};
			}
			for(uint32_t i = 0; i < 5; ++i) {  // 校正データ表示
				widget::param wp(vtx::irect(ofsx + 110 + i * 80, ofsy + 50, 70, 40), root);
				wp.pre_group_ = 1000;
				widget_text::param wp_("---");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				reg_refs_[i] = wd.add_widget<widget_text>(wp, wp_);
			}
			if(0) {  // 校正ダイアログ
				widget::param wp(vtx::irect(100, 100, 500, 300));
				wp.pre_group_ = 1000;
				widget_dialog::param wp_(widget_dialog::style::CANCEL_OK);
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [=](bool ok) {
					switch(refs_task_) {
					case refs_task::term1:
						dialog_->enable(ok);
						refs_task_ = refs_task::term2;
						refs_sub_("0.0011");
						break;
					case refs_task::term2:
						dialog_->enable(ok);
						refs_task_ = refs_task::term3;
						refs_sub_("0.0012");
						break;
					case refs_task::term3:
						dialog_->enable(ok);
						refs_task_ = refs_task::term4;
						refs_sub_("0.0013");
						break;
					case refs_task::term4:
						dialog_->enable(ok);
						refs_task_ = refs_task::term5;
						refs_sub_("0.0014");
						break;
					case refs_task::term5:
						dialog_->enable(ok);
						refs_task_ = refs_task::fin;
						refs_sub_("0.0015");
						break;
					default:
						break;
					}
				};
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
					int32_t v = client_.get_mod_status().crrd_;
					int32_t ofs = sample_num * 0x7FFFF;
					v -= ofs;
//					if(v < -ofs || v > ofs) {
//						ans_->set_text("OVF");
//						return;
//					}
					double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
// std::cout << a << std::endl;
					a *= 778.2;  // 778.2 mV P-P
					static const double itbl[4] = {  // 電流テーブル
						0.2, 2.0, 20.0, 200.0
					};
					a /= itbl[amps_->get_select_pos()];
					crrd_vals_.push_back(a);
					if(crrd_vals_.size() >= crm_count_limit_) {
						crrd_value_ = median_(crrd_vals_);
						ans_->set_text((boost::format("%6.5f Ω") % crrd_value_).str());
					} else {
						auto str = make_crm_param_(0);
						client_.send_data(str);
					}
				}
			} else { // CRCD (1, 2)
				if(crcd_id_ != client_.get_mod_status().crcd_id_) {
					crcd_id_ = client_.get_mod_status().crcd_id_;
					int32_t v = client_.get_mod_status().crcd_;
					int32_t ofs = sample_num * 0x7FFFF;
					v -= ofs;
//					if(v < -ofs || v > ofs) {
//						ans_->set_text("OVF");
//						return;
//					}
					double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
					a *= 778.2;  // 778.2 mV P-P
					double b = 390.0;
					static const double itbl[4] = {  // 電流テーブル
						0.2, 2.0, 20.0, 200.0
					};
//					a /= itbl[amps_->get_select_pos()];
//					a = 1.0 / (2.0 * 3.141592654 * 1000.0 * a);
///					a = itbl[amps_->get_select_pos()] / (2.0 * 3.141592654 * 1000.0 * a);
					a = (itbl[amps_->get_select_pos()] * a * a)
						/ (2.0 * 3.141592654 * 1000.0 * (a * a + b * b));
					a *= 1e6;
					crcd_vals_.push_back(a);
					if(crcd_vals_.size() >= crm_count_limit_) {
						crcd_value_ = median_(crcd_vals_);
						ans_->set_text((boost::format("%6.5f uF") % crcd_value_).str());
					} else {
						auto str = make_crm_param_(0);
						client_.send_data(str);
					}
				}
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
		}
	};
}
