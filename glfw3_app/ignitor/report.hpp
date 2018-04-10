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
#include "widgets/widget_image.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_chip.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "gl_fw/glmobj.hpp"
#include "img_io/img_files.hpp"

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

		gui::widget_image*		wav_image_;

		gui::widget_text*		str_title_;
		gui::widget_text*		str_date_;

		gui::widget_text*		str_result_;

		gui::widget_label*		str_total_;
		gui::widget_label*		str_pass_;
		gui::widget_label*		str_fail_;
		gui::widget_label*		str_retry_;

		gui::widget_spinbox*	csv_idx_;

		gui::widget_table*		table_;

		struct unit_t {
			gui::widget_label*	symbol_;  ///< 検査記号
			gui::widget_label*	okng_;    ///< 合否
			gui::widget_label*	retry_;   ///< リトライ回数
			gui::widget_label*	val_;     ///< 測定値
			gui::widget_label*	min_;     ///< 最小値
			gui::widget_label*	max_;     ///< 最大値
			gui::widget_label*	unit_;    ///< 単位
			unit_t() : symbol_(nullptr), okng_(nullptr), retry_(nullptr),
				val_(nullptr), min_(nullptr), max_(nullptr),
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

		uint32_t	index_;

		gl::mobj	mobj_;

		void add_unit_label_(gui::widget* frame, int x, int y, int h, unit_t& t)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			static const int wtbls[] = { 200, 50, 80, 100, 100, 100, 70 };
			int xx = x;
			for(uint32_t i = 0; i < 7; ++i) {
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
				else if(i == 1) t.okng_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 2) t.retry_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 3) t.val_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 4) t.min_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 5) t.max_ = wd.add_widget<widget_label>(wp, wp_);
				else if(i == 6) t.unit_ = wd.add_widget<widget_label>(wp, wp_);
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
			wav_image_(nullptr),
			str_title_(nullptr), str_date_(nullptr), str_result_(nullptr),
			str_total_(nullptr), str_pass_(nullptr), str_fail_(nullptr), str_retry_(nullptr),
			csv_idx_(nullptr), table_(nullptr),
			units_(), ena_(false),
			total_(0), pass_(0), fail_(0), retry_(0),
			index_(1), mobj_()
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
			@param[in]	vers	バージョン
		*/
		//-----------------------------------------------------------------//
		void init(int vers)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int ww = 1440;
			int hh = 730;
			{  // コントローラー設定ダイアログ
				widget::param wp(vtx::irect(100, 100, ww, hh));
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_dialog::param wp_(widget_dialog::style::OK);
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
//				dialog_->at_local_param().select_func_ = [=](bool ok) {
//				};
			}

			int x = 20;
			int y = 20;
			{
				widget::param wp(vtx::irect(x, y + 50, 660, 660), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_image::param wp_;
				wav_image_ = wd.add_widget<widget_image>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x, y, 200, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_text::param wp_;
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				str_title_ = wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x + 300, y, 150, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				auto s = (boost::format("Ver %d.%02d") % (vers / 100) % (vers % 100)).str();
				widget_text::param wp_(s);
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(x + 700, y + 50, 300, 100), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_text::param wp_("OK");
				wp_.text_param_.font_size_ = 96;
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				str_result_ = wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ww - 280, y, 270, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_text::param wp_;
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				str_date_ = wd.add_widget<widget_text>(wp, wp_);
			}

			y += 50;
			{
				widget::param wp(vtx::irect(ww - 260, y, 90, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_text::param wp_("Total:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ww - 165, y, 150, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_label::param wp_;
				str_total_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 40;
			{
				widget::param wp(vtx::irect(ww - 265, y, 90, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_text::param wp_("Pass:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ww - 165, y, 150, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_label::param wp_;
				str_pass_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 40;
			{
				widget::param wp(vtx::irect(ww - 265, y, 90, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_text::param wp_("Fail:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ww - 165, y, 150, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_label::param wp_;
				str_fail_ = wd.add_widget<widget_label>(wp, wp_);
			}
			y += 40;
			{
				widget::param wp(vtx::irect(ww - 265, y, 90, 35), dialog_);
				widget_text::param wp_("Retry:");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT,
									 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ww - 165, y, 150, 35), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_label::param wp_;
				str_retry_ = wd.add_widget<widget_label>(wp, wp_);
			}
			static const int lh = 32;  // ラベルの高さ
			static const int lw = 700;
			x = ww - lw - 24 - 15;
			{  // 試料番号設定
				{
					widget::param wp(vtx::irect(x, y, 70, 40), dialog_);
					wp.pre_group_ = widget::PRE_GROUP::_4;
					widget_text::param wp_("試料:");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT,
										 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);	
				}
				widget::param wp(vtx::irect(x + 80, y, 130, 40), dialog_);
				wp.pre_group_ = widget::PRE_GROUP::_4;
				widget_spinbox::param wp_(1, 1, 1000);
				csv_idx_ = wd.add_widget<widget_spinbox>(wp, wp_);
				csv_idx_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					index_ = newpos;
					ena_ = false;
					return (boost::format("%d") % newpos).str();
				};
			}
			y += 50;

			add_unit_label_(dialog_, x, y, lh, units_str_);
	  		units_str_.symbol_->set_text("検査項目");
	  		units_str_.okng_->set_text("合否");
	  		units_str_.retry_->set_text("リトライ");
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
				table_->at_local_param().select_func_ = [=](uint32_t pos, uint32_t id) {
					auto path = project_.get_image_path(pos);
					if(utils::probe_file(path)) {
						img::img_files imfs;
						if(imfs.load(path)) {
							mobj_.destroy();
							mobj_.initialize();
							auto img = imfs.get_image();
							auto isz = img.get()->get_size();
							auto sz = wav_image_->get_rect().size;
							float sx = static_cast<float>(sz.x) / static_cast<float>(isz.x);
							float sy = static_cast<float>(sz.y) / static_cast<float>(isz.y);
							wav_image_->at_local_param().scale_.x = sx < sy ? sx : sy;
							wav_image_->at_local_param().scale_.y = sx < sy ? sx : sy;
							wav_image_->at_local_param().mobj_handle_ = mobj_.install(img.get());
						}
					}
				};
			}

			mobj_.initialize();
			wav_image_->at_local_param().mobj_ = mobj_;
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
				{
					auto ts = project_.get_csv1_timestamp();
					struct tm* t = localtime(&ts);
					auto s = (boost::format("%4d年") % (t->tm_year + 1900)).str();
					s += (boost::format("%d月") % (t->tm_mon + 1)).str();
					s += (boost::format("%d日") % (t->tm_mday)).str();
					s += (boost::format(" %2d") % (t->tm_hour)).str();
					s += (boost::format(":%02d") % (t->tm_min)).str();
					s += (boost::format(":%02d") % (t->tm_sec)).str();
					str_date_->set_text(s);
				}
				str_title_->set_text(project_.get_project_title());
				total_ = 0;
				pass_ = 0;
				fail_ = 0;
				uint32_t nocnt = 0;
				for(uint32_t i = 0; i < 50; ++i) {
					auto sym = c.get(i + 1, 1);
					if(sym.empty()) break;
					units_[i].symbol_->set_text(sym);
					std::string ret = "0";  // リトライ
					units_[i].retry_->set_text(ret);
					auto vals = c.get(i + 1, 9 + index_ - 1);
					units_[i].val_->set_text(vals);
					auto mins = c.get(i + 1, 3);
					units_[i].min_->set_text(mins);
					auto maxs = c.get(i + 1, 2);
					units_[i].max_->set_text(maxs);
					if(!vals.empty() && !mins.empty() && !maxs.empty()) {
						float val;
						utils::input("%f", vals.c_str()) % val;
						float min;
						utils::input("%f", mins.c_str()) % min;
						float max;
						utils::input("%f", maxs.c_str()) % max;
						if(min <= val && val <= max) {
							units_[i].okng_->set_text("○");
							++pass_;
						} else {
							units_[i].okng_->set_text("×");
							++fail_;
						}
					} else {
						units_[i].okng_->set_text("－");
						++nocnt;
					}
					units_[i].unit_->set_text(c.get(i + 1, 4));
					++total_;
				}
				if(total_ == nocnt) {
					str_result_->set_text("Ready");
				} else if(total_ == pass_) {  
					str_result_->set_text("All OK");
				} else {
					auto s = (boost::format("NG %d") % (total_ - pass_)).str();
					str_result_->set_text(s);
				}
			}
			ena_ = ena;

			str_total_->set_text((boost::format("%d") % total_).str());
			str_pass_->set_text((boost::format("%d") % pass_).str());
			str_fail_->set_text((boost::format("%d") % fail_).str());
			str_retry_->set_text((boost::format("%d") % retry_).str());
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
