#pragma once
//=====================================================================//
/*!	@file
	@brief	スイッチ・マネージャー・テンプレート
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2016 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	スイッチ・マネージャー・クラス
		@param[in]	BITS	 ビット定義クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class BITS>
	class switch_man {

		BITS	level_;
		BITS	positive_;
		BITS	negative_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	レベルを取得
			@return レベル
		*/
		//-----------------------------------------------------------------//
		const BITS& get_level() const { return level_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	立ち上がりを取得（押した瞬間）
			@return 立ち上がり
		*/
		//-----------------------------------------------------------------//
		const BITS& get_positive() const { return positive_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	立ち下がりを取得（離した瞬間）
			@return 立ち下がり
		*/
		//-----------------------------------------------------------------//
		const BITS& get_negative() const { return negative_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
			@param[in]	level	スイッチ・レベル
		*/
		//-----------------------------------------------------------------//
		void service(const BITS& level)
		{
			positive_.preset( level() & ~level_());
			negative_.preset(~level() &  level_());
			level_ = level;
		}
	};
}
