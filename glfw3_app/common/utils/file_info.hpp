#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル情報クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <string>
#include <dirent.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <algorithm>
#include <sys/stat.h>
#include "utils/file_io.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル情報クラス@n
				ファイル名、サイズ、ディレクトリーの判定を格納
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class file_info {
		std::string		name_;
		bool			directory_;
		size_t			size_;
		time_t			time_;
		mode_t			mode_;
		bool			drive_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	標準コンストラクター
		*/
		//-----------------------------------------------------------------//
		file_info() : name_(), directory_(false), size_(0), time_(0), mode_(0), drive_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化コンストラクター
			@param[in]	name	ファイル名
			@param[in]	directory	ディレクトリーの場合に「true」
			@param[in]	size	ファイル・サイズ
			@param[in]	tm		時間
			@param[in]	mt		モード
			@param[in]	drv		ドライブ（省略すると「false」）
		*/
		//-----------------------------------------------------------------//
		file_info(const std::string& name, bool directory, size_t size, const time_t tm, const mode_t mt, bool drv = false) :
			name_(name), directory_(directory), size_(size), time_(tm), mode_(mt), drive_(drv)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	name	ファイル名
			@param[in]	directory	ディレクトリーの場合に「true」
			@param[in]	size	ファイル・サイズ
			@param[in]	tm		時間
			@param[in]	mt		モード
			@param[in]	drv		ドライブ（省略すると「false」）
		*/
		//-----------------------------------------------------------------//
		void initialize(const std::string& name, bool directory, size_t size, const time_t tm, const mode_t mt, bool drv = false)
		{
			file_info(name, directory, size, tm, mt, drv);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル名を得る
			@return ファイル名を返す
		*/
		//-----------------------------------------------------------------//
		const std::string& get_name() const { return name_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	デイレクトリーか検査
			@return ディレクトリーの場合は「true」
		*/
		//-----------------------------------------------------------------//
		bool is_directory() const { return directory_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・サイズを得る
			@return ファイルサイズを返す@n
					ディレクトリーの場合、常に０
		*/
		//-----------------------------------------------------------------//
		size_t get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	タイム・スタンプを得る
			@return ファイルサイズを返す
		*/
		//-----------------------------------------------------------------//
		time_t get_time() const { return time_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルモード得る
			@return ファイルモードを返す
		*/
		//-----------------------------------------------------------------//
		mode_t get_mode() const { return mode_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ドライブか検査
			@return ドライブの場合は「true」
		*/
		//-----------------------------------------------------------------//
		bool is_drive() const { return drive_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	sort 用オペレーター
			@return 文字列の比較
		*/
		//-----------------------------------------------------------------//
		bool operator < (const file_info& right) const {
			return name_ < right.name_;
		}
	};

	typedef std::vector<file_info>					file_infos;
	typedef std::vector<file_info>::iterator		file_infos_it;
	typedef std::vector<file_info>::const_iterator	file_infos_cit;


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーのファイルリストを作成
		@param[in]	root	ルート・パス
		@param[out]	list	ファイルリストを受け取るクラス
		@return リストの取得に失敗した場合「false」
	*/
	//-----------------------------------------------------------------//
	static bool create_file_list(const std::string& root, file_infos& list)
	{
		if(root.empty()) return false;

#ifdef WIN32
		utils::wstring wsr;
		utf8_to_utf16(root, wsr);
		if(!wsr.empty()) {
			// driver latter convert
			if(root[0] == '/' && ((root[1] >= 'A' && root[1] <= 'Z') || (root[1] >= 'a' && root[1] <= 'z')) && root[2] == '/') {
				wsr[0] = wsr[1];
				wsr[1] = ':';
			}
		}
		wchar_t* wtmp = new wchar_t[wsr.size() + 1];
		for(uint32_t i = 0; i < wsr.size(); ++i) {
			wtmp[i] = wsr[i];
		}
		wtmp[wsr.size()] = 0;
		_WDIR* dir = _wopendir(wtmp);
		delete[] wtmp;
#else
		DIR* dir = opendir(root.c_str());
#endif
		if(dir) {
#ifdef WIN32
			struct _wdirent* ent;
			while((ent = _wreaddir(dir)) != 0) {
				struct _stat st;
				utils::wstring fn;
				utf8_to_utf16(root, fn);
				fn += '/';
				{
					wchar_t wch;
					wchar_t* p = ent->d_name;
					while((wch = *p++) != 0) {					
						fn += wch;
					}
				}
				wchar_t* wfn = new wchar_t[fn.size() + 1];
				for(uint32_t i = 0; i < fn.size(); ++i) {
					wfn[i] = fn[i];
				}
				wfn[fn.size()] = 0;
				if(ent->d_namlen > 0 && _wstat(wfn, &st) == 0) {
					bool d = S_ISDIR(st.st_mode);
					wstring ws;
					wchar_t wch;
					wchar_t* p = ent->d_name;
					while((wch = *p++) != 0) {
						ws += wch;
					}
					std::string s;
					utf16_to_utf8(ws, s);
					file_info info(s, d, st.st_size, st.st_mtime, st.st_mode);
					list.push_back(info);
				}
				delete[] wfn;
			}
			_wclosedir(dir);
#else
			struct dirent* ent;
			while((ent = readdir(dir)) != 0) {
				struct stat st;
				std::string fn = root;
				fn += '/';
				fn += ent->d_name;
				if(stat(fn.c_str(), &st) == 0) {
					bool d = S_ISDIR(st.st_mode);
					file_info info(ent->d_name, d, st.st_size, st.st_mtime, st.st_mode);
					list.push_back(info);
				}
			}
			closedir(dir);
#endif
#ifdef __linux__
			std::sort(list.begin(), list.end());
#endif
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報からフォーマットされた文字列を生成
		@param[in]	list	fileinfos クラス
		@param[in]	format	フォーマット（ディレクトリー出力専用形式）@n
							%s ---> ファイル名@n
							%n ---> ファイルサイズ@n
							%/ ---> ディレクトリーなら、'/'を追加
		@param[in]	dst		出力先列
	*/
	//-----------------------------------------------------------------//
	static void create_format_file_infos(const file_infos& list, const std::string& format, utils::strings& dst)
	{
		BOOST_FOREACH(const file_info& fi, list) {
			if(format.empty()) {
				std::string s = fi.get_name();
				if(fi.is_directory()) s += '/';
				dst.push_back(s);
			} else {
				std::string s;
				char m = 0;
				BOOST_FOREACH(char c, format) {
					if(m) {
						if(c == '%') ;
						else if(c == 's' || c == 'S') {
							s += fi.get_name();
							c = 0;
						} else if(c == 'n') {
							s += boost::io::str(boost::format("%d") % fi.get_size());
							c = 0;
						} else if(c == '/') {
							if(!fi.is_directory()) c = 0;
						}
						m = 0;
					} else if(c == '%') {
						m = c;
						c = 0;
					}
					if(c) s += c;
				}
				dst.push_back(s);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報リストを正規表現フィルターで再構成
		@param[in]	src		fileinfos クラス
		@param[in]	filter	拡張子文字列 (ex: "xxx,yyy,...")
		@param[in]	cap		「false」なら大文字小文字を判定する
		@param[in]	last	「false」なら最初に現れた「.」にマッチ
		@return		出力列
	*/
	//-----------------------------------------------------------------//
	static file_infos filter_file_infos(const file_infos& src, const std::string& filter, bool cap = true, bool last = true)
	{
		file_infos dst;

		utils::strings ss;
		if(!filter.empty()) {
			ss = utils::split_text(filter, ",");
		}

		BOOST_FOREACH(const file_info& f, src) {
			bool add = false;
			if(f.is_directory() || ss.empty()) add = true;
			else {
				const std::string& n = f.get_name();
				BOOST_FOREACH(const std::string& s, ss) {
					const char* p = nullptr;
					if(last) p = strrchr(n.c_str(), '.');
					else p = strchr(n.c_str(), '.');
					if(p) {
						std::string ext(p + 1);
						if(cap) {
							if(s == ext) {
								add = true;
								break;
							}
						} else {
							if(utils::no_capital_strcmp(ext, s) == 0) {
								add = true;
								break;
							}
						}
					}
				}
			}
			if(add) dst.push_back(f);
		}
		return dst;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーのファイルリストを作成
		@param[in]	root	ルート・パス
		@return		ファイルリスト
	*/
	//-----------------------------------------------------------------//
	inline file_infos create_file_list(const std::string& root) {
		file_infos dst;
		create_file_list(root, dst);
		return dst;
	}
}
