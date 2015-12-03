#pragma once
//=====================================================================//
/*!	@file
	@brief	テキスト編集クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include "utils/file_io.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキスト編集クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct text_edit {

		utils::strings	buff_;

		bool			cr_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		text_edit() : buff_(), cr_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	全体読み込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool read_all(const std::string& file) {
			file_io fin;
			if(!fin.open(file, "rb")) {
				return false;
			}

			buff_.clear();

			std::string s;
			while(fin.get_line(s)) {
				buff_.push_back(s);
			}
			cr_ = fin.is_cr();

			fin.close();
			return true;
		}




		//-----------------------------------------------------------------//
		/*!
			@brief	全体書き込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool write_all(const std::string& file) {
			file_io fout;
			if(!fout.open(file, "wb")) {
				return false;
			}

			for(auto s : buff_) {
				if(!fout.put_line(s, cr_)) {
					break;
				}
			}

			fout.close();
			return true;
		}
	};

}
