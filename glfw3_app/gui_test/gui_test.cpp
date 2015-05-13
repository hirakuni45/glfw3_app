//=====================================================================//
/*! @file
	@brief  player クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "core/glcore.hpp"
#include "gui_test.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_filer.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void gui_test::initialize()
	{
		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		if(1) {	// ラジオボタンのテスト
			widget::param wpr(vtx::srect(20, 20, 130, 130), 0);
			widget_null::param wpr_;
			widget* root = wd.add_widget<widget_null>(wpr, wpr_);
			root->set_state(widget::state::POSITION_LOCK);

			widget::param wp(vtx::srect(0, 0, 130, 30), root);
			widget_radio::param wp_("Enable");
			for(int i = 0; i < 3; ++i) {
				if(i == 2) wp_.check_ = true;
				widget_radio* w = wd.add_widget<widget_radio>(wp, wp_);
				w->at_local_param().select_func_ = [this](bool f, int n) {
					std::cout << "Radio button: " << static_cast<int>(f) << " (" << n << ")" << std::endl;
				};
				wp.rect_.org.y += 40;
			}
		}

		if(1) {	// イメージのテスト
			widget::param wp(vtx::srect(400, 20, 500, 250));
			img::paint pa;
			pa.set_fore_color(img::rgba8(0, 255, 0));
			pa.create(vtx::spos(500, 250), true);
			pa.fill(img::rgba8(255, 100, 100));
			pa.alpha_blend();
			pa.fill_circle(vtx::spos(250), 220);
			vtx::sposs ss;
			ss.push_back(vtx::spos(10, 10));
			ss.push_back(vtx::spos(100, 100));
			ss.push_back(vtx::spos(50, 200));
			pa.set_fore_color(img::rgba8(240, 40, 50));
//			pa.fill_polygon(ss);
			widget_image::param wp_(&pa);
			image_ = wd.add_widget<widget_image>(wp, wp_);
		}

		if(1) {	// テキストのテスト
			widget::param wp(vtx::srect(40, 50, 200, 250), image_);
			widget_text::param wp_;
			wp_.text_param_.set_text("日本の美しい漢字\n吉野家qwertyuiop\nxcvbnm"
				);
			wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
			wd.add_widget<widget_text>(wp, wp_);
		}

		if(1) { // ダイアログのテスト
			widget::param wp(vtx::srect(300, 300, 300, 200));
			widget_dialog::param wp_;
			wp_.style_ = widget_dialog::param::style::CANCEL_OK;
			dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			dialog_->set_text("ああああ\nうううう\nいいいい\n漢字\n日本");
		}

		if(1) { // ボタンのテスト（ファイラー開始ボタン）
			widget::param wp(vtx::srect(30, 150, 100, 40));
			widget_button::param wp_("Filer");
			filer_open_ = wd.add_widget<widget_button>(wp, wp_);
			filer_open_->at_local_param().text_param_.set_alias("ファイラー");
			filer_open_->at_local_param().text_param_.alias_enable_ = true;
		}

		if(1) { // ボタンのテスト（メニュー開始ボタン）
			widget::param wp(vtx::srect(30, 200, 100, 40));
			widget_button::param wp_("Menu");
			menu_open_ = wd.add_widget<widget_button>(wp, wp_);
		}

		if(1) { // ボタンのテスト（ダイアログ開始ボタン）
			widget::param wp(vtx::srect(30, 250, 100, 40));
			widget_button::param wp_("Daialog");
			dialog_open_ = wd.add_widget<widget_button>(wp, wp_);
		}

		if(1) { // ラベルのテスト
			widget::param wp(vtx::srect(30, 300, 150, 40));
			widget_label::param wp_("Asdfg", false);
			label_ = wd.add_widget<widget_label>(wp, wp_);
			label_->at_local_param().select_func_ = [this](const std::string& t) {
				std::cout << "Label: " << t << std::endl << std::flush;
			};
		}

		if(1) {	// チェックボックスのテスト
			widget::param wp(vtx::srect(20, 350, 150, 40));
			widget_check::param wp_("Disable-g");
			check_ = wd.add_widget<widget_check>(wp, wp_);
			check_->at_local_param().select_func_ = [this](bool f) {
				std::cout << "Check: " << static_cast<int>(f) << std::endl;
			};
		}

		if(1) { // リストのテスト
			widget::param wp(vtx::srect(30, 400, 150, 40), 0);
			widget_list::param wp_("List Box");
			wp_.text_list_.push_back("abc");
			wp_.text_list_.push_back("1234");
			wp_.text_list_.push_back("qwert");
			wd.add_widget<widget_list>(wp, wp_);
		}

		if(1) { // スライダーのテスト
			widget::param wp(vtx::srect(30, 450, 180, 20));
			widget_slider::param wp_;
			slider_ = wd.add_widget<widget_slider>(wp, wp_);
		}

		if(1) { // メニューのテスト
			widget::param wp(vtx::srect(10, 10, 180, 30));
			widget_menu::param wp_;
//			wp_.round_ = false;
			wp_.text_list_.push_back("First");
			wp_.text_list_.push_back("Second");
			wp_.text_list_.push_back("Third");
			wp_.text_list_.push_back("Force");
			menu_ = wd.add_widget<widget_menu>(wp, wp_);
		}

		if(1) { // フレームのテスト
			widget::param wp(vtx::srect(200, 20, 100, 80));
			widget_frame::param wp_;
			frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}

		if(1) {	// ファイラーのテスト
			widget::param wp(vtx::srect(10, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path(), "", true);
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}

		if(1) {	// ツリーのテスト
			{
				widget::param wp(vtx::srect(400, 500, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(30);
				tree_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}

			widget::param wp(vtx::srect(0), tree_frame_);
			widget_tree::param wp_;
			tree_core_ = wd.add_widget<widget_tree>(wp, wp_);
			tree_core_->set_state(widget::state::CLIP_PARENTS);
///			tree_core_->set_state(widget::state::POSITION_LOCK);
			tree_core_->set_state(widget::state::RESIZE_ROOT);
			tree_core_->set_state(widget::state::MOVE_ROOT, false);

			widget_tree::tree_unit& tu = tree_core_->at_tree_unit();
			tu.make_directory("/root0");
			tu.make_directory("/root1");
			tu.set_current_path("/root0");
			{
				widget_tree::value v;
				v.data_ = "AAA";
				tu.install("sub0", v);
			}
			{
				widget_tree::value v;
				v.data_ = "BBB";
				tu.install("sub1", v);
			}
			tu.make_directory("sub2");
			tu.set_current_path("sub2");
			{
				widget_tree::value v;
				v.data_ = "CCC";
				tu.install("sub-sub", v);
			}
			tu.set_current_path("/root1");
			{
				widget_tree::value v;
				v.data_ = "ASDFG";
				tu.install("sub_A", v);
			}
			{
				widget_tree::value v;
				v.data_ = "ZXCVB";
				tu.install("sub_B", v);
			}

//			tu.list("/root");
//			tu.list();
		}

		if(1) {	// ターミナルのテスト
			{
				widget::param wp(vtx::srect(700, 500, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(30);
				terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::srect(0), terminal_frame_);
				widget_terminal::param wp_;
				terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
			}
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->load(pre);
		}
		if(label_) {
			label_->load(pre);
		}
		if(slider_) {
			slider_->load(pre);
		}
		if(check_) {
			check_->load(pre);
		}
		if(frame_) {
			frame_->load(pre);
		}
		if(tree_frame_) {
			tree_frame_->load(pre);
		}
		if(terminal_frame_) {
			terminal_frame_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void gui_test::update()
	{
		gui::widget_director& wd = director_.at().widget_director_;

		if(dialog_open_) {
			if(dialog_open_->get_selected()) {
				if(dialog_) {
					dialog_->enable();
				}
			}
		}

		if(filer_open_) {
			if(filer_open_->get_selected()) {
				if(filer_) {
					filer_->enable(!filer_->get_state(gui::widget::state::ENABLE));
				}
			}
		}
		if(filer_) {
			if(filer_id_ != filer_->get_select_file_id()) {
				filer_id_ = filer_->get_select_file_id();
				std::cout << "Filer: '" << filer_->get_file() << "'" << std::endl;
				std::cout << std::flush;
			}
		}

		if(menu_open_) {
			if(menu_open_->get_selected()) {
				if(menu_) {
					menu_->enable(!menu_->get_state(gui::widget::state::ENABLE));
					if(menu_->get_state(gui::widget::state::ENABLE)) {
//						menu_->at_rect().org = 
					}

				}
			}
		}
		// メニューが選択された！
		if(menu_) {
			if(menu_id_ != menu_->get_select_id()) {
				menu_id_ = menu_->get_select_id();
				std::cout << "Menu: " << menu_->get_select_text() << std::endl;
			}
		}

///		if(terminal_core_) {
///			static wchar_t ch = ' ';
///			terminal_core_->output(ch);
///			++ch;
///			if(ch >= 0x7f) ch = ' ';
///		}		

		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void gui_test::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void gui_test::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->save(pre);
		}
		if(terminal_frame_) {
			terminal_frame_->save(pre);
		}
		if(tree_frame_) {
			tree_frame_->save(pre);
		}
		if(check_) {
			check_->save(pre);
		}
		if(slider_) {
			slider_->save(pre);
		}
		if(label_) {
			label_->save(pre);
		}
		if(frame_) {
			frame_->save(pre);
		}
	}
}
