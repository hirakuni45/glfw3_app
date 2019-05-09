#pragma once
//=====================================================================//
/*! @file
	@brief  WAD File クラス @n
			Copyright 2019 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include "utils/file_io.hpp"
#include "utils/format.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	WAD File クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wad_file {
	public:
		struct pad_t {
			uint32_t	ofs;
			uint32_t	len;
			char		name[8];
			pad_t() : ofs(0), len(0), name{ 0 } { }
		};

	private:
		std::string	file_;

		char		sig_[4];
		uint32_t	num_;
		uint32_t	offset_;
		uint32_t	file_size_;

		pad_t get_(file_io& fin) const noexcept
		{
			pad_t t;
			fin.get32(t.ofs);
			fin.get32(t.len);
			fin.read(t.name, 8);
			return t;
		}

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		wad_file() noexcept : sig_{ 0 }, num_(0), offset_(0), file_size_(0) { }


		//-------------------------------------------------------------//
		/*!
			@brief  オープン
			@param[in]	file	ファイル名
			@return エラーなら「false」
		*/
		//-------------------------------------------------------------//
		bool open(const char* file) noexcept
		{
			file_io fin;
			if(!fin.open(file, "rb")) {
				return false;
			}
			fin.read(sig_, 4);
			fin.get32(num_);
			fin.get32(offset_);
			file_size_ = fin.get_file_size();
			fin.close();

			file_ = file;

			return true;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ファイルサイズの取得
			@return ファイルサイズ
		*/
		//-------------------------------------------------------------//
		uint32_t get_file_size() const noexcept { return file_size_; }


		//-------------------------------------------------------------//
		/*!
			@brief  検索
			@param[in]	name	名前
			@param[out]	idx		インデックス
			@param[out]	len		長さ
			@return 成功なら「true」
		*/
		//-------------------------------------------------------------//
		bool find(const char* name, uint32_t& idx, uint32_t& len) const noexcept
		{
			file_io fin;
			if(!fin.open(file_, "rb")) {
				return false;
			}

			fin.seek(offset_, file_io::seek::set);
			for(uint32_t i = 0; i < num_; ++i) {
				auto t = get_(fin);
				if(strncmp(name, t.name, 8) == 0) {
					idx = i;
					len = t.len;
					return true;
				}
			}
			return false;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  リード
			@param[in]	idx		インデックス
			@param[out]	dst		リード先
			@return 成功なら「true」
		*/
		//-------------------------------------------------------------//
		bool read(uint32_t idx, void* dst) const noexcept
		{
			if(dst == nullptr) return false;
			if(idx >= num_) return false;

			file_io fin;
			if(!fin.open(file_, "rb")) {
				return false;
			}

			fin.seek(offset_ + idx * 16, file_io::seek::set);
			auto t = get_(fin);
			fin.seek(t.ofs, file_io::seek::set);
			fin.read(dst, t.len);

			fin.close();

			return true;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  リスト
		*/
		//-------------------------------------------------------------//
		void list() noexcept
		{
			file_io fin;
			if(!fin.open(file_, "rb")) {
				return;
			}

			format("Sig: '%c%c%c%c'\n") % sig_[0] % sig_[1] % sig_[2] % sig_[3];
			format("Num: %u\n") % num_;
			format("Ofs: %u\n") % offset_;

			fin.seek(offset_, file_io::seek::set);
			for(uint32_t i = 0; i < num_; ++i) {
				auto t = get_(fin);
				char tmp[10];
				memcpy(tmp, t.name, 8);
				tmp[8] = 0;
				format("(%d) '%s': %u\n") % i % tmp % t.len;  
			}
			fin.close();
		}
	};
}
