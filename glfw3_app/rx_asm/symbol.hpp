#pragma once
//=====================================================================//
/*!	@file
	@brief	Symbol class @n
			Copyright 2020 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <unordered_map>
#include <string>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シンボル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class VALUE>
	class symbol {

		typedef std::string KEY;

		typedef std::unordered_map<KEY, VALUE> MAP;

		MAP		map_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		symbol()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  検索
			@param[in]	sym		シンボル名
			@return 見つかれば「true」
		*/
		//-----------------------------------------------------------------//
		bool find(const char* sym) const
		{
			return map_.find(sym) != map_.end();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  検索
			@param[in]	sym		シンボル名
			@param[out]	out		値の取得
			@return 見つかれば「true」
		*/
		//-----------------------------------------------------------------//
		bool find(const char* sym, VALUE& out) const
		{
			auto it = map_.find(sym);
			if(it != map_.end()) {
				out = it->second;
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  削除
			@param[in]	sym		シンボル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool remove(const char* sym) const
		{
			return map_.erase(sym);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  追加
			@param[in]	sym		シンボル名
			@param[in]	t		関連付ける値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool add(const char* sym, const VALUE& t)
		{
			return map_.emplace(sym, t);
		}
	};
}
