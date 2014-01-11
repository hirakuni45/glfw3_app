//=====================================================================//
/*!	@file
	@brief	GUI Widget ツリー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	void widget_tree::render_(gl::mobj::handle h, const vtx::spos& pos)
	{
		using namespace gl;
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		const vtx::spos& size = igl->get_size();
		const widget::param& wp = get_param();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 
			glPushMatrix();

			vtx::srect rect;
			const vtx::spos& mosz = wd_.at_mobj().get_size(h);
			vtx::spos ofs(0);
			ofs += pos;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				draw_mobj(wd_, h, wp.clip_, ofs);
				rect.org  = wp.rpos_;
				rect.size = wp.rect_.size;
			} else {
				wd_.at_mobj().draw(h, gl::mobj::attribute::normal, ofs.x, ofs.y);
				rect.org.set(0);
				rect.size = wp.rect_.size;
			}

			rect.org.x += mosz.x + 4;
			rect.org += pos;
///			img::rgba8 fc = param_.text_param_.fore_color_;
//			if(param_.disable_gray_text_ && !obj_state_) {
//				param_.text_param_.fore_color_ *= param_.gray_text_gain_;
//			}
///			widget::text_param tmp = param_.text_param_;
///			const img::rgbaf& cf = wd_.get_color();
///			tmp.fore_color_ *= cf.r;
///			tmp.fore_color_.alpha_scale(cf.a);
///			tmp.shadow_color_ *= cf.r;
///			tmp.shadow_color_.alpha_scale(cf.a);
			draw_text(param_.text_param_, rect, wp.clip_);

			igl->at_fonts().restore_matrix();

///			param_.text_param_.fore_color_ = fc;

			glPopMatrix();
			glViewport(0, 0, size.x, size.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_tree::initialize()
	{
		// 自由な大きさの変更
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);
		at_param().state_.set(widget::state::SERVICE);

		param_.plate_param_.resizeble_ = true;
		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

		// 共有テクスチャー
		mins_h_ = wd_.get_share_image().minus_box_;
		plus_h_ = wd_.get_share_image().plus_box_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_tree::update()
	{
		tree_unit_.create_list("", tree_unit_cits_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_tree::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, 0, 0);

		vtx::spos pos(0);
		BOOST_FOREACH(tree_unit::unit_map_cit cit, tree_unit_cits_) {
			param_.text_param_.text_ = cit->first;
			render_(plus_h_, pos);
///			wd_.at_mobj().draw(plus_h_, gl::mobj::attribute::normal, pos.x, pos.y);
			pos.y += 32;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_tree::service()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_tree::save(sys::preference& pre)
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
	bool widget_tree::load(const sys::preference& pre)
	{
		return true;
	}
}
