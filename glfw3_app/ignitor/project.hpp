#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・プロジェクト設定クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <array>
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "img_io/img_files.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  イグナイター・プロジェクト設定クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class project {

		utils::director<core>&	director_;

		gui::widget_dialog*		dialog_;

		gui::widget_label*		csv_name_;

		gui::widget_label*		pbase_;
		gui::widget_label*		pext_;
		gui::widget_label*		pname_[50];
		gui::widget_text*		help_;

		std::string get_path_(uint32_t no) const {
			if(pname_[no]->get_text().empty()) return "";

			std::string s = pbase_->get_text();
			s += pname_[no]->get_text();
			s += pext_->get_text();
			if(!s.empty()) {
				auto& core = gl::core::get_instance();
				std::string path = core.get_current_path();
				path += '/';
				path += s;
				return path;
			}
			return s;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		project(utils::director<core>& d) : director_(d),
			dialog_(nullptr),
			csv_name_(nullptr),
			pbase_(nullptr), pext_(nullptr),
			pname_{ nullptr }, help_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト名の取得
			@param[in]	no	テスト番号（０～４９）
			@return テスト名
		*/
		//-----------------------------------------------------------------//
		std::string get_test_name(uint32_t no) const {
			if(no >= 50) return "";
			return get_path_(no);
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int w = 1020;
			int h = 720;
			{  // 単体試験設定ダイアログ
				widget::param wp(vtx::irect(120, 120, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);

				int yy = 20;
				{  // CSV 名設定
					{
						widget::param wp(vtx::irect(20, yy, 60, 40), dialog_);
						widget_text::param wp_("CSV：");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 60 + 10, yy, 150, 40), dialog_);
					widget_label::param wp_("", false);
					csv_name_ = wd.add_widget<widget_label>(wp, wp_);
				}
				yy += 50;
				{  // 単体試験ベース名設定
					{
						widget::param wp(vtx::irect(20, yy, 100, 40), dialog_);
						widget_text::param wp_("ベース名：");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 100 + 10, yy, 150, 40), dialog_);
					widget_label::param wp_("", false);
					pbase_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // 単体試験拡張子設定
					{
						widget::param wp(vtx::irect(320, yy, 90, 40), dialog_);
						widget_text::param wp_("拡張子：");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(320 + 90 + 10, yy, 150, 40), dialog_);
					widget_label::param wp_(".unt", false);
					pext_ = wd.add_widget<widget_label>(wp, wp_);
				}

				for(int i = 0; i < 50; ++i) {
					int x = (i / 10) * 200;
					int y = 40 + 10 + (i % 10) * 50;
					{
						widget::param wp(vtx::irect(x + 20, y + yy, 50, 40), dialog_);
						std::string no = (boost::format("%d:") % (i + 1)).str();
						widget_text::param wp_(no);
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);
					}
					widget::param wp(vtx::irect(x + 60, y + yy, 130, 40), dialog_);
					widget_label::param wp_("", false);
					pname_[i] = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(20, h - 100, w - 20 * 2, 40), dialog_);
					widget_text::param wp_;
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT,
										 vtx::placement::vertical::CENTER);
					help_ = wd.add_widget<widget_text>(wp, wp_);	
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(dialog_->get_state(gui::widget::state::ENABLE)) {
				std::string s;
				for(int i = 0; i < 50; ++i) {
					if(pname_[i]->get_focus()) {
						if(pname_[i]->get_text().empty()) continue;
						s = get_path_(i);
						if(utils::probe_file(s)) {
							s += " (find !)";
						} else {
							s += " (can't find)";
						}
						break;
					}
				}
				help_->set_text(s);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
			if(dialog_ == nullptr) return false;
			if(pbase_ == nullptr) return false;
			if(pext_ == nullptr) return false;

			dialog_->save(pre);
			pbase_->save(pre);
			pext_->save(pre);
			for(int i = 0; i < 50; ++i) {
				if(pname_[i] == nullptr) return false;
				pname_[i]->save(pre);
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre)
		{
			if(dialog_ == nullptr) return false;
			if(pbase_ == nullptr) return false;
			if(pext_ == nullptr) return false;

			dialog_->load(pre);
			pbase_->load(pre);
			pext_->load(pre);
			for(int i = 0; i < 50; ++i) {
				if(pname_[i] == nullptr) return false;
				pname_[i]->load(pre);
			}
			return true;
		}
	};
}
