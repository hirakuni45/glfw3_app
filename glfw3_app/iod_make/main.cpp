//=====================================================================//
/*! @file
	@brief  I/O デバイス・メーカー・メイン
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/format.hpp>
#include "iod_make.hpp"
#include "def_in.hpp"

namespace {

	const std::string version_("0.01b");


	void title_(const std::string& cmd)
	{
		using namespace std;

		std::string c = utils::get_file_base(cmd);

		cout << "I/O Device Template Class Maker Version " << version_ << endl;
		cout << "Copyright (C) 2016, Hiramatsu Kunihito (hira@rvf-rc45.net)" << endl;
		cout << "usage:" << endl;
		cout << c << "[options] [config_file] [out_file]" << endl;
		cout << endl;
//		cout << "Options :" << endl;
//		cout << "" << endl;
	}
}

int main(int argc, char** argv)
{
	std::string in_file;
	std::string out_file;
	bool verbose = false;
	for(int i = 1; i < argc; ++i) {
		std::string s = argv[i];
		if(!s.empty() && s[0] == '-') {
			if(s == "-v" || s == "--verbose") {
				verbose = true;
			}
		}
		in_file = out_file;
		out_file = s;
	}

	if(in_file.empty() || out_file.empty()) {
		title_(argv[0]);
		return 0;
	}

	using namespace utils;

	def_in dfi(verbose);
	if(!dfi.load(in_file)) {
		std::cerr << "Error: load file: '" << in_file << "'" << std::endl;
		return -1;
	}

	if(!dfi.analize()) {
		std::cerr << "Error: analize input file: " << dfi.get_last_error() << std::endl;
		return -1;
	}

	iod_make iod;
	iod.start(dfi.get_base());

	for(const auto& t : dfi.get_classies()) {
		iod.add(t);
	}

	if(!out_file.empty()) {
		if(!iod.save(out_file)) {
			std::cerr << boost::format("Write error output file: '%1%'") % out_file << std::endl;
		}
	}
}

// EOF
