#pragma once
//=====================================================================//
/*!	@file
	@breif	ビット・アレイ関係
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

		uint32_t	bit_pos_;		///> カレントのビット位置
		uint32_t	bit_limit_;		///> ビットの最大数
		uint8_t		bit_val_;		///> カレントのビット情報（バイト単位）
		std::vector<uint8_t>	bit_list_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	bitio クラス・コンストラクター
		*/
		//-----------------------------------------------------------------//
		bit_array() : bit_pos_(0), bit_limit_(0), bit_val_(0) { };


		//-----------------------------------------------------------------//
		/*!
			@breif	bitio クラス・デストラクター
		*/
		//-----------------------------------------------------------------//
		~bit_array() { };


		//-----------------------------------------------------------------//
		/*!
			@breif	ビット情報を設定して、カレントのビット位置を進める。
			@param[in]	val ビット情報
		*/
		//-----------------------------------------------------------------//
		void put_bit(bool val) {
			if(val == true) {
				bit_val_ |= 1 << (bit_limit_ % 8);
			}
			++bit_limit_;
			if((bit_limit_ % 8) == 0) {
				bit_list_.push_back(bit_val_);
				bit_val_ = 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	任意ビットを設定して、カレントのビット位置を進める。
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
			@breif	カレントのビット情報を得る、カレントのビット位置を進める。
			@return	true の場合は「１」、false の場合は「０」
		*/
		//-----------------------------------------------------------------//
		bool get_bit() {
			if(bit_pos_ >= bit_limit_) {
				return false;
			}
			uint32_t bpos = bit_pos_ % 8;
			uint8_t	c = bit_list_[bit_pos_ >> 3];
			++bit_pos_;
			if(c & (1 << bpos)) return true; else return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	カレントのビット情報を得る、カレントのビット位置を進める。
			@param[in]	num	読み込むビット数
							※最大３２
			@return	読み込んだ値
		*/
		//-----------------------------------------------------------------//
		uint32_t get_bits(int num) {
			uint32_t val = 0;

			for(int i = 0; i < num; ++i) {
				if(get_bit() == true) val |= 1 << i;
			}
			return val;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	カレントのビット位置を変更する。
			@param[in]	pos	ビット位置
		*/
		//-----------------------------------------------------------------//
		void set_pos(unsigned int pos) {
			if(bit_limit_ < pos) pos = bit_limit_ - 1;
			bit_pos_ = pos;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	カレントのビット位置を得る。
			@return	カレントのビット位置
		*/
		//-----------------------------------------------------------------//
		uint32_t get_pos() const { return bit_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@breif	最大ビット位置を得る。
			@return	最大ビット位置
		*/
		//-----------------------------------------------------------------//
		uint32_t get_limit() const { return bit_limit_; };


		//-----------------------------------------------------------------//
		/*!
			@breif	クリア
		*/
		//-----------------------------------------------------------------//
		void clear() { bit_limit_ = bit_pos_ = 0; bit_list_.clear(); }


		//-----------------------------------------------------------------//
		/*!
			@breif		セーブ
			@param[in]	fout	ファイル I/O
			@return		セーブしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t save_file(file_io& fout) {
			// テンポラリーの余剰分を処理（「０」で埋める）
			while((bit_limit_ % 8) != 0) {
				put_bit(false);
			}
			BOOST_FOREACH(uint8_t ch, bit_list_) {
				fout.put_char(ch);
			}
			return bit_list_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@breif		セーブ
			@param[in]	fname ファイル名
			@return		セーブしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t save_file(const std::string& fname) {
			if(fname.empty() || bit_list_.size() == 0) {
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
			@breif		ロード
			@param[in]	fin	ファイル　I/O
			@return		ロードしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t load_file(file_io& fin) {
			bit_list_.reserve(fin.get_file_size());
			bit_list_.clear();
			char ch;
			while(fin.get_char(ch)) {
				bit_list_.push_back(ch);
			}
			bit_limit_ = fin.get_file_size() * 8;
			return fin.get_file_size();
		}


		//-----------------------------------------------------------------//
		/*!
			@breif		ファイルをビット情報としてロード
			@param[in]	fname ファイル名
			@return		ロードしたバイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t load_file(const std::string& fname) {
			if(fname.empty()) return 0;

			bit_list_.clear();

			file_io fin;
			if(!fin.open(fname, "rb")) {
				return 0;
			}

			uint32_t sz = load_file(fin);

			fin.close();
			return sz;
		}
	};
}
