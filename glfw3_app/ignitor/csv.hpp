#pragma once
//=====================================================================//
/*! @file
    @brief  CSV 簡易クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <string>
#include <boost/format.hpp>

#include "utils/string_utils.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  CSV 簡易クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class csv {

		typedef std::vector<utils::strings> CELL;

		CELL	cell_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		csv() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  最大行を返す
			@return 最大行
		*/
		//-----------------------------------------------------------------//
		uint32_t get_rows() const { return cell_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  作成
			@param[in]	columns	列
			@param[in]	rows	行
		*/
		//-----------------------------------------------------------------//
		void create(uint32_t columns, uint32_t rows)
		{
			cell_.clear();
			for(uint32_t r = 0; r < rows; ++r) {
				utils::strings line;
				for(uint32_t c = 0; c < columns; ++c) {
					line.push_back("");
				}
				cell_.push_back(line);
			}
		}

		//-----------------------------------------------------------------//
		/*!
			@brief  列の検索
			@param[in]	key	検索キー
			@param[in]	row	検索行
			@return 位置を返す（見つからない場合「最大列」を返す）
		*/
		//-----------------------------------------------------------------//
		uint32_t find_colum(const std::string& key, uint32_t row) const
		{
			if(row >= cell_.size()) return cell_.size();

			const auto& ss = cell_[row];
			uint32_t n = 0;
			for(const auto& s : ss) {
				if(s == key) {
					return n;
				}
				++n;
			}
			return cell_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  行の検索
			@param[in]	key	検索キー
			@param[in]	colum	検索列
			@return 行の位置を返す（見つからない場合「最大行」を返す）
		*/
		//-----------------------------------------------------------------//
		uint32_t find_rows(const std::string& key, uint32_t colum) const
		{
			uint32_t n = 0;
			for(const auto& ss : cell_) {
				if(colum < ss.size()) {
					if(key == ss[colum]) {
						return n;
					}
				}
				++n;
			}
			return n;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  クリア
		*/
		//-----------------------------------------------------------------//
		void clear()
		{
			cell_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アイテムの設定
			@param[in]	columns	列
			@param[in]	rows	行
			@param[in]	str		文字列
			@return 設定不可なら「false」
		*/
		//-----------------------------------------------------------------//
		bool set(uint32_t columns, uint32_t rows, const std::string& str)
		{
			if(cell_.size() <= rows) return false;

			utils::strings& ss = cell_[rows];
			if(ss.size() <= columns) return false;
			ss[columns] = str;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アイテムの取得
			@param[in]	columns	列
			@param[in]	rows	行
			@return 文字列
		*/
		//-----------------------------------------------------------------//
		std::string get(uint32_t columns, uint32_t rows) const
		{
			if(cell_.size() <= rows) return "";

			const utils::strings& ss = cell_[rows];
			if(ss.size() <= columns) return "";
			return ss[columns];
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	path	セーブ・パス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path)
		{
			utils::file_io fio;
			if(!fio.open(path, "wb")) {
				return false;
			}

			for(uint32_t r = 0; r < cell_.size(); ++r) {
				utils::strings& line = cell_[r];
				for(uint32_t c = 0; c < line.size(); ++c) {
					const std::string& s = line[c];
					fio.put(s);
					if(c < (line.size() - 1)) fio.put(",");
					else if(c == (line.size() - 1)) fio.put("\n");
				}
			}
			fio.close();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	path	ロード・パス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& path)
		{
			utils::file_io fio;
			if(!fio.open(path, "rb")) {
				return false;
			}

			uint32_t lno = 0;
			while(!fio.eof()) {
				auto line = fio.get_line();
				if(line.empty()) continue;
				auto ss = utils::split_text(line, ",");
				uint32_t n = 0;
				for(auto s : ss) {
					set(n, lno, s);
					++n;
				}
				++lno;
			}
			fio.close();

			return true;
		}
	};
}
