//=====================================================================//
/*!	@file
	@brief	GUI Widget ツリー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	void widget_tree::create_()
	{
		units_.reserve(tree_unit_cits_.size());
		vtx::srect r;
		r.org.set(param_.plate_param_.frame_width_);
		r.size.x = get_rect().size.x;
		r.size.y = param_.height_;
		BOOST_FOREACH(tree_unit::unit_map_cit cit, tree_unit_cits_) {
			widget::param wp(r, this);
			widget_check::param wp_(cit->first);
			wp_.type_ = widget_check::style::MINUS_PLUS;
			widget_check* w = wd_.add_widget<widget_check>(wp, wp_);
			w->set_state(widget::state::POSITION_LOCK);
			w->set_state(widget::state::MOVE_ROOT);
			w->set_state(widget::state::RESIZE_ROOT);
			w->set_state(widget::state::DRAG_UNSELECT);
			units_.push_back(w);
			r.org.y += param_.height_;
		}
	}

	void widget_tree::destroy_()
	{
		BOOST_FOREACH(widget_check* w, units_) {
			wd_.del_widget(w);
		}
		units_.clear();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_tree::initialize()
	{
		// 自由な大きさの変更
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);
		at_param().state_.set(widget::state::SERVICE);

		param_.plate_param_.resizeble_ = true;
		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_tree::update()
	{
		uint32_t n = tree_unit_cits_.size();
		tree_unit_.create_list("", tree_unit_cits_);

		if(n != tree_unit_cits_.size()) {
			create_();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_tree::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, 0, 0);

		vtx::spos pos(0);
		BOOST_FOREACH(tree_unit::unit_map_cit cit, tree_unit_cits_) {
//			param_.text_param_.text_ = cit->first;

			pos.y += 32;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_tree::service()
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
		if(!pre.put_position(path + "/locate",  vtx::ipos(get_rect().org))) ++err;
		if(!pre.put_position(path + "/size", vtx::ipos(get_rect().size))) ++err;

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
		vtx::ipos p;
		if(pre.get_position(path + "/locate", p)) {
			at_rect().org = p;
		} else {
			++err;
		}
		if(pre.get_position(path + "/size", p)) {
			at_rect().size = p;
		} else {
			++err;
		}

		return err == 0;
	}
}
