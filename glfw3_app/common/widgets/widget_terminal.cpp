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
		param_.terminal_.output(wch);
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
					param_.terminal_.resize(
						vtx::spos(sr.size.x / param_.font_size_ * 2, sr.size.y / param_.height_)
					);
				}
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
		IGLcore* igl = get_glcore();

		const vtx::spos& size = igl->get_size();

		gl::fonts& fonts = igl->at_fonts();

		const widget::param& wp = get_param();

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

			std::string cft = fonts.get_font_type();
			short cfs = fonts.get_font_size(); 
			fonts.set_font_type(param_.font_);
			fonts.set_font_size(param_.font_size_);

			vtx::srect clip_ = wp.clip_;

			glViewport(clip_.org.x, size.y - clip_.org.y - clip_.size.y,
				clip_.size.x, clip_.size.y);
			fonts.setup_matrix(clip_.size.x, clip_.size.y);

			fonts.set_proportional(false);
			fonts.enable_back_color();

			vtx::spos pos;
			vtx::spos chs(rect.org);
			for(pos.y = 0; pos.y < param_.terminal_.size().y; ++pos.y) {
				for(pos.x = 0; pos.x < param_.terminal_.size().x; ++pos.x) {
					const utils::terminal::cha_t& t = param_.terminal_.get_char(pos);
					fonts.set_fore_color(t.fc);
					fonts.set_back_color(t.bc);
					chs.x += fonts.draw(chs, t.cha);
				}
				chs.y += param_.height_;
				chs.x = rect.org.x;
			}

			fonts.set_font_type(cft);
			fonts.set_font_size(cfs);
			fonts.enable_back_color(false);

			fonts.restore_matrix();
			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::service()
	{
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
