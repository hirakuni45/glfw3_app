//=====================================================================//
/*!	@file
	@brief	GUI Widget ツリー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_tree.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

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

		// 共有テクスチャー
		mins_h_ = wd_.get_share_image().minus_box_;
		plus_h_ = wd_.get_share_image().plus_box_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_tree::update()
	{
		tree_unit_.create_list("", tree_unit_cits_);
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
			wd_.at_mobj().draw(plus_h_, gl::mobj::attribute::normal, pos.x, pos.y);
//			cit->first
			pos.y += 24;
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
		return true;
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
		return true;
	}
}
