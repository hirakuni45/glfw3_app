//=====================================================================//
/*!	@file
	@brief	GUI widget_image クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "gl_fw/glutils.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_frame.hpp"
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
		at_param().state_.set(widget::state::AREA_ROOT);

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

			img::copy_to_rgba8(param_.image_, vtx::srect(vtx::spos(0), s), pa, o);

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
		if(get_param().parents_ && get_state(widget::state::AREA_ROOT)) {
			if(get_param().parents_->type() == get_type_id<widget_frame>()) {
				widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
				w->get_draw_area(at_rect());
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_image::render()
	{
		using namespace gl;
		core& core = core::get_instance();
		const vtx::spos& vsz = core.get_size();
		const vtx::spos& siz = core.get_rect().size;
		const widget::param& wp = get_param();

		glPushMatrix();
		if(objh_) {
			if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {
				vtx::srect rect;
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					draw_mobj(wd_, objh_, wp.clip_);
				} else {
					wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));
				}
			}
		} else if(param_.mobj_ && param_.mobj_handle_) {
			mobj& mo = *param_.mobj_;
			if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {
//				vtx::srect rect;
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					int sx = vsz.x / siz.x;
					int sy = vsz.y / siz.y;
					glViewport(wp.clip_.org.x * sx,
							   vsz.y - wp.clip_.org.y * sy - wp.clip_.size.y * sy,
						wp.clip_.size.x * sx, wp.clip_.size.y * sy);
						mo.setup_matrix(wp.clip_.size.x, wp.clip_.size.y);
				}
				glScale(param_.scale_);
				mo.draw(param_.mobj_handle_, gl::mobj::attribute::normal, param_.offset_, param_.linear_);
			}
		}
		glPopMatrix();

		glViewport(0, 0, vsz.x, vsz.y);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_image::save(sys::preference& pre)
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
	bool widget_image::load(const sys::preference& pre)
	{
		return true;
	}
}
