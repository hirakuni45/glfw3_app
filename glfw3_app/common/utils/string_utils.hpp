#pragma once
//=====================================================================//
/*!	@file
	@brief	文字列操作ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include "utils/mtx.hpp"

namespace utils {

	typedef std::string::iterator						string_it;
	typedef std::string::const_iterator					string_cit;

	typedef std::basic_string<wchar_t>					wstring;
	typedef std::basic_string<wchar_t>::iterator		wstring_it;
	typedef std::basic_string<wchar_t>::const_iterator	wstring_cit;

	typedef std::vector<std::string>					strings;
	typedef std::vector<std::string>::iterator			strings_it;
	typedef std::vector<std::string>::const_iterator	strings_cit;

	typedef std::vector<wstring>						wstrings;
	typedef std::vector<wstring>::iterator				wstrings_it;
	typedef std::vector<wstring>::const_iterator		wstrings_cit;

	typedef std::vector<strings>						strings_array;
	typedef std::vector<strings>::iterator	   			strings_array_it;
	typedef std::vector<strings>::const_iterator	   	strings_array_cit;

	typedef std::pair<std::string, std::string>		str_str_pair;
	typedef std::map<std::string, std::string>		str_str_map;
	typedef str_str_map::iterator					str_str_map_it;
	typedef str_str_map::const_iterator				str_str_map_cit;
	typedef boost::unordered_map<std::string, std::string>	str_str_undmap;
	typedef str_str_undmap::iterator						str_str_undmap_it;
	typedef str_str_undmap::const_iterator					str_str_undmap_cit;

	typedef boost::optional<const std::string&>	optional_str_ref;

	typedef std::vector<int>	int_vector;
	typedef std::vector<float>	float_vector;


	bool string_to_int(const std::string& src, int_vector& dst);
	bool string_to_float(const std::string& src, float_vector& dst);
	bool string_to_matrix4x4(const std::string& src, mtx::fmat4& dst);

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


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strchr の string/wstring 版
		@param[in]	src	ソース
		@param[in]	ch	探す文字
		@return 見つかればポインターを返す
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline const typename T::value_type* string_strchrT(const T& src, typename T::value_type ch) {
		size_t idx = src.find_first_of(ch);
		if(idx == T::npos) return 0;
		else return &src[idx];
	}
	inline const char* string_strchr(const std::string& src, char ch) { return string_strchrT(src, ch); }
	inline const wchar_t* string_strchr(const wstring& src, wchar_t ch) { return string_strchrT(src, ch); }


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strrchr の string/wstring 版
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
	inline const wchar_t* string_strrchr(const wstring& src, wchar_t ch) { return string_strrchrT(src, ch); }


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
	inline int string_strcmp(const wstring& srca, const utils::wstring& srcb) {
		return string_strcmpT(srca, srcb); }


	//-----------------------------------------------------------------//
	/*!
		@brief	要するに、strncmp の string/wstring 版
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
	inline int string_strncmp(const wstring& srca, const utils::wstring& srcb, wstring::size_type n) {
		return string_strncmpT(srca, srcb, n); }


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の大文字を小文字に変換
		@param[in]	src	ソース文字列
		@param[out]	dst	変換後の文字列
	*/
	//-----------------------------------------------------------------//
	template <class T>
	void to_lower_text(const T& src, T& dst) {
		BOOST_FOREACH(typename T::value_type ch, src) {
			if(ch >= 'A' && ch <= 'Z') {
				dst += (ch + 0x20);
			} else {
				dst += ch;
			}
		}
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
		T a;
		to_lower_text(srca, a);
		T b;
		to_lower_text(srcb, b);
		return a.compare(b);
	}
	inline int no_capital_strcmp(const std::string& srca, const std::string& srcb) {
		return no_capital_strcmpT(srca, srcb);
	}
	inline int no_capital_strcmp(const wstring& srca, const utils::wstring& srcb) {
		return no_capital_strcmpT(srca, srcb);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8(ucs2) から UTF-16 への変換
		@param[in]	src	UTF-8 ソース
		@param[in]	dst	UTF-16 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool utf8_to_utf16(const std::string& src, wstring& dst)
	{
		if(src.empty()) return false;

		bool f = true;
		int cnt = 0;
		wchar_t	code = 0;
		BOOST_FOREACH(char tc, src) {
			unsigned char c = static_cast<unsigned char>(tc);
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
			}
		}
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から UTF-8(ucs2) への変換
		@param[in]	src	UTF-8 ソース
		@param[in]	dst	UTF-16 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool utf16_to_utf8(const wstring& src, std::string& dst)
	{
		if(src.empty()) return false;

		bool f = true;
		BOOST_FOREACH(wchar_t c, src) {
			uint16_t code = static_cast<uint16_t>(c);
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
		@brief	Shift-JIS から UTF-8(ucs2) への変換
		@param[in]	src	Shift-JIS ソース
		@param[in]	dst	UTF-8 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool sjis_to_utf8(const std::string& src, std::string& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	Shift-JIS から UTF-16 への変換
		@param[in]	Shift-JIS src	ソース
		@param[in]	UTF-16 dst	出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool sjis_to_utf16(const std::string& src, wstring& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-8(ucs2) から Shift-JIS への変換
		@param[in]	src	UTF8 ソース
		@param[in]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf8_to_sjis(const std::string& src, std::string& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-16 から Shift-JIS への変換
		@param[in]	src	UTF16 ソース
		@param[in]	dst	Shift-JIS 出力
		@return 変換が正常なら「true」
	*/
	//-----------------------------------------------------------------//
	bool utf16_to_sjis(const wstring& src, std::string& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストの変換
		@param[in]	src		入力文字列リスト
		@param[out]	dst		出力文字列リスト
	*/
	//-----------------------------------------------------------------//
	inline void strings_to_strings(const wstrings& src, strings& dst) {
		BOOST_FOREACH(const wstring& ws, src) {
			std::string tmp;
			utf16_to_utf8(ws, tmp);
			dst.push_back(tmp);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストの変換
		@param[in]	src		入力文字列リスト
		@param[out]	dst		出力文字列リスト
	*/
	//-----------------------------------------------------------------//
	inline void strings_to_strings(const strings& src, wstrings& dst) {
		BOOST_FOREACH(const std::string& s, src) {
			wstring tmp;
			utf8_to_utf16(s, tmp);
			dst.push_back(tmp);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストを繋げて、一つの文字列にする。(wstring)
		@param[in]	src		入力文字列リスト
		@param[in]	crlf	改行を挿入する場合「true」
		@param[out]	dst		出力文字列
	*/
	//-----------------------------------------------------------------//
	inline void strings_to_string(const wstrings& src, bool crlf, wstring& dst) {
		BOOST_FOREACH(const wstring& ws, src) {
			dst += ws;
			if(crlf) dst += '\n';
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列リストを繋げて、一つの文字列にする。(std::string)
		@param[in]	src		入力文字列リスト
		@param[in]	crlf	改行を挿入する場合「true」
		@param[out]	dst		出力文字列
	*/
	//-----------------------------------------------------------------//
	inline void strings_to_string(const strings& src, bool crlf, std::string& dst) {
		BOOST_FOREACH(const std::string& s, src) {
			dst += s;
			if(crlf) dst += '\n';
		}
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
		BOOST_FOREACH(typename T::value_type c, src) {
			if(string_strchr(list, static_cast<typename M::value_type>(c))) {
				++n;
			} else {
				out += c;
			}
		}
		return n;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	キャラクター・リスト中のコードで分割する
		@param[in]	src		入力文字列
		@param[in]	list	分割にするキャラクター列
		@param[out]	dst		出力文字列
		@param[in]	limit	分割する最大数を設定する場合正の値
	*/
	//-----------------------------------------------------------------//
	template <class T>
	void split_text(const typename T::value_type& src,
					const typename T::value_type& list, T& dst, int limit = 0)
	{
		bool tab_back = true;
		typename T::value_type word;
		BOOST_FOREACH(typename T::value_type::value_type c, src) {
			bool tab = false;
			if(limit <= 0 || static_cast<int>(dst.size()) < (limit - 1)) {
				if(string_strchr(list, c)) {
					tab = true;
				}
			}
			if(tab_back == true && tab == false && word.empty() == false) {
				dst.push_back(word);
				word.clear();
			}
			if(tab == false) word += c;
			tab_back = tab;
		}
		if(word.empty() == false) {
			dst.push_back(word);
		}
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
	template <class T, class M>
	int code_conv(const T& src, typename T::value_type a, typename T::value_type b, M& dst) {
		int i = 0;
		BOOST_FOREACH(typename T::value_type c, src) {
			if(c == a) { c = b; i++; }
			dst += static_cast<typename M::value_type>(c);
		}
		return i;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フルパスから、ファイル名だけを取得する
		@param[in]	src	ソース文字列
		@return ファイル名（ポインター）
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline const typename T::value_type* get_file_nameT(const T& src) {
		if(src.empty()) return 0;
		const typename T::value_type* p = string_strrchr(src, '/');
		if(p != 0) {
			++p;
			return p;
		} else {
			const typename T::value_type* p = string_strrchr(src, ':');
			if(p != 0) {
				++p;
				return p;
			}
		}
		return src.c_str();
	}
	inline const char* get_file_name(const std::string& src) { return get_file_nameT(src); }
	inline const wchar_t* get_file_name(const wstring& src) { return get_file_nameT(src); }


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・ベース名を取得
		@param[in]	src	ソース文字列
		@param[out]	dst	出力文字列
	*/
	//-----------------------------------------------------------------//
	inline void get_file_base(const std::string& src, std::string& dst) {
		const char* p = strrchr(src.c_str(), '.');
		if(p) {
			dst.append(src.c_str(), p - src.c_str());
		} else {
			dst += src;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子を取得
		@param[in]	src	ソース文字列
		@return		拡張子の文字列
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline const typename T::value_type* get_file_extT(const T& src) {
		if(src.empty()) return 0;
		const typename T::value_type* p = string_strrchr(src, '.');
		if(p) {
			++p;
			if((p - &src[0]) <= 0) {
				return 0;
			}
			if(string_strchr(p, '/')) {
				return 0;
			}
			return p;
		}
		return 0;
	}
	inline const char* get_file_ext(const std::string& src) { return get_file_extT(src); }
	inline const wchar_t* get_file_ext(const wstring& src) { return get_file_extT(src); }


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・パスを取得
		@param[in]	src	フルパス文字列
		@param[out]	dst ファイルパス
		@return 取得できたら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool get_file_path(const std::string& src, std::string& dst) {
		const char* p = strrchr(src.c_str(), '/');
		if(p) {
			dst.append(src.c_str(), p - src.c_str());
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列終端が「/」なら取り除く
		@param[in]	src	ソースパス
		@param[out]	dst	出力パス
		@return 取り除いたら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool strip_last_of_delimita_path(const std::string& src, std::string& dst) {
		if(src.size() > 0 && src[src.size() - 1] == '/') {
			dst = src.substr(0, src.size() - 1);
			return true;
		} else {
			dst = src;
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	階層を一つ戻ったパスを得る
		@param[in]	src	ソースパス
		@param[out]	dst	出力パス
		@return エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool previous_path(const std::string& src, std::string& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	パスを追加
		@param[in]	src	ソースパス
		@param[in]	add	追加パス
		@param[out]	dst	出力パス
		@return エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool append_path(const std::string& src, const std::string& add, std::string& dst);


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
	bool convert_delimiter(const std::string& src, char org_ch, char cnv_ch, std::string& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子フィルター
		@param[in]	src	ソース
		@param[in]	ext	拡張子
		@param[out]	dst	出力
		@param[in]	cap	「false」なら大文字小文字を判定する
	*/
	//-----------------------------------------------------------------//
	void ext_filter_path(const strings& src, const std::string& ext, strings& dst, bool cap = true);

}
