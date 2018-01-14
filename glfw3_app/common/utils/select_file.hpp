#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル選択（WIN32) @n
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#ifdef WIN32
#include <windows.h>
#endif
#include <string>
#include <pthread.h>

namespace utils {

	class select_file {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	情報構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct info_t {

			volatile uint32_t	id_;
			volatile bool		exit_;

			pthread_mutex_t		sync_;

			std::string			path_;

			info_t() : id_(0), exit_(false) { }
		};

	private:
		pthread_t		pth_;
		info_t			info_;

		volatile uint32_t	id_;

		bool			open_;

		static void* task_(void* entry)
		{
			info_t& t = *(static_cast<info_t*>(entry));
#ifdef WIN32
			OPENFILENAME ofn;
			char szFile[ MAX_PATH ] = "";
			ZeroMemory( &ofn, sizeof( ofn ) );
			ofn.lStructSize = sizeof( OPENFILENAME );
			ofn.lpstrFilter = "テキストファイル(*.txt)\0*.txt\0"
				"すべてのファイル(*.*)\0*.*\0\0";
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			bool ret = GetOpenFileName(&ofn) != 0;
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
		*/
		//-----------------------------------------------------------------//
		void open()
		{
			pthread_mutex_init(&info_.sync_, nullptr);
			id_ = info_.id_;
			info_.path_.clear();
			pthread_create(&pth_, nullptr, task_, &info_);
			open_ = true;
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
				path = info_.path_;
				pthread_mutex_unlock(&info_.sync_);
				open_ = false;
			}
			return path;
		}
	};
}
