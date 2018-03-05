#pragma once
//=====================================================================//
/*! @file
    @brief  テスト・パラメータ・クラス
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

#include "tools.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  検査パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class test_param
	{
		utils::director<core>&	director_;

	public:
		gui::widget_label*		symbol_;	///< 検査記号
		gui::widget_spinbox*	retry_;		///< リトライ回数
		gui::widget_label*		wait_;		///< 検査遅延時間設定
		gui::widget_list*		term_;		///< 検査端子設定
		gui::widget_label*		delay_;		///< 検査信号遅延時間
		gui::widget_list*		filter_;	///< 検査信号フィルター
		gui::widget_label*		width_;		///< 検査信号取得幅
		gui::widget_label*		min_;		///< 検査最小値
		gui::widget_label*		max_;		///< 検査最大値

		test::value_t			value_;


		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		test_param(utils::director<core>& d) : director_(d),
			symbol_(nullptr), retry_(nullptr), wait_(nullptr),
			term_(nullptr), delay_(nullptr), filter_(nullptr),
			width_(nullptr), min_(nullptr), max_(nullptr), value_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  値の構築
		*/
		//-----------------------------------------------------------------//
		void build_value()
		{
			value_.symbol_ = symbol_->get_text();
			value_.retry_ = retry_->get_select_pos();
			{
				int n = 0;
				utils::string_to_int(wait_->get_text(), n);
				value_.wait_ = n;
			}
			value_.term_ = term_->get_select_pos();
			{
				double a = 0.0f;
				utils::string_to_double(delay_->get_text(), a);
				value_.delay_ = a;
			}
			value_.filter_ = filter_->get_select_pos();
			{
				double a = 0.0f;
				utils::string_to_double(width_->get_text(), a);
				value_.width_ = a;
			}
			{
				double a = 0.0f;
				utils::string_to_double(min_->get_text(), a);
				value_.min_ = a;
			}
			{
				double a = 0.0f;
				utils::string_to_double(max_->get_text(), a);
				value_.max_ = a;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
			@param[in]	root	ルート
			@param[in]	d_w		横幅
			@param[in]	ofsx	オフセットＸ
			@param[in]	ofsy	オフセットＹ
		*/
		//-----------------------------------------------------------------//
		void init(gui::widget* root, int d_w, int ofsx, int ofsy)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
#if 0
			{  // 単体試験名
				widget::param wp(vtx::irect(ofsx, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_("", false);
				unit_name_ = wd.add_widget<widget_label>(wp, wp_);
			}
#endif
			{  // 検査記号
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_label::param wp_("", false);
				symbol_ = wd.add_widget<widget_label>(wp, wp_);
			}
			ofsy += 50;
			{  // リトライ回数
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_spinbox::param wp_(1, 1, 5);
				retry_ = wd.add_widget<widget_spinbox>(wp, wp_);
				retry_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
				widget::param wp(vtx::irect(ofsx + 100, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				wait_ = wd.add_widget<widget_label>(wp, wp_);
				wait_->at_local_param().select_func_ = [=](const std::string& str) {
					wait_->set_text(tools::limitf(str, 0.0f, 1.0f, "%3.2f"));
				};
			}
			{  // 計測対象選択
				widget::param wp(vtx::irect(ofsx + 200, ofsy, 90, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				wp_.init_list_.push_back("CH4");
				wp_.init_list_.push_back("DC2");
				wp_.init_list_.push_back("CRM");
				term_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // テスト 信号計測ポイント（時間）
				widget::param wp(vtx::irect(ofsx + 300, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				delay_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 計測信号フィルター
				widget::param wp(vtx::irect(ofsx + 400, ofsy, 90, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("SIG");
				wp_.init_list_.push_back("MIN");
				wp_.init_list_.push_back("MAX");
				wp_.init_list_.push_back("AVE");
				filter_ = wd.add_widget<widget_list>(wp, wp_);
				filter_->at_local_param().select_func_
					= [=](const std::string& text, uint32_t pos) {
					if(pos == 0) {
						width_->set_stall();
					} else {
						width_->set_stall(false);
					}
				};
			}
			{  // テスト 信号計測ポイント（時間）
				widget::param wp(vtx::irect(ofsx + 500, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				width_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // テスト MIN 値設定
				widget::param wp(vtx::irect(ofsx + 600, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				min_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // テスト MAX 値設定
				widget::param wp(vtx::irect(ofsx + 700, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				max_ = wd.add_widget<widget_label>(wp, wp_);
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
			if(symbol_->get_focus()) {
				tools::set_help(chip, symbol_, "検査記号");
			} else if(wait_->get_focus()) {
				tools::set_help(chip, wait_, "検査遅延: 0.0 to 1.0 [秒], 0.01 [秒] / step");
			} else if(retry_->get_focus()) {
				tools::set_help(chip, retry_, "検査リトライ回数");
			} else if(delay_->get_focus()) {
				auto ch = term_->get_select_pos() + 1;
				auto str = (boost::format("CH%d 検査ポイント（時間）") % ch).str();
				tools::set_help(chip, delay_, str);
			} else if(width_->get_focus()) {
				tools::set_help(chip, width_, "検査幅（時間）");
			} else if(min_->get_focus()) {
				tools::set_help(chip, min_, "検査：最低値");
			} else if(max_->get_focus()) {
				tools::set_help(chip, max_, "検査：最大値");
			} else {
				ret = false;
			}
#if 0
			else if(unit_name_->get_focus()) {
				tools::set_help(chip_, unit_name_, "単体試験名");
#endif
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
			symbol_->save(pre);
			retry_->save(pre);
			wait_->save(pre);
			term_->save(pre);
			delay_->save(pre);
			filter_->save(pre);
			width_->save(pre);
			min_->save(pre);
			max_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			symbol_->load(pre);
			retry_->load(pre);
			wait_->load(pre);
			term_->load(pre);
			delay_->load(pre);
			filter_->load(pre);
			width_->load(pre);
			min_->load(pre);
			max_->load(pre);
		}
	};
}
