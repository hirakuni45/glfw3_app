#pragma once
//=====================================================================//
/*!	@file
	@brief	フォルダ選択（WIN32) @n
			※comdlg32.lib をリンクする。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#ifdef WIN32
#include <shlobj.h>
#endif
#include <string>
#include <pthread.h>
#include "utils/string_utils.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	フォルダ選択 (WIN32)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class select_dir {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	フォルダ選択ステート
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class state {
			none,		///< 無し
			select,		///< 選択中
			cancel,		///< キャンセル
			selected,	///< 選択した
		};

	private:
		struct info_t {
			volatile bool		active_;
			volatile uint32_t	id_;

			pthread_mutex_t		sync_;

			std::string			title_;
			std::string			fpath_;  // 初期パス
			std::string			path_;

			info_t() : active_(false), id_(0) { }
		};

		pthread_t		pth_;
		info_t			info_;

		volatile uint32_t	id_;

		bool			open_;

		static int CALLBACK SHBrowseProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
		{
    		if(uMsg == BFFM_INITIALIZED && lpData) {
				//  デフォルトで選択させるパスの指定
				SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData);
    		}
    		return 0;
		}

		static void SHFree(ITEMIDLIST* pidl)
		{
			IMalloc*  pMalloc;
    		SHGetMalloc(&pMalloc);
    		if(pMalloc != nullptr) {
        		pMalloc->Free(pidl);
        		pMalloc->Release();
			}
		}

		static void* task_(void* entry)
		{
			info_t& t = *(static_cast<info_t*>(entry));
#ifdef WIN32
			bool ret = false;
			BROWSEINFO bi;
			ZeroMemory(&bi, sizeof(BROWSEINFO));
//			bi.hwndOwner	= hwndOwner;
			bi.lpfn			= SHBrowseProc;	//  コールバック関数を指定
			//  デフォルトで選択させておくフォルダを指定
			bi.lParam		= (LPARAM)t.fpath_.c_str();
			bi.lpszTitle	= t.title_.c_str();
// "フォルダを選択してください";	//  タイトルの指定
			ITEMIDLIST* pidl = SHBrowseForFolder(&bi);	//  フォルダダイアログの起動
			char path[_MAX_PATH];
			path[0] = 0;
			if(pidl) {
///				LPTSTR lpszBuffer;
///				DWORD dwBufferSize = _MAX_PATH;
				//  選択されたフォルダ名を取得
				SHGetPathFromIDList(pidl, path);
				SHFree(pidl);
				ret = true;
			}
			t.active_ = false;
#else
			char szFile[256] = { 0 };
			bool ret = false;
#endif
			pthread_mutex_lock(&t.sync_);
			if(ret) {
				++t.id_;
				t.path_ = path;
			}
			pthread_mutex_unlock(&t.sync_);
			return nullptr;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		select_dir() : id_(0), open_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@param[in]	title	選択タイトル
			@param[in]	fpath	初期パス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& title = "", const std::string fpath = "/")
		{
			if(open_) return false;

			pthread_mutex_init(&info_.sync_, nullptr);
			info_.active_ = true;
			id_ = info_.id_;
			info_.title_ = utils::utf8_to_sjis(title);
			info_.fpath_ = utils::convert_delimiter(utils::utf8_to_sjis(fpath), '/', '\\');
			info_.path_.clear();
			pthread_create(&pth_, nullptr, task_, &info_);
			open_ = true;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ステートを取得
			@return ステート
		*/
		//-----------------------------------------------------------------//
		state get_state()
		{
			if(!open_) return state::none;
			if(info_.active_) return state::select;

			pthread_mutex_lock(&info_.sync_);
			bool ret = id_ != info_.id_;
			pthread_mutex_unlock(&info_.sync_);
			if(!ret) {  // cancel
				open_ = false;
			}
			return ret ? state::selected : state::cancel;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォルダ・パスを取得
			@return フォルダ・パス
		*/
		//-----------------------------------------------------------------//
		std::string get()
		{
			std::string path;
			if(get_state() == state::selected) {
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
