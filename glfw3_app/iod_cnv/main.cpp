//=====================================================================//
/*! @file
	@brief  main
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "iod_make.hpp"

int main(int argc, char** argv)
{
	using namespace utils;

	iod_make iod;

	iod.start("RL78/G13 グループ・ポート・レジスター定義");
	{
		iod_make::reg_t reg;

		reg.title   = "ポート制御レジスター";
		reg.base    = "io8";
		reg.address = "0xff0001";
		reg.local   = "inpdir";
		reg.name    = "INPDIR";

		iod.add(reg);
	}

	iod.save("test.hpp");
}

// EOF
