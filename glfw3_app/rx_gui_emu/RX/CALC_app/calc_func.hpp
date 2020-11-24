#pragma once
//=====================================================================//
/*! @file
    @brief  電卓関数クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>
#include <cstring>
#include "calc_def.hpp"

namespace utils {

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  CALC FUNC クラス
		@param[in]	NVAL	数値クラス
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class NVAL>
    class calc_func : public calc_def {

		NVAL	pai_;
		NVAL	rad_;
		NVAL	grad_;
		NVAL	c2_;

		ATYPE	atype_;

	public:
		typedef calc_def::NAME NAME;

		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクタ
		*/
		//-------------------------------------------------------------//
		calc_func() noexcept :
			rad_(360), grad_(400), c2_(2),
			atype_(ATYPE::Deg) { }


		//-------------------------------------------------------------//
		/*!
			@brief  角度からラジアンへの変換
			@param[in]	ang		角度（０～３６０）
			@return ラジアン（2 x pai)
		*/
		//-------------------------------------------------------------//
		NVAL conv_rad(const NVAL& in) noexcept
		{
			switch(atype_) {
			case ATYPE::Deg:
				return in / rad_ * c2_ * NVAL::get_pi();
			case ATYPE::Rad:
				return in;
			case ATYPE::Grad:
				return in / grad_ * c2_ * NVAL::get_pi();
			}
		}


		//-------------------------------------------------------------//
		/*!
			@brief  角度タイプの指定
			@param[in]	atype	角度表現
		*/
		//-------------------------------------------------------------//
		void set_atype(ATYPE atype) noexcept { atype_ = atype; }


		//-------------------------------------------------------------//
		/*!
			@brief  角度タイプの取得
			@return	角度表現
		*/
		//-------------------------------------------------------------//
		ATYPE get_atype() const noexcept { return atype_; }


		//-------------------------------------------------------------//
		/*!
			@brief	  関数機能
			@param[in]	name	関数名
			@param[in]	in		入力
			@param[out]	out		出力
			@return 関数機能がマッチしない場合「false」
		*/
		//-------------------------------------------------------------//
		bool operator() (NAME name, const NVAL& in, NVAL& out) noexcept
		{
			switch(name) {
			case NAME::SIN:
				{
					auto a = conv_rad(in);
					out = NVAL::sin(a);
				}
				break;
			case NAME::COS:
				{
					auto a = conv_rad(in);
					out = NVAL::cos(a);
				}
				break;
			case NAME::TAN:
				{
					auto a = conv_rad(in);
					out = NVAL::tan(a);
				}
				break;
			case NAME::SQRT:
				{
					out = NVAL::sqrt(in);
				}
				break;
			case NAME::LOG:
				{
					out = NVAL::log10(in);
				}
				break;
			case NAME::LN:
				{
					out = NVAL::log(in);
				}
				break;


			default:
				return false;
			}
			return true;
		}
	};
}
