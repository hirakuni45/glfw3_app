#pragma once
//=========================================================================//
/*!	@file
	@brief	縮小ビットセット・テンプレート
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2016 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=========================================================================//
#include <cstdint>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	縮小ビットセット・テンプレート
		@param[in]	T	基本型
		@param[in]	R	ビット位置型（enum class）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T, typename R>
	class bitset {
		T	bits_;

	public:
		typedef T value_type;  ///< 基本型
		typedef R index_type;  ///< ビット位置型

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		bitset() : bits_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	全てクリア（リセット）
		*/
		//-----------------------------------------------------------------//
		void clear() { bits_ = 0; }


		//-----------------------------------------------------------------//
		/*!
			@brief	セット
			@param[in]	type	ビット位置
		*/
		//-----------------------------------------------------------------//
		void set(R type) { bits_ |= 1 << static_cast<T>(type); }


		//-----------------------------------------------------------------//
		/*!
			@brief	リセット
			@param[in]	type	ビット位置
		*/
		//-----------------------------------------------------------------//
		void reset(R type) { bits_ &= ~(1 << static_cast<T>(type)); }


		//-----------------------------------------------------------------//
		/*!
			@brief	取得
			@param[in]	type	ビット位置
			@return ビット位置の値
		*/
		//-----------------------------------------------------------------//
		bool get(R type) const {
			return (bits_ & (1 << static_cast<T>(type))) != 0 ? true : false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体設定
			@param[in]	bits	全体値
		*/
		//-----------------------------------------------------------------//
		void preset(T bits) { bits_ = bits; }


		//-----------------------------------------------------------------//
		/*!
			@brief	全体を取得
			@return 全体値
		*/
		//-----------------------------------------------------------------//
		T operator() () const { return bits_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	コピー・オペレーター
			@return 自分
		*/
		//-----------------------------------------------------------------//
		bitset& operator = (const bitset& base) {
			bits_ = base.bits_;
			return *this;
		}
	};
}