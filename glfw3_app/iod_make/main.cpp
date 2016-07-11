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
		std::cerr << "Error: analize input file: '" << in_file << "'" << std::endl;
		return -1;
	}

	if(!dfi.analize()) {
		std::cerr << "Error: define file analize: " << dfi.get_last_error() << std::endl;
		return -1;
	}


	return 0;
	iod_make iod;

	iod.start("RL78/G13 グループ・ポート・レジスター定義",
			  "Copyright 2016 Kunihito Hiramatsu",
			  "",
			  "平松邦仁 (hira@rvf-rc45.net)");
	{
		iod_make::reg_t reg;

		reg.title   = "ポート制御レジスター";
		reg.base    = "io8";
		reg.address = "0xff0001";
		reg.local   = "inpdir";
		reg.name    = "INPDIR";

		iod_make::bits_type bits;
		bits.emplace_back("bit_t", "6", "TRJIOSEL",
			"TRJIO 入力信号選択（0: 外部 TRJIO端子から、1: VCOUT1 から内部入力）");
		iod.add(reg, bits);
	}


	if(!out_file.empty()) {
		if(!iod.save(out_file)) {
			std::cerr << boost::format("Write error output file: '%1%'") % out_file << std::endl;
		}
	}
}

// EOF
