#pragma once
//=====================================================================//
/*! @file
    @brief  ThR クラス（熱抵抗）
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
#include "tools.hpp"
#include "wave_cap.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ThR クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class thr
	{
		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;
		kikusui&				kikusui_;
		wave_cap&				wave_cap_;

		gui::widget_check*		sw_[5];		///< DC1 接続スイッチ

		gui::widget_label*		thk_;		///< THR 温度係数
		gui::widget_label*		current_;	///< DC1（電流）
		gui::widget_spinbox*	count_;		///< DC1 熱抵抗測定回数
		gui::widget_label*		probe_;		///< THR 熱抵抗値
		gui::widget_button*		exec_;		///< THR 設定転送
	public:
		gui::widget_check*		all_;		///< THR 全体

	private:

		struct thr_t {
			uint16_t	sw;
			uint32_t	curr;	///< 20 bits
			uint32_t	count;	///< 熱抵抗測定回数（最大５００）

			thr_t() : sw(0), curr(0), count(0) { }

			std::string build() const
			{
				std::string s;
				// DC1 熱抵抗関係設定
				{  // DC1: S42, S49, 定電流モード、加熱電流設定、出力は OFF
					s += (boost::format("dc1 D1SW%02X\n") % sw).str();
					uint16_t delay = 10;
					s += (boost::format("delay %d\n") % delay).str();
					s += "dc1 D1MD0\n";  // CC mode
					s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
					s += (boost::format("dc1 D1TT%03X\n") % (count & 0x1ff)).str();
				}
				return s;
			}
		};
		thr_t		thr_;

		uint32_t	id_;

		std::string build_sub_() const
		{
			std::string s;
			{  // DC2 S15, S28, 定電圧１４Ｖ、
				uint32_t dc2_sw = 0b10000000000001;
				s += (boost::format("dc2 D2SW%04X\n") % dc2_sw).str();
				kikusui_.set_output(1);
				kikusui_.set_volt(14.0f, 0.1f);  // 14V 定電圧、最大 0.1A
				uint16_t delay = 10;
				s += (boost::format("delay %d\n") % delay).str();
			}
			{  // WGM: S45, S48, 内臓電源電圧 7.2V、出力 OFF
				// S44, S41, S42, S43, S44
				uint16_t wgm_sw = 0b01001;
				s += (boost::format("wgm WGSW%02X\n") % wgm_sw).str();

				uint32_t ivolt = 7.2f / 15.626e-6;
				s += (boost::format("wgm WGDV%05X\n") % (ivolt & 0xfffff)).str();
				uint32_t volt = 5.0f / 0.02f;
				s += (boost::format("wgm WGPV%03X\n") % (volt & 0x3ff)).str();
				int iena = 1;
				s += (boost::format("wgm WGDE%d\n") % iena).str();
			}
			{  // WDM 熱抵抗関係設定
				// SW　(0,1,2,3　CH2）
				uint32_t cmd = 0b00001000;
				cmd <<= 16;
				uint32_t sw = 0b1111;  // all ON!
				cmd |= sw << 12;
				s += (boost::format("wdm %06X\n") % cmd).str();

				// sampling freq
				// 100K (10uS)
				cmd = (0b00010000 << 16) | (static_cast<uint32_t>(0b01000011) << 8);
				s += (boost::format("wdm %06X\n") % cmd).str();

				// channel gain
				cmd = ((0b00011000 | 2) << 16) | (7 << 13);
				s += (boost::format("wdm %06X\n") % cmd).str();

				cmd = (0b00100000 << 16);
				s += (boost::format("wdm %06X\n") % cmd).str();

				// trigger channel
				cmd = (0b00100000 << 16);
				cmd |= 0x0200;  // external trigger, single trigger
//				if(sm_trg_->get_select_pos()) cmd |= 0x0100;
				uint32_t ch = 0;
				uint8_t sub = 0;
				sub |= 0x80;  // trigger ON
				cmd |= static_cast<uint32_t>(ch & 3) << 14;
//				if(slope_->get_select_pos()) sub |= 0x40;
				sub |= 1;  // windows = 1
				cmd |= sub;
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			return s;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		thr(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc, kikusui& kik, wave_cap& wac) :
			director_(d), client_(client), interlock_(ilc), kikusui_(kik), wave_cap_(wac),
			sw_{ nullptr },
			thk_(nullptr), 
			current_(nullptr), count_(nullptr),
			probe_(nullptr),
			exec_(nullptr), all_(nullptr),
			thr_(), id_(0)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  測定単位文字列の取得
			@return 測定単位文字列
		*/
		//-----------------------------------------------------------------//
		const std::string get_unit_str() const {
			return "℃/W";
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
			std::string s;
			{  // DC2
				kikusui_.set_output(0);
			}
			{  // DC1
				uint16_t sw = 0;
				s += (boost::format("dc1 D1SW%02X\n") % sw).str();
				uint16_t mode = 0;
				s += (boost::format("dc1 D1MD%d\n") % mode).str();
				uint32_t curr = 0;
				s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
				uint16_t ena = 0;
				s += (boost::format("dc1 D1OE%d\n") % ena).str();
			}
			{  // WDM
				// SW
				uint32_t cmd = 0b00001000;
				cmd <<= 16;
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			{  // WGM
				uint32_t sw = 0;
				uint32_t type = 0;
				uint32_t frq = 0;
				uint32_t duty = 0;
				uint32_t volt = 0;
				uint32_t ena = 0;
				uint32_t iena = 0;
				uint32_t ivolt = 0;
				s += (boost::format("wgm WGSW%02X\n") % sw).str();
				s += (boost::format("wgm WGSP%d\n") % type).str();
				s += (boost::format("wgm WGFQ%02X\n") % (frq & 0x7f)).str();
				s += (boost::format("wgm WGPW%03X\n") % (duty & 0x3ff)).str();
				s += (boost::format("wgm WGPV%03X\n") % (volt & 0x3ff)).str();
				s += (boost::format("wgm WGOE%d\n") % ena).str();
				s += (boost::format("wgm WGDV%05X\n") % (ivolt & 0xfffff)).str();
				s += (boost::format("wgm WGDE%d\n") % iena).str();
			}
			client_.send_data(s);
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
				widget::param wp(vtx::irect(15, ofsy, 70, 40), root);
				widget_text::param wp_("DC1:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			static const char* swt[] = {
				"40", "41", "42", "43", "49"
			};

			tools::init_sw_dc1(wd, root, interlock_, ofsx, ofsy, sw_, 5, swt);
			ofsy += 50;

			{  // Max: 30A / step: 10mA
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				current_ = wd.add_widget<widget_label>(wp, wp_);
				current_->at_local_param().select_func_ = [=](const std::string& str) {
					current_->set_text(tools::limitf(str, 0.0f, 30.0f, "%4.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 100, ofsy, 30, 40), root);
				widget_text::param wp_("A");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 熱抵抗回数
				widget::param wp(vtx::irect(ofsx + 130, ofsy, 110, 40), root);
				widget_spinbox::param wp_(10, 10, 500);
				count_ = wd.add_widget<widget_spinbox>(wp, wp_);
				count_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			} 
			{  // 温度係数
				widget::param wp(vtx::irect(ofsx + 250, ofsy, 110, 40), root);
				widget_label::param wp_("0.707", false);
				thk_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 測定値
				widget::param wp(vtx::irect(ofsx + 370, ofsy, 150, 40), root);
				widget_label::param wp_("");
				probe_ = wd.add_widget<widget_label>(wp, wp_);
			}

			{
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					std::string s = build_sub_();
// std::cout << s << std::flush;
					thr_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.count = count_->get_select_pos();
					float v;
					if((utils::input("%f", current_->get_text().c_str()) % v).status()) {
						t.curr = v / 31.25e-6;
					}
					s += t.build();
// std::cout << s << std::endl;
					client_.send_data(s);
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
					for(uint32_t i = 0; i < 5; ++i) {
						sw_[i]->set_stall(!ena, widget::STALL_GROUP::_1);
					}
					thk_->set_stall(!ena, widget::STALL_GROUP::_1);
					current_->set_stall(!ena, widget::STALL_GROUP::_1);
					count_->set_stall(!ena, widget::STALL_GROUP::_1);
					exec_->set_stall(!ena, widget::STALL_GROUP::_1);
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

			if(id_ != wave_cap_.get_treg_value_id()) {
				auto a = wave_cap_.get_treg_value();
				probe_->set_text((boost::format("%5.4f mV") % (a * 1000.0f)).str());
				id_ = wave_cap_.get_treg_value_id();
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
			if(current_->get_focus()) {
				tools::set_help(chip, current_, "0.0 to 30.0 [A]");
			} else if(count_->get_focus()) {
				tools::set_help(chip, count_, "パルス数");
			} else if(thk_->get_focus()) {
				tools::set_help(chip, thk_, "温度係数");
			} else if(all_->get_focus()) {
				tools::set_help(chip, all_, "THR ON/OFF");
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
			tools::save_sw(pre, sw_, 5);
			current_->save(pre);
			count_->save(pre);
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
			tools::load_sw(pre, sw_, 5);
			current_->load(pre);
			count_->load(pre);
			all_->load(pre);
			all_->exec();
		}
	};
}
