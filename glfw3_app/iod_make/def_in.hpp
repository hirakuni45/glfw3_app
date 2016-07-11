#pragma once
//=====================================================================//
/*!	@file
	@brief	設定ファイル入力 @n
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/format.hpp>
#include "utils/string_utils.hpp"
#include "utils/text_edit.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Def　input クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct def_in {


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	レジスター定義
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

	private:

		bool				verbose_;
		utils::text_edit	te_;
		std::string			analize_error_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	verbose	詳細時「true」
		*/
		//-----------------------------------------------------------------//
		def_in(bool verbose = false) : verbose_(verbose) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	設定書式をロード
			@param[in]	file	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& file)
		{
			auto ret = te_.load(file);
			if(ret && verbose_) {
				std::cout << "Input: '" << file << "'" << std::endl;
				std::cout << "lines: " << te_.get_lines() << std::endl;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	解析
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool analize()
 		{
			analize_error_.clear();

			te_.loop([](uint32_t pos, const std::string& line) {
				if(!line.empty() && line[0] == '#') ;
				else {
					auto ss = utils::split_text(line, " \t", "\"'");
					for(const auto& s : ss) {
						if(!s.empty()) {
							// std::cout << boost::format("Code: %d\n") % static_cast<int>(s[0]);
						}
						std::cout << s << ", ";
					}
					std::cout << std::endl;
				}
			} );

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	解析エラーを取得
			@return 解析エラー文字列
		*/
		//-----------------------------------------------------------------//
		std::string get_analize_error() const { return analize_error_; }

	};

}
