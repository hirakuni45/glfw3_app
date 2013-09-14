//=====================================================================//
/*!	@file
	@brief	GUI Widget リスト
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_null.hpp"
#include <boost/foreach.hpp>

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_list::initialize()
	{
		// 標準的に固定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().action_.set(widget::action::SELECT_HIGHLIGHT);

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
		root_h_ = wd_.share_add(t);

		t.size_ = size;
		t.plate_param_.round_radius_ = 0;
		t.plate_param_.frame_width_ = 0;
		list_h_ = wd_.share_add(t);

		vtx::spos frsz;
		frsz.x = t.size_.x;
		frsz.y = t.size_.y * param_.text_list_.size();
		widget::param wp(vtx::srect(vtx::spos(0), frsz), this);
		widget_null::param wp_;
		frame_ = wd_.add_widget<widget_null>(wp, wp_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_list::update()
	{
		if(get_selected()) {
			param_.open_ = !param_.open_;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_list::render()
	{
		using namespace gl;

		IGLcore* igl = get_glcore();

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(root_h_, get_param().rect_.size);
		}

		render_text(wd_, root_h_,
			get_param(), param_.text_param_, param_.plate_param_);

		const vtx::spos& size = igl->get_size();
		wd_.at_mobj().setup_matrix(size.x, size.y);
		glTranslatef(get_param().rpos_.x, get_param().rpos_.y, 1.0f);

		const vtx::spos& bsz = wd_.at_mobj().get_size(wd_.get_share_image().down_box_);
		vtx::spos ofs;
		ofs.x = get_rect().size.x - bsz.x - param_.plate_param_.frame_width_ - 4;
		ofs.y = (get_rect().size.y - bsz.y) / 2;
		wd_.at_mobj().draw(wd_.get_share_image().down_box_, gl::glmobj::normal,
			ofs.x, ofs.y);

		if(!param_.open_) return;

		vtx::spos pos(0);
		const vtx::spos& sz = wd_.at_mobj().get_size(list_h_);
		for(uint32_t i = 0; i < param_.text_list_.size(); ++i) {
			glPushMatrix();
			wd_.at_mobj().draw(list_h_, gl::glmobj::normal, pos.x, pos.y);
			glPopMatrix();
			pos.y += sz.y;
		}

//		BOOST_FOREACH(

//		render_text(wd_, list_h_,
//			get_param(), param_.text_param_, param_.plate_param_);

	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_list::destroy()
	{
	}

}
