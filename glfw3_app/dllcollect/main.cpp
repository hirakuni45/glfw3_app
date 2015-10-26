#include <iostream>
#include <string>
#include <set>
#include <cstdio>
#include <cstdlib>
#include "utils/sjis_utf16.hpp"
#include "utils/file_io.hpp"

int main(int argc, char* argv[]);

#if 0
bool scan_basic_dll_(const std::string& s)
{
	if(s.find("KERNEL32.dll") == std::string::npos) return true;
	else if(s.find("msvcrt.dll") == std::string::npos) return true;
}
#endif

typedef std::set<std::string> dll_set;
static dll_set dll_set_;

static bool scan_dll_(const std::string& base, const std::string& path, dll_set& set)
{
	std::string cmd("objdump -p ");
	cmd += base;
	cmd += " > " + std::string("dlllist");
	system(cmd.c_str());

	utils::file_io fin;
	if(!fin.open("dlllist", "rb")) {
		return false;
	}

	while(!fin.eof()) {
		std::string l;
		fin.get_line(l);
		std::string key("DLL Name: ");
		size_t pos = l.find(key);
		if(pos != std::string::npos) {
			std::string name = l.substr(pos + key.size());
//			std::cout << name << std::endl;
			if(utils::probe_file(path + '/' + name)) {
				set.insert(name);
			}
		}
	}
	fin.close();
	return true;
}

int main(int argc, char* argv[])
{
	utils::init_utf16_to_sjis();

	std::string dll_path("c:/msys64/mingw64/bin");

	if(scan_dll_("player.exe", dll_path, dll_set_)) {
		size_t n = 0;
		do {
			n = dll_set_.size();
			for(auto s : dll_set_) {
				scan_dll_(dll_path + '/' + s, dll_path, dll_set_);
			}
		} while(n < dll_set_.size()) ;

		for(auto s : dll_set_) {
			auto path = dll_path + '/' + s;
			std::cout << " cp " + path + " dlls/." << std::endl;
		}
	}
}
