//=====================================================================//
/*!	@file
	@brief	デバイス用ラッパークラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <GLFW/glfw3.h>
#include "core/device.hpp"

namespace gl {

	//-----------------------------------------------------------------//
	/*!
		@brief	サービス@n
				サンプリングと状態の作成
		@param[in]	bits	スイッチの状態
		@param[in]	poss	位置情報
	*/
	//-----------------------------------------------------------------//
	void device::service(const bits_t& bits, const locator& poss)
	{
		bits_t b = bits;
		int joy = GLFW_JOYSTICK_1;  // first JOY-STICK
		if(glfwJoystickPresent(joy) == GL_TRUE) {
			int count;
			const unsigned char* hats = glfwGetJoystickHats(joy, &count);
			if((hats[0] & GLFW_HAT_UP) != 0) {
				b.set(key::GAME_UP);
			}
			if((hats[0] & GLFW_HAT_DOWN) != 0) {
				b.set(key::GAME_DOWN);
			}
			if((hats[0] & GLFW_HAT_LEFT) != 0) {
				b.set(key::GAME_LEFT);
			}
			if((hats[0] & GLFW_HAT_RIGHT) != 0) {
				b.set(key::GAME_RIGHT);
			}
//			const float* axes = glfwGetJoystickAxes(joy, &count);
//			for(int i = 0; i < count; ++i) {
//				if(i == 1) {
//					if(axes[i] > 0.5f) b.set(key::GAME_UP);
//					else if(axes[i] < -0.5f) b.set(key::GAME_DOWN);
//				} else if(i == 0) {
//					if(axes[i] > 0.5f) b.set(key::GAME_RIGHT);
//					else if(axes[i] < -0.5f) b.set(key::GAME_LEFT);
//				}
//			}
			const unsigned char* bl = glfwGetJoystickButtons(joy, &count);
			if(count > 16) count = 16;
			for(int i = 0; i < count; ++i) {
				if(bl[i] != 0) b.set(static_cast<key>(static_cast<int>(key::GAME_0) + i));
			}
		}

		b.set(key::STATE_CAPS_LOCK, level_.test(key::STATE_CAPS_LOCK));
		b.set(key::STATE_SCROLL_LOCK, level_.test(key::STATE_SCROLL_LOCK));
			  b.set(key::STATE_NUM_LOCK, level_.test(key::STATE_NUM_LOCK));

		positive_ =  b & ~level_;
		negative_ = ~b &  level_;

		if(positive_.test(key::CAPS_LOCK)) {
			b.flip(key::STATE_CAPS_LOCK);
		}
		if(positive_.test(key::SCROLL_LOCK)) {
			b.flip(key::STATE_SCROLL_LOCK);
		}
		if(positive_.test(key::NUM_LOCK)) {
			b.flip(key::STATE_NUM_LOCK);
		}

		level_ = b;

		locator_ = poss;
	}
}
