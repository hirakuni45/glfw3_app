//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::initialize()
	{
		// 自由な大きさの変更
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);
		at_param().state_.set(widget::state::SERVICE);

		param_.plate_param_.resizeble_ = true;
		at_param().resize_min_ = param_.plate_param_.grid_ * 3;

		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

		// ターミナル用フォントの登録
#if 0
		gl::IGLcore* igl = gl::get_glcore();
		gl::glfonts& fonts = igl->at_fonts();
		std::string cf = fonts.get_font_type();

		fonts.set_font_type(cf);
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::normal, 0, 0);

		gl::IGLcore* igl = gl::get_glcore();
		gl::fonts& fonts = igl->at_fonts();



	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_terminal::service()
	{

	}

}
