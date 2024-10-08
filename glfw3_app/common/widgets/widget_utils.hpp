#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget ユーティリティー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include "core/glcore.hpp"
#include "utils/vmath.hpp"
#include "utils/string_utils.hpp"
#include "gl_fw/glmobj.hpp"
#include "gl_fw/glutils.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_utils.hpp"
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像を作成
		@param[in]	image	画像
		@param[in]	size	サイズ
		@param[out]	pa		ペイントクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void create_image_base(const img::i_img* image, const vtx::ipos& size, img::paint& pa)
	{
		vtx::ipos s;
		vtx::ipos o;
		if(size.x <= 0) {
			o.x = 0;
			s.x = image->get_size().x;
		} else {
			o.x = (size.x - image->get_size().x) / 2;
			s.x = size.x;
		}
		if(size.y <= 0) {
			o.y = 0;
			s.y = image->get_size().y;
		} else {
			o.y = (size.y - image->get_size().y) / 2;
			s.y = size.y;
		}
		pa.create(s, true);
		pa.fill(img::rgba8(0));
		img::copy_to_rgba8(image, vtx::srect(vtx::spos(0), s), pa, o);
	}


	static void mix_round_(img::paint& pa, const widget::color_param& cp, const widget::plate_param& pp)
	{
		if(pp.round_style_ == widget::plate_param::round_style::ALL) {
			return;
		}

		const vtx::ipos& size = pa.get_size();
		img::paint npa;
		npa.create(size, pa.test_alpha());
		npa.fill(img::rgba8(0, 0));
		npa.set_fore_color(cp.fore_color_);
		npa.set_back_color(cp.back_color_);
		if(pp.frame_width_) {
			npa.set_round(0);
			npa.fill_rect();
			npa.set_intensity_rect(cp.inten_rect_);
			npa.swap_color();
			short wf = pp.frame_width_;
			if(pp.round_style_ == widget::plate_param::round_style::UPPER_OPEN) {
				npa.fill_rect(wf, wf + wf, size.x - 2 * wf, size.y - wf, cp.ir_enable_);
			} else if(pp.round_style_ == widget::plate_param::round_style::UNDER_OPEN) {
				npa.fill_rect(wf, wf, size.x - 2 * wf, size.y - wf, cp.ir_enable_);
			} else {
				npa.fill_rect(wf, wf, size.x - 2 * wf, size.y - 2 * wf, cp.ir_enable_);
			}
		} else {
			npa.set_intensity_rect(cp.inten_rect_);
			npa.set_round(0);
			npa.swap_color();
			npa.fill_rect(cp.ir_enable_);
		}

		if(pp.round_style_ == widget::plate_param::round_style::UPPER_OPEN) {
			pa.copy(vtx::spos(0), npa, vtx::srect(0, 0, size.x, pp.round_radius_));
		} else if(pp.round_style_ == widget::plate_param::round_style::UNDER_OPEN) {
			pa.copy(vtx::spos(0, size.y - pp.round_radius_), npa,
				vtx::srect(0, size.y - pp.round_radius_, size.x, pp.round_radius_));
		} else if(pp.round_style_ == widget::plate_param::round_style::TOP) {
			pa.copy(vtx::spos(0, size.y - pp.round_radius_), npa,
				vtx::srect(0, size.y - pp.round_radius_, size.x, pp.round_radius_));
		} else if(pp.round_style_ == widget::plate_param::round_style::BOTTOM) {
			pa.copy(vtx::spos(0), npa, vtx::srect(0, 0, size.x, pp.round_radius_));
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ラウンド・フレームの作成
		@param[out]	pa		ペイント・フレーム
		@param[in]	pp		プレート・パラメーター
		@param[in]	cp		カラー・パラメーター
		@param[in]	size	サイズ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void create_round_frame(img::paint& pa, const widget::plate_param& pp, const widget::color_param& cp, const vtx::ipos& size)
	{
		pa.create(size, true);
		pa.fill(img::rgba8(0, 0));
		pa.set_fore_color(cp.fore_color_);
		pa.set_back_color(cp.back_color_);
		if(pp.frame_width_) {
			pa.set_round(pp.round_radius_);
			pa.fill_rect();
			pa.set_intensity_rect(cp.inten_rect_);
			short wf = pp.frame_width_;
			short rd;
			if(pp.round_style_ == widget::plate_param::round_style::NONE) rd = 0;
			else if(pp.round_radius_ > wf) rd = pp.round_radius_ - wf;
			else rd = 0;
			pa.set_round(rd);
			pa.swap_color();
			pa.alpha_blend();
			pa.fill_rect(wf, wf + pp.caption_width_, size.x - 2 * wf,
				size.y - 2 * wf - pp.caption_width_, cp.ir_enable_);
			mix_round_(pa, cp, pp);
		} else {
			pa.set_intensity_rect(cp.inten_rect_);
			short rd;
			if(pp.round_style_ == widget::plate_param::round_style::NONE) rd = 0;
			else rd = pp.round_radius_;
			pa.set_round(rd);
			pa.swap_color();
			pa.fill_rect(cp.ir_enable_);
			mix_round_(pa, cp, pp);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキストを設定
		@param[in]	w	ウィジェット
		@param[in]	text	テキスト
		@return 失敗なら「false」
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class W>
	void set_widget_text(W* w, const std::string& text) {
		w->at_local_param().text_param_.set_text(text);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	親子関係を継承した位置を合算する
		@param[in]	root	起点になるウィジェット
		@param[out]	pos		位置を受け取る参照
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void final_position(const widget* root, vtx::ipos& pos)
	{
		pos.set(0);
		while(root != 0) {
			const widget::param& wp = root->get_param();
			pos += wp.rect_.org;
			root = wp.parents_;
		}
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	親子関係を継承したクリップ領域を得る
		@param[in]	root	起点になるウィジェット
		@param[out]	org		基点を受け取る参照
		@param[out]	rect	領域を受け取る参照
		@return クリップ内なら「true」
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static bool final_clip(widget* root, vtx::ipos& org, vtx::irect& rect)
	{
		widgets ws;
		vtx::irect sr(vtx::ipos(0), root->get_rect().size);
		while(root != 0) {
			sr.org += root->get_rect().org;
			ws.push_back(root);
			root = root->get_param().parents_;
		}

		widgets_cit cit = ws.end();
		vtx::ipos psum(0);
		vtx::ipos top = sr.org;
		while(cit != ws.begin()) {
			--cit;
			vtx::irect r = (*cit)->get_rect();
			r.org += psum;
			psum = r.org;
			if(!sr.clip(r)) {
				rect.size.set(0);
				return false;
			}
		}
		rect = sr;
		org = top - sr.org;

		return true;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	オブジェクトの描画
		@param[in]	wd		Widget ディレクター
		@param]in]	h		オブジェクト・ハンドル
		@param[in]	clip	描画エリア
		@param[in]	ofs		描画オフセット
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void draw_mobj(widget_director& wd, gl::mobj::handle h, const vtx::irect& clip, const vtx::ipos& ofs = vtx::ipos(0))
	{
		gl::core& core = gl::core::get_instance();

		const vtx::ipos& vsz = core.get_size();
///		const vtx::ipos& siz = core.get_rect().size;

///		int sx = vsz.x / siz.x;
///		int sy = vsz.y / siz.y;
		float sx = core.get_dpi_scale();
		float sy = core.get_dpi_scale();
		glViewport(clip.org.x * sx, vsz.y - clip.org.y * sy - clip.size.y * sy, clip.size.x *sx, clip.size.y * sy);
		wd.at_mobj().setup_matrix(clip.size.x, clip.size.y);

		wd.at_mobj().draw(h, gl::mobj::attribute::normal, ofs);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキストのサイズ取得
		@param[in]	tp	テキスト・パラメーター
		@return テキストのサイズ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static vtx::ipos get_text_size(const widget::text_param& tp)
	{
		gl::core& core = gl::core::get_instance();

		utils::lstring ls;
		if(tp.alias_enable_ && tp.cursor_ < 0) {
			ls = tp.alias_;
		} else {
			ls = tp.text_;
		}
		if(ls.empty()) return vtx::ipos(0);

		gl::fonts& fonts = core.at_fonts();

		if(!tp.font_.empty()) {
			fonts.push_font_face();
			fonts.set_font_type(tp.font_);
			fonts.set_font_size(tp.font_size_);
		}

		auto fsize = fonts.get_size(ls);
//		vtx::placement tpl = tp.placement_;

		if(!tp.font_.empty()) {
			fonts.pop_font_face();
		}
		return fsize;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フォントの描画
		@param[in]	tp	テキスト・パラメーター
		@param[in]	rect   	描画位置
		@param[in]	clip	描画エリア
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void draw_text(const widget::text_param& tp, const vtx::irect& rect, const vtx::irect& clip)
	{
		gl::core& core = gl::core::get_instance();

		utils::lstring ls;
		if(tp.alias_enable_ && tp.cursor_ < 0) {
			ls = tp.alias_;
		} else {
			ls = tp.text_;
		}
		if(ls.empty()) return;

		const vtx::ipos& vsz = core.get_size();
//		const vtx::ipos& siz = core.get_rect().size;

		gl::fonts& fonts = core.at_fonts();

		vtx::irect clip_ = clip;
		vtx::irect rect_ = rect;

		if(!tp.font_.empty()) {
			fonts.push_font_face();
			fonts.set_font_type(tp.font_);
			fonts.set_font_size(tp.font_size_);
		} else {
			fonts.set_font_size(tp.font_size_);
		}

//		float sx = vsz.x / siz.x;
//		float sy = vsz.y / siz.y;
		float sx = core.get_dpi_scale();
		float sy = core.get_dpi_scale();
		glViewport(clip_.org.x * sx, vsz.y - clip_.org.y * sy - clip_.size.y * sy,
			clip_.size.x * sx, clip_.size.y * sy);
		fonts.setup_matrix(clip_.size.x, clip_.size.y);

		fonts.enable_proportional(tp.proportional_);
		const auto& fsize = fonts.get_size(ls);
		vtx::placement tpl = tp.placement_;
		if(fsize.x > clip_.size.x) {
			tpl.hpt = vtx::placement::holizontal::LEFT;
		}
		vtx::ipos pos;
		vtx::create_placement(rect_, fsize, tpl, pos);

		short clx = 0;
		if(utils::string_strchr(ls, '\n')) {
			clx = clip_.size.x;
		}

		pos += tp.offset_;
		if(tp.shadow_offset_.x != 0 || tp.shadow_offset_.y != 0) {
			vtx::ipos p = pos + tp.shadow_offset_;
			fonts.set_fore_color(tp.shadow_color_);
			fonts.draw(p, ls, clx);
		}

		fonts.set_fore_color(tp.fore_color_);
		fonts.draw(pos, ls, clx, tp.cursor_);

		if(!tp.font_.empty()) {
			fonts.pop_font_face();
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキスト・レンダリング
		@param[in]	wd	widget_director
		@param[in]	oh	object ハンドル
		@param[in]	wp	widget パラメーター
		@param[in]	tp	テキスト・パラメーター
		@param[in]	pp	プレート・パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void render_text(widget_director& wd, gl::mobj::handle oh, const widget::param& wp, const widget::text_param& tp, const widget::plate_param& pp)
	{
		gl::core& core = gl::core::get_instance();

		const vtx::ipos& vsz = core.get_size();

		gl::fonts& fonts = core.at_fonts();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {
			glPushMatrix();
			vtx::irect rect;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				draw_mobj(wd, oh, wp.clip_);
				rect.org  = wp.rpos_;
				rect.size = wp.rect_.size;
			} else {
				wd.at_mobj().draw(oh, gl::mobj::attribute::normal, vtx::spos(0));
				rect.org.set(0);
				rect.size = wp.rect_.size;
			}

			{
				widget::text_param tmp = tp;
				const img::rgbaf& cf = wd.get_color();
				tmp.fore_color_ *= cf.r;
				tmp.fore_color_.alpha_scale(cf.a);
				tmp.shadow_color_ *= cf.r;
				tmp.shadow_color_.alpha_scale(cf.a);

				vtx::irect clip = wp.clip_;
				rect.size.x -= pp.frame_width_ * 2;
				rect.size.y -= pp.frame_width_ * 2;
				clip.org.x += pp.frame_width_;
				clip.org.y += pp.frame_width_;
				clip.size.x -= pp.frame_width_ * 2;
				clip.size.y -= pp.frame_width_ * 2;
				draw_text(tmp, rect, clip);
			}

			fonts.restore_matrix();

			glPopMatrix();
			glViewport(0, 0, vsz.x, vsz.y);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フレームの初期化
		@param[in]	wd	widget_director
		@param[in]	wp	widget::param
		@param[in]	pp	widget::plate_param
		@param[in]	cp	widget::color_param
		@return モーション・オブジェクトのハンドルを返す
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static gl::mobj::handle frame_init(widget_director& wd, widget::param& wp, const widget::plate_param& pp, const widget::color_param& cp)
	{
		vtx::ipos size;
		if(pp.resizeble_) {
//			vtx::spos rsz = pp.grid_ * 3;
//			if(wp.rect_.size.x >= rsz.x) size.x = rsz.x;
//			else size.x = wp.rect_.size.x;
//			if(wp.rect_.size.y >= rsz.y) size.y = rsz.y;
//			else size.y = wp.rect_.size.y;
			size = pp.grid_ * 3;
		} else {
			size = wp.rect_.size;
		}

		share_t t;
		t.size_ = size;
		t.color_param_ = cp;
		t.plate_param_ = pp;
		return wd.share_add(t);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	クリッピングされたモーションオブジェクトのレンダリング
		@param[in]	mo	モーションオブジェクト
		@param[in]	moh	モーションオブジェクトハンドル
		@param[in]	clip	clip パラメーター
		@param[in]	ofs	描画オフセット
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void render_clipped_mobj(gl::mobj& mo, gl::mobj::handle moh, const vtx::irect& clip, const vtx::ipos& ofs)
	{
		gl::core& core = gl::core::get_instance();

		const vtx::ipos& vsz = core.get_size();
		const vtx::ipos& siz = core.get_rect().size;

		if(clip.size.x > 0 && clip.size.y > 0) {
			glPushMatrix();
			int sx = vsz.x / siz.x;
			int sy = vsz.y / siz.y;
			glViewport(clip.org.x * sx, vsz.y - clip.org.y * sy - clip.size.y * sy,
				clip.size.x * sx, clip.size.y * sy);
			mo.setup_matrix(clip.size.x, clip.size.y);
			mo.draw(moh, gl::mobj::attribute::normal, ofs);
			glPopMatrix();
			glViewport(0, 0, vsz.x, vsz.y);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シフト・テキスト更新
		@param[in]	bp	widget ベースパラメーター
		@param[in]	tp	text パラメーター
		@param[in]	sp	shift パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void shift_text_update(const widget::param& bp, widget::text_param& tp, widget::shift_param& sp)
	{
		gl::core& core = gl::core::get_instance();
		gl::fonts& fonts = core.at_fonts();

		if(sp.every_ ||
		  (sp.enable_ && bp.hold_frame_ >= sp.hold_frame_)) {
			if(!tp.font_.empty()) {
				fonts.push_font_face();
				fonts.set_font_type(tp.font_);
			}
			fonts.enable_proportional(tp.proportional_);
			short fw = fonts.get_width(tp.text_);
			if(!tp.font_.empty()) {
				fonts.pop_font_face();
			}
			if(sp.size_ < fw) {
				if(sp.offset_ == 0 && sp.org_wait_count_) {
					--sp.org_wait_count_;
				} else {
					sp.offset_ -= sp.speed_;
					if(sp.offset_ == 0 && sp.org_wait_frame_) {
						sp.org_wait_count_ = sp.org_wait_frame_;
					}
					if((static_cast<short>(sp.offset_) + fw) <= 0) {
						sp.offset_ = sp.size_;
					}
				}
				tp.offset_.x = sp.offset_;
			} else {
				tp.offset_.x = 0;
				sp.offset_ = 0.0f;
			}
		} else {
			tp.offset_.x = 0;
			sp.offset_ = 0.0f;
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シフト・テキスト・レンダー
		@param[in]	wd		Widget ディレクター
		@param[in]	bp	widget ベースパラメーター
		@param[in]	tp	text パラメーター
		@param[in]	pp	plate パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void shift_text_render(widget_director& wd, const widget::param& bp, const widget::text_param& tp, const widget::plate_param& pp)
	{
		if(pp.caption_width_ <= 0) return;
		if(tp.text_.empty()) return;

		gl::core& core = gl::core::get_instance();
		const vtx::ipos& vsz = core.get_size();

		glPushMatrix();
		vtx::irect rect;
		short fw = pp.frame_width_;
		rect.org.set(0);
		rect.size.set(bp.rect_.size.x - fw * 2, pp.caption_width_);

		vtx::irect clip = bp.clip_;
		clip.org.x += fw;
		clip.org.y += fw;
		clip.size.x -= fw * 2;
		clip.size.y  = pp.caption_width_;

		widget::text_param tmp = tp;
		const img::rgbaf& cf = wd.get_color();
		tmp.fore_color_ *= cf.r;
		tmp.fore_color_.alpha_scale(cf.a);
		tmp.shadow_color_ *= cf.r;
		tmp.shadow_color_.alpha_scale(cf.a);
		draw_text(tmp, rect, clip);

		core.at_fonts().restore_matrix();

		glPopMatrix();
		glViewport(0, 0, vsz.x, vsz.y);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ボーダーの描画
		@param[in]	rect   	描画位置とサイズ
		@param[in]	color	描画カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void draw_border(const vtx::srect& rect, const widget::color_param& color = widget_director::default_border_color_)
	{
		vtx::srect r = rect;
		vtx::spos o(0);
		if(r.size.x >= r.size.y) {  // Holizontal
			r.size.y /= 2;
			o.y = r.size.y;
		} else {  // Vertical
			r.size.x /= 2;
			o.x = r.size.x;
		}

		gl::glColor(color.fore_color_);
		gl::draw_filled_rectangle(r);
		r.org += o;
		gl::glColor(color.back_color_);
		gl::draw_filled_rectangle(r);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	プラットフォーム依存、ファイル選択
		@return 正常選択された場合「true」
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#if 0
#ifdef WIN32
	static bool get_open_file_name()
	{
		static bool init = false;
		if(!init) {
//			CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			init = true;
		}

		OPENFILENAME ofn;
		char szFile[MAX_PATH] = "";
		std::memset(&ofn, sizeof(ofn), 0);
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = "テキストファイル(*.txt)\0*.txt\0"
						  "すべてのファイル(*.*)\0*.*\0\0";
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
 
		if(GetOpenFileName(&ofn) != 0) {
//			path = szFile;
			return true;	
		} else {
//			path = "";
			return false;
		}
	}
#endif
#endif
}
