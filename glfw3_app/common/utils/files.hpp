#pragma once
//=====================================================================//
/*!	@file
	@brief	ディレクトリー情報取得クラス @n
			ディレクトリー情報取得をスレッドにて並行して行う
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <string>
#include "utils/drive_info.hpp"
#include "utils/file_info.hpp"
#include "utils/string_utils.hpp"
#include <pthread.h>
#include <unistd.h>

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
			volatile uint32_t	ans_;
			pthread_mutex_t		sync_;
			std::string			path_;
			std::string			filter_;
			file_infos			infos_;
			file_t() : loop_(true), idx_(0), ans_(0) { }
		};

		volatile uint32_t	ans_;

		uint32_t	init_;
		file_t		file_t_;
		pthread_t	pth_;

		static void sleep_(uint32_t ms)
		{
			usleep(ms * 1000);
#if 0
			struct timespec in;
			in.tv_sec = 0;
			in.tv_nsec = ms * 1000 * 1000;
			struct timespec out;
			out.tv_sec = 0;
			out.tv_nsec = 0;
			nanosleep(&in, &out);
#endif
		}

		static void* task_(void* in)
		{
			file_t& t = *(static_cast<file_t*>(in));

			volatile uint32_t idx = t.idx_;
			while(t.loop_) {
				if(idx != t.idx_) {
					pthread_mutex_lock(&t.sync_);
					t.infos_.clear();
					if(t.filter_.empty()) {
						t.infos_ = create_file_list(t.path_);
					} else {
						auto fis = create_file_list(t.path_);
						t.infos_ = filter_file_infos(fis, t.filter_);
					}
					idx = t.idx_;
					++t.ans_;
					pthread_mutex_unlock(&t.sync_);
				} else {
					sleep_(10);
				}
			}

			return nullptr;
		}

		void start_()
		{
			if(init_ == 0) {
				pthread_mutex_init(&file_t_.sync_, nullptr);
				pthread_create(&pth_, nullptr, task_, &file_t_);
			}
			++init_;
		}

		void end_()
		{
			if(init_) {
				--init_;
				if(init_ == 0) {
					file_t_.loop_ = false;
					pthread_join(pth_, nullptr);
					pthread_mutex_destroy(&file_t_.sync_);
				}
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	path	パス
		*/
		//-----------------------------------------------------------------//
		files() : ans_(0), init_(0) { start_(); }


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
			if(path.empty()) return;

			pthread_mutex_lock(&file_t_.sync_);
			ans_ = file_t_.ans_;
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
		bool probe() { return ans_ != file_t_.ans_; }


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
