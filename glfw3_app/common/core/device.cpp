//=====================================================================//
/*!	@file
	@brief	デバイス用ラッパークラス @n
			「windows.h」をインクルードする弊害が色々とあるので、このソースに集約している。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <GLFW/glfw3.h>
#include <sys/stat.h>
#include "core/device.hpp"
#include "utils/string_utils.hpp"
#include "utils/drive_info.hpp"

#ifdef WIN32
#include <windows.h>
#endif

namespace utils {

	std::string system_path(const std::string& path)
	{
#ifdef WIN32
		auto ws = utils::utf8_to_utf16(path);
		auto l = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ws.c_str(), ws.size(), NULL, 0, NULL, NULL);
		if(l <= 0) {
			return std::string();
		}
		char buff[l + 1];
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ws.c_str(), ws.size(), buff, l, NULL, NULL);
		buff[l] = 0;
		return std::string(buff);
#else
		return path;
#endif
	}


	std::FILE* wfopen(const utils::lstring& fn, const std::string& md)
	{
		std::FILE* fp = 0;
#ifdef WIN32
		wchar_t* wsm = new wchar_t[md.size() + 1];
		for(uint32_t i = 0; i < md.size(); ++i) {
			wsm[i] = md[i];
		}
		wsm[md.size()] = 0;
		wchar_t* wfn = new wchar_t[fn.size() + 1];
		for(uint32_t i = 0; i < fn.size(); ++i) {
			wfn[i] = fn[i];
		}
		wfn[fn.size()] = 0;
		fp = _wfopen(wfn, wsm);
		delete[] wfn;
		delete[] wsm;
#else
		std::string s;
		utils::utf32_to_utf8(fn, s);
		fp = fopen(s.c_str(), md.c_str());
#endif
#ifndef NDEBUG
		if(fp == 0) {
			std::string s;
			utils::utf32_to_utf8(fn, s);
			std::string tt;
			if(strchr(md.c_str(), 'w')) tt = "output";
			else tt = "input";
			std::cerr << boost::format("Can't open %1% file (file_io::wfopen): '%2%'") % tt % s << std::endl;
		}
#endif
		return fp;
	}


	bool create_directory(const std::string& dir)
	{
		bool ret = true;
#ifdef WIN32
		utils::wstring ws;
		utf8_to_utf16(dir, ws);
		if(_wmkdir((const wchar_t*)ws.c_str()) != 0) ret = false;
#else
		mode_t t = S_IRWXU | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH);
		if(mkdir(dir.c_str(), t) != 0) ret = false;
#endif
		return ret;
	}


	bool is_directory(const std::string& fn)
	{
#ifdef WIN32
		struct _stat st;
		wchar_t* wfn = new wchar_t[fn.size() + 1];
		for(uint32_t i = 0; i < fn.size(); ++i) {
			wfn[i] = fn[i];
		}
		wfn[fn.size()] = 0;
		int ret = _wstat(wfn, &st);
		delete[] wfn;
		if(ret == 0) {
			return S_ISDIR(st.st_mode) != 0;
		}
#else
		struct stat st;
		if(stat(fn.c_str(), &st) == 0) {
			return S_ISDIR(st.st_mode) != 0;
		}
#endif
		return false;
	}


	bool probe_file(const utils::lstring& fn, bool dir)
	{
#ifdef WIN32
		struct _stat st;
		wchar_t* wfn = new wchar_t[fn.size() + 1];
		for(uint32_t i = 0; i < fn.size(); ++i) {
			wfn[i] = fn[i];
		}
		wfn[fn.size()] = 0;
		int ret = _wstat(wfn, &st);
		delete[] wfn;
		if(ret == 0) {
#else
		std::string s;
		utf32_to_utf8(fn, s);
		struct stat st;
		if(stat(s.c_str(), &st) == 0) {
#endif
			if(dir) {
				bool d = S_ISDIR(st.st_mode);
				if(d) return true;
			} else {
				return true;
			}
		}
		return false;
	}


	bool remove_file(const std::string& fn)
	{
#ifdef WIN32
		auto fname = utf8_to_sjis(fn);
		return remove(fname.c_str()) == 0;
#else
		return remove(fn.c_str()) == 0;
#endif
	}


	void drive_info::initialize_()
	{
			infos_.clear();
#ifdef WIN32
			drives_ = GetLogicalDrives();
#else
			drives_ = 0;
#endif
#ifdef WIN32
			uint32_t drvbits = drives_;

			uint32_t bits = 1;
			int idx = 0;
			while(drvbits) {
				drive::type t = drive::none;
				if(drvbits & bits) {
					char drvtxt[4];
					drvtxt[0] = 'A' + idx;
					drvtxt[1] = ':';
					drvtxt[2] = '\\';
					drvtxt[3] = 0;
					switch(GetDriveType(drvtxt)) {
					case DRIVE_UNKNOWN:
						t = drive::unknown;
						break;
					case DRIVE_NO_ROOT_DIR:
						t = drive::no_root;
						break;
					case DRIVE_REMOVABLE:
						t = drive::removable;
						break;
					case DRIVE_FIXED:
						t = drive::fixed;
						break;
					case DRIVE_REMOTE:
						t = drive::remote;
						break;
					case DRIVE_CDROM:
						t = drive::cdrom;
						break;
					case DRIVE_RAMDISK:
						t = drive::ramdisk;
						break;
					default:
						t = drive::none;
						break;
					}
					if(t != drive::none) {
						info_t info;
						info.drive_ = idx;
						info.type_ = t;
						infos_.push_back(info);
					}
					drvbits &= ~bits;
				}
				idx++;
				bits <<= 1;
			}
#endif
		}
}

namespace gl {

	//-----------------------------------------------------------------//
	/*!
		@brief	サービス@n
				サンプリングと状態の作成
		@param[in]	bits	スイッチの状態
		@param[in]	poss	位置情報
	*/
	//-----------------------------------------------------------------//
	void device::service(const bits_t& bits, const locator& poss)
	{
		bits_t b = bits;
		int joy = GLFW_JOYSTICK_1;  // first JOY-STICK
		if(glfwJoystickPresent(joy) == GL_TRUE) {
			int count;
			const unsigned char* hats = glfwGetJoystickHats(joy, &count);
			if((hats[0] & GLFW_HAT_UP) != 0) {
				b.set(key::GAME_UP);
			}
			if((hats[0] & GLFW_HAT_DOWN) != 0) {
				b.set(key::GAME_DOWN);
			}
			if((hats[0] & GLFW_HAT_LEFT) != 0) {
				b.set(key::GAME_LEFT);
			}
			if((hats[0] & GLFW_HAT_RIGHT) != 0) {
				b.set(key::GAME_RIGHT);
			}
//			const float* axes = glfwGetJoystickAxes(joy, &count);
//			for(int i = 0; i < count; ++i) {
//				if(i == 1) {
//					if(axes[i] > 0.5f) b.set(key::GAME_UP);
//					else if(axes[i] < -0.5f) b.set(key::GAME_DOWN);
//				} else if(i == 0) {
//					if(axes[i] > 0.5f) b.set(key::GAME_RIGHT);
//					else if(axes[i] < -0.5f) b.set(key::GAME_LEFT);
//				}
//			}
			const unsigned char* bl = glfwGetJoystickButtons(joy, &count);
			if(count > 16) count = 16;
			for(int i = 0; i < count; ++i) {
				if(bl[i] != 0) b.set(static_cast<key>(static_cast<int>(key::GAME_0) + i));
			}
		}

		b.set(key::STATE_CAPS_LOCK, level_.test(key::STATE_CAPS_LOCK));
		b.set(key::STATE_SCROLL_LOCK, level_.test(key::STATE_SCROLL_LOCK));
			  b.set(key::STATE_NUM_LOCK, level_.test(key::STATE_NUM_LOCK));

		positive_ =  b & ~level_;
		negative_ = ~b &  level_;

		if(positive_.test(key::CAPS_LOCK)) {
			b.flip(key::STATE_CAPS_LOCK);
		}
		if(positive_.test(key::SCROLL_LOCK)) {
			b.flip(key::STATE_SCROLL_LOCK);
		}
		if(positive_.test(key::NUM_LOCK)) {
			b.flip(key::STATE_NUM_LOCK);
		}

		level_ = b;

		locator_ = poss;
	}
}
