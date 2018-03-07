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

	public:
		gui::widget_check*		sw_[5];		///< DC1 接続スイッチ

		gui::widget_label*		thk_;		///< THR 温度係数

		gui::widget_label*		current_;	///< DC1（電流）
		gui::widget_spinbox*	count_;		///< DC1 熱抵抗測定回数

		gui::widget_label*		probe_;		///< THR 熱抵抗値

		gui::widget_button*		exec_;		///< THR 設定転送
		gui::widget_check*		all_;		///< THR 全体

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		thr(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc) :
			director_(d), client_(client), interlock_(ilc),
			sw_{ nullptr },
			thk_(nullptr), 
			current_(nullptr), count_(nullptr),
			probe_(nullptr),
			exec_(nullptr), all_(nullptr)
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
				widget_label::param wp_("", false);
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
#if 0
					dc1_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = ena_->get_check();
					if(mode_->get_select_pos() < 2) {
						t.mode = mode_->get_select_pos() & 1;
						t.count = 10;
						t.thermal = false;
					} else {
						t.count = count_->get_select_pos();
						t.thermal = true;
					}

					float v;
					if((utils::input("%f", voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 62.5e-6;
					}
					if((utils::input("%f", current_->get_text().c_str()) % v).status()) {
						t.curr = v / 31.25e-6;
					}
					client_.send_data(t.build());
#endif
				};
			}
			{
				widget::param wp(vtx::irect(d_w - 45, ofsy, 30, 30), root);
				widget_check::param wp_;
				all_ = wd.add_widget<widget_check>(wp, wp_);
				all_->at_local_param().select_func_ = [=](bool ena) {
					if(!ena) {
///						startup();
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
			bool ena = false;
			if(client_.probe() && all_->get_check()) ena = true; 
			exec_->set_stall(!ena);
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
		}
	};
}
