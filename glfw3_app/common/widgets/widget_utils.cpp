//=====================================================================//
/*!	@file
	@brief	GUI widget ユーティリティー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "utils/string_utils.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_utils.hpp"
#include "widget_utils.hpp"
#include "widget_button.hpp"
#include "widget_label.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像を作成
		@param[in]	image	画像
		@param[in]	size	サイズ
		@param[out]	pa		ペイントクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void create_image_base(const img::i_img* image, const vtx::spos& size, img::paint& pa)
	{
		vtx::spos s;
		vtx::spos o;
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
		img::copy_to_rgba8(image, 0, 0, s.x, s.y, pa, o.x, o.y);
	}


	static void mix_round_(img::paint& pa, const widget::color_param& cp, const widget::plate_param& pp)
	{
		if(pp.round_style_ == widget::plate_param::round_style::ALL) {
			return;
		} else {
			const vtx::spos& size = pa.get_size();
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
				npa.fill_rect(wf, wf, size.x - 2 * wf, size.y - 2 * wf, cp.ir_enable_);
			} else {
				npa.set_intensity_rect(cp.inten_rect_);
				npa.set_round(0);
				npa.swap_color();
				npa.fill_rect(cp.ir_enable_);
			}

			if(pp.round_style_ == widget::plate_param::round_style::TOP) {
				pa.copy(0, size.y - pp.round_radius_, npa,
					0, size.y - pp.round_radius_, size.x, pp.round_radius_);
			} else if(pp.round_style_ == widget::plate_param::round_style::BOTTOM) {
				pa.copy(0, 0, npa,
					0, 0, size.x, pp.round_radius_);
			}
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
	void create_round_frame(img::paint& pa, const widget::plate_param& pp,
		const widget::color_param& cp, const vtx::spos& size)
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
			if(pp.round_radius_ > wf) rd = pp.round_radius_ - wf;
			else rd = 0;
			pa.set_round(rd);
			pa.swap_color();
			pa.alpha_blend();
			pa.fill_rect(wf, wf + pp.caption_width_, size.x - 2 * wf, size.y - 2 * wf - pp.caption_width_, cp.ir_enable_);
			mix_round_(pa, cp, pp);
		} else {
			pa.set_intensity_rect(cp.inten_rect_);
			pa.set_round(pp.round_radius_);
			pa.swap_color();
			pa.fill_rect(cp.ir_enable_);
			mix_round_(pa, cp, pp);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	親子関係を継承した位置を合算する
		@param[in]	root	起点になるウィジェット
		@param[out]	pos		位置を受け取る参照
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void final_position(const widget* root, vtx::spos& pos)
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
	bool final_clip(widget* root, vtx::spos& org, vtx::srect& rect)
	{
		widgets ws;
		vtx::srect sr(vtx::spos(0), root->get_rect().size);
		while(root != 0) {
			sr.org += root->get_rect().org;
			ws.push_back(root);
			root = root->get_param().parents_;
		}

		widgets_cit cit = ws.end();
		vtx::spos psum(0);
		vtx::spos top = sr.org;
		while(cit != ws.begin()) {
			--cit;
			vtx::srect r = (*cit)->get_rect();
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
	void draw_mobj(widget_director& wd, gl::mobj::handle h, const vtx::srect& clip, const vtx::spos& ofs)
	{
		using namespace gl;
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		const vtx::spos& size = igl->get_size();

		glViewport(clip.org.x, size.y - clip.org.y - clip.size.y, clip.size.x, clip.size.y);
		wd.at_mobj().setup_matrix(clip.size.x, clip.size.y);

		wd.at_mobj().draw(h, gl::mobj::attribute::normal, ofs.x, ofs.y);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フォントの描画
		@param[in]	tp	テキスト・パラメーター
		@param[in]	rect   	描画位置
		@param[in]	clip	描画エリア
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void draw_text(const widget::text_param& tp, const vtx::srect& rect, const vtx::srect& clip)
	{
		if(tp.text_.empty()) return;

		gl::IGLcore* igl = gl::get_glcore();
		if(igl == 0) return;

		const vtx::spos& size = igl->get_size();

		gl::fonts& fonts = igl->at_fonts();

		vtx::srect clip_ = clip;
		vtx::srect rect_ = rect;

		std::string cft;
		short cfs = 0;
		if(!tp.font_.empty()) {
			cft = fonts.get_font_type();
			fonts.set_font_type(tp.font_);
			cfs = fonts.get_font_size();
			fonts.set_font_size(tp.font_size_);
		}

		glViewport(clip_.org.x, size.y - clip_.org.y - clip_.size.y,
			clip_.size.x, clip_.size.y);
		fonts.setup_matrix(clip_.size.x, clip_.size.y);

		fonts.set_proportional(tp.proportional_);
		vtx::spos pos;
		const vtx::spos& fsize = fonts.get_size(tp.text_);
		vtx::placement tpl = tp.placement_;
		if(fsize.x > clip_.size.x) {
			tpl.hpt = vtx::placement::holizontal::LEFT;
		}
		vtx::create_placement(rect_, fsize, tpl, pos);

		short clx = 0;
		if(utils::string_strchr(tp.text_, '\n')) {
			clx = clip_.size.x;
		}

		pos += tp.offset_;
		if(tp.shadow_offset_.x != 0 || tp.shadow_offset_.y != 0) {
			vtx::spos p = pos + tp.shadow_offset_;
			fonts.set_fore_color(tp.shadow_color_);
			fonts.draw(p, tp.text_, clx);
		}

		fonts.set_fore_color(tp.fore_color_);
		fonts.draw(pos, tp.text_, clx);

		if(!tp.font_.empty()) {
			fonts.set_font_type(cft);
			fonts.set_font_size(cfs);
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
	void render_text(widget_director& wd, gl::mobj::handle oh, const widget::param& wp, const widget::text_param& tp, const widget::plate_param& pp)
	{
		gl::IGLcore* igl = gl::get_glcore();

		const vtx::spos& size = igl->get_size();

		gl::fonts& fonts = igl->at_fonts();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 
			glPushMatrix();
			vtx::srect rect;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				draw_mobj(wd, oh, wp.clip_);
				rect.org  = wp.rpos_;
				rect.size = wp.rect_.size;
			} else {
				wd.at_mobj().draw(oh, gl::mobj::attribute::normal, 0, 0);
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

				vtx::srect clip = wp.clip_;
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
			glViewport(0, 0, size.x, size.y);
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
	gl::mobj::handle frame_init(widget_director& wd, widget::param& wp, const widget::plate_param& pp, const widget::color_param& cp)
	{
		vtx::spos size;
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
	void render_clipped_mobj(gl::mobj& mo, gl::mobj::handle moh, const vtx::srect& clip,
		const vtx::spos& ofs)
	{
		using namespace gl;
		IGLcore* igl = get_glcore();

		const vtx::spos& size = igl->get_size();

		if(clip.size.x > 0 && clip.size.y > 0) {
			glPushMatrix();
			glViewport(clip.org.x, size.y - clip.org.y - clip.size.y,
				clip.size.x, clip.size.y);
			mo.setup_matrix(clip.size.x, clip.size.y);
			mo.draw(moh, gl::mobj::attribute::normal, ofs.x, ofs.y);
			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}
}
