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
#include "widgets/widget_table.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "img_io/img_files.hpp"
#include "utils/select_dir.hpp"
#include "utils/select_file.hpp"
#include "utils/files.hpp"

#include "csv.hpp"
#include "tools.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  イグナイター・プロジェクト設定クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class project {

		static constexpr const char* PROJECT_EXT_ = "ipr";

		utils::director<core>&	director_;

		gui::widget_dialog*		name_dialog_;
		gui::widget_label*		proj_name_;
		gui::widget_label*		proj_root_;

		gui::widget_dialog*		edit_dialog_;
		gui::widget_label*		csv_all_;
		gui::widget_spinbox*	csv_idx_;
		gui::widget_label*		csv_base_;
		gui::widget_list*		image_ext_;
		gui::widget_table*		files_core_;
		gui::widget_label*		log_name_;
		gui::widget_check*		wav_save_;

		gui::widget_dialog*		msg_dialog_;

		gui::widget_button*		proj_dir_;

		std::string				last_path_;

		utils::select_dir		sel_dir_;
		utils::select_file		proj_load_filer_;
		utils::select_file		proj_save_filer_;

		utils::files			files_;

		csv						csv1_;
		csv						csv2_;
		csv						retry_;

		bool save_project_file_(const std::string& path)
		{
			sys::preference pre;

			save(pre);
			auto ph = path;
			if(utils::get_file_ext(path).empty()) {
				ph += '.';
				ph += PROJECT_EXT_;
			}
			return pre.save(ph);
		}


		bool load_project_file_(const std::string& path)
		{
			sys::preference pre;
			auto ph = path;
			if(utils::get_file_ext(path).empty()) {
				ph += '.';
				ph += PROJECT_EXT_;
			}
			auto ret = pre.load(ph);
			if(ret) {
				load(pre);
			}
			return ret;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		project(utils::director<core>& d) : director_(d),
			name_dialog_(nullptr), proj_name_(nullptr), proj_root_(nullptr),
			edit_dialog_(nullptr),
			csv_all_(nullptr), csv_idx_(nullptr), csv_base_(nullptr), image_ext_(nullptr),
			files_core_(nullptr), log_name_(nullptr), wav_save_(nullptr),
			msg_dialog_(nullptr), proj_dir_(nullptr), last_path_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  名前ダイアログの有効、無効
			@param[in]	ena	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_name_dialog(bool ena = true) { name_dialog_->enable(ena); }


		//-----------------------------------------------------------------//
		/*!
			@brief  ユニット・リストの更新
		*/
		//-----------------------------------------------------------------//
		void update_unit_list()
		{
// std::cout << proj_root_->get_text() << std::endl;
			files_.set_path(proj_root_->get_text(), "unt");
			auto& cell = files_core_->get_local_param().cell_;
			for(auto t : cell) {  // 全てのセルを一度「空」にする。
				static_cast<gui::widget_label*>(t)->set_text("");
			}
			const auto& fis = files_.get();
			uint32_t i = 0;
			for(const auto& fi : fis) {
				if(fi.get_name() == ".") continue;
				else if(fi.get_name() == "..") continue;
				if(i < cell.size()) {
					gui::widget_label* w = static_cast<gui::widget_label*>(cell[i]);
					w->set_text(fi.get_name());
///					std::cout << fi.get_name() << std::endl;
					++i;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  編集ダイアログの有効、無効
			@param[in]	ena	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_edit_dialog(bool ena = true) {
			if(ena) {
				update_unit_list();
			}
			edit_dialog_->enable(ena);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・ロード・ファイル選択
		*/
		//-----------------------------------------------------------------//
		void open_load_file()
		{
			std::string filter = "プロジェクト(*.";
			filter += PROJECT_EXT_;
			filter += ")\t*.";
			filter += PROJECT_EXT_;
			filter += "\t";
			proj_load_filer_.open(filter, false, proj_root_->get_text());
			update_unit_list();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・セーブ・ファイル選択
		*/
		//-----------------------------------------------------------------//
		void open_save_file()
		{
			std::string filter = "プロジェクト(*.";
			filter += PROJECT_EXT_;
			filter += ")\t*.";
			filter += PROJECT_EXT_;
			filter += "\t";
			proj_save_filer_.open(filter, true, proj_root_->get_text());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・ファイルをセーブ
		*/
		//-----------------------------------------------------------------//
		bool save_proj()
		{
			if(last_path_.empty()) return false;
			return save_project_file_(last_path_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アイテム・カウントを更新
		*/
		//-----------------------------------------------------------------//
		void next_item()
		{
			auto n = csv_idx_->get_select_pos();
			++n;
			if(n < 1000) {
				csv_idx_->set_select_pos(n);
			}
		}


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
		std::string get_project_root() const {
			if(proj_root_ == nullptr) return "";
			return proj_root_->get_text();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  画像ファイル名の取得
			@return 画像ファイル名
		*/
		//-----------------------------------------------------------------//
		std::string get_image_path(uint32_t idx) const {
			auto path = get_project_root();
			std::string base = csv_base_->get_text();
			std::string ext = utils::to_lower_text(image_ext_->get_select_text());
			path += (boost::format("/%s%04d.%s") % base % idx % ext).str();
			return path; 
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV ファイル・インデックスの取得
			@return CSV ファイル・インデックス
		*/
		//-----------------------------------------------------------------//
		uint32_t get_csv_index() const { return csv_idx_->get_select_pos(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  有効なテスト・ユニットの数を取得
			@return 有効なテスト・ユニットの数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_unit_count() const
		{
			uint32_t num = 0;
			auto& cell = files_core_->get_local_param().cell_;
			for(auto t : cell) {  // 全てのセルを一度「空」にする。
				gui::widget_label* w = static_cast<gui::widget_label*>(t);
				auto path = w->get_text();
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
			auto& cell = files_core_->get_local_param().cell_;
			if(no < cell.size()) {
				gui::widget_label* w = static_cast<gui::widget_label*>(cell[no]);
				return w->get_text();
			} else {
				return "";
			}
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief  波形データ名の取得
			@param[in]	no	テスト番号（０～４９）
			@return 波形データ名
		*/
		//-----------------------------------------------------------------//
		std::string get_wave_name(uint32_t no) const {
			auto& cell = files_core_->get_local_param().cell_;
			if(no < cell.size()) {
				gui::widget_label* w = static_cast<gui::widget_label*>(cell[no]);
				auto str = w->get_text();
				return str;
			} else {
				return "";
			}
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 参照
			@return CSV1
		*/
		//-----------------------------------------------------------------//
		csv& at_csv1() { return csv1_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 ファイルのタイムスタンプ取得
			@return CSV1 ファイルのタイムスタンプ
		*/
		//-----------------------------------------------------------------//
		time_t get_csv1_timestamp() const {
			auto path = utils::append_path(proj_root_->get_text(), csv_all_->get_text());
			path += ".csv";
			struct stat st;
		    int ierr = stat(path.c_str(), &st);
			time_t t = 0;
		    if(ierr == 0) {
				t = st.st_mtime;
			}
			return t;
		}


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
			auto path = utils::append_path(proj_root_->get_text(), csv_all_->get_text());
			path += ".csv";
			csv1_.load(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 の統計計算
		*/
		//-----------------------------------------------------------------//
		void calc_csv1()
		{
			for(uint32_t c = 1; c < (1 + 50); ++c) {
				if(csv1_.get(c, 9).empty()) break;
				double ref_max;
				if(!utils::string_to_double(csv1_.get(c, 2), ref_max)) {
					break;
				}
				double ref_min;
				if(!utils::string_to_double(csv1_.get(c, 3), ref_min)) {
					break;
				}
				double min = 0.0;
				double max = 0.0;
				double avg = 0.0;
				bool init = false;
				uint32_t count = 0;
				uint32_t ng = 0;
				for(uint32_t r = 9; r < (9 + 1000); ++r) {
					double a = 0.0f;
					auto s = csv1_.get(c, r);
					if(s.empty()) break;
					if(utils::string_to_double(s, a)) {
						if(!init) {
							avg = min = max = a;
							init = true;
						} else {
							if(min > a) min = a;
							else if(max < a) max = a;
							avg += a;
						}
						++count;
						if(ref_min <= a && a <= ref_max) {
						} else {
							++ng;
						}
					}
				}
				if(count > 0) {
					avg /= static_cast<double>(count);
					csv1_.set(c, 5, tools::double_to_str(max));
					csv1_.set(c, 6, tools::double_to_str(min));
					csv1_.set(c, 7, tools::double_to_str(avg));
				}
				csv1_.set(c, 8, (boost::format("%d") % ng).str());
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 のセーブ
		*/
		//-----------------------------------------------------------------//
		void save_csv1()
		{
			auto path = utils::append_path(proj_root_->get_text(), csv_all_->get_text());
			path += ".csv";
			csv1_.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV1 のリセット
		*/
		//-----------------------------------------------------------------//
		void reset_csv1()
		{
			csv1_.clear();
			// CSV1 定型
			uint32_t num = 1000;
			csv1_.create(51, 8 + num);
//			csv1_.set(0, 0, "検査番号");
			csv1_.set(0, 0, "No.");
			for(uint32_t i = 0; i < 50; ++i) {
				csv1_.set(i + 1, 0, (boost::format("%d") % (i + 1)).str());
			}

//			csv1_.set(0, 1, "検査項目");
			csv1_.set(0, 1, "Item");
//			csv1_.set(0, 2, "規格 MAX");
			csv1_.set(0, 2, "Spec.Max");
//			csv1_.set(0, 3, "規格 MIN");
			csv1_.set(0, 3, "Spec.Min");
//			csv1_.set(0, 4, "単位");
			csv1_.set(0, 4, "Unit");
			csv1_.set(0, 5, "MAX");
			csv1_.set(0, 6, "MIN");
			csv1_.set(0, 7, "AVG");
			csv1_.set(0, 8, "NG");
			for(uint32_t i = 0; i < num; ++i) {
				csv1_.set(0, 9 + i, (boost::format("%d") % (i + 1)).str());
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV2 のリセット
		*/
		//-----------------------------------------------------------------//
		void reset_csv2()
		{
			csv2_.clear();
			csv2_.create(51, 30);
//			csv2_.set(0, 0, "検査番号");
			csv2_.set(0, 0, "No.");
			for(uint32_t i = 0; i < 50; ++i) {
				csv2_.set(i + 1, 0, (boost::format("%d") % (i + 1)).str());
			}
//			csv2_.set(0,  1, "検査項目");
			csv2_.set(0,  1, "Item");
//			csv2_.set(0,  2, "検査規格 MAX");
			csv2_.set(0,  2, "Spec.Max");
//			csv2_.set(0,  3, "検査規格 MIN");
			csv2_.set(0,  3, "Spec.Min");
//			csv2_.set(0,  4, "単位");
			csv2_.set(0,  4, "Unit");
//			csv2_.set(0,  5, "検査結果");
			csv2_.set(0,  5, "Result");
			csv2_.set(0,  6, "Time/Div");
			csv2_.set(0,  7, "CH1 A/Div");
			csv2_.set(0,  8, "CH2 V/Div");
			csv2_.set(0,  9, "CH3 V/Div");
			csv2_.set(0, 10, "CH4 KV/Div");
//			csv2_.set(0, 11, "画像ファイル");
			csv2_.set(0, 11, "PhotoFile");
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV2 が有効か検査
			@return 有効な場合「true」
		*/
		//-----------------------------------------------------------------//
		bool probe_csv2() const {
			return !csv_base_->get_text().empty();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV2 のロード
		*/
		//-----------------------------------------------------------------//
		void load_csv2()
		{
			auto path = utils::append_path(proj_root_->get_text(), csv_base_->get_text());
			path += ".csv";
			csv2_.load(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  CSV2 のセーブ
		*/
		//-----------------------------------------------------------------//
		void save_csv2()
		{
			if(!probe_csv2()) {
				return;
			}
			auto path = utils::append_path(proj_root_->get_text(), csv_base_->get_text());
			path += ".csv";
			csv2_.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  RETRY 参照
			@return RETRY
		*/
		//-----------------------------------------------------------------//
		csv& at_retry() { return retry_; }




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
				int w = 500;
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
						s = proj_root_->get_text();
						if(s.empty()) {
							msg_dialog_->set_text(
								"プロジェクトのルートフォルダ\nが指定されていません。");
							msg_dialog_->enable();
							return;
						} else {
							auto path = proj_root_->get_text();
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
					widget::param wp(vtx::irect(10, 20, 400, 40), name_dialog_);
					widget_text::param wp_("プロジェクト名：");
					wd.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 50, w - 10 * 2, 40), name_dialog_);
					widget_label::param wp_("", false);
					proj_name_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 120, 400, 40), name_dialog_);
					widget_text::param wp_("プロジェクト・フォルダ：");
					wd.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(330, 100, 160, 40), name_dialog_);
					widget_button::param wp_("フォルダ選択");
					proj_dir_ = wd.add_widget<widget_button>(wp, wp_);
					proj_dir_->at_local_param().select_func_ = [=](uint32_t) {
						sel_dir_.open("プロジェクト・フォルダ選択", proj_root_->get_text()); 
					};
				}
				{
					widget::param wp(vtx::irect(10, 150, w - 10 * 2, 40), name_dialog_);
					auto& core = gl::core::get_instance();
					widget_label::param wp_(core.get_current_path(), false);
					proj_root_ = wd.add_widget<widget_label>(wp, wp_);
				}
			}

			{  // 全体試験設定ダイアログ
				int w = 600;
				int h = 630;

				widget::param wp(vtx::irect(120, 120, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				edit_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				edit_dialog_->enable(false);

				int yy = 20;
				{  // CSV ALL 名設定
					{
						widget::param wp(vtx::irect(20, yy, 100, 40), edit_dialog_);
						widget_text::param wp_("CSV(1):");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 110, yy, 150, 40), edit_dialog_);
					widget_label::param wp_("", false);
					csv_all_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // 試料番号設定
					{
						widget::param wp(vtx::irect(20 + 280, yy, 70, 40), edit_dialog_);
						widget_text::param wp_("試料:");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 350, yy, 130, 40), edit_dialog_);
					widget_spinbox::param wp_(1, 1, 1000);
					csv_idx_ = wd.add_widget<widget_spinbox>(wp, wp_);
					csv_idx_->at_local_param().select_func_ =
						[=](widget_spinbox::state st, int before, int newpos) {
						return (boost::format("%d") % newpos).str();
					};
				}

				yy += 50;
				{  // CSV BASE 名設定
					{
						widget::param wp(vtx::irect(20, yy, 90, 40), edit_dialog_);
						widget_text::param wp_("CSV(2):");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 110, yy, 150, 40), edit_dialog_);
					widget_label::param wp_("", false);
					csv_base_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					{
						widget::param wp(vtx::irect(20 + 280, yy, 70, 40), edit_dialog_);
						widget_text::param wp_("画像:");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 350, yy, 100, 40), edit_dialog_);
					widget_list::param wp_;
					wp_.init_list_.push_back("JPEG");
					wp_.init_list_.push_back("PNG");
					wp_.init_list_.push_back("BMP");
					image_ext_ = wd.add_widget<widget_list>(wp, wp_);
				}

				yy += 50;
				static const int lh = 32;  // ラベルの高さ
				static const int lw = 530;
				{
					widget::param wpt(vtx::irect(20, yy, lw, lh * 12), edit_dialog_);
					widget_table::param wpt_;
					wpt_.scroll_bar_v_ = true;
					wpt_.item_size_.set(500, 32);
					for(int i = 0; i < 50; ++i) {
						widget::param wp(vtx::irect(0, i * lh, lw, lh));
						widget_label::param wp_;
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wp_.plate_param_.frame_width_ = 0;
						wp_.plate_param_.round_radius_ = 0;
						wp_.plate_param_.resizeble_ = true;
						wpt_.cell_.push_back(wd.add_widget<widget_label>(wp, wp_));
					}
					files_core_ = wd.add_widget<widget_table>(wpt, wpt_);
				}
				yy += lh * 12 + 10;
				{  // ログファイル名
					{
						widget::param wp(vtx::irect(20, yy, 90, 40), edit_dialog_);
						widget_text::param wp_("ログ名:");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 110, yy, 150, 40), edit_dialog_);
					widget_label::param wp_("", false);
					log_name_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(20 + 290, yy, 140, 40), edit_dialog_);
					wp.pre_group_ = widget::PRE_GROUP::_2;
					widget_check::param wp_("波形保存");
					wav_save_ = wd.add_widget<widget_check>(wp, wp_);
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
			if(name_dialog_->get_state(gui::widget::state::ENABLE)) {
				if(sel_dir_.get_state() == utils::select_dir::state::selected) {
					proj_root_->set_text(sel_dir_.get());
				}
			}

			if(proj_load_filer_.state()) {
				auto path = proj_load_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += PROJECT_EXT_;
					}
					if(load_project_file_(path)) {
						last_path_ = path;
						// CSV1 ファイルをロード
						reset_csv1();
						load_csv1();
					}
				}
			}
			if(proj_save_filer_.state()) {
				auto path = proj_save_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += PROJECT_EXT_;
					}
					if(save_project_file_(path)) {
						last_path_ = path;
					}
				}
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
			if(csv_all_ == nullptr) return false;
			if(csv_idx_ == nullptr) return false;
			if(csv_base_ == nullptr) return false;
			if(image_ext_ == nullptr) return false;

			csv_all_->save(pre);
			csv_idx_->save(pre);
			csv_base_->save(pre);
			image_ext_->save(pre);
			proj_name_->save(pre);
			proj_root_->save(pre);

			files_core_->save(pre);

			log_name_->save(pre);
			wav_save_->save(pre);

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
			if(csv_all_ == nullptr) return false;
			if(csv_idx_ == nullptr) return false;
			if(csv_base_ == nullptr) return false;
			if(image_ext_ == nullptr) return false;

			csv_all_->load(pre);
			csv_idx_->load(pre);
			csv_base_->load(pre);
			image_ext_->load(pre);
			proj_name_->load(pre);
			proj_root_->load(pre);

			files_core_->load(pre);

			log_name_->load(pre);
			wav_save_->load(pre);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログ・セーブ（位置）
			@param[in]	pre	プリファレンス（参照）
		*/
		//-----------------------------------------------------------------//
		void save_sys(sys::preference& pre)
		{
			edit_dialog_->save(pre);
			name_dialog_->save(pre);
			proj_root_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログ・ロード（位置）
			@param[in]	pre	プリファレンス（参照）
		*/
		//-----------------------------------------------------------------//
		void load_sys(sys::preference& pre)
		{
			edit_dialog_->load(pre);
			name_dialog_->load(pre);
			proj_root_->load(pre);
		}
	};
}
