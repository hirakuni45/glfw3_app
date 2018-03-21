#pragma once
//=====================================================================//
/*! @file
    @brief  DIF クラス
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
		@brief  DIF クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dif
	{
		utils::director<core>&	director_;

		gui::widget_label*		tag1_;	///< 検査記号１
		gui::widget_label*		val1_;	///< 値１
		gui::widget_list*		calc_;	///< 計算モード
		gui::widget_label*		tag2_;	///< 検査記号２
		gui::widget_label*		val2_;	///< 値２
		gui::widget_label*		ans_;	///< 答え
		gui::widget_button*		exec_;	///< 計算

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		dif(utils::director<core>& d) :
			director_(d),
			tag1_(nullptr), val1_(nullptr), calc_(nullptr), tag2_(nullptr), val2_(nullptr),
			ans_(nullptr), exec_(nullptr)
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
				widget::param wp(vtx::irect(15, ofsy, 120, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_text::param wp_("差分検査:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			ofsx += 50;
			{  // TAG1
				widget::param wp(vtx::irect(ofsx, ofsy, 100, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_label::param wp_("", false);
				tag1_ = wd.add_widget<widget_label>(wp, wp_);
//				tag1_->at_local_param().select_func_ = [=](const std::string& str) {
//				};
			}
			{  // VAL1
				widget::param wp(vtx::irect(ofsx, ofsy + 50, 100, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_label::param wp_("");
				val1_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 110, ofsy, 70, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_list::param wp_;
				wp_.init_list_.push_back("－");
				wp_.init_list_.push_back("＋");
				calc_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // TAG2
				widget::param wp(vtx::irect(ofsx + 190, ofsy, 100, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_label::param wp_("", false);
				tag2_ = wd.add_widget<widget_label>(wp, wp_);
//				tag2_->at_local_param().select_func_ = [=](const std::string& str) {
//				};
			}
			{  // VAL2
				widget::param wp(vtx::irect(ofsx + 190, ofsy + 50, 100, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_label::param wp_("");
				val2_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 300, ofsy + 50, 100, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_label::param wp_("");
				ans_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(d_w - 60, ofsy + 50, 30, 30), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
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
			if(tag1_->get_focus()) {
				tools::set_help(chip, tag1_, "検査記号１");
			} else if(tag2_->get_focus()) {
				tools::set_help(chip, tag2_, "検査記号２");
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
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
		}
	};
}

