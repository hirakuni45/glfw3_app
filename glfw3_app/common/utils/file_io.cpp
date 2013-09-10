//=====================================================================//
/*!	@file
	@brief	ファイル入出力関連、ユーティリティー@n
			文字列のコード変換など
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/file_io.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#ifdef __PPU__
#include <sys/paths.h>
#include <cell/sysmodule.h>
#include <cell/l10n.h>
#endif

#include <iostream>
#include <boost/format.hpp>

namespace utils {

	using namespace std;

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化(filepathh class)
		@param[in]	path	初期化パスを指定@n
							初期化パスを指定しない場合、アプリケーション@n
							起動パスが設定される。
	*/
	//-----------------------------------------------------------------//
	void fpathman::initialize(const std::string& path)
	{
#ifdef __PPU__
		if(m_init == false) {
			int ret = cellSysmoduleLoadModule(CELL_SYSMODULE_L10N);
			if(ret != CELL_OK) {
				cout << boost::format("module load failed(L10N): 0x%08X") % ret << std::endl;
			}
			m_init = true;
			m_exit = false;
		}
#else
		if(init_ == false) {
			setlocale(LC_ALL, "ja");
			init_ = true;
			exit_ = false;
		}

#if 0
		iconv_t icd = iconv_open("UTF-8", "Shift_JIS");
		if(icd == 0) {
			std::cout << "Can't open iconv..." << std::endl;
			return false;
		}

		size_t srclen = strlen(src);
		size_t dstlen = srclen * 4 + 1;
		char* dst_tmp = new char[dstlen];
		size_t st = iconv(icd, &src, &srclen, &dst_tmp, &dstlen);
		dst_tmp[dstlen] = 0;
		dst += dst_tmp;
		delete[] dst_tmp;
		iconv_close(icd);
#endif

#endif

		if(path.empty()) {
#ifdef WIN32
			static const int blen = 1024;
			wchar_t buff[blen];
			buff[0] = 0;
			_wgetcwd(buff, blen);
			utils::code_conv(utils::wstring(buff), '\\', '/', app_path_);
#else
#ifdef __PPU__
// PS3 ではカレントディレクトリーの概念が無い？
			string s = SYS_APP_HOME;

			sjis_to_utf16(s, app_path_);
#else
			static const int blen = 1024;
			char	cbuff[blen];
			cbuff[0] = 0;
			getcwd(cbuff, blen);
			std::string s;
			utils::code_conv(cbuff, '\\', '/', s);
			utils::sjis_to_utf16(s.c_str(), app_path_);
#endif
#endif
		} else {
			app_path_ = path;
		}

		path_ = app_path_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	カレントのパスを 1 階層前に移動
		@return	エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool fpathman::previous()
	{
		const char* p = strrchr(path_.c_str(), '/');
		if(p == 0) {
			return false;
		}

		path_back_ = path_;

		const char* t = path_.c_str();
		std::string tmp = path_.substr(0, (p - t));
		path_ = tmp;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	カレントのパスを 1 階層前に移動
		@return	エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool fpathman::move(const std::string& new_path)
	{
		if(new_path.empty()) return false;
		path_back_ = path_;
		if(new_path[0] == '/') {
			path_ = new_path;
		} else {
			path_ += '/';
			path_ += new_path;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄(filepathh class)
	*/
	//-----------------------------------------------------------------//
	void fpathman::destroy()
	{
		if(exit_ == false) {
			exit_ = true;
#ifdef __PPU__
			int ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_L10N);
			if(ret != CELL_OK) {
				std::cout << boost::format("module unload failed(L10N): 0x%08X" % ret) << std::endl;
			}
#endif
		}
	}


	/// PS3 向け、マルチバイトの文字変換
	/// ※この関数を使う前にモジュールをロードしておく必要がある。
	/// ※fpathman クラスを作成後、初期化しておく事
#ifdef __PPU__
	static size_t wstrcpy(wchar_t* dst, const wchar_t* src)
	{
		size_t len = 0;
		while((wchar_t wc = *src++) != 0) {
			*dst++ = wc;
			len++;
		}
		*dst++ = 0;
		return len;
	}

	static size_t mbstowcs(wchar_t* dst, const char* src, size_t sz)
	{
		size_t slen = strlen(src);
		size_t u8len;
		SJISstoUTF8s((const uint8_t*)src, &slen, NULL, &u8len);

		uint8_t* utf8 = new uint8_t[u8len + 1];
		L10nResult r = SJISstoUTF8s((const uint8_t*)src, &slen, utf8, &u8len);
		if(r == ConversionOK) {
			utf8[u8len] = 0;
			wstring tmp;
			utf8_to_utf16((const char*)utf8, tmp);
			wstrcpy(dst, tmp.c_str());
		} else {
			wstring tmp;
			utf8_to_utf16(src, tmp);
			wstrcpy(dst, tmp.c_str());
		}
		delete[] utf8;
		return wstrlen(dst);
	}


	static size_t wcstombs(char* dst, const wchar_t* src, size_t sz)
	{
		std::string utf8;
		utf16_to_utf8(src, utf8);
		size_t u8len = strlen(utf8.c_str());
		size_t slen;
		UTF8stoSJISs((const uint8_t*)utf8.c_str(), &u8len, NULL, &slen);

		uint8_t* sjis = new uint8_t[slen + 1];
		L10nResult r = UTF8stoSJISs((const uint8_t*)utf8.c_str(), &u8len, sjis, &slen);
		if(r == ConversionOK) {
			sjis[slen] = 0;
			strcpy(dst, (const char*)sjis);
		} else {
			strcpy(dst, utf8.c_str());
		}
		delete[] sjis;
		return strlen(dst);
	}
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 対応のファイルオープン
		@param[in]	fn	ファイル名
		@param[in]	md	オープンモード
		@return オープンできれば、ファイル構造体のポインターを返す
	*/
	//-----------------------------------------------------------------//
	std::FILE* wfopen(const wstring& fn, const std::string& md)
	{
		std::FILE* fp;
// mingw サポート、UTF-16 対応 fopen

#ifdef WIN32
		utils::wstring wsm;
		utils::utf8_to_utf16(md, wsm);
		fp = _wfopen(fn.c_str(), wsm.c_str());
#else
		std::string s;
		utils::utf16_to_sjis(fn, s);
		std::string m;
		utils::utf16_to_utf8(md, m);
		fp = fopen(s.c_str(), m.c_str());
#endif
#ifndef NDEBUG
		if(fp == 0) {
			std::string s;
			utils::utf16_to_utf8(fn, s);
			std::string tt;
			if(strchr(md.c_str(), 'w')) tt = "output";
			else tt = "input";
			std::cerr << boost::format("Can't open %1% file (file_io::wfopen): '%2%'") % tt % s << std::endl;
		}
#endif
		return fp;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーか調べる
		@param[in]	fn	ファイル名
		@return ディレクトリーなら「true」
	*/
	//-----------------------------------------------------------------//
	bool is_directory(const wstring& fn)
	{
#ifdef WIN32
		struct _stat st;
		if(_wstat(fn.c_str(), &st) == 0) {
			return S_ISDIR(st.st_mode);
		}
#else
		std::string s;
		utf16_to_sjis(fn, s);
		struct stat st;
		if(stat(s.c_str(), &st) == 0) {
			return S_ISDIR(st.st_mode);
		}
#endif
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルの検査（UTF16)
		@param[in]	fn	ファイル名
		@param[in]	dir	「true」ならディレクトリーとして検査
		@return ファイルが有効なら「true」
	*/
	//-----------------------------------------------------------------//
	bool probe_file(const wstring& fn, bool dir)
	{
#ifdef WIN32
		struct _stat st;
		if(::_wstat(fn.c_str(), &st) == 0) {
#else
		std::string s;
		utf16_to_sjis(fn.c_str(), s);
		struct stat st;
		if(stat(s.c_str(), &st) == 0) {
#endif
			if(dir) {
				bool d = S_ISDIR(st.st_mode);
				if(d) return true;
			} else {
				return true;
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１バイト読み出し
		@param[out]	ch	読み込み先
		@return	ファイルの終端なら「false」
	*/
	//-----------------------------------------------------------------//
	bool file_io::get_char(char& ch)
	{
		if(open_ == false) return false;
		if(fp_) {
			int cha = ::fgetc(fp_);
			if(cha != EOF) {
				ch = cha;
				return true;
			} else {
				return false;
			}
		} else if(rbuff_ != 0 && size_ > 0) {
			if(fpos_ < size_) {
				ch = rbuff_[fpos_];
				fpos_++;
				return true;
			} else {
				return false;
			}
		} else return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１バイト書き出し
		@param[out]	c	書き出しデータ
		@return	エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool file_io::put_char(char c)
	{
		if(open_ == false) return false;
		if(fp_) {
			::fputc(c, fp_);
			return true;
		} else {
			wbuff_.push_back(c);
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルサイズを得る
		@return	ファイルのサイズ
	*/
	//-----------------------------------------------------------------//
	long file_io::get_file_size()
	{
		long pos = tell();
		seek(0, seek::end);
		long size = tell();
		seek(pos, seek::set);
		return size;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	1 行読み込み
		@param[out]	buff	読み込み先
		@return	ファイルの終端なら「false」
	*/
	//-----------------------------------------------------------------//
	bool file_io::get_line(std::string& buff)
	{
		if(open_ == false) return false;

		char ch;
		while(get_char(ch) == true) {
			if(ch == 0x0d) ;
			else if(ch == 0x0a) {
				return true;
			} else {
				buff.append(1, ch);
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	エンディアン並べ替え
		@param[in]	ptr	元データ
		@param[in]	size	構造体のサイズ
		@param[in]	list	構造体、個々のイニシャル
	*/
	//-----------------------------------------------------------------//
	void file_io::reorder_memory(void* ptr, size_t size, const char* list)
	{
		if(list == 0) return;

		unsigned char* p = static_cast<unsigned char*>(ptr);
		const char* l = list;
		size_t s = 0;
		while(s < size) {
			char c = *l++;
			switch(c) {
			case 0:
				l = list;
				break;
			case 2:
			case 's':
			case 'S':
				{
					unsigned char l = p[0];
					unsigned char h = p[1];
					*p++ = h;
					*p++ = l;
					s += 2;
				}
				break;
			case 4:
			case 'i':
			case 'I':
			case 'l':
			case 'L':
			case 'f':
			case 'F':
				{
					unsigned char t[4];
					memcpy(p, t, 4);
					for(int i = 0; i < 4; ++i) {
						*p++ = t[4 - 1 - i];
					}
					s += 4;
				}
				break;
			case 8:
			case 'd':
			case 'D':
				{
					unsigned char t[8];
					memcpy(p, t, 8);
					for(int i = 0; i < 8; ++i) {
						*p++ = t[8 - 1 - i];
					}
					s += 4;
				}
				break;
			default:
				++s;
				++p;
				break;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルをメモリー上に全て読み込む
		@param[in]	fin	ファイル入力コンテキスト
		@param[out]	array	アレイ構造
		@param[in]	len	読み込むバイト数（省略する「０」と全て）
		@return 成功すれば「true」
	*/
	//-----------------------------------------------------------------//
	bool read_array(file_io& fin, array_uc& array, size_t len)
	{
		size_t l;
		if(len == 0) l = fin.get_file_size() - fin.tell();
		else l = len;
		array.resize(l);
		if(fin.read(&array[0], l) != l) return false;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	メモリー上のデータを全てファイルに書き込む
		@param[in]	fin	ファイル出力コンテキスト
		@param[in]	array	アレイ構造
		@param[in]	len	書き込むバイト数（省略する「０」と全て）
		@return 成功すれば「true」
	*/
	//-----------------------------------------------------------------//
	bool write_array(file_io& fout, const array_uc& array, size_t len)
	{
		size_t l;
		if(len > 0 && array.size() > len) l = len;
		else l = array.size();
		if(fout.write(&array[0], l) != l) return false;
		return true;
	}

}
