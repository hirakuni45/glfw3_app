#pragma once
//=====================================================================//
/*!	@file
	@brief	spring damper class
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
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

		static constexpr float slip_gain_  = 0.5f;
		static constexpr float damping_    = 0.85f;
		static constexpr float speed_gain_ = 0.95f;	 ///< 速度の減衰
		static constexpr float acc_gain_   = 0.15f;  ///< ドラッグを離した時の加速度ゲイン

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
			if(select_in) {  // 選択開始時
				speed_ = 0.0f;
				offset_ = static_cast<float>(pos);
			}
			float lim = static_cast<float>(limit);
			if(select) {  // 選択中の挙動
				speed_ = static_cast<float>(drag);
				position_ = offset_ + speed_;
				speed_ *= acc_gain_;  // 加速度の調整
				if(position_ > 0.0f) {
					position_ *= slip_gain_;
					speed_ = 0.0f;
				} else if(lim < 0.0f && position_ < lim) {
					position_ -= lim;
					position_ *= slip_gain_;
					position_ += lim;
					speed_ = 0.0f;
				} else if(position_ < 0.0f) {
					position_ *= slip_gain_;
					speed_ = 0.0f;
				}
			} else {
				if(scr != 0) {
					position_ += static_cast<float>(scr);
					if(position_ > 0.0f) {
						position_ = 0.0f;
						speed_ = 0.0f;
					} else if(position_ < lim) {
						position_ = lim;
						speed_ = 0.0f;
					}
				}

				position_ += speed_;
				speed_ *= speed_gain_;
				if(-0.5f < speed_ && speed_ < 0.5f) speed_ = 0.0f; 

				if(position_ > 0.0f) {
					speed_ = 0.0f;
					position_ *= damping_;
					if(position_ < 1.0f) {
						position_ = 0.0f;
					}
				} else if(lim < 0.0f && position_ < lim) {
					speed_ = 0.0f;
					position_ -= lim;
					position_ *= damping_;
					position_ += lim;
					if(position_ > (lim - 1.0f)) {
						position_ = lim;
					}
				} else if(position_ < 0.0f) {
					speed_ = 0.0f;
					position_ *= damping_;
					if(position_ > -1.0f) {
						position_ = 0.0f;
					}
				}
			}
			return static_cast<int32_t>(position_);
		}
	};
}
