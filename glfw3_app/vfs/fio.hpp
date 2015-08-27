#pragma once
#include <array>
#include "vfs.hpp"
#include "handle_set.hpp"

namespace utils {

	class fio {
		files&	files_;

		static const uint32_t files_max_ = 16;

		typedef handle_set<uint16_t> hndset;
		hndset	hndset_;

		typedef std::vector<vfs> vfss;
		vfss	vfss_;

	public:
		fio(files& fs) : files_(fs), hndset_(files_max_), vfss_(files_max_, vfs(files_)) { }

		uint32_t open(const std::string& path, open_mode mode) {
			uint32_t hnd = hndset_.create();
			vfs& fs = vfss_[hnd];
			if(!fs.open(path, mode)) {
				hndset_.erase(hnd);
				return 0;
			}
			return hnd;
		}


		int read(uint32_t hnd, const void* ptr, uint32_t len) {
			if(!hndset_.probe(hnd)) {
				return -1;
			}
			return vfss_[hnd].read(ptr, len);
		}


		int write(uint32_t hnd, void* ptr, uint32_t len) {
			if(!hndset_.probe(hnd)) {
				return -1;
			}
			return vfss_[hnd].write(ptr, len);
		}


		int seek(uint32_t hnd, seek_mode mode, uint32_t offset) {
			if(!hndset_.probe(hnd)) {
				return -1;
			}
			return vfss_[hnd].seek(mode, offset);
		}


		int close(uint32_t hnd) {
			if(!hndset_.probe(hnd)) {
				return -1;
			}
			return vfss_[hnd].close();
		}

	};
}
