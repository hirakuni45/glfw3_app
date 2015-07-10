#pragma once
//=====================================================================//
/*!	@file
	@brief	ディレクトリー情報取得クラス（ヘッダー）@n
			ディレクトリー情報取得をスレッドにて並行して行う
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <string>
#include "utils/drive_info.hpp"
#include "utils/file_info.hpp"
#include "utils/string_utils.hpp"
#include <pthread.h>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ディレクトリー情報取得クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class files {

		struct file_t {
			volatile bool		loop_;
			volatile uint32_t	idx_;
			pthread_mutex_t		sync_;
			std::string			path_;
			std::string			filter_;
			file_infos			infos_;
			file_t() : loop_(true), idx_(0) { }
		};

		volatile uint32_t	idx_;

		uint32_t	init_;
		file_t		file_t_;
		pthread_t	pth_;

		void start_();
		void end_();

		static void sleep_(uint32_t ms);
		static void* task_(void* in);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	path	パス
		*/
		//-----------------------------------------------------------------//
		files() : idx_(0), init_(0) { start_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~files() { end_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ルートパスを設定
			@param[in]	path	パス
			@param[in]	filter	拡張子フィルター
		*/
		//-----------------------------------------------------------------//
		void set_path(const std::string& path, const std::string& filter = "") {
			pthread_mutex_lock(&file_t_.sync_);
			idx_ = file_t_.idx_;
			file_t_.path_ = path;
			file_t_.filter_ = filter;
			++file_t_.idx_;
			pthread_mutex_unlock(&file_t_.sync_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ルートパスを取得
			@return ルートパス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_path() const { return file_t_.path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	拡張子を取得
			@return 拡張子
		*/
		//-----------------------------------------------------------------//
		const std::string& get_exts() const { return file_t_.filter_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報取得かどうか？
			@return ファイル情報取得なら「true」
		*/
		//-----------------------------------------------------------------//
		bool probe() { return idx_ != file_t_.idx_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報郡を取得
			@return ファイル情報郡
		*/
		//-----------------------------------------------------------------//
		const file_infos& get() {
			while(!probe()) {
				sleep_(10);
			}
			return file_t_.infos_;
		}
	};
}
