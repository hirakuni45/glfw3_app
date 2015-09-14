#pragma once
//=====================================================================//
/*!	@file
	@brief	文字列操作ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>

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

	bool string_to_hex(const std::string& src, uint32_t& dst);
	bool string_to_hex(const std::string& src, std::vector<uint32_t>& dst, const std::string& spc = " ,:");
	bool string_to_int(const std::string& src, int32_t& dst);
	bool string_to_int(const std::string& src, std::vector<int32_t>& dst, const std::string& spc = " ,:");
	bool string_to_float(const std::string& src, float& dst);
	bool string_to_float(const std::string& src, std::vector<float>& dst, const std::string& spc = " ,:");


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
		@brief	文字列リストを繋げて、一つの文字列にする。(wstring)
		@param[in]	src		入力文字列リスト
		@param[in]	crlf	改行を挿入する場合「true」
		@return	文字列
	*/
	//-----------------------------------------------------------------//
	inline wstring strings_to_string(const wstrings& src, bool crlf) noexcept {
		wstring dst;
		for(auto ws : src) {
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
		for(auto s : src) {
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
		for(auto c : src) {
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
		@param[in]	limit	分割する最大数を設定する場合正の値
		@return	文字列リスト
	*/
	//-----------------------------------------------------------------//
	template <class SS>
	SS split_textT(const typename SS::value_type& src,
					const typename SS::value_type& list,
					int limit = 0) noexcept
	{
		SS dst;
		bool tab_back = true;
		typename SS::value_type word;
		for(auto ch : src) {
			bool tab = false;
			if(limit <= 0 || static_cast<int>(dst.size()) < (limit - 1)) {
				if(string_strchr(list, ch)) {
					tab = true;
				}
			}
			if(tab_back && !tab && !word.empty()) {
				dst.push_back(word);
				word.clear();
			}
			if(!tab) word += ch;
			tab_back = tab;
		}
		if(!word.empty()) {
			dst.push_back(word);
		}
		return dst;
	}

	inline strings split_text(const std::string& src, const std::string& list, int limit = 0) noexcept {
		return split_textT<strings>(src, list, limit);
	}
	inline wstrings split_text(const wstring& src, const wstring& list, int limit = 0) noexcept {
		return split_textT<wstrings>(src, list, limit);
	}
	inline lstrings split_text(const lstring& src, const lstring& list, int limit = 0) noexcept {
		return split_textT<lstrings>(src, list, limit);
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
		for(auto c : src) {
			if(c == a) { c = b; n++; }
			dst += static_cast<typename DT::value_type>(c);
		}
		return n;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	文字列中の文字コードを変換
		@param[in]	src ソース文字列
		@param[in]	tbl 変換表（変換前、返還後と交互に並べる）@n
					※「返還後」コードとして０を指定すると、バッファから除外される。
		@param[out]	dst 変換後の文字列
		@return 変換された数
	*/
	//-----------------------------------------------------------------//
	template <class STR>
	int code_conv(const STR& src, const STR& tbl, STR& dst) {
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
		@brief	フルパスか、相対パスか検査する
		@param[in]	path	ファイルパス
		@return フル・パスなら「true」
	*/
	//-----------------------------------------------------------------//
	bool probe_full_path(const std::string& path);


	//-----------------------------------------------------------------//
	/*!
		@brief	フルパスから、ファイル名だけを取得する
		@param[in]	src	ソース文字列
		@return ファイル名（ポインター）
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline const typename T::value_type* get_file_nameT(const T& src) {
		if(src.empty()) return nullptr;
		const typename T::value_type* p = string_strrchr(src, '/');
		if(p != nullptr) {
			++p;
			return p;
		} else {
			const typename T::value_type* p = string_strrchr(src, ':');
			if(p != nullptr) {
				++p;
				return p;
			}
		}
		return src.c_str();
	}
	inline const char* get_file_name(const std::string& src) { return get_file_nameT(src); }
	inline const wstring::value_type* get_file_name(const wstring& src) { return get_file_nameT(src); }
	inline const lstring::value_type* get_file_name(const lstring& src) { return get_file_nameT(src); }


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・ベース名を取得
		@param[in]	src	ソース文字列
		@return ベース名
	*/
	//-----------------------------------------------------------------//
	inline std::string get_file_base(const std::string& src) {
		std::string fn = get_file_name(src);
		// ピリオドがあるか？
		std::string::size_type pos = fn.find_last_of('.');
		std::string dst;
		if(pos != std::string::npos) {
			dst += fn.substr(0, pos);
		} else {
			dst += fn;
		}
		return dst;
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
		@param[out]	dst ファイルパス
		@return 取得できたら「true」
	*/
	//-----------------------------------------------------------------//
	inline bool get_file_path(const std::string& src, std::string& dst) {
		const char* p = string_strrchr(src, '/');
		if(p) {
			dst.append(src.c_str(), p - src.c_str());
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・パスを取得
		@param[in]	src	フルパス文字列
		@return	ファイルパス
	*/
	//-----------------------------------------------------------------//
	inline std::string get_file_path(const std::string& src) {
		std::string dst;
		get_file_path(src, dst);
		return dst;
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
		@brief	文字列終端が「/」なら取り除く
		@param[in]	src	ソースパス
		@return	出力パス
	*/
	//-----------------------------------------------------------------//
	inline std::string strip_last_of_delimita_path(const std::string& src) {
		std::string dst;
		strip_last_of_delimita_path(src, dst);
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	階層を一つ戻ったパスを得る
		@param[in]	src	ソースパス
		@return 戻ったパス
	*/
	//-----------------------------------------------------------------//
	std::string previous_path(const std::string& src);


	//-----------------------------------------------------------------//
	/*!
		@brief	パスを追加
		@param[in]	src	ソースパス
		@param[in]	add	追加パス
		@return 合成パス（エラーならempty）
	*/
	//-----------------------------------------------------------------//
	std::string append_path(const std::string& src, const std::string& add);


	//-----------------------------------------------------------------//
	/*!
		@brief	デリミタを変換
		@param[in]	src	ソースパス
		@param[in]	org_ch 元のキャラクター
		@param[in]	cnv_ch  変換後のキャラクター
		@return 出力パス
	*/
	//-----------------------------------------------------------------//
	std::string convert_delimiter(const std::string& src, char org_ch, char cnv_ch);


	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子フィルター
		@param[in]	src	ソース
		@param[in]	ext	拡張子（「,」で複数指定）
		@param[in]	cap	「false」なら大文字小文字を判定する
		@return リスト
	*/
	//-----------------------------------------------------------------//
	strings ext_filter_path(const strings& src, const std::string& ext, bool cap = true) noexcept;


	//-----------------------------------------------------------------//
	/*!
		@brief	マッチする文字をカウントする
		@param[in]	s	文字列
		@param[in]	ch	カウントする文字
		@return 数
	*/
	//-----------------------------------------------------------------//
	template <class T>
	inline uint32_t count_char(const T& src, typename T::value_type ch) noexcept {
		uint32_t cnt = 0;
		for(auto c : src) {
			if(c == ch) ++cnt;
		}
		return cnt;
	}
}
