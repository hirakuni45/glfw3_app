//=====================================================================//
/*!	@file
	@brief	GUI widget_image クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_utils.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_image::initialize()
	{
		// 標準的に固定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().state_.set(widget::state::MOVE_ROOT);

		using namespace img;

		if(param_.image_) {
			paint pa;
			const vtx::spos& size = get_rect().size;

			vtx::spos s;
			vtx::spos o;
			if(size.x <= 0) {
				o.x = 0;
				s.x = param_.image_->get_size().x;
			} else {
				o.x = (size.x - param_.image_->get_size().x) / 2;
				s.x = size.x;
			}
			if(size.y <= 0) {
				o.y = 0;
				s.y = param_.image_->get_size().y;
			} else {
				o.y = (size.y - param_.image_->get_size().y) / 2;
				s.y = size.y;
			}
			at_rect().size = s;
			pa.create(s, true);
			pa.fill(img::rgba8(0, 0));

			img::copy_to_rgba8(param_.image_, 0, 0, s.x, s.y, pa, o.x, o.y);

			objh_ = wd_.at_mobj().install(&pa);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_image::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_image::render()
	{
		if(objh_ == 0) return;

		using namespace gl;
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		const vtx::spos& size = igl->get_size();

		const widget::param& wp = get_param();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 
			glPushMatrix();
			vtx::srect rect;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				draw_mobj(wd_, objh_, wp.clip_);
				rect.org  = wp.rpos_;
				rect.size = wp.rect_.size;
			} else {
				wd_.at_mobj().draw(objh_, gl::mobj::normal, 0, 0);
				rect.org.set(0);
				rect.size = wp.rect_.size;
			}
			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}
}
