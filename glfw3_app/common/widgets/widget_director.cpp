//=====================================================================//
/*!	@file
	@brief	GUI widget ディレクター
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdio>
#include <map>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "gl_fw/gl_info.hpp"
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"

namespace gui {

	using namespace gl;

	widget::color_param widget_director::default_frame_color_;
	widget::color_param widget_director::default_button_color_;
	widget::color_param widget_director::default_label_color_;
	widget::color_param widget_director::default_label_color_select_;
	widget::color_param widget_director::default_slider_color_;
	widget::color_param widget_director::default_check_color_;
	widget::color_param widget_director::default_list_color_;
	widget::color_param widget_director::default_list_color_select_;
	widget::color_param widget_director::default_dialog_color_;
	widget::color_param widget_director::default_filer_color_;
	widget::color_param widget_director::default_tree_color_;
	widget::color_param widget_director::default_terminal_color_;

	void widget_director::message_widget_(widget* w, const std::string& s)
	{
		std::string type;

		type = w->type_name();
		if(w->type() == get_type_id<widget_label>()) {
			widget_label* wl = dynamic_cast<widget_label*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_button>()) {
			widget_button* wl = dynamic_cast<widget_button*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_check>()) {
			widget_check* wl = dynamic_cast<widget_check*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_radio>()) {
			widget_radio* wl = dynamic_cast<widget_radio*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		}

		printf("(%d:%s)%s: '%s'\n", w->get_serial(), w->get_symbol().c_str(),
			s.c_str(), type.c_str());
	}


	void make_clip_(widget* w)
	{
		if(w->get_state(widget::state::CLIP_PARENTS)) {
			final_clip(w, w->at_param().rpos_, w->at_param().clip_);
		} else {
			vtx::spos p;
			final_position(w, w->at_param().rpos_);
			w->at_param().clip_.org = w->get_param().rpos_;
			w->at_param().clip_.size = w->get_rect().size;
		}
	}


	void clip_widgets_(widgets& ws, bool check_mark = false)
	{
		BOOST_FOREACH(widget* w, ws) {
			if(check_mark) {
				if(w->get_mark()) continue;
				w->set_mark();
			}
			make_clip_(w);
		}
	}


	void widget_director::parents_widget_mark_(widget* root)
	{
		root->set_mark();
		widgets ws;
		parents_widget(root, ws);
		BOOST_FOREACH(widget* w, ws) {
			w->set_mark();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ウィジェットの許可、不許可
		@param[in]	root	ウィジェット
		@param[in]	flag	不許可なら「false」
		@param[in]	child	子も不許可にする場合「true」
	*/
	//-----------------------------------------------------------------//
	void widget_director::enable(widget* root, bool flag, bool child)
	{
		if(root == 0) return;

		root->at_param().state_[widget::state::ENABLE] = flag;
		if(!child) {
			return;
		}

		widgets ws;
		parents_widget(root, ws);
		BOOST_FOREACH(widget* w, ws) {
			w->at_param().state_[widget::state::ENABLE] = flag;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ウィジェットの削除
		@param[in]	w	ウィジェット
	*/
	//-----------------------------------------------------------------//
	bool widget_director::del_widget(widget* w)
	{
		if(w == 0) return false;

		widgets ws;
		BOOST_FOREACH(widget* ww, widgets_) {
			if(ww != w) {
				ws.push_back(ww);
			}
		}
		widgets_ = ws;

		delete w;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ペアレンツ・ウィジェットの収集
		@param[in]	pw	ペアレンツ・ウィジェット
		@param[out]	ws	ウィジェット列
	*/
	//-----------------------------------------------------------------//
	void widget_director::parents_widget(widget* pw, widgets& ws)
	{
		BOOST_FOREACH(widget* w, widgets_) {
			if(w->get_param().parents_ == pw) {
				ws.push_back(w);
				parents_widget(w, ws);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ルート・ウィジェットを返す
		@param[in]	w	基準ウィジェット
		@return ルート・ウィジェット
	*/
	//-----------------------------------------------------------------//
	widget* widget_director::root_widget(widget* w) const
	{
		if(w == 0) return 0;

		do {
			if(w->get_param().parents_ == 0) {
				break;
			}
			w = w->get_param().parents_;
		} while(w) ;
		return w;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	マーキングをリセットする
	*/
	//-----------------------------------------------------------------//
	void widget_director::reset_mark()
	{
		BOOST_FOREACH(widget* w, widgets_) {
			w->set_mark(false);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	最前面にする
		@param[in]	w	ウィジェット
	*/
	//-----------------------------------------------------------------//
	void widget_director::top_widget(widget* w)
	{
		if(w == 0) return;

		reset_mark();
		widgets ws;
		ws.push_back(w);
		parents_widget(w, ws);
		BOOST_FOREACH(widget* cw, ws) {
			cw->set_mark();
		}
		widgets wss;
		BOOST_FOREACH(widget* cw, widgets_) {
			if(!cw->get_mark()) {
				wss.push_back(cw);
			}
		}
		BOOST_FOREACH(widget* cw, ws) {
			wss.push_back(cw);
		}
		widgets_ = wss;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_director::initialize()
	{
		using namespace gl;

		img_files_.initialize();

		mobj_.initialize();

		// 「各」標準的な色彩や、形状などの設定
		img::rgba8 fc, bc;
		fc.set( 55, 157, 235);
		bc = fc * 0.7f;
		default_frame_color_ = widget::color_param(fc, bc);

		fc.set( 72, 193, 241);
		bc = fc * 0.7f;
		default_button_color_ = widget::color_param(fc, bc);

		fc.set( 48, 193, 241);
		bc = fc * 0.7f;
		default_label_color_ = widget::color_param(fc, bc);
		fc.set(118, 223, 231);
		bc = fc * 0.7f;
		default_label_color_select_ = widget::color_param(fc, bc);

		fc.set( 48, 193, 241);
		bc.set( 31,  72,  86);
		default_slider_color_ = widget::color_param(fc, bc);

		fc.set( 72, 193, 241);
		bc.set( 47,  72,  86);
		default_check_color_ = widget::color_param(fc, bc);

		fc.set( 48, 193, 241);
		bc = fc * 0.7f;
		default_list_color_ = widget::color_param(fc, bc);
		fc.set(118, 223, 231);
		bc = fc * 0.7f;
		default_list_color_select_ = widget::color_param(fc, bc);

		fc.set(235, 157,  95);
		bc = fc * 0.7f;
		default_dialog_color_ = widget::color_param(fc, bc);

		fc.set(235, 157,  95);
		bc = fc * 0.7f;
		default_filer_color_ = widget::color_param(fc, bc);

		fc.set( 55, 157, 235);
		bc = fc * 0.7f;
		default_tree_color_ = widget::color_param(fc, bc);

		fc.set( 55, 157, 235);
		bc = fc * 0.7f;
		default_terminal_color_ = widget::color_param(fc, bc);

		img::paint::intensity_rect ir;
		// ボタンの頂点輝度設定
		ir.left_top    = 200; ir.center_top    = 255; ir.right_top    = 200;
		ir.left_center = 255; ir.center_center = 255; ir.right_center = 255;
		ir.left_bottom = 180; ir.center_bottom = 255; ir.right_bottom = 180;
		default_button_color_.inten_rect_ = ir;
		default_button_color_.ir_enable_ = true;

		// ラベルの頂点輝度設定
		ir.left_top    = 255; ir.center_top    = 255; ir.right_top    = 255;
		ir.left_center = 200; ir.center_center = 200; ir.right_center = 200;
		ir.left_bottom = 100; ir.center_bottom = 100; ir.right_bottom = 100;
		default_label_color_.inten_rect_  = ir;
		default_label_color_.ir_enable_ = true;
		ir.left_top    = 150; ir.center_top    = 150; ir.right_top    = 150;
		ir.left_center = 255; ir.center_center = 255; ir.right_center = 255;
		ir.left_bottom = 150; ir.center_bottom = 150; ir.right_bottom = 150;
		default_label_color_select_.inten_rect_  = ir;
		default_label_color_select_.ir_enable_ = true;

		// リストの頂点輝度設定
		ir.left_top    = 255; ir.center_top    = 255; ir.right_top    = 255;
		ir.left_center = 200; ir.center_center = 200; ir.right_center = 200;
		ir.left_bottom =  80; ir.center_bottom =  80; ir.right_bottom =  80;
		default_list_color_.inten_rect_  = ir;
		default_list_color_.ir_enable_ = true;
		ir.left_top    = 150; ir.center_top    = 150; ir.right_top    = 150;
		ir.left_center = 255; ir.center_center = 255; ir.right_center = 255;
		ir.left_bottom = 150; ir.center_bottom = 150; ir.right_bottom = 150;
		default_list_color_select_.inten_rect_  = ir;
		default_list_color_select_.ir_enable_ = true;

		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		fonts& fonts = igl->at_fonts();
		fonts.set_clip_size(igl->get_size());
		fonts.set_font_size(24);

		// 共通部品の作成
		{
			using vtx::spos;

			spos size(24);
			img::paint fr;
			widget::plate_param pp;
			pp.frame_width_ = 4;
			pp.round_radius_ = 4;
			create_round_frame(fr, pp, default_check_color_, size);
			share_img_.un_check_ = mobj_.install(&fr);

			img::paint pa;
			pa = fr;

			pa.swap_color();
			pa.fill_rect(8, 8, 8, 8);
			share_img_.to_check_ = mobj_.install(&pa);

			pa = fr;
			pa.swap_color();
			pa.fill_rect(6, 10, 12, 4);
			share_img_.minus_box_ = mobj_.install(&pa);

			pa.fill_rect(10, 6, 4, 12);
			share_img_.plus_box_ = mobj_.install(&pa);

			vtx::sposs ss;
			pa = fr;
			pa.swap_color();
			ss.push_back(spos(12,    8));
			ss.push_back(spos(12+4, 16));
			ss.push_back(spos(12-4, 16));
			pa.fill_polygon(ss);
			share_img_.up_box_ = mobj_.install(&pa);

			pa = fr;
			pa.swap_color();
			ss.clear();
			ss.push_back(spos(12,   16));
			ss.push_back(spos(12-4,  8));
			ss.push_back(spos(12+4,  8));
			pa.fill_polygon(ss);
			share_img_.down_box_ = mobj_.install(&pa);

			pa = fr;
			pa.swap_color();
			ss.clear();
			ss.push_back(spos(16, 12));
			ss.push_back(spos(8,  12-4));
			ss.push_back(spos(8,  12+4));
			pa.fill_polygon(ss);
			share_img_.right_box_ = mobj_.install(&pa);

			pa = fr;
			pa.swap_color();
			ss.clear();
			ss.push_back(spos(8,  12));
			ss.push_back(spos(16, 12+4));
			ss.push_back(spos(16, 12-4));
			pa.fill_polygon(ss);
			share_img_.left_box_ = mobj_.install(&pa);

			// ラジオボタン
			pa.fill(img::rgba8(0, 0));
			pa.alpha_blend(false);
			pa.set_fore_color(default_check_color_.fore_color_);
			pa.fill_circle(vtx::spos(12), 11);
			pa.alpha_blend();
			pa.set_fore_color(default_check_color_.back_color_);
			pa.fill_circle(vtx::spos(12), 8);
			share_img_.un_radio_ = mobj_.install(&pa);

			pa.set_fore_color(default_check_color_.fore_color_);
			pa.fill_circle(vtx::spos(12), 5);
			share_img_.to_radio_ = mobj_.install(&pa);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	挙動制御
		@return 操作があった場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_director::update()
	{
		IGLcore* Igl = get_glcore();

		// ダイアログがある場合の優先順位とストール処理
		{
			reset_mark();
			widgets ws;
			BOOST_FOREACH(widget* w, widgets_) {
				if(!w->get_state(widget::state::ENABLE)) continue;
				if(w->type() == get_type_id<widget_dialog>()) {
					parents_widget_mark_(w);
					ws.push_back(w);
				}
			}
			if(ws.empty()) {
				BOOST_FOREACH(widget* w, widgets_) {
					w->set_state(widget::state::SYSTEM_STALL, false);
				}
			} else {
				BOOST_FOREACH(widget* w, widgets_) {
					if(!w->get_mark()) {
						w->set_state(widget::state::SYSTEM_STALL); 
					}
				}
			}
			BOOST_FOREACH(widget* w, ws) {
				top_widget(w);
			}
		}

		const device& dev = Igl->get_device();

		scroll_ = dev.get_locator().scroll_;

		const vtx::spos& msp = dev.get_locator().cursor_;

		struct trg_t {
			bool	lvl;
			bool	pos;
			bool	neg;
		};
		trg_t left;
		left.lvl = dev.get_level(device::key::MOUSE_LEFT);
		left.pos = dev.get_positive(device::key::MOUSE_LEFT);
		left.neg = dev.get_negative(device::key::MOUSE_LEFT);
		trg_t right;
		right.lvl = dev.get_level(device::key::MOUSE_RIGHT);
		right.pos = dev.get_positive(device::key::MOUSE_RIGHT);
		right.neg = dev.get_negative(device::key::MOUSE_RIGHT);

		if(!left.lvl) {
			top_move_ = 0;
		}
		if(!right.lvl) {
			top_resize_ = 0;
		}

		vtx::spos msp_speed = msp - msp_speed_;
		msp_speed_ = msp;

		if(left.pos) {
			position_positive_ = msp;
			position_level_ = msp;
			msp_length_ = 0.0f;
		}
		if(left.lvl) {
			msp_length_ += std::sqrt((msp - position_level_).sqr());
			position_level_ = msp;
		}
		if(left.neg) {
			position_negative_ = msp;
		}

		// フォーカス、選択、を設定
		widget* select = 0;
		BOOST_FOREACH(widget* w, widgets_) {
			w->set_state(widget::state::DRAG, false);
			w->set_state(widget::state::RESIZE, false);
			if(!w->get_state(widget::state::ENABLE) ||
			  w->get_state(widget::state::STALL) ||
			  w->get_state(widget::state::SYSTEM_STALL)) {
				w->set_state(widget::state::FOCUS, false);
				w->set_state(widget::state::SELECT, false);
				continue;
			}

			bool focus = w->get_param().clip_.is_focus(msp);
			if(w->get_state(widget::state::FOCUS_ENABLE)) {
				w->set_state(widget::state::FOCUS, focus);
			}
			if(left.pos && focus) {
				w->at_param().move_org_ = w->get_rect().org;
				top_move_ = w;	// 移動を行う widget 候補
			}
			if(right.pos && focus) {
				w->at_param().resize_org_ = msp;
				w->at_param().resize_ref_ = w->get_rect().size;
				top_resize_ = w;	// リサイズを行う widget 候補
			}
			// 選択している widget 候補
			if(left.lvl && focus) {
				w->set_state(widget::state::DRAG);
				select = w;
				if(w->get_state(widget::state::DRAG_UNSELECT)
				  && msp_length_ > unselect_length_) {
					w->set_state(widget::state::SELECT, false);
					w->set_state(widget::state::IS_SELECT, false);
					select = 0;
				}
			}
			// 移動時
			if(left.lvl) {
				vtx::spos d = msp - position_positive_;
				w->at_param().move_pos_ = w->get_param().move_org_ + d;
			}
			// リサイズ時
			if(right.lvl && focus) {
				w->set_state(widget::state::RESIZE);
			}
		}

		bool touch = false;
		// 一番手前だけ選択される
		BOOST_FOREACH(widget* w, widgets_) {
			if(select == w) {
				w->set_state(widget::state::SELECT);
				touch = true;
			} else {
				w->set_state(widget::state::SELECT, false);
			}
		}

		if(top_move_) {
			if(top_move_->get_state(widget::state::MOVE_ROOT)) {
				top_move_ = root_widget(top_move_);
			}
		}
		if(top_resize_) {
			if(top_resize_->get_state(widget::state::RESIZE_ROOT)) {
				top_resize_ = root_widget(top_resize_);
			}
		}

		// フォーカス、セレクトの動的な状態は常に作成
		BOOST_FOREACH(widget* w, widgets_) {
			bool f;
			f = w->get_state(widget::state::IS_FOCUS);
			w->set_state(widget::state::BEFORE_FOCUS, f);
			f = w->get_state(widget::state::FOCUS);
			w->set_state(widget::state::IS_FOCUS, f);
			if(f) { w->at_param().in_point_ = msp - w->get_param().rpos_; }
			f = w->get_state(widget::state::IS_SELECT);
			w->set_state(widget::state::BEFORE_SELECT, f);
			f = w->get_state(widget::state::SELECT);
			w->set_state(widget::state::IS_SELECT, f);
			if(f) { ++w->at_param().hold_frame_; }
			else {
				if(w->get_state(widget::state::BEFORE_SELECT)) {
					w->at_param().holded_frame_ = w->get_param().hold_frame_;
				}
				w->at_param().hold_frame_ = 0;
			}
		}

		// リサイズ
		if(top_resize_ && !top_resize_->get_state(widget::state::SIZE_LOCK)) {
			if(right.lvl) {
				vtx::spos d = msp - top_resize_->at_param().resize_org_;
				if(!top_resize_->get_state(widget::state::RESIZE_H_ENABLE)) d.x = 0;
				if(!top_resize_->get_state(widget::state::RESIZE_V_ENABLE)) d.y = 0;
				const vtx::spos& min = top_resize_->get_param().resize_min_;
				const vtx::spos& size = top_resize_->get_rect().size;
				vtx::spos newsize = top_resize_->at_param().resize_ref_ + d;
				const vtx::spos& ref = top_resize_->at_param().resize_ref_;
				if(ref.x >= min.x) {
					if(newsize.x < min.x) newsize.x = min.x;
				} else {
					newsize.x = ref.x;
				}
				if(ref.y >= min.y) {
					if(newsize.y < min.y) newsize.y = min.y;
				} else {
					newsize.x = ref.x;
				}
				touch = true;
				top_resize_->at_rect().size = newsize;
			}
		}

		// 移動
		if(top_move_ && !top_move_->get_state(widget::state::POSITION_LOCK)) {
			touch = true;
			top_widget(top_move_);
			top_move_->at_rect().org = top_move_->get_param().move_pos_;
		}

		// 最後に各部品の update 処理 
		{
			widgets ws = widgets_;
			BOOST_FOREACH(widget* w, ws) {
				if(!w->get_state(widget::state::ENABLE)) continue;
				if(w->get_state(widget::state::STALL)) continue;
				if(w->get_state(widget::state::UPDATE_ENABLE)) {
					w->update();
				}
			}
		}

//		action_monitor();
		return touch;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_director::render()
	{
		IGLcore* Igl = get_glcore();
		if(Igl == 0) return;

		// クリップ領域を全てアップデート
		reset_mark();
		BOOST_FOREACH(widget* w, widgets_) {
			if(!w->get_state(widget::state::ENABLE)) {
				w->set_mark();
				continue;
			}
			widget* pw = w->get_param().parents_;
			if(pw == 0) {	// is root.
				w->set_mark();
				make_clip_(w);
				widgets ws;
				parents_widget(w, ws);
				clip_widgets_(ws, true);
			}
		}

		const vtx::spos& size = Igl->get_size();

		// 各 描画
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		BOOST_FOREACH(widget* w, widgets_) {
			const widget::param& pa = w->get_param();
			if(!pa.state_[widget::state::ENABLE]) continue;
			if(!pa.state_[widget::state::RENDER_ENABLE]) continue;
			if(w->get_param().clip_.size.x <= 0 || w->get_param().clip_.size.y <= 0) {
				continue;
			}

			mobj_.setup_matrix(size.x, size.y);
			vtx::spos pos(0);
			final_position(w, pos);
			float x = static_cast<float>(pos.x);
			float y = static_cast<float>(pos.y);
			float a = 1.0f;
			float i = 0.75f;
			float s = 1.0f;
			if(w->get_focus()) {
				if(pa.action_[widget::action::FOCUS_HIGHLIGHT]) {
					i = 1.0f;
				}
				if(pa.action_[widget::action::FOCUS_ALPHA25]) {
					a = 0.25f;
				}
				if(pa.action_[widget::action::FOCUS_ALPHA50]) {
					a = 0.5f;
				}
				if(pa.action_[widget::action::FOCUS_SCALE]) {
					s = 1.075f;
					float ss = (s - 1.0f) * 0.5f;
					x -= static_cast<float>(pa.rect_.size.x) * ss;
					y -= static_cast<float>(pa.rect_.size.y) * ss;
				}
			}
			if(w->get_select() || w->get_state(widget::state::SYSTEM_SELECT)) {
				if(pa.action_[widget::action::SELECT_HIGHLIGHT]) {
					i = 1.0f;
				}
				if(pa.action_[widget::action::SELECT_ALPHA]) {
					a = 0.5f;
				}
				if(pa.action_[widget::action::SELECT_SCALE]) {
					s = 1.075f;
					float ss = (s - 1.0f) * 0.5f;
					x -= static_cast<float>(pa.rect_.size.x) * ss;
					y -= static_cast<float>(pa.rect_.size.y) * ss;
				}
			}
			if(w->get_state(widget::state::STALL) ||
				w->get_state(widget::state::SYSTEM_STALL)) {
				i = 0.25f;
				a = 0.85f;
			}

			position_.set(x, y);
			scale_ = s;
			color_.set(i, i, i, a);

			set_TSC();

			w->render();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_director::service()
	{
		widgets ws;
		BOOST_FOREACH(widget* w, widgets_) {
			if(w->get_state(widget::state::SERVICE)) {
				ws.push_back(w);
			}
		}
		BOOST_FOREACH(widget* w, ws) {
			w->service();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_director::destroy()
	{
		// ハイブリッドから消去
		widgets hyws;
		BOOST_FOREACH(widget* w, widgets_) {
			if(w->hybrid()) {
				hyws.push_back(w);
			}
		}
		BOOST_FOREACH(widget* w, hyws) {
			del_widget(w);
		}

		BOOST_FOREACH(widget* w, widgets_) {
			delete w;
		}

		widgets().swap(widgets_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アクション・モニター
		@param[in]	wd	widget_director
	*/
	//-----------------------------------------------------------------//
	void widget_director::action_monitor()
	{
		static int id = 0;
		bool f = false;
		BOOST_FOREACH(widget* w, widgets_) {
			if(!w->get_state(widget::state::ENABLE)) continue;

			if(w->get_focus_in()) {
				message_widget_(w, "Focus In");
				f = true;
			}
			if(w->get_focus_out()) {
				message_widget_(w, "Focus Out");
				f = true;
			}
			if(w->get_select_in()) {
				message_widget_(w, "Select In");
				f = true;
			}
			if(w->get_select_out()) {
				message_widget_(w, "Select Out");
				f = true;
			}
		}
		if(f) {
			printf("(%d)\n", id);
			++id;
		}
		fflush(stdout);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	widget 固有の文字列を生成
		@param[in]	w	生成する widget
		@return widget 固有の文字列
	*/
	//-----------------------------------------------------------------//
	const std::string widget_director::create_widget_name(const widget* w) const
	{
		std::map<uint32_t, widget*> tbl;
		typedef std::pair<uint32_t, widget*> tbl_p;
		BOOST_FOREACH(widget* ww, widgets_) {
			if(w->type() == ww->type()) {
				tbl.insert(tbl_p(ww->get_serial(), ww));
			}
		}

		int n = 0;
		BOOST_FOREACH(const tbl_p& t, tbl) {
			if(t.second == w) break;
			++n;
		}

		std::string s = w->type_name();
		s += '/';
		s += (boost::format("%05d") % n).str();
		return s;
	}
}
