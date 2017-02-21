//=====================================================================//
/*!	@file
	@brief	GUI widget ディレクター
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <cstdio>
#include <map>
#include <boost/format.hpp>
#include "core/glcore.hpp"
#include "gl_fw/gl_info.hpp"
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
	widget::color_param widget_director::default_border_color_;
	widget::color_param widget_director::default_button_color_;
	widget::color_param widget_director::default_label_color_;
	widget::color_param widget_director::default_label_color_select_;
	widget::color_param widget_director::default_slider_color_;
	widget::color_param widget_director::default_progress_color_;
	widget::color_param widget_director::default_check_color_;
	widget::color_param widget_director::default_list_color_;
	widget::color_param widget_director::default_list_color_select_;
	widget::color_param widget_director::default_menu_color_;
	widget::color_param widget_director::default_menu_color_select_;
	widget::color_param widget_director::default_spinbox_color_;
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
				type += wl->get_local_param().text_param_.get_text();
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_button>()) {
			widget_button* wl = dynamic_cast<widget_button*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.get_text();
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_check>()) {
			widget_check* wl = dynamic_cast<widget_check*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.get_text();
				type += "'";
			}
		} else if(w->type() == get_type_id<widget_radio>()) {
			widget_radio* wl = dynamic_cast<widget_radio*>(w);
			if(wl) {
				type += ": ";
				type += "'";
				type += wl->get_local_param().text_param_.get_text();
				type += "'";
			}
		}

		std::cout << boost::format("(%d:%s)%s: '%s'") % w->get_serial() % w->get_symbol() % s % type << std::endl;
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
		for(auto w : ws) {
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
		for(auto w : ws) {
			w->set_mark();
		}
	}


	void widget_director::unselect_parents_(widget* root)
	{
		if(root == nullptr) return;

		root->set_state(widget::state::SELECT, false);
		root->set_state(widget::state::FOCUS, false);
		root->set_state(widget::state::DRAG, false);

		if(root->get_state(widget::state::SELECT_CHILDS)) {
			widgets ws;
			parents_widget(root, ws);
			for(auto w : ws) {
				if(w->get_state(widget::state::SELECT_PARENTS)) {
					w->set_state(widget::state::SELECT, false);
					w->set_state(widget::state::FOCUS, false);
					w->set_state(widget::state::DRAG, false);
				}
			}
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
		if(w == nullptr) return false;

		widgets ws;
		for(auto ww : widgets_) {
			if(ww != w) {
				ws.push_back(ww);
			}
		}
		widgets_ = ws;

		if(select_widget_ == w) select_widget_ = nullptr;
		if(move_widget_ == w) move_widget_ = nullptr;
		if(resize_l_widget_ == w) resize_l_widget_ = nullptr;
		if(resize_r_widget_ == w) resize_r_widget_ = nullptr;
		if(top_widget_ == w) top_widget_ = nullptr;
		if(focus_widget_ == w) select_widget_ = nullptr;

		del_mark_.insert(w);

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
		for(auto w : widgets_) {
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
		if(w == nullptr) return nullptr;

		do {
			if(w->get_param().parents_ == nullptr) {
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
		for(auto w : widgets_) {
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
		if(w == nullptr) return;

		reset_mark();
		widgets ws;
		ws.push_back(w);
		parents_widget(w, ws);
		for(auto cw : ws) {
			cw->set_mark();
		}
		widgets wss;
		for(auto cw : widgets_) {
			if(!cw->get_mark()) {
				wss.push_back(cw);
			}
		}
		for(auto cw : ws) {
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

		mobj_.initialize();

		// 「各」標準的な色彩や、形状などの設定
		img::rgba8 fc, bc;
		fc.set( 55, 157, 235);
		bc = fc * 0.7f;
		default_frame_color_ = widget::color_param(fc, bc);

		fc.set( 72, 193, 241);
		bc = fc * 0.7f;
		default_border_color_ = widget::color_param(fc, bc);

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

		fc.set( 48, 193, 241);
		bc.set( 31,  72,  86);
		default_progress_color_ = widget::color_param(fc, bc);

		fc.set( 72, 193, 241);
		bc.set( 47,  72,  86);
		default_check_color_ = widget::color_param(fc, bc);

		fc.set( 48, 193, 251);
		bc = fc * 0.7f;
		default_list_color_ = widget::color_param(fc, bc);
		fc.set(118, 223, 251);
		bc = fc * 0.7f;
		default_list_color_select_ = widget::color_param(fc, bc);

		fc.set( 38, 123, 180);
		bc = fc * 0.7f;
		default_menu_color_ = widget::color_param(fc, bc);
		fc.set(118, 223, 251);
		bc = fc * 0.7f;
		default_menu_color_select_ = widget::color_param(fc, bc);

		fc.set( 72, 193, 241);
		bc = fc * 0.7f;
		default_spinbox_color_ = widget::color_param(fc, bc);

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

		// スピンボックスの頂点輝度設定
		ir.left_top    = 200; ir.center_top    = 255; ir.right_top    = 200;
		ir.left_center = 255; ir.center_center = 255; ir.right_center = 255;
		ir.left_bottom = 180; ir.center_bottom = 255; ir.right_bottom = 180;
		default_spinbox_color_.inten_rect_ = ir;
		default_spinbox_color_.ir_enable_ = true;

		core& core = core::get_instance();
		fonts& fonts = core.at_fonts();

		// ターミナル用フォントのインストール
		fonts.push_font_face();
		{
#ifdef __APPLE__
			std::string ff = "~/Library/Fonts/Inconsolata.otf";
#elif __linux__
			std::string ff = "inconsolata/Inconsolata.otf";
#else
			std::string ff = "Inconsolata.otf";
#endif
			if(!fonts.install_font_type(ff, "Inconsolata")) {
				std::cerr << boost::format("Can't find font file: '%s'") % ff << std::endl; 
			}
		}
		fonts.pop_font_face();
#ifdef WIN32
		fonts.install_font_type("meiryo.ttc", "meiryo");
#endif
		fonts.set_font_size(20);
		fonts.set_clip_size(core.get_rect().size);

		// 共通部品の作成
		{
			using vtx::spos;

			spos size(24);
			img::paint fr;
			widget::plate_param pp;
			pp.frame_width_  = 4;
			pp.round_radius_ = 4;
			create_round_frame(fr, pp, default_check_color_, size);
			share_img_.un_check_ = mobj_.install(&fr);

			img::paint pa;
			pa = fr;

			// スクエアドットでチェックされた見た目
			pa.swap_color();
			pa.fill_rect(8, 8, 8, 8);
			share_img_.to_check_ = mobj_.install(&pa);

			// でチェックされた見た目
//			pa.swap_color();
//			pa.fill_rect(8, 8, 8, 8);
//			share_img_.to_check_ = mobj_.install(&pa);

			// 「－」でチェックされた見た目
			pa = fr;
			pa.swap_color();
			pa.fill_rect(6, 10, 12, 4);
			share_img_.minus_box_ = mobj_.install(&pa);

			// 「＋」でチェックされた見た目
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

			// ツリー表示用部品
			pa.fill(img::rgba8(0, 0));
			pa.set_fore_color(default_check_color_.fore_color_);
			pa.fill_rect(10,  0,  4, 14);
			pa.fill_rect(10, 10, 14,  4);
			share_img_.R_junction_ = mobj_.install(&pa);

			pa.fill(img::rgba8(0, 0));
			pa.set_fore_color(default_check_color_.fore_color_);
			pa.fill_rect(10, 0, 4, 24);
			share_img_.V_line_ = mobj_.install(&pa);

			pa.fill_rect(10, 10, 14, 4);
			share_img_.VR_junction_ = mobj_.install(&pa);

			pa.fill(img::rgba8(0, 0));
			pa.set_fore_color(default_check_color_.fore_color_);
			pa.fill_rect(0, 10, 24, 4);
			share_img_.H_line_ = mobj_.install(&pa);
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
		{  // ダイアログがある場合の優先順位とストール処理
			reset_mark();
			widgets ws;
			for(auto w : widgets_) {
				if(!w->get_state(widget::state::ENABLE)) continue;
				if(w->type() == get_type_id<widget_dialog>()) {
					parents_widget_mark_(w);
					ws.push_back(w);
				}
			}
			if(ws.empty()) {
				for(auto w : widgets_) {
					w->set_state(widget::state::SYSTEM_STALL, false);
				}
			} else {
				for(auto w : widgets_) {
					if(!w->get_mark()) {
						w->set_state(widget::state::SYSTEM_STALL);
					}
				}
			}
			for(auto w : ws) {
				top_widget(w);
			}
		}

		core& core = core::get_instance();
		const device& dev = core.get_device();

		scroll_ = dev.get_locator().get_scroll();

		const vtx::spos& msp = dev.get_locator().get_cursor();

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
			unselect_parents_(select_widget_);
			select_widget_ = nullptr;
			if(move_widget_) {
				move_widget_->set_state(widget::state::SELECT, false);
				move_widget_->set_state(widget::state::DRAG, false);
				move_widget_ = nullptr;
			}
			if(resize_l_widget_) {
				resize_l_widget_->set_state(widget::state::SELECT, false);
				resize_l_widget_->set_state(widget::state::DRAG, false);
				resize_l_widget_ = nullptr;
			}
		}
		if(!right.lvl) {
			if(resize_r_widget_) {
				resize_r_widget_->set_state(widget::state::SELECT, false);
				resize_r_widget_->set_state(widget::state::DRAG, false);
				resize_r_widget_ = nullptr;
			}
		}

		msp_speed_ = msp;

		if(left.pos) {
			position_positive_ = msp;
			position_level_ = msp;
		}
		if(left.lvl) {
			msp_length_ += std::sqrt((msp - position_level_).sqr());
			position_level_ = msp;
		} else {
			msp_length_ = 0.0f;
		}
		if(left.neg) {
			position_negative_ = msp;
		}

		// フォーカス、選択、を決定
		bool resize_trigger = false;
		bool select_trigger = false;
		for(auto w : widgets_) {
			if(!w->get_state(widget::state::ENABLE) ||
			  w->get_state(widget::state::STALL) ||
			  w->get_state(widget::state::SYSTEM_STALL)) {
				w->set_state(widget::state::BEFORE_FOCUS, false);
				w->set_state(widget::state::FOCUS, false);
				w->set_state(widget::state::IS_FOCUS, false);
				w->set_state(widget::state::BEFORE_SELECT, false);
				w->set_state(widget::state::SELECT, false);
				w->set_state(widget::state::IS_SELECT, false);
				w->set_state(widget::state::SELECTED, false);
				continue;
			}

			// クリッピングフォーカス（クリッピング範囲）は、全てに対して評価する。
			// ※FOCUS_ENABLE が有効な場合に限る
			bool focus = w->get_param().clip_.is_focus(msp);
			if(w->get_state(widget::state::FOCUS_ENABLE)) {
				w->set_state(widget::state::FOCUS, focus);
			}

			if(!focus) continue;

			if(left.pos) {  // LEFT 選択、移動、エッジリサイズ
				focus_widget_ = w;
				if(w->get_state(widget::state::RESIZE_EDGE_ENABLE)) {
					// キャプションを握った場合はリサイズしない（現状キャプションがあるのは、「frame」のみ）
					bool caption = false;
					if(w->type() == get_type_id<widget_frame>()) {
						widget_frame* wf = static_cast<widget_frame*>(w);
						auto r = w->get_param().clip_;
						r.size.y = wf->get_local_param().plate_param_.caption_width_;
						if(r.is_focus(msp)) {
							caption = true;
						}
					}
					if(!caption) {
						auto r = w->get_param().clip_;
						r.org  += 8;
						r.size -= 16;
						if(!r.is_focus(msp)) {
							if(w->get_state(widget::state::SIZE_LOCK)) continue;
							resize_l_widget_ = w;
							resize_trigger = true;
							continue;
						}
					}
				}

				select_widget_ = w;
				select_trigger = true;
				if(w->get_state(widget::state::MOVE_STALL)) move_widget_ = nullptr;
				if(w->get_state(widget::state::POSITION_LOCK)) continue;
				move_widget_ = w;
			} else if(right.pos) {  // RIGHT 選択
				if(w->get_state(widget::state::SIZE_LOCK)) continue;
				resize_r_widget_ = w;
				resize_trigger = true;
			}
		}
		if(left.pos && move_widget_) {
			move_widget_->at_param().move_org_ = move_widget_->get_rect().org;
		}

		// 選択権の追跡
		if(select_trigger) {
			if(select_widget_->get_state(widget::state::SELECT_ROOT)) {
				select_widget_ = root_widget(select_widget_);
			}
			select_widget_->set_state(widget::state::SELECT);
			if(move_widget_) {
				if(move_widget_->get_state(widget::state::MOVE_ROOT)) {
					move_widget_ = root_widget(move_widget_);
				}
				move_widget_->set_state(widget::state::SELECT);
			}
		}
		if(resize_trigger) {
			if(resize_l_widget_) {
				if(resize_l_widget_->get_state(widget::state::RESIZE_ROOT)) {
					resize_l_widget_ = root_widget(resize_l_widget_);
				}
			}
			if(resize_r_widget_) {
				if(resize_r_widget_->get_state(widget::state::RESIZE_ROOT)) {
					resize_r_widget_ = root_widget(resize_r_widget_);
				}
			}
		}

		// 「ペアレンツ選択」の操作
		if(select_trigger) {
			if(select_widget_ && select_widget_->get_state(widget::state::SELECT_PARENTS)) {
				widget* w = select_widget_->get_param().parents_;
				if(w) {
					select_widget_ = w;
					if(select_widget_->get_state(widget::state::SELECT_CHILDS)) {
						widgets ws;
						parents_widget(select_widget_, ws);
						BOOST_FOREACH(widget* w, ws) {
							if(w->get_state(widget::state::SELECT_PARENTS)) {
								w->set_state(widget::state::SELECT);
							}
						}
					}
				}
			}
		}

		if(resize_trigger && (resize_l_widget_ || resize_r_widget_)) {
			widget* rw = resize_l_widget_;
			if(resize_r_widget_) rw = resize_r_widget_;
			rw->at_param().resize_org_ = rw->get_rect().org;
			vtx::spos sign(1);
			if(msp.x < (rw->get_rect().org.x + rw->get_rect().size.x / 2)) {
				sign.x = -1;
			}
			if(msp.y < (rw->get_rect().org.y + rw->get_rect().size.y / 2)) {
				sign.y = -1;
			}
			rw->at_param().resize_sign_ = sign;
			rw->at_param().resize_pos_ = msp;
			rw->at_param().resize_org_ = rw->get_rect().org;
			rw->at_param().resize_ref_ = rw->get_rect().size;
			rw->set_state(widget::state::SELECT);
		}

		// グループ的「選択」を実現するフォーカスの伝搬機能
		if(select_widget_ && select_widget_->get_state(widget::state::FOCUS_CHILDS)) {
			widgets ws;
			parents_widget(select_widget_, ws);
			for(auto w : ws) {
				if(!w->get_state(widget::state::FOCUS_ENABLE)) continue;
				w->set_state(widget::state::FOCUS);
			}
		}

		// ドラッグにより非選択になる
		if(select_widget_ && left.lvl) {
			widget* w = select_widget_;
			if(w->get_state(widget::state::DRAG_UNSELECT) && msp_length_ > unselect_length_) {
				unselect_parents_(w);
				select_widget_ = nullptr;
			}
		}

		// フォーカス、セレクトの動的な状態は常に作成
		for(auto w : widgets_) {
			if(!w->get_state(widget::state::ENABLE)) continue;
			bool f;
			f = w->get_state(widget::state::DRAG);
			w->set_state(widget::state::BEFORE_DRAG, f);
			f = w->get_state(widget::state::RESIZE);
			w->set_state(widget::state::BEFORE_RESIZE, f);
			f = w->get_state(widget::state::IS_FOCUS);
			w->set_state(widget::state::BEFORE_FOCUS, f);
			f = w->get_state(widget::state::FOCUS);
			w->set_state(widget::state::IS_FOCUS, f);
			if(f) { w->at_param().in_point_ = msp - w->get_param().rpos_; }
			f = w->get_state(widget::state::IS_SELECT);
			w->set_state(widget::state::BEFORE_SELECT, f);
			f = w->get_state(widget::state::SELECT);
			w->set_state(widget::state::IS_SELECT, f);
			if(f) {
				++w->at_param().hold_frame_;
			} else {
				if(w->get_state(widget::state::BEFORE_SELECT)) {
					w->at_param().holded_frame_ = w->get_param().hold_frame_;
				}
				w->at_param().hold_frame_ = 0;
			}
		}

		bool touch = false;

		// リサイズ処理
		// 移動がある場合、リサイズしない
		if(resize_l_widget_ || resize_r_widget_) {
			widget* rw = nullptr;
			if(left.lvl && resize_l_widget_) rw = resize_l_widget_;
			if(right.lvl && resize_r_widget_) rw = resize_r_widget_;
			if(rw) {
				if(rw->get_state(widget::state::RESIZE_TOP)) {
					top_widget(rw);
				}
				rw->set_state(widget::state::RESIZE);
				const widget::param& param = rw->get_param();
				vtx::spos d = msp - param.resize_pos_;
				if(param.resize_sign_.x < 0) d.x = -d.x;
				if(param.resize_sign_.y < 0) d.y = -d.y;
				if(!rw->get_state(widget::state::RESIZE_H_ENABLE)) d.x = 0;
				if(!rw->get_state(widget::state::RESIZE_V_ENABLE)) d.y = 0;
				// X、Y が、フレームの中心付近（１／３）にある場合は片軸固定
				{
					vtx::spos ds = param.resize_pos_ - param.resize_org_;
					vtx::spos s = param.resize_ref_ / 3;
					ds -= s;
					if(ds.x >= 0 && ds.x < s.x) d.x = 0;
					if(ds.y >= 0 && ds.y < s.y) d.y = 0;
				}
				const vtx::spos& min = param.resize_min_;
				const vtx::spos& ref = param.resize_ref_;
				vtx::spos newsize = ref + d;
				if(newsize.x < min.x) newsize.x = min.x;
				if(newsize.y < min.y) newsize.y = min.y;
				vtx::spos ofs(0);
				if(param.resize_sign_.x < 0) ofs.x = ref.x - newsize.x;
				if(param.resize_sign_.y < 0) ofs.y = ref.y - newsize.y;
				rw->at_rect().org  = param.resize_org_ + ofs;
				rw->at_rect().size = newsize;
				touch = true;
			}
		}

		// 移動時
		if(!touch && move_widget_ && left.lvl) {
			if(msp_length_) {
				move_widget_->set_state(widget::state::DRAG);
			}
			if(move_widget_->get_state(widget::state::MOVE_TOP)) {
				top_widget(move_widget_);
			}
			vtx::spos d = msp - position_positive_;
			move_widget_->at_param().move_pos_ = move_widget_->get_param().move_org_ + d;
			move_widget_->at_rect().org = move_widget_->get_param().move_pos_;
			touch = true;
		}

		// 最上位を更新
		if(!widgets_.empty()) {
			top_widget_ = root_widget(widgets_.back());
		}

		// 最後に各部品の update 処理
		{
			del_mark_.clear();
			widgets ws = widgets_;
			for(auto w : ws) {
				if(del_mark_.find(w) != del_mark_.end()) continue;
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
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_director::service()
	{
		// キーボードのサービス
		keyboard_.service();

		del_mark_.clear();
		widgets ws = widgets_;
		for(auto w : ws) {
///			if(!w->get_state(widget::state::ENABLE)) continue;
			if(del_mark_.find(w) != del_mark_.end()) continue;
			if(w->get_state(widget::state::SERVICE)) {
				w->service();
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_director::render()
	{
		core& core = core::get_instance();

		const vtx::spos& size = core.get_rect().size;
		// WIN32 では、アプリケーションを待機状態にすると、サイズが「０」などが来る
		if(size.x <= 0 || size.y <= 0) return;

		// クリップ領域を全てアップデート
		reset_mark();
		for(auto w : widgets_) {
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

		// 各 描画
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// フォントは基本、バックカラーを描画しない
		core.at_fonts().enable_back_color(false);

		uint32_t rn = 0;
		for(auto w : widgets_) {
			const widget::param& pa = w->get_param();
			if(!pa.state_[widget::state::ENABLE]) continue;
			if(!pa.state_[widget::state::RENDER_ENABLE]) continue;
			if(w->get_param().clip_.size.x <= 0 || w->get_param().clip_.size.y <= 0) {
				continue;
			}

			mobj_.setup_matrix(size.x, size.y);
			vtx::ipos pos(0);
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
			if((w->get_select() && w->get_focus())
			   || w->get_state(widget::state::SELECTED)) {
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
			++rn;
		}
		if(render_num_ < rn) {
			render_num_ = rn;
///			std::cout << "Render peak num: " << rn << std::endl;
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
		for(auto w : widgets_) {
			if(w->hybrid()) {
				hyws.push_back(w);
			}
		}
		for(auto w : hyws) {
			del_widget(w);
		}

		for(auto w : widgets_) {
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
		for(auto w : widgets_) {
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
			std::cout << boost::format("(%d)\n") % id;
			++id;
		}
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
		for(auto ww : widgets_) {
			if(w->type() == ww->type()) {
				tbl.insert(tbl_p(ww->get_serial(), ww));
			}
		}

		int n = 0;
		for(const auto& t : tbl) {
			if(t.second == w) break;
			++n;
		}

		std::string s = w->type_name();
		s += '/';
		s += (boost::format("%05d") % n).str();
		return s;
	}
}
