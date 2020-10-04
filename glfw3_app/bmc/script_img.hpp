#pragma once
//=====================================================================//
/*! @file
	@brief  スクリプト・イメージ・クラス @n
			簡単なスクリプトで、プリミティブを組み合わせて、画像を作成 @n
			Copyright 2020 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include "utils/file_io.hpp"
#include "utils/arith.hpp"
#include "utils/format.hpp"

namespace script {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	イメージ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class image {


	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		image() { }


		//-------------------------------------------------------------//
		/*!
			@brief  スクリプト・ファイルを開く
			@param[in]	file	ファイル名
			@return オープン出来たら「true」
		*/
		//-------------------------------------------------------------//
		bool open(const std::string& file) noexcept
		{
			utils::file_io fin;

			if(!fin.open(file, "rb")) {
				return false;
			}

			uint32_t lineno = 0;
			do {
				++lineno;
				auto lin = fin.get_line();



			} while(!fin.eof()) ;



			fin.close();

			return true;
		}
	};
}
