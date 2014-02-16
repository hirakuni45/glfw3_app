//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"
#include <boost/foreach.hpp>

namespace gui {

	void widget_terminal::rebuild_texts_()
	{
		uint32_t n = get_rect().size.y / param_.height_;
		if(texts_.size() == n) return;

		short ofs = 0;
		if(texts_.size() > n) {
			for(uint32_t i = texts_.size(); i < n; ++i) {
				wd_.del_widget(texts_[i]);
			}
			texts_.resize(n);
			return;
		} else if(texts_.size() < n) {
			n -= texts_.size();
			ofs = texts_.size() * param_.height_;
		}

		vtx::srect sr(0, ofs, get_rect().size.x, param_.height_);
		for(uint32_t i = 0; i < n; ++i) {
			widget::param wp(sr, this);
			widget_text::param wp_;
			wp_.text_param_ = param_.text_param_;
			wp_.text_param_.text_ = "AbcdefiWw 漢字";
			widget_text* w = wd_.add_widget<widget_text>(wp, wp_);
			if(w) {
				w->set_state(widget::state::CLIP_PARENTS);
				w->set_state(widget::state::RESIZE_ROOT);
				w->set_state(widget::state::MOVE_ROOT, false);
				w->set_state(widget::state::POSITION_LOCK);
				w->set_state(widget::state::SIZE_LOCK);
				texts_.push_back(w);
				sr.org.y += param_.height_;
			}
		}
	}


	void widget_terminal::scroll_()
	{
		uint32_t n = get_rect().size.y / param_.height_;
		for(uint32_t i = 1; i < n; ++i) {
			texts_[i - 1]->at_local_param().text_param_.text_ = texts_[i]->get_local_param().text_param_.text_;
		}
		texts_[n - 1]->at_local_param().text_param_.text_.clear();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１文字出力
		@param[in]	wch	文字
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::output(wchar_t wch)
	{
		// param_.cursor_pos_.x;

		// scroll_();
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
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::service()
	{
		rebuild_texts_();
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
