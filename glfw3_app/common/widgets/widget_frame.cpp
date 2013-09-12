//=====================================================================//
/*!	@file
	@brief	GUI widget_frame クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widget_frame.hpp"
#include "widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_frame::initialize()
	{
		param_.color_param_ = wd_.get_default_frame_color();

		// 自由な大きさの変更
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);

		param_.plate_param_.resizeble_ = true;

		vtx::spos size;
		if(param_.plate_param_.resizeble_) {
			vtx::spos rsz = param_.plate_param_.grid_ * 3;
			if(get_param().rect_.size.x >= rsz.x) size.x = rsz.x;
			else size.x = get_param().rect_.size.x;
			if(get_param().rect_.size.y >= rsz.y) size.y = rsz.y;
			else size.y = get_param().rect_.size.y;
		} else {
			size = get_param().rect_.size;
		}

		share_t t;
		t.size_ = size;
		t.color_param_ = param_.color_param_;
		t.plate_param_ = param_.plate_param_;
		objh_ = wd_.share_add(t);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_frame::update()
	{
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
		wd_.at_mobj().draw(objh_, gl::glmobj::normal, 0, 0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_frame::destroy()
	{
	}

}
