//=====================================================================//
/*!	@file
	@brief	GUI Widget ツリー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"
#include <stack>

namespace gui {

	void widget_tree::create_()
	{
		vtx::srect r;
		r.org.set(0);
		r.size.x = get_rect().size.x;
		r.size.y = param_.height_;
		tree_unit::unit_map_its its;
		tree_unit_.create_list("", its);
		BOOST_FOREACH(tree_unit::unit_map_it it, its) {
			if(!it->second.value.path_) {
				gl::IGLcore* igl = gl::get_glcore();
				gl::fonts& fonts = igl->at_fonts();
				r.size.x = fonts.get_width(utils::get_file_name(it->first)) + r.size.y + 8;
				widget::param wp(r, this);
				widget_check::param wp_(utils::get_file_name(it->first));
				wp_.type_ = widget_check::style::MINUS_PLUS;
				widget_check* w = wd_.add_widget<widget_check>(wp, wp_);
				w->set_state(widget::state::POSITION_LOCK);
				w->set_state(widget::state::SIZE_LOCK);
				w->set_state(widget::state::MOVE_ROOT, false);
				w->set_state(widget::state::RESIZE_ROOT);
				w->set_state(widget::state::DRAG_UNSELECT);
				w->set_state(widget::state::CLIP_PARENTS);
				if(tree_unit_.is_directory(it)) {
					r.org.y += param_.height_;
				} else {
					wd_.enable(w, false);
				}
				it->second.value.path_ = w;
			}
		}
	}


	void widget_tree::destroy_()
	{
		tree_unit::unit_map_its its;
		tree_unit_.create_list("", its);
		BOOST_FOREACH(tree_unit::unit_map_it it, its) {
			wd_.del_widget(it->second.value.path_);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_tree::initialize()
	{
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE, false);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE, false);
		at_param().state_.set(widget::state::SERVICE);
		at_param().state_.set(widget::state::MOVE_ROOT, false);
		at_param().state_.set(widget::state::RESIZE_ROOT);
		at_param().state_.set(widget::state::CLIP_PARENTS);
		at_param().state_.set(widget::state::AREA_ROOT);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_tree::update()
	{
		if(get_param().parents_ && get_state(widget::state::AREA_ROOT)) {
			if(get_param().parents_->type() == get_type_id<widget_frame>()) {
				widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
				w->get_draw_area(at_rect());
			}
		}

		if(param_.single_) {
			tree_unit::unit_map_its its;
			tree_unit_.create_list("", its);
			widget_check* sel = 0;
			BOOST_FOREACH(tree_unit::unit_map_it it, its) {
				widget_check* w = it->second.value.path_;
				if(w == 0) continue;
				if(tree_unit_.is_directory(it)) continue;
				if(w->get_select_out()) {
					sel = w;
					break;
				}
			}
			if(sel) {
				BOOST_FOREACH(tree_unit::unit_map_it it, its) {
					widget_check* w = it->second.value.path_;
					if(w == 0) continue;
					if(tree_unit_.is_directory(it)) continue;
					if(w != sel) {
						w->set_check(false);
					} else {
						select_it_ = it;
					}
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_tree::service()
	{
		if(!get_state(widget::state::ENABLE)) {
			return;
		}

		// ツリーが更新されたら、アイテムを作り直す
		if(tree_unit_.get_serial_id() != serial_id_ ||
		   tree_unit_.get_unit_num() != unit_num_) {
			destroy_();
			create_();
			serial_id_ = tree_unit_.get_serial_id();
			unit_num_ = tree_unit_.get_unit_num();
		}

		tree_unit::unit_map_its its;
		tree_unit_.create_list("", its);
		vtx::spos pos(0);
		std::stack<bool> open_stack;
		bool open = true;
		uint32_t nest = 1;
		BOOST_FOREACH(tree_unit::unit_map_it it, its) {
			widget_check* w = it->second.value.path_;
			if(w == 0) continue;
			uint32_t n = utils::count_char(it->first, '/');
			bool draw = open;
			if(n <= 1) {
				draw = true;
				open = w->get_check();
			}
			wd_.enable(w, draw);
			if(draw) {
				w->at_local_param().draw_box_ = tree_unit_.is_directory(it);
				pos.x = (n - 1) * param_.height_;
				w->at_rect().org = pos;
				pos.y += param_.height_;
			}
			if(tree_unit_.is_directory(it)) {
				open_stack.push(open);
				if(open) {
					open = w->get_check();
				}
			}
			if(nest > n) {
				open = open_stack.top();
				open_stack.pop();
			}
			nest = n;
		}

		if(get_select_in()) {
			speed_.set(0.0f);
			offset_ = position_;
		}
		float damping = 0.85f;
		float slip_gain = 0.5f;
		short d = get_rect().size.y - pos.y;
		if(get_select()) {
			position_ = offset_ + get_param().move_pos_ - get_param().move_org_;
			if(d < 0) {
				if(position_.y < d) {
					position_.y -= d;
					position_.y *= slip_gain;
					position_.y += d;
				} else if(position_.y > 0) {
					position_.y *= slip_gain;
				}
			} else {
				position_.y *= slip_gain;
			}
		} else {
			if(d < 0) {
				if(position_.y < d) {
					position_.y -= d;
					position_.y *= damping;
					position_.y += d;
					speed_.y = 0.0f;
					if(position_.y > (d - 0.5f)) {
						position_.y = d;
					}
				} else if(position_.y > 0.0f) {
					position_.y *= damping;
					speed_.y = 0.0f;
					if(position_.y < 0.5f) {
						position_.y = 0.0f;
					}
				} else {
					const vtx::spos& scr = wd_.get_scroll();
					if(get_focus() && scr.y != 0) {
						position_.y += scr.y * param_.height_;
						if(position_.y < d) {
							position_.y = d;
						} else if(position_.y > 0.0f) {
							position_.y = 0.0f;
						}
					}
				}
			} else {
				position_.y *= damping;
				if(-0.5f < position_.y && position_.y < 0.5f) {
					position_.y = 0.0f;
					speed_.y = 0.0f;
				}
			}
		}
//		if(get_select_out()) {
//		}

		short ofsy = position_.y;
		BOOST_FOREACH(tree_unit::unit_map_it it, its) {
			widget_check* w = it->second.value.path_;
			if(w == 0) continue;
			w->at_rect().org.y += ofsy;
		}

	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_tree::render()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_tree::save(sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		tree_unit::unit_map_its its;
		tree_unit_.create_list("", its);
		BOOST_FOREACH(tree_unit::unit_map_it it, its) {
			widget_check* w = it->second.value.path_;
			if(w == 0) continue;
			
		}

		return err == 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_tree::load(const sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		tree_unit::unit_map_its its;
		tree_unit_.create_list("", its);
		BOOST_FOREACH(tree_unit::unit_map_it it, its) {
			widget_check* w = it->second.value.path_;
			if(w == 0) continue;
			
		}

		return err == 0;
	}
}
