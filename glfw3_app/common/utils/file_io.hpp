#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル入出力関連、ユーティリティー（ヘッダー）@n
			文字列のコード変換など
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdio>
#include "utils/string_utils.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル・パスなどを扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class fpathman {

		bool			init_;

		std::string		app_path_;
		std::string		path_back_;
		std::string		path_;

		bool			exit_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		fpathman() : init_(false), exit_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~fpathman() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	path	初期化パスを指定@n
								初期化パスを指定しない場合、アプリケーション@n
								起動パスが設定される。
		*/
		//-----------------------------------------------------------------//
		void initialize(const std::string& path = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを 1 階層前に移動
			@return	エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool previous();


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスの移動
			@param[in]	new_path	新しいパス（先頭文字が'/'の場合は絶対パス）
			@return	エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool move(const std::string& new_path);


		//-----------------------------------------------------------------//
		/*!
			@brief	アプリケーションの起動パスを取得する
			@return	utf16(wstring) 文字列出力
		*/
		//-----------------------------------------------------------------//
		const std::string& get_application_path() const { return app_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを取得する（アプリケーション起動パス）
			@return	出力
		*/
		//-----------------------------------------------------------------//
		const std::string& get_path() const { return path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 対応のファイルオープン
		@param[in]	fn	ファイル名
		@param[in]	md	オープンモード
		@return オープンできれば、ファイル構造体のポインターを返す
	*/
	//-----------------------------------------------------------------//
	std::FILE* wfopen(const utils::wstring& fn, const std::string& md);


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーか調べる（UTF16）
		@param[in]	fn	ファイル名
		@return ディレクトリーなら「true」
	*/
	//-----------------------------------------------------------------//
	bool is_directory(const utils::wstring& fn);


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーか調べる（UTF8）
		@param[in]	fn	ファイル名
		@return ディレクトリーなら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool is_directory(const std::string& fn) {
		utils::wstring wfn;
		utils::utf8_to_utf16(fn, wfn);
		return is_directory(wfn);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルの検査（UTF16)
		@param[in]	fn	ファイル名
		@param[in]	dir	「true」ならディレクトリーとして検査
		@return ファイルが有効なら「true」
	*/
	//-----------------------------------------------------------------//
	bool probe_file(const utils::wstring& fn, bool dir = false);


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルの検査
		@param[in]	fn	ファイル名
		@param[in]	dir	「true」ならディレクトリーとして検査
		@return ファイルが有効なら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool probe_file(const std::string& fn, bool dir = false) {
		utils::wstring ws;
		utils::utf8_to_utf16(fn, ws);
		return probe_file(ws, dir);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル入出力・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class file_io {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	seek タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct seek {
			enum type {
				set = SEEK_SET,	///< 先頭からのオフセット
				cur = SEEK_CUR,	///< 現在位置からのオフセット
				end = SEEK_END	///< 終端からのオフセット
			};
		};

	private:
		uint32_t	count_;

		bool	open_;
		bool	file_;

		std::string	fpath_;
		std::string	mode_;

		::FILE*	fp_;

		void*	w_buff_;
		const char*			rbuff_;
		std::vector<char>	wbuff_;

		size_t	fpos_;
		size_t	size_;

		bool	binary_mode_;
		bool	read_mode_;
		bool	write_mode_;
		bool	append_mode_;

		void make_file_mode_(const char* mode) {
			if(::strrchr(mode, 'b')) binary_mode_ = true;
			else binary_mode_ = false;
			if(::strrchr(mode, 'w')) write_mode_ = true;
			else write_mode_ = false;
			if(::strrchr(mode, 'r')) read_mode_ = true;
			else read_mode_ = false;
			if(::strrchr(mode, 'a')) append_mode_ = true;
			else append_mode_ = false;
		}
	public:

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		file_io() : count_(0), open_(false), file_(false),
		  fp_(0), w_buff_(0), rbuff_(0), fpos_(0), size_(0),
		  binary_mode_(true), read_mode_(false), write_mode_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~file_io() { close(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルパスを得る
			@return	パス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_path() const { return fpath_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・オープン
			@param[in]	fileame	ファイル名(wchar_t)
			@param[in]	mode		モード
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& filename, const std::string& mode) {
			if(open_) return false;	///< 既にオープン済み

			file_ = true;
			fpath_ = filename;
			mode_ = mode;

			utils::wstring wfn;
			utf8_to_utf16(fpath_, wfn);

			fp_ = wfopen(wfn, mode);
			if(fp_ == 0) {
				return false;
			} else {
				make_file_mode_(mode.c_str());
				open_ = true;
				++count_;
				return true;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・オープン
			@param[in]	filename	ファイル名(char)
			@param[in]	mode		モード
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const utils::wstring& filename, const std::string& mode) {
			std::string s;
			utils::utf16_to_utf8(filename, s);
			return open(s, mode);
		}



		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・リード・オープン（記憶領域）
			@param[in]	buff	メモリーの先頭
			@param[in]	size	最大サイズ
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const char* buff, size_t size) {
			if(size == 0) return false;
			if(open_) return false;
			if(fp_) return false;

			file_ = false;

			make_file_mode_("rb");

			rbuff_ = buff;

			fpos_ = 0;
			size_ = size;
			open_ = true;

			++count_;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・オープン（記憶領域）
			@param[in]	buff	メモリーの先頭
			@param[in]	size	最大サイズ
			@param[in]	mode	モード
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(void* buff, size_t size, const std::string& mode) {
			if(open_) return false;
			if(fp_) return false;
			mode_ = mode;
			make_file_mode_(mode.c_str());

			file_ = false;

			w_buff_ = buff;

			if(read_mode_) {
				rbuff_ = static_cast<const char*>(buff);
				if(write_mode_) {	// Read/Write (append)
					wbuff_.clear();
					wbuff_.resize(size);
				}
			} else if(write_mode_) {
				wbuff_.clear();
			}
			fpos_ = 0;
			size_ = size;
			open_ = true;

			++count_;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	再オープン
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool re_open() {
			if(open_) return false;

			if(count_ == 0) return false;
 
			if(file_) {
				std::string fn = fpath_;
				std::string md = mode_;
				return open(fn, md);
			} else {
				if(write_mode_) {
					std::string md = mode_;
					return open(w_buff_, size_, md);
				} else {
					return open(rbuff_, size_);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの終端（EOF)を検査
			@return	終端なら「true」
		*/
		//-----------------------------------------------------------------//
		bool eof() const {
			if(fp_) {
				if(feof(fp_)) return true;
				else return false;
			} else {
				if(fpos_ >= size_) return true;
				else return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの現在位置を得る
			@return	ファイル位置
		*/
		//-----------------------------------------------------------------//
		long tell() const {
			if(fp_) {
				return ftell(fp_);
			} else {
				return static_cast<long>(fpos_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの位置を変更する
			@param[in]	offset	オフセット
			@param[in]	stp	シーク・タイプ
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool seek(long offset, seek::type stp) {
			if(fp_) {
				if(fseek(fp_, offset, stp) == 0) return true;
				else return false;
			} else {
				long pos = static_cast<long>(fpos_);
				long limit = static_cast<long>(size_);
				switch(stp) {
				case seek::set:
					pos = offset;
					break;
				case seek::cur:
					pos += offset;
					break;
				case seek::end:
					pos = limit + offset;
					break;
				default:
					return false;
					break;
				}
				if(pos <= limit) {
					fpos_ = static_cast<size_t>(pos);
					return true;
				} else return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	１バイト読み出し
			@param[out]	ch	読み込み先
			@return	ファイルの終端なら「false」
		*/
		//-----------------------------------------------------------------//
		bool get_char(char& ch);


		//-----------------------------------------------------------------//
		/*!
			@brief	複数バイト読み出し
			@param[out]	ptr	読み込み先
			@param[in]	size	オブジェクトのサイズ
			@param[in]	num		オブジェクト数
			@return	読み込んだ数
		*/
		//-----------------------------------------------------------------//
		size_t read(void* ptr, size_t size, size_t num) {
			if(fp_) {
				return fread(ptr, size, num, fp_);
			} else {
				char *p = static_cast<char*>(ptr);
				size_t i;
				for(i = 0; i < (size * num); ++i) {
					char c;
					if(get_char(c) == false) { return i / size; }
					*p++ = c;
				}
				return i / size;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	複数バイト読み出し
			@param[out]	ptr	読み込み先
			@param[in]	size	サイズ
			@return	読み込んだ数
		*/
		//-----------------------------------------------------------------//
		size_t read(void* ptr, size_t size) { return read(ptr, 1, size); }


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列の読み込み
			@param[out]	pad	読み込み先
			@param[in]	size	サイズ（０なら終端文字まで読み込む）
			@param[in]	term	終端文字
			@return	読み込んだ数
		*/
		//-----------------------------------------------------------------//
		size_t get(std::string& pad, size_t size = 0, char term = 0) {
			if(size == 0) {
				size_t n = 0;
				while(1) {
					char ch;
					if(!get_char(ch)) {
						return n;
					}
					if(ch == term) {
						return n;
					}
					pad += ch;
					++n;
				}
			} else {
				for(size_t i = 0; i < size; ++i) {
					char ch;
					if(!get_char(ch)) {
						return i;
					}
					pad += ch;
				}
				return size;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	T の読み込み
			@param[out]	pad	読み込み先
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		template <typename T>
		bool get(T& pad) {
			if(read(&pad, sizeof(T)) != sizeof(T)) {
				return false;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	１バイト書き出し
			@param[in]	c	書き出しデータ
			@return	エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool put_char(char c);


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列の書き出し
			@param[in]	text	書き出し文字列
			@return	書き出した数
		*/
		//-----------------------------------------------------------------//
		size_t put(const std::string& text) {
			if(text.empty()) return 0;
			size_t n = 0;
			BOOST_FOREACH(char ch, text) {
				if(!put_char(ch)) {
					return n;
				}
				++n;
			}
			return n;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	複数バイト書き出し
			@param[in]	ptr	書き出し元
			@param[in]	size	オブジェクトのサイズ
			@param[in]	num		オブジェクト数
			@return	書き出した数
		*/
		//-----------------------------------------------------------------//
		size_t write(const void* ptr, size_t size, size_t num) {
			const char*p = static_cast<const char*>(ptr);
			size_t i;
			for(i = 0; i < (size * num); ++i) {
				char c = *p++;
				if(put_char(c) == false) { return i / size; }
			}
			return i / size;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	複数バイト書き出し
			@param[in]	ptr	書き出し元
			@param[in]	size	サイズ
			@return	書き出した数
		*/
		//-----------------------------------------------------------------//
		size_t write(const void* ptr, size_t size) { return write(ptr, 1,size); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームのフラッシュ
			@return	エラーが無ければ「０」が返る。
		*/
		//-----------------------------------------------------------------//
		int flush() {
			int ret = 0;
			if(open_) {
				if(fp_) {
					ret = ::fflush(fp_);
				}
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルディスクリプタの番号を返す。@n
					※メモリーファイルの場合は「-1」が返る。
			@return	ファイルのサイズ
		*/
		//-----------------------------------------------------------------//
		int file_handle() const {
			int fd = -1;
			if(open_) {
				if(fp_) {
#ifdef __USE_MINGW_ANSI_STDIO
					fd = fp_->_file;
#else
					fd = fileno(fp_);
#endif
				}
			}
			return fd;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルサイズを得る
			@return	ファイルのサイズ
		*/
		//-----------------------------------------------------------------//
		long get_file_size();


		//-----------------------------------------------------------------//
		/*!
			@brief	1 行読み込み
			@param[out]	buff	読み込み先
			@return	ファイルの終端なら「false」
		*/
		//-----------------------------------------------------------------//
		bool get_line(std::string& buff);


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン中か検査する
			@return	オープンなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_open() const { return open_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・クローズ
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool close() {
			if(open_) {
				if(fp_) {
					fclose(fp_);
					fp_ = 0;
				}
				open_ = false;
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エンディアン並べ替え
			@param[in]	ptr	元データ
			@param[in]	size	構造体のサイズ
			@param[in]	list	構造体、個々のイニシャル
		*/
		//-----------------------------------------------------------------//
		static void reorder_memory(void* ptr, size_t size, const char* list);


		//-----------------------------------------------------------------//
		/*!
			@brief	オブジェクトの読み込み
			@param[in]	obj	オブジェクト
			@return	書き出した数
		*/
		//-----------------------------------------------------------------//
		template <class T>
		size_t read(T& obj) {
			return read(&obj, sizeof(T));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オブジェクトの書き出し
			@param[in]	obj	オブジェクト
			@return	書き出した数
		*/
		//-----------------------------------------------------------------//
		template <class T>
		size_t write(const T& obj) {
			return write(&obj, sizeof(T));
		}


	};

	typedef std::vector<unsigned char>	array_uc;

	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルをメモリー上に全て読み込む
		@param[in]	fin	ファイル入力コンテキスト
		@param[out]	array	バイト列
		@param[in]	len	読み込むバイト数（省略する「０」と全て）
		@return 成功すれば「true」
	*/
	//-----------------------------------------------------------------//
	bool read_array(file_io& fin, array_uc& array, size_t len = 0);


	//-----------------------------------------------------------------//
	/*!
		@brief	メモリー上のデータを全てファイルに書き込む
		@param[in]	fin	ファイル出力コンテキスト
		@param[in]	array	バイト列
		@param[in]	len	書き込むバイト数（省略する「０」と全て）
		@return 成功すれば「true」
	*/
	//-----------------------------------------------------------------//
	bool write_array(file_io& fout, const array_uc& array, size_t len = 0);

}

