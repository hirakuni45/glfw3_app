//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_terminal.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_frame.hpp"
#include <boost/foreach.hpp>

namespace gui {


	//-----------------------------------------------------------------//
	/*!
		@brief	１文字出力
		@param[in]	wch	文字
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::output(uint32_t wch)
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

		auto ls = utils::utf8_to_utf32(text);

		BOOST_FOREACH(uint32_t ch, ls) {
			output(ch);
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
		core& core = core::get_instance();
		fonts& fonts = core.at_fonts();

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
		if(wd_.get_focus_widget() == this || wd_.get_focus_widget() == wd_.root_widget(this)) {
			focus_ = true;
		} else {
			focus_ = false;
		}

		if(get_param().parents_ && get_state(widget::state::AREA_ROOT)) {
			if(get_param().parents_->type() == get_type_id<widget_frame>()) {
				// 親になってるフレームを取得
				widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
				if(w) {
					bool resize = false;
					vtx::irect sr;
					w->get_draw_area(sr);
					if(param_.auto_fit_) {
						vtx::ipos ss(sr.size.x / param_.font_width_,
									 sr.size.y / param_.height_);
						ss.x *= param_.font_width_;
//						if(ss.x < w->get_param().resize_min_.x) {
//							ss.x = w->get_param().resize_min_.x / param_.font_width_;
//							ss.x *= param_.font_width_;
//						}
						ss.y *= param_.height_;
//						if(ss.y < w->get_param().resize_min_.y) {
//							ss.y = w->get_param().resize_min_.y / param_.height_;
//							ss.y *= param_.height_;
//						}
						w->set_draw_area(ss);
						w->get_draw_area(sr);
					}
					if(sr.size != get_rect().size) resize = true;
					at_rect() = sr;
					if(resize) {

					}
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
			if(param_.echo_) {
				terminal_.output(wd_.at_keyboard().input());
			}
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
		core& core = core::get_instance();
		const vtx::spos& vsz = core.get_size();
		const vtx::spos& siz = core.get_rect().size;
		gl::fonts& fonts = core.at_fonts();

		const widget::param& wp = get_param();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {

			glPushMatrix();

			vtx::irect rect;
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

			vtx::irect clip_ = wp.clip_;

			int sx = vsz.x / siz.x;
			int sy = vsz.y / siz.y;
			glViewport(clip_.org.x * sx, vsz.y - clip_.org.y * sy - clip_.size.y * sy,
				clip_.size.x * sx, clip_.size.y * sy);
			fonts.setup_matrix(clip_.size.x, clip_.size.y);

			fonts.enable_center(false);
			fonts.enable_proportional(false);

//			tpr.shadow_color_ *= cf.r;
//			tpr.shadow_color_.alpha_scale(cf.a);
			const img::rgbaf& cf = wd_.get_color();
			vtx::ipos limit(clip_.size.x / param_.font_width_, clip_.size.y / param_.height_);
			vtx::ipos chs(rect.org);
			vtx::ipos pos;
			for(pos.y = 0; pos.y < limit.y; ++pos.y) {
				for(pos.x = 0; pos.x < limit.x; ++pos.x) {
					const auto& t = terminal_.get_char(pos);
					img::rgba8 fc = t.fc_;
					fc *= cf.r;
					fc.alpha_scale(cf.a);
					fonts.set_fore_color(fc);
					img::rgba8 bc = t.bc_;
					bc *= cf.r;
					bc.alpha_scale(cf.a);
					fonts.set_back_color(bc);
					if(focus_ && pos == terminal_.get_cursor()) {
						if((interval_ % 40) < 20) {
							fonts.swap_color();
						}
					}
					auto cha = t.cha_;
					if(cha < 0x20) cha = 0x3F;  // 制御コードは DEL-char として扱う
					if(cha > 0x7f) {
						fonts.pop_font_face();
					}
					int fw = fonts.get_width(cha);
					vtx::irect br(chs, vtx::ipos(fw, param_.height_));
					fonts.draw_back(br);
					chs.x += fonts.draw(chs, cha);
					fonts.swap_color(false);
					if(cha > 0x7f) {
						fonts.push_font_face();
						fonts.set_font_type(param_.font_);
					}
				}
				chs.y += param_.height_;
				chs.x = rect.org.x;
			}
			++interval_;

			fonts.restore_matrix();
			fonts.pop_font_face();
			glPopMatrix();
			glViewport(0, 0, vsz.x, vsz.y);
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
