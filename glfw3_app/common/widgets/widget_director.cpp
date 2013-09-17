//=====================================================================//
/*!	@file
	@brief	GUI widget ディレクター
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/gl_info.hpp"
#include "gl_fw/IGLcore.hpp"
#include "widget_director.hpp"
#include "widget_utils.hpp"
#include "widget_null.hpp"
#include "widget_image.hpp"
#include "widget_frame.hpp"
#include "widget_button.hpp"
#include "widget_label.hpp"
#include "widget_slider.hpp"
#include "widget_check.hpp"
#include "widget_radio.hpp"
#include "widget_list.hpp"
#include <boost/foreach.hpp>

#include <cstdio>

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

	void widget_director::message_widget_(widget* w, const std::string& s)
	{
		std::string type;

		if(w->type() == get_type_id<widget_label>()) {
			type = "label";
			widget_label* wl = dynamic_cast<widget_label*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_null>()) {
			type = "null";
		} else if(w->type() == get_type_id<widget_image>()) {
			type = "image";
		} else if(w->type() == get_type_id<widget_frame>()) {
			type = "frame";
		} else if(w->type() == get_type_id<widget_button>()) {
			type = "button";
			widget_button* wl = dynamic_cast<widget_button*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_slider>()) {
			type = "slider";
		} else if(w->type() == get_type_id<widget_check>()) {
			type = "check";
			widget_check* wl = dynamic_cast<widget_check*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_radio>()) {
			type = "radio";
			widget_radio* wl = dynamic_cast<widget_radio*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.text_;
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_list>()) {
			type = "list";
		} else {
			type = "(none)";
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

		widgets tmp;
		parents_widget(root, tmp);
		BOOST_FOREACH(widget* w, tmp) {
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
		@brief	最も手前のウィジェットを返す
		@param[in]	w	基準ウィジェット
		@return 手前のウィジェット
	*/
	//-----------------------------------------------------------------//
	widget* widget_director::get_top_widget(widget* w) const
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
		default_label_color_select_ = widget::color_param(fc * 1.2f, fc);

		fc.set( 48, 193, 241);
		bc.set( 31,  72,  86);
		default_slider_color_ = widget::color_param(fc, bc);

		fc.set( 72, 193, 241);
		bc.set( 47,  72,  86);
		default_check_color_ = widget::color_param(fc, bc);

		fc.set( 48, 193, 241);
		bc = fc * 0.7f;
		default_list_color_ = widget::color_param(fc, bc);
		default_list_color_select_ = widget::color_param(bc, fc * 1.2f);

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

		glfonts& fonts = igl->at_fonts();
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
	*/
	//-----------------------------------------------------------------//
	void widget_director::update()
	{
		IGLcore* Igl = get_glcore();
		if(Igl == 0) return;

		const device& dev = Igl->get_device();

		scroll_ = dev.get_scroll();

		const vtx::spos& msp = dev.get_cursor();

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
		widgets gs;
		BOOST_FOREACH(widget* w, widgets_) {
			if(w->get_state(widget::state::STALL)) continue;

			bool focus = w->get_param().clip_.is_focus(msp);
			if(w->get_state(widget::state::FOCUS_ENABLE)) {
				w->set_state(widget::state::FOCUS, focus);
			}
			bool select = false;
			if(focus) {
				select = left.lvl;
			}
			if(w->get_state(widget::state::SELECT_ENABLE)) {
				if(w->get_state(widget::state::DRAG_UNSELECT)
				  && msp_length_ > unselect_length_) {
					select = false;
				}
				w->set_state(widget::state::SELECT, select);
				if(w->get_state(widget::state::SELECT_PARENTS) && select) {
					gs.push_back(w);
				}
			}
			w->at_param().speed_ = msp_speed;

			// 移動を行う widget 候補を選択
			if(left.pos && focus && !w->get_state(widget::state::POSITION_LOCK)) {
				top_move_ = w;
			}
			// リサイズを行う widget 候補を選択
			if(right.pos && focus && !w->get_state(widget::state::SIZE_LOCK)) {
				top_resize_ = w;
			}
		}

		// フォーカスとセレクトの状態を作る。
		BOOST_FOREACH(widget* w, gs) {
			if(w->get_state(widget::state::STALL)) continue;
			BOOST_FOREACH(widget* tw, widgets_) {
				if(w == tw->get_param().parents_) {
					tw->set_state(widget::state::SELECT, true);
				}
			}
		}
		BOOST_FOREACH(widget* w, widgets_) {
			if(w->get_state(widget::state::STALL)) continue;
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
		if(top_resize_) {
			if(right.pos) {
				top_resize_->at_param().resize_org_ = msp;
				top_resize_->at_param().resize_ref_ = top_resize_->get_rect().size;
			}
			if(right.lvl) {
				vtx::spos d = msp - top_resize_->at_param().resize_org_;
				if(!top_resize_->get_state(widget::state::RESIZE_H_ENABLE)) d.x = 0;
				if(!top_resize_->get_state(widget::state::RESIZE_V_ENABLE)) d.y = 0;
				vtx::spos newsize = top_resize_->get_param().resize_ref_ + d;
				vtx::spos min = top_resize_->get_param().resize_min_;
				const vtx::spos& size = top_resize_->get_rect().size;
				if(size.x < min.x) min.x = size.x;
				if(size.y < min.y) min.y = size.y;
   				if(newsize.x < min.x) newsize.x = min.x;
				if(newsize.y < min.y) newsize.y = min.y;
				top_resize_->at_rect().size = newsize;
			}
			if(right.neg) {
				top_resize_ = 0;
			}
		}

		// 移動
		if(top_move_) {
			if(left.pos) {
				top_move_->at_param().move_org_ = top_move_->get_rect().org;
			}
			vtx::spos d = msp - position_positive_;
			top_move_->at_rect().org = top_move_->at_param().move_org_ + d;
			if(!left.lvl) {
				top_move_ = 0;
			}
		}

		// 最後に各部品の update 処理 
		BOOST_FOREACH(widget* w, widgets_) {
			if(!w->get_state(widget::state::ENABLE)) continue;
			if(w->get_state(widget::state::STALL)) continue;
			if(w->get_state(widget::state::UPDATE_ENABLE)) {
				w->update();
			}
		}

///		action_monitor();
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
			if(w->get_param().clip_.size.x <= 0 || w->get_param().clip_.size.y <= 0) continue;

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
			if(w->get_select()) {
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
			if(w->get_state(widget::state::STALL)) {
				i = 0.25f;
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
		BOOST_FOREACH(widget* w, widgets_) {
			if(!w->get_state(widget::state::ENABLE)) continue;

			if(w->get_focus_in()) message_widget_(w, "Focus In");
			if(w->get_focus_out()) message_widget_(w, "Focus Out");
			if(w->get_select_in()) message_widget_(w, "Select In");
			if(w->get_select_out()) message_widget_(w, "Select Out");
		}
		fflush(stdout);
	}
}
