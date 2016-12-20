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
			widget::param wp(vtx::irect(vtx::ipos(
				(size.x - fw - btn_width) / 2, y),
				btn_size), this);
			widget_button::param wp_("OK");
			ok_ = wd_.add_widget<widget_button>(wp, wp_);
		} else if(param_.style_ == param::style::CANCEL_OK) {
			short ofs = (size.x - fw * 2 - btn_width * 2) / 3;
			{
				widget::param wp(vtx::irect(vtx::ipos(fw + ofs, y),
				btn_size), this);
				widget_button::param wp_("Cancel");
				cancel_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(vtx::ipos(fw + ofs + btn_width + ofs, y),
				btn_size), this);
				widget_button::param wp_("OK");
				ok_ = wd_.add_widget<widget_button>(wp, wp_);
			}
		}

		{
			param_.text_area_.size.x = size.x - param_.text_area_.org.x * 2;
			param_.text_area_.size.y = size.y - param_.text_area_.org.y * 2
				- space_height - btn_height;
			widget::param wp(param_.text_area_, this);
			widget_text::param wp_;
			wp_.text_param_.placement_.hpt = vtx::placement::holizontal::CENTER;
			wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
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
		bool close = false;
		if(ok_ && ok_->get_selected()) {
			param_.return_ok_     = true;
			param_.return_cancel_ = false;
			close = true;
		}
		if(cancel_ && cancel_->get_selected()) {
			param_.return_ok_     = false;
			param_.return_cancel_ = true;
			close = true;

		}

		if(close) {
			enable(false);
			if(param_.select_func_ != nullptr) param_.select_func_(param_.return_ok_);
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
		wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_dialog::save(sys::preference& pre)
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
	bool widget_dialog::load(const sys::preference& pre)
	{
		return true;
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
