//=====================================================================//
/*!	@file
	@brief	ディレクトリー情報取得クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "files.hpp"
#include <unistd.h>
// #include <time.h>

namespace utils {

	void files::sleep_(uint32_t ms)
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

	void* files::task_(void* in)
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

	void files::start_()
	{
		if(init_ == 0) {
			pthread_mutex_init(&file_t_.sync_, nullptr);
			pthread_create(&pth_, nullptr, task_, &file_t_);
		}
		++init_;
	}


	void files::end_()
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
}
