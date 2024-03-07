#pragma once
//=====================================================================//
/*! @file
    @brief  シンプル、固定長、文字 map
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2024 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <stdint.h>
#include <cstring>

namespace utils {

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  固定長文字マップ・クラス
		@param[in]	NBN		文字確保サイズ
		@param[in]	NONE	格納型無効
		@param[in]	ORG		格納型先頭
		@param[in]	END		格納型終端
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint32_t NBN, uint8_t NONE, uint8_t ORG, uint8_t END>
    class fixed_map {

		uint32_t	name_pos_;
		char		name_[NBN];
		uint8_t		id_;

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクタ
		*/
		//-------------------------------------------------------------//
		fixed_map() noexcept :
			name_pos_(0), id_(ORG)
		{ }


		//-------------------------------------------------------------//
		/*!
			@brief  全消去
		*/
		//-------------------------------------------------------------//
		void clear() noexcept
		{
			name_pos_ = 0;
			id_ = ORG;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  照会
			@param[in]	name	名前
			@return ID を返す
		*/
		//-------------------------------------------------------------//
		auto find(const char* name) const noexcept
		{
			if(name == nullptr) return NONE;

			auto l = strlen(name);
			if(NBN < (name_pos_ + l + 1)) {
				return NONE;
			}

			auto id = ORG;
			const auto p = name_;
			while(p < &name_[name_pos_]) {
				if(strcmp(p, name) == 0) {
					return id;
				}
				p += strlen(p);
				++p;
				++id;
			}
			return NONE;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  インサート
			@param[in]	name	名前
			@return ID を返す
		*/
		//-------------------------------------------------------------//
		auto insert(const char* name) noexcept
		{
			if(name == nullptr) return NONE;

			if(find(name) != NONE) {
				return NONE;
			}

			auto l = strlen(name);
			if((name_pos_ + l + 1) >= NBN) {
				return NONE;
			}
			auto p = name_[name_pos_];
			strcpy(p, name);
			p += name;
			*p = 0;
			name_pos_ += l + 1;
			auto id = id_;
			++id_;
			return id;
		}


	};
}
