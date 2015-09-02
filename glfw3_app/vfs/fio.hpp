#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル I/O ラッパー
	@author	平松邦仁 (hira@bexide.co.jp)
*/
//=====================================================================//
#ifdef WIN32
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#endif
#ifdef __psp2__
#include <stdint.h>
#include <stddef.h>
#include <kernel.h>
#include <apputil.h>
#endif
#include <string>
#include <vector>
#include "finfo.hpp"

extern "C" {
	unsigned int	sceLibcHeapSize	= 1*1024*1024;
};

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
			for (int i = 0; i < 4; ++i) {
				uint16_t n = (nmb >> ((4 - i - 1) * 4)) & 15;
				if (n < 10) s += '0' + n;
				else s += 'A' + n - 10;
			}
			return std::move(s);
		}


#ifdef WIN32
		void init_() {
		}


		int file_size_(const std::string& path) {
			FILE* fp = fopen(path.c_str(), "rb");
			if(fp != nullptr) {
				fseek(fp, 0, SEEK_END);
				int sz = ftell(fp);
				fclose(fp);
				return sz;
			}
			return -1;
		}

		
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
				if(fi.cpos_) fwrite(fi.cash_ptr(), 1, fi.cpos_, fp);
				if(close) file_align_(fp, finfo::file_align_size_);
				fidxes_[idx].pos_ = ftell(fp);
				fidxes_[idx].hnd_ = fi.handle_;
				//				std::cout << static_cast<int>(fidxes_[idx].pos_) << std::endl;
				fclose(fp);
			}
		}


		bool read_(uint32_t idx, finfo& fi) {
			FILE* fp = fopen(block_name_(idx + 1).c_str(), "rb");
			if(fp != nullptr) {
				fseek(fp, fi.fpos_, SEEK_SET);
				uint32_t rl = fi.fsize_ - fi.fpos_;
				if(rl > fi.cash_size()) rl = fi.cash_size();
				fi.cpos_ = fread(fi.cash_ptr(), 1, rl, fp);
				//				std::cout << "Read: " << static_cast<int>(fi.fpos_) << ", " << static_cast<int>(fi.cpos_) << std::endl;
				fclose(fp);
				return true;
			} else {
				return false;
			}
		}

		void remove_file_(const std::string& path) {
			remove(path.c_str());  // for POSIX a remove file
		}


		void write_dir_(const std::string& path, const finfos& fis) {
			FILE* fp = fopen(path.c_str(), "wb");
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
		}


		void read_dir_(const std::string& path, finfos& fis) {
			FILE* fp = fopen(path.c_str(), "rb");
			fseek(fp, 0, SEEK_END);
			int fs = ftell(fp);
			fseek(fp, 0, SEEK_SET);
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
		}

#endif

