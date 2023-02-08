#pragma once
//=====================================================================//
/*!	@file
    @brief	RX DisAssembler
    @author 平松邦仁 (hira@rvf-rc45.net)
    @copyright	Copyright (C) 2023 Kunihito Hiramatsu @n
                Released under the MIT license @n
                https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>
#include <string>

#include "utils/format.hpp"

namespace renesas {

	struct rxdis_t {

		enum class NIMO : uint8_t {
			ABS,		// 2/2
			ADC,		// 3/3
			ADD,
			AND,
			BCLR,
			BCnd,
			BMCnd,
			BNOT,
			BRA,
			BRK,		// 1/1
			BSET,
			BSR,
			BTST,
			CLRPSW,
			CMP,
			DIV,
			DIVU,
			EMUL,
			EMULA,
			EMULU,
			FADD,		// 2/2
			FCMP,		// 2/2
			FDIV,		// 2/2
			FMUL,		// 2/2
			FSQRT,
			FSUB,		// 2/2
			FTOI,
			INT,		// 1/1
			ITOF,
			JMP,		// 1/1
			JSR,		// 1/1
			MACHI,		// 1/1
			MACLO,		// 1/1
			MAX,
			MIN,
			MOV,
			MOVU,
			MUL,
			MULHI,
			MULLO,
			MVFACHI,
			MVFACMI,
			MVFC,
			MVTACHI,
			MVTACLO,
			MVTC,
			MVTIPL,
			NEG,
			NOP,		// 1/1
			NOT,
			OR,
			POP,
			POPC,
			POPM,
			PUSH,
			PUSHC,
			PUSHM,
			RACW,
			REVL,
			REVW,
			RMPA,
			ROLC,
			RORC,
			ROTL,
			ROTR,
			ROUND,		// 1/1
			RTE,		// 1/1
			RTFI,		// 1/1
			RTS,		// 1/1
			RTSD,		// 1/1
			SAT,		// 1/1
			SATR,		// 1/1
			SBB,		// 2/2
			SCCnd,
			SCMPU,
			SETPSW,
			SHAR,
			SHLL,
			SHLR,
			SMOVB,		// 1/1
			SMOVF,		// 1/1
			SMOVU,		// 1/1
			SSTR,		// 1/1
			STNZ,		// 1/1
			STZ,		// 1/1
			SUB,		// 2/2
			SUNTIL,		// 1/1
			SWHILE,		// 1/1
			TST,		// 2/2
			WAIT,		// 1/1
			XCHG,		// 2/2
			XOR			// 2/2
		};

		static constexpr const char* nimo_tbl_[] = {
			"ABS",
			"ADC",
			"ADD",
			"AND",
			"BCLR",
			"BC",	// nd: flag condition
			"BMC",	// nd: flag condition
			"BNOT",
			"BRA",
			"BRK",
			"BSET",
			"BSR",
			"BTST",
			"CLRPSW",
			"CMP",
			"DIV",
			"DIVU",
			"EMUL",
			"EMULA",
			"EMULU",
			"FADD",
			"FCMP",
			"FDIV",
			"FMUL",
			"FSQRT",
			"FSUB",
			"FTOI",
			"INT",
			"ITOF",
			"JMP",
			"JSR",
			"MACHI",
			"MACLO",
			"MAX",
			"MIN",
			"MOV",
			"MOVU",
			"MUL",
			"MULHI",
			"MULLO",
			"MVFACHI",
			"MVFACMI",
			"MVFC",
			"MVTACHI",
			"MVTACLO",
			"MVTC",
			"MVTIPL",
			"NEG",
			"NOP",
			"NOT",
			"OR",
			"POP",
			"POPC",
			"POPM",
			"PUSH",
			"PUSHC",
			"PUSHM",
			"RACW",
			"REVL",
			"REVW",
			"RMPA",
			"ROLC",
			"RORC",
			"ROTL",
			"ROTR",
			"ROUND",
			"RTE",
			"RTFI",
			"RTS",
			"RTSD",
			"SAT",
			"SATR",
			"SBB",
			"SCC",
			"SCMPU",
			"SETPSW",
			"SHAR",
			"SHLL",
			"SHLR",
			"SMOVB",
			"SMOVF",
			"SMOVU",
			"SSTR",
			"STNZ",
			"STZ",
			"SUB",
			"SUNTIL",
			"SWHILE",
			"TST",
			"WAIT",
			"XCHG",
			"XOR"
		};

		/// @brief アドレッシングモード
		enum class ADRM : uint8_t {
			ONLY,		///< 単独命令
			Rd,			///< ディストネーションレジスタ
			RsRd,		///< ソースレジスタ、ディストネーションレジスタ
			RsRsRd,		///< ソースレジスタ、ソースレジスタ、ディストネーションレジスタ
			Ofs8,		///< オフセットアドレス（s8）
			Ofs16,		///< オフセットアドレス（s16）
			Ofs24,		///< オフセットアドレス（s24）
			Imm8,		///< ソースイミディエイト（u8）
			ImxRd,		///< ソースイミディエイト（s8,s16,s24,u32）、ディストネーションレジスタ
			Im4Rd,		///< ソースイミディエイト（u4）、ディストネーションレジスタ
			Im5Rd,		///< ソースイミディエイト（u5）、ディストネーションレジスタ
			DsRsRd_f,	///< ソースディスプレースメント付きレジスタ（Rs, [Rs], dsp:8[Rs], dsp:16[Rs]）、ディストネーションレジスタ、短命令
			DsRsRd,		///< ソースディスプレースメント付きレジスタ（Rs, [Rs], dsp:8[Rs], dsp:16[Rs]）、ディストネーションレジスタ
			DsRsRd_L,	///< ソースディスプレースメント付きレジスタ（[Rs].L, dsp:8[Rs].L, dsp:16[Rs].L）、ディストネーションレジスタ、
			Im5RsRd,	///< ソースイミディエイト（u5）、ソースレジスタ、ディストネーションレジスタ
			ImxRsRd,	///< ソースイミディエイト（s8,s16,s24,u32）、ソースレジスタ、ディストネーションレジスタ
			cr,			///< コンディションレジスタ
			Im8Rd2Rd,	///< ソースイミディエイト（u8）、ディストネーションレジスタ to ディストネーションレジスタ
			Rd2Rd,		///< ディストネーションレジスタ to ディストネーションレジスタ
			Rs2Rs,		///< ソースレジスタ to ソースレジスタ
			ImfpRd,		///< ソースイミディエイト（float）、ディストネーションレジスタ
			FpRsRd,		///< FPU ソースレジスタ、ディストネーションレジスタ
			Ns,			///< ニーモニックサイズオペレーション
			bc3,		///< ブランチコンディション（８種類の分岐距離）
			bc8,		///< ブランチコンディション（-128/+127）
			bc16,		///< ブランチコンディション（-32768/+32767）
		};

		/// @brief オペコード比較長
		enum class CMP : uint8_t {
			N1 = 0b00'00'0000,
			N2 = 0b01'00'0000,
			N3 = 0b10'00'0000,
			N4 = 0b11'00'0000
		};

		/// @brief アセンブラ定義構造体
		struct ASM {
			union {
				NIMO	nimo_;
				uint8_t	code_;
			};
			constexpr ASM(NIMO nimo) : nimo_(nimo) { }
			constexpr ASM(ADRM adrm, CMP cmp) : code_(static_cast<uint8_t>(cmp) | static_cast<uint8_t>(adrm)) { }
			constexpr ASM(uint8_t code) : code_(code) { }

			/// @brief	アドレスモードの取得
			/// @return	アドレスモード
			inline auto get_adrm() const noexcept { return static_cast<ADRM>(code_ & 0b0011'1111); }

			/// @brief  比較長の取得
			/// @return 比較長
			inline auto get_cmpl() const noexcept { return (code_ >> 6) + 1; }
		};
	};


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief	RX DisAssembler クラス
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    class rxdisasm : public rxdis_t {
	public:
		typedef std::string STRING;

	private:
		STRING		out_;

		void list_nimo_(NIMO nimo, char szc = 0) noexcept {
			auto idx = static_cast<uint8_t>(nimo);
			out_ += nimo_tbl_[idx];
			if(szc != 0) {
				out_ += '.';
				out_ += szc;
			}
		}

		void list_space_() noexcept {
			out_ += '\t';
		}

		void list_reg_(uint32_t n, char sep = 0) noexcept {
			if(sep != 0) {
				out_ += sep;
			}
			out_ += 'R';
			char tmp[8];
			tmp[0] = 0;
			utils::sformat("%d", tmp, sizeof(tmp)) % n;
			out_ += tmp;
		}

		void list_ireg_(uint32_t n, char sep = 0) noexcept {
			if(sep != 0) {
				out_ += sep;
			}
			out_ += '[';
			list_reg_(n);
			out_ += ']';
		}

		void list_imm_(const uint8_t* src, uint8_t size) noexcept {
			out_ += "#0x";
			uint32_t val = 0;
			char tmp[12];
			if(size == 1) {
				val = src[0];
				utils::sformat("%02X", tmp, sizeof(tmp)) % val;
			} else if(size == 2) {
				val = src[0] | (src[1] << 8);
				utils::sformat("%04X", tmp, sizeof(tmp)) % val;
			} else if(size == 3) {
				val = src[0] | (src[1] << 8) | (src[2] << 16);
				utils::sformat("%06X", tmp, sizeof(tmp)) % val;
			} else if(size == 4) {
				val = src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24);
				utils::sformat("%06X", tmp, sizeof(tmp)) % val;
			}
			out_ += tmp;
		}

		void list_udec_(uint32_t num) noexcept {
			uint32_t val = 0;
			char tmp[12];
			utils::sformat("%u", tmp, sizeof(tmp)) % num;
			out_ += tmp;
		}

		void list_sdec_(int32_t num) noexcept {
			uint32_t val = 0;
			char tmp[12];
			utils::sformat("%d", tmp, sizeof(tmp)) % num;
			out_ += tmp;
		}

		void list_cr_(uint32_t cr) noexcept {
			switch(cr) {
			case 0b0000:
				out_ += "PSW";
				break;
			case 0b0010:
				out_ += "USP";
				break;
			case 0b0011:
				out_ += "FPSW";
				break;
			case 0b1000:
				out_ += "BPSW";
				break;
			case 0b1001:
				out_ += "BPC";
				break;
			case 0b1010:
				out_ += "ISP";
				break;
			case 0b1011:
				out_ += "FINTV";
				break;
			case 0b1100:
				out_ += "INTB";
				break;
			default:
				out_ += "-";
				break;
			}
		}


		static constexpr ASM asm_[] = {
			// 単独命令(1 byte)
			NIMO::NOP,		{ ADRM::ONLY,		CMP::N1 },	0b0000'0011,
			NIMO::BRK,		{ ADRM::ONLY,		CMP::N1 },	0b0000'0000,
			NIMO::RTS,		{ ADRM::ONLY,		CMP::N1 },	0b0000'0010,
			// 単独命令(2 bytes)
			NIMO::WAIT,		{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1001'0110,
			NIMO::RTE,		{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1001'0101,
			NIMO::RTFI,		{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1001'0100,
			NIMO::SATR,		{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1001'0011,
			NIMO::SCMPU,	{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1000'0011,
			NIMO::SMOVB,	{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1000'1011,
			NIMO::SMOVF,	{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1000'1111,
			NIMO::SMOVU,	{ ADRM::ONLY,		CMP::N2 },	0b0111'1111, 0b1000'0111,

			// Rd
			NIMO::ABS,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0010'0000,
			NIMO::BSR,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0101'0000,
			NIMO::BRA,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0100'0000,
			NIMO::JMP,		{ ADRM::Rd,			CMP::N2 },	0b0111'1111, 0b0000'0000,
			NIMO::JSR,		{ ADRM::Rd,			CMP::N2 },	0b0111'1111, 0b0001'0000,
			NIMO::NEG,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0001'0000,
			NIMO::NOT,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0000'0000,
			NIMO::POP,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b1011'0000,
			NIMO::ROLC,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0101'0000,
			NIMO::RORC,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0100'0000,
			NIMO::SAT,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0011'0000,

			// cr
			NIMO::POPC,		{ ADRM::cr,			CMP::N2 },	0b0111'1110, 0b1110'0000,
			NIMO::PUSHC,	{ ADRM::cr,			CMP::N2 },	0b0111'1110, 0b1100'0000,

			// Rs,Rd
			NIMO::ABS,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0000'1111,
			NIMO::ADC,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b1000'0011,
			NIMO::BCLR,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'0111,
			NIMO::BNOT,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'1111,
			NIMO::BSET,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'0011,
			NIMO::MACHI,	{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0000'0100,
			NIMO::MACLO,	{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0000'0101,
			NIMO::MVFC,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'1010,
			NIMO::NEG,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0000'0111,
			NIMO::NOT,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0011'1011,
			NIMO::ROTL,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0110,
			NIMO::ROTR,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0100,
			NIMO::SBB,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0000'0000,
			NIMO::SHAR,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0001,
			NIMO::SHLL,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0010,
			NIMO::SHLR,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0000,

			// Rs,Rs,Rd
			NIMO::ADD,		{ ADRM::RsRsRd,		CMP::N2 },	0b1111'1111, 0b0010'0000,
			NIMO::AND,		{ ADRM::RsRsRd,		CMP::N2 },	0b1111'1111, 0b0100'0000,
			NIMO::MUL,		{ ADRM::RsRsRd,		CMP::N2 },	0b1111'1111, 0b0011'0000,
			NIMO::OR,		{ ADRM::RsRsRd,		CMP::N2 },	0b1111'1111, 0b0101'0000,
			NIMO::SUB,		{ ADRM::RsRsRd,		CMP::N2 },	0b1111'1111, 0b0000'0000,

			// Address Offset (s8)
			NIMO::BRA,		{ ADRM::Ofs8,		CMP::N1 },	0b0010'1110,

			// Address Offset (s16)
			NIMO::BRA,		{ ADRM::Ofs16,		CMP::N1 },	0b0011'1000,
			NIMO::BSR,		{ ADRM::Ofs16,		CMP::N1 },	0b0011'1001,

			// Address Offset (s24)
			NIMO::BRA,		{ ADRM::Ofs24,		CMP::N1 },	0b0000'0100,
			NIMO::BSR,		{ ADRM::Ofs24,		CMP::N1 },	0b0000'0101,

			// #u8,Rd-Rd
			NIMO::RTSD,		{ ADRM::Im8Rd2Rd,	CMP::N1 },	0b0011'1111,

			// Rd-Rd
			NIMO::POPM,		{ ADRM::Rd2Rd,		CMP::N1 },	0b0110'1111,
			// Rs-Rs
			NIMO::PUSHM,	{ ADRM::Rs2Rs,		CMP::N1 },	0b0110'1110,

			// [Rs],Rd / u8[Rs],Rd / u16[Rs],Rd
			NIMO::ADC,		{ ADRM::DsRsRd_L,	CMP::N3 },	0b0000'0110, 0b1010'0000, 0b0000'0010,
			NIMO::SBB,		{ ADRM::DsRsRd_L,	CMP::N3 },	0b0000'0110, 0b1010'0000, 0b0000'0000,

			// Rs,Rd / [Rs].UB,Rd / u8[Rs].UB,Rd / u16[Rs].UB,Rd（短命令）
			NIMO::ADD,		{ ADRM::DsRsRd_f,	CMP::N1 },	0b0100'1000,
			NIMO::AND,		{ ADRM::DsRsRd_f,	CMP::N1 },	0b0101'0000,
			NIMO::CMP,		{ ADRM::DsRsRd_f,	CMP::N1 },	0b0100'0100,
			NIMO::MUL,		{ ADRM::DsRsRd_f,	CMP::N1 },	0b0100'1100,
			NIMO::OR,		{ ADRM::DsRsRd_f,	CMP::N1 },	0b0101'0100,
			NIMO::SUB,		{ ADRM::DsRsRd_f,	CMP::N1 },	0b0100'0000,
			// Rs.mi,Rd / [Rs].mi,Rd / u8[Rs].mi,Rd / u16[Rs].mi,Rd (mi: B, W, L, UW)（短命令）
			NIMO::ADD,		{ ADRM::DsRsRd_f,	CMP::N2 },	0b0000'0110, 0b0000'1000,
			NIMO::AND,		{ ADRM::DsRsRd_f,	CMP::N2 },	0b0000'0110, 0b0001'0000,
			NIMO::CMP,		{ ADRM::DsRsRd_f,	CMP::N2 },	0b0000'0110, 0b0000'0100,
			NIMO::MUL,		{ ADRM::DsRsRd_f,	CMP::N2 },	0b0000'0110, 0b0000'1100,
			NIMO::OR,		{ ADRM::DsRsRd_f,	CMP::N2 },	0b0000'0110, 0b0001'0100,
			NIMO::SUB,		{ ADRM::DsRsRd_f,	CMP::N2 },	0b0000'0110, 0b0000'0000,

			// Rs,Rd / [Rs].UB,Rd / u8[Rs].UB,Rd / u16[Rs].UB,Rd
			NIMO::DIV,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0010'0000,
			NIMO::DIVU,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0010'0100,
			NIMO::EMUL,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0001'1000,
			NIMO::EMULU,	{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0001'1100,
			NIMO::MAX,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0001'0000,
			NIMO::MIN,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0001'0100,
			NIMO::TST,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0011'0000,
			NIMO::XCHG,		{ ADRM::DsRsRd,		CMP::N2 },	0b1111'1100, 0b0100'0000,
			// Rs.mi,Rd / [Rs].mi,Rd / u8[Rs].mi,Rd / u16[Rs].mi,Rd (mi: B, W, L, UW)
			NIMO::DIV,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'1000,
			NIMO::DIVU,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'1001,
			NIMO::EMUL,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'0110,
			NIMO::EMULU,	{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'0111,
			NIMO::MAX,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'0100,
			NIMO::MIN,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'0101,
			NIMO::TST,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0000'1100,
			NIMO::XCHG,		{ ADRM::DsRsRd,		CMP::N3 },	0b0000'0110, 0b0010'0000, 0b0001'0000,

			// #(8,16,24,32),Rs,Rd / Rs=Rd: #(8,16,24,32),Rd
			NIMO::ADD,		{ ADRM::ImxRsRd,	CMP::N1 },	0b0111'0000,
			// #(8,16,24,32),Rd
			NIMO::ADC,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0010'0000,
			NIMO::STNZ,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b1111'0000,
			NIMO::STZ,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0010'0000,
			NIMO::TST,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b1100'0000,

			// #(u4),Rd
			NIMO::ADD,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0010,
			NIMO::CMP,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0001,
			NIMO::OR,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0101,
			NIMO::SUB,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0000,

			// #(u5),Rd
			NIMO::BCLR,		{ ADRM::Im5Rd,		CMP::N1 },	0b0111'1010,
			NIMO::BSET,		{ ADRM::Im5Rd,		CMP::N1 },	0b0111'1000,
			NIMO::SHAR,		{ ADRM::Im5Rd,		CMP::N1 },	0b0110'1010,
			NIMO::SHLL,		{ ADRM::Im5Rd,		CMP::N1 },	0b0110'1100,
			NIMO::SHLR,		{ ADRM::Im5Rd,		CMP::N1 },	0b0110'1000,
			NIMO::ROTL,		{ ADRM::Im5Rd,		CMP::N2 },	0b1111'1101, 0b0110'1110,
			NIMO::ROTR,		{ ADRM::Im5Rd,		CMP::N2 },	0b1111'1101, 0b0110'1100,
			NIMO::BNOT,		{ ADRM::Im5Rd,		CMP::N3 },	0b1111'1101, 0b1110'0000, 0b1111'0000,

			// #(u5),Rs,Rd
			NIMO::SHAR,		{ ADRM::Im5RsRd,	CMP::N2 },	0b1111'1101, 0b1010'0000,
			NIMO::SHLL,		{ ADRM::Im5RsRd,	CMP::N2 },	0b1111'1101, 0b1100'0000,
			NIMO::SHLR,		{ ADRM::Im5RsRd,	CMP::N2 },	0b1111'1101, 0b1000'0000,

			// #(8)
			NIMO::RTSD,		{ ADRM::Imm8,		CMP::N1 },	0b0110'0111,
			NIMO::INT,		{ ADRM::Imm8,		CMP::N2 },	0b0111'0101, 0b0110'0000,

			// nmo.[BWL]
			NIMO::SSTR,		{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'1000,
			NIMO::SUNTIL,	{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'0000,
			NIMO::SWHILE,	{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'0100,

			NIMO::BCnd,		{ ADRM::bc3,		CMP::N1 },	0b0001'0000,
		};

		uint32_t DspRsRd1_(const uint8_t* bin, uint8_t ofs = 0) noexcept
		{
			auto ld = bin[ofs] & 0b0000'0011;
			uint32_t al = 0;
			auto rs = (bin[1 + ofs] & 0b1111'0000) >> 4;
			if(ld == 0b11) {  // Rs
				list_reg_(rs);
			} else if(ld == 0b00) {  // [Rs].UB
				list_ireg_(rs);
				out_ += ".UB";
			} else if(ld == 0b01) {  // dsp:8
				list_udec_(bin[2 + ofs]);
				list_ireg_(rs);
				out_ += ".UB";
				al = 1;
			} else {  // dsp:16
				list_udec_(bin[2 + ofs] | (bin[3 + ofs] << 8));
				list_ireg_(rs);
				out_ += ".UB";
				al = 2;
			}
			list_reg_(bin[1 + ofs] & 0b0000'1111, ',');
			return al;
		}

		static constexpr const char* size_str_[] = { ".B", ".W", ".L", ".UW" };

		uint32_t DspRsRd2_(const uint8_t* bin, uint8_t ofs = 0) noexcept
		{
			auto mi = (bin[1] & 0b1100'0000) >> 6;
			auto ld = bin[1] & 0b0000'0011;
			auto rs = (bin[2 + ofs] & 0b1111'0000) >> 4;
			uint32_t al = 0;
			static constexpr const uint8_t multi[] = { 1, 2, 4, 2 };
			switch(ld) {
			case 0b11:
				list_reg_(rs);
				break;
			case 0b00:
				list_ireg_(rs);
				break;
			case 0b01:	// dsp:8
				list_udec_(bin[3 + ofs] * multi[mi]);
				list_ireg_(rs);
				al = 1;
				break;
			case 0b10:  // dsp:16
				list_udec_((bin[3 + ofs] | (bin[4 + ofs] << 8)) * multi[mi]);
				list_ireg_(rs);
				al = 2;
				break;
			}
			out_ += size_str_[mi];
			list_reg_(bin[2 + ofs] & 0b0000'1111, ',');
			return al;
		}

		uint32_t rxv1_(const uint8_t* bin) noexcept
		{
			uint32_t l = 0;
			while(l < sizeof(asm_)) {
				auto nimo = asm_[l].nimo_;
				++l;
				auto adrm = asm_[l].get_adrm();
				auto cmpl = asm_[l].get_cmpl();
				++l;
				switch(adrm) {
				case ADRM::ONLY:
					if(cmpl == 1) {
						if(bin[0] == asm_[l].code_) {
							list_nimo_(nimo);
							return 1;
						}
					} else if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_) {
							list_nimo_(nimo);
							return 2;
						}
					}
					break;
				case ADRM::Rd:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_space_();
						list_reg_(bin[1] & 0x0F);
						return 2;
					}
					break;
				case ADRM::RsRd:
					if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_space_();
						list_reg_((bin[2] & 0b1111'0000) >> 4);
						list_reg_(bin[2] & 0b0000'1111, ',');
						return 3;
					}
					break;
				case ADRM::RsRsRd:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_space_();
						list_reg_((bin[2] & 0b1111'0000) >> 4);
						list_reg_(bin[2] & 0b0000'1111, ',');
						list_reg_(bin[1] & 0b0000'1111, ',');
						return 3;
					}
					break;
				case ADRM::Ofs8:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo, 'B');
						list_space_();
						int32_t ofs = static_cast<int8_t>(bin[1]);
						list_sdec_(ofs);
						return 3;
					}
					break;
				case ADRM::Ofs16:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo, 'W');
						list_space_();
						int32_t ofs = static_cast<int8_t>(bin[2]);
						ofs <<= 8;
						ofs |= bin[1];
						list_sdec_(ofs);
						return 3;
					}
					break;
				case ADRM::Ofs24:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo, 'A');
						list_space_();
						int32_t ofs = static_cast<int8_t>(bin[3]);
						ofs <<= 16;
						ofs |= (static_cast<uint32_t>(bin[2]) << 8) | bin[1];
						list_sdec_(ofs);
						return 4;
					}
					break;
				case ADRM::ImxRd:
					if(bin[0] == asm_[l].code_ &&
					  (bin[1] & 0b1111'0011) == asm_[l + 1].code_ &&
					  (bin[2] & 0b1111'0000) == asm_[l + 2].code_) {
						list_nimo_(nimo);
						list_space_();
						auto il = (bin[1] & 0b00001100) >> 2;
						if(il == 0) il = 4;
						list_imm_(bin + 3, il);
						list_reg_(bin[2] & 0b0000'1111, ',');
						return 3 + il;
					}
					break;
				case ADRM::Im4Rd:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo);
						list_space_();
						auto num = (bin[1] & 0b1111'0000) >> 4;
						out_ += '#';
						list_udec_(num);
						list_reg_(bin[1] & 0b0000'1111, ',');
						return 2;
					}
					break;

				case ADRM::Im5Rd:
					if(cmpl == 1) {
						if((bin[0] & 0b1111'1110) == asm_[l].code_) {
							list_nimo_(nimo);
							list_space_();
							out_ += '#';
							auto num = ((bin[0] & 0b0000'0001) << 4) | ((bin[1] & 0b1111'0000) >> 4);
							list_udec_(num);
							list_reg_(bin[1] & 0b0000'1111, ',');
							return 2;
						}
					} else if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1110) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							list_space_();
							out_ += '#';
							auto num = ((bin[1] & 0b0000'0001) << 4) | ((bin[2] & 0b1111'0000) >> 4);
							list_udec_(num);
							list_reg_(bin[2] & 0b0000'1111, ',');
							return 3;
						}
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1110'0000) == asm_[l + 1].code_
						  && (bin[2] & 0b1111'0000) == asm_[l + 2].code_) {
							list_nimo_(nimo);
							list_space_();
							out_ += '#';
							auto num = bin[1] & 0b0001'1111;
							list_udec_(num);
							list_reg_(bin[2] & 0b0000'1111, ',');
							return 3;
						}
					}
					break;
				case ADRM::DsRsRd_f:
					if(cmpl == 1) {  // memex: UB or src==Rs(L)
						if((bin[0] & 0b1111'1100) == asm_[l].code_) {
							list_nimo_(nimo);
							list_space_();
							auto al = DspRsRd1_(bin);
							return 2 + al;
						}
					} else if(cmpl == 2) {  // memex: B, W, L, UW
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b0011'1100) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							list_space_();
							auto al = DspRsRd2_(bin);
							return 3 + al;
						}
					}
					break;
				case ADRM::DsRsRd:
					if(cmpl == 2) {  // memex == UB or src==Rs(L)
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1100) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							list_space_();
							auto al = DspRsRd1_(bin, 1);
							return 3 + al;
						}
					} else if(cmpl == 3) {  // memex: B, W, L, UW
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b0011'1100) == asm_[l + 1].code_ && bin[2] == asm_[l + 2].code_) {
							list_nimo_(nimo);
							list_space_();
							auto al = DspRsRd2_(bin, 1);
							return 4 + al;
						}
					}
					break;
				case ADRM::DsRsRd_L:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1100) == asm_[l + 1].code_ && bin[2] == asm_[l + 2].code_) {
						auto id = bin[1] & 0b0000'0011;
						if(id == 3) {
							break;
						}
						list_nimo_(nimo);
						list_space_();
						if(id == 1) {
							uint32_t num = bin[4];
							list_udec_(num * 4);
						} else if(id == 2) {
							uint32_t num = bin[4] | (bin[5] << 8);
							list_udec_(num * 4);
						}
						list_ireg_((bin[3] & 0b1111'0000) >> 4);
						list_reg_(bin[3] & 0b0000'1111, ',');
						return 4 + id;
					}
					break;
				case ADRM::Im5RsRd:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1110'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_space_();
						auto imm = bin[1] & 0b0001'1111;
						out_ += '#';
						list_udec_(imm);
						auto rs = (bin[2] & 0b1111'0000) >> 4;
						auto rd = bin[2] & 0b0000'1111;
						list_reg_(rs, ',');
						list_reg_(rd, ',');
						return 3;
					}
					break;
				case ADRM::Imm8:
					if(cmpl == 1) {
						if(bin[0] == asm_[l].code_) {
							list_nimo_(nimo);
							list_space_();
							auto imm = bin[1];
							out_ += '#';
							list_udec_(imm);
							return 2;
						}
					} else if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_) {
							list_nimo_(nimo);
							list_space_();
							auto imm = bin[2];
							out_ += '#';
							list_udec_(imm);
							return 3;
						}
					}
					break;
				case ADRM::ImxRsRd:
					if((bin[0] & 0b1111'1100) == asm_[l].code_) {
						auto il = bin[0] & 0b0000'0011;
						if(il == 0) il = 4;
						list_nimo_(nimo);
						list_space_();
						list_imm_(bin + 2, il);
						auto rs = (bin[1] & 0b1111'0000) >> 4;
						auto rd = bin[1] & 0b0000'1111;
						if(rs != rd) {
							list_reg_(rs, ',');
						} 
						list_reg_(rd, ',');
						return 2 + il;
					}
					break;
				case ADRM::cr:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_space_();
						list_cr_(bin[1] & 0b0000'1111);
						return 2;
					}
					break;
				case ADRM::Im8Rd2Rd:
					if(bin[0] == asm_[l].code_) {
						auto rd1 = bin[1] >> 4;
						auto rd2 = bin[1] & 0b0000'1111;
						if(rd1 == 0 || rd2 == 0) {
							break;
						}
						list_nimo_(nimo);
						list_space_();
						out_ += '#';
						list_udec_(bin[2]);
						list_reg_(rd1);
						list_reg_(rd2, '-');
						return 3;
					}
					break;
				case ADRM::Rd2Rd:
				case ADRM::Rs2Rs:
					if(bin[0] == asm_[l].code_) {
						auto rd1 = bin[1] >> 4;
						auto rd2 = bin[1] & 0b0000'1111;
						if(rd1 < 1 || rd1 > 14 || rd2 < 2) {
							break;
						}
						list_nimo_(nimo);
						list_space_();
						list_reg_(rd1);
						list_reg_(rd2, '-');
						return 2;
					}
					break;
				case ADRM::Ns:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1100) == asm_[l + 1].code_) {
						auto sz = bin[1] & 0b11;
						if(sz == 0b11) {
							break;
						}
						char ch = 0;
						if(sz == 0b00) ch = 'B';
						else if(sz == 0b01) ch = 'W';
						else if(sz == 0b10) ch = 'L';
						list_nimo_(nimo, ch);
					}
					break;
				case ADRM::bc3:
					if((bin[0] & 0b1111'0000) == asm_[l].code_) {
						if(bin[0] & 0b0000'1000) {  // BNE, BNZ
							out_ += "BNE";
						} else {  // BEQ, BZ
							out_ += "BEQ";
						}
						list_space_();
						static constexpr int8_t tbl[] = { 8, 9, 10, 3, 4, 5, 6, 7 };
						auto dsp = tbl[bin[0] & 0b111];
						list_sdec_(dsp);
					}
					break;
				default:
					return 0;
				}
				l += cmpl;
			}
			return 0;
		}


		void list_fp_(const uint8_t* bin) noexcept
		{
			struct pad {
				union {
					uint32_t	u32;
					float		fp;
				};
			};
			pad p;
			p.u32 = bin[0] | (bin[1] << 8) | (bin[2] << 16) | (bin[3] << 24);
			char tmp[16];
			utils::sformat("0x%08X", tmp, sizeof(tmp)) % p.u32;
			out_ += tmp;
		}


		static constexpr ASM fpu_[] = {
			// #fp32,Rd
			NIMO::FADD,   { ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0010'0000,
			NIMO::FCMP,   { ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0001'0000,
			NIMO::FDIV,   { ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0100'0000,
			NIMO::FMUL,   { ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0011'0000,
			NIMO::FSUB,   { ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0000'0000,
			// Rs,Rd / [Rs],Rd / dsp8[Rs],Rd / dsp16[Rs],Rd
			NIMO::FADD,   { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'1000,
			NIMO::FCMP,   { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'0100,
			NIMO::FDIV,   { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1001'0000,
			NIMO::FMUL,   { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'1100,
			NIMO::FSUB,   { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'0000,
			NIMO::FTOI,   { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1001'0100,
			NIMO::ROUND,  { ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1001'1000,
			// Rs.UB,Rd / [Rs].UB,Rd / dsp8[Rs].UB,Rd / dsp16[Rs].UB,Rd
			NIMO::ITOF,   { ADRM::DsRsRd,	CMP::N2 },	0b1111'1100, 0b0100'0100,
			// Rs.mi,Rd / [Rs].mi,Rd / dsp8[Rs].mi,Rd / dsp16[Rs].mi,Rd (mi: B, W, L, UW)
			NIMO::ITOF,   { ADRM::DsRsRd,	CMP::N3 },	0b0000'0110, 0b0000'0001,
		};

		uint32_t rx_fpu_(const uint8_t* bin)
		{
			uint32_t l = 0;
			while(l < sizeof(fpu_)) {
				auto nimo = fpu_[l].nimo_;
				++l;
				auto adrm = fpu_[l].get_adrm();
				auto cmpl = fpu_[l].get_cmpl();
				++l;
				switch(adrm) {
				case ADRM::ImfpRd:
					if(bin[0] == fpu_[l].code_ && bin[1] == fpu_[l + 1].code_ && (bin[2] & 0b1111'0000) == fpu_[l + 2].code_) {
						list_nimo_(nimo);
						list_space_();
						out_ += '#';
						list_fp_(bin + 3);
						list_reg_(bin[2] & 0b0000'1111, ',');
						return 7;
					}
					break;
				case ADRM::FpRsRd:
					if(bin[0] == fpu_[l].code_ && (bin[1] & 0b1111'1100) == fpu_[l + 1].code_) {
						list_nimo_(nimo);
						list_space_();
						auto ld = bin[1] & 0b0000'0011;
						auto rs = bin[2] >> 4;
						auto rd = bin[2] & 0b0000'1111;
						uint32_t dsp = 0;
						switch(ld) {
						case 0b11:
							list_reg_(rs);
							break;
						case 0b00:
							list_ireg_(rs);
							break;
						case 0b01:
							dsp = 1;
							list_udec_(bin[3] * 4);
							list_ireg_(rs);
							break;
						case 0b10:
							dsp = 2;
							list_udec_((bin[3] | (bin[4] << 8)) * 4);
							list_ireg_(rs);
							break;
						default:
							break;
						}
						list_reg_(rd, ',');
						return 3 + dsp;
					}
					break;
				case ADRM::DsRsRd:
					if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1100) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							list_space_();
							auto al = DspRsRd1_(bin, 1);
							return 3 + al;
						}
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b0011'1100) == asm_[l + 1].code_ && bin[2] == asm_[l + 2].code_) {
							list_nimo_(nimo);
							list_space_();
							auto al = DspRsRd2_(bin, 1);
							return 4 + al;
						}
					}
				default:
					return 0;
				}
				l += cmpl;
			}
			return 0;
		}

    public:
        //-------------------------------------------------------------//
        /*!
            @brief  コンストラクター
        */
        //-------------------------------------------------------------//
        rxdisasm() noexcept { }


        //-------------------------------------------------------------//
        /*!
            @brief  逆アセンブラ
			@param[in]	bin		バイナリーソース
			@param[out]	out		アセンブラソース出力
        */
        //-------------------------------------------------------------//
		uint32_t disasm(const uint8_t* bin, STRING& out) noexcept
		{
			out_ = "";
			auto n = rxv1_(bin);
			if(n == 0) {
				n = rx_fpu_(bin);
			}
			out = out_;
			return n;
		}

    };
}
