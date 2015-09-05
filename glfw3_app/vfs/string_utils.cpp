//=====================================================================//
/*!	@file
	@brief	文字列操作ユーティリティー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "string_utils.hpp"
// #include <boost/format.hpp>
// #include <boost/lexical_cast.hpp>
// #include "utils/sjis_utf16.hpp"

// #include <iostream>

namespace utils {

	using namespace std;
#if 0
	bool string_to_hex(const std::string& src, uint32_t& dst)
	{
		uint32_t v = 0;
		BOOST_FOREACH(char ch, src) {
			v <<= 4;
			if(ch >= '0' && ch <= '9') v |= ch - '0';
			else if(ch >= 'A' && ch <= 'F') v |= ch - 'A' + 10;
			else if(ch >= 'a' && ch <= 'f') v |= ch - 'a' + 10;
			else return false;
		}
		dst = v;
		return true;
	}


	bool string_to_hex(const std::string& src, std::vector<uint32_t>& dst, const std::string& spc)
	{
		string s;
		BOOST_FOREACH(char ch, src) {
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


	bool string_to_int(const std::string& src, int32_t& dst)
	{
		try {
			dst = boost::lexical_cast<int32_t>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	bool string_to_int(const std::string& src, std::vector<int32_t>& dst, const std::string& spc)
	{
		try {
			string s;
			BOOST_FOREACH(char ch, src) {
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


	bool string_to_float(const std::string& src, float& dst)
	{
		try {
			dst = boost::lexical_cast<float>(src);
		} catch(boost::bad_lexical_cast& bad) {
			return false;
		}
		return true;
	}


	bool string_to_float(const std::string& src, std::vector<float>& dst, const std::string& spc)
	{
		try {
			string s;
			BOOST_FOREACH(char ch, src) {
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
#endif


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
			((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z'))) {
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
		@return 戻ったパス
	*/
	//-----------------------------------------------------------------//
	std::string previous_path(const std::string& src)
	{
		std::string dst;
		if(src.empty()) {
			return std::move(dst);
		}
		std::string tmp;
		strip_last_of_delimita_path(src, tmp);
		std::string::size_type n = tmp.find_last_of('/');
		if(n == std::string::npos) {
			return std::move(dst);
		}
		dst = tmp.substr(0, n);
		// ルートの場合
		if(dst.find('/') == std::string::npos) {
			dst += '/';
		}
		return std::move(dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パスを追加
		@param[in]	src	ソースパス
		@param[in]	add	追加パス
		@return 合成パス（エラーならempty）
	*/
	//-----------------------------------------------------------------//
	std::string append_path(const std::string& src, const std::string& add)
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
			std::string tmp;
			strip_last_of_delimita_path(src, tmp);
			dst = tmp + '/' + add;
		}
		return std::move(dst);
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
	std::string convert_delimiter(const std::string& src, char org_ch, char cnv_ch)
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

		return std::move(dst);
	}

#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	拡張子フィルター
		@param[in]	src	ソース
		@param[in]	ext	拡張子（「,」で複数指定）
		@param[in]	cap	「false」なら大文字小文字を判定する
		@return リスト
	*/
	//-----------------------------------------------------------------//
	strings ext_filter_path(const strings& src, const std::string& ext, bool cap) noexcept
	{
		strings dst;
		strings exts = split_text(ext, ",");
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
		return std::move(dst);
	}
#endif
}
