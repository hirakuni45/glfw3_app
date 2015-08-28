#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル I/O ラッパー
	@author	平松邦仁 (hira@bexide.co.jp)
*/
//=====================================================================//
#include <stdint.h>
#include <string>
#include <vector>
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

		finfos	fis_;

		static const uint32_t file_limit_size_ = 1024 * 1024;
		static const uint32_t file_align_size_ = 512;
		
		std::string	base_;

		struct fidx {
			uint32_t	pos_;
			bool		close_;
			fidx() : pos_(0), close_(true) { }
		};
		std::vector<fidx>	fidxes_;

		std::string block_name_(uint16_t nmb) {
			std::string s = base_;
			for(int i = 0; i < 4; ++i) {
				uint16_t n = (nmb >> ((4 - i - 1) * 4)) & 15;
				if(n < 10) s += '0' + n;
				else s += 'A' + n - 10;
			}
			return std::move(s);
		}


#ifdef WIN32
		void falign_(FILE* fp, uint32_t als) {
			uint32_t pos = fseek(fp, 0, SEEK_CUR);
			uint32_t mod = pos % als;
			if(mod) {
				std::vector<uint8_t> tmp;
				if(als - mod) {
					tmp.resize(als - mod);
					fwrite(&tmp[0], als - mod, 1, fp);
				}
			}
		}


		void write_(uint32_t idx, const finfo& fi, bool close) {
			const char* om;
			if(fidxes_[idx].pos_ == 0) {
				om = "wb";  // new file
			} else {
				om = "ab";  // append file
			}
			FILE* fp = fopen(block_name_(idx).c_str(), om);
			if(fp != nullptr) {
				fwrite(&fi.cash_[0], fi.cpos_, 1, fp);
				if(close) falign_(fp, file_align_size_);
				fidxes_[idx].pos_ = ftell(fp);
				fidxes_[idx].close_ = close;
				fclose(fp);
			}
		}
#endif

	public:
		fio(finfos& fis, const std::string& base) : fis_(fis), base_(base) { }

		int file_size(const std::string& path) {
#ifdef WIN32
			FILE* fp = fopen(path.c_str(), "rb");
			if(fp != nullptr) {
				fseek(fp, 0, SEEK_END);
				int sz = ftell(fp);
				fclose(fp);
				return sz;
			}
			return -1;
#endif
		}

		
		void write_cash(finfo& fi, bool close) {
			if(fidxes_.empty()) {
				fidx fx;
				fidxes_.push_back(fx);
			}
			uint32_t idx = 1;
			for(auto fx : fidxes_) {
				uint32_t fsz = file_limit_size_ - fx.pos_;  // free size
				if(fx.close_ && fsz > fi.cpos_) {
					write_(idx, fi, close);
					fi.blocks_.push_back(idx);
					break;
				}
				++idx;
			}
			{
				fidx fx;
				fidxes_.push_back(fx);
				write_(idx, fi, close);
				fi.blocks_.push_back(idx);
			}
		}


		void write_dir(const finfos& fis) {
#ifdef WIN32
			FILE* fp = fopen(block_name_(0).c_str(), "wb");
			if(fp != nullptr) {
				for(auto fi : fis) {
					if(fi.handle_ == 0) continue;
					std::string path = fi.path_;
//					std::cout << path << std::endl;
					path.resize(256, 0);
					fwrite(path.c_str(), path.size(), 1, fp);
					fwrite(&fi.fsize_, sizeof(fi.fsize_), 1, fp);
///					fwrite(&fi.fofs_, sizeof(fi.fofs_), 1, fp);
					uint32_t len = fi.blocks_.size();
					fwrite(&len, sizeof(len), 1, fp);
					fwrite(&fi.blocks_[0], 2, len, fp);
					uint32_t pos = ftell(fp);
					path.clear();
					path.resize(256 - (pos & 255), 0);
					fwrite(path.c_str(), path.size(), 1, fp);
				}
				fclose(fp);
			}
#endif
		}


		void read_dir(finfos& fis) {
			int fs = file_size(block_name_(0));
//			std::cout << fs << std::endl;
#ifdef WIN32
			FILE* fp = fopen(block_name_(0).c_str(), "rb");
			if(fp != nullptr) {
				while(fs > 0) {
					char buff[256];
					fread(buff, 256, 1, fp);
					finfo fi;
					fi.path_ = buff;
//					std::cout << buff << std::endl;
					fread(&fi.fsize_, sizeof(fi.fsize_), 1, fp);
///					fread(&fi.fofs_, sizeof(fi.fofs_), 1, fp);
					uint32_t len;
					fread(&len, sizeof(len), 1, fp);
					fi.blocks_.resize(len);
					fread(&fi.blocks_[0], 2, len, fp);
					uint32_t pos = ftell(fp);
					fseek(fp, 256 - (pos & 255), SEEK_CUR);
					fs -= 512;
					fis.push_back(fi);
				}
				fclose(fp);
			}
#endif
		}

	};
}
