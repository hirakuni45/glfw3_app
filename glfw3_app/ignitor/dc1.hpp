#pragma once
//=====================================================================//
/*! @file
    @brief  DC1 クラス
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

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  DC1 クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dc1
	{
		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;

		gui::widget_check*		sw_[5];		///< DC1 接続スイッチ
		gui::widget_check*		ena_;		///< DC1 有効、無効
		gui::widget_list*		mode_;		///< DC1 電流、電圧モード
		gui::widget_label*		voltage_;	///< DC1（電圧）
		gui::widget_label*		current_;	///< DC1（電流）
	public:
		gui::widget_button*		exec_;		///< DC1 設定転送
		gui::widget_check*		all_;		///< DC1 全体

	private:
		uint32_t				offline_;

		struct dc1_t {
			uint32_t	sw;		///< 5 bits
			uint32_t	delay;	///< SW オンからコマンド発行までの遅延（10ms単位）
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits
			bool		thermal;	///< 熱抵抗モード
			uint32_t	count;	///< 熱抵抗測定回数（最大５００）

			dc1_t() : sw(0), delay(1), ena(0), mode(0), volt(0), curr(0),
				thermal(false), count(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("dc1 D1SW%02X\n") % sw).str();
				if(thermal) {
					s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
					s += (boost::format("dc1 D1TT%03X\n") % (count & 0x1ff)).str();
				} else {
					if(ena) {
						s += (boost::format("delay %d\n") % delay).str();
						s += (boost::format("dc1 D1MD%d\n") % mode).str();
						if(mode != 0) {
							s += (boost::format("dc1 D1VS%05X\n") % (volt & 0xfffff)).str();
						} else {
							s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
						}
					}
					s += (boost::format("dc1 D1OE%d\n") % ena).str();
				}			
				return s;
			}
		};

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		dc1(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc) :
			director_(d), client_(client), interlock_(ilc),
			sw_{ nullptr },
			ena_(nullptr), mode_(nullptr), voltage_(nullptr), current_(nullptr),
			exec_(nullptr), all_(nullptr), offline_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  リセット・スイッチ
		*/
		//-----------------------------------------------------------------//
		void reset_sw()
		{
			for(uint32_t i = 0; i < 5; ++i) {
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
			dc1_t t;
			client_.send_data(t.build());
// std::cout << "DC1 Startup" << std::endl;
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
			{  // 20V/0.1V
				widget::param wp(vtx::irect(ofsx + 230, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				voltage_ = wd.add_widget<widget_label>(wp, wp_);
				voltage_->at_local_param().select_func_ = [=](const std::string& str) {
					voltage_->set_text(tools::limitf(str, 0.0f, 20.0f, "%3.1f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 330, ofsy, 40, 40), root);
				widget_text::param wp_("V");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // Max: 24A / step: 10mA
				widget::param wp(vtx::irect(ofsx + 370, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				current_ = wd.add_widget<widget_label>(wp, wp_);
				current_->at_local_param().select_func_ = [=](const std::string& str) {
					current_->set_text(tools::limitf(str, 0.0f, 24.0f, "%4.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 470, ofsy, 50, 40), root);
				widget_text::param wp_("A");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					dc1_t t;
					uint16_t sw = 0;
					for(uint32_t i = 0; i < 5; ++i) {
						sw <<= 1;
						if(sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = ena_->get_check();
					if(mode_->get_select_pos() < 2) {
						t.mode = mode_->get_select_pos() & 1;
						t.count = 10;
						t.thermal = false;
///					} else {
///						t.count = count_->get_select_pos();
///						t.thermal = true;
					}

					float v;
					if((utils::input("%f", voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 62.5e-6;
					}
					if((utils::input("%f", current_->get_text().c_str()) % v).status()) {
						t.curr = v / 31.25e-6;
					}
					client_.send_data(t.build());
				};
			}
			{
				widget::param wp(vtx::irect(d_w - 45, ofsy, 30, 30), root);
				widget_check::param wp_;
				all_ = wd.add_widget<widget_check>(wp, wp_);
				all_->at_local_param().select_func_ = [=](bool ena) {
					if(!ena) {  // オフラインは WGM の関係があり、遅延させる。
						offline_ = 2;
					}
					for(uint32_t i = 0; i < 5; ++i) {
						sw_[i]->set_stall(!ena, widget::STALL_GROUP::_1);
					}
					ena_->set_stall(!ena, widget::STALL_GROUP::_1);
					mode_->set_stall(!ena, widget::STALL_GROUP::_1);
					voltage_->set_stall(!ena, widget::STALL_GROUP::_1);
					current_->set_stall(!ena, widget::STALL_GROUP::_1);
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
			// オフライン遅延処理
			if(offline_ > 0) {
				--offline_;
				if(offline_ == 0) {
					tools::set_checks(sw_, false, 5);
					startup();
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
			if(current_->get_focus()) {
				tools::set_help(chip, current_, "0.0 to 24.0 [A]");
			} else if(voltage_->get_focus()) {
				tools::set_help(chip, voltage_, "0.0 to 20.0 [V]");
			} else if(all_->get_focus()) {
				tools::set_help(chip, all_, "DC1 ON/OFF");
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
			ena_->save(pre);
			mode_->save(pre);
			voltage_->save(pre);
			current_->save(pre);
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
			ena_->load(pre);
			mode_->load(pre);
			voltage_->load(pre);
			current_->load(pre);
			all_->load(pre);
//			if(exec) all_->exec();
		}
	};
}
