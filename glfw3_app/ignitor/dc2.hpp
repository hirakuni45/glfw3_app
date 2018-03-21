#pragma once
//=====================================================================//
/*! @file
    @brief  DC2 クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
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
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "kikusui.hpp"
#include "tools.hpp"

// DC2 を菊水電源で置き換える場合有効にする
#define DC2_KIKUSUI

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  DC2 クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dc2
	{
		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;
		kikusui&				kikusui_;

	public:
		// DC2 設定
		gui::widget_check*		sw_[14];	///< DC2 接続スイッチ
		gui::widget_check*		ena_;		///< DC2 有効、無効
		gui::widget_list*		mode_;		///< DC2 電流、電圧モード
		gui::widget_label*		voltage_;	///< DC2 出力電圧
		gui::widget_label*		current_;	///< DC2 出力電流
		gui::widget_label*		delay_;		///< DC2 測定遅延
		gui::widget_label*		probe_;		///< DC2（電流、電圧測定値）
		gui::widget_button*		exec_;		///< DC2 設定転送
		gui::widget_check*		all_;		///< DC2 全体

		bool					probe_mode_;
		uint32_t				curr_delay_;
		uint32_t				curr_id_;
		float					curr_value_;
		uint32_t				volt_delay_;
		uint32_t				volt_id_;
		float					volt_value_;
		uint32_t				id_;

	private:

		struct dc2_t {
			uint32_t	sw;		///< 14 bits
			uint32_t	delay;	///< SW オンからコマンド発行までの遅延（10ms単位）
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits

			dc2_t() : sw(0), ena(0), mode(0), volt(0), curr(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("dc2 D2SW%04X\n") % sw).str();
#ifndef DC2_KIKUSUI
				if(ena) {
					s += (boost::format("delay %d\n") % delay).str();
					s += (boost::format("dc2 D2MD%d\n") % mode).str();
					if(mode != 0) {
						s += (boost::format("dc2 D2VS%05X\n") % (volt & 0xfffff)).str();
					} else {
						s += (boost::format("dc2 D2IS%05X\n") % (curr & 0xfffff)).str();
					}
				}
				s += (boost::format("dc2 D2OE%d\n") % ena).str();
				if(ena) {
					s += "delay 1\n";
					s += "dc2 D2M?\n";
				}
#endif
				return s;
			}
		};


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		dc2(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc, kikusui& kik) :
			director_(d), client_(client), interlock_(ilc), kikusui_(kik),
			sw_{ nullptr },
			ena_(nullptr), mode_(nullptr), voltage_(nullptr), current_(nullptr),
			delay_(nullptr), probe_(nullptr),
			exec_(nullptr), all_(nullptr),
			probe_mode_(false),
			curr_delay_(0), curr_id_(0), curr_value_(0.0f),
			volt_delay_(0), volt_id_(0), volt_value_(0.0f),
			id_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  値の更新 ID 取得
			@return 値の更新 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_id() const { return id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  測定単位文字列の取得
			@return 測定単位文字列
		*/
		//-----------------------------------------------------------------//
		const std::string get_unit_str() const {
			if(mode_->get_select_pos() == 0) {  // 電流設定、電圧測定 [V]
				return "V";
			} else {  // 電圧設定、電流測定 [mA]
				return "mA";
			} 
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
			dc2_t t;
			client_.send_data(t.build());
			kikusui_.set_output(0);
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
				widget_text::param wp_("DC2:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			tools::init_sw(wd, root, interlock_, ofsx, ofsy, sw_, 14, 15);
			ofsy += 50;
			{
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_check::param wp_("有効");
				ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 90, ofsy, 110, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("定電流");
				wp_.init_list_.push_back("定電圧");
				mode_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 300V/0.1V, 100mA/0.01mA
				widget::param wp(vtx::irect(ofsx + 210, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				voltage_ = wd.add_widget<widget_label>(wp, wp_);
				voltage_->at_local_param().select_func_ = [=](const std::string& str) {
					voltage_->set_text(tools::limitf(str, 0.0f, 300.0f, "%2.1f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 310, ofsy, 30, 40), root);
				widget_text::param wp_("V");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // Max: 100mA / step: 0.01mA
				widget::param wp(vtx::irect(ofsx + 340, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				current_ = wd.add_widget<widget_label>(wp, wp_);
				current_->at_local_param().select_func_ = [=](const std::string& str) {
					current_->set_text(tools::limitf(str, 0.0f, 100.0f, "%3.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 440, ofsy, 40, 40), root);
				widget_text::param wp_("mA");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 測定遅延時間
				widget::param wp(vtx::irect(ofsx + 490, ofsy, 100, 40), root);
				widget_label::param wp_("1.0", false);
				delay_ = wd.add_widget<widget_label>(wp, wp_);
				delay_->at_local_param().select_func_ = [=](const std::string& str) {
					delay_->set_text(tools::limitf(str, 0.45f, 30.0f, "%2.1f"));
				};
			}
			{  // 電流、電圧測定値
				widget::param wp(vtx::irect(ofsx + 600, ofsy, 150, 40), root);
				widget_label::param wp_("");
				probe_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					dc2_t t;
					uint16_t sw = 0;
					for(uint32_t i = 0; i < 14; ++i) {
						sw <<= 1;
						if(sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
#ifdef DC2_KIKUSUI
					kikusui_.set_output(ena_->get_check());
					int err = 0;
					float c = 0.0f;
					if((utils::input("%f", current_->get_text().c_str()) % c).status()) {
						c /= 1000.0f;
					} else {
						++err;
					}
					float v = 0.0f;
					if((utils::input("%f", voltage_->get_text().c_str()) % v).status()) {
					} else {
						++err;
					}
					float tm = 0.0f;
					if((utils::input("%f", delay_->get_text().c_str()) % tm).status()) {
					} else {
						++err;
					}

					if(err == 0) {
						if(mode_->get_select_pos() == 0) {  // 電流 [mA]
							kikusui_.set_curr(c, v);
							volt_delay_ = static_cast<uint32_t>(tm * 60.0f);
						} else {  // 電圧 [V]
							kikusui_.set_volt(v, c);
							curr_delay_ = static_cast<uint32_t>(tm * 60.0f);
						}
					}
#else
					t.ena = ena_->get_check();
					t.mode = mode_->get_select_pos() & 1;
					probe_mode_ = t.mode;
					float v;
					if((utils::input("%f", voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 312.5e-6;
					}
					if((utils::input("%f", current_->get_text().c_str()) % v).status()) {
						t.curr = v / 100e-6;
					}
#endif
					client_.send_data(t.build());
					probe_->set_text("");
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
					mode_->set_stall(!ena, widget::STALL_GROUP::_1);
					voltage_->set_stall(!ena, widget::STALL_GROUP::_1);
					current_->set_stall(!ena, widget::STALL_GROUP::_1);
					delay_->set_stall(!ena, widget::STALL_GROUP::_1);
					exec_->set_stall(!ena, widget::STALL_GROUP::_1);
					probe_->set_text("");
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

#ifdef DC2_KIKUSUI
			if(curr_delay_ > 0) {
				--curr_delay_;
				if(curr_delay_ == 0) {
					curr_id_ = kikusui_.get_curr_id();
					kikusui_.req_curr();
				}
			}
			if(volt_delay_ > 0) {
				--volt_delay_;
				if(volt_delay_ == 0) {
					volt_id_ = kikusui_.get_volt_id();
					kikusui_.req_volt();
				}
			}
			auto id = id_;
			if(curr_id_ != kikusui_.get_curr_id()) {
				curr_id_ = kikusui_.get_curr_id();
				auto c = kikusui_.get_curr();
				c *= 1000.0f;
				probe_->set_text((boost::format("%5.4f mA") % c).str());
				curr_value_ = c;
				++id_;
			}
			if(volt_id_ != kikusui_.get_volt_id()) {
				volt_id_ = kikusui_.get_volt_id();
				auto v = kikusui_.get_volt();
				probe_->set_text((boost::format("%5.4f V") % v).str());
				volt_value_ = v;
				++id_;
			}
			if(id != id_) {
				kikusui_.set_output(0);
			}
#endif
#if 0
			// D2MD
			if(d2md_id_ != client_.get_mod_status().d2md_id_) {
				d2md_id_ = client_.get_mod_status().d2md_id_;
				uint32_t v = client_.get_mod_status().d2md_;
				float a = static_cast<float>(v);
				if(dc2_probe_mode_) {
					a /= 999960.2f;
					a *= 100.0f;
					dc2_probe_->set_text((boost::format("%5.2f mA") % a).str());
				} else {
					a /= static_cast<float>(0xFFFFF);
	   				a *= 330.0f;
					dc2_probe_->set_text((boost::format("%5.2f V") % a).str());
				}
			}
#endif
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
			if(voltage_->get_focus()) {
				tools::set_help(chip, voltage_, "0.0 to 300.0 [V]");
			} else if(current_->get_focus()) {
				tools::set_help(chip, current_, "0.0 to 100.0 [mA]");
			} else if(delay_->get_focus()) {
				tools::set_help(chip, delay_, "測定遅延時間 [Sec]");
			} else if(all_->get_focus()) {
				tools::set_help(chip, all_, "DC2 ON/OFF");
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
			mode_->save(pre);
			voltage_->save(pre);
			current_->save(pre);
			delay_->save(pre);
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
			mode_->load(pre);
			voltage_->load(pre);
			current_->load(pre);
			delay_->load(pre);
			all_->load(pre);
			all_->exec();
		}
	};
}
