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
			min_(nullptr), max_(nullptr), value_()
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

			{
				widget::param wp(vtx::irect(20, ofsy, 130, 40), root);
				widget_text::param wp_("試験設定:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
#if 0
			{  // 単体試験名
				widget::param wp(vtx::irect(ofsx, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_("", false);
				unit_name_ = wd.add_widget<widget_label>(wp, wp_);
			}
#endif
			{  // 検査記号
				widget::param wp(vtx::irect(ofsx, ofsy, 150, 40), root);
				widget_label::param wp_("", false);
				symbol_ = wd.add_widget<widget_label>(wp, wp_);
			}
			ofsx += 160;
			{  // リトライ回数
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_spinbox::param wp_(1, 1, 5);
				retry_ = wd.add_widget<widget_spinbox>(wp, wp_);
				retry_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			ofsx += 100;
			{  // Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				wait_ = wd.add_widget<widget_label>(wp, wp_);
				wait_->at_local_param().select_func_ = [=](const std::string& str) {
					wait_->set_text(tools::limitf(str, 0.0f, 10.0f, "%3.2f"));
				};
			}
			ofsx += 100;
			{  // テスト MIN 値設定
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_label::param wp_("0", false);
				min_ = wd.add_widget<widget_label>(wp, wp_);
			}
			ofsx += 100;
			{  // テスト MAX 値設定
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
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
				tools::set_help(chip, wait_, "検査遅延: [秒]");
			} else if(retry_->get_focus()) {
				tools::set_help(chip, retry_, "検査リトライ回数");
			} else if(min_->get_focus()) {
				tools::set_help(chip, min_, "検査：最低値 (Min)");
			} else if(max_->get_focus()) {
				tools::set_help(chip, max_, "検査：最大値 (Max)");
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
			min_->load(pre);
			max_->load(pre);
		}
	};
}