#ifdef __psp2__
		static const int savedata_slot_ = 0;
		static constexpr const char* main_title_id_ = "SAVETEST";
		static constexpr const char*  sub_title_id_ = "TEST";

		void init_() {
			SceAppUtilInitParam		initParam_;
			SceAppUtilBootParam		bootParam_;
			memset(&initParam_, 0, sizeof(SceAppUtilInitParam) );
			memset(&bootParam_, 0, sizeof(SceAppUtilBootParam) );

			// アプリケーションユーティリティライブラリの初期化処理を行う
			auto ret = sceAppUtilInit(&initParam_, &bootParam_);
			if(ret != SCE_OK) {
				std::cout << "ERROR sceAppUtilInit: " << std::hex << ret << std::dec << std::endl;
			}
		}


		int file_size_(const std::string& path) {
			int fd = sceIoOpen(path.c_str(), SCE_O_RDONLY, 0);
			if(fd < 0) {
				return -1;
			}
			int fs = sceIoLseek32(fd, 0, SCE_SEEK_END);
			sceIoClose(fd);
			return fs;
		}


		void write_sub_(const std::string& path, const void* dataorg, uint32_t dataofs, uint32_t datalen) {
			char sdf[SCE_APPUTIL_MOUNTPOINT_DATA_MAXSIZE + 256];
			memset(sdf, 0, sizeof(sdf));
			strncpy(sdf, path.c_str(), sizeof(sdf));

			{
				SceAppUtilSaveDataSlotParam slotParam;
				memset(&slotParam, 0, sizeof(SceAppUtilSaveDataSlotParam));

				SceInt32 res = sceAppUtilSaveDataSlotGetParam(savedata_slot_, &slotParam, NULL);
				if(res == SCE_OK && slotParam.status == SCE_APPUTIL_SAVEDATA_SLOT_STATUS_BROKEN) {
					SceAppUtilSaveDataDataSlot dataSlot;
					memset(&dataSlot, 0, sizeof(SceAppUtilSaveDataDataSlot));
					dataSlot.id = savedata_slot_;

					SceAppUtilSaveDataDataRemoveItem removeData;
					memset(&removeData, 0, sizeof(removeData));
					removeData.dataPath = (SceChar8*)(sdf);
					removeData.mode     = SCE_APPUTIL_SAVEDATA_DATA_REMOVE_MODE_DEFAULT;

					SceInt32 res = sceAppUtilSaveDataDataRemove(&dataSlot, &removeData, 1, NULL);
					if (res != SCE_OK) {
						std::cout << "ERROR: sceAppUtilSaveDataDataRemove: " << std::hex << res << std::dec << std::endl;
					}
				}
			}

			// initalize savedata parameters
			SceAppUtilSaveDataSlotParam slotParam;
			memset(&slotParam, 0, sizeof(SceAppUtilSaveDataSlotParam));
			strncpy((char*)&slotParam.title,    main_title_id_, SCE_APPUTIL_SAVEDATA_SLOT_TITLE_MAXSIZE-1 );
			strncpy((char*)&slotParam.subTitle, sub_title_id_,  SCE_APPUTIL_SAVEDATA_SLOT_SUBTITLE_MAXSIZE-1 );
			strncpy((char*)&slotParam.detail,   "VFS",          SCE_APPUTIL_SAVEDATA_SLOT_DETAIL_MAXSIZE-1 );
			strncpy((char*)&slotParam.iconPath, "app0:/icon0.png",  SCE_APPUTIL_SAVEDATA_SLOT_ICON_PATH_MAXSIZE-1 );

			// set savedata file slot parameters
			SceAppUtilSaveDataDataSlot saveSlot;
			memset(&saveSlot, 0, sizeof(saveSlot));
			saveSlot.id        = savedata_slot_;
			saveSlot.slotParam = &slotParam;

			// set savedata file parameters
			SceAppUtilSaveDataDataSaveItem saveData;
			memset(&saveData, 0, sizeof(saveData));
			saveData.dataPath = (SceChar8*)sdf;
			saveData.buf      = dataorg;
			saveData.bufSize  = datalen;
			saveData.offset   = dataofs;  // 512 byte align

			SceSize requiredSizeKiB;
			SceInt32 res = sceAppUtilSaveDataDataSave(&saveSlot, &saveData, 1, NULL, &requiredSizeKiB);
			if (res != SCE_OK) {
				std::cout << "ERROR: sceAppUtilSaveDataDataSave: " << std::hex << res << std::dec << std::endl;
			}
		}


		void write_(uint32_t idx, finfo& fi, bool close) {
			auto path = block_name_(idx + 1);

			// アライメントして書き込み
			uint32_t len = fi.cpos_;
			uint32_t mod = len % finfo::file_align_size_;
			if(mod) {
				len += finfo::file_align_size_ - mod;
				std::memset(fi.cash_ptr(fi.cpos_), 0, finfo::file_align_size_ - mod);
			}
			write_sub_(path, fi.cash_ptr(), fidxes_[idx].pos_, len);
			fidxes_[idx].pos_ += len;
			fidxes_[idx].hnd_ = fi.handle_;
		}


		bool read_(uint32_t idx, finfo& fi) {
			std::string path("savedata0:");
			path += block_name_(idx + 1);
			int fd = sceIoOpen(path.c_str(), SCE_O_RDONLY, 0);
			if(fd >= 0) {
				sceIoLseek32(fd, fi.fpos_, SCE_SEEK_SET);
				uint32_t rl = fi.fsize_ - fi.fpos_;
				if(rl > fi.cash_size()) rl = fi.cash_size();
				int rlen = sceIoRead(fd, fi.cash_ptr(), rl);
				if(rlen >= 0) fi.cpos_ = rlen;
				sceIoClose(fd);
				return true;
			}
			return false;
		}


		void remove_file_(const std::string& path) {
			char sdFile[SCE_APPUTIL_MOUNTPOINT_DATA_MAXSIZE + 256];
			memset(sdFile, 0, sizeof(sdFile));
			strncpy(sdFile, path.c_str(), sizeof(sdFile));

			SceAppUtilSaveDataDataSlot dataSlot;
			memset(&dataSlot, 0, sizeof(SceAppUtilSaveDataDataSlot));
			dataSlot.id = savedata_slot_;

			SceAppUtilSaveDataDataRemoveItem removeData;
			memset(&removeData, 0, sizeof(removeData));
			removeData.dataPath = (SceChar8*)(sdFile);
			removeData.mode     = SCE_APPUTIL_SAVEDATA_DATA_REMOVE_MODE_DEFAULT;

			SceInt32 res = sceAppUtilSaveDataDataRemove(&dataSlot, &removeData, 1, NULL);
			if (res != SCE_OK) {
				std::cout << "ERROR: sceAppUtilSaveDataDataRemove: " << std::hex << res << std::dec << std::endl;
			} else {
///				std::cout << "Remove save data: '" << path << "'" << std::endl;
			}
		}


		void write_dir_(const std::string& path, const finfos& fis) {
//			write_sub_(path, fi.cash_ptr(), fidxes_[idx].pos_, len);
			uint32_t i = 0;
			for(auto& fi : fis) {
				if(fi.handle_ == 0) continue;
				std::vector<uint8_t> buff;
				buff.resize(512, uint8_t(0));
				uint32_t pos = 0;
				std::memcpy(&buff[pos], fi.path_.c_str(), fi.path_.size() + 1);
				pos += 256;
				std::memcpy(&buff[pos], &fi.fsize_, sizeof(fi.fsize_));
				pos += sizeof(fi.fsize_);
				uint32_t len = fi.blocks_.size();
				std::memcpy(&buff[pos], &len, sizeof(len));
				pos += sizeof(len);
				std::memcpy(&buff[pos], &fi.blocks_[0], sizeof(finfo::block) * fi.blocks_.size());
//				pos += sizeof(finfo::block) * fi.blocks_.size();
				write_sub_(path, &buff[0], 512 * i, 512);
				++i;
			}
		}


		void read_dir_(const std::string& path, finfos& fis) {
			std::string str("savedata0:");
			str += path;
			int fd = sceIoOpen(str.c_str(), SCE_O_RDONLY, 0);
			if(fd >= 0) {
				int fs = sceIoLseek32(fd, 0, SCE_SEEK_END);
				sceIoLseek32(fd, 0, SCE_SEEK_SET);
				while(fs > 0) {
					char buff[256];
					sceIoRead(fd, buff, 256);
					finfo fi;
					fi.path_ = buff;
//					std::cout << buff << std::endl;
					sceIoRead(fd, &fi.fsize_, sizeof(fi.fsize_));
					uint32_t len;
					sceIoRead(fd, &len, sizeof(len));
					fi.blocks_.resize(len);
					sceIoRead(fd, &fi.blocks_[0], sizeof(finfo::block) * len);
					uint32_t pos = sceIoLseek32(fd, 0, SCE_SEEK_CUR);
					sceIoLseek32(fd, 256 - (pos & 255), SCE_SEEK_CUR);
					fs -= 512;
					fis.push_back(fi);
				}
				sceIoClose(fd);
			}
		}
