//=====================================================================//
/*!	@file
	@brief	GUI widget_frame クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_frame::initialize()
	{
		// 標準的設定（自由な大きさの変更）
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);
		at_param().state_.set(widget::state::RESIZE_EDGE_ENABLE);

		param_.plate_param_.resizeble_ = true;
		param_.text_param_.shadow_offset_.set(0);	// 通常「影」は付けない。
		param_.text_param_.fore_color_.set(250, 250, 250);
		at_param().resize_min_ = param_.plate_param_.grid_ * 3;

		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_frame::update()
	{
		param_.shift_param_.size_ = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
		shift_text_update(get_param(), param_.text_param_, param_.shift_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_frame::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));

		shift_text_render(get_param(), param_.text_param_, param_.plate_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_frame::save(sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		if(!pre.put_position(path + "/locate",  vtx::ipos(get_rect().org))) ++err;
		if(!pre.put_position(path + "/size", vtx::ipos(get_rect().size))) ++err;

		return err == 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@param[in]	w_ena	幅を無視する場合「false」
		@param[in]	h_ena	縦を無視する場合「false」
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_frame::load(const sys::preference& pre, bool w_ena, bool h_ena)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		vtx::ipos p;
		if(pre.get_position(path + "/locate", p)) {
			at_rect().org = p;
		} else {
			++err;
		}
		if(pre.get_position(path + "/size", p)) {
			if(w_ena) at_rect().size.x = p.x;
			if(h_ena) at_rect().size.y = p.y;
		} else {
			++err;
		}

		return err == 0;
	}
}
