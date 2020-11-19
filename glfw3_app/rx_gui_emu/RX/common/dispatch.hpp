#pragma once
//=====================================================================//
/*!	@file
	@brief	ディスパッチ・クラス @n
			グループ割り込みの分類を行うサポート用
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/device.hpp"

namespace utils {

	typedef void (*TASK)();		///< 関数呼び出し型

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  dispatch class
		@param[in]	VEC	グループ・ベクター
		@param[in]	NUM	分岐数（最大３２）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template<device::ICU::VECTOR VEC, uint32_t NUM>
	class dispatch {

		TASK		task_[NUM];
		uint32_t	mask_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		dispatch() noexcept : task_{ nullptr }, mask_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズを返す
			@return サイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t size() const noexcept { return NUM; }


		//-----------------------------------------------------------------//
		/*!
			@brief	マスクを返す
			@return マスク
		*/
		//-----------------------------------------------------------------//
		uint32_t get_mask() const noexcept { return mask_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	分岐タスクを登録
			@param[in]	index	インデックス
			@param[in]	task	タスク
			@return 不正なインデックスの場合「false」
		*/
		//-----------------------------------------------------------------//
		bool set_task(uint32_t index, TASK task) noexcept
		{
			if(index < NUM) {
				task_[index] = task;
				if(task != nullptr) {
					mask_ |= 1 << index;
				} else {
					mask_ &= ~(1 << index);
				}
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	分岐タスクを取得
			@param[in]	index	インデックス
			@return 分岐タスク
		*/
		//-----------------------------------------------------------------//
		TASK get_task(uint32_t index) noexcept
		{
			if(index < NUM) {
				return task_[index];
			} else {
				return nullptr;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	分岐タスクを実行
			@param[in]	index	インデックス
		*/
		//-----------------------------------------------------------------//
		void run_task(uint32_t index) noexcept
		{
			if(index < NUM) {
				if(task_[index] != nullptr) {
					(*task_[index])();
				}
			}
		}
	};
}