#endif

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	base	ベース名
		*/
		//-----------------------------------------------------------------//
		fio(finfos& fis, const std::string& base) : fis_(fis), base_(base) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void init() {
			init_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルのサイズを返す
			@param[in]	path	ファイル名
			@return 正常な場合ファイル・サイズ、エラーの場合は「-1」
		*/
		//-----------------------------------------------------------------//
		int file_size(const std::string& path) {
			return file_size_(path);
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
			remove_file_(path);
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
			@return 読み込めたら「true」
		*/
		//-----------------------------------------------------------------//
		bool read_cash(finfo& fi) {
			for(const finfo::block& bk : fi.blocks_) {
				uint32_t top = bk.offset_ * finfo::file_align_size_;
				uint32_t end = top + bk.blocks_ * finfo::file_align_size_;
				if(top <= fi.fpos_ && fi.fpos_ < end) {
					return read_(bk.fileno_, fi);
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリー情報の書き込み
			@param[in]	fis		ファイル情報群
		*/
		//-----------------------------------------------------------------//
		void write_dir(const finfos& fis) {
			auto path = block_name_(0);
			write_dir_(path, fis);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリー情報の読み込み
			@param[in]	fis		ファイル情報群
		*/
		//-----------------------------------------------------------------//
		void read_dir(finfos& fis) {
			auto path = block_name_(0);
			read_dir_(path, fis);
		}

	};
}
