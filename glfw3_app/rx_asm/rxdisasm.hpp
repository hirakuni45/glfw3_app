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
			ADD,		// 4/4
			AND,		// 4/4
			BCLR,		// 4/4
			BCnd,
			BMCnd,
			BNOT,		// 4/4
			BRA,		// 5/5
			BRK,		// 1/1
			BSET,		// 4/4
			BSR,		// 3/3
			BTST,		// 4/4
			CLRPSW,		// 1/1
			CMP,		// 4/4
			DIV,		// 2/2
			DIVU,		// 2/2
			EMACA,		//     RXv2
			EMSBA,		//     RXv2
			EMUL,		// 2/2
			EMULA,		//     RXv2
			EMULU,		// 2/2
			FADD,		// 2/2
			FCMP,		// 2/2
			FDIV,		// 2/2
			FMUL,		// 2/2
			FSQRT,		// 1/1 RXv2
			FSUB,		// 2/2
			FTOI,		// 1/1
			FTOU,		// 1/1 RXv2
			INT,		// 1/1
			ITOF,		// 1/1
			JMP,		// 1/1
			JSR,		// 1/1
			MACHI,		// 1/1
			MACLO,		// 1/1
			MACLH,		//     RXv2
			MAX,		// 2/2
			MIN,		// 2/2
			MOV,		// 13/15 (1), (2), (3), (4), (5), (6), (7),   (9), (10), (11), (12),   (14), (15)
			MOVCO,		//     RXv2
			MOVLI,		//     RXv2
			MOVU,
			MUL,		// 4/4
			MULHI,		// 1/1
			MULLO,		// 1/1
			MVFACHI,	// 1/1
			MVFACMI,	// 1/1
			MVFC,		// 1/1
			MVTACHI,	// 1/1
			MVTACLO,	// 1/1
			MVTC,		// 2/2
			MVTIPL,		// 1/1
			NEG,		// 2/2
			NOP,		// 1/1
			NOT,		// 2/2
			OR,			// 4/4
			POP,		// 1/1
			POPC,		// 1/1
			POPM,		// 1/1
			PUSH,		// 2/2
			PUSHC,		// 1/1
			PUSHM,		// 1/1
			RACW,		// 1/1
			REVL,		// 1/1
			REVW,		// 1/1
			RMPA,		// 1/1
			ROLC,		// 1/1
			RORC,		// 1/1
			ROTL,		// 2/2
			ROTR,		// 2/2
			ROUND,		// 1/1
			RTE,		// 1/1
			RTFI,		// 1/1
			RTS,		// 1/1
			RTSD,		// 1/1
			SAT,		// 1/1
			SATR,		// 1/1
			SBB,		// 2/2
			SCCnd,
			SCMPU,		// 1/1
			SETPSW,		// 1/1
			SHAR,		// 3/3
			SHLL,		// 3/3
			SHLR,		// 3/3
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
			UTOF,		// 1/1 RXv2
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
			"EMACA",
			"EMSBA",
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
			"FTOU",
			"INT",
			"ITOF",
			"JMP",
			"JSR",
			"MACHI",
			"MACLO",
			"MACLH",
			"MAX",
			"MIN",
			"MOV",
			"MOVCO",
			"MOVLI",
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
			"UTOF",
			"WAIT",
			"XCHG",
			"XOR"
		};

		/// @brief アドレッシングモード（最大６４種）
		enum class ADRM : uint8_t {
			ONLY,		///< 単独命令
			Rd,			///< ディストネーションレジスタ
			RsRd,		///< ソースレジスタ、ディストネーションレジスタ
			RsCR,		///< ソースレジスタ、ディストネーションコンディションレジスタ
			RsRsRd,		///< ソースレジスタ、ソースレジスタ、ディストネーションレジスタ
			Ofs8,		///< オフセットアドレス（s8）
			Ofs16,		///< オフセットアドレス（s16）
			Ofs24,		///< オフセットアドレス（s24）

			Imm1,		///< ソースイミディエイト（u1）1 or 2
			Imm4,		///< ソースイミディエイト（u4）
			Imm8,		///< ソースイミディエイト（u8）
			ImxRd,		///< ソースイミディエイト（s8,s16,s24,u32）、ディストネーションレジスタ
			ImxCR,		///< ソースイミディエイト（s8,s16,s24,u32）、ディストネーションコンディションレジスタ
			Im4Rd,		///< ソースイミディエイト（u4）、ディストネーションレジスタ
			Im5Rd,		///< ソースイミディエイト（u5）、ディストネーションレジスタ
			Im8Rd,		///< ソースイミディエイト（u8）、ディストネーションレジスタ

			Im3DsRd,	///< ソースイミディエイト（u3）、ディスプレースメント付きディストネーションレジスタ
			RsDsRd,		///< ソースレジスタ、ディスプレースメント付きディストネーションレジスタ
			DsRsRd_f,	///< ソースディスプレースメント付きレジスタ（Rs, [Rs], dsp:8[Rs], dsp:16[Rs]）、ディストネーションレジスタ、短命令
			DsRsRd,		///< ソースディスプレースメント付きレジスタ（Rs, [Rs], dsp:8[Rs], dsp:16[Rs]）、ディストネーションレジスタ
			DsRsRd_L,	///< ソースディスプレースメント付きレジスタ（[Rs].L, dsp:8[Rs].L, dsp:16[Rs].L）、ディストネーションレジスタ、
			Im5RsRd,	///< ソースイミディエイト（u5）、ソースレジスタ、ディストネーションレジスタ
			ImxRsRd,	///< ソースイミディエイト（s8,s16,s24,u32）、ソースレジスタ、ディストネーションレジスタ
			cr,			///< コンディションレジスタ

			cb,			///< コンディションフラグ
			Im8Rd2Rd,	///< ソースイミディエイト（u8）、ディストネーションレジスタ to ディストネーションレジスタ
			Rd2Rd,		///< ディストネーションレジスタ to ディストネーションレジスタ
			Rs2Rs,		///< ソースレジスタ to ソースレジスタ
			ImfpRd,		///< ソースイミディエイト（float）、ディストネーションレジスタ
			FpRsRd,		///< FPU ソースレジスタ、ディストネーションレジスタ
			Ns,			///< ニーモニックサイズオペレーション
			NsRs,		///< ニーモニックサイズオペレーション　ソースレジスタ

			NsDsRs,		///< ニーモニックサイズオペレーション　ディスプレースメントソースレジスタ
			bc3,		///< ブランチコンディション（８種類の分岐距離）
			bc8,		///< ブランチコンディション（-128/+127）
			bc16,		///< ブランチコンディション（-32768/+32767）
			RsDs5Rd,	///< MOV( 1) R0-7,dsp:5[R0-7]（MOV 専用）
			Ds5RsRd,	///< MOV( 2) dsp:5[R0-7],R0-7（MOV 専用）
			Im8Ds5Rd,	///< MOV (4) #u8,dsp:5[R0-R7]（MOV 専用）
			ImxRd_,		///< MOV (6) ソースイミディエイト（s8,s16,s24,u32）、ディストネーションレジスタ（MOV 専用）
			NsRsRd,		///< MOV (7) ニーモニックサイズ付き、ソースレジスタ、ディストネーションレジスタ（MOV 専用）


			NsDsRsRd,	///< MOV ( 9) MOV.sz [Rs],Rd / dsp:8[Rs],Rd / dsp:16[Rs],Rd
			NsRiRbRd,	///< MOV (10) MOV.sz [RiRb],Rd]
			NsRsDsRd,	///< MOV (11) MOV.sz Rs,[Rd] / Rs,dsp:8[Rd] / dsp:16[Rs],Rd
			NsRsRiRb,	///< MOV (12) MOV.sz Rs,[Ri,Rb]

			RsPidRd,	///< MOV (14) Rs,[Rd+] [-Rd]
			PidRsRd,	///< MOV (15) [Rs+] [-Rs],Rd
		};

		/// @brief オペコード比較長（最大４種）
		enum class CMP : uint8_t {
			N1 = 0b00'00'0000,	///< 1 byte
			N2 = 0b01'00'0000,	///< 2 bytes
			N3 = 0b10'00'0000,	///< 3 bytes
			N4 = 0b11'00'0000	///< 4 bytes
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


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief	RX DisAssembler クラス
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    class rxdisasm : public rxdis_t {
	public:
		typedef std::string STRING;

		//=================================================================//
		/*
			@brief	RX タイプ型
		*/
		//=================================================================//
		enum class RX_TYPE : uint8_t {
			RXv1,		///< RXv1
			RXv2,		///< RXv2
			RXv3,		///< RXv3
			RXv3_DFPU,	///< RXv3 + DFPU
		};

	private:
		STRING		nimo_;
		STRING		adrm_;
		float		fpval_;

		RX_TYPE		rx_type_;

		static constexpr const char* size_str_[] = { ".B", ".W", ".L", ".UW" };

		void list_nimo_(NIMO nimo, char szc = 0) noexcept {
			auto idx = static_cast<uint8_t>(nimo);
			nimo_ += nimo_tbl_[idx];
			if(szc != 0) {
				nimo_ += '.';
				nimo_ += szc;
			}
		}

		void list_nimo_size_(NIMO nimo, uint8_t sz) noexcept {
			list_nimo_(nimo);
			nimo_ += size_str_[sz];
		}

		void list_reg_(uint32_t n, char sep = 0) noexcept {
			if(sep != 0) {
				adrm_ += sep;
			}
			adrm_ += 'R';
			char tmp[8];
			tmp[0] = 0;
			utils::sformat("%d", tmp, sizeof(tmp)) % n;
			adrm_ += tmp;
		}

		void list_ireg_(uint32_t n, char sep = 0) noexcept {
			if(sep != 0) {
				adrm_ += sep;
			}
			adrm_ += '[';
			list_reg_(n);
			adrm_ += ']';
		}

		void list_ireg2_(uint32_t ri, uint32_t rb, char sep = 0) noexcept {
			if(sep != 0) {
				adrm_ += sep;
			}
			adrm_ += '[';
			list_reg_(ri);
			adrm_ += ',';
			list_reg_(rb);
			adrm_ += ']';
		}


		void list_preg_(uint32_t n, uint8_t pid, char sep = 0) noexcept {
			if(sep != 0) {
				adrm_ += sep;
			}
			adrm_ += '[';
			if((pid & 1) != 0) {
				adrm_ += '-';
			}
			list_reg_(n);
			if((pid & 1) == 0) {
				adrm_ += '+';
			}
			adrm_ += ']';
		}

		void list_imm_(const uint8_t* src, uint8_t size) noexcept {
			uint32_t val = 0;
			char tmp[16];
			if(size == 1) {
				val = src[0];
				utils::sformat("#%d", tmp, sizeof(tmp)) % static_cast<int32_t>(val);
			} else if(size == 2) {
				val = src[0] | (src[1] << 8);
				utils::sformat("#0x%04X", tmp, sizeof(tmp)) % val;
			} else if(size == 3) {
				val = src[0] | (src[1] << 8) | (src[2] << 16);
				utils::sformat("#0x%06X", tmp, sizeof(tmp)) % val;
			} else if(size == 4) {
				val = src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24);
				utils::sformat("#0x%08X", tmp, sizeof(tmp)) % val;
			}
			adrm_ += tmp;
		}

		void list_udec_(uint32_t num, char ch = 0) noexcept {
			if(ch != 0) {
				adrm_ += ch;
			}
			char tmp[12];
			utils::sformat("%u", tmp, sizeof(tmp)) % num;
			adrm_ += tmp;
		}

		void list_sdec_(int32_t num, char ch = 0) noexcept {
			if(ch != 0) {
				adrm_ += ch;
			}
			char tmp[12];
			utils::sformat("%d", tmp, sizeof(tmp)) % num;
			adrm_ += tmp;
		}

		void list_cr_(uint8_t cr, char sep = 0) noexcept {
			if(sep != 0) {
				adrm_ += sep;
			}
			switch(cr) {
			case 0b0000: adrm_ += "PSW";   break;
			case 0b0010: adrm_ += "USP";   break;
			case 0b0011: adrm_ += "FPSW";  break;
			case 0b1000: adrm_ += "BPSW";  break;
			case 0b1001: adrm_ += "BPC";   break;
			case 0b1010: adrm_ += "ISP";   break;
			case 0b1011: adrm_ += "FINTV"; break;
			case 0b1100: adrm_ += "INTB";  break;
			default:     adrm_ += "-";     break;
			}
		}

		void list_cb_(uint8_t cb) noexcept {
			switch(cb) {
			case 0b0000: adrm_ += 'C'; break;
			case 0b0001: adrm_ += 'Z'; break;
			case 0b0010: adrm_ += 'S'; break;
			case 0b0011: adrm_ += 'O'; break;
			case 0b1000: adrm_ += 'I'; break;
			case 0b1001: adrm_ += 'U'; break;
			default:     adrm_ += "-"; break;
			}
		}

		uint32_t list_dsp_reg_(const uint8_t* bin, uint8_t ld, uint8_t mi, uint8_t rx, char sep = 0) noexcept {
			if(sep != 0) {
				adrm_ += sep;
			}
			uint32_t al = 0;
			static constexpr const uint8_t multi[] = { 1, 2, 4, 2 };  // B, W, L, UW
			switch(ld) {
			case 0b11:  // Rx
				list_reg_(rx);
				break;
			case 0b00:  // [Rx]
				list_ireg_(rx);
				break;
			case 0b01:	// dsp:8[Rx]
				list_udec_(bin[0] * multi[mi]);
				list_ireg_(rx);
				al = 1;
				break;
			case 0b10:  // dsp:16[Rx]
				list_udec_((bin[0] | (bin[1] << 8)) * multi[mi]);
				list_ireg_(rx);
				al = 2;
				break;
			}
			return al;
		}

		//----------------------------------------------//

		static constexpr ASM asm_[] = {
			// MOV 専用
			NIMO::MOV,		{ ADRM::RsDs5Rd,	CMP::N1 },	0b1000'0000,  // MOV (1)
			NIMO::MOV,		{ ADRM::Ds5RsRd,	CMP::N1 },	0b1000'1000,  // MOV (2)
			NIMO::MOV,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0110,  // MOV (3)
			NIMO::MOV,		{ ADRM::Im8Ds5Rd,	CMP::N1 },	0b0011'1100,  // MOV (4)
			NIMO::MOV,		{ ADRM::ImxRd_,		CMP::N2 },	0b1111'1011, 0b0000'0010,  // MOV (6)
			NIMO::MOV,		{ ADRM::NsRsRd,		CMP::N1 },	0b1100'1111,  // MOV (7)

			NIMO::MOV,		{ ADRM::NsDsRsRd,	CMP::N1 },	0b1100'1100,  // MOV (9)
			NIMO::MOV,		{ ADRM::NsRiRbRd,	CMP::N2 },	0b1111'1110, 0b0100'0000,  // MOV (10)
			NIMO::MOV,		{ ADRM::NsRsDsRd,	CMP::N1 },	0b1100'0011,  // MOV (11)

			NIMO::MOV,		{ ADRM::NsRsRiRb,	CMP::N2 },	0b1111'1110, 0b0000'0000,  // MOV (12)


			NIMO::MOV,		{ ADRM::RsPidRd,	CMP::N2	},	0b1111'1101, 0b0010'0000,  // MOV (14)
			NIMO::MOV,		{ ADRM::PidRsRd,	CMP::N2	},	0b1111'1101, 0b0010'1000,  // MOV (15)

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
			NIMO::MVFACHI,	{ ADRM::Rd,			CMP::N3 },	0b1111'1101, 0b0001'1111, 0b0000'0000,
			NIMO::MVFACMI,	{ ADRM::Rd,			CMP::N3 },	0b1111'1101, 0b0001'1111, 0b0010'0000,
			NIMO::MVTACHI,	{ ADRM::Rd,			CMP::N3 },	0b1111'1101, 0b0001'0111, 0b0000'0000,
			NIMO::MVTACLO,	{ ADRM::Rd,			CMP::N3 },	0b1111'1101, 0b0001'0111, 0b0001'0000,
			NIMO::NEG,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0001'0000,
			NIMO::NOT,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0000'0000,
			NIMO::POP,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b1011'0000,
			NIMO::ROLC,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0101'0000,
			NIMO::RORC,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0100'0000,
			NIMO::SAT,		{ ADRM::Rd,			CMP::N2 },	0b0111'1110, 0b0011'0000,

			// cb
			NIMO::CLRPSW,	{ ADRM::cb,			CMP::N2 },	0b0111'1110, 0b1011'0000,
			NIMO::SETPSW,	{ ADRM::cb,			CMP::N2 },	0b0111'1110, 0b1010'0000,

			// cr
			NIMO::POPC,		{ ADRM::cr,			CMP::N2 },	0b0111'1110, 0b1110'0000,
			NIMO::PUSHC,	{ ADRM::cr,			CMP::N2 },	0b0111'1110, 0b1100'0000,

			// Rs,Rd
			NIMO::ABS,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0000'1111,
			NIMO::ADC,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b1000'0011,
			NIMO::BCLR,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'0111,
			NIMO::BNOT,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'1111,
			NIMO::BSET,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'0011,
			NIMO::BTST,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0110'1011,
			NIMO::MACHI,	{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0000'0100,
			NIMO::MACLO,	{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0000'0101,
			NIMO::MULHI,	{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0000'0000,
			NIMO::MULLO,	{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0000'0001,
			NIMO::MVFC,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'1010,
			NIMO::NEG,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0000'0111,
			NIMO::NOT,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1100, 0b0011'1011,
			NIMO::REVL,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0111,
			NIMO::REVW,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0111,
			NIMO::ROTL,		{ ADRM::RsRd,		CMP::N2 },	0b1111'1101, 0b0110'0101,
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
			NIMO::DIV,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b1000'0000,
			NIMO::DIVU,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b1001'0000,
			NIMO::EMUL,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0110'0000,
			NIMO::EMULU,	{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0111'0000,
			NIMO::MAX,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0100'0000,
			NIMO::MIN,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0101'0000,
			NIMO::STNZ,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b1111'0000,
			NIMO::STZ,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b0010'0000,
			NIMO::TST,		{ ADRM::ImxRd,		CMP::N3 },	0b1111'1101, 0b0111'0000, 0b1100'0000,
			NIMO::CMP,		{ ADRM::ImxRd,		CMP::N2 },	0b0111'0100, 0b0111'0000,
			NIMO::OR,		{ ADRM::ImxRd,		CMP::N2 },	0b0111'0100, 0b0011'0000,
			NIMO::MUL,		{ ADRM::ImxRd,		CMP::N2 },	0b0111'0100, 0b0001'0000,

			// #(8,16,24,32),cr
			NIMO::MVTC,		{ ADRM::ImxCR,		CMP::N3 },	0b1111'1101, 0b0111'0011, 0b0000'0000,

			// #(u4),Rd
			NIMO::ADD,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0010,
			NIMO::AND,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0100,
			NIMO::CMP,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0001,
			NIMO::MOV,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0110,
			NIMO::MUL,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0011,
			NIMO::OR,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0101,
			NIMO::SUB,		{ ADRM::Im4Rd,		CMP::N1 },	0b0110'0000,

			// #(u5),Rd
			NIMO::BCLR,		{ ADRM::Im5Rd,		CMP::N1 },	0b0111'1010,
			NIMO::BSET,		{ ADRM::Im5Rd,		CMP::N1 },	0b0111'1000,
			NIMO::BTST,		{ ADRM::Im5Rd,		CMP::N1 },	0b0111'1100,
			NIMO::SHAR,		{ ADRM::Im5Rd,		CMP::N1 },	0b0110'1010,
			NIMO::SHLL,		{ ADRM::Im5Rd,		CMP::N1 },	0b0110'1100,
			NIMO::SHLR,		{ ADRM::Im5Rd,		CMP::N1 },	0b0110'1000,
			NIMO::ROTL,		{ ADRM::Im5Rd,		CMP::N2 },	0b1111'1101, 0b0110'1110,
			NIMO::ROTR,		{ ADRM::Im5Rd,		CMP::N2 },	0b1111'1101, 0b0110'1100,
			NIMO::BNOT,		{ ADRM::Im5Rd,		CMP::N3 },	0b1111'1101, 0b1110'0000, 0b1111'0000,

			// #(u8),Rd
			NIMO::CMP,		{ ADRM::Im8Rd,		CMP::N2 },	0b1111'0101, 0b0101'0000,
			NIMO::MOV,		{ ADRM::Im8Rd,		CMP::N2 },	0b0111'0101, 0b0101'0000,  // MOV (5)

			// #(u5),Rs,Rd
			NIMO::SHAR,		{ ADRM::Im5RsRd,	CMP::N2 },	0b1111'1101, 0b1010'0000,
			NIMO::SHLL,		{ ADRM::Im5RsRd,	CMP::N2 },	0b1111'1101, 0b1100'0000,
			NIMO::SHLR,		{ ADRM::Im5RsRd,	CMP::N2 },	0b1111'1101, 0b1000'0000,

			// #(1)
			NIMO::RACW,		{ ADRM::Imm1,		CMP::N3 },	0b0111'1101, 0b0001'1000, 0b0000'0000,

			// #(4)
			NIMO::MVTIPL,	{ ADRM::Imm4,		CMP::N3 },	0b0111'0101, 0b0111'0000, 0b0000'0000,

			// #(8)
			NIMO::RTSD,		{ ADRM::Imm8,		CMP::N1 },	0b0110'0111,
			NIMO::INT,		{ ADRM::Imm8,		CMP::N2 },	0b0111'0101, 0b0110'0000,

			// nmo.[BWL]
			NIMO::RMPA,		{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'1100,
			NIMO::SSTR,		{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'1000,
			NIMO::SUNTIL,	{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'0000,
			NIMO::SWHILE,	{ ADRM::Ns,			CMP::N2 },	0b0111'1111, 0b1000'0100,

			// nmo.[BWL] Rs
			NIMO::PUSH,		{ ADRM::NsRs,		CMP::N2 },	0b0111'1110, 0b1000'0000,
			// nmo.[BWL] [Rs] / dsp8[Rs] / dsp16[Rs]
			NIMO::PUSH,		{ ADRM::NsDsRs,		CMP::N2 },	0b1111'0110, 0b0000'1000,

			// #(3),[Rd] / dsp8[Rd] / dsp16[Rd]
			NIMO::BCLR,		{ ADRM::Im3DsRd,	CMP::N2 },	0b1111'0000, 0b0000'1000,
			NIMO::BNOT,		{ ADRM::Im3DsRd,	CMP::N3 },	0b1111'1100, 0b1110'0000, 0b0000'1111,
			NIMO::BSET,		{ ADRM::Im3DsRd,	CMP::N2 },	0b1111'0000, 0b0000'0000,
			NIMO::BTST,		{ ADRM::Im3DsRd,	CMP::N2 },	0b1111'0100, 0b0000'0000,

			// Rs,[Rd] / dsp8[Rd] / dsp16[Rd]
			NIMO::BCLR,		{ ADRM::RsDsRd,		CMP::N2 },	0b1111'1100, 0b0110'0100,
			NIMO::BNOT,		{ ADRM::RsDsRd,		CMP::N2 },	0b1111'1100, 0b0110'1100,
			NIMO::BSET,		{ ADRM::RsDsRd,		CMP::N2 },	0b1111'1100, 0b0110'0000,
			NIMO::BTST,		{ ADRM::RsDsRd,		CMP::N2 },	0b1111'1100, 0b0110'1000,

			NIMO::BCnd,		{ ADRM::bc3,		CMP::N1 },	0b0001'0000,
		};

		uint32_t DspRsRd1_(const uint8_t* bin, uint8_t ofs) noexcept
		{
			auto ld = bin[ofs] & 0b0000'0011;
			uint8_t mi = 0; // Byte
			auto rs = (bin[1 + ofs] & 0b1111'0000) >> 4;
			auto al = list_dsp_reg_(bin + 2 + ofs, ld, mi, rs);
			if(ld != 0b11) {
				adrm_ += ".UB";
			}
			list_reg_(bin[1 + ofs] & 0b0000'1111, ',');
			return al;
		}

		uint32_t DspRsRd2_(const uint8_t* bin, uint8_t ofs) noexcept
		{
			auto mi = (bin[1] & 0b1100'0000) >> 6;
			auto ld = bin[1] & 0b0000'0011;
			auto rs = (bin[2 + ofs] & 0b1111'0000) >> 4;
			auto al = list_dsp_reg_(bin + 2 + ofs, ld, mi, rs);
			adrm_ += size_str_[mi];
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
					if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							list_reg_(bin[1] & 0x0F);
							return 2;
						}
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_ && (bin[1] & 0b1111'0000) == asm_[l + 2].code_) {
							list_nimo_(nimo);
							list_reg_(bin[2] & 0x0F);
							return 3;
						}
					}
					break;
				case ADRM::RsRd:
					if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_reg_((bin[2] & 0b1111'0000) >> 4);
						list_reg_(bin[2] & 0b0000'1111, ',');
						return 3;
					}
					break;
				case ADRM::RsCR:
					if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_reg_((bin[2] & 0b1111'0000) >> 4);
						list_cr_(bin[2] & 0b0000'1111, ',');
						return 3;
					}
					break;
				case ADRM::RsRsRd:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_reg_((bin[2] & 0b1111'0000) >> 4);
						list_reg_(bin[2] & 0b0000'1111, ',');
						list_reg_(bin[1] & 0b0000'1111, ',');
						return 3;
					}
					break;
				case ADRM::Ofs8:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo, 'B');
						int32_t ofs = static_cast<int8_t>(bin[1]);
						list_sdec_(ofs);
						return 3;
					}
					break;
				case ADRM::Ofs16:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo, 'W');
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
						int32_t ofs = static_cast<int8_t>(bin[3]);
						ofs <<= 16;
						ofs |= (static_cast<uint32_t>(bin[2]) << 8) | bin[1];
						list_sdec_(ofs);
						return 4;
					}
					break;
				case ADRM::ImxRd:
					if(cmpl == 2) {
						if((bin[0] & 0b1111'1100) == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							auto li = (bin[1] & 0b0000'0011) >> 2;
							if(li == 0) li = 4;
							list_imm_(bin + 3, li);
							list_reg_(bin[1] & 0b0000'1111, ',');
							return 3 + li;
						} 
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0011) == asm_[l + 1].code_ &&
						  (bin[2] & 0b1111'0000) == asm_[l + 2].code_) {
							list_nimo_(nimo);
							auto li = (bin[1] & 0b0000'1100) >> 2;
							if(li == 0) li = 4;
							list_imm_(bin + 3, li);
							list_reg_(bin[2] & 0b0000'1111, ',');
							return 3 + li;
						}
					}
					break;
				case ADRM::ImxCR:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0011) == asm_[l + 1].code_ &&
					  (bin[2] & 0b1111'0000) == asm_[l + 2].code_) {
						list_nimo_(nimo);
						auto li = (bin[1] & 0b0000'1100) >> 2;
						if(li == 0) li = 4;
						list_imm_(bin + 3, li);
						list_cr_(bin[2] & 0b0000'1111, ',');
						return 3 + li;
					}
					break;
				case ADRM::RsDsRd:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1100) == asm_[l + 1].code_) {
						auto ld = bin[1] & 0b11;
						if(ld == 0b11) {
							break;
						}
						list_nimo_(nimo);
						list_reg_(bin[2] & 0b1111);
						adrm_ += ',';
//						auto rs = bin[2] >> 4;
//						auto al = list_dsp_(bin + 2, ld, mi, rs);
						if(ld == 0b01) {
							list_udec_(bin[3]);
						} else if(ld == 0b10) {
							list_udec_(bin[3] | (bin[4] << 8));
						}
						list_ireg_(bin[2] >> 4);
						return 3 + ld;
					}
					break;
				case ADRM::Im3DsRd:
					if(cmpl == 2) {
						if((bin[0] & 0b1111'1100) == asm_[l].code_ && (bin[1] & 0b1111'0111) == asm_[l + 1].code_) {
							auto ld = bin[0] & 0b11;
							if(ld == 0b11) {
								break;
							}
							auto im = bin[1] & 0b111;
							list_nimo_(nimo);
							list_udec_(im, '#');
							adrm_ += ',';
							if(ld == 0b01) {
								list_udec_(bin[2]);
							} else if(ld == 0b10) {
								list_udec_(bin[2] | (bin[3] << 8));
							}
							list_ireg_(bin[1] >> 4);
							return 2 + ld;
						}
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1110'0000) == asm_[l + 1].code_
						  && (bin[2] & 0b0000'1111) == asm_[l + 2].code_) {
							auto ld = bin[1] & 0b11;
							if(ld == 0b11) {
								break;
							}
							auto im = (bin[1] & 0b0001'1100) >> 2;
							list_nimo_(nimo);
							list_udec_(im, '#');
							adrm_ += ',';
							if(ld == 0b01) {
								list_udec_(bin[3]);
							} else if(ld == 0b10) {
								list_udec_(bin[3] | (bin[4] << 8));
							}
							list_ireg_(bin[2] >> 4);
							return 3 + ld;
						}
					}
					break;
				case ADRM::Im4Rd:
					if(bin[0] == asm_[l].code_) {
						list_nimo_(nimo);
						auto num = (bin[1] & 0b1111'0000) >> 4;
						list_udec_(num, '#');
						list_reg_(bin[1] & 0b0000'1111, ',');
						return 2;
					}
					break;

				case ADRM::Im5Rd:
					if(cmpl == 1) {
						if((bin[0] & 0b1111'1110) == asm_[l].code_) {
							list_nimo_(nimo);
							auto num = ((bin[0] & 0b0000'0001) << 4) | ((bin[1] & 0b1111'0000) >> 4);
							list_udec_(num, '#');
							list_reg_(bin[1] & 0b0000'1111, ',');
							return 2;
						}
					} else if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1110) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							auto num = ((bin[1] & 0b0000'0001) << 4) | ((bin[2] & 0b1111'0000) >> 4);
							list_udec_(num, '#');
							list_reg_(bin[2] & 0b0000'1111, ',');
							return 3;
						}
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1110'0000) == asm_[l + 1].code_
						  && (bin[2] & 0b1111'0000) == asm_[l + 2].code_) {
							list_nimo_(nimo);
							auto num = bin[1] & 0b0001'1111;
							list_udec_(num, '#');
							list_reg_(bin[2] & 0b0000'1111, ',');
							return 3;
						}
					}
					break;
				case ADRM::Im8Rd:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_udec_(bin[2], '#');
						list_reg_(bin[1] & 0b0000'1111, ',');
						return 3;
					}
					break;
				case ADRM::DsRsRd_f:
					if(cmpl == 1) {  // memex: UB or src==Rs(L)
						if((bin[0] & 0b1111'1100) == asm_[l].code_) {
							list_nimo_(nimo);
							auto al = DspRsRd1_(bin, 0);
							return 2 + al;
						}
					} else if(cmpl == 2) {  // memex: B, W, L, UW
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b0011'1100) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							auto al = DspRsRd2_(bin, 1);
							return 3 + al;
						}
					}
					break;
				case ADRM::DsRsRd:
					if(cmpl == 2) {  // memex == UB or src==Rs(L)
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1100) == asm_[l + 1].code_) {
							list_nimo_(nimo);
							auto al = DspRsRd1_(bin, 1);
							return 3 + al;
						}
					} else if(cmpl == 3) {  // memex: B, W, L, UW
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b0011'1100) == asm_[l + 1].code_ && bin[2] == asm_[l + 2].code_) {
							list_nimo_(nimo);
							auto al = DspRsRd2_(bin, 2);
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
						auto imm = bin[1] & 0b0001'1111;
						adrm_ += '#';
						list_udec_(imm);
						auto rs = (bin[2] & 0b1111'0000) >> 4;
						auto rd = bin[2] & 0b0000'1111;
						list_reg_(rs, ',');
						list_reg_(rd, ',');
						return 3;
					}
					break;
				case ADRM::Imm1:
					if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_ && (bin[2] & 0b1110'1111) == asm_[l + 2].code_) {
						auto im = (bin[2] & 0b0001'0000) >> 4;
						im++;
						list_nimo_(nimo);
						list_udec_(im, '#');
						return 3;
					}
					break;
				case ADRM::Imm4:
					if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_ && (bin[2] & 0b1111'0000) == asm_[l + 2].code_) {
						list_nimo_(nimo);
						list_udec_(bin[2] & 0b0000'1111, '#');
						return 3;
					}
					break;
				case ADRM::Imm8:
					if(cmpl == 1) {
						if(bin[0] == asm_[l].code_) {
							list_nimo_(nimo);
							auto imm = bin[1];
							list_udec_(imm, '#');
							return 2;
						}
					} else if(cmpl == 2) {
						if(bin[0] == asm_[l].code_ && bin[1] == asm_[l + 1].code_) {
							list_nimo_(nimo);
							auto imm = bin[2];
							list_udec_(imm, '#');
							return 3;
						}
					}
					break;
				case ADRM::ImxRsRd:
					if((bin[0] & 0b1111'1100) == asm_[l].code_) {
						auto li = bin[0] & 0b0000'0011;
						if(li == 0) li = 4;
						list_nimo_(nimo);
						list_imm_(bin + 2, li);
						auto rs = (bin[1] & 0b1111'0000) >> 4;
						auto rd = bin[1] & 0b0000'1111;
						if(rs != rd) {
							list_reg_(rs, ',');
						} 
						list_reg_(rd, ',');
						return 2 + li;
					}
					break;
				case ADRM::cb:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
						list_cb_(bin[1] & 0b0000'1111);
						return 2;
					}
					break;
				case ADRM::cr:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'0000) == asm_[l + 1].code_) {
						list_nimo_(nimo);
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
						adrm_ += '#';
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
						list_nimo_size_(nimo, sz);
						return 2;
					}
					break;
				case ADRM::NsRs:
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1100'0000) == asm_[l + 1].code_) {
						auto sz = (bin[1] & 0b0011'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						list_reg_(bin[1] & 0b0000'1111);
						return 2;
					}
					break;
				case ADRM::NsDsRs:
					if((bin[0] & 0b1111'1100) == asm_[l].code_ && (bin[1] & 0b0000'1100) == asm_[l + 1].code_) {
						auto ld = bin[0] & 0b11;
						if(ld == 0b11) {
							break;
						}
						auto sz = bin[1] & 0b11;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						if(ld == 1) {
							list_udec_(bin[2]);
						} else if(ld == 2) {
							list_udec_(bin[2] | (bin[3] << 8));
						}
						list_ireg_(bin[1] >> 4);
						return 2 + ld;
					}
					break;
				case ADRM::bc3:
					if((bin[0] & 0b1111'0000) == asm_[l].code_) {
						if(bin[0] & 0b0000'1000) {  // BNE, BNZ
							adrm_ += "BNE";
						} else {  // BEQ, BZ
							adrm_ += "BEQ";
						}
						static constexpr int8_t tbl[] = { 8, 9, 10, 3, 4, 5, 6, 7 };
						auto dsp = tbl[bin[0] & 0b111];
						list_sdec_(dsp);
						return 1;
					}
					break;
				case ADRM::RsDs5Rd:  // MOV (1)
					if((bin[0] & 0b1100'1000) == asm_[l].code_) {
						auto sz = (bin[0] & 0b0011'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						auto dsp = (bin[1] >> 3) & 1;
						dsp |= (bin[1] >> 6) & 0b10;
						dsp |= bin[0] << 2;
						list_reg_((bin[1] & 0b0111'0000) >> 4);
						list_udec_(dsp);
						list_ireg_(bin[1] & 0b111, ',');
						return 2;
					}
					break;
				case ADRM::Ds5RsRd:  // MOV (2)
					if((bin[0] & 0b1100'1000) == asm_[l].code_) {
						auto sz = (bin[0] & 0b0011'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						auto dsp = (bin[1] >> 3) & 1;
						dsp |= (bin[1] >> 6) & 0b10;
						dsp |= bin[0] << 2;
						list_udec_(dsp);
						list_ireg_(bin[1] & 0b111);
						list_reg_((bin[1] & 0b0111'0000) >> 4, ',');
						return 2;
					}
					break;
				case ADRM::Im8Ds5Rd:  // MOV (4)
					if((bin[0] & 0b1111'1100) == asm_[l].code_) {
						auto sz = bin[0] & 0b1111'11;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						auto dsp = ((bin[1] & 0b1000'0000) >> 3) | bin[1];
						adrm_ += '#';
						list_udec_(bin[2]);
						adrm_ += ',';
						list_udec_(dsp);
						list_ireg_((bin[1] & 0b0111'0000) >> 4);
						return 3;
					}
					break;
				case ADRM::ImxRd_:  // MOV (6)
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1000) == asm_[l + 1].code_) {
						auto li = (bin[1] >> 2) & 0b11;
						if(li == 0) li = 4;
						list_nimo_(nimo);
						list_imm_(bin + 2, li);
						list_reg_(bin[1] >> 4, ',');
						return 2 + li;
					}
					break;
				case ADRM::NsRsRd:  // MOV (7)
					if((bin[0] & 0b1100'1111) == asm_[l].code_) {
						auto sz = (bin[0] & 0b0011'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						list_reg_(bin[1] >> 4);
						list_reg_(bin[1] & 0b1111, ',');
						return 2;
					}
					break;


				case ADRM::NsDsRsRd:  // MOV (9)
					if((bin[0] & 0b1100'1100) == asm_[l].code_) {
						auto sz = (bin[0] & 0b0011'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						auto ld = bin[0] & 0b11;
						if(ld == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						list_dsp_reg_(bin + 2, ld, sz, bin[1] >> 4);
						list_reg_(bin[1] & 0b1111, ',');
						return 2 + ld;
					}
					break;
				case ADRM::NsRiRbRd:  // MOV (10)
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1100'0000) == asm_[l + 1].code_) {
						auto sz = (bin[1] & 0b11'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						list_ireg2_(bin[1] & 0b1111, bin[2] >> 4);
						list_reg_(bin[2] & 0b1111, ',');
						return 3;
					}
					break;
				case ADRM::NsRsDsRd:  // MOV (11)
					if((bin[0] & 0b1100'1100) == asm_[l].code_) {
						auto sz = (bin[0] & 0b0011'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						auto ld = (bin[0] & 0b1100) >> 2;
						if(ld == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						list_reg_(bin[1] >> 4);
						list_dsp_reg_(bin + 2, ld, sz, bin[1] & 0b1111, ',');
						return 2 + ld;
					}
					break;

				case ADRM::NsRsRiRb:  // MOV (12)
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1100'0000) == asm_[l + 1].code_) {
						auto sz = (bin[1] & 0b11'0000) >> 4;
						if(sz == 0b11) {
							break;
						}
						list_nimo_size_(nimo, sz);
						list_reg_(bin[2] & 0b1111);
						list_ireg2_(bin[1] & 0b1111, bin[2] >> 4, ',');
						return 3;
					}
					break;



				case ADRM::RsPidRd:  // MOV (14)
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1000) == asm_[l + 1].code_) {
						auto sz = bin[1] & 0b11;
						if(sz == 0b11) {
							break;
						}
						auto ad = (bin[1] & 0b0000'0100) >> 2;
						list_nimo_size_(nimo, sz);
						list_preg_(bin[2] & 0b1111, ad);
						list_reg_(bin[2] >> 4, ',');
						return 3;
					}
					break;
				case ADRM::PidRsRd:  // MOV (15)
					if(bin[0] == asm_[l].code_ && (bin[1] & 0b1111'1000) == asm_[l + 1].code_) {
						auto sz = bin[1] & 0b11;
						if(sz == 0b11) {
							break;
						}
						auto ad = (bin[1] & 0b0000'0100) >> 2;
						list_nimo_size_(nimo, sz);
						list_reg_(bin[2] >> 4);
						list_preg_(bin[2] & 0b1111, ad, ',');
						return 3;
					}
					break;
				default:
					return 0;
				}
				l += cmpl;
			}
			return 0;
		}


		void list_fp_(const uint8_t* bin, char ch = 0) noexcept
		{
			if(ch != 0) {
				adrm_ += ch;
			}
			struct pad {
				union {
					uint32_t	u32;
					float		fp;
				};
			};
			pad p;
			p.u32 = bin[0] | (bin[1] << 8) | (bin[2] << 16) | (bin[3] << 24);
			fpval_ = p.fp;
			char tmp[16];
			utils::sformat("0x%08X", tmp, sizeof(tmp)) % p.u32;
			adrm_ += tmp;
		}


		static constexpr ASM fpu_[] = {
			// #fp32,Rd
			NIMO::FADD,		{ ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0010'0000,
			NIMO::FCMP,		{ ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0001'0000,
			NIMO::FDIV,		{ ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0100'0000,
			NIMO::FMUL,		{ ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0011'0000,
			NIMO::FSUB,		{ ADRM::ImfpRd,	CMP::N3 },	0b1111'1101, 0b0111'0010, 0b0000'0000,
			// Rs,Rd / [Rs],Rd / dsp8[Rs],Rd / dsp16[Rs],Rd
			NIMO::FADD,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'1000,
			NIMO::FCMP,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'0100,
			NIMO::FDIV,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1001'0000,
			NIMO::FMUL,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'1100,
			NIMO::FSUB,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1000'0000,
			NIMO::FTOI,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1001'0100,
			NIMO::FTOU,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1010'0100,
			NIMO::ROUND,	{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1001'1000,
			NIMO::FSQRT,	{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b1010'1000,
			// Rs.UB,Rd / [Rs].UB,Rd / dsp8[Rs].UB,Rd / dsp16[Rs].UB,Rd
			NIMO::ITOF,		{ ADRM::DsRsRd,	CMP::N2 },	0b1111'1100, 0b0100'0100,
			NIMO::UTOF,		{ ADRM::FpRsRd,	CMP::N2 },	0b1111'1100, 0b0101'0100,
			// Rs.mi,Rd / [Rs].mi,Rd / dsp8[Rs].mi,Rd / dsp16[Rs].mi,Rd (mi: B, W, L, UW)
			NIMO::ITOF,		{ ADRM::DsRsRd,	CMP::N3 },	0b0000'0110, 0b0000'0001, 0b0001'0001,
			NIMO::UTOF,		{ ADRM::DsRsRd,	CMP::N3 },	0b0000'0110, 0b0000'0001, 0b0001'0101,
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
						list_fp_(bin + 3, '#');
						list_reg_(bin[2] & 0b0000'1111, ',');
						return 7;
					}
					break;
				case ADRM::FpRsRd:
					if(bin[0] == fpu_[l].code_ && (bin[1] & 0b1111'1100) == fpu_[l + 1].code_) {
						if(rx_type_ == RX_TYPE::RXv1) {
							if(nimo == NIMO::FSQRT || nimo == NIMO::FTOU) {
								break;
							}
						}
						list_nimo_(nimo);
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
							if(rx_type_ == RX_TYPE::RXv1 && nimo == NIMO::UTOF) {
								break;
							}
							list_nimo_(nimo);
							auto al = DspRsRd1_(bin, 1);
							return 3 + al;
						}
					} else if(cmpl == 3) {
						if(bin[0] == asm_[l].code_ && (bin[1] & 0b0011'1100) == asm_[l + 1].code_ && bin[2] == asm_[l + 2].code_) {
							if(rx_type_ == RX_TYPE::RXv1 && nimo == NIMO::UTOF) {
								break;
							}
							list_nimo_(nimo);
							auto al = DspRsRd2_(bin, 2);
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
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		rxdisasm() noexcept : nimo_(), adrm_(), fpval_(0.0f), rx_type_(RX_TYPE::RXv1) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  RX タイプを設定
			@param[in]	type	タイプ型
		*/
		//-----------------------------------------------------------------//
		void set_cpu_type(RX_TYPE type) noexcept { rx_type_ = type; }


		//-----------------------------------------------------------------//
		/*!
			@brief  逆アセンブラ @n
					結果が「０」の場合、未定義命令（単なるバイトコード）とする
			@param[in]	bin		バイナリーソース
			@param[out]	nimo	ニーモニック出力
			@param[out]	adrm	アドレスモード出力
			@return 翻訳命令数
		*/
		//-----------------------------------------------------------------//
		uint32_t disasm(const uint8_t* bin, STRING& nimo, STRING& adrm) noexcept
		{
			adrm_ = "";
			nimo_ = "";
			fpval_ = 0.0f;
			auto n = rxv1_(bin);
			if(n == 0) {
				n = rx_fpu_(bin);
			}
			nimo = nimo_;
			adrm = adrm_;
			return n;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  浮動小数点の直値 @n
					イミディエイト浮動小数点命令翻訳時に取引される値
			@return	浮動小数点数
		*/
		//-----------------------------------------------------------------//
		auto get_float() const noexcept { return fpval_; }
	};
}
