#pragma once
//=====================================================================//
/*! @file
    @brief  CRM 直接、クラス
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

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  CRM クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class crm
	{
		utils::director<core>&	director_;

		gui::widget_check*		sw_[14];	///< CRM 接続スイッチ
		gui::widget_check*		ena_;		///< CRM 有効、無効
		gui::widget_list*		amps_;		///< CRM 電流レンジ切り替え
		gui::widget_list*		freq_;		///< CRM 周波数（100Hz, 1KHz, 10KHz）
		gui::widget_list*		mode_;		///< CRM 抵抗測定、容量測定
		gui::widget_label*		ans_;		///< CRM 測定結果 
		gui::widget_button*		exec_;		///< CRM 設定転送

		double					crrd_value_;
		double					crcd_value_;

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
				s = (boost::format("CRSW%04X\n") % sw).str();
				if(ena) {
					s += (boost::format("CRIS%d\n") % amps).str();
					static const char* frqtbl[3] = { "001", "010", "100" };
					s += (boost::format("CRFQ%s\n") % frqtbl[freq % 3]).str();
				}
				s += (boost::format("CROE%d\n") % ena).str();
				if(ena) {
					if(mode > 0) {  // mode 1, 2
						s += (boost::format("CRC?1\n")).str();
					} else {  // mode 0
						s += (boost::format("CRR?1\n")).str();
					}
				}
				return s;
			}
		};

#if 0
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
#endif

#if 0
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
#endif

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		crm(utils::director<core>& d) :
			director_(d),
			sw_{ nullptr },
			ena_(nullptr), amps_(nullptr), freq_(nullptr), mode_(nullptr),
			ans_(nullptr), exec_(nullptr),
			crrd_value_(0.0), crcd_value_(0.0)
		{ }


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

///			tools::init_sw(wd, root, interlock_, ofsx, ofsy, sw_, 14, 1);
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
//					auto s = make_crm_param_();

					ans_->set_text("");
				};
			}
#if 0
			{  // 合成回路、抵抗設定
				widget::param wp(vtx::irect(ofsx + 220, ofsy + 50, 110, 40), root);
				widget_label::param wp_("390.0", false);
				net_regs_ = wd.add_widget<widget_label>(wp, wp_);
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
#if 0
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
// std::cout << v << std::endl;
					int32_t ofs = sample_num * 0x7FFFF;
					v -= ofs;
// std::cout << v << std::endl;
					double lim = static_cast<double>(ofs) / 1.570798233;
					if(v >= lim) {
						ans_->set_text("OVF");
						return;
					}
					static const double itbl[4] = {  // 電流テーブル
						0.2, 2.0, 20.0, 200.0
					};
					double ib = itbl[amps_->get_select_pos()];
					v -= 447158 * ib;
					v += 23989;

					double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
					a *= 778.2;  // 778.2 mV P-P


					static const double ftbl[4] = {  // 周波数テーブル
						100.0, 1000.0, 10000.0
					};
					double fq = ftbl[freq_->get_select_pos()];
					if(mode_->get_select_pos() == 1) {  // 容量
						a = ib / (2.0 * 3.141592654 * fq * a);
					} else {  // 合成
						float b = 390.0;
						if((utils::input("%f", net_regs_->get_text().c_str()) % b).status()) {
							b *= ib;
						}
						a = (ib * a) / (2.0 * 3.141592654 * fq * (a * a + b * b));
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
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
//			tools::save_sw(pre, sw_, 14);
			ena_->save(pre);
			amps_->save(pre);
			freq_->save(pre);
			mode_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
//			tools::load_sw(pre, sw_, 14);
			ena_->load(pre);
			amps_->load(pre);
			freq_->load(pre);
			mode_->load(pre);
		}
	};
}
