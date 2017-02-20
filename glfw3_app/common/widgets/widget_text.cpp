//=====================================================================//
/*!	@file
	@brief	GUI widget_image クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_frame.hpp"
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
		at_param().state_.set(widget::state::MOVE_ROOT);
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
		core& core = core::get_instance();

		const vtx::spos& vsz = core.get_size();
		const widget::param& wp = get_param();

		if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {

			auto clip = wp.clip_;
			glPushMatrix();

			vtx::irect rect;
			if(wp.state_[widget::state::CLIP_PARENTS]) {
				vtx::ipos o(0);
				vtx::ipos w(0);
				widget_frame* par = static_cast<widget_frame*>(wp.parents_);
				if(par != nullptr && par->type() == get_type_id<widget_frame>()) {
					const auto& plate = par->get_local_param().plate_param_; 
					o.x = plate.frame_width_;
					o.y = plate.frame_width_ + plate.caption_width_;
					w.x = plate.frame_width_ * 2;
					w.y = o.y + plate.frame_width_ + 4;
					clip.size.y -= plate.frame_width_;
				}
				rect.org  = wp.rpos_ + o;
				rect.size = wp.rect_.size - w;
			} else {
				rect.org.set(0);
				rect.size = wp.rect_.size;
			}

			widget::text_param tpr = param_.text_param_;
			const img::rgbaf& cf = wd_.get_color();
			tpr.fore_color_ *= cf.r;
			tpr.fore_color_.alpha_scale(cf.a);
			tpr.shadow_color_ *= cf.r;
			tpr.shadow_color_.alpha_scale(cf.a);
			draw_text(tpr, rect, clip);

			core.at_fonts().restore_matrix();

			glPopMatrix();
			glViewport(0, 0, vsz.x, vsz.y);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_text::save(sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);
		int err = 0;
		if(!pre.put_text(path + "/text", param_.text_param_.get_text())) ++err;
//		if(!pre.put_position(path + "/locate", vtx::ipos(get_rect().org))) ++err;
//		if(!pre.put_position(path + "/size", vtx::ipos(get_rect().size))) ++err;
		return err == 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_text::load(const sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		std::string s;
		if(!pre.get_text(path + "/text", s)) {
			++err;
		} else {
			param_.text_param_.set_text(s);
		}
		return err == 0;
	}
}
