//=====================================================================//
/*!	@file
	@brief	GUI widget_dialog クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_dialog.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::initialize()
	{
		const vtx::spos& size = get_rect().size;
		{	// ベース・フレームの作成
			widget::param wp(vtx::srect(vtx::spos(0), get_rect().size), this);
			widget_frame::param wp_;
			wp_.plate_param_ = param_.plate_param_;
			wp_.color_param_ = param_.color_param_;
			base_ = wd_.add_widget<widget_frame>(wp, wp_);
			base_->set_state(widget::state::SIZE_LOCK);
		}

		short fw = param_.plate_param_.frame_width_;
		short btn_width = 100;
		short btn_height = 40;
		short space_height = 10;
		vtx::spos btn_size(btn_width, btn_height);
		short y = size.y - space_height - btn_height;
		if(param_.style_ == param::style::OK) {
			widget::param wp(vtx::srect(vtx::spos(
				(size.x - fw - btn_width) / 2, y),
				btn_size), base_);
			widget_button::param wp_("OK");
			ok_ = wd_.add_widget<widget_button>(wp, wp_);
		} else if(param_.style_ == param::style::CANCEL_OK) {
			short ofs = (size.x - fw * 2 - btn_width * 2) / 3;
			{
				widget::param wp(vtx::srect(vtx::spos(fw + ofs, y),
				btn_size), base_);
				widget_button::param wp_("Cancel");
				ok_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::srect(vtx::spos(fw + ofs + btn_width + ofs, y),
				btn_size), base_);
				widget_button::param wp_("OK");
				cancel_ = wd_.add_widget<widget_button>(wp, wp_);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::update()
	{
		bool f = false;
		if(ok_ && ok_->get_selected()) {
			param_.return_ok_ = true;
			f = true;
		}
		if(cancel_ && cancel_->get_selected()) {
			param_.return_cancel_ = true;
			f = true;
		}

		if(f) wd_.enable(this, false, true);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::render()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::destroy()
	{
		wd_.del_widget(cancel_);
		wd_.del_widget(ok_);
		wd_.del_widget(base_);
	}
}
