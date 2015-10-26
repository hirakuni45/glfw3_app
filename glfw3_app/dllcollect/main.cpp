#include <iostream>
#include <string>
#include <set>
#include <boost/format.hpp>
#include <cstdio>
#include <cstdlib>
#include "utils/sjis_utf16.hpp"
#include "utils/file_io.hpp"

static const float version_ = 0.01f;

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

///	std::cout << cmd << std::endl;

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


static std::string true_path_(const std::string& path)
{
	std::string s;
	utils::code_conv(utils::sjis_to_utf8(path), '\\', '/', s);
	return s;
}


static void help_(const std::string& cmd)
{
	using namespace std;

	auto s = true_path_(cmd);

	cout << "DLL collect copy" << endl;
	cout << "Copyright (C) 2015, Hiramatsu Kunihito" << endl;
	cout << "Version " << boost::format("%1.2f") % version_ << endl;
	cout << "usage:" << endl;
	string cm;
	auto n = s.rfind('/');
	if(n != std::string::npos) { cm = s.substr(n + 1); }
	else { cm = s; }
	cout << "    " << utils::get_file_base(cm) << " [options] base-file" << endl;
//	cout << "    -verbose          verbose" << endl;
	cout << endl;
}


int main(int argc, char* argv[])
{
	utils::init_utf16_to_sjis();

	if(argc < 2) {
		help_(argv[0]);
		return 0;
	}

	auto exe = true_path_(argv[1]);
	std::string dll_path("c:/msys64/mingw64/bin");
	std::string dir_name("dlls");
	
	if(scan_dll_(exe, dll_path, dll_set_)) {
		size_t n = 0;
		do {
			n = dll_set_.size();
			for(auto s : dll_set_) {
				scan_dll_(dll_path + '/' + s, dll_path, dll_set_);
			}
		} while(n < dll_set_.size()) ;

		system(std::string("rm -rf " + dir_name).c_str());
		system(std::string("mkdir " + dir_name).c_str());

		for(auto s : dll_set_) {
			auto path = dll_path + '/' + s;
			auto copy = "cp " + path + ' ' + dir_name + "/.";
//			std::cout << copy << std::endl;
			system(copy.c_str());
		}
	}
}
