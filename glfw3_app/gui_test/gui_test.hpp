//=====================================================================//
/*! @file
	@brief  GUI Test クラス @n
			GUI のテストと、描画サンプル
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_arrow.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_progress.hpp"
#include "widgets/widget_menu.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_progress.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_view.hpp"

#include "utils/select_file.hpp"

namespace app {

	class gui_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_button*		dialog_open_;
		gui::widget_button*		filer_open_;
		gui::widget_button*		menu_open_;
		gui::widget_button*		menu_ins_;
		gui::widget_button*		menu_era_;
		gui::widget_check*		check_;
		gui::widget_slider*		slider_;
		gui::widget_progress*	progress_;
		gui::widget_dialog*		dialog_;
		gui::widget_button*		dialog2_open_;
		gui::widget_dialog*		dialog2_;
		gui::widget_image*		image_;
		gui::widget_label*		label_;

		gui::widget_frame*		frame_;

		gui::widget_menu*		menu_;
		gui::widget_list*		list_;

		gui::widget_frame*		tree_frame_;
		gui::widget_tree*		tree_core_;

		gui::widget_filer*		filer_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_spinbox*	spinbox_;

		gui::widget_frame*		view_frame_;
		gui::widget_view*		view_core_;

		gui::widget_arrow*		arrow_up_;
		gui::widget_arrow*		arrow_dn_;

		uint32_t	filer_id_;
		uint32_t	menu_id_;
		int			menu_ins_cnt_;


		utils::select_file		sel_file_;


		void view_update_()
		{

		}

		void view_render_(const vtx::irect& clip)
		{
			gui::widget_director& wd = director_.at().widget_director_;

			glDisable(GL_TEXTURE_2D);

			gl::glColor(wd.get_color());
//			gl::draw_line(vtx::spos(clip.org.x, clip.org.y), vtx::spos(clip.size.x, clip.size.y));
			gl::draw_line(vtx::spos(0, 0), vtx::spos(100, 100));
			glEnable(GL_TEXTURE_2D);
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		gui_test(utils::director<core>& d) : director_(d),
			dialog_open_(nullptr),
			filer_open_(nullptr),
			menu_open_(nullptr), menu_ins_(nullptr), menu_era_(nullptr),
			check_(nullptr),
			slider_(nullptr),
			dialog_(nullptr), dialog2_open_(nullptr), dialog2_(nullptr),
			image_(nullptr),
			label_(nullptr),
			frame_(nullptr),
			menu_(nullptr),
			list_(nullptr),
			tree_frame_(nullptr), tree_core_(nullptr),
			filer_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			spinbox_(nullptr),
			view_frame_(nullptr), view_core_(nullptr),
			arrow_up_(nullptr), arrow_dn_(nullptr),
			filer_id_(0), menu_id_(0), menu_ins_cnt_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~gui_test() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			if(1) {	// ラジオボタンのテスト
				widget::param wpr(vtx::irect(20, 20, 130, 130), 0);
				widget_null::param wpr_;
				widget* root = wd.add_widget<widget_null>(wpr, wpr_);
				root->set_state(widget::state::POSITION_LOCK);

				widget::param wp(vtx::irect(0, 0, 130, 30), root);
				widget_radio::param wp_("Enable");
				for(int i = 0; i < 3; ++i) {
					if(i == 2) wp_.check_ = true;
					widget_radio* w = wd.add_widget<widget_radio>(wp, wp_);
					w->at_local_param().select_func_ = [=](bool f, int n) {
						std::cout << "Radio button: " << static_cast<int>(f) << " (" << n << ")" << std::endl;
					};
					wp.rect_.org.y += 40;
				}
			}

			if(1) {	// イメージのテスト
				widget::param wp(vtx::irect(400, 20, 500, 250));
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
//				pa.fill_polygon(ss);
				widget_image::param wp_(&pa);
				image_ = wd.add_widget<widget_image>(wp, wp_);
			}

			if(1) {	// テキストのテスト
				widget::param wp(vtx::irect(40, 50, 200, 250), image_);
				widget_text::param wp_;
				wp_.text_param_.set_text("日本の美しい漢字\n吉野家qwertyuiop\nxcvbnm"
					);
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				wd.add_widget<widget_text>(wp, wp_);
			}

			if(1) { // ダイアログのテスト
				widget::param wp(vtx::irect(300, 300, 300, 200));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->set_text("ああああ\nうううう\nいいいい\n漢字\n日本");
				{
					widget::param wp(vtx::irect(30, 20, 150, 40), dialog_);
					widget_button::param wp_("ダイアログ２");
					dialog2_open_ = wd.add_widget<widget_button>(wp, wp_);
					dialog2_open_->at_local_param().select_func_ = [this](int id) {
						dialog2_->enable();
					};
				}
			}
			if(1) { // ダイアログ２のテスト
				widget::param wp(vtx::irect(300, 300, 500, 300));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				dialog2_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog2_->enable(false);
			}

			if(1) { // ボタンのテスト（ファイラー開始ボタン）
				widget::param wp(vtx::irect(30, 150, 100, 40));
				widget_button::param wp_("Filer");
				filer_open_ = wd.add_widget<widget_button>(wp, wp_);
				filer_open_->at_local_param().text_param_.set_alias("ファイラー");
				filer_open_->at_local_param().text_param_.alias_enable_ = true;
			}

			if(1) { // ボタンのテスト（メニュー開始ボタン）
				widget::param wp(vtx::irect(30, 200, 100, 40));
				widget_button::param wp_("Menu");
				menu_open_ = wd.add_widget<widget_button>(wp, wp_);
			}

			if(1) { // ボタンのテスト（メニュー挿入ボタン）
				widget::param wp(vtx::irect(160, 200, 100, 40));
				widget_button::param wp_("Insert");
				menu_ins_ = wd.add_widget<widget_button>(wp, wp_);
			}

			if(1) { // ボタンのテスト（メニュー削除ボタン）
				widget::param wp(vtx::irect(290, 200, 100, 40));
				widget_button::param wp_("Erase");
				menu_era_ = wd.add_widget<widget_button>(wp, wp_);
			}

			if(1) { // ボタンのテスト（ダイアログ開始ボタン）
				widget::param wp(vtx::irect(30, 250, 100, 40));
				widget_button::param wp_("Daialog");
				dialog_open_ = wd.add_widget<widget_button>(wp, wp_);
			}

			if(1) { // ラベルのテスト
				widget::param wp(vtx::irect(30, 300, 150, 40));
				widget_label::param wp_("Asdfg", false);
				label_ = wd.add_widget<widget_label>(wp, wp_);
				label_->at_local_param().select_func_ = [=](const std::string& t) {
					std::cout << "Label: " << t << std::endl << std::flush;
				};
			}

			if(1) {	// チェックボックスのテスト
				widget::param wp(vtx::irect(20, 350, 150, 40));
				widget_check::param wp_("Disable-g");
				check_ = wd.add_widget<widget_check>(wp, wp_);
				check_->at_local_param().select_func_ = [=](bool f) {
					std::cout << "Check: " << static_cast<int>(f) << std::endl;
				};
			}

			if(1) { // リストのテスト
				widget::param wp(vtx::irect(30, 400, 150, 40), 0);
				widget_list::param wp_("3rd");
				wp_.init_list_.push_back("1st");
				wp_.init_list_.push_back("2nd");
				wp_.init_list_.push_back("3rd");
				wp_.init_list_.push_back("4th");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					utils::format("List Selected: '%s', (%d)\n") % text.c_str() % pos;
				};
				list_ = wd.add_widget<widget_list>(wp, wp_);
			}

			if(1) { // スライダーのテスト
				widget::param wp(vtx::irect(30, 450, 180, 20));
				widget_slider::param wp_;
				wp_.select_func_ = [=] (float lvl) {
					if(progress_ != nullptr) {
						progress_->set_ratio(lvl);
					}
				};
				slider_ = wd.add_widget<widget_slider>(wp, wp_);
			}

			if(1) { // progress のテスト
				widget::param wp(vtx::irect(30, 500, 180, 20));
				widget_progress::param wp_(0.5f);
				progress_ = wd.add_widget<widget_progress>(wp, wp_);
			}

			if(1) { // スピンボックスのテスト
				widget::param wp(vtx::irect(30, 550, 100, 40));
				widget_spinbox::param wp_(1, 3, 50);
				spinbox_ = wd.add_widget<widget_spinbox>(wp, wp_);
				spinbox_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					std::cout << "SpinBox ";
					if(st == widget_spinbox::state::initial) std::cout << "initial";
					else if(st == widget_spinbox::state::inc) std::cout << "inc";
					else if(st == widget_spinbox::state::dec) std::cout << "dec";
					else std::cout << "select";
					std::cout << ": " << newpos << std::endl;
					return (boost::format("%d") % newpos).str();
				};
			}

			if(1) { // アロー UP のテスト
				widget::param wp(vtx::irect(30, 600, 0, 0));
				widget_arrow::param wp_(widget_arrow::direction::up);
				arrow_up_ = wd.add_widget<widget_arrow>(wp, wp_);
				arrow_up_->at_local_param().level_func_ = [=](uint32_t level) {
					std::cout << "Arrow: " << level << std::endl;
				};
			}
			if(1) { // アロー DOWN のテスト
				widget::param wp(vtx::irect(30, 650, 0, 0));
				widget_arrow::param wp_(widget_arrow::direction::down);
				wp_.master_ = arrow_up_;
				arrow_dn_ = wd.add_widget<widget_arrow>(wp, wp_);
			}

			if(1) { // メニューのテスト
				widget::param wp(vtx::irect(10, 10, 180, 30));
				widget_menu::param wp_;
				wp_.init_list_.push_back("First");
				wp_.init_list_.push_back("Second");
				wp_.init_list_.push_back("Third");
				wp_.init_list_.push_back("Force");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					utils::format("Menu Selected: '%s', (%d)\n") % text.c_str() % pos;
				};
				menu_ = wd.add_widget<widget_menu>(wp, wp_);
			}



			if(1) { // フレームのテスト
				widget::param wp(vtx::irect(200, 20, 100, 80));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}

			if(1) {	// ファイラーのテスト
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path(), "", true);
				filer_ = wd.add_widget<widget_filer>(wp, wp_);
				filer_->enable(false);
			}

			if(1) {	// ツリーのテスト
				{
					widget::param wp(vtx::irect(400, 500, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(30);
					tree_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}

				widget::param wp(vtx::irect(0), tree_frame_);
				widget_tree::param wp_;
				tree_core_ = wd.add_widget<widget_tree>(wp, wp_);
				tree_core_->set_state(widget::state::CLIP_PARENTS);
///				tree_core_->set_state(widget::state::POSITION_LOCK);
				tree_core_->set_state(widget::state::RESIZE_ROOT);
				tree_core_->set_state(widget::state::MOVE_ROOT, false);
#if 0
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
#endif

//				tu.list("/root");
//				tu.list();
			}

			if(1) {	// ターミナルのテスト
				{
					widget::param wp(vtx::irect(700, 500, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(30);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					terminal_core_->output("漢字");
				}
			}

			if(1) {
				{
					widget::param wp(vtx::irect(200, 300, 300, 300));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					view_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), view_frame_);
					widget_view::param wp_;
					wp_.update_func_ = [=]() {
						view_update_();
					};
					wp_.render_func_ = [=](const vtx::irect& clip) {
						view_render_(clip);
					};
					view_core_ = wd.add_widget<widget_view>(wp, wp_);
				}
			}

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(filer_ != nullptr) {
				filer_->load(pre);
			}
			if(label_ != nullptr) {
				label_->load(pre);
			}
			if(slider_ != nullptr) {
				slider_->load(pre);
			}
			if(check_ != nullptr) {
				check_->load(pre);
			}
			if(list_ != nullptr) {
				list_->load(pre);
			}
			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(tree_frame_ != nullptr) {
				tree_frame_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(view_frame_ != nullptr) {
				view_frame_->load(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
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
					sel_file_.open();
///					if(filer_) {
///						filer_->enable(!filer_->get_state(gui::widget::state::ENABLE));
///					}
				}
			}
			if(sel_file_.state()) {
				std::cout << sel_file_.get() << std::endl;
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
						}
					}
				}
			}
			if(menu_ins_) {
				if(menu_ins_->get_selected() && menu_ != nullptr) {
					++menu_ins_cnt_;
					menu_->insert((boost::format("Ins-%d") % menu_ins_cnt_).str(), 4);
				}
			}
			if(menu_era_) {
				if(menu_era_->get_selected() && menu_ != nullptr) {
					menu_->erase(3);
				}
			}

		// メニューが選択された！
#if 0
			if(menu_) {
				if(menu_id_ != menu_->get_select_id()) {
					menu_id_ = menu_->get_select_id();
					utils::format("Menu: '%s', (%d)\n")
						% menu_->get_select_text().c_str() % menu_->get_select_pos();
				}
			}
#endif

			if(terminal_core_) {
//				static wchar_t ch = ' ';
//				terminal_core_->output(ch);
//				++ch;
//				if(ch >= 0x7f) ch = ' ';
			}		

			wd.update();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override
		{
			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override
		{
			sys::preference& pre = director_.at().preference_;
			if(filer_ != nullptr) {
				filer_->save(pre);
			}
			if(view_frame_ != nullptr) {
				view_frame_->save(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(tree_frame_ != nullptr) {
				tree_frame_->save(pre);
			}
			if(check_ != nullptr) {
				check_->save(pre);
			}
			if(list_ != nullptr) {
				list_->save(pre);
			}
			if(slider_ != nullptr) {
				slider_->save(pre);
			}
			if(label_ != nullptr) {
				label_->save(pre);
			}
			if(frame_ != nullptr) {
				frame_->save(pre);
			}
		}
	};

}
