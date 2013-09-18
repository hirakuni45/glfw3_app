//=====================================================================//
/*!	@file
	@brief	GUI Widget リスト
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_list.hpp"
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

		widget::param wp = get_param();
		widget_label::param wp_;
		wp_.plate_param_ = param_.plate_param_;
		wp_.color_param_ = param_.color_param_;
		wp_.text_param_ = param_.text_param_;
		wp_.color_param_select_ = param_.color_param_select_;
		root_ = wd_.add_widget<widget_label>(wp, wp_);

		{
			wp.rect_.size.y *= param_.text_list_.size();
			widget_null::param wp_;
			frame_ = wd_.add_widget<widget_null>(wp, wp_);
			frame_->set_state(widget::state::POSITION_LOCK);
		}

		wp.parents_ = frame_;
		wp.rect_.org.set(0);
		wp.rect_.size.y = root_->get_rect().size.y;
		wp_.plate_param_.frame_width_ = 0;
		int n = 0;
		BOOST_FOREACH(const std::string& s, param_.text_list_) {
			wp_.text_param_.text_ = s;
			if(n == 0) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_ = widget::plate_param::round_style::TOP;
			} else if(n == (param_.text_list_.size() - 1)) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_ = widget::plate_param::round_style::BOTTOM;
			} else {
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.round_style_ = widget::plate_param::round_style::ALL;
			}
			widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
			w->set_state(widget::state::ENABLE, false);
			list_.push_back(w);
			wp.rect_.org.y += root_->get_rect().size.y;
			++n;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_list::update()
	{
		param_.open_before_ = param_.open_;
		if(root_->get_selected()) {
			param_.open_ = true;
			wd_.enable(root_, false);
			wd_.enable(frame_, param_.open_, true);
		} else if(frame_->get_selected()) {
			param_.open_ = false;
			wd_.enable(root_);
			wd_.enable(frame_, param_.open_, true);
		}

		uint32_t n = 0;
		BOOST_FOREACH(widget_label* w, list_) {
			if(w->get_select()) {
				param_.select_pos_ = n;
				root_->at_local_param().text_param_.text_
					= w->get_local_param().text_param_.text_;
			}
			++n;
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
		const vtx::spos& sc = igl->get_size();

		if(!param_.open_ && param_.drop_box_) {
			// チップの描画
			gl::glmobj::handle h;
			if((root_->get_rect().org.y + frame_->get_rect().size.y) > sc.y) {
				h = wd_.get_share_image().up_box_;
			} else {
				h = wd_.get_share_image().down_box_;
			}

			const vtx::spos& bs = wd_.at_mobj().get_size(h);
			const vtx::spos& size = root_->get_rect().size;
			short wf = param_.plate_param_.frame_width_;
			short space = 4;
			vtx::spos pos(size.x - bs.x - wf - space, (size.y - bs.y) / 2);
			wd_.at_mobj().draw(h, gl::glmobj::normal, pos.x, pos.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_list::destroy_()
	{
		BOOST_FOREACH(widget_label* w, list_) {
			wd_.del_widget(w);
		}
		wd_.del_widget(frame_);
		wd_.del_widget(root_);
	}

}
