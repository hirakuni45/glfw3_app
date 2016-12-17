#pragma once
//=====================================================================//
/*!	@file
	@brief	spring damper class
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/vmath.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	スプリング・ダンパー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class spring_damper {

		float	position_;
		float	offset_;
		float	speed_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		spring_damper() : position_(0.0f), offset_(0.0f), speed_(0.0f) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
			@param[in]	select_in	選択開始 (get_select_in)
			@param[in]	select		選択 (get_select)
			@param[in]	pos			現在位置
			@param[in]	scr			スクロール制御量
			@param[in]	limit		リミット
			@param[in]	drag		ドラッグ長
			@return 更新位置
		*/
		//-----------------------------------------------------------------//
		int32_t update(bool select_in, bool select, int32_t pos, int32_t scr, int32_t limit, int32_t drag)
		{
			if(select_in) {
				speed_ = 0.0f;
				offset_ = static_cast<float>(pos);
			}
			float damping = 0.85f;
			float slip_gain = 0.5f;
			float d = static_cast<float>(limit - pos);
			if(select) {
				position_ = offset_ + static_cast<float>(drag);
				if(d < 0.0f) {
					if(position_ < d) {
						position_ -= d;
						position_ *= slip_gain;
						position_ += d;
					} else if(position_ > 0.0f) {
						position_ *= slip_gain;
					}
				} else {
					position_ *= slip_gain;
				}
			} else {
				if(d < 0.0f) {
					if(position_ < d) {
						position_ -= d;
						position_ *= damping;
						position_ += d;
						speed_ = 0.0f;
///						if(position_ >= limit) {
							position_ = limit;
///						}
					} else if(position_ >= 0.0f) {
						position_ *= damping;
						speed_ = 0.0f;
						if(position_ < 0.5f) {
							position_ = 0.0f;
						}
					} else {
						if(scr != 0) {
							position_ += static_cast<float>(scr);
							if(position_ < d) {
								position_ = d;
							} else if(position_ > 0.0f) {
								position_ = 0.0f;
							}
						}
					}
				} else {
///					position_ *= damping;
///					if(-0.5f < position_ && position_ < 0.5f) {
///						position_ = 0.0f;
///						speed_ = 0.0f;
///					}
				}
			}
			return static_cast<int32_t>(position_);
		}
	};
}
