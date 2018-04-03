#pragma once
//=====================================================================//
/*! @file
    @brief  Report クラス
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
#include "project.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  report クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class report
	{
		utils::director<core>&	director_;
		project&				project_;

		gui::widget_dialog*		dialog_;

		gui::widget_label*		msg_total_;
		gui::widget_label*		msg_pass_;
		gui::widget_label*		msg_fail_;
		gui::widget_label*		msg_retry_;

//		gui::widget_spinbox*	csv_idx_;

		gui::widget_table*		table_;

		struct unit_t {
			gui::widget_label*	symbol_;
			gui::widget_label*	val_;
			gui::widget_label*	min_;
			gui::widget_label*	max_;
			gui::widget_label*	unit_;
			unit_t() : symbol_(nullptr), val_(nullptr), min_(nullptr), max_(nullptr),
				unit_(nullptr) { }
		};
		unit_t					units_str_;
		typedef std::vector<unit_t> UNITS;
		UNITS					units_;

		bool		ena_;

		uint32_t	total_;
		uint32_t	pass_;
		uint32_t	fail_;
		uint32_t	retry_;

		void add_unit_label_(gui::widget* frame, int x, int y, int h, unit_t& t)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			static const int wtbls[] = { 250, 100, 100, 100, 100 };
			int xx = x;
			for(uint32_t i = 0; i < 5; ++i) {
				widget::param wp(vtx::irect(xx, y, wtbls[i], h), frame);
				xx += wtbls[i];
				wp.pre_group_ = widget::PRE_GROUP::_4;
				wp.state_.set(widget::state::CLIP_PARENTS);
				widget_label::param wp_;
				if(i & 1) {
					wp_.color_param_.fore_color_ *= 0.9f;
					wp_.color_param_.back_color_ *= 0.9f;
				}
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wp_.plate_param_.frame_width_ = 0;
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.resizeble_ = true;
				if(i == 0) t.symbol_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 1) t.val_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 2) t.min_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 3) t.max_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 4) t.unit_ = wd.add_widget<widget_label>(wp, wp_);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		report(utils::director<core>& d, project& proj) :
			director_(d), project_(proj),
			dialog_(nullptr),
			msg_total_(nullptr), msg_pass_(nullptr), msg_fail_(nullptr), msg_retry_(nullptr),
			table_(nullptr),
			units_(), ena_(false),
			total_(0), pass_(0), fail_(0), retry_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get() const { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void init()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{  // コントローラー設定ダイアログ
				int w = 840;
				int h = 680;
				widget::param wp(vtx::irect(100, 100, w, h));
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_dialog::param wp_(widget_dialog::style::OK);
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [=](bool ok) {
				};
			}

			int x = 20;
			int y = 20;
			{
				widget::param wp(vtx::irect(x + 560, y, 90, 35), dialog_);
				widget_text::param wp_("Total:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x + 650, y, 150, 35), dialog_);
				widget_label::param wp_;
				msg_total_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 40;
			{
				widget::param wp(vtx::irect(x + 560, y, 90, 35), dialog_);
				widget_text::param wp_("Pass:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x + 650, y, 150, 35), dialog_);
				widget_label::param wp_;
				msg_pass_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 40;
			{
				widget::param wp(vtx::irect(x + 560, y, 90, 35), dialog_);
				widget_text::param wp_("Fail:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x + 650, y, 150, 35), dialog_);
				widget_label::param wp_;
				msg_fail_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 40;
			{
				widget::param wp(vtx::irect(x + 560, y, 90, 35), dialog_);
				widget_text::param wp_("Retry:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x + 650, y, 150, 35), dialog_);
				widget_label::param wp_;
				msg_retry_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 50;

			static const int lh = 32;  // ラベルの高さ
			static const int lw = 650;
			add_unit_label_(dialog_, x, y, lh, units_str_);
	  		units_str_.symbol_->set_text("検査項目");
	  		units_str_.val_->set_text("結果");
	  		units_str_.min_->set_text("最小");
	  		units_str_.max_->set_text("最大");
	  		units_str_.unit_->set_text("単位");
			y += lh;
			{
				widget::param wpt(vtx::irect(x, y, lw + 24, lh * 12), dialog_);
				wpt.pre_group_ = widget::PRE_GROUP::_4;
				widget_table::param wpt_;
				wpt_.scroll_bar_v_ = true;
				wpt_.item_size_.set(lw, lh);
				for(int i = 0; i < 50; ++i) {
					widget_null* frame;
					{
						widget::param wp(vtx::irect(0, i * lh, lw, lh));
						wp.pre_group_ = widget::PRE_GROUP::_4;
						widget_null::param wp_;
						frame = wd.add_widget<widget_null>(wp, wp_);
					}
					unit_t t;
					add_unit_label_(frame, 0, 0, lh, t);
					wpt_.cell_.push_back(frame);
					units_.push_back(t);
				}
				table_ = wd.add_widget<widget_table>(wpt, wpt_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			bool ena = dialog_->get_enable();
			if(!ena_ && ena) {
				csv& c = project_.at_csv1();
				uint32_t idx = 0;
				for(uint32_t i = 0; i < 50; ++i) {
					units_[i].symbol_->set_text(c.get(i + 1, 1));
					units_[i].val_->set_text(c.get(idx + 1, 9 + i));
					units_[i].min_->set_text(c.get(i + 1, 3));
					units_[i].max_->set_text(c.get(i + 1, 2));
					units_[i].unit_->set_text(c.get(i + 1, 4));
				}
			}
			ena_ = ena;

			msg_total_->set_text((boost::format("%d") % total_).str());
			msg_pass_->set_text((boost::format("%d") % pass_).str());
			msg_fail_->set_text((boost::format("%d") % fail_).str());
			msg_retry_->set_text((boost::format("%d") % retry_).str());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
			dialog_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			dialog_->load(pre);
		}
	};
}
