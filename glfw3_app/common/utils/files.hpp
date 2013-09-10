#pragma once
//=====================================================================//
/*!	@file
	@brief	ディレクトリー・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include "utils/drive_info.hpp"
#include "utils/file_info.hpp"
#include "utils/string_utils.hpp"
#include "utils/handle_set.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ディレクトリー・クラス@n
				ディレクトリー情報を格納
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class files {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	パス・タスク構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct files_io {
			pthread_t	pth_t_;
			sem_t	start_;
			sem_t	send_;
			sem_t	recv_;

			std::string	path_;
			std::string	exts_;

			file_infos	file_infos_;

			volatile bool	exit_;

			bool set_path(const std::string& path, const std::string& exts) {
				if(path.empty()) return false;
				path_ = path;
				exts_ = exts;
				return true;
			}
			files_io() : exit_(false) { }
		};

	private:
		files_io   	files_io_;

		file_infos	file_infos_;

		bool	start_;

		void init_task_();
		void destroy_task_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	path	パス
		*/
		//-----------------------------------------------------------------//
		files() : start_(false) { init_task_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~files() { destroy_task_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ルートパスを設定
			@param[in]	path	パス
			@param[in]	ext		拡張子フィルター
		*/
		//-----------------------------------------------------------------//
		void set_path(const std::string& path, const std::string& ext = "") {
			if(!start_) {
				sem_wait(&files_io_.start_);
				start_ = true;
			}
			if(files_io_.set_path(path, ext)) {
				sem_post(&files_io_.recv_);
				file_infos_.clear();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報取得かどうか？
			@return ファイル情報取得なら「true」
		*/
		//-----------------------------------------------------------------//
		bool probe();


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報郡を取得
			@return ファイル情報郡
		*/
		//-----------------------------------------------------------------//
		const file_infos& get();
	};

}
