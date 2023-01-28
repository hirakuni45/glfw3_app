#pragma once
//=====================================================================//
/*!	@file
	@brief	文字列操作ユーティリティー @n
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "utils/sjis_utf16.hpp"
#include "utils/mtx.hpp"

namespace utils {

	typedef std::string									string;
	typedef std::string::iterator						string_it;
	typedef std::string::const_iterator					string_cit;

	typedef std::basic_string<uint16_t>					wstring;
	typedef std::basic_string<uint16_t>::iterator		wstring_it;
	typedef std::basic_string<uint16_t>::const_iterator	wstring_cit;

	typedef std::basic_string<uint32_t>					lstring;
	typedef std::basic_string<uint32_t>::iterator		lstring_it;
	typedef std::basic_string<uint32_t>::const_iterator	lstring_cit;

	typedef std::vector<std::string>					strings;
	typedef std::vector<std::string>::iterator			strings_it;
	typedef std::vector<std::string>::const_iterator	strings_cit;

	typedef std::vector<wstring>						wstrings;
	typedef std::vector<wstring>::iterator				wstrings_it;
	typedef std::vector<wstring>::const_iterator		wstrings_cit;

	typedef std::vector<lstring>						lstrings;
	typedef std::vector<lstring>::iterator				lstrings_it;
	typedef std::vector<lstring>::const_iterator		lstrings_cit;


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列のサイズ
		@param[in]	src	文字列
		@return 文字数
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline size_t string_strlenT(const T& src) { return src.size(); }
	inline size_t string_strlen(const std::string& src) { return string_strlenT(src); }
	inline size_t string_strlen(const wstring& src) { return string_strlenT(src); }
	inline size_t string_strlen(const lstring& src) { return string_strlenT(src); }


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strchr の string 版
		@param[in]	src	ソース
		@param[in]	ch	探す文字
		@return 見つかればポインターを返す
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline const typename T::value_type* string_strchrT(const T& src, typename T::value_type ch) {
		size_t idx = src.find_first_of(ch);
		if(idx == T::npos) return nullptr;
		else return &src[idx];
	}
	inline const char* string_strchr(const string& src, char ch) { return string_strchrT(src, ch); }
	inline const uint16_t* string_strchr(const wstring& src, uint16_t ch) { return string_strchrT(src, ch); }
	inline const uint32_t* string_strchr(const lstring& src, uint32_t ch) { return string_strchrT(src, ch); }


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strrchr の Xstring 版
		@param[in]	src	ソース
		@param[in]	ch	探す文字
		@return 見つかればポインターを返す
	*/
	//-----------------------------------------------------------------//
	template <class T>
	const typename T::value_type* string_strrchrT(const T& src, typename T::value_type ch) {
		size_t idx = src.find_last_of(ch);
		if(idx == T::npos) return 0;
		else return &src[idx];
	}
	inline const char* string_strrchr(const std::string& src, char ch) { return string_strrchrT(src, ch); }
	inline const uint16_t* string_strrchr(const wstring& src, uint16_t ch) { return string_strrchrT(src, ch); }
	inline const uint32_t* string_strrchr(const lstring& src, uint32_t ch) { return string_strrchrT(src, ch); }


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strcmp の string/wstring 版
		@param[in]	srca 文字列 A
		@param[in]	srcb 文字列 B
		@return strcmp() と同じ比較結果
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline int string_strcmpT(const T& srca, const T& srcb) {
		return srca.compare(srcb);
	}
	inline int string_strcmp(const std::string& srca, const std::string& srcb) {
		return string_strcmpT(srca, srcb); }
	inline int string_strcmp(const wstring& srca, const wstring& srcb) {
		return string_strcmpT(srca, srcb); }
	inline int string_strcmp(const lstring& srca, const lstring& srcb) {
		return string_strcmpT(srca, srcb); }


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strncmp の Xstring 版
		@param[in]	srca 文字列 A
		@param[in]	srcb 文字列 B
		@param[in]	n	比較長さ
		@return strcmp() と同じ比較結果
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline int string_strncmpT(const T& srca, const T& srcb, typename T::size_type n) {
		return srca.compare(0, n, srcb);
	}
	inline int string_strncmp(const std::string& srca, const std::string& srcb, std::string::size_type n) {
		return string_strncmpT(srca, srcb, n); }
	inline int string_strncmp(const wstring& srca, const wstring& srcb, wstring::size_type n) {
		return string_strncmpT(srca, srcb, n); }
	inline int string_strncmp(const lstring& srca, const lstring& srcb, lstring::size_type n) {
		return string_strncmpT(srca, srcb, n); }


	static bool string_to_hex(const std::string& src, uint32_t& dst)
	{
		uint32_t v = 0;
		for(auto ch : src) {
			v <<= 4;
			if(ch >= '0' && ch <= '9') v |= ch - '0';
			else if(ch >= 'A' && ch <= 'F') v |= ch - 'A' + 10;
			else if(ch >= 'a' && ch <= 'f') v |= ch - 'a' + 10;
			else return false;
		}
		dst = v;
		return true;
	}


	static bool string_to_hex(const std::string& src, std::vector<uint32_t>& dst, const std::string& spc = " ,:")
	{
		string s;
		for(auto ch : src) {
			if(string_strchr(spc, ch) != nullptr) {
				uint32_t v;
				if(string_to_hex(s, v)) {
					dst.push_back(v);
					s.clear();
				} else {
					return false;
				}
			} else {
				s += ch;
			}
		}
		if(!s.empty()) {
			uint32_t v;
			if(string_to_hex(s, v)) {
				dst.push_back(v);
			} else {
				return false;
			}
		}
		return true;
	}


	static bool string_to_int(const std::string& src, int32_t& dst)
	{
		try {
			dst = boost::lexical_cast<int32_t>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	static bool string_to_int(const std::string& src, std::vector<int32_t>& dst, const std::string& spc = " ,:")
	{
		try {
			string s;
			for(auto ch : src) {
				if(string_strchr(spc, ch) != nullptr) {
					int32_t v = boost::lexical_cast<int32_t>(s);
					dst.push_back(v);
					s.clear();
				} else {
					s += ch;
				}
			}
			if(!s.empty()) {
				int32_t v = boost::lexical_cast<int32_t>(s);
				dst.push_back(v);
			}
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	static bool string_to_float(const std::string& src, float& dst)
	{
		try {
			dst = boost::lexical_cast<float>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	static bool string_to_float(const std::string& src, std::vector<float>& dst, const std::string& spc = " ,:")
	{
		try {
			string s;
			for(auto ch : src) {
				if(string_strchr(spc, ch) != nullptr) {
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


	static bool string_to_double(const std::string& src, double& dst)
	{
		try {
			dst = boost::lexical_cast<double>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	static bool string_to_matrix4x4(const std::string& src, mtx::fmat4& dst)
	{
		std::vector<float> vv;
		if(!string_to_float(src, vv)) {
			return false;
		}
		if(vv.size() == 16) {
			for(int i = 0; i < 16; ++i) {
				int j = ((i & 3) << 2) | ((i >> 2) & 3);
				dst[j] = vv[i];
			}
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の大文字を小文字に変換
		@param[in]	src	ソース文字列
		@return	変換後の文字列
	*/
	//-----------------------------------------------------------------//
	template <class T>
	T to_lower_text(const T& src) {
		T dst;
		for(auto ch : src) {
			if(ch >= 'A' && ch <= 'Z') {
				dst += (ch + 0x20);
			} else {
				dst += ch;
			}
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	大文字小文字を伴わない文字列の比較
		@param[in]	srca 文字列 A
		@param[in]	srcb 文字列 B
		@return strcmp() と同じ比較結果
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline int no_capital_strcmpT(const T& srca, const T& srcb) {
		T a = to_lower_text(srca);
		T b = to_lower_text(srcb);
		return a.compare(b);
	}
	inline int no_capital_strcmp(const std::string& srca, const std::string& srcb) {
		return no_capital_strcmpT(srca, srcb);
	}
	inline int no_capital_strcmp(const wstring& srca, const wstring& srcb) {
		return no_capital_strcmpT(srca, srcb);
	}
	inline int no_capital_strcmp(const lstring& srca, const lstring& srcb) {
		return no_capital_strcmpT(srca, srcb);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8 から UTF-16 への変換
		@param[in]	src	UTF-8 ソース
		@param[out]	dst	UTF-16（追記）
		@return 変換エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	static bool utf8_to_utf16(const std::string& src, wstring& dst) noexcept
	{
		if(src.empty()) return true;

		bool f = true;
		int cnt = 0;
		uint16_t code = 0;
		for(auto tc : src) {
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
		@brief	UTF-8 から UTF-16 への変換
		@param[in]	src	UTF-8 ソース
		@return	UTF-16
	*/
	//-----------------------------------------------------------------//
	inline wstring utf8_to_utf16(const std::string& src) noexcept {
		wstring dst;
		utf8_to_utf16(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8 から UTF-32 への変換
		@param[in]	src	UTF-8 ソース
		@param[out]	dst	UTF-32（追記）
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool utf8_to_utf32(const std::string& src, lstring& dst) noexcept
	{
		if(src.empty()) return false;

		bool f = true;
		int cnt = 0;
		uint32_t code = 0;
		for(auto tc : src) {
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
		@brief	UTF-8 から UTF-32 への変換
		@param[in]	src	UTF-8 ソース
		@return	UTF-32
	*/
	//-----------------------------------------------------------------//
	inline lstring utf8_to_utf32(const std::string& src) noexcept {
		lstring dst;
		utf8_to_utf32(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から UTF-8 への変換
		@param[in]	src	UTF-16 ソース
		@param[out]	dst	UTF-8（追記）
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool utf16_to_utf8(const wstring& src, std::string& dst) noexcept
	{
		if(src.empty()) return false;

		bool f = true;
		for(auto code : src) {
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
		@brief	UTF-16 から UTF-8 への変換
		@param[in]	src	UTF-16 ソース
		@return	UTF-8
	*/
	//-----------------------------------------------------------------//
	inline std::string utf16_to_utf8(const wstring& src) noexcept {
		std::string dst;
		utf16_to_utf8(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から UTF-32 への変換（単なるコピー）
		@param[in]	src	UTF-16 ソース文字列
		@param[out]	dst	UTF-32（追記）
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool utf16_to_utf32(const wstring& src, lstring& dst) noexcept {
		for(auto ch : src) {
			dst += ch;
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から UTF-32 への変換（単なるコピー）
		@param[in]	src	UTF-16 ソース文字列
		@return	UTF-32
	*/
	//-----------------------------------------------------------------//
	inline lstring utf16_to_utf32(const wstring& src) noexcept {
		lstring dst;
		utf16_to_utf32(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-32 から UTF-8 への変換
		@param[in]	src	UTF-32 ソース
		@param[out]	dst	UTF-8（追記）
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool utf32_to_utf8(const lstring& src, std::string& dst) noexcept
	{
		if(src.empty()) return false;

		bool f = true;
		for(auto code : src) {
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
		@brief	UTF-32 から UTF-8 への変換
		@param[in]	src	UTF-32 ソース
		@return	UTF-8
	*/
	//-----------------------------------------------------------------//
	inline std::string utf32_to_utf8(const lstring& src) noexcept {
		std::string dst;
		utf32_to_utf8(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-8 への変換
		@param[in]	src	Shift-JIS ソース
		@param[out]	dst	UTF-8（追記）
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool sjis_to_utf8(const std::string& src, std::string& dst) noexcept
	{
		if(src.empty()) return false;
		wstring ws;
		uint16_t wc = 0;
		for(auto ch : src) {
			uint8_t c = static_cast<uint8_t>(ch);
			if(wc) {
				if(0x40 <= c && c <= 0x7e) {
					wc <<= 8;
					wc |= c;
					ws += sjis_to_utf16(wc);
				} else if(0x80 <= c && c <= 0xfc) {
					wc <<= 8;
					wc |= c;
					ws += sjis_to_utf16(wc);
				}
				wc = 0;
			} else {
				if(0x81 <= c && c <= 0x9f) wc = c;
				else if(0xe0 <= c && c <= 0xfc) wc = c;
				else ws += sjis_to_utf16(c);
			}
		}
		utf16_to_utf8(ws, dst);
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-8 への変換
		@param[in]	src	Shift-JIS ソース
		@return	UTF-8
	*/
	//-----------------------------------------------------------------//
	inline std::string sjis_to_utf8(const std::string& src) noexcept {
		std::string dst;
		sjis_to_utf8(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-16 への変換
		@param[in]	src	Shift-JIS ソース
		@param[out]	dst	UTF-16（追記）
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool sjis_to_utf16(const std::string& src, wstring& dst) noexcept
	{
		if(src.empty()) return false;
		std::string tmp;
		bool f = sjis_to_utf8(src, tmp);
		if(f) utf8_to_utf16(tmp, dst);
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-16 への変換
		@param[in]	src	Shift-JIS ソース
		@return	UTF-16（追記）
	*/
	//-----------------------------------------------------------------//
	inline wstring sjis_to_utf16(const std::string& src) noexcept {
		wstring dst;
		sjis_to_utf16(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8 から Shift-JIS への変換
		@param[in]	src	UTF8 ソース
		@param[out]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool utf8_to_sjis(const std::string& src, std::string& dst) noexcept
	{
		if(src.empty()) return false;

		wstring ws;
		utf8_to_utf16(src, ws);
		for(auto wc : ws) {
			uint16_t ww = utf16_to_sjis(wc);
			if(ww <= 255) {
				dst += ww;
			} else {
				dst += ww >> 8;
				dst += ww & 0xff;
			}
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8 から Shift-JIS への変換
		@param[in]	src	UTF8 ソース
		@return	Shift-JIS 出力
	*/
	//-----------------------------------------------------------------//
	inline std::string utf8_to_sjis(const std::string& src) noexcept {
		std::string dst;
		utf8_to_sjis(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から Shift-JIS への変換
		@param[in]	src	UTF16 ソース
		@param[out]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	static bool utf16_to_sjis(const wstring& src, std::string& dst) noexcept
	{
		if(src.empty()) return false;

		std::string tmp;
		utf16_to_utf8(src, tmp);
		utf8_to_sjis(tmp, dst);
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から Shift-JIS への変換
		@param[in]	src	UTF16 ソース
		@param[out]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	inline std::string utf16_to_sjis(const wstring& src) noexcept {
		std::string dst;
		utf16_to_sjis(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストの変換
		@param[in]	src		入力文字列リスト
		@return	文字列リスト
	*/
	//-----------------------------------------------------------------//
	inline strings strings_to_strings(const wstrings& src) noexcept {
		strings dst;
		for(const auto& ws : src) {
			auto tmp = utf16_to_utf8(ws);
			dst.push_back(tmp);
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストの変換
		@param[in]	src		入力文字列リスト
		@return	文字列リスト
	*/
	//-----------------------------------------------------------------//
	inline wstrings strings_to_strings(const strings& src) noexcept {
		wstrings dst;
		for(const auto& s : src) {
			auto tmp = utf8_to_utf16(s);
			dst.push_back(tmp);
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストを繋げて、一つの文字列にする。(wstring)
		@param[in]	src		入力文字列リスト
		@param[in]	crlf	改行を挿入する場合「true」
		@return	文字列
	*/
	//-----------------------------------------------------------------//
	inline wstring strings_to_string(const wstrings& src, bool crlf) noexcept {
		wstring dst;
		for(const auto& ws : src) {
			dst += ws;
			if(crlf) dst += '\n';
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストを繋げて、一つの文字列にする。(std::string)
		@param[in]	src		入力文字列リスト
		@param[in]	crlf	改行を挿入する場合「true」
		@param[out]	dst		出力文字列
	*/
	//-----------------------------------------------------------------//
	inline std::string strings_to_string(const strings& src, bool crlf) noexcept {
		std::string dst;
		for(const auto& s : src) {
			dst += s;
			if(crlf) dst += '\n';
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	キャラクター・リストのコードを取り除く
		@param[in]	src		入力文字列
		@param[in]	list	取り除くキャラクター列
		@param[out]	out		出力文字列
		@return 取り除かれた数
	*/
	//-----------------------------------------------------------------//
	template <class T, class M>
	int strip_char(const T& src, const M& list, T& out) {
		int n = 0;
		for(auto ch : src) {
			if(string_strchr(list, static_cast<typename M::value_type>(ch))) {
				++n;
			} else {
				out += ch;
			}
		}
		return n;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	キャラクター・リストのコードを取り除く
		@param[in]	src		入力文字列
		@param[in]	list	取り除くキャラクター列
		@return 取り除かれた文字列
	*/
	//-----------------------------------------------------------------//
	inline std::string strip_char(const std::string& src, const std::string& list) {
		std::string ans;
		strip_char(src, list, ans);
		return ans;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	キャラクター・リスト中のコードで分割する
		@param[in]	src		入力文字列
		@param[in]	list	分割するキャラクター列
		@param[in]	inhc	分割を無効にするキャラクター列
		@param[in]	limit	分割する最大数を設定する場合正の値
		@return	文字列リスト
	*/
	//-----------------------------------------------------------------//
	template <class SS>
	SS split_textT(const typename SS::value_type& src,
				   const typename SS::value_type& list,
				   const typename SS::value_type& inhc,
				   int limit = 0) noexcept
	{
		SS dst;
		bool tab_back = true;
		typename SS::value_type word;
		typename SS::value_type::value_type ihc = 0;
		for(auto ch : src) {
			bool tab = false;
			if(limit <= 0 || static_cast<int>(dst.size()) < (limit - 1)) {
				if(ihc == 0 && list.find(ch) != std::string::npos) {
					tab = true;
				}
			}
			if(tab_back && !tab && !word.empty()) {
				dst.push_back(word);
				word.clear();
				ihc = 0;
			}
			if(!tab) {
				if(!inhc.empty()) {
					if(word.empty() && inhc.find(ch) != std::string::npos) {
						ihc = ch;
					} else if(ch == ihc) {
						ihc = 0;
					}
				}
				word += ch;
			}
			tab_back = tab;
		}
		if(!word.empty()) {
			dst.push_back(word);
		}
		return dst;
	}

	inline strings split_text(const std::string& src, const std::string& list, const std::string& inhc = "",
		int limit = 0) noexcept {
		return split_textT<strings>(src, list, inhc, limit);
	}
	inline wstrings split_text(const wstring& src, const wstring& list, const wstring& inhc = wstring(),
		int limit = 0) noexcept {
		return split_textT<wstrings>(src, list, inhc, limit);
	}
	inline lstrings split_text(const lstring& src, const lstring& list, const lstring& inhc = lstring(),
		int limit = 0) noexcept {
		return split_textT<lstrings>(src, list, inhc, limit);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の文字コードを変換
		@param[in]	src ソース文字列
		@param[in]	a   変換前のコード
		@param[in]	b   変換後のコード
		@param[out]	dst 変換後の文字列
		@return 変換された数
	*/
	//-----------------------------------------------------------------//
	template <class ST, class DT>
	int code_conv(const ST& src, typename ST::value_type a, typename ST::value_type b, DT& dst) {
		int n = 0;
		for(auto ch : src) {
			if(ch == a) { ch = b; n++; }
			dst += static_cast<typename DT::value_type>(ch);
		}
		return n;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の文字コードを変換
		@param[in]	src ソース文字列
		@param[in]	a   変換前のコード
		@param[in]	b   変換後のコード
		@return 変換後の文字列
	*/
	//-----------------------------------------------------------------//
	inline std::string code_conv(const std::string& src, char a, char b) {
		std::string dst;
		code_conv(src, a, b, dst);
	    return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の文字コードを変換
		@param[in]	src ソース文字列
		@param[in]	a   変換前のコード
		@param[in]	b   変換後のコード
		@return 変換後の文字列
	*/
	//-----------------------------------------------------------------//
	inline std::string code_conv(const std::string& src, const std::string& a, const std::string& b) {
		if(a.empty() || b.empty()) return src;
		if(src.empty() || src.size() < a.size()) return "";
  
 		std::string ans;
		std::string::size_type pos = 0;
		do {
			auto n = src.find(a, pos);
			if(n != std::string::npos) {
				ans += b;
				n += a.size();
			}
			ans += src.substr(pos, n - pos);
			pos = n;
		} while(pos != std::string::npos) ;
		return ans;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の文字コードを変換
		@param[in]	src ソース文字列
		@param[in]	tbl 変換表（変換前、返還後と交互に並べる）@n
					※「返還後」コードとして０を指定すると、削除される。
		@param[out]	dst 変換後の文字列
		@return 変換された数
	*/
	//-----------------------------------------------------------------//
	template <class STR>
	int code_convs(const STR& src, const STR& tbl, STR& dst) {
		int n = 0;
		uint32_t tsz = tbl.size();
		if(tsz & 1) --tsz;
		for(auto ch : src) {
			for(uint32_t i = 0; i < tsz; i += 2) {
				if(ch == tbl[i]) {
					ch = tbl[i + 1];
					++n;
				}
			}
			if(ch) dst += ch;
		}
		return n;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列の評価変換
		@param[in]	src	ソース文字列
		@param[out]	dst 変換後の文字列
		@return 変換された文字数
	*/
	//-----------------------------------------------------------------//
	static int string_conv(const lstring& src, lstring& dst)
	{
		if(src.empty()) return 0;

		static const lstring tbl = {
			0x0009, ' ',	/// TAB ---> SPACE
			0x3000, ' ',	/// 全角スペース ---> SPACE
		};

		lstring s;
		int n = code_convs(src, tbl, s);

		lstring spc = { ' ' };
		lstrings ss = split_text(s, spc);

		for(const auto& l : ss) {
			dst += l;
			dst += ' ';
		}

		return n;
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	文字列の評価比較
		@param[in]	srca	ソース文字列 A
		@param[in]	srcb	ソース文字列 B
		@return 正確に一致したら 1.0 を返す
	*/
	//-----------------------------------------------------------------//
	static float compare(const lstring& srca, const lstring& srcb)
	{
		if(srca.empty() || srcb.empty()) return 0.0f;

		lstring a;
		string_conv(srca, a);
		lstring b;
		string_conv(srcb, b);

		lstring spcs = { ' ' };
		lstrings aa = split_text(a, spcs);
		lstrings bb = split_text(b, spcs);

		uint32_t anum = 0;
		for(const auto& s : aa) {
			anum += s.size();
		}
		uint32_t bnum = 0;
		for(const auto& s : bb) {
			bnum += s.size();
		}

		float ans = 0.0f;
		uint32_t n = aa.size();
		uint32_t num = anum;
		if(n > bb.size()) {
			n = bb.size();
			num = bnum;
		}
		for(uint32_t i = 0; i < n; ++i) {
			if(aa[i] == bb[i]) {
				ans += static_cast<float>(aa[i].size()) / static_cast<float>(num);
			}
		}
		return ans;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列の評価比較
		@param[in]	srca	ソース文字列 A
		@param[in]	srcb	ソース文字列 B
		@return 正確に一致したら 1.0 を返す
	*/
	//-----------------------------------------------------------------//
	inline float compare(const std::string& srca, const std::string& srcb) {
		lstring a;
		utf8_to_utf32(srca, a);
		lstring b;
		utf8_to_utf32(srcb, b);
		return compare(a, b);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フルパスか、相対パスか検査する
		@param[in]	path	ファイルパス
		@return フル・パスなら「true」
	*/
	//-----------------------------------------------------------------//
	static bool probe_full_path(const std::string& path)
	{
		if(path.empty()) return false;

		char ch = path[0];
#ifdef WIN32
		// WIN32 ではドライブレターの検査
		if(path.size() >= 3 && path[1] == ':' &&
			((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z'))) {
			ch = path[2];
		} else {
			ch = 0;
		}
		if(ch != 0 && (ch == '/' || ch == '\\')) {
			return true;
		}
#else
		if(ch != 0 && ch == '/') {
			return true;
		}
#endif
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フルパスから、ファイル名だけを取得する
		@param[in]	src		ソース文字列
		@param[in]	bks		バックスラッシュもセパレータとする場合「true」
		@return ファイル名（ポインター）
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline T get_file_nameT(const T& src, bool bks) {
		if(src.empty()) return T();
		const typename T::value_type* p = string_strrchr(src, '/');
		if(p != nullptr) {
			++p;
			return T(p);
		} else {
			if(bks) {
				const typename T::value_type* p = string_strrchr(src, '\\');
				if(p != nullptr) {
					++p;
					return T(p);
				}
			}
			const typename T::value_type* p = string_strrchr(src, ':');
			if(p != nullptr) {
				++p;
				return T(p);
			}
		}
		return src;
	}
	inline std::string get_file_name(const std::string& src, bool bks =
#ifdef WIN32
		true
#else
		false
#endif
	) {
		return get_file_nameT(src, bks);
	}
	inline wstring get_file_name(const wstring& src, bool bks =
#ifdef WIN32
		true
#else
		false
#endif
	) {
		return get_file_nameT(src, bks);
	}
	inline lstring get_file_name(const lstring& src, bool bks =
#ifdef WIN32
		true
#else
		false
#endif
	) {
		return get_file_nameT(src, bks);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・ベース名を取得
		@param[in]	src	ソース文字列
		@param[in]	bks		バックスラッシュもセパレータとする場合「true」
		@return ベース名
	*/
	//-----------------------------------------------------------------//
	inline std::string get_file_base(const std::string& src, bool bks =
#ifdef WIN32
		true
#else
		false
#endif
	) {
		auto tmp = get_file_name(src, bks);
		return tmp.substr(0, tmp.find_last_of('.'));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子を取得
		@param[in]	src	ソース文字列
		@return		拡張子の文字列
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline T get_file_extT(const T& src) {
		T s;
		if(src.empty()) return s;
		auto p = string_strrchr(src, '.');
		if(p != nullptr) {
			++p;
			auto q = string_strrchr(p, '/');
			if(q != nullptr) {
				s = q + 1;
			} else {
				s = p;
			}
		}
		return s;
	}

	inline std::string get_file_ext(const std::string& src) {
		return get_file_extT<std::string>(src);
	}
	inline wstring get_file_ext(const wstring& src) {
		return get_file_extT<wstring>(src);
	}
	inline lstring get_file_ext(const lstring& src) {
		return get_file_extT<lstring>(src);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・パスを取得
		@param[in]	src	フルパス文字列
		@return	ファイルパス
	*/
	//-----------------------------------------------------------------//
	inline std::string get_file_path(const std::string& src) {
		return src.substr(0, src.find_last_of('/'));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列終端が「/」なら取り除く
		@param[in]	src	ソースパス
		@return	出力パス
	*/
	//-----------------------------------------------------------------//
	inline std::string strip_last_of_delimita_path(const std::string& src) {
		std::string dst;
		if(!src.empty() && src[src.size() - 1] == '/') {
			dst = src.substr(0, src.size() - 1);
		} else {
			dst = src;
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	階層を一つ戻ったパスを得る
		@param[in]	src	ソースパス
		@return 戻ったパス
	*/
	//-----------------------------------------------------------------//
	static std::string previous_path(const std::string& src)
	{
		std::string dst;
		if(src.empty()) {
			return dst;
		}
		auto tmp = strip_last_of_delimita_path(src);
		std::string::size_type n = tmp.find_last_of('/');
		if(n == std::string::npos) {
			return dst;
		}
		dst = tmp.substr(0, n);
		// ルートの場合
		if(dst.find('/') == std::string::npos) {
			dst += '/';
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パスを追加
		@param[in]	src	ソースパス
		@param[in]	add	追加パス
		@return 合成パス（エラーならempty）
	*/
	//-----------------------------------------------------------------//
	static std::string append_path(const std::string& src, const std::string& add)
	{
		if(src.empty() || add.empty()) return std::string();
		std::string dst;
		if(add[0] == '/') {	// 新規パスとなる
			if(add.size() > 1) {
				dst = add;
			} else {
				return std::string();
			}
		} else {
			auto tmp = strip_last_of_delimita_path(src);
			dst = tmp + '/' + add;
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	デリミタを変換
		@param[in]	src	ソースパス
		@param[in]	org_ch 元のキャラクター
		@param[in]	cnv_ch  変換後のキャラクター
		@return 出力パス
	*/
	//-----------------------------------------------------------------//
	static std::string convert_delimiter(const std::string& src, char org_ch, char cnv_ch)
	{
		char back = 0;
		std::string dst;
		for(auto ch : src) {
			if(ch == org_ch) {
				if(back != 0 && back != cnv_ch) ch = cnv_ch;
			}
			if(back) dst += back;
			back = ch;
		}
		if(back) dst += back;

		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子フィルター
		@param[in]	src	ソース
		@param[in]	ext	拡張子（「,」で複数指定）
		@param[in]	cap	「false」なら大文字小文字を判定する
		@return リスト
	*/
	//-----------------------------------------------------------------//
	static strings ext_filter_path(const strings& src, const std::string& ext, bool cap = true) noexcept
	{
		strings dst;
		strings exts = split_text(ext, ",");
		for(const auto& s : src) {
			std::string src_ext = get_file_ext(s);
			for(const auto& ex : exts) {
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
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	マッチする文字をカウントする
		@param[in]	s	文字列
		@param[in]	cha	カウントする文字
		@return 数
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline uint32_t count_char(const T& src, typename T::value_type cha) noexcept {
		uint32_t cnt = 0;
		for(auto ch : src) {
			if(ch == cha) ++cnt;
		}
		return cnt;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列の回数追加
		@param[in]	ref	基準文字列
		@param[in]	cnt	追加回数
		@return 生成文字列
	*/
	//-----------------------------------------------------------------//
	inline std::string add_string(const std::string& ref, uint32_t cnt) {
		std::string str;
		for(uint32_t i = 0; i < cnt; ++i) {
			str += ref;
		}
		return str;
	}


	struct str {

		static bool oemc_to_utf8(const char* src, char* dst, uint32_t len) noexcept {
			auto a = sjis_to_utf8(src);
			if(a.size() < len) {
				strncpy(dst, a.c_str(), len);
				return true;
			} else {
				return false;
			}
		}


		static uint32_t utf16_to_utf8(uint16_t code, char* dst, uint32_t dsz) noexcept {
            uint32_t len = 0;
            if(code < 0x0080) {
                if(dsz >= 1) {
                    *dst++ = code;
                    len = 1;
                }
            } else if(code >= 0x0080 && code <= 0x07ff) {
                if(dsz >= 2) {
                    *dst++ = 0xc0 | ((code >> 6) & 0x1f);
                    *dst++ = 0x80 | (code & 0x3f);
                    len = 2;
                }
            } else if(code >= 0x0800) {
                if(dsz >= 3) {
                    *dst++ = 0xe0 | ((code >> 12) & 0x0f);
                    *dst++ = 0x80 | ((code >> 6) & 0x3f);
                    *dst++ = 0x80 | (code & 0x3f);
                    len = 3;
                }
            }
            return len;
		}


        static bool utf16_to_utf8(const uint16_t* src, char* dst, uint32_t dsz) noexcept
        {
            if(dsz <= 1) return false;

            uint16_t code;
            while((code = *src++) != 0) {
                auto len = utf16_to_utf8(code, dst, dsz);
                if(dsz > len) {
                    dsz -= len;
                    dst += len;
                } else {
                    break;
                }
            }
            *dst = 0;
            return dsz >= 1;
        }

	};
}
