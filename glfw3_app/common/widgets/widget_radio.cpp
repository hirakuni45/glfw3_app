//=====================================================================//
/*!	@file
	@brief	widget check クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_utils.hpp"
#include "img_io/paint.hpp"
#include <boost/foreach.hpp>

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_radio::initialize()
	{
		// ボタンは標準的に固定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);

		dis_h_ = wd_.get_share_image().un_radio_;
		ena_h_ = wd_.get_share_image().to_radio_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_radio::update()
	{
		bool f = param_.check_;
		if(get_select()) {
			obj_state_ = true;
		} else if(get_selected()) {
			param_.check_ = true;
			obj_state_ = param_.check_;
		} else {
			obj_state_ = param_.check_;
		}

		// グループの自動更新
		if(!f && param_.check_) {
			widgets ws;
			wd_.parents_widget(get_param().parents_, ws);
			BOOST_FOREACH(widget* w, ws) {
				if(w->type() == get_type_id<widget_radio>()) {
					if(w != this) {
						widget_radio* wl = dynamic_cast<widget_radio*>(w);
						if(wl) {
							wl->at_local_param().check_ = false;
						}
					}
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_radio::render()
	{
		using namespace gl;
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		const vtx::spos& size = igl->get_size();
		const widget::param& wp = get_param();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 
			gl::mobj::handle h;
			if(obj_state_) h = ena_h_;
			else h = dis_h_;

			glPushMatrix();

			vtx::srect rect;
			const vtx::spos& mosz = wd_.at_mobj().get_size(h);
			vtx::spos ofs(0, (wp.rect_.size.y - mosz.y) / 2);
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
			img::rgba8 fc = param_.text_param_.fore_color_;
			if(param_.disable_gray_text_ && !obj_state_) {
				param_.text_param_.fore_color_ *= param_.gray_text_gain_;
			}
			widget::text_param tmp = param_.text_param_;
			const img::rgbaf& cf = wd_.get_color();
			tmp.fore_color_ *= cf.r;
			tmp.fore_color_.alpha_scale(cf.a);
			tmp.shadow_color_ *= cf.r;
			tmp.shadow_color_.alpha_scale(cf.a);
			draw_text(tmp, rect, wp.clip_);

			igl->at_fonts().restore_matrix();

			param_.text_param_.fore_color_ = fc;

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
	bool widget_radio::save(sys::preference& pre)
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
	bool widget_radio::load(const sys::preference& pre)
	{
		return true;
	}
}
