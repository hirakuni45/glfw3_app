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
#include "csv.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  DIF クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dif
	{
		utils::director<core>&	director_;
		csv&					csv1_;

		gui::widget_label*		tag1_;	///< 検査記号１
		gui::widget_label*		val1_;	///< 値１
		gui::widget_list*		calc_;	///< 計算モード
		gui::widget_label*		tag2_;	///< 検査記号２
		gui::widget_label*		val2_;	///< 値２
		gui::widget_label*		ans_;	///< 答え
		gui::widget_button*		exec_;	///< 計算

		std::string				unit_;

		uint32_t				dif_id_;
		uint32_t				index_;

	public:
		float					dif_value_;

	private:

		void run_()
		{
			std::string unit;
			float v1 = 0.0f;
			{  // tag1
				auto pos = csv1_.find_colum(tag1_->get_text(), 1);
				if(pos >= csv1_.get_rows()) {
					ans_->set_text("? " + tag1_->get_text());
					return;
				}
				if(!(utils::input("%f", csv1_.get(pos, index_ - 1 + 9).c_str()) % v1).status()) {
					ans_->set_text("#? " + tag1_->get_text());
					return;
				}
				val1_->set_text((boost::format("%4.3f") % v1).str());
				unit = csv1_.get(pos, 4);
			}
			float v2 = 0.0f;
			{  // tag2
				auto pos = csv1_.find_colum(tag2_->get_text(), 1);
				if(pos >= csv1_.get_rows()) {
					ans_->set_text("? " + tag2_->get_text());
					return;
				}
				if(!(utils::input("%f", csv1_.get(pos, index_ - 1 + 9).c_str()) % v2).status()) {
					ans_->set_text("#? " + tag2_->get_text());
					return;
				}
				val2_->set_text((boost::format("%4.3f") % v2).str());
				if(unit != csv1_.get(pos, 4)) {
					unit += ',';
					unit += csv1_.get(pos, 4);
				}
			}
			float d = 0.0f;
			if(calc_->get_select_pos() == 0) {
				d = v1 - v2;
			} else {
				d = v1 + v2;
			}
			ans_->set_text((boost::format("%4.3f [%s]") % d % unit).str());
			unit_ = unit;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		dif(utils::director<core>& d, csv& csv1) :
			director_(d), csv1_(csv1),
			tag1_(nullptr), val1_(nullptr), calc_(nullptr), tag2_(nullptr), val2_(nullptr),
			ans_(nullptr), exec_(nullptr),
			unit_(), dif_id_(0), index_(0), dif_value_(0.0f)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  値の更新 ID 取得
			@return 値の更新 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_id() const { return dif_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  測定単位文字列の取得
			@return 測定単位文字列
		*/
		//-----------------------------------------------------------------//
		const std::string get_unit_str() const {
			return unit_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計算インデックスを設定
			@param[in]	index	計算インデックス
		*/
		//-----------------------------------------------------------------//
		void set_index(uint32_t index) { index_ = index; }


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
			}
			{  // VAL2
				widget::param wp(vtx::irect(ofsx + 190, ofsy + 50, 100, 40), root);
				wp.pre_group_ = widget::PRE_GROUP::_1;
				widget_label::param wp_("");
				val2_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 300, ofsy + 50, 150, 40), root);
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
					run_();
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
			tag1_->save(pre);
			calc_->save(pre);
			tag2_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			tag1_->load(pre);
			calc_->load(pre);
			tag2_->load(pre);
		}
	};
}

