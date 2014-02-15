//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	void widget_terminal::create_text_()
	{
		vtx::srect sr(0, 0, get_rect().size.x, param_.height_);
		for(uint32_t i = 0; i < param_.lines_; ++i) {
			widget::param wp(sr, this);
			widget_text::param wp_;
			wp_.text_param_ = param_.text_param_;
			wp_.text_param_.text_ = "Abcdef";
			texts_.push_back(wd_.add_widget<widget_text>(wp, wp_));
			sr.org.y += param_.height_;
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

		uint32_t n = get_rect().size.y / param_.height_;
		if(n > param_.lines_) param_.lines_ = n;

		create_text_();
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
