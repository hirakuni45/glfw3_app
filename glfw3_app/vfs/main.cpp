//=====================================================================//
/*! @file
	@brief  main
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <vector>
#include "fio.hpp"

int main(int argc, char** argv)
{
	utils::files fs("VFS");
	utils::fio fio(fs);

	fs.mkdir("qwe");
	fs.mkdir("asd");
	fs.mkdir("zxc");

	fs.set_current_directory("asd");
	fs.install("asdx");
	fs.mkdir("poi");
	fs.install("mnbv");

	fs.list();

	fs.erase("asdx");

	fs.list();

	fs.set_current_directory("/");
	fs.rmdir("asd");

	fs.list();
}
