#pragma once
#include "files.hpp"

namespace utils {

	class vfs {

		files&	files_;

		static const uint32_t cash_size_ = 8192;

		open_mode	open_mode_;

		std::vector<uint16_t>	list_;
		std::vector<uint8_t>	cash_;

		uint32_t	handle_;

	public:
		vfs(files& fs) : files_(fs), open_mode_(open_mode::none), handle_(0) { }

		void init() {
		}

		bool open(const std::string& path, open_mode mode) {
			handle_ = files_.install(path);
			if(handle_ == 0) {
				return false;
			}
			open_mode_ = mode;


			return true;
		}


		int read(const void* ptr, uint32_t len) {
			if(open_mode_ != open_mode::read) {
				return -1;
			}



			return 0;
		}


		int write(void* ptr, uint32_t len) {
			if(open_mode_ != open_mode::write) {
				return -1;
			}


			return 0;
		}


		int seek(seek_mode mode, uint32_t offset) {



			return static_cast<int>(offset);
		}


		int close() {
			if(open_mode_ == open_mode::none) {
				return -1;
			}
			open_mode_ = open_mode::none;
			return 0;
		}

	};
}
