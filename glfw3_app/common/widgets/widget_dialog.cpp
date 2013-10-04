//=====================================================================//
/*!	@file
	@brief	GUI widget_dialog クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	文書の設定
		@param[in]	text	文書
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::set_text(const std::string& text)
	{
		if(text_) {
			text_->at_local_param().text_param_.text_ = text;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::initialize()
	{
		// 自由な大きさの変更を禁止
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().state_.set(widget::state::SERVICE);

		param_.plate_param_.resizeble_ = true;
		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

		// 構成部品の作成
		const vtx::spos& size = get_rect().size;

		short fw = param_.plate_param_.frame_width_;
		short btn_width = 100;
		short btn_height = 40;
		short space_height = 10;
		vtx::spos btn_size(btn_width, btn_height);
		short y = size.y - space_height - btn_height;
		if(param_.style_ == param::style::OK) {
			widget::param wp(vtx::srect(vtx::spos(
				(size.x - fw - btn_width) / 2, y),
				btn_size), this);
			widget_button::param wp_("OK");
			ok_ = wd_.add_widget<widget_button>(wp, wp_);
		} else if(param_.style_ == param::style::CANCEL_OK) {
			short ofs = (size.x - fw * 2 - btn_width * 2) / 3;
			{
				widget::param wp(vtx::srect(vtx::spos(fw + ofs, y),
				btn_size), this);
				widget_button::param wp_("Cancel");
				ok_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::srect(vtx::spos(fw + ofs + btn_width + ofs, y),
				btn_size), this);
				widget_button::param wp_("OK");
				cancel_ = wd_.add_widget<widget_button>(wp, wp_);
			}
		}

		{
			param_.text_area_.size.x = size.x - param_.text_area_.org.x * 2;
			param_.text_area_.size.y = size.y - param_.text_area_.org.y * 2
				- space_height - btn_height;
			widget::param wp(param_.text_area_, this);
			widget_text::param wp_;
			text_ = wd_.add_widget<widget_text>(wp, wp_);
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

		if(f) {
			enable(false);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::normal, 0, 0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_dialog::destroy_()
	{
		wd_.del_widget(cancel_);
		wd_.del_widget(ok_);
		wd_.del_widget(text_);
	}
}
