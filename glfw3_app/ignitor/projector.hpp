#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・プロジェクター・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include "utils/file_io.hpp"

namespace ign {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  イグナイター・プロジェクター・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class projector {

		bool		status_;

		std::string	name_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		projector() : status_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクトが有効か？
			@return 有効な場合「true」
		*/
		//-----------------------------------------------------------------//
		bool status() const { return status_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト名を取得
			@return	プロジェクト名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_name() const { return name_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  新規プロジェクトを開始
			@param[in]	name	プロジェクト名
		*/
		//-----------------------------------------------------------------//
		void start(const std::string& name)
		{
			name_ = name;
			status_ = name.empty() ? false : true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクトをロード
			@param[in]	path	プロジェクトファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& path)
		{
			utils::file_io fio;
			auto ret = fio.open(path, "rb");
			if(ret) {

				fio.close();
			}			
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクトをセーブ
			@param[in]	path	プロジェクトファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path)
		{
			utils::file_io fio;
			auto ret = fio.open(path, "wb");
			if(ret) {

				fio.close();
			}			
			return ret;
		}
	};
}
