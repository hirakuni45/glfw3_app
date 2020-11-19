#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル・入出力クラス @n
			※ FatFs のラッパー（ff14 以降が必要） @n
			※ FatFs のファイル操作系をラップして fopen ぽい機能を提供する。@n
			※ fopen と違って、バッファリング（キャッシュ）されない。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018, 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#ifndef FAT_FS
#  error "file_io.hpp requires FAT_FS to be defined and include FATFS module"
#endif

#include "common/string_utils.hpp"
#include "common/format.hpp"
#include "common/dir_list.hpp"

// カレントパスの管理を自前で行う場合
#if FF_FS_EXFAT > 0
#define MANAGE_CURRENT_PATH_
#endif

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル入出力クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class _>
	class file_io_ {

		static const uint32_t COPY_TMP_SIZE = 512;	///< コピーを行う場合のテンポラリサイズ

	public:

#ifdef MANAGE_CURRENT_PATH_
		static const uint32_t PATH_MAX_SIZE = 256 + 1;			///< パスの最大サイズ
#else
		static const uint32_t PATH_MAX_SIZE = FF_MAX_LFN + 1;	///< パスの最大サイズ
#endif

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        /*!
            @brief  SEEK タイプ
        */
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        enum class SEEK {
			SET,	///< 先頭からのオフセット
			CUR,	///< 現在位置からのオフセット
			END		///< 終端からのオフセット
        };


        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        /*!
            @brief  ファイルサイズタイプ
        */
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef FSIZE_t FSIZE;

	private: 
		FIL			fp_;
		bool		open_;
		bool		error_;

		struct dir_list_t {
			bool		ll_;
			uint16_t	count_;
			uint16_t	block_;
			uint16_t	limit_;
		};

		///< 標準的、ディレクトリーリスト
		static void dir_list_func_(const char* name, const FILINFO* fi, bool dir, void* option)
		{
			if(fi == nullptr) return;

			auto p = static_cast<dir_list_t*>(option);

			if(p->ll_) {
				time_t t = str::fatfs_time_to(fi->fdate, fi->ftime);
				struct tm *m = localtime(&t);
				if(dir) {
					format("           ");
				} else {
					format("%10d ") % fi->fsize;
				}
				format("%s %2d %4d %02d:%02d ") 
					% get_mon(m->tm_mon)
					% static_cast<int>(m->tm_mday)
					% static_cast<int>(m->tm_year + 1900)
					% static_cast<int>(m->tm_hour)
					% static_cast<int>(m->tm_min);
				if(dir) {
					format("%c[34m") % 0x1b;
					format("/");
				} else {
					format("%c[37m") % 0x1b;
					format(" ");
				}
				format("%s\n") % name;
				format("%c[37m") % 0x1b;
			} else {
				auto l = utils::str::utf8_string_length(name);
				if(dir) ++l;
				++l;  // space
				if((p->count_ + l) >= p->limit_) {
					format("\n");
					p->count_ = 0;
				}
				if(dir) {
					format("%c[34m") % 0x1b;
				} else {
					format("%c[37m") % 0x1b;
				}
				format("%s") % name;
				if(dir) format("/");
				format(" ");
				p->count_ += l;
				auto d = p->limit_ - p->count_;
				if(d < p->block_) {
					format("%c[37m") % 0x1b;
					format("\n");
					p->count_ = 0;
				} else {
					auto block = p->block_;
					while(l > block) { block += p->block_; }
					while(l < block) {
						format(" ");
						++p->count_;
						++l;
					}
				}
			}
		}


		///< 標準的、コピー用
		static void dir_copy_func_(const char* name, const FILINFO* fi, bool dir, void* option)
		{
			if(fi == nullptr) return;
		}


		struct copy_t {
			uint32_t	count_;
			uint32_t	match_;
			char*		dst_;
			uint32_t	dstlen_;
		};

		static void path_copy_func_(const char* name, const FILINFO* fi, bool dir, void* option)
			noexcept
		{
			copy_t* t = static_cast<copy_t*>(option);
			if(t->count_ == t->match_) {
				if(t->dst_ != nullptr && t->dstlen_ > 0) {
					str::strncpy_(t->dst_, name, t->dstlen_);
					if(dir) {
						std::strncat(t->dst_, "/", t->dstlen_);
					}
				}
			}
			++t->count_;
		}

#ifdef MANAGE_CURRENT_PATH_
		static char current_path_[PATH_MAX_SIZE];
#endif

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		file_io_() noexcept :
			fp_(),
			open_(false), error_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~file_io_() { close(); }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ディレクトリーの作成
			@param[in]	path	相対パス、又は、絶対パス
			@return 成功なら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool mkdir(const char* path) noexcept
		{
			return f_mkdir(path) == FR_OK;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ファイルの削除
			@param[in]	path	相対パス、又は、絶対パス
			@return 削除成功なら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool remove(const char* path) noexcept
		{
			return f_unlink(path) == FR_OK;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ファイル名の変更
			@param[in]	org_path	元名
			@param[in]	new_path	新名
			@return 成功なら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool rename(const char* org_path, const char* new_path) noexcept
		{
			return f_rename(org_path, new_path) == FR_OK;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	カレント・パスを取得
			@param[in]	dst		パス格納
			@param[in]	len		格納サイズ
			@return 取得出来たら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool pwd(char* dst, uint32_t len) noexcept {
			if(dst == nullptr) {
				return false;
			}
#ifdef MANAGE_CURRENT_PATH_
			str::strncpy_(dst, current_path_, len);
			return true;
#else
			auto ret = f_getcwd(dst, len);
			if(ret == FR_OK) {
				return true;
			} else {
				dst[0] = 0;
				return false;
			}
#endif
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	フル・パスを取作成
			@param[in]	base	ベース名
			@param[in]	dst		格納先
			@param[in]	len		格納先サイズ
			@return 取得出来たら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool make_full_path(const char* base, char* dst, uint32_t len) noexcept {
			if(base == nullptr || dst == nullptr) return false;

			dst[0] = 0;

#ifdef MANAGE_CURRENT_PATH_
			if(strcmp(base, "..") == 0) {
				const char* p = strrchr(current_path_, '/');
				if(p != nullptr) {
					if(current_path_ == p) {
						current_path_[1] = 0;
					} else {
						current_path_[p - current_path_] = 0;
					}
					str::strncpy_(dst, current_path_, len);
				}
				return true;
			} else 
#endif
			if(base[0] == '/') {
				str::strncpy_(dst, base, len);
			} else {
				if(!pwd(dst, len)) {
					return false;
				}
				auto l = strlen(dst);
				if(l == 0) {
					dst[l] = '/';
					++l;
				} else if(l > 1) {
					if(strlen(base) > 0) {
						dst[l] = '/';
						++l;
					}
				}
				str::strncpy_(&dst[l], base, len - l);
			}
			return true;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	カレント・パスの移動
			@param[in]	path	相対パス、又は、絶対パス
			@return 移動成功なら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool cd(const char* path) noexcept
		{
			if(path == nullptr) return false;

#ifdef MANAGE_CURRENT_PATH_
			// 絶対パスに変換
			char tmp[PATH_MAX_SIZE];
			make_full_path(path, tmp, PATH_MAX_SIZE);

			if(!is_directory(tmp)) {
				return false;
			}

			str::strncpy_(current_path_, tmp, PATH_MAX_SIZE);
#endif
			auto ret = f_chdir(path);
			return ret == FR_OK;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルを開く
			@param[in]	filename	ファイル名
			@param[in]	mode		オープン・モード
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const char* filename, const char* mode) noexcept
		{
			if(filename == nullptr || mode == nullptr) return false;

			BYTE mdf = 0;
			if(strchr(mode, 'r') != nullptr) {
				mdf = FA_READ | FA_OPEN_EXISTING;
			}
			if(strchr(mode, 'w') != nullptr) {
				mdf = FA_WRITE;
//				mdf |= FA_CREATE_NEW;  // 上書き禁止
				mdf |= FA_CREATE_ALWAYS;
			}
//			else if(rwm == O_RDWR) mode = FA_READ | FA_WRITE;
			if(strchr(mode, 'a') != nullptr) {
				mdf |= FA_OPEN_APPEND;
			}

//			char tmp[PATH_MAX_SIZE];
//			if(!make_full_path(filename, tmp, PATH_MAX_SIZE)) {
//				return false;
//			}

			FRESULT res = f_open(&fp_, filename, mdf);
			if(res != FR_OK) {
				return false;
			}
			open_ = true;
			error_ = false;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・ディスクリプタへの参照
			@return ファイル・ディスクリプタ
		*/
		//-----------------------------------------------------------------//
		FIL& at() noexcept { return fp_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オープンの確認
			@return オープンなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_open() const noexcept { return open_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリかどうか @n
					※ファイルがオープンしていない場合「false」を返す。
			@return ディレクトリなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_directory() const noexcept {
			if(!open_) return false;
			return (fp_.obj.attr & AM_DIR) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをクローズする
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool close() noexcept
		{
			if(!open_) {
				return false;
			}
			open_ = false;
			return f_close(&fp_) == FR_OK;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リード
			@param[out]	dst		読込先
			@param[in]	num		読み込みサイズ
			@return 読み込みサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t read(void* dst, uint32_t len) noexcept
		{
			if(!open_) return 0; 

			UINT rl = 0;
			FRESULT res = f_read(&fp_, dst, len, &rl);
			if(res != FR_OK) {
				error_ = true;
				return 0;
			}
			return rl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リード
			@param[out]	dst		読込先
			@param[in]	block	ブロックサイズ
			@param[in]	num		個数
			@return 読み込みサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t read(void* dst, uint32_t block, uint32_t num) noexcept
		{
			return read(dst, block * num) / block;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	１文字取得
			@param[out]	ch	文字（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_char(char& ch) noexcept
		{
			char tmp[1];
			if(read(tmp, 1) != 1) {
				return false;
			}
			ch = tmp[0];
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライト
			@param[in]	src		ソース
			@param[in]	num		書き込みサイズ
			@return 書き込みサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t write(const void* src, uint32_t len) noexcept
		{
			if(!open_) return 0; 

			UINT wl = 0;
			FRESULT res = f_write(&fp_, src, len, &wl);
			if(res != FR_OK) {
				error_ = true;
				return 0;
			}
			return wl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	シーク（fseek 準拠）
			@param[in]	seek	シーク形式
			@param[in]	ofs		オフセット
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool seek(SEEK seek, FSIZE ofs) noexcept
		{
			if(!open_) return false;
			FRESULT ret;
			switch(seek) {
			case SEEK::SET:
				ret = f_lseek(&fp_, ofs);
				break;
			case SEEK::CUR:
				{
					auto pos = tell();
					pos += ofs;
					ret = f_lseek(&fp_, pos);
				}
				break;
			case SEEK::END:
				{
					auto pos = get_file_size();
					pos -= ofs;
					ret = f_lseek(&fp_, pos);
				}
				break;
			default:
				return false;
				break;
			}
			if(ret != FR_OK) {
				error_ = true;
			}
			return ret == FR_OK;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル位置を返す
			@return ファイル位置
		*/
		//-----------------------------------------------------------------//
		FSIZE tell() const noexcept
		{
			if(!open_) return 0;
			return f_tell(&fp_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの終端か検査
			@return ファイルの終端なら「true」
		*/
		//-----------------------------------------------------------------//
		bool eof() const noexcept
		{
			if(!open_) return false;
			return f_eof(&fp_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーの取得
			@return エラーなら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_error() const noexcept { return error_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルサイズを返す
			@return ファイルサイズ
		*/
		//-----------------------------------------------------------------//
		FSIZE get_file_size() const noexcept
		{
			if(!open_) return 0;
			return f_size(&fp_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・フラッシュ
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool flush() noexcept
		{
			if(!open_) return false;

			return f_sync(&fp_) == FR_OK;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ファイルの存在を検査
			@param[in]	filename	ファイル名
			@return ファイルがある場合「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool probe(const char* filename) noexcept
		{
			if(filename == nullptr) return false;

			BYTE mdf = FA_READ | FA_OPEN_EXISTING;

			FIL fil;
			FRESULT res = f_open(&fil, filename, mdf);
			if(res != FR_OK) {
				return false;
			}

			f_close(&fil);

			return true;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ファイルサイズを返す
			@param[in]	filename	ファイル名
			@return ファイルサイズ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static uint32_t get_file_size(const char* filename) noexcept
		{
			if(filename == nullptr) return 0;

			BYTE mdf = FA_READ | FA_OPEN_EXISTING;

			FIL fil;
			FRESULT res = f_open(&fil, filename, mdf);
			if(res != FR_OK) {
				return 0;
			}

			auto sz = f_size(&fil);

			f_close(&fil);

			return sz;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ファイルの更新時間を取得
			@param[in]	filename	ファイル名
			@return ファイルの更新時間（０の場合エラー）
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static time_t get_time(const char* filename) noexcept
		{
			if(filename == nullptr) return 0;

			FILINFO fno;
			if(f_stat(filename, &fno) != FR_OK) {
				return 0;
			}

			return str::fatfs_time_to(fno.fdate, fno.ftime);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ディレクトリかどうか
			@param[in]	path	パス
			@return ディレクトリなら「true」
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool is_directory(const char* path) noexcept
		{
			if(path == nullptr) return false;

#ifdef MANAGE_CURRENT_PATH_
			if(strcmp(path, "/") == 0) return true;
#endif

			FILINFO fno;
			if(f_stat(path, &fno) != FR_OK) {
				return false;
			}
			return (fno.fattrib & AM_DIR) != 0;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	SD カードのディレクトリーをリストする
			@param[in]	root	ルート・パス
			@param[in]	ll		ショート表示の場合「false」
			@return ファイル数
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static uint32_t dir(const char* root, bool ll = true) noexcept
		{
			dir_list dl;
			if(!dl.start(root)) return 0;

			dir_list_t t;
			t.ll_ = ll;
			t.count_ = 0;
			t.block_ = 16;	// １ファイルの表示範囲
			t.limit_ = 80;  // 80 行のターミナルを想定

			do {
				dl.service(10, dir_list_func_, true, &t);
			} while(dl.probe()) ;
			auto n = dl.get_total();

			format("%c[37m") % 0x1b;  // color white

			if(t.ll_) utils::format("Total %d file%s\n") % n % (n > 1 ? "s" : "");
			else {
				if(t.count_ != 0) {
					utils::format("\n");
				}
			}
			return n;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ディレクトリから、ファイル名の取得
			@param[in]	root	ルート・ディレクトリ
			@param[in]	idx		リストのインデックス
			@param[out]	dst		パスのコピー先
			@param[in]	dstlen	コピー先サイズ
			@return 成功なら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool get_dir_path(const char* root, uint32_t idx, char* dst, uint32_t dstlen)
		{
			dir_list dl;
			if(!dl.start(root)) return false;

			copy_t t;
			t.count_ = 0;
			t.match_ = idx;
			t.dst_ = dst;
			t.dstlen_ = dstlen;
			do {
				dl.service(10, path_copy_func_, true, &t);
			} while(dl.probe()) ;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	空き容量の取得
			@param[out]	fspc	フリー・スペース（単位Ｋバイト)
			@param[out]	capa	最大容量（単位Ｋバイト）
			@return 成功なら「true」
		 */
		//-----------------------------------------------------------------//
		static bool get_free_space(uint32_t& fspc, uint32_t& capa) noexcept
		{
			FATFS* fs;
			DWORD nclst = 0;
			if(f_getfree("", &nclst, &fs) == FR_OK) {
				// 全セクタ数と空きセクタ数を計算（５１２バイトセクタ）
				capa = (fs->n_fatent - 2) * fs->csize / 2;
				fspc = nclst * fs->csize / 2;
				return true;
			} else {
				return false;
			}
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ファイルの複製
			@param[in]	org_path	元名
			@param[in]	new_path	新名
			@param[in]	dir			ディレクトリをコピーする場合「true」
			@return 成功なら「true」
		 */
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		static bool copy(const char* org_path, const char* new_path, bool dir = false) noexcept
		{
			if(org_path == nullptr || new_path == nullptr) return false;

			if(dir) {
				if(!is_directory(org_path)) {
					return false;
				}

				char tmp[PATH_MAX_SIZE];
				make_full_path(org_path, tmp, sizeof(tmp));
				dir_list dl;
				if(!dl.start(tmp)) return false;

				do {
//					dl.service(10, dir_copy_func_, true, org_path);
				} while(dl.probe()) ;

				return true;
			} else {
				file_io_<void> forg;
				if(!forg.open(org_path, "rb")) {
					return false;
				}

				file_io_<void> fnew;
				if(!fnew.open(new_path, "wb")) {
					return false;
				}

				uint8_t tmp[COPY_TMP_SIZE];
				auto len = forg.get_file_size();
				uint32_t pos = 0;
				do {
					auto r = forg.read(tmp, sizeof(tmp));
					if(r == 0) break;
					pos += r;
					auto w = fnew.write(tmp, r);
					if(w == 0) break;
				} while(pos < len) ;
				forg.close();
				fnew.close();
				return pos == len;
			}
		}
	};
	typedef file_io_<void> file_io;

#ifdef MANAGE_CURRENT_PATH_
	template <class _> char file_io_<_>::current_path_[file_io_<_>::PATH_MAX_SIZE];
#endif
}
