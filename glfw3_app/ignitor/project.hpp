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

#include "csv.hpp"

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
		gui::widget_label*		csv_all_;
		gui::widget_spinbox*	csv_idx_;
		gui::widget_label*		csv_base_;
		gui::widget_list*		image_ext_;

		gui::widget_label*		pbase_;
		gui::widget_label*		pext_;
		gui::widget_check*		auto_save_;
		gui::widget_label*		pname_[50];
		gui::widget_text*		help_;

		gui::widget_dialog*		msg_dialog_;


		csv						csv1_;
		csv						csv2_;


		std::string get_path_(uint32_t no) const {
			std::string s;
			if(pname_[no] == nullptr) return s;
			if(pname_[no]->get_text().empty()) return s;

			s += proj_dir_->get_text();
			s += '/';
			s += pbase_->get_text();
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
			csv_all_(nullptr), csv_idx_(nullptr), csv_base_(nullptr), image_ext_(nullptr),
			pbase_(nullptr), pext_(nullptr), auto_save_(nullptr),
			pname_{ nullptr }, help_(nullptr),
			msg_dialog_(nullptr)
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
		std::string get_project_title() const {
			if(proj_name_ == nullptr) return "";
			return proj_name_->get_text();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ルート・パスの取得
			@return ルート・パス
		*/
		//-----------------------------------------------------------------//
		std::string get_project_path() const {
			if(proj_dir_ == nullptr) return "";
			return proj_dir_->get_text();
		}

#if 0
		//-----------------------------------------------------------------//
		/*!
			@brief  CSV ファイル名の取得
			@return CSV ファイル名
		*/
		//-----------------------------------------------------------------//
		std::string get_csv_name() const {
			if(csv_name_ == nullptr) return "";
			return csv_name_->get_text();
		}
#endif

		//-----------------------------------------------------------------//
		/*!
			@brief  CSV ファイル・インデックスの取得
			@return CSV ファイル・インデックス
		*/
		//-----------------------------------------------------------------//
		uint32_t get_csv_index() const { return csv_idx_->get_select_pos(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・ダイアログの取得
			@return プロジェクト・ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  有効なテスト・ユニットの数を取得
			@return 有効なテスト・ユニットの数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_unit_count() const
		{
			uint32_t num = 0;
			for(uint32_t i = 0; i < 50; ++i) {
				auto path = get_unit_name(i);
				if(path.empty()) continue;
				if(utils::probe_file(path)) {
					++num;
				}
			}
			return num; 
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ユニット名の取得
			@param[in]	no	テスト番号（０～４９）
			@return テスト名
		*/
		//-----------------------------------------------------------------//
		std::string get_unit_name(uint32_t no) const {
			if(no >= 50) return "";
			return get_path_(no);
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief  自動セーブの取得
			@return 自動セーブ
		*/
		//-----------------------------------------------------------------//
		bool get_autosave() const { return auto_save_->get_check(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 参照
			@return CSV1
		*/
		//-----------------------------------------------------------------//
		csv& at_csv1() { return csv1_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV2 参照
			@return CSV2
		*/
		//-----------------------------------------------------------------//
		csv& at_csv2() { return csv2_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 のロード
		*/
		//-----------------------------------------------------------------//
		void load_csv1()
		{
			auto& core = gl::core::get_instance();
			std::string path = core.get_current_path();
			path += '/';
			path += proj_dir_->get_text();
			path += '/';
			path += csv_all_->get_text();
			path += ".csv";
			csv1_.load(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 のセーブ
		*/
		//-----------------------------------------------------------------//
		void save_csv1()
		{
			auto& core = gl::core::get_instance();
			std::string path = core.get_current_path();
			path += '/';
			path += proj_dir_->get_text();
			path += '/';
			path += csv_all_->get_text();
			path += ".csv";
// std::cout << path << std::endl;
			csv1_.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV2 のセーブ
		*/
		//-----------------------------------------------------------------//
		void save_csv2()
		{
			auto& core = gl::core::get_instance();
			std::string path = core.get_current_path();
			path += '/';
			path += proj_dir_->get_text();
			path += '/';
			path += csv_base_->get_text();
			path += (boost::format("%04d") % csv_idx_->get_select_pos()).str();
			path += ".csv";
// std::cout << path << std::endl;
			csv2_.save(path);
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
				{  // CSV ALL 名設定
					{
						widget::param wp(vtx::irect(20, yy, 100, 40), dialog_);
						widget_text::param wp_("CSV(1):");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 110, yy, 150, 40), dialog_);
					widget_label::param wp_("", false);
					csv_all_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // 試料番号設定
					{
						widget::param wp(vtx::irect(20 + 280, yy, 70, 40), dialog_);
						widget_text::param wp_("試料:");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 350, yy, 130, 40), dialog_);
					widget_spinbox::param wp_(1, 1, 1000);
					csv_idx_ = wd.add_widget<widget_spinbox>(wp, wp_);
					csv_idx_->at_local_param().select_func_ =
						[=](widget_spinbox::state st, int before, int newpos) {
						return (boost::format("%d") % newpos).str();
					};
				}
				{  // CSV BASE 名設定
					{
						widget::param wp(vtx::irect(20 + 500, yy, 90, 40), dialog_);
						widget_text::param wp_("CSV(2):");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 600, yy, 150, 40), dialog_);
					widget_label::param wp_("", false);
					csv_base_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(20 + 770, yy, 100, 40), dialog_);
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
				{
					widget::param wp(vtx::irect(320 + 100 + 160, yy, 130, 40), dialog_);
					widget_check::param wp_("自動保存");
					auto_save_ = wd.add_widget<widget_check>(wp, wp_);
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

			// CSV1 定型
			uint32_t num = 100;
			csv1_.create(51, 8 + num);
			csv1_.set(0, 0, "検査番号");
			for(uint32_t i = 0; i < 50; ++i) {
				csv1_.set(i + 1, 0, (boost::format("%d") % (i + 1)).str());
			}
			csv1_.set(0, 1, "検査項目");
			csv1_.set(0, 2, "規格 MAX");
			csv1_.set(0, 3, "規格 MIN");
			csv1_.set(0, 4, "単位");
			csv1_.set(0, 5, "MAX");
			csv1_.set(0, 6, "MIN");
			csv1_.set(0, 7, "AVG");
			csv1_.set(0, 8, "NG");
			for(uint32_t i = 0; i < num; ++i) {
				csv1_.set(0, 9 + i, (boost::format("%d") % (i + 1)).str());
			}

			csv2_.create(51, 30);
			csv2_.set(0, 0, "検査番号");
			for(uint32_t i = 0; i < 50; ++i) {
				csv2_.set(i + 1, 0, (boost::format("%d") % (i + 1)).str());
			}
			csv2_.set(0,  1, "検査項目");
			csv2_.set(0,  2, "検査規格 MAX");
			csv2_.set(0,  3, "検査規格 MIN");
			csv2_.set(0,  4, "単位");
			csv2_.set(0,  5, "検査結果");
			csv2_.set(0,  6, "Time/Div");
			csv2_.set(0,  7, "CH1 A/Div");
			csv2_.set(0,  8, "CH2 V/Div");
			csv2_.set(0,  9, "CH3 V/Div");
			csv2_.set(0, 10, "CH4 KV/Div");
			csv2_.set(0, 11, "画像ファイル");
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
			if(auto_save_ == nullptr) return false;
			if(csv_all_ == nullptr) return false;
			if(csv_idx_ == nullptr) return false;
			if(csv_base_ == nullptr) return false;
			if(image_ext_ == nullptr) return false;

			dialog_->save(pre);
			csv_all_->save(pre);
			csv_idx_->save(pre);
			csv_base_->save(pre);
			auto_save_->save(pre);
			image_ext_->save(pre);
			proj_name_->save(pre);
			proj_dir_->save(pre);

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
			if(auto_save_ == nullptr) return false;
			if(csv_all_ == nullptr) return false;
			if(csv_idx_ == nullptr) return false;
			if(csv_base_ == nullptr) return false;
			if(image_ext_ == nullptr) return false;

			dialog_->load(pre);
			csv_all_->load(pre);
			csv_idx_->load(pre);
			csv_base_->load(pre);
			auto_save_->load(pre);
			image_ext_->load(pre);
			proj_name_->load(pre);
			proj_dir_->load(pre);

			pbase_->load(pre);
			pext_->load(pre);
			for(int i = 0; i < 50; ++i) {
				if(pname_[i] == nullptr) return false;
				pname_[i]->load(pre);
			}

//			auto& core = gl::core::get_instance();
//			std::string cp = core.get_current_path();
//			std::cout << cp << std::endl;

			return true;
		}
	};
}
