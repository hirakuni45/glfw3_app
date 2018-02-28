#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル選択（WIN32) @n
			※comdlg32.lib をリンクする。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#ifdef WIN32
#include <commdlg.h>
#endif
#include <string>
#include <pthread.h>
#include "utils/string_utils.hpp"

namespace utils {

	class select_file {

		struct info_t {

			volatile uint32_t	id_;
			volatile bool		save_;

			pthread_mutex_t		sync_;

			std::string			init_dir_;
			std::string			filter_;
			std::string			path_;

			info_t() : id_(0), save_(false) { }
		};

		pthread_t		pth_;
		info_t			info_;

		volatile uint32_t	id_;

		bool			open_;

		static void* task_(void* entry)
		{
			info_t& t = *(static_cast<info_t*>(entry));
#ifdef WIN32
			OPENFILENAME ofn;
			char szFile[MAX_PATH] = "";
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);

			std::string filter = t.filter_;
			filter += utils::utf8_to_sjis("すべてのファイル(*.*)");
			filter += '\0';
			filter += utils::utf8_to_sjis("*.*");
			filter += '\0';
			filter += '\0';
			ofn.lpstrFilter = filter.c_str();
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrInitialDir = t.init_dir_.c_str();
			ofn.Flags = OFN_FILEMUSTEXIST;
			bool ret = false;
			if(t.save_) {
				ret = GetSaveFileName(&ofn) != 0;
			} else {
				ret = GetOpenFileName(&ofn) != 0;
			}
#else
			char szFile[256] = { 0 };
			bool ret = false;
#endif
			pthread_mutex_lock(&t.sync_);
			if(ret) {
				t.path_ = szFile;
			}
			++t.id_;
			pthread_mutex_unlock(&t.sync_);

			return nullptr;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		select_file() : id_(0), open_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@param[in]	filter	フィルター（"xxxx\tyyyy\t"）
			@param[in]	save	保存の場合「true」
			@param[in]	idir	初期フォルダ
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& filter = "", bool save = false, const std::string& idir = "")
		{
			if(open_) return false;

			pthread_mutex_init(&info_.sync_, nullptr);
			id_ = info_.id_;
			info_.init_dir_ = convert_delimiter(utf8_to_sjis(idir), '/', '\\');
			info_.filter_ = utils::code_conv(utf8_to_sjis(filter), '\t', '\0');
			info_.path_.clear();
			info_.save_ = save;
			pthread_create(&pth_, nullptr, task_, &info_);
			open_ = true;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ステート
			@return パス取得なら「true」
		*/
		//-----------------------------------------------------------------//
		bool state()
		{
			if(!open_) return false;

			pthread_mutex_lock(&info_.sync_);
			bool ret = id_ != info_.id_;
			pthread_mutex_unlock(&info_.sync_);
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@return キャンセルの場合「false」
		*/
		//-----------------------------------------------------------------//
		std::string get()
		{
			std::string path;
			if(state()) {
				pthread_mutex_lock(&info_.sync_);
				if(!info_.path_.empty()) {
					path = utils::code_conv(utils::sjis_to_utf8(info_.path_), '\\', '/');
				}
				pthread_mutex_unlock(&info_.sync_);
				open_ = false;
			}
			return path;
		}
	};
}
