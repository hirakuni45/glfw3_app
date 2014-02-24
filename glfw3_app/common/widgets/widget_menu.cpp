//=====================================================================//
/*!	@file
	@brief	GUI widget_menu クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_menu.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_menu::initialize()
	{
		widget::param wp(vtx::srect(vtx::spos(0), get_rect().size), this);
		widget_label::param wp_;
		wp_.plate_param_ = param_.plate_param_;
		wp_.color_param_ = param_.color_param_select_;
		wp_.plate_param_.frame_width_ = 0;
		int n = 0;
		BOOST_FOREACH(const std::string& s, param_.text_list_) {
			wp_.text_param_.text_ = s;
			if(n == 0) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_
					= widget::plate_param::round_style::TOP;
			} else if(n == (param_.text_list_.size() - 1)) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_
					= widget::plate_param::round_style::BOTTOM;
			} else {
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.round_style_
					= widget::plate_param::round_style::ALL;
			}
			widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
			w->set_state(widget::state::ENABLE, false);
			list_.push_back(w);
			wp.rect_.org.y += get_rect().size.y;
			++n;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_menu::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_menu::service()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_menu::render()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_menu::destroy_()
	{
		BOOST_FOREACH(widget_label* w, list_) {
			wd_.del_widget(w);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_menu::save(sys::preference& pre)
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
	bool widget_menu::load(const sys::preference& pre)
	{
		return true;
	}
}
