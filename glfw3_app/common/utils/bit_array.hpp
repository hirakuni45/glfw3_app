#pragma once
//=====================================================================//
/*!	@file
	@brief	ビット・アレイ関係 @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/foreach.hpp>
#include "utils/file_io.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	bit_array クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bit_array {

		uint32_t	put_pos_;		///> put ビット位置
		uint32_t	get_pos_;		///> get ビット位置
		std::vector<uint8_t>	array_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	bitio クラス・コンストラクター
		*/
		//-----------------------------------------------------------------//
		bit_array() : put_pos_(0), get_pos_(0) { };


		//-----------------------------------------------------------------//
		/*!
			@brief	bitio クラス・デストラクター
		*/
		//-----------------------------------------------------------------//
		~bit_array() { };


		//-----------------------------------------------------------------//
		/*!
			@brief	ビット情報を設定して、カレントのビット位置を進める。
			@param[in]	val ビット情報
		*/
		//-----------------------------------------------------------------//
		void put_bit(bool val) {
			if((put_pos_ & 7) == 0) {
				array_.push_back(0);
			}
			if(val) {
				array_[put_pos_ >> 3] |= 1 << (put_pos_ & 7);
			}
			++put_pos_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	任意ビットを設定して、カレントのビット位置を進める。
					※最大３２ビット
			@param[in]	val 設定する値
			@param[in]	num 設定するビット数
		*/
		//-----------------------------------------------------------------//
		void put_bits(uint32_t val, int num) {
			for(int i = 0; i < num; ++i) {
				bool bit;
				if(val & (1 << i)) bit = true; else bit = false;
				put_bit(bit);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのビット情報を得る、カレントのビット位置を進める。
			@return	true の場合は「１」、false の場合は「０」
		*/
		//-----------------------------------------------------------------//
		bool get_bit() {
			if(get_pos_ >= (array_.size() << 3)) {
				return false;
			}
			bool bit = array_[get_pos_ >> 3] & (1 << (get_pos_ & 7));
			++get_pos_;
			return bit;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのビット情報を得る、カレントのビット位置を進める。
			@param[in]	num	読み込むビット数
							※最大３２
			@return	読み込んだ値
		*/
		//-----------------------------------------------------------------//
		uint32_t get_bits(int num) {
			uint32_t bits = 0;

			for(int i = 0; i < num; ++i) {
				if(get_bit() == true) bits |= 1 << i;
			}
			return bits;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	取得用ビットポインターを設定
			@param[in] bpos ビット位置
		*/
		//-----------------------------------------------------------------//
		void set_pos(uint32_t bpos) { get_pos_ = bpos; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズを取得
		*/
		//-----------------------------------------------------------------//
		uint32_t size() const { return put_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	バイトサイズを取得
		*/
		//-----------------------------------------------------------------//
		uint32_t byte_size() const { return (put_pos_ + 7) >> 3; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリア
		*/
		//-----------------------------------------------------------------//
		void clear() { put_pos_ = get_pos_ = 0; array_.clear(); }


		//-----------------------------------------------------------------//
		/*!
			@brief		セーブ
			@param[in]	fout	ファイル I/O
			@return		セーブしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t save_file(file_io& fout) {
			BOOST_FOREACH(uint8_t ch, array_) {
				fout.put_char(ch);
			}
			return array_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		セーブ
			@param[in]	fname ファイル名
			@return		セーブしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t save_file(const std::string& fname) {
			if(fname.empty() || array_.size() == 0) {
				return 0;
			}

			file_io fout;
			if(!fout.open(fname, "wb")) {
				return 0;
			}

			uint32_t sz = save_file(fout);
			fout.close();

			return sz;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		ロード
			@param[in]	fin	ファイル　I/O
			@return		ロードしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t load_file(file_io& fin) {
			array_.reserve(fin.get_file_size());
			array_.clear();
			char ch;
			while(fin.get_char(ch)) {
				array_.push_back(ch);
			}
			return fin.get_file_size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		ファイルをビット情報としてロード
			@param[in]	fname ファイル名
			@return		ロードしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t load_file(const std::string& fname) {
			if(fname.empty()) return 0;

			file_io fin;
			if(!fin.open(fname, "rb")) {
				return 0;
			}

			uint32_t sz = load_file(fin);

			fin.close();
			return sz;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief		バイトアクセス
			@param[in]	idx	バイト位置
			@return		値
		*/
		//-----------------------------------------------------------------//
		uint8_t get_byte(uint32_t idx) const {
			return array_[idx];
		}

	};
}
