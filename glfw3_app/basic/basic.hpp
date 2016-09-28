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
			uint8_t		len;
			str_t() : str(nullptr), len(0) { }
		};


		static void get_word_(const char* p, str_t& out) {
			char bc = ' ';
			out.str = nullptr;
			out.len = 0;
			while(1) {
				auto ch = *p;
				if(bc == ' ' && ch != ' ') {
					out.str = p;
					if(ch == 0) return;
				}
				if(bc != ' ' && (ch == ' ' || ch == 0)) {
					out.len = p - out.str;
					return;
				}
				++p;
				bc = ch;
			}
		}


		static bool get_dec_(const str_t& no, bool sign, VAL& val) {
			const char* p = no.str;
			val = 0;
			bool inv = false;
			for(uint8_t n = 0; n < no.len; ++n) {
				auto ch = *p++;
				if(sign) {
					if(ch == '-') { inv = true; sign = false; }
					else if(ch == '+') { sign = false; }
				} else if(ch >= '0' && ch <= '9') {
					val *= 10;
					val += ch - '0';
				} else {
					utils::format("'%c'\n") % ch;
					return false;
				}
			}
			if(inv) val = -val;
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
			THEN,
			STOP,

			NONE_,
			str0_,	///< 文字列[0]
			str1_,	///< 文字列[1]
			str2_,	///< 文字列[2]
			ndec_,	///< 符号なし整数
			sdec_,	///< 符号つき整数
			cmp_,	///< 比較
		};

		static const char* opr_key_[];

		static OPR scan_opr_(const str_t& param) {
			for(uint8_t i = 0; i < static_cast<uint8_t>(OPR::NONE_); ++i) { 
				if(std::strncmp(opr_key_[i], param.str, param.len) == 0) {
					return static_cast<OPR>(i);
				}
			}
			return OPR::NONE_;
		}

		struct parse_ret {
			uint16_t	match_;
			VAL			dec_;
			str_t		str_[3];
			void reset() {
				match_ = 0;
				dec_ = 0;
			}
		};

		class parse {
			parse_ret&	ret_;
			const char* command_;
		public:
			parse(parse_ret& ret, const char* command) : ret_(ret), command_(command) { ret_.reset(); }

			parse& operator % (OPR opr) {
				if(command_[0] == 0) return *this;

				str_t w;
				get_word_(command_, w);
				if(w.len == 0) return *this;

				ret_.match_ <<= 1;
				if(opr == OPR::ndec_) {
					VAL val;
					if(get_dec_(w, false, val)) {
						ret_.dec_ = val;
						ret_.match_ |= 1;
					}
				} else if(opr == OPR::sdec_) {
					VAL val;
					if(get_dec_(w, true, val)) {
						ret_.dec_ = val;
						ret_.match_ |= 1;
					}
				} else if(opr == OPR::str0_) {
					ret_.str_[0] = w;
					ret_.match_ |= 1;
				} else if(opr == OPR::str1_) {
					ret_.str_[1] = w;
					ret_.match_ |= 1;
				} else if(opr == OPR::str2_) {
					ret_.str_[2] = w;
					ret_.match_ |= 1;
				} else if(opr == scan_opr_(w)) {
					ret_.match_ |= 1;
				}
				command_ = w.str + w.len;

				return *this;
			}
		};

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

		bool put_opr_(OPR opr) {
			uint8_t v = static_cast<uint8_t>(opr);
			return buff_.add_front(&v, 1);
		}

		void output_str_(const str_t& t) {
			const char* p = t.str;
			for(uint8_t i = 0; i < t.len; ++i) {
				utils::format("%c") % *p;
				++p;
			}
		}

		bool put_str_(const str_t& t) {
			uint8_t n = t.len;
			if(!buff_.add_front(&n, 1)) {
				return false;
			}
			return buff_.add_front(t.str, t.len);
		}

		bool put_block_(VAL idx, uint8_t len) {
			if(len > buff_.get_free()) {
				return false;
			}
			buff_.add_front(&idx, sizeof(VAL));
			buff_.add_front(&len, 1);
			return true;	
		}


		void list_sub_(uint16_t pos) {
			OPR opr = static_cast<OPR>(buff_.get8(pos));
			++pos;
			switch(opr) {
			case OPR::REM:
				{
					str_t t;
					t.len = buff_.get8(pos);
					++pos;
					t.str = static_cast<const char*>(buff_.get(pos));
					utils::format("rem ");
					output_str_(t);
					pos += t.len;
				}
				break;
			default:
				break;
			}
			utils::format("\n");
		}

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
			@param[in]	idx		行番号
			@param[in]	line	ライン
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool decode(VAL idx, const str_t& line)
		{
			parse_ret ret;

			// rem
			parse(ret, line.str) % OPR::REM % OPR::str0_;
			if(ret.match_ == 0b11) {
				if(!put_block_(idx, 2 + ret.str_[0].len)) {
					return false;
				}
				put_opr_(OPR::REM);
				put_str_(ret.str_[0]);
				return true;
			}

			// goto
			parse(ret, line.str) % OPR::GOTO % OPR::ndec_;
			if(ret.match_ == 0b11) {
				utils::format("GOTO: %d\n") % ret.dec_;
				return true;
			}

			// gosub
			parse(ret, line.str) % OPR::GOSUB % OPR::ndec_;
			if(ret.match_ == 0b11) {
				utils::format("GOSUB: %d\n") % ret.dec_;
				return true;
			}

			// stop
			parse(ret, line.str) % OPR::STOP;
			if(ret.match_ == 0b1) {
				utils::format("STOP\n");
				return true;
			}

			// return
			parse(ret, line.str) % OPR::RETURN;
			if(ret.match_ == 0b1) {
				utils::format("RETURN\n");
				return true;
			}

			// if a = V then CMD
			parse(ret, line.str) % OPR::IF % OPR::str0_ % OPR::cmp_ % OPR::str1_ % OPR::THEN % OPR::str2_;
			if(ret.match_ == 0b111111) {
				utils::format("IF\n");
				return true;
			}

#if 0
			// for $ = n to m step l 
			parse(ret, line.str)
				% OPR::FOR % OPR::str_ % OPR::EQ % OPR::dec_ % OPR::TO % OPR::dec_ % OPR::STEP % OPR::dec_;
			if(ret.match_ == 0b11) {
				utils::format("GOSUB: %d\n") % ret.dec_;
				return true;
			}

			// next
			parse(ret, line.str) % OPR::NEXT;
			if(ret.match_ == 0b1) {
				utils::format("NEXT\n");
				return true;
			}
#endif

			utils::format("Return code: %04X\n") % static_cast<uint16_t>(ret.match_);

			return false;
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
			if(!get_dec_(index, false, v)) {
				utils::format("Syntax error: %s\n") % index.str;
				return false;
			}

			auto t = find_index_(v);
			if(t.len == 0) {  // 行番号が見つからない場合（新規）
				if(!decode(v, param)) {
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
				utils::format("%d ") % idx;
				uint8_t bkn = buff_.get8(pos);
				++pos;
				list_sub_(pos);
				pos += bkn;
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

			str_t cmd;
			get_word_(line, cmd);
			if(cmd.len == 0) return;

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
		"if", "then", "stop",
	};

#if 0
	"INPUT", "PRINT", "LET",
	",", ";",
	"-", "+", "*", "/", "(", ")",
	">=", "#", ">", "=", "<=", "<",
	 "@", "RND", "ABS", "SIZE",
	"LIST", "RUN", "NEW", "SYSTEM"
#endif

}