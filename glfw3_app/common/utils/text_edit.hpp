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
	template <typename T>
	class basic_text_edit {
	public:
		typedef std::basic_string<T> value_type;
		typedef std::vector<value_type> buffer_type;
		typedef std::function<void (uint32_t, const value_type&)> loop_func;
		typedef std::function<bool (uint32_t, const value_type&, value_type&)> conversion_func;

	private:
		buffer_type	buffer_;

		bool		cr_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		basic_text_edit() : buffer_(), cr_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	全体読み込み
			@param[in]	file	ファイル名
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool load(file_io& fin) {
			buffer_.clear();
			value_type s;
			while(fin.get_line(s)) {
				buffer_.emplace_back(s);
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
		uint32_t get_lines() const { return buffer_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	複数行を挿入
			@param[in]	pos	削除する位置（０から始まる）
			@param[in]	lines	挿入する行
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool insert_lines(uint32_t pos, const buffer_type& lines) {
			for(auto line : lines) {
				if(pos < buffer_.size()) {
					buffer_.insert(buffer_.cbegin() + pos, line);
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
		bool insert_line(uint32_t pos, const value_type& line) {
			if(pos < buffer_.size()) {
				buffer_.insert(buffer_.cbegin() + pos, line);
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
			if(pos < buffer_.size() && last < buffer_.size()) {
				buffer_.erase(buffer_.cbegin() + pos, buffer_.cbegin() + last);
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
			for(const auto s : buffer_) {
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
			uint32_t pos = 0;
			for(auto it = buffer_.begin(); it != buffer_.end(); ++it) {
				value_type tmp;
				if(func(pos, *it, tmp)) {
					*it = tmp;
					++pos;
				}
			}
			return pos;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全体書き込み
			@param[in]	fout	ファイル I/O クラス
			@return 成功なら[true」
		*/
		//-----------------------------------------------------------------//
		bool save(file_io& fout) {
			for(auto s : buffer_) {
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
		const buffer_type& get() const { return buffer_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	テキスト・バッファを参照
			@return テキスト・バッファ
		*/
		//-----------------------------------------------------------------//
		buffer_type& at() { return buffer_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	交換
			@param[in]	ted	ソース
		*/
		//-----------------------------------------------------------------//
		void swap(basic_text_edit<T>& ted) {
			buffer_.swap(ted.buffer_);
			std::swap(cr_, ted.cr_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	= オペレーター
			@param[in]	ted	ソース
			@return 自分を返す
		*/
		//-----------------------------------------------------------------//
		basic_text_edit<T>& operator = (const basic_text_edit<T>& ted) {
			buffer_ = ted.buffer_;
			cr_ = ted.cr_;
			return *this;
		}
	};

	typedef basic_text_edit<char> text_edit;
	typedef basic_text_edit<uint16_t> text_editw;
	typedef basic_text_edit<uint32_t> text_editl;
}
