//=====================================================================//
/*!	@file
	@brief	デバイス用ラッパークラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <GLFW/glfw3.h>
#include "device.hpp"

namespace gl {

	//-----------------------------------------------------------------//
	/*!
		@brief	サービス@n
				サンプリングと状態の作成
		@param[in]	bits	スイッチの状態
		@param[in]	poss	位置情報
	*/
	//-----------------------------------------------------------------//
	void device::service(const bitsets& bits, const locator& poss)
	{
		bitsets b = bits;
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
				if(bl[i] != 0) b.set(key::GAME_0 + i);
			}
		}

		positive_ =  b & ~level_;
		negative_ = ~b &  level_;
		level_ = b;

		locator_.cursor_ = poss.cursor_;
		locator_.scroll_ = poss.scroll_;		
	}


}
