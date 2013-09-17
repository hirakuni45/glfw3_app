//=====================================================================//
/*!	@file
	@brief	GUI widget_image クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_text::initialize()
	{
		// 標準的に固定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_text::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_text::render()
	{
		using namespace gl;
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		const vtx::spos& size = igl->get_size();
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

			draw_text(param_.text_param_, rect, wp.clip_);

			igl->at_fonts().restore_matrix();

			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_text::destroy()
	{
	}

}
