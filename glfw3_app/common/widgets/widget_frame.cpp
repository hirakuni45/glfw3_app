//=====================================================================//
/*!	@file
	@brief	GUI widget_frame クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_frame::initialize()
	{
		// 標準的設定（自由な大きさの変更）
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);
		at_param().state_.set(widget::state::RESIZE_EDGE_ENABLE);
		at_param().state_.set(widget::state::MOVE_TOP);
		at_param().state_.set(widget::state::RESIZE_TOP);

		param_.plate_param_.resizeble_ = true;
		param_.text_param_.shadow_offset_.set(0);	// 通常「影」は付けない。
		param_.text_param_.fore_color_.set(250, 250, 250);
		at_param().resize_min_ = param_.plate_param_.grid_ * 3;

		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_frame::update()
	{
		param_.shift_param_.size_ = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
		shift_text_update(get_param(), param_.text_param_, param_.shift_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_frame::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));

		shift_text_render(get_param(), param_.text_param_, param_.plate_param_);
	}
}
