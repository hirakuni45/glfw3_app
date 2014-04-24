//=====================================================================//
/*!	@file
	@brief	文字列操作ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/string_utils.hpp"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <windows.h>

namespace utils {

	using namespace std;

	bool string_to_int(const std::string& src, int& dst)
	{
		try {
			dst = boost::lexical_cast<int>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	bool string_to_int(const std::string& src, std::vector<int>& dst)
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


	bool string_to_float(const std::string& src, float& dst)
	{
		try {
			dst = boost::lexical_cast<float>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	bool string_to_float(const std::string& src, std::vector<float>& dst)
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
		std::vector<float> vv;
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
		@brief	UTF-8 から UTF-16 への変換
		@param[in]	src	UTF-8 ソース
		@param[in]	dst	UTF-16 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf8_to_utf16(const std::string& src, wstring& dst)
	{
		if(src.empty()) return false;

		bool f = true;
		int cnt = 0;
		char16_t code = 0;
		BOOST_FOREACH(char tc, src) {
			uint8_t c = static_cast<uint8_t>(tc);
			if(c < 0x80) { code = c; cnt = 0; }
			else if((c & 0xf0) == 0xe0) { code = (c & 0x0f); cnt = 2; }
			else if((c & 0xe0) == 0xc0) { code = (c & 0x1f); cnt = 1; }
			else if((c & 0xc0) == 0x80) {
				code <<= 6;
				code |= c & 0x3f;
				cnt--;
				if(cnt == 0 && code < 0x80) {
					code = 0;	// 不正なコードとして無視
					f = false;
				} else if(cnt < 0) {
					code = 0;
				}
			}
			if(cnt == 0 && code != 0) {
				dst += code;
				code = 0;
			}
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8 から UTF-32 への変換
		@param[in]	src	UTF-8 ソース
		@param[in]	dst	UTF-32 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf8_to_utf32(const std::string& src, lstring& dst)
	{
		if(src.empty()) return false;

		bool f = true;
		int cnt = 0;
		char32_t code = 0;
		BOOST_FOREACH(char tc, src) {
			uint8_t c = static_cast<uint8_t>(tc);
			if(c < 0x80) { code = c; cnt = 0; }
			else if((c & 0xfe) == 0xfc) { code = (c & 0x03); cnt = 5; }
			else if((c & 0xfc) == 0xf8) { code = (c & 0x07); cnt = 4; }
			else if((c & 0xf8) == 0xf0) { code = (c & 0x0e); cnt = 3; }
			else if((c & 0xf0) == 0xe0) { code = (c & 0x0f); cnt = 2; }
			else if((c & 0xe0) == 0xc0) { code = (c & 0x1f); cnt = 1; }
			else if((c & 0xc0) == 0x80) {
				code <<= 6;
				code |= c & 0x3f;
				cnt--;
				if(cnt == 0 && code < 0x80) {
					code = 0;	// 不正なコードとして無視
					f = false;
				} else if(cnt < 0) {
					code = 0;
				}
			}
			if(cnt == 0 && code != 0) {
				dst += code;
				code = 0;
			}
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から UTF-8 への変換
		@param[in]	src	UTF-16 ソース
		@param[in]	dst	UTF-8 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf16_to_utf8(const wstring& src, std::string& dst)
	{
		if(src.empty()) return false;

		bool f = true;
		BOOST_FOREACH(char16_t code, src) {
			if(code < 0x0080) {
				dst += code;
			} else if(code >= 0x0080 && code <= 0x07ff) {
				dst += 0xc0 | ((code >> 6) & 0x1f);
				dst += 0x80 | (code & 0x3f);
			} else if(code >= 0x0800) {
				dst += 0xe0 | ((code >> 12) & 0x0f);
				dst += 0x80 | ((code >> 6) & 0x3f);
				dst += 0x80 | (code & 0x3f);
			} else {
				f = false;
			}
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-32 から UTF-8 への変換
		@param[in]	src	UTF-32 ソース
		@param[in]	dst	UTF-8 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf32_to_utf8(const lstring& src, std::string& dst)
	{
		if(src.empty()) return false;

		bool f = true;
		BOOST_FOREACH(char32_t code, src) {
			if(code < 0x0080) {
				dst += code;
			} else if(code >= 0x0080 && code <= 0x07ff) {
				dst += 0xc0 | ((code >> 6) & 0x1f);
				dst += 0x80 | (code & 0x3f);
			} else if(code >= 0x0800 && code <= 0xffff) {
				dst += 0xe0 | ((code >> 12) & 0x0f);
				dst += 0x80 | ((code >> 6) & 0x3f);
				dst += 0x80 | (code & 0x3f);
			} else if(code >= 0x00010000 && code <= 0x001fffff) {
				dst += 0xf0 | ((code >> 18) & 0x07); 
				dst += 0x80 | ((code >> 12) & 0x3f);
				dst += 0x80 | ((code >> 6) & 0x3f);
				dst += 0x80 | (code & 0x3f);
			} else if(code >= 0x00200000 && code <= 0x03ffffff) {
				dst += 0xF8 | ((code >> 24) & 0x03);
				dst += 0x80 | ((code >> 18) & 0x3f);
				dst += 0x80 | ((code >> 12) & 0x3f);
				dst += 0x80 | ((code >> 6) & 0x3f);
				dst += 0x80 | (code & 0x3f);
			} else if(code >= 0x04000000 && code <= 0x7fffffff) {
				dst += 0xfc | ((code >> 30) & 0x01);
				dst += 0x80 | ((code >> 24) & 0x3f);
				dst += 0x80 | ((code >> 18) & 0x3f);
				dst += 0x80 | ((code >> 12) & 0x3f);
				dst += 0x80 | ((code >> 6) & 0x3f);
				dst += 0x80 | (code & 0x3f);
			} else {
				f = false;
			}
		}
		return f;
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

		uint32_t sz = src.size() + 1;
		wchar_t* tmp = new wchar_t[sz];
		int len = mbstowcs(tmp, src.c_str(), sz);
		wstring ws;
		for(uint32_t i = 0; i < len; ++i) {
			ws += tmp[i];
		}
		utf16_to_utf8(ws, dst);
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

		uint32_t sz = src.size() + 1;
		wchar_t* tmp = new wchar_t[sz];
		int len = mbstowcs(tmp, src.c_str(), sz);
		for(uint32_t i = 0; i < len; ++i) {
			dst += tmp[i];
		}
		delete[] tmp;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8 から Shift-JIS への変換
		@param[in]	src	UTF8 ソース
		@param[in]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf8_to_sjis(const std::string& src, std::string& dst)
	{
		if(src.empty()) return false;

		wstring ws;
		utf8_to_utf16(src, ws);
		wchar_t* stmp = new wchar_t[ws.size() + 1];
		for(uint32_t i = 0; i < ws.size(); ++i) {
			stmp[i] = ws[i];
		}
		stmp[ws.size()] = 0;
		uint32_t sz = ws.size() * 6 + 1;	// 一応6倍分のバイト数確保
		char* dtmp = new char[sz];
		int len = wcstombs(dtmp, stmp, sz);
		if(len >= 0) {
			dtmp[len] = 0;
			dst += dtmp;
		}
		delete[] dtmp;
		delete[] stmp;
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

		wchar_t* stmp = new wchar_t[src.size()];
		for(uint32_t i = 0; i < src.size(); ++i) {
			stmp[i] = src[i];
		}
		uint32_t sz = src.size() * 6 + 1;	// 一応6倍分のバイト数確保
		char* dtmp = new char[sz];
		int len = wcstombs(dtmp, stmp, sz);
		dtmp[len] = 0;
		dst += dtmp;
		delete[] dtmp;
		delete[] stmp;
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
