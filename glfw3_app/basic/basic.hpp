#pragma once
//=====================================================================//
/*!	@file
	@brief	BASIC インタープリター・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include <cstring>
#include "buff.hpp"
#include "format.hpp"

namespace interpreter {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	BASIC 言語テンプレート・クラス
		@param[in]	VAL	基本となる変数の型
		@param[in]	BUFF	バッファ定義
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename VAL, class BUFF = utils::buff<1024> >
	class basic
	{
		// バッファの構造：
		// n: VAL のタイプにより（１、２、４）
		// ------
		// +0:   行番号インデックス
		// +n:   行のサイズ（１）最大２５６バイト
		// +n+1: 
		BUFF	buff_;

		struct adr_t {
			uint16_t	pos;
			uint8_t		len;
		};

		VAL get_(uint16_t pos) const {
			if(sizeof(VAL) == 1) return buff_.get8(pos);
			else if(sizeof(VAL) == 2) return buff_.get16(pos);
			else if(sizeof(VAL) == 4) return buff_.get32(pos);
			else return 0;
		}

		struct str_t {
			const char* str;
			uint8_t	len;
		};

		static uint8_t count_word_(const char* p) {
			char bc = ' ';
			uint8_t n = 0;
			while(1) {
				auto ch = *p++;
				if(ch == 0) break;
				if(bc == ' ' && ch != ' ') {
					++n;
				}
				bc = ch;
			}
			return n;
		}

		static bool get_word_(const char* p, str_t& out) {
			char bc = ' ';
			out.str = nullptr;
			out.len = 0;
			while(1) {
				auto ch = *p;
				if(bc == ' ' && ch != ' ') {
					out.str = p;
				}
				if(bc != ' ' && (ch == ' ' || ch == 0)) {
					out.len = p - out.str;
					return true;
				}
				++p;
				bc = ch;
			}
			return false;
		}


		static bool get_dec_(const str_t& no, VAL& val) {
			const char* p = no.str;
			val = 0;
			for(uint8_t n = 0; n < no.len; ++n) {
				auto ch = *p++;
				if(ch >= '0' && ch <= '9') {
					val *= 10;
					val += ch - '0';
				} else {
					utils::format("'%c'\n") % ch;
					return false;
				}
			}
			return true;
		}


		adr_t find_index_(VAL index) {
			typename BUFF::index_type i = 0;
			adr_t t;
			t.len = 0;
			while(i < buff_.get_front_size()) {
				auto n = get_(i);
				i += sizeof(VAL);
				if(n == index) {
					t.pos = i + 1;
					t.len = buff_.get8(i);
					break;
				}
				i += buff_.get8(i) + 1;
			}
			return t;
		}


		void dump_(uint16_t pos, uint16_t len) {
			while(len > 0) {
				auto v = buff_.get8(pos);
				utils::format(" %02X") % static_cast<uint16_t>(v);
				++pos;
				--len;
			}
		}

		enum class OPR {
			REM,
			GOTO,
			GOSUB,
			RETURN,
			FOR,
			TO,
			STEP,
			NEXT,
			IF,
			STOP,

			NONE_
		};

		static const char* opr_key_[];

		OPR scan_opr_(const str_t& param) {
			for(uint8_t i = 0; i < static_cast<uint8_t>(OPR::NONE_); ++i) { 
				if(std::strncmp(opr_key_[i], param.str, param.len) == 0) {
					return static_cast<OPR>(i);
				}
			}
			return OPR::NONE_;
		}

#if 0
	"OK",
	"Devision by zero",
	"Overflow",
	"Subscript out of range",
	"Icode buffer full",
	"List full",
	"GOSUB too many nested",
	"RETURN stack underflow",
	"FOR too many nested",
	"NEXT without FOR",
	"NEXT without counter",
	"NEXT mismatch FOR",
	"FOR without variable",
	"FOR without TO",
	"LET without variable",
	"IF without condition",
	"Undefined line number",
	"\'(\' or \')\' expected",
	"\'=\' expected",
	"Illegal command",
	"Syntax error",
	"Internal error",
	"Abort by [ESC]"
#endif


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		basic() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デコード
			@param[in]	line	ライン
			@param[in]	pos		中間コードの出力先
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool decode(const str_t& line, uint16_t pos)
		{
			auto n = count_word_(line.str);
			const char* p = line.str;
			while(n > 0) {
				str_t w;
				if(get_word_(p, w)) {
					auto opr = scan_opr_(w);
					switch(opr) {
					case OPR::REM:
						p += w.len;
						--n;
						get_word_(p, w);
						
						break;
					case OPR::NONE_:
					default:

						break;
					}
				}
				p += w.len;
				--n;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	インサート（プログラム入力）
			@param[in]	index	行番号
			@param[in]	param	パラメーター
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool insert(const str_t& index, const str_t& param)
		{
			VAL v;
			if(!get_dec_(index, v)) {
				utils::format("Syntax error: %s\n") % index.str;
				return false;
			}
			auto t = find_index_(v);
			if(t.len == 0) {  // 行番号が見つからない場合（新規）
				if(!decode(param, buff_.get_front_size())) {
					utils::format("Syntax error: %s\n") % param.str;
					return false;
				}
			} else {


			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	INFO
			@param[in]	param	パラメーター
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool cmd_info(const str_t& param)
		{
			utils::format("Free space: %d bytes\n") % buff_.get_free();
//			utils::format("Variable: 
			utils::format("\n");
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	LIST
			@param[in]	param	パラメーター
		*/
		//-----------------------------------------------------------------//
		bool cmd_list(const str_t& param)
		{
			typename BUFF::index_type pos = 0;
			while(pos < buff_.get_front_size()) {
				auto idx = get_(pos);
				pos += sizeof(VAL);
				uint8_t len = buff_.get8(pos);
				++pos;
				utils::format("%d ") % idx;

				dump_(pos, len);

				pos += len;
			}
			utils::format("\n");
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	RUN
			@param[in]	param	パラメーター
		*/
		//-----------------------------------------------------------------//
		bool cmd_run(const str_t& param)
		{
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	NEW
			@param[in]	param	パラメーター
		*/
		//-----------------------------------------------------------------//
		bool cmd_new(const str_t& param)
		{
			buff_.clear();
			utils::format("\n");
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
			@param[in]	line	行
		*/
		//-----------------------------------------------------------------//
		void service(const char* line)
		{
			if(line == nullptr) return;

			auto cn = count_word_(line);
			if(cn == 0) return;

			str_t cmd;
			if(!get_word_(line, cmd)) {
				utils::format("Command error: %s\n") % line;
				return;
			}

			str_t param;
			param.str = cmd.str + cmd.len;
			param.len = sizeof(line) - cmd.len;

			if(cmd.str[0] >= '0' && cmd.str[0] <= '9') {
				insert(cmd, param);
			} else if(std::strncmp("info", cmd.str, cmd.len) == 0) {
				cmd_info(param);
			} else if(std::strncmp("list", cmd.str, cmd.len) == 0) {
				cmd_list(param);
			} else if(std::strncmp("run", cmd.str, cmd.len) == 0) {
				cmd_run(param);
			} else if(std::strncmp("new", cmd.str, cmd.len) == 0) {
				cmd_new(param);
			} else {
				utils::format("Syntax error: %s\n") % cmd.str;
			}
		}
	};

	// テンプレート内「static」の実体
	template <typename VAL, class BUFF>
	const char* basic<VAL, BUFF>::opr_key_[] = {
		"rem",
		"goto", "gosub", "return",
		"for", "to", "step", "next",
		"if", "stop",
	};

#if 0
	"IF", "REM", "STOP",
	"INPUT", "PRINT", "LET",
	",", ";",
	"-", "+", "*", "/", "(", ")",
	">=", "#", ">", "=", "<=", "<",
	 "@", "RND", "ABS", "SIZE",
	"LIST", "RUN", "NEW", "SYSTEM"
#endif

}
