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

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		text_edit() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	読み込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool read(const std::string& file) {
			file_io fin;
			if(!fin.open(file, "rb")) {
				return false;
			}

			buff_.clear();

			std::string s;
			while(fin.get_line(s)) {
				buff_.push_back(s);
			}

			fin.close();
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	書き込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool write(const std::string& file) {
			file_io fout;
			if(!fout.open(file, "wb")) {
				return false;
			}

			for(auto s : buff_) {
//				fout.put_line(s);
			}

			fout.close();
			return true;
		}


	};

}
