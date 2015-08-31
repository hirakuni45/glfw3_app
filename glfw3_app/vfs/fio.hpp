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

		std::string	base_;

		struct fidx {
			uint32_t	pos_;
			uint32_t	hnd_;
			fidx() : pos_(0), hnd_(0) { }
			void reset() {
				pos_ = 0;
				hnd_ = 0;
			}
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
		void file_align_(FILE* fp, uint32_t als) {
			uint32_t pos = ftell(fp);
			uint32_t mod = als - (pos % als);
//			std::cout << static_cast<int>(pos) << ", " << static_cast<int>(mod) << std::endl;
			if(mod) {
				std::vector<uint8_t> tmp;
				tmp.resize(mod, 0);
				fwrite(&tmp[0], mod, 1, fp);
			}
		}


		void write_(uint32_t idx, const finfo& fi, bool close) {
			const char* om;
			if(fidxes_[idx].pos_ == 0) {
				om = "wb";  // new file
			} else {
				om = "ab";  // append file
			}
			FILE* fp = fopen(block_name_(idx + 1).c_str(), om);
			if(fp != nullptr) {
				if(fi.cpos_) fwrite(&fi.cash_[0], 1, fi.cpos_, fp);
				if(close) file_align_(fp, finfo::file_align_size_);
				fidxes_[idx].pos_ = ftell(fp);
				fidxes_[idx].hnd_ = fi.handle_;
//				std::cout << static_cast<int>(fidxes_[idx].pos_) << std::endl;
				fclose(fp);
			}
		}


		void read_(uint32_t idx, finfo& fi) {
			FILE* fp = fopen(block_name_(idx + 1).c_str(), "rb");
			if(fp != nullptr) {
				fseek(fp, fi.fpos_, SEEK_SET);
				uint32_t rl = fi.fsize_ - fi.fpos_;
				if(rl > fi.cash_.size()) rl = fi.cash_.size();
				fi.cpos_ = fread(&fi.cash_[0], 1, rl, fp);
//				std::cout << "Read: " << static_cast<int>(fi.fpos_) << ", " << static_cast<int>(fi.cpos_) << std::endl;
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


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの消去
			@param[in]	idx	ファイル・インデックス
		*/
		//-----------------------------------------------------------------//
		void remove_file(uint32_t idx) {
			auto path = block_name_(idx + 1);
//			std::cout << "Remove file: '" << path << "'" << std::endl;
#ifdef WIN32
			remove(path.c_str());
#endif
			fidxes_[idx].reset();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャッシュ・バッファの書き込み
			@param[in]	fi		ファイル情報
			@param[in]	close	ファイル・クローズの場合「true」
		*/
		//-----------------------------------------------------------------//
		void write_cash(finfo& fi, bool close) {
			if(fidxes_.empty()) {
				fidx fx;
				fidxes_.push_back(fx);
			}

			// 追記できるか？
			if(!fi.blocks_.empty()) {
				finfo::block& bk = fi.blocks_.back();
				uint32_t idx = bk.fileno_;
				fidx& fx = fidxes_[idx];
				if(fx.hnd_ == fi.handle_) {
					write_(idx, fi, close);
					uint32_t org = bk.offset_ * finfo::file_align_size_;
					bk.blocks_ = (fx.pos_ - org) / finfo::file_align_size_;
					fi.cpos_ = 0;
//					std::cout << "Append block: " << static_cast<int>(idx) << std::endl;
					return;
				}
			}

			uint32_t idx = 0;
			for(auto& fx : fidxes_) {
				if(fx.pos_ >= finfo::file_limit_size_) ;
				else {
					if((finfo::file_limit_size_ - fx.pos_) > fi.cpos_) {
						uint32_t pos = fx.pos_;
						write_(idx, fi, close);		
						finfo::block bk;
						bk.fileno_ = idx;
						bk.offset_ = pos / finfo::file_align_size_;
						bk.blocks_ = (fx.pos_ - pos) / finfo::file_align_size_;
						fi.blocks_.push_back(bk);
						fi.cpos_ = 0;
						return;
					}
				}
				++idx;
			}

			{ // 新規ブロック
				idx = fidxes_.size();
				fidx fx;
				fidxes_.push_back(fx);
				finfo::block bk;
				uint32_t pos = fx.pos_;
				bk.offset_ = fx.pos_ / finfo::file_align_size_;
				write_(idx, fi, close);
				bk.fileno_ = idx;
				bk.blocks_ = (fx.pos_ - pos) / finfo::file_align_size_;
				fi.blocks_.push_back(bk);
				fi.cpos_ = 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャッシュ・バッファへの読み込み
			@param[in]	fi		ファイル情報
		*/
		//-----------------------------------------------------------------//
		void read_cash(finfo& fi) {
			for(const finfo::block& bk : fi.blocks_) {
				uint32_t top = bk.offset_ * finfo::file_align_size_;
				uint32_t end = top + bk.blocks_ * finfo::file_align_size_;
				if(top <= fi.fpos_ && fi.fpos_ < end) {
					read_(bk.fileno_, fi);
					return;
				}
			}
		}


		void write_dir(const finfos& fis) {
#ifdef WIN32
			FILE* fp = fopen(block_name_(0).c_str(), "wb");
			if(fp != nullptr) {
				for(auto& fi : fis) {
					if(fi.handle_ == 0) continue;
					std::string path = fi.path_;
//					std::cout << path << std::endl;
					path.resize(256, 0);
					fwrite(path.c_str(), path.size(), 1, fp);
					fwrite(&fi.fsize_, sizeof(fi.fsize_), 1, fp);
					uint32_t len = fi.blocks_.size();
					fwrite(&len, sizeof(len), 1, fp);
					fwrite(&fi.blocks_[0], sizeof(finfo::block), len, fp);
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
					uint32_t len;
					fread(&len, sizeof(len), 1, fp);
					fi.blocks_.resize(len);
					fread(&fi.blocks_[0], sizeof(finfo::block), len, fp);
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
