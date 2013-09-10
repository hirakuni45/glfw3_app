//=====================================================================//
/*!	@file
	@brief	widget zero クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widget_zero.hpp"
#include "img_io/paint.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_zero::initialize()
	{
		param_.color_param_ = director_.get_default_frame_color();

		// 標準的に位置固定
		at_param().state_.set(widget::state::POSITION_LOCK);

		using namespace img;

		img::paint pa;

		const vtx::spos& size = get_param().rect_.size;

		pa.create(size, true);

		pa.set_fore_color(param_.color_param_.fore_color_);
		pa.set_round(param_.round_radius_);
		pa.fill_rect(true);

		int wf = param_.frame_width_;
		pa.set_round(param_.round_radius_ - wf);
		pa.set_fore_color(param_.color_param_.back_color_);
		pa.fill_rect(wf, wf, size.x - 2 * wf, size.y - 2 * wf);

		pa.set_round(0);
		pa.set_fore_color(param_.color_param_.fore_color_);
		int hs = wf + param_.space_;
		pa.fill_rect(hs, size.y / 2 - param_.bar_width_ / 2, size.x - hs * 2, param_.bar_width_);

		hobj_minus_ = director_.at_mobj().install(&pa);

		pa.fill_rect(size.x / 2 - param_.bar_width_ / 2, hs, param_.bar_width_, size.y - hs * 2);

		hobj_cross_ = director_.at_mobj().install(&pa);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_zero::update()
	{
		if(hobj_cross_ == 0 || hobj_minus_ == 0) return;

//		unsigned int count = get_count();
//		if(count_ != count) {
//			param_.cross_ = !param_.cross_;
//		}
//		count_ = count;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_zero::render()
	{
		if(hobj_cross_ == 0 || hobj_minus_ == 0) return;

		if(param_.cross_) {
			director_.at_mobj().draw(hobj_cross_, gl::glmobj::normal, 0, 0);
		} else {
			director_.at_mobj().draw(hobj_minus_, gl::glmobj::normal, 0, 0);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_zero::destroy()
	{
	}

}
