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

		std::string	title_;


		void write_(utils::file_io& fio, const std::string& key, const std::string& body) {
			fio.put(key);
			fio.put_char(' ');
			fio.put(body);
			fio.put_char('\n');
		}


		bool decode_(utils::file_io& fio)
		{
			std::string title;
			uint32_t no = 0;
			while(!fio.eof()) {
				++no;
				auto line = fio.get_line();
				if(line.empty()) continue;
				if(line[0] == '#') continue;  // comment line

				auto ss = utils::split_text(line, " ");
				if(ss.size() < 2) {
					return false;
				}
				if(ss[0] == "title") {
					title = ss[1];
				} else {
					return false;
				}
			}

			start(title);
			return true;
		}

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
			@brief  プロジェクト・タイトルを取得
			@return	プロジェクト・タイトル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_title() const { return title_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  新規プロジェクトを開始
			@param[in]	title	プロジェクト名
		*/
		//-----------------------------------------------------------------//
		void start(const std::string& title)
		{
			title_ = title;
			status_ = title.empty() ? false : true;
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
				ret = decode_(fio);
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
				write_(fio, "title", title_);

				fio.close();
			}			
			return ret;
		}
	};
}
