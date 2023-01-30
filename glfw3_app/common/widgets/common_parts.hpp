#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget 共通パーツ（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <boost/unordered_map.hpp>
#include "gl_fw/glmobj.hpp"
#include "img_io/paint.hpp"
#include "widgets/widget.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	widget 共通パーツ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct common_parts {

	private:
		gl::mobj&		mobj_;

		typedef gl::mobj::handle handle;
		typedef boost::unordered_map<share_t, handle>	share_map;
		typedef share_map::iterator			share_map_it;
		typedef share_map::const_iterator	share_map_cit;
		share_map	share_map_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		common_parts(gl::mobj& mo) : mobj_(mo) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	共有ハンドルを取得
			@param[in]	k	キー
			@return	共有ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle get(const share_t& k) const {
			gl::mobj::handle h = 0;
			share_map_cit cit = share_map_.find(k);
			if(cit != share_map_.end()) {
				h = cit->second;
			}
			return h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	共有ハンドルを追加
			@param[in]	k	キー
			@return	描画ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle add(const share_t& k);
		// この実装は、widget_director.cpp 内にある。
		// 「widget_utils/create_round_frame」は、「widget_director」のインスタンスが必要で、インクルードが入れ子になってしまう。
		// その回避策として・・
#if 0
		{
			gl::mobj::handle h = get(k);
			if(h == 0) {
				img::paint pa;
				create_round_frame(pa, k.plate_param_, k.color_param_, k.size_);
				if(k.plate_param_.resizeble_) {
// std::cout << "grid: " << static_cast<int>(k.plate_param_.grid_.x) << ", " << static_cast<int>(k.plate_param_.grid_.y) << std::endl;
// std::cout << "size: " << static_cast<int>(pa.get_size().x) << ", " << static_cast<int>(pa.get_size().y) << std::endl;
					h = mobj_.install(&pa, k.size_, k.plate_param_.grid_);
				} else {
					h = mobj_.install(&pa);
				}
				std::pair<share_t, handle> t(k, h);
				share_map_.insert(t);
			}
// std::cout << static_cast<int>(h) << std::endl;
			return h;
		}
#endif
	};
}
