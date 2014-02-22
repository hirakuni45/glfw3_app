//=====================================================================//
/*!	@file
	@brief	GUI Widget スライダー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widget_slider.hpp"
#include "widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_slider::initialize()
	{
		// 標準的設定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);

		vtx::spos size = get_rect().size;
		img::paint pa;
		{
			const widget::plate_param& pp = param_.plate_param_;
			if(pp.resizeble_) {
				vtx::spos bsz = size;
				if(bsz.x >= pp.grid_.x * 3) bsz.x = pp.grid_.x * 3;
				if(bsz.y >= pp.grid_.y * 3) bsz.y = pp.grid_.y * 3;
				create_round_frame(pa, param_.plate_param_, param_.color_param_, bsz);
				base_h_ = wd_.at_mobj().install(&pa, bsz, param_.plate_param_.grid_);
			} else {
				create_round_frame(pa, param_.plate_param_, param_.color_param_, size);
				base_h_ = wd_.at_mobj().install(&pa);
			}
		}

		const slider_param& sp = param_.slider_param_;
		if(param_.hand_image_) {
			const vtx::spos& size = param_.hand_image_->get_size();
			create_image_base(param_.hand_image_, size, pa);
		} else {
			short wf = param_.plate_param_.frame_width_;
			if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
				size.x = (size.x - wf * 2) * param_.slider_param_.handle_ratio_;
				size.y -= wf * 2;
			} else if(sp.direction_ == slider_param::direction::VERTICAL) {
				size.x -= wf * 2;
				size.y = (size.y - wf * 2) * param_.slider_param_.handle_ratio_;
			}
			plate_param pp = param_.plate_param_;
			pp.round_radius_ -= param_.plate_param_.frame_width_;
			pp.frame_width_ = 0;
			color_param cp = param_.color_param_;
			cp.swap_color();
			cp.back_color_ *= 0.75f;
			create_round_frame(pa, pp, cp, size);
			pa.set_fore_color(cp.fore_color_);
			pa.alpha_blend();
			if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
				pa.line(size.x / 2, 3, size.x / 2, size.y - 3 * 2);
			} else if(sp.direction_ == slider_param::direction::VERTICAL) {

			}
		}
		hand_h_ = wd_.at_mobj().install(&pa);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_slider::update()
	{
		const slider_param& sp = param_.slider_param_;
		short fw = param_.plate_param_.frame_width_;
		if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
			handle_offset_.x = param_.plate_param_.frame_width_;
			short ofs = wd_.at_mobj().get_size(hand_h_).y - wd_.at_mobj().get_size(base_h_).y;
			handle_offset_.y = -ofs / 2;
		} else if(sp.direction_ == slider_param::direction::VERTICAL) {
			short ofs = wd_.at_mobj().get_size(hand_h_).x - wd_.at_mobj().get_size(base_h_).x;
			handle_offset_.x = -ofs / 2;
			handle_offset_.y = param_.plate_param_.frame_width_;
		}

		if(get_focus() && param_.hand_ctrl_) {
			if(get_select()) {
				if(get_select_in()) {
					ref_position_ = sp.position_;
					ref_point_ = get_param().in_point_;
				}
				const vtx::spos size = get_rect().size;
				float ratio = 0.0f;
				vtx::spos ref = get_param().in_point_ - ref_point_;
				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
					short sz = size.x - fw * 2;
					short ofs = sz * sp.handle_ratio_;
					ratio = static_cast<float>(ref.x) / static_cast<float>(sz - ofs);
				} else if(sp.direction_ == slider_param::direction::VERTICAL) {
					short sz = size.y - fw * 2;
					short ofs = sz * sp.handle_ratio_;
					ratio = static_cast<float>(ref.y) / static_cast<float>(sz - ofs);
				}
				ratio += ref_position_;
				if(ratio < 0.0f) ratio = 0.0f;
				else if(ratio > 1.0f) ratio = 1.0f;
				if(sp.grid_ > 0.0f) {
					short step = ratio / (sp.grid_ * 0.5f);
					ratio = ((step + 1) / 2) * sp.grid_;
				}
				param_.slider_param_.position_ = ratio;
			}
			// アクセレーター・アクセス
			if(get_select_out()) {
				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {

				} else if(sp.direction_ == slider_param::direction::VERTICAL) {

				}
			}
		}

		const vtx::spos& size = get_rect().size;
		if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
			short sz = size.x - fw * 2;
			short ofs;
			if(param_.hand_image_) {
				ofs = wd_.at_mobj().get_size(hand_h_).x;
			} else {
				ofs = sz * sp.handle_ratio_;
			}						
			handle_offset_.x = fw + sp.position_ * (sz - ofs);
		} else if(sp.direction_ == slider_param::direction::VERTICAL) {
			short sz = size.y - fw * 2;
			short ofs;
			if(param_.hand_image_) {
				ofs = wd_.at_mobj().get_size(hand_h_).y;
			} else {
				ofs = sz * sp.handle_ratio_;
			}						
			handle_offset_.y = fw + sp.position_ * (sz - ofs);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_slider::render()
	{
		if(base_h_ == 0) return;

		using namespace gl;
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(base_h_, get_param().rect_.size);
		}

		const vtx::spos& size = igl->get_size();

		const widget::param& wp = get_param();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 
			glPushMatrix();
			vtx::srect rect;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				draw_mobj(wd_, base_h_, wp.clip_);
			} else {
				wd_.at_mobj().draw(base_h_, gl::mobj::attribute::normal, 0, 0);
			}

			wd_.at_mobj().draw(hand_h_, gl::mobj::attribute::normal,
				handle_offset_.x, handle_offset_.y);
			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_slider::save(sys::preference& pre)
	{
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_slider::load(const sys::preference& pre)
	{
		return true;
	}
}
