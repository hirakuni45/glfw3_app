//=====================================================================//
/*!	@file
	@brief	GUI widget_label クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_label.hpp"
#include "widgets/widget_utils.hpp"
#include "gl_fw/IGLcore.hpp"
#include "img_io/paint.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_label::initialize()
	{
		// 標準的に固定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);

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

		t.color_param_ = param_.color_param_select_;
		select_objh_ = wd_.share_add(t);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_label::update()
	{
		using namespace gl;
		IGLcore* Igl = get_glcore();
		if(Igl == 0) return;

		const vtx::spos& size = Igl->get_size();

		glfonts& fonts = Igl->at_fonts();

		const widget::param& bp = get_param();
		if(param_.shift_every_ ||
		  (param_.shift_enable_ && bp.hold_frame_ >= param_.shift_hold_frame_)) {
			std::string cft;
			if(!param_.text_param_.font_.empty()) {
				cft = fonts.get_font_type();
				fonts.set_font_type(param_.text_param_.font_);
			}
			fonts.set_proportional(param_.text_param_.proportional_);
			short fw = fonts.get_width(param_.text_param_.text_);
			if(!cft.empty()) {
				fonts.set_font_type(cft);
			}
			const vtx::spos& size = bp.rect_.size;
			if(size.x < fw) {
				param_.shift_offset_ -= param_.shift_speed_;
				if((static_cast<short>(param_.shift_offset_) + fw) <= 0) {
					param_.shift_offset_ = size.x;
				}
				param_.text_param_.offset_.x = param_.shift_offset_;
			} else {
				param_.text_param_.offset_.x = 0;
				param_.shift_offset_ = 0.0f;
			}
		} else {
			param_.text_param_.offset_.x = 0;
			param_.shift_offset_ = 0.0f;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_label::render()
	{
		gl::glmobj::handle h = objh_;
		if(get_select() || get_state(widget::state::SYSTEM_SELECT)) {
			h = select_objh_;
		}

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(h, get_param().rect_.size);
		}

		render_text(wd_, h, get_param(), param_.text_param_, param_.plate_param_);
	}
}
