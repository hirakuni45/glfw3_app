//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_terminal.hpp"
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_frame.hpp"
#include <boost/foreach.hpp>

namespace gui {


	//-----------------------------------------------------------------//
	/*!
		@brief	１文字出力
		@param[in]	wch	文字
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::output(wchar_t wch)
	{
		terminal_.output(wch);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テキストの出力
		@param[in]	text	テキスト
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::output(const std::string& text)
	{
		if(text.empty()) return;

		utils::wstring ws;
		utils::utf8_to_utf16(text, ws);

		BOOST_FOREACH(wchar_t wch, ws) {
			output(wch);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::initialize()
	{
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE, false);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE, false);
		at_param().state_.set(widget::state::SERVICE);
		at_param().state_.set(widget::state::MOVE_ROOT, false);
		at_param().state_.set(widget::state::RESIZE_ROOT);
		at_param().state_.set(widget::state::CLIP_PARENTS);
		at_param().state_.set(widget::state::AREA_ROOT);

		using namespace gl;
		IGLcore* igl = get_glcore();
		gl::fonts& fonts = igl->at_fonts();

		fonts.push_font_face();
		fonts.set_font_type(param_.font_);
		fonts.set_font_size(param_.font_height_);
		fonts.enable_proportional(false);
		fonts.set_spaceing(0);
		param_.font_width_ = fonts.get_width(' ');
		fonts.pop_font_face();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::update()
	{
		bool resize = false;
		if(get_param().parents_ && get_state(widget::state::AREA_ROOT)) {
			if(get_param().parents_->type() == get_type_id<widget_frame>()) {
				widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
				vtx::srect sr;
				w->create_draw_area(sr);
				if(sr.size != get_rect().size) resize = true;
				at_rect() = sr;
				if(resize) {
					terminal_.resize(
						vtx::spos(sr.size.x / param_.font_width_, sr.size.y / param_.height_)
					);
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::service()
	{
		if(focus_) {
			terminal_.service();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::render()
	{
		using namespace gl;
		IGLcore* igl = get_glcore();

		const vtx::spos& size = igl->get_size();

		gl::fonts& fonts = igl->at_fonts();

		const widget::param& wp = get_param();

		if(wd_.get_top_widget() == this || wd_.get_top_widget() == wd_.root_widget(this)) {
			focus_ = true;
		} else {
			focus_ = false;
		}

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 

			glPushMatrix();

			vtx::srect rect;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				rect.org  = wp.rpos_;
				rect.size = wp.rect_.size;
			} else {
				rect.org.set(0);
				rect.size = wp.rect_.size;
			}

			fonts.push_font_face();
			fonts.set_font_type(param_.font_);
			fonts.set_font_size(param_.font_height_);

			vtx::srect clip_ = wp.clip_;

			glViewport(clip_.org.x, size.y - clip_.org.y - clip_.size.y,
				clip_.size.x, clip_.size.y);
			fonts.setup_matrix(clip_.size.x, clip_.size.y);

			fonts.enable_center(false);
			fonts.enable_proportional(false);

//			tpr.shadow_color_ *= cf.r;
//			tpr.shadow_color_.alpha_scale(cf.a);

			const img::rgbaf& cf = wd_.get_color();
			vtx::spos pos;
			vtx::spos chs(rect.org);
			for(pos.y = 0; pos.y < terminal_.size().y; ++pos.y) {
				for(pos.x = 0; pos.x < terminal_.size().x; ++pos.x) {
					const utils::terminal::cha_t& t = terminal_.get_char(pos);
					img::rgba8 fc  = t.fc;
					fc *= cf.r;
					fc.alpha_scale(cf.a);
					fonts.set_fore_color(fc);
					img::rgba8 bc = t.bc;
					bc *= cf.r;
					bc.alpha_scale(cf.a);
					fonts.set_back_color(bc);
					if(focus_ && pos == terminal_.cursor()) {
						if((interval_ % 40) < 20) {
							fonts.swap_color();
						}
					}
					vtx::srect br(chs, vtx::spos(param_.font_width_, param_.height_));
					fonts.draw_back(br);
					chs.x += fonts.draw(chs, t.cha);
					fonts.swap_color(false);
				}
				chs.y += param_.height_;
				chs.x = rect.org.x;
			}
			++interval_;

			fonts.restore_matrix();
			fonts.pop_font_face();
			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_terminal::save(sys::preference& pre)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_terminal::load(const sys::preference& pre)
	{
		return false;
	}
}
