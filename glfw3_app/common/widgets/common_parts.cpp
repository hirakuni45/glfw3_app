//=====================================================================//
/*!	@file
	@brief	GUI widget 共通パーツ
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/common_parts.hpp"
#include "img_io/paint.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	共有ハンドルを追加
		@param[in]	k	キー
		@return	描画ハンドル
	*/
	//-----------------------------------------------------------------//
	gl::mobj::handle common_parts::add(const share_t& k)
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
}
