#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル I/O ラッパー
	@author	平松邦仁 (hira@bexide.co.jp)
*/
//=====================================================================//
#include <stdint.h>
#include <string>
#ifdef WIN32
#include <cstdio>
#include <cstdlib>
#endif
#include "finfo.hpp"

namespace vfs {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル I/O クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class fio {

		static const uint32_t file_align_size_ = 512;
		
		std::string	base_;
		uint32_t	idx_;

		std::string hex_to_string_(uint16_t nmb) {
			std::string s;
			for(int i = 0; i < 4; ++i) {
				uint16_t n = (nmb >> ((4 - i - 1) * 4)) & 15;
				if(n < 10) s += '0' + n;
				else s += 'A' + n - 10;
			}
			return s;
		}

	public:
		fio(const std::string& base) : base_(base), idx_(1) { }


		int file_size(const std::string& path) {
#ifdef WIN32
			FILE* fp = fopen(path.c_str(), "rb");
			if(fp != nullptr) {
				int sz = fseek(fp, 0, SEEK_END);
				return sz;
			}
			return -1;
#endif
		}


		void falign_(FILE* fp, uint32_t al) {
#ifdef WIN32
			uint32_t pos = fseek(fp, 0, SEEK_CUR);
			uint32_t mod = pos % al;
			if(mod) {
				std::vector<uint8_t> tmp;
				tmp.resize(mod);
				fwrite(&tmp[0], mod, 1, fp);
			}
#endif
		}

		
		void write_cash(finfo& fi, bool close) {
#ifdef WIN32
			if(fi.idxs_.empty()) {
				auto fn = base_ + hex_to_string_(idx_);
				FILE* fp = fopen(fn.c_str(), "wb");
				if(fp != nullptr) {
					fwrite(&fi.cash_[0], fi.cpos_, 1, fp);
					if(close) falign_(fp, file_align_size_);
					fclose(fp);
					fi.idxs_.push_back(idx_);
					++idx_;
				}
			} else {
				auto fn = base_ + hex_to_string_(fi.idxs_.back());
				int fs = file_size(fn);
				if(fs < 0) {
					// 重大なエラー
					return;
				}
				if((fs + fi.cpos_) <= (1024 * 1024)) {  // 1MB 以内
					FILE* fp = fopen(fn.c_str(), "ab");
					if(fp != nullptr) {
						fwrite(&fi.cash_[0], fi.cpos_, 1, fp);
						if(close) falign_(fp, file_align_size_);
						fclose(fp);
					}
				} else {
					
					
				}
			}



#endif
		}





		void read_dir(finfos& finfos_) {
#ifdef WIN32
			
//			FILE* fp = fopen(path.c_str(), "rb");
//			if(fp != nullptr) {


//			fclose(fp);
#endif
		}


		void write_dir(const finfos& finfos_) {
#ifdef WIN32
//			FILE* fp = fopen(path.c_str(), "wb");
//			if(fp != nullptr) {
//				for(auto fi : finfos_) {
//					auto path = tree_unit_.find(fi.handle_);
					
//				}
//				fclose(fp);
//			}
#endif
		}
	};
}
