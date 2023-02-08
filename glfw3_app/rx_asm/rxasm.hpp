#pragma once
//=====================================================================//
/*!	@file
	@brief	RX Assembler
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <unordered_map>
#include <string>

#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"
#include "symbol.hpp"

namespace renesas {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	RX Assembler クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class rxasm {

		static const uint16_t ADRM_NONE = 0b0000'0000'0000'0000;
		static const uint16_t ADRM_REG  = 0b0000'0000'0000'0001;

		struct adrm_t {
			uint16_t	src_;
			uint16_t	dst_;
			adrm_t(uint16_t s = ADRM_NONE, uint16_t d = ADRM_NONE) : src_(s), dst_(d) { }
		};

		typedef std::string KEY;
		typedef std::unordered_map<KEY, adrm_t> MAP;

		MAP		map_;

		// シンボルの収集と、簡単なエラー検査、サイズの計算
		bool pass1_(utils::file_io& fin)
		{
			uint32_t lno = 0;
			while(!fin.eof()) {
				++lno;
				auto l = fin.get_line();
				auto ll = utils::split_text(l, " \t", "", 4);

			}
utils::format("Pass1: %d lines\n") % lno;
			return true;
		}

		// 最終的なバイナリー出力
		bool pass2_(utils::file_io& fin)
		{
			fin.seek(0, utils::file_io::SEEK::SET);
			uint32_t lno = 0;
			while(!fin.eof()) {
				++lno;
				auto l = fin.get_line();
				auto ll = utils::split_text(l, " \t", "", 4);

			}
utils::format("Pass2: %d lines\n") % lno;
			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		rxasm() :
			map_{
			{"ABS",   { ADRM_NONE, ADRM_REG }},
			{"ADC",   {}},
			{"ADD",   {}},
			{"AND",   {}},
			{"BCLR",  {}},
			{"BCnd",  {}},
			{"BMCnd", {}},
			{"BNOT",  {}},
			{"BRA",   {}},
			{"BRK",   {}},
			{"BSET",  {}},
			{"BSR",   {}},
			{"BTST",  {}},
			{"CLRPSW",{}},
			{"CMP",   {}},
			{"DIV",   {}},
			{"DIVU",  {}},
			{"EMUL",  {}},
			{"EMULU", {}},
			{"FADD",  {}},
			{"FCMP",  {}},
			{"FDIV",  {}},
			{"FMUL",  {}},
			{"FSUB",  {}},
			{"FTOI",  {}},
			{"INT",   {}},
			{"ITOF",  {}},
			{"JMP",   {}},
			{"JSR",   {}},
			{"MACHI", {}},
			{"MACLO", {}},
			{"MAX",   {}},
			{"MIN",   {}},
			{"MOV",   {}},
			{"MOVU",  {}},
			{"MUL",   {}},
			{"MULHI", {}},
			{"MULLO", {}},
			{"MVFACHI",{}},
			{"MVFACMI",{}},
			{"MVFC",  {}},
			{"MVTACHI",{}},
			{"MVTACLO",{}},
			{"MVTC",  {}},
			{"MVTIPL",{}},
			{"NEG",   {}},
			{"NOP",   {}},
			{"NOT",   {}},
			{"OR",    {}},
			{"POP",   {}},
			{"POPC",  {}},
			{"POPM",  {}},
			{"PUSH",  {}},
			{"PUSHC", {}},
			{"PUSGM", {}},
			{"RACW",  {}},
			{"REVL",  {}},
			{"REVW",  {}},
			{"RMPA",  {}},
			{"ROLC",  {}},
			{"RORC",  {}},
			{"ROTL",  {}},
			{"ROTR",  {}},
			{"ROUND", {}},
			{"RTE",   {}},
			{"RTFI",  {}},
			{"RTS",   {}},
			{"RTSD",  {}},
			{"SAT",   {}},
			{"SATR",  {}},
			{"SBB",   {}},
			{"SCCnd", {}},
			{"SCMPU", {}},
			{"SETPSW",{}},
			{"SHAR",  {}},
			{"SHLL",  {}},
			{"SHLR",  {}},
			{"SMOVB", {}},
			{"SMOVF", {}},
			{"SMOVU", {}},
			{"SSTR",  {}},
			{"STNZ",  {}},
			{"STZ",   {}},
			{"SUB",   {}},
			{"SUNTIL",{}},
			{"SWHILE",{}},
			{"TST",   {}},
			{"WAIT",  {}},
			{"XCHG",  {}},
			{"XOR",   {}}
		  }
		{ }
 

		//-----------------------------------------------------------------//
		/*!
			@brief  アセンブル
			@param[in]	filename	ソース・ファイル名
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool assemble(const char* filename)
		{
			utils::file_io	fin;

			if(!fin.open(filename, "rb")) {
				return false;
			}

			if(!pass1_(fin)) {
				return false;
			}

			if(!pass2_(fin)) {
				return false;
			}

			fin.close();

			return true;
		}
	};
}
