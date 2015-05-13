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
		// 標準的に固定、リサイズ不可
		at_param().state_.set(widget::state::SERVICE);
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().state_.set(widget::state::ENABLE, false);

		widget::param wp(vtx::srect(vtx::spos(0), get_rect().size), this);
		widget_label::param wp_;
		wp_.plate_param_ = param_.plate_param_;
		wp_.color_param_ = param_.color_param_select_;
		wp_.plate_param_.frame_width_ = 0;
		int n = 0;
		BOOST_FOREACH(const std::string& s, param_.text_list_) {
			wp_.text_param_.set_text(s);
			if(n == 0 && param_.round_) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_
					= widget::plate_param::round_style::TOP;
			} else if(n == (param_.text_list_.size() - 1) && param_.round_) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_
					= widget::plate_param::round_style::BOTTOM;
			} else {
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.round_style_
					= widget::plate_param::round_style::ALL;
			}
			widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
//			w->set_state(widget::state::MOVE_ROOT);
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
		if(!get_state(widget::state::ENABLE) || list_.empty()) {
			return;
		}

		wd_.top_widget(this);

		uint32_t n = 0;
		bool selected = false;
		BOOST_FOREACH(widget_label* w, list_) {
			if(w->get_select()) {
				param_.select_pos_ = n;
				at_local_param().text_param_.text_
					= w->get_local_param().text_param_.text_;
			} else if(w->get_selected()) {
				selected = true;
			}
			++n;
		}
		if(selected) {
			if(param_.select_pos_ < list_.size()) {
				param_.select_text_ = list_[param_.select_pos_]->get_local_param().text_param_.get_text();
			}
			++select_id_;
			wd_.enable(this, false, true);
			if(param_.select_func_) param_.select_func_(param_.select_text_);
		} else {
			const vtx::spos& scr = wd_.get_scroll();
			if(get_focus() && scr.y != 0) {
				int pos = param_.select_pos_;
				pos += scr.y;
				if(pos < 0) {
					pos = 0;
				} else if(pos >= static_cast<int>(list_.size())) {
					pos = list_.size() - 1;
				}
				param_.select_pos_ = pos;
			}
			uint32_t n = 0;
			BOOST_FOREACH(widget_label* w, list_) {
				if(n == param_.select_pos_) {
					w->set_action(widget::action::SELECT_HIGHLIGHT);
				} else {
					w->set_action(widget::action::SELECT_HIGHLIGHT, false);
				}
				++n;
			}
		}
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
