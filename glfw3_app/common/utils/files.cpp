//=====================================================================//
/*!	@file
	@brief	ディレクトリー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "files.hpp"
#include <unistd.h>

namespace utils {

	static void* files_task_(void* ptr)
	{
		files::files_io* io = static_cast<files::files_io*>(ptr);
		sem_post(&io->start_);
		while(!io->exit_) {
			sem_wait(&io->recv_);
			if(io->exit_) break;
			io->file_infos_.clear();
			if(io->exts_.empty()) {
				create_file_list(io->path_, io->file_infos_);
			} else {
				file_infos fis;
   				create_file_list(io->path_, fis);
				filter_file_infos(fis, io->exts_, io->file_infos_);
			}
			sem_post(&io->send_);
		}
		return 0;
	}


	void files::init_task_()
	{
		sem_init(&files_io_.start_, 0, 0);
		sem_init(&files_io_.send_, 0, 0);
		sem_init(&files_io_.recv_, 0, 0);
		pthread_create(&files_io_.pth_t_, 0, files_task_, &files_io_);
	}


	void files::destroy_task_()
	{
		files_io_.exit_ = true;
		sem_post(&files_io_.recv_);
		pthread_join(files_io_.pth_t_, 0);
		sem_destroy(&files_io_.recv_);
		sem_destroy(&files_io_.send_);
		sem_destroy(&files_io_.start_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報取得かどうか？
		@return ファイル情報取得なら「true」
	*/
	//-----------------------------------------------------------------//
	bool files::probe()
	{
		if(sem_trywait(&files_io_.send_) == 0) {
			file_infos_ = files_io_.file_infos_;
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報郡を取得
		@return ファイル情報郡
	*/
	//-----------------------------------------------------------------//
	const file_infos& files::get()
	{
		probe();
		return file_infos_;
	}
}
