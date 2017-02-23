//=====================================================================//
/*!	@file
	@brief	デバイス用ラッパークラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
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
		int joy = 0;
		if(glfwJoystickPresent(joy) == GL_TRUE) {
			int count;
			const float* axes = glfwGetJoystickAxes(joy, &count);
			for(int i = 0; i < count; ++i) {
				if(i == 1) {
					if(axes[i] > 0.5f) b.set(key::GAME_UP);
					else if(axes[i] < -0.5f) b.set(key::GAME_DOWN);
				} else if(i == 0) {
					if(axes[i] > 0.5f) b.set(key::GAME_RIGHT);
					else if(axes[i] < -0.5f) b.set(key::GAME_LEFT);
				}
			}
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
