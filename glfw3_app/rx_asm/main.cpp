//=====================================================================//
/*!	@file
	@brief	RX マイコン、アセンブラ
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <cstring>
#include "utils/format.hpp"

#include "rxasm.hpp"
#include "symbol.hpp"

#include "rxdisasm.hpp"

namespace {
	// タイトルの表示と簡単な説明
	void title(const char *cmd)
	{
		const char	*p;
		char	tmp[128];

		p = strrchr(cmd, '\\');
		if(p != nullptr) {
			strcpy(tmp, p + 1);
		} else {
			p = strrchr(cmd, '/');
			if(p != nullptr) {
				strcpy(tmp, p+1);
			} else {
				strcpy(tmp, cmd);
			}
		}

		p = strrchr(tmp, '.');
		if(p != nullptr) tmp[p - tmp] = 0;

		utils::format("RX DisAssembler/Assembler\n");
		utils::format("Copyright (C) 2020, 2023, Hiramatsu Kunihito\n");
		utils::format("usage:\n");
		utils::format("    %s [options] file\n") % tmp;
		utils::format("\n");
	}

	static constexpr uint8_t rx_bin_[] = {
		0x03,  							// NOP
		0x7E, 0x25, 					// ABS R5
		0xFC, 0x0F, 0x39,				// ABS R3,R9
		0xFD, 0x74, 0x24, 0xaa,			// ADC #0xAA,R4
		0xFD, 0x78, 0x25, 0xaa, 0xbb,	// ADC #0xBBAA,R5
		0xFC, 0x83, 0x3A,				// ADC R3,R10
		0x62, 0x77,						// ADD #7,R7
		0xFD, 0x6E, 0x78,				// ROTL #7,R8
		0xFD, 0x6C, 0x78,				// ROTR #7,R8
		0x06, 0xA0, 0x02, 0xAB,
		0x06, 0xA1, 0x02, 0xAB, 100,	// ADC x[a],[b]
		0b011100'10, 0x5a, 0x05, 0x01,	// ADD #0x105,R5,R10
		0b011100'10, 0x66, 0x05, 0x01,	// ADD #0x105,R6
		0b1111'1111, 0b0010'0100, 0x12,	// ADD R1,R2,R4
		0b0100'1011, 0xab,				// ADD R10,R11
		0b0100'1000, 0xab,				// ADD [R10].UB,R11
		0b0000'0110, 0b01'00'1001, 0x59, 100,	// ADD 200[R5].W,R9
		0b0000'0110, 0b11'00'1001, 0x59, 100,	// ADD 200[R5].UW,R9
		0b0000'0110, 0b10'00'1001, 0x59, 100,	// ADD 200[R5].L,R9
		0b0000'0110, 0b00'00'1001, 0x59, 100,	// ADD 200[R5].B,R9

		0x03, 0x03, 0x03,
	};
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
	renesas::rxdisasm dis_;

	uint32_t len = 0;
	while(len < sizeof(rx_bin_)) {
		std::string out;
		auto step = dis_.disasm(&rx_bin_[len], out);
		if(step == 0 || out.empty()) break;

		std::cout << out << std::endl;
		len += step;
	}

return 0;

	char*	fname = nullptr;
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
