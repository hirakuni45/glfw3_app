//=====================================================================//
/*!	@file
	@brief	文字列操作ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "string_utils.hpp"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <windows.h>

namespace utils {

	using namespace std;

	bool string_to_int(const std::string& src, int_vector& dst)
	{
		try {
			string s;
			BOOST_FOREACH(char ch, src) {
				if(ch == ' ') {
					int v = boost::lexical_cast<int>(s);
					dst.push_back(v);  
					s.clear();
				} else {
					s += ch;
				}
			}
			if(!s.empty()) {
				int v = boost::lexical_cast<int>(s);
				dst.push_back(v);
			}
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	bool string_to_float(const std::string& src, float_vector& dst)
	{
		try {
			string s;
			BOOST_FOREACH(char ch, src) {
				if(ch == ' ') {
					float v = boost::lexical_cast<float>(s);
					dst.push_back(v);  
					s.clear();
				} else {
					s += ch;
				}
			}
			if(!s.empty()) {
				float v = boost::lexical_cast<float>(s);
				dst.push_back(v);
			}
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	bool string_to_matrix4x4(const std::string& src, mtx::fmat4& dst)
	{
		float_vector vv;
		if(!string_to_float(src, vv)) {
			return false;
		}
		if(vv.size() == 16) {
			for(int i = 0; i < 16; ++i) {
				int j = ((i & 3) << 2) | ((i >> 2) & 3);
				dst.m[j] = vv[i];
			}
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-8(ucs2) への変換
		@param[in]	src	Shift-JIS ソース
		@param[in]	dst	UTF-8 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool sjis_to_utf8(const std::string& src, std::string& dst)
	{
		if(src.empty()) return false;

		size_t sz = src.size() + 1;
		wchar_t* tmp = new wchar_t[sz];
		int len = mbstowcs(tmp, src.c_str(), sz);
		tmp[len] = 0;
		utf16_to_utf8(tmp, dst);
		delete[] tmp;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-16 への変換
		@param[in]	Shift-JIS src	ソース
		@param[in]	UTF-16 dst	出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool sjis_to_utf16(const std::string& src, wstring& dst)
	{
		if(src.empty()) return false;

		size_t sz = src.size() + 1;
		wchar_t* tmp = new wchar_t[sz];
		int len = mbstowcs(tmp, src.c_str(), sz);
		tmp[len] = 0;
		dst += tmp;
		delete[] tmp;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8(ucs2) から Shift-JIS への変換
		@param[in]	src	UTF8 ソース
		@param[in]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf8_to_sjis(const std::string& src, std::string& dst)
	{
		if(src.empty()) return false;

		wstring stmp;
		utf8_to_utf16(src.c_str(), stmp);
		size_t sz = stmp.size() * 4 + 1;	// 一応４倍分のバイト数確保
		char* dtmp = new char[sz];
		int len = wcstombs(dtmp, stmp.c_str(), sz);
		dtmp[len] = 0;
		dst += dtmp;
		delete[] dtmp;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から Shift-JIS への変換
		@param[in]	src	UTF16 ソース
		@param[in]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf16_to_sjis(const wstring& src, std::string& dst)
	{
		if(src.empty()) return false;

		size_t sz = src.size() * 4 + 1;	// 一応４倍分のバイト数確保
		char* dtmp = new char[sz];
		int len = wcstombs(dtmp, src.c_str(), sz);
		dtmp[len] = 0;
		dst += dtmp;
		delete[] dtmp;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フルパスか、相対パスか検査する
		@param[in]	path	ファイルパス
		@return フル・パスなら「true」
	*/
	//-----------------------------------------------------------------//
	bool probe_full_path(const std::string& path)
	{
		if(path.empty()) return false;

		char ch = path[0];
#ifdef WIN32
		// WIN32 ではドライブレターの検査
		if(path.size() >= 3 && path[1] == ':' &&
			(path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) {
			ch = path[2];
		} else {
			ch = 0;
		}
#endif
		if(ch != 0 && ch == '/') {
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	階層を一つ戻ったパスを得る
		@param[in]	src	ソースパス
		@param[out]	dst	出力パス
		@return エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool previous_path(const std::string& src, std::string& dst)
	{
		if(src.empty()) return false;

		std::string tmp;
		strip_last_of_delimita_path(src, tmp);
		std::string::size_type n = tmp.find_last_of('/');
		if(n == std::string::npos) {
			dst = src;
			return false;
		}
		dst = tmp.substr(0, n);
		// ルートの場合
		if(dst.find('/') == std::string::npos) {
			dst += '/';
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パスを追加
		@param[in]	src	ソースパス
		@param[in]	add	追加パス
		@param[out]	dst	出力パス
		@return エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool append_path(const std::string& src, const std::string& add, std::string& dst)
	{
		if(src.empty() || add.empty()) return false;
		if(add[0] == '/') {	// 新規パスとなる
			if(add.size() > 1) {
				dst = add;
			} else {
				return false;
			}
		} else {
			std::string tmp;
			strip_last_of_delimita_path(src, tmp);
			dst = tmp + '/' + add;
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	デリミタを変換
		@param[in]	src	ソースパス
		@param[in]	org_ch 元のキャラクター
		@param[in]	cnv_ch  変換後のキャラクター
		@param[out]	dst	出力パス
		@return エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool convert_delimiter(const std::string& src, char org_ch, char cnv_ch, std::string& dst)
	{
		char back = 0;
		BOOST_FOREACH(char ch, src) {
			if(ch == org_ch) {
				if(back != 0 && back != cnv_ch) ch = cnv_ch;
			}
			if(back) dst += back;
			back = ch;
		}
		if(back) dst += back;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子フィルター
		@param[in]	src	ソース
		@param[in]	ext	拡張子
		@param[out]	dst	出力
		@param[in]	cap	「false」なら大文字小文字を判定する
	*/
	//-----------------------------------------------------------------//
	void ext_filter_path(const strings& src, const std::string& ext, strings& dst, bool cap)
	{
		strings exts;
		split_text(ext, ",", exts);
		BOOST_FOREACH(const std::string& s, src) {
			std::string src_ext = get_file_ext(s);
			BOOST_FOREACH(const std::string& ex, exts) {
				if(cap) {
					if(no_capital_strcmp(src_ext, ex) == 0) {
						dst.push_back(s);
					}
				} else {
					if(ex == src_ext) {
						dst.push_back(s);
					}
				}
			}
		}
	}

}
