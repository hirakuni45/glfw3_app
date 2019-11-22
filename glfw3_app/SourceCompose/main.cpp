//=====================================================================//
/*!	@file
	@breif	C++ ソースとヘッダーを合成する
			Copyright (C) 2019, Hiramatsu Kunihito
	@author	平松邦仁( hira@rvf-rc45.net )
*/
//=====================================================================//
#include <iostream>
#include <string>
#include <cstring>
#include "utils/format.hpp"

#include "compose.hpp"

// タイトルの表示と簡単な説明
static void title(const char *cmd)
{
	char	*p;
	char	buff[256];

	p = strrchr(cmd, '\\');
	if(p != nullptr) {
		strcpy(buff, p+1);
	} else {
		p = strrchr(cmd, '/');
		if(p != nullptr) {
			strcpy(buff, p+1);
		} else {
			strcpy(buff, cmd);
		}
	}

	p = strrchr(buff, '.');
	if(p != nullptr) *p = 0;

	using namespace std;
	cout << "Source and Header Compose" << endl;
	cout << "Copyright (C) 2019, Hiramatsu Kunihito" << endl;
	cout << "usage:" << endl;
	cout << "	" << buff << " -h header-file -s source_file" << endl;
	cout << endl;
}


//-----------------------------------------------------------------//
/*!
	@breif	main 関数
	@param[in]	argc	コマンド入力パラメーターの数	
	@param[in]	argv	コマンド入力パラメーターのリスト
	@return	常に０
*/
//-----------------------------------------------------------------//
int main(int argc, char *argv[])
{
	const char* source = nullptr;
	const char* header = nullptr;
	const char* fname = nullptr;
	bool s = false;
	bool h = false;
	bool opterr = false;
	for(int i = 1; i < argc; ++i) {
		char* p = argv[i];
		if(p[0] == '-') {
			if(strcmp(p, "-h")==0) h = true;
			else if(strcmp(p, "-s")==0) s = true;
			else opterr = true;
		} else {
			if(h) {
				header = p;
				h = false;
			} else if(s) {
				source = p;
				s = false;
			} else {
				fname = p;
			}
		}
	}

	if(opterr == true) {
		std::cerr << "Illegual option..." << std::endl << std::endl;
		title((const char *)argv[0]);
		return 1;
	}


	if(header != nullptr) {
		utils::format("Header file: '%s'\n") % header;
	}
	if(source != nullptr) {
		utils::format("Source file: '%s'\n") % source;
	}


	text::compose cmpo;

	if(!cmpo.load_header(header)) {
		utils::format("Can't open header: '%s'\n") % header;
	}

	cmpo.analize_header();

	if(!cmpo.load_source(source)) {
		utils::format("Can't open source: '%s'\n") % source;
	}

	cmpo.analize_source();

	cmpo.list_all();
}
