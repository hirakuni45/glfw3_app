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
#include "stack.hpp"
#include "utils/format.hpp"
#include "basic_arith.hpp"

// #include <boost/format.hpp>

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
	public:
		typedef bool (*save_func)(const char*, const BUFF&);
		typedef bool (*load_func)(const char*, BUFF&);

	private:
		// バッファの構造：
		// n: VAL のタイプにより（１、２、４）
		// ------
		// +0:   行番号インデックス
		// +n:   行のサイズ（１）最大２５６バイト
		// +n+1: 
		BUFF	buff_;

		save_func	save_func_;
		load_func	load_func_;

		utils::basic_arith<VAL> arith_;

		struct str_t {
			const char* str;
			uint8_t		len;
			str_t() : str(nullptr), len(0) { }
		};


		static void get_word_(const char* p, str_t& out) {
			out.str = nullptr;
			out.len = 0;
			if(p == nullptr) return;
			char bc = ' ';
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

		static const char* get_symbol_(const char* p, str_t& out) {
			get_word_(p, out);
			if(out.len == 0) return nullptr;

			auto ch = out.str[0];
			if((ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z')) ;
			else {
				return nullptr;
			}

			const char* eqp = std::strchr(out.str, '=');
			if(eqp == nullptr) return nullptr;
			const char* spc = std::strchr(out.str, ' ');
			if(spc != nullptr) out.len = spc - out.str;
			else out.len = eqp - out.str;
			return eqp;
		}

		static bool get_dec_(const str_t& no, bool sign, VAL& val) {
			const char* p = no.str;
			val = 0;
			bool inv = false;
			for(uint8_t n = 0; n < no.len; ++n) {
				auto ch = *p++;
				if(ch == 0) break;
				else if(sign) {
					if(ch == '-') { inv = true; sign = false; }
					else if(ch == '+') { sign = false; }
				} else if(ch >= '0' && ch <= '9') {
					val *= 10;
					val += ch - '0';
				} else {
					return false;
				}
			}
			if(inv) val = -val;
			return true;
		}


		bool find_index_(uint16_t index, uint16_t& pos) {
			pos = 0;
			while(pos < buff_.get_front_size()) {
				uint16_t idx = buff_.get2(pos);
				if(idx == index) {
					return true;
				}
				pos += buff_.get1(pos + 2);
				pos += 3;
			}
			return false;
		}


		uint16_t scan_index_(uint16_t index) {
			uint16_t pos = 0;
			while(pos < buff_.get_front_size()) {
				uint16_t org = buff_.get2(pos);
				uint16_t len = buff_.get1(pos + 2);
				len += 3;
				uint16_t end = 65535;
				if((pos + len) < buff_.get_front_size()) {
					end = buff_.get2(pos + len);
				}
				if(index <= org) break;
				if(org < index && index <= end) { 
					return pos + len;
				}
				pos += len;
			}
			return pos;
		}

#if 0
		void dump_(uint16_t pos, uint16_t len) {
			while(len > 0) {
				auto v = buff_.get8(pos);
				utils::format(" %02X") % static_cast<uint16_t>(v);
				++pos;
				--len;
			}
		}
#endif

		enum class OPR : uint8_t {
			REM,
			GOTO,
			GOSUB,
			STOP,
			RETURN,
			PRINT,
			INPUT,
			FOR,
			TO,
			STEP,
			NEXT,
			IF,
			THEN,

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
			const char* last_;
			void reset() {
				match_ = 0;
				dec_ = 0;
				last_ = nullptr;
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
				ret_.last_ = command_;

				return *this;
			}
		};

		void output_str_(const str_t& t, char ch) {
			if(ch != 0) utils::format("%c") % ch;
			const char* p = t.str;
			for(uint8_t i = 0; i < t.len; ++i) {
				utils::format("%c") % *p;
				++p;
			}
			if(ch != 0) utils::format("%c") % ch;
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
		void put_block_(uint16_t idx, uint8_t len, uint16_t dst) {
			buff_.set2(idx, dst);
			buff_.set1(len, dst + 2);
		}

		void put_opr_(OPR opr, uint16_t dst) {
			buff_.set1(static_cast<uint8_t>(opr), dst);
		}

		void list_sub_(uint16_t pos) {
			auto opr = buff_.get1(pos);
			++pos;
			if(opr < static_cast<uint8_t>(OPR::NONE_)) {
				utils::format("%s ") % opr_key_[opr];
			} else {
				utils::format("(%02X) ") % static_cast<int>(opr);
			}
			switch(static_cast<OPR>(opr)) {
			case OPR::REM:
				{
					str_t t;
					t.len = buff_.get1(pos);
					++pos;
					t.str = static_cast<const char*>(buff_.get(pos));
					output_str_(t, 0);
					pos += t.len;
				}
				break;

			case OPR::GOTO:
			case OPR::GOSUB:
				{
					utils::format("%d") % buff_.get2(pos);
					pos += 4;
				}
				break;

			case OPR::PRINT:
				{
					str_t t;
					t.len = buff_.get1(pos);
					++pos;
					t.str = static_cast<const char*>(buff_.get(pos));
					output_str_(t, '"');
					pos += t.len;
				}
				break;

			default:
				break;
			}
			utils::format("\n");
		}

		bool size_check_move_(uint16_t ins, uint16_t tlen) {
		   	if(tlen > buff_.get_free()) {
		   		utils::format("Memory empty: %d\n") % tlen;
		   		return false;
		   	}
		   	if(ins < buff_.get_front_size()) {
		   		buff_.move(ins, ins + tlen, buff_.get_front_size() - ins);
		   	}
			return true;
		}

		uint16_t get_index_(const parse_ret& ret) {
		   	VAL val;
			str_t t;
			t.str = ret.str_[0].str;
			t.len = std::strlen(ret.str_[0].str);
		   	if(!get_dec_(t, false, val)) {
				utils::format("Syntax error: %s\n") % t.str;
		   		return 0;
		   	}
		   	if(val == 0 || val > 65535) {
				utils::format("Overflow: %d\n") % val; 
		   		return 0;
		   	}
			return val;
		}

		str_t get_string_(const parse_ret& ret) {
			const char* p = ret.str_[0].str;
			str_t t;
			t.str = nullptr;
			t.len = 0;
			if(p == nullptr) return t;
			if(p[0] != '"') return t;
			uint16_t l = std::strlen(p);
			if(p[l - 1] != '"') return t;
			t.str = p + 1;
			t.len = l - 2;
			return t;
		}

		bool last_check_(const parse_ret& ret) {
			str_t t;
			get_word_(ret.last_, t);
			if(t.len == 0) return true;

			utils::format("Syntax error: %s\n") % t.str;
			return false;			
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	svf	SAVE 関数
			@param[in]	ldf	LOAD 関数
		*/
		//-----------------------------------------------------------------//
		basic(save_func svf = nullptr, load_func ldf = nullptr) : save_func_(svf), load_func_(ldf) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デコード
			@param[in]	idx	行番号
			@param[in]	src	スクリプト・ソース
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool decode(uint16_t idx, const str_t& src)
		{
			// デコード位置を取得
			auto ins = scan_index_(idx);

			parse_ret ret;

			// rem
			parse(ret, src.str) % OPR::REM % OPR::str0_;
			if(ret.match_ == 0b11) {
				uint8_t slen = std::strlen(ret.str_[0].str);
				uint16_t tlen = 2 + 1 + 1 + 1 + slen;
				if(!size_check_move_(ins, tlen)) return false;
				put_block_(idx, tlen - 3, ins);
				put_opr_(OPR::REM, ins + 3);
				buff_.set1(slen, ins + 4);
				buff_.set(ret.str_[0].str, slen, ins + 5);
				buff_.resize_front(buff_.get_front_size() + tlen);
				return true;
			}

			// goto
			parse(ret, src.str) % OPR::GOTO % OPR::ndec_;
			if(ret.match_ == 0b11) {
				auto adr = get_index_(ret);
				if(adr == 0) return false;
				uint16_t tlen = 2 + 1 + 1 + 2 + 2;
				if(!size_check_move_(ins, tlen)) return false;
				put_block_(idx, tlen - 3, ins);
				put_opr_(OPR::GOTO, ins + 3);
				buff_.set2(adr, ins + 4);
				buff_.set2(0, ins + 6);  // optimize dummy
				buff_.resize_front(buff_.get_front_size() + tlen);
				return true;
			}

			// gosub
			parse(ret, src.str) % OPR::GOSUB % OPR::ndec_;
			if(ret.match_ == 0b11) {
				auto adr = get_index_(ret);
				if(adr == 0) return false;
				uint16_t tlen = 2 + 1 + 1 + 2 + 2;
				if(!size_check_move_(ins, tlen)) return false;
				put_block_(idx, tlen - 3, ins);
				put_opr_(OPR::GOSUB, ins + 3);
				buff_.set2(adr, ins + 4);
				buff_.set2(0, ins + 6);  // optimize dummy
				buff_.resize_front(buff_.get_front_size() + tlen);
				return true;
			}

			// stop
			parse(ret, src.str) % OPR::STOP;
			if(ret.match_ == 0b1) {
				if(!last_check_(ret)) return false;
				uint16_t tlen = 2 + 1 + 1;
				if(!size_check_move_(ins, tlen)) return false;
				put_block_(idx, tlen - 3, ins);
				put_opr_(OPR::STOP, ins + 3);
				buff_.resize_front(buff_.get_front_size() + tlen);
				return true;
			}

			// return
			parse(ret, src.str) % OPR::RETURN;
			if(ret.match_ == 0b1) {
				if(!last_check_(ret)) return false;
				uint16_t tlen = 2 + 1 + 1;
				if(!size_check_move_(ins, tlen)) return false;
				put_block_(idx, tlen - 3, ins);
				put_opr_(OPR::RETURN, ins + 3);
				buff_.resize_front(buff_.get_front_size() + tlen);
				return true;
			}

			// print
			parse(ret, src.str) % OPR::PRINT % OPR::str0_;
			if(ret.match_ == 0b11) {
				str_t s = get_string_(ret);
				if(s.len == 0) {
					utils::format("String error: %s\n") % ret.str_[0].str;
					return false;
				}
				uint16_t tlen = 2 + 1 + 1 + 1 + s.len;
				if(!size_check_move_(ins, tlen)) return false;
				put_block_(idx, tlen - 3, ins);
				put_opr_(OPR::PRINT, ins + 3);
				buff_.set1(s.len, ins + 4);
				buff_.set(s.str, s.len, ins + 5);
				buff_.resize_front(buff_.get_front_size() + tlen);
				return true;
			}

			// input
			parse(ret, src.str) % OPR::INPUT % OPR::str0_;
			if(ret.match_ == 0b11) {

				return true;
			}

#if 0
			// if a = V then CMD
			parse(ret, src.str) % OPR::IF % OPR::str0_ % OPR::cmp_ % OPR::str1_ % OPR::THEN % OPR::str2_;
			if(ret.match_ == 0b111111) {
				utils::format("IF\n");
				return true;
			}

			// for $ = n to m step l 
			parse(ret, src.str)
				% OPR::FOR % OPR::str_ % OPR::EQ % OPR::dec_ % OPR::TO % OPR::dec_ % OPR::STEP % OPR::dec_;
			if(ret.match_ == 0b11) {
				utils::format("GOSUB: %d\n") % ret.dec_;
				return true;
			}

			// next
			parse(ret, src.str) % OPR::NEXT;
			if(ret.match_ == 0b1) {
				utils::format("NEXT\n");
				return true;
			}
#endif

			// 変数への値の代入
			str_t sym;
			auto eqp = get_symbol_(src.str, sym);
			if(eqp != nullptr) {
				str_t t;
				get_word_(eqp + 1, t);
				if(t.len > 0) {

					utils::format("Symbol: ");
					output_str_(sym, '\'');
					utils::format(" : '%s'\n") % t.str;
					return true;
				}
			}

			utils::format("Syntax error: %s\n") % src.str;
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	指定の行を消す
			@param[in]	idx	行番号
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool remove(uint16_t idx)
		{
			uint16_t pos;
			if(!find_index_(idx, pos)) {
				return false;
			}
			uint16_t len = buff_.get1(pos + 2);
			len += 3;
			buff_.move(pos + len, pos, buff_.get_front_size() - len);
			buff_.resize_front(buff_.get_front_size() - len);

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
			VAL idx;
			if(!get_dec_(index, false, idx)) {
				utils::format("Syntax error: %s\n") % index.str;
				return false;
			}

			if(idx == 0 || idx > 65535) {
				utils::format("Overflow: %s\n") % index.str;
				return false;
			}

			remove(idx);
			if(param.str == nullptr || param.len == 0) {
				return true;
			}

			if(!decode(idx, param)) {
				return false;
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
			utils::format("Code size:  %d bytes\n") % buff_.get_front_size();
			utils::format("Label size: %d bytes\n") % buff_.get_back_size();
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
			uint16_t pos = 0;
			while(pos < buff_.get_front_size()) {
				utils::format("%d ") % buff_.get2(pos);
				pos += 2;
				uint8_t bkn = buff_.get1(pos);
				++pos;
				list_sub_(pos);
				pos += bkn;
			}
			utils::format("\n");
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	OPTIMIZE（最適化）
			@param[in]	param	パラメーター
		*/
		//-----------------------------------------------------------------//
		bool cmd_optimize(const str_t& param)
		{
			uint16_t pos = 0;
			while(pos < buff_.get_front_size()) {
				auto idx = buff_.get2(pos);
				pos += 2;
				uint8_t bkn = buff_.get1(pos);
				++pos;

				pos += bkn;
			}
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
			cmd_optimize(param);

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
			utils::format("Ok\n\n");
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	SAVE
			@param[in]	param	パラメーター
		*/
		//-----------------------------------------------------------------//
		bool cmd_save(const str_t& param) const
		{
			if(save_func_ == nullptr) return false;
			if(param.len == 0) {
				utils::format("Illegal command\n");
				return false;
			}
			auto f = save_func_(param.str, buff_);
			if(f) utils::format("Ok\n\n");
			else utils::format("Save error: '%s'\n\n") % param.str;
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	LOAD
			@param[in]	param	パラメーター
		*/
		//-----------------------------------------------------------------//
		bool cmd_load(const str_t& param)
		{
			if(load_func_ == nullptr) return false;
			if(param.len == 0) {
				utils::format("Illegal command\n");
				return false;
			}
			auto f = load_func_(param.str, buff_);
			if(f) utils::format("Ok\n\n");
			else utils::format("Load error: '%s'\n\n") % param.str;
			return f;
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
			} else if(save_func_ != nullptr && std::strncmp("save", cmd.str, cmd.len) == 0) {
				cmd_save(param);
			} else if(load_func_ != nullptr && std::strncmp("load", cmd.str, cmd.len) == 0) {
				cmd_load(param);
			} else if(std::strncmp("optimize", cmd.str, cmd.len) == 0) {
				cmd_optimize(param);
			} else {
				utils::format("Syntax error: %s\n") % cmd.str;
			}
		}
	};

	// テンプレート内「static」の実体
	template <typename VAL, class BUFF>
	const char* basic<VAL, BUFF>::opr_key_[] = {
		"rem",
		"goto", "gosub", "stop", "return",
		"print", "input",
		"for", "to", "step", "next",
		"if", "then",
	};

#if 0
	"LET",
	",", ";",
	"-", "+", "*", "/", "(", ")",
	">=", "#", ">", "=", "<=", "<",
	 "@", "RND", "ABS", "SIZE",
	"LIST", "RUN", "NEW", "SYSTEM"
#endif

}
