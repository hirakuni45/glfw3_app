#pragma once
//=====================================================================//
/*! @file
	@brief  Logic FORM クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/spring_damper.hpp"
#include "logic.hpp"
#include "logic_edit.hpp"
#include "ch4_file.hpp"

#include "utils/intmath.hpp"

// #include <boost/format.hpp>

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ロジック・フォーム・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class logic_form : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		menu_;
		gui::widget_button*		load_;
		gui::widget_button*		save_;
		gui::widget_button*		export_;
		bool					export_on_;

		// プロジェクト管理
		struct project_t {
			gui::widget_frame*		base_;	// プロジェクト全体フレーム
			gui::widget_null*		main_;	// 内装描画領域
			gui::widget_null*		tool_;	// ツール関係
			gui::widget_view*		view_;

			tools::logic			logic_;
			tools::logic_edit		logic_edit_;

			vtx::ipos				view_org_;
			vtx::ipos				view_offset_;

			gui::spring_damper		sd_x_;
			gui::spring_damper		sd_y_;

			vtx::ipos				sel_pos_;

			project_t() : base_(nullptr), main_(nullptr), tool_(nullptr), view_(nullptr),
				logic_(), logic_edit_(logic_),
				view_org_(0), view_offset_(0),
				sd_x_(), sd_y_(),
				sel_pos_(0)
				{ }

			void load(sys::preference& pre) {
				if(base_ != nullptr) {
					base_->load(pre);
				}
			}

			void save(sys::preference& pre) {
				if(base_ != nullptr) {
					base_->save(pre);
				}
			}
		};

		project_t	project_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_filer*		load_ctx_;
		gui::widget_filer*		save_ctx_;

		gui::widget_dialog*		dialog_;
		gui::widget_dialog*		dialog_yn_;

		static const int pin_n_ = 30;
		static const int pin_w_ = 30;
		static const int pin_h_ = 30;
		static const int logic_ofs_ = 25;
		static const int logic_lvl_  = 20;
		static const int logic_step_ = 20;
		static const int logic_tool_height_ = 50;

		void draw_logic_(project_t& t, const vtx::irect& rect, int bitpos)
		{
			vtx::sposs list;
			int lv = rect.org.y + logic_ofs_ + t.view_offset_.y;
			for(uint32_t i = 0; i < t.logic_.size(); ++i) {
				auto l = t.logic_.get_logic(bitpos, i);
				vtx::ipos p;
				p.x = rect.org.x + t.view_offset_.x + (logic_step_ * i);
				p.y = lv - static_cast<int>(l) * logic_lvl_;
				if((p.x + logic_step_) < rect.org.x) continue;
				int step = logic_step_;
				if(p.x < rect.org.x) {
					step -= rect.org.x - p.x;
					p.x = rect.org.x;
				}
				list.emplace_back(p.x, p.y);
				p.x += step;
				list.emplace_back(p.x, p.y);
				if(p.x > rect.end_x()) break;
			}

			if(list.size() > 2) {
				gl::glColor(img::rgba8(255, 255));
				gl::draw_line_strip(list);
			}
		}


		void update_main_()
		{
			using namespace gui;

			auto wn = project_.main_;
			if(wn == nullptr) return;

			if(wn->get_param().parents_) {
				if(wn->get_param().parents_->type() == get_type_id<widget_frame>()) {
					widget_frame* w = static_cast<widget_frame*>(wn->at_param().parents_);
					wn->at_rect() = w->get_draw_area();
				}
			}
		}


		void update_tool_()
		{
			using namespace gui;

			auto wn = project_.tool_;
			if(wn == nullptr) return;

			if(wn->get_param().parents_) {
				if(wn->get_param().parents_->type() == get_type_id<widget_frame>()) {
					widget_frame* w = static_cast<widget_frame*>(wn->at_param().parents_);
					auto r = w->get_draw_area();
					r.size.y = logic_tool_height_;
					wn->at_rect() = r;
				}
			}
		}


		void update_view_(project_t& t)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			auto w = t.view_;
			w->at_rect().org.y  += logic_tool_height_;
			w->at_rect().size.y -= logic_tool_height_;

			if(project_.logic_.size() > 0) {
				if(w->get_select_in()) {
					project_.sel_pos_ = wd.get_positive_pos() - w->get_param().clip_.org;
				}

				if(w->get_focus() && w->get_select_out()) {
					auto np = wd.get_negative_pos() - w->get_param().clip_.org;
					auto d = np - project_.sel_pos_;
					if(std::abs(d.x) <= 5 && std::abs(d.y) <= 5) { 
/// std::cout << "Select out: " << project_.sel_pos_.x << ", " << project_.sel_pos_.y << std::endl;
						uint32_t pos = (project_.sel_pos_.x - t.view_offset_.x - pin_n_) / logic_step_;
						uint32_t ch = (project_.sel_pos_.y - t.view_offset_.y) / pin_h_;
/// std::cout << "Select CH: " << ch << ", POS: " << pos << std::endl;
						project_.logic_.flip_logic(ch, pos);
					}
				}
			}
		}


		void service_view_(project_t& t)
		{
			auto w = t.view_;
			auto in = w->get_select_in();
			auto sel = w->get_select();
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			vtx::ipos scr(0);
			auto top = wd.get_top_widget();
			if(w->get_focus() && (wd.root_widget(top) == project_.base_)) {
				scr = wd.get_scroll();
				scr.x *= logic_step_ >> 1;
				scr.y *= pin_h_ >> 1;
			}
			vtx::ipos limit;
			auto r = t.base_->get_draw_area();
			r.size.y -= logic_tool_height_;
			limit.x = r.size.x - pin_n_ - (t.logic_.size() * logic_step_);
			limit.y = r.size.y - (pin_h_ * 24 + 2);
			vtx::ipos d(0);
			if(sel) {
				d = w->get_param().move_pos_ - w->get_param().move_org_;
			}
			t.view_offset_.x = t.sd_x_.update(in, sel, t.view_offset_.x, scr.x, limit.x, d.x);
			t.view_offset_.y = t.sd_y_.update(in, sel, t.view_offset_.y, scr.y, limit.y, d.y);
		}


		// プロジェクトの描画
		void render_view_(project_t& t, const vtx::irect& clip)
		{
			glDisable(GL_TEXTURE_2D);
			vtx::srect rect(pin_n_, 0, 2, pin_h_ * 24);
			rect.org.y += t.view_offset_.y;
			gui::draw_border(rect);

			rect.org.x = 0;
			rect.size.x = clip.size.x;
			rect.size.y = 2;
			gui::draw_border(rect);
			for(int i = 0; i < 24; ++i) {
				rect.org.y = (i + 1) * pin_h_ + t.view_offset_.y;
				gui::draw_border(rect);
				draw_logic_(t, vtx::irect(pin_n_ + 2, i * pin_h_, clip.size.x - pin_n_, logic_lvl_), i);
			}

			gui::widget::text_param tp("", img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::TOP));

			gl::core& core = gl::core::get_instance();
			gl::fonts& fonts = core.at_fonts();
			for(int i = 0; i < 24; ++i) {
				vtx::irect tr(0, i * pin_h_ + 3, pin_n_, pin_h_);
				tr.org.y += t.view_offset_.y;
				tp.set_text(std::to_string(i));
				gui::draw_text(tp, tr, clip);
			}
			fonts.restore_matrix();
		}


		// プロジェクト・リソースの作成
		void create_project_(project_t& t, int w, int h)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{   // プロジェクト・フレーム
				widget::param wp(vtx::irect(20, 50, w, h));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				t.base_ = wd.add_widget<widget_frame>(wp, wp_);
			}

			{   // メイン
				widget::param wp(vtx::irect(0, 0, w, h), t.base_);
				wp.state_.reset(widget::state::RENDER_ENABLE);
				widget_null::param wp_;
				wp_.update_func_ = [this]() {
					update_main_();
				};
				t.main_ = wd.add_widget<widget_null>(wp, wp_);
				t.main_->set_state(widget::state::POSITION_LOCK);
				t.main_->set_state(widget::state::SIZE_LOCK);
				t.main_->set_state(widget::state::RESIZE_ROOT);
			}

			{   // ツール  
				widget::param wp(vtx::irect(0, 0, w, logic_tool_height_), t.base_);
				wp.state_.reset(widget::state::RENDER_ENABLE);
				widget_null::param wp_;
				wp_.update_func_ = [this]() {
					update_tool_();
				};
				t.tool_ = wd.add_widget<widget_null>(wp, wp_);
				t.tool_->set_state(widget::state::POSITION_LOCK);
				t.tool_->set_state(widget::state::SIZE_LOCK);
				t.tool_->set_state(widget::state::RESIZE_ROOT);
			}

			{   // 描画ビュー
				widget::param wp(vtx::irect(0, logic_tool_height_, 500, 100), t.main_);
				wp.state_.set(widget::state::CLIP_PARENTS);
				widget_view::param wp_;
				wp_.update_func_ = [this]() {
					update_view_(project_);
				};
				wp_.render_func_ = [this](const vtx::irect& clip) {
					render_view_(project_, clip);
				};
				wp_.service_func_ = [this]() {
					service_view_(project_);
				};
				t.view_ = wd.add_widget<widget_view>(wp, wp_);
				t.view_->set_state(widget::state::CLIP_PARENTS);
				t.view_->set_state(widget::state::RESIZE_ROOT);
				t.view_->set_state(widget::state::MOVE_ROOT, false);
				t.view_->set_state(widget::state::POSITION_LOCK, false);
				t.view_->set_state(widget::state::SERVICE);
			}
		}

		// ターミナル、行入力
		void term_enter_(const utils::lstring& text) {
			auto s = utils::utf32_to_utf8(text);
			project_.logic_edit_.command(s);
///			std::cout << s << std::endl;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		logic_form(utils::director<core>& d) : director_(d),
			menu_(nullptr), load_(nullptr), save_(nullptr), export_(nullptr), export_on_(false),
			project_(),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			load_ctx_(nullptr), save_ctx_(nullptr),
			dialog_(nullptr), dialog_yn_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~logic_form() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			gl::core& core = gl::core::get_instance();

			{	// メニューパレット
				widget::param wp(vtx::irect(20, 20, 110, 300));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				menu_ = wd.add_widget<widget_frame>(wp, wp_);
				menu_->set_state(gui::widget::state::SIZE_LOCK);
			}
			{ // ロード起動ボタン
				widget::param wp(vtx::irect(10, 20+40*0, 90, 30), menu_);
				widget_button::param wp_("load");
				wp_.select_func_ = [this]() {
					if(load_ctx_) {
						bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
						load_ctx_->enable(!f);
						save_->set_stall(!f);
						export_->set_stall(!f);
					}
				};
				load_ = wd.add_widget<widget_button>(wp, wp_);
			}
			{ // セーブ起動ボタン
				widget::param wp(vtx::irect(10, 20+40*1, 90, 30), menu_);
				widget_button::param wp_("save");
				wp_.select_func_ = [this]() {
					if(save_ctx_) {
						export_on_ = false;
						bool f = save_ctx_->get_state(gui::widget::state::ENABLE);
						save_ctx_->enable(!f);
						load_->set_stall(!f);
						export_->set_stall(!f);
					}
				};
				save_ = wd.add_widget<widget_button>(wp, wp_);
			}
			{ // エキスポート起動ボタン
				widget::param wp(vtx::irect(10, 20+40*2, 90, 30), menu_);
				widget_button::param wp_("export");
				wp_.select_func_ = [this]() {
					if(save_ctx_) {
						export_on_ = true;
						bool f = save_ctx_->get_state(gui::widget::state::ENABLE);
						save_ctx_->enable(!f);
						load_->set_stall(!f);
						save_->set_stall(!f);
					}
				};
				export_ = wd.add_widget<widget_button>(wp, wp_);
			}


			create_project_(project_, 200, 100);

			{	// ターミナル
				{
					widget::param wp(vtx::irect(20, 100, 100, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [this](const utils::lstring& text) {
						term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
				}

				// ロジック編集クラスの出力先の設定
				project_.logic_edit_.set_output([this](const std::string& s) {
					terminal_core_->output(s);
				}
				);
			}

			{ // load ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				wp_.select_file_func_ = [this](const std::string& path) {
					bool f = project_.logic_.load(path);
					save_->set_stall(false);
					export_->set_stall(false);
					if(!f) {  // load error
						dialog_->set_text("Load error:\n" + path);
						dialog_->enable();
					}
				};
				load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				load_ctx_->enable(false);
			}
			{ // save ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				wp_.select_file_func_ = [this](const std::string& path) {
					std::string newpath = path;
					bool f = false;
					if(export_on_) {
						auto ext = utils::get_file_ext(newpath);
						if(ext.empty() || ext != ".csv") {
							newpath = utils::get_file_base(path);
							newpath += ".csv";
						}
						tools::ch4_file ch4(project_.logic_);

						f = ch4.save(newpath);
					} else {

						f = project_.logic_.save(newpath);
					}
					load_->set_stall(false);
					save_->set_stall(false);
					export_->set_stall(false);
					if(!f) {  // save error
						std::string err;
						if(export_on_) {
							err = "Export error: ";
						} else {
							err = "Save error:";
						}
						dialog_->set_text(err + project_.logic_.get_error() + "\n" + newpath);
						dialog_->enable();
					}
				};
				wp_.new_file_ = true;
				save_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				save_ctx_->enable(false);
			}

			{ // ダイアログ(OK)
				widget::param wp(vtx::irect(100, 100, 500, 150));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::param::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
			}

			{ // ダイアログ(OK/Cancel)
				widget::param wp(vtx::irect(110, 110, 500, 150));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::param::style::CANCEL_OK;
				dialog_yn_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_yn_->enable(false);
			}

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;

			if(menu_ != nullptr) {
				menu_->load(pre, false, false);
			}

			project_.load(pre);

			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(load_ctx_ != nullptr) load_ctx_->load(pre);
			if(save_ctx_ != nullptr) save_ctx_->load(pre);

			// デバッグ
//			project_.logic_.create(2048);
///			project_.logic_.create(100);
///			project_.logic_.build_clock(0, 0, 0, 1, 2);
//			project_.logic_.build_clock(0);
///			project_.logic_.build_noise(1);
///			project_.logic_.build_noise(2);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;
			wd.update();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render()
		{
			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			sys::preference& pre = director_.at().preference_;

			if(load_ctx_ != nullptr) load_ctx_->save(pre);
			if(save_ctx_ != nullptr) save_ctx_->save(pre);

			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}

			project_.save(pre);

			if(menu_ != nullptr) {
				menu_->save(pre);
			}
		}

	};

}
