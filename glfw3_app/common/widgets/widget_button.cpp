//=====================================================================//
/*!	@file
	@brief	GUI widget_button クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widget_button.hpp"
#include "img_io/paint.hpp"
#include "widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_button::initialize()
	{
		// ボタンは標準的に固定、サイズ固定、選択時スケーリング
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().action_.set(widget::action::SELECT_SCALE);

		using namespace img;

		if(param_.handle_) {
			at_rect().size = wd_.at_mobj().get_size(param_.handle_);
			objh_ = param_.handle_;
		} else if(param_.image_) {
			paint pa;
			const vtx::spos& size = get_rect().size;
			create_image_base(param_.image_, size, pa);
			at_rect().size = pa.get_size();
			objh_ = wd_.at_mobj().install(&pa);
		} else {
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
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_button::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_button::render()
	{
		if(objh_ == 0) return;

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(objh_, get_param().rect_.size);
		}

		render_text(wd_, objh_, get_param(), param_.text_param_, param_.plate_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_button::destroy()
	{
	}

}
