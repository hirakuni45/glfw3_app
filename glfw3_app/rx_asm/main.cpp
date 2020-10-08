//=====================================================================//
/*!	@file
	@breif	RX マイコン、アセンブラ
			Copyright (C) 2020, Hiramatsu Kunihito
	@author	平松邦仁( hira@rvf-rc45.net )
*/
//=====================================================================//
#include <iostream>
#include <cstring>
#include "utils/format.hpp"

#include "rxasm.hpp"
#include "symbol.hpp"

// タイトルの表示と簡単な説明
static void title(const char *cmd)
{
	char	*p;
	char	tmp[128];

	p = strrchr(cmd, '\\');
	if(p != NULL) {
		strcpy(tmp, p + 1);
	} else {
		p = strrchr(cmd, '/');
		if(p != NULL) {
			strcpy(tmp, p+1);
		} else {
			strcpy(tmp, cmd);
		}
	}

	p = strrchr(tmp, '.');
	if(p != NULL) *p = 0;

	utils::format("RX Assembler\n");
	utils::format("Copyright (C) 2020, Hiramatsu Kunihito\n");
	utils::format("usage:\n");
	utils::format("    %s [options] file\n") % tmp;
	utils::format("\n");
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
	char*	fname = NULL;
	bool	opterr = false;

	for(int i = 1; i < argc; ++i) {
		char* p = argv[i];
		if(p[0] == '-') {
//			opterr = true;
		} else {
			fname = p;
		}
	}

	if(opterr == true) {
		utils::format("Illegual option...\n\n");
		title(argv[0]);
		return 1;
	}

	if(fname == nullptr) {
		title(argv[0]);
		return 1;
	}

	renesas::rxasm rxasm;

	rxasm.assemble(fname);

	return 0;
}

/* ---- End Of File "main.cpp" ----- */
