//=====================================================================//
/*!	@file
	@brief	ディレクトリー情報取得クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "files.hpp"
#include <unistd.h>

namespace utils {
#if 0
	static sem_t* recv_ = 0;

	static void* files_task_(void* ptr)
	{
		files::files_io* io = static_cast<files::files_io*>(ptr);
		while(!io->exit_) {
///			sem_wait(&io->recv_);
			int ret = sem_wait(recv_);
			std::cout << "Task: " << io->path_ << " " << ret << std::endl;
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
		return;
		files_io_ = new files_io;
		std::cout << "Task init" << std::endl;
		pthread_attr_init(&files_io_->attr_);
		pthread_attr_setdetachstate(&files_io_->attr_, PTHREAD_CREATE_DETACHED);
		sem_init(&files_io_->send_, 0, 0);
		sem_init(&files_io_->recv_, 0, 0);
		recv_ = sem_open("files_recv", O_CREAT, 0777, 0);
		pthread_create(&files_io_->pth_t_, &files_io_->attr_, files_task_, files_io_);
	}


	void files::destroy_task_()
	{
		return;

		if(files_io_ == 0) return;
		files_io_->exit_ = true;
		sem_post(&files_io_->recv_);
		sem_post(recv_);
		pthread_join(files_io_->pth_t_, 0);
		sem_destroy(&files_io_->recv_);
		sem_destroy(&files_io_->send_);
///		sem_destroy(recv_);
		sem_close(recv_);
		sem_unlink("files_recv");
		pthread_attr_destroy(&files_io_->attr_);
		delete files_io_;
		files_io_ = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報取得かどうか？
		@return ファイル情報取得なら「true」
	*/
	//-----------------------------------------------------------------//
	bool files::probe()
	{
#if 0
		if(files_io_ == 0) return false;
		if(sem_trywait(&files_io_->send_) == 0) {
			file_infos_ = files_io_->file_infos_;
			return true;
		} else {
			return false;
		}
#endif
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報郡を取得
		@return ファイル情報郡
	*/
	//-----------------------------------------------------------------//
	const file_infos& files::get()
	{
///		probe();
		return file_infos_;
	}
#endif
}
