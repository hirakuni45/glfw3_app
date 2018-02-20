#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・インターロック・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include "widgets/widget_check.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  インターロック・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class interlock {
	public:
		//=================================================================//
		/*!
			@brief  モジュールの種別
		*/
		//=================================================================//
		enum class swtype {
			N,
			S01,
			S02,
			S03,
			S04,
			S05,
			S06,
			S07,
			S08,
			S09,
			S10,
			S11,
			S12,
			S13,
			S14,
			S15,
			S16,
			S17,
			S18,
			S19,
			S20,
			S21,
			S22,
			S23,
			S24,
			S25,
			S26,
			S27,
			S28,
			S29,
			S30,
			S31,
			S32,
			S33,
			S34,
			S35,
			S36,
			S37,
			S38,
			S39,
			S40,
			S41,
			S42,
			S43,
			S44,
			S45,
			S46,
			S47,
			S48,
			S49,
		};


		// バスの型
		enum class bustype {
			N,
			T1,
			T2,
			T3,
			T4,
			T5,
			T6,
			T7,
			RP,
			RN,
			VP,
			VN,
		};

	private:
		struct interlock_t {
			swtype	swtype_;
			bustype	bustype_;
			gui::widget_check*	check_;
			interlock_t() : swtype_(swtype::N), bustype_(bustype::N), check_(nullptr) { }
		};

		std::vector<interlock_t>	ilocks_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		interlock() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  モジュールのスイッチ登録
			@param[in]	swtype	スイッチ種別
			@param[in]	check	チェック GUI
			@return 登録できたら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_type(swtype swt, gui::widget_check* check)
		{


			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
		}
	};
}
