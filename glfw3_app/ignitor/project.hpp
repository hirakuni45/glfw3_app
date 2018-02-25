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

		gui::widget_dialog*		name_dialog_;
		gui::widget_label*		proj_name_;
		gui::widget_label*		proj_dir_;

		gui::widget_dialog*		dialog_;
		gui::widget_label*		csv_name_;
		gui::widget_list*		image_ext_;

		gui::widget_label*		pbase_;
		gui::widget_label*		pext_;
		gui::widget_label*		pname_[50];
		gui::widget_text*		help_;

		gui::widget_dialog*		msg_dialog_;

		std::string				proj_title_;
		std::string				root_path_;

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
			name_dialog_(nullptr), proj_name_(nullptr), proj_dir_(nullptr),
			dialog_(nullptr),
			csv_name_(nullptr),
			pbase_(nullptr), pext_(nullptr),
			pname_{ nullptr }, help_(nullptr),
			msg_dialog_(nullptr), proj_title_(), root_path_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト名ダイアログの取得
			@return プロジェクト名ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_name_dialog() { return name_dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・タイトルの取得
			@return プロジェクト・タイトル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_project_title() const { return proj_title_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ルート・パスの取得
			@return ルート・パス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_root_path() const { return root_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・ダイアログの取得
			@return プロジェクト・ダイアログ
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

			{  // プロジェクト名入力ダイアログ
				int w = 300;
				int h = 260;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				name_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				name_dialog_->enable(false);
				name_dialog_->at_local_param().select_func_ = [=](bool ok) {
					proj_title_.clear();
					root_path_.clear();
					if(ok) {
						auto s = proj_name_->get_text();
						if(s.empty()) {
							msg_dialog_->set_text(
								"プロジェクトのタイトル\nが指定されていません。");
							msg_dialog_->enable();
							return;
						}
						s = proj_dir_->get_text();
						if(s.empty()) {
							msg_dialog_->set_text(
								"プロジェクトのルートフォルダ\nが指定されていません。");
							msg_dialog_->enable();
							return;
						} else {
							auto path = proj_dir_->get_text();
							if(!utils::is_directory(path)) {
								msg_dialog_->set_text((boost::format(
									"プロジェクトのルートフォルダ\n"
									"%s\n"
									"がありません。") % path).str());
								msg_dialog_->enable();
								return;
							}
						}
						proj_title_ = proj_name_->get_text();
						root_path_ = proj_dir_->get_text();
					}
				};
				{
					widget::param wp(vtx::irect(10, 10, w - 10 * 2, 40), name_dialog_);
					widget_text::param wp_("プロジェクト名：");
					wd.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 50, w - 10 * 2, 40), name_dialog_);
					widget_label::param wp_("", false);
					proj_name_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 100, w - 10 * 2, 40), name_dialog_);
					widget_text::param wp_("ルートフォルダ：");
					wd.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 140, w - 10 * 2, 40), name_dialog_);
					widget_label::param wp_("", false);
					proj_dir_ = wd.add_widget<widget_label>(wp, wp_);
				}
			}

			{  // 単体試験設定ダイアログ
				int w = 1020;
				int h = 720;

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
				{

					widget::param wp(vtx::irect(20 + 70 + 170, yy, 100, 40), dialog_);
					widget_list::param wp_;
					wp_.init_list_.push_back("JPEG");
					wp_.init_list_.push_back("PNG");
					wp_.init_list_.push_back("BMP");
					image_ext_ = wd.add_widget<widget_list>(wp, wp_);
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

			{  // メッセージ・ダイアログ
				int w = 400;
				int h = 200;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				msg_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				msg_dialog_->enable(false);
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
			if(csv_name_ == nullptr) return false;
			if(image_ext_ == nullptr) return false;

			dialog_->save(pre);
			csv_name_->save(pre);
			image_ext_->save(pre);
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
			if(csv_name_ == nullptr) return false;
			if(image_ext_ == nullptr) return false;

			dialog_->load(pre);
			csv_name_->load(pre);
			image_ext_->load(pre);
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
