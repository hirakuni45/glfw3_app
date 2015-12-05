#pragma once
//=====================================================================//
/*!	@file
	@brief	テキスト編集クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include <functional>
#include "utils/file_io.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキスト編集クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct text_edit {
		typedef std::function<void (uint32_t, const std::string&)> loop_func;
		typedef std::function<bool (const std::string&, std::string)> conversion_func;

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
		bool load(file_io& fin) {
			buff_.clear();
			std::string s;
			while(fin.get_line(s)) {
				buff_.push_back(s);
				s.clear();
			}
			cr_ = fin.is_cr();
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体読み込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& file) {
			file_io fin;
			if(!fin.open(file, "rb")) {
				return false;
			}
			auto f = load(fin);
			fin.close();
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	行数を取得
			@return 行数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_lines() const { return buff_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	複数行を挿入
			@param[in]	pos	削除する位置（０から始まる）
			@param[in]	lines	挿入する行
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool insert_lines(uint32_t pos, const utils::strings& lines) {
			for(auto line : lines) {
				if(pos < buff_.size()) {
					buff_.insert(buff_.cbegin() + pos, line);
				} else {
					return false;
				}
				++pos;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	行を挿入
			@param[in]	pos	削除する位置（０から始まる）
			@param[in]	line	挿入する行
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool insert_line(uint32_t pos, const std::string& line) {
			if(pos < buff_.size()) {
				buff_.insert(buff_.cbegin() + pos, line);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	複数行を削除
			@param[in]	pos	削除する位置（０から始まる）
			@param[in]	len	削除する行数
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool delete_lines(uint32_t pos, uint32_t len) {
			uint32_t last = pos + len;
			if(pos < buff_.size() && last < buff_.size()) {
				buff_.erase(buff_.cbegin() + pos, buff_.cbegin() + last);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	行を削除
			@param[in]	pos	削除する位置（０から始まる）
			@param[in]	len	削除する行数（通常１行）
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool delete_line(uint32_t pos) {
			return delete_lines(pos, 1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体ループ
			@param[in]	func	ループ関数
		*/
		//-----------------------------------------------------------------//
		void loop(loop_func func) const {
			uint32_t pos = 0;
			for(const auto s : buff_) {
				func(pos, s);
				++pos;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体変換
			@param[in]	func	変換関数
			@return 変換された数
		*/
		//-----------------------------------------------------------------//
		uint32_t conversion(conversion_func func) {
			uint32_t n = 0;
			for(auto it = buff_.begin(); it != buff_.end(); ++it) {
				std::string tmp;
				if(func(*it, tmp)) {
					*it = tmp;
					++n;
				}
			}
			return n;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体書き込み
			@param[in]	fout	ファイル I/O クラス
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool save(file_io& fout) {
			for(auto s : buff_) {
				if(!fout.put_line(s, cr_)) {
					break;
				}
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体書き込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& file) {
			file_io fout;
			if(!fout.open(file, "wb")) {
				return false;
			}
			auto f =  save(fout);
			fout.close();
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキスト・バッファを参照
			@return テキスト・バッファ
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get() const { return buff_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	テキスト・バッファを参照
			@return テキスト・バッファ
		*/
		//-----------------------------------------------------------------//
		utils::strings& at() { return buff_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	ted	ソース
		*/
		//-----------------------------------------------------------------//
		void swap(text_edit& ted) {
			buff_.swap(ted.buff_);
			std::swap(cr_, ted.cr_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	= オペレーター
			@param[in]	ted	ソース
			@return 自分を返す
		*/
		//-----------------------------------------------------------------//
		text_edit& operator = (const text_edit& ted) {
			buff_ = ted.buff_;
			cr_ = ted.cr_;
			return *this;
		}
	};
}
