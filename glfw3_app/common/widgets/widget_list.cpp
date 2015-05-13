//=====================================================================//
/*!	@file
	@brief	GUI Widget リスト
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_utils.hpp"
#include <boost/foreach.hpp>

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_list::initialize()
	{
		// 標準的に固定、リサイズ不可
		at_param().state_.set(widget::state::SERVICE);
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);

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

		t.color_param_ = param_.color_param_select_;
		select_objh_ = wd_.share_add(t);

		{
			widget::param wp(vtx::srect(0, 0,
				get_rect().size.x, get_rect().size.y * param_.text_list_.size()), this);
			widget_null::param wp_;
			frame_ = wd_.add_widget<widget_null>(wp, wp_);
			frame_->set_state(widget::state::POSITION_LOCK);
			frame_->set_state(widget::state::ENABLE, false);
		}

		{
			widget::param wp(vtx::srect(vtx::spos(0), get_rect().size), frame_);
			widget_label::param wp_;
			wp_.plate_param_ = param_.plate_param_;
			wp_.color_param_ = param_.color_param_select_;
			wp_.plate_param_.frame_width_ = 0;
			int n = 0;
			BOOST_FOREACH(const std::string& s, param_.text_list_) {
				wp_.text_param_.set_text(s);
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
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_list::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_list::service()
	{
		if(!get_state(widget::state::ENABLE)) {
			return;
		}

		param_.open_before_ = param_.open_;
		if(get_selected() && list_.size() > 0) {
			param_.open_ = true;
			wd_.enable(frame_, param_.open_, true);
			wd_.top_widget(frame_);
		}

		if(param_.open_ && list_.size() > 0) {
			uint32_t n = 0;
			bool selected = false;
			BOOST_FOREACH(widget_label* w, list_) {
				if(w->get_select()) {
					param_.select_pos_ = n;
					param_.text_param_.text_ = w->get_local_param().text_param_.text_;
				} else if(w->get_selected()) {
					selected = true;
				}
				++n;
			}
			if(selected) {
				param_.open_ = false;
				wd_.enable(frame_, param_.open_, true);
			} else {
				const vtx::spos& scr = wd_.get_scroll();
				if(frame_->get_focus() && scr.y != 0) {
					int pos = param_.select_pos_;
					pos += scr.y;
					if(pos < 0) {
						pos = 0;
					} else if(pos >= static_cast<int>(list_.size())) {
						pos = list_.size() - 1;
					}
					param_.select_pos_ = pos;
				}
				int n = 0;
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
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_list::render()
	{
		using namespace gl;

		core& core = core::get_instance();
		const vtx::spos& siz = core.get_rect().size;

		gl::mobj::handle h = objh_;
		if(get_select()) {
			h = select_objh_;
		}

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(objh_, get_param().rect_.size);
		}

		render_text(wd_, h, get_param(), param_.text_param_, param_.plate_param_);

		if(!param_.open_ && param_.drop_box_) {
			wd_.at_mobj().setup_matrix(siz.x, siz.y);
			wd_.set_TSC();
			// チップの描画
			gl::mobj::handle h;
			if((get_rect().org.y + frame_->get_rect().size.y) > siz.y) {
				h = wd_.get_share_image().up_box_;
			} else {
				h = wd_.get_share_image().down_box_;
			}

			const vtx::spos& bs = wd_.at_mobj().get_size(h);
			const vtx::spos& size = get_rect().size;
			short wf = param_.plate_param_.frame_width_;
			short space = 4;
			vtx::spos pos(size.x - bs.x - wf - space, (size.y - bs.y) / 2);
			wd_.at_mobj().draw(h, gl::mobj::attribute::normal, pos);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void widget_list::destroy_()
	{
		BOOST_FOREACH(widget_label* w, list_) {
			wd_.del_widget(w);
		}
		wd_.del_widget(frame_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_list::save(sys::preference& pre)
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
	bool widget_list::load(const sys::preference& pre)
	{
		return true;
	}
}
