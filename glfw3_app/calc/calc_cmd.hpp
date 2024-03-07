#pragma once
//=====================================================================//
/*! @file
    @brief  四則演算コマンドライン
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020, 2024 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>
#include <cstring>
#include <string>
#include <functional>

#include <boost/unordered_map.hpp>

#include "utils/format.hpp"
#include "utils/input.hpp"

#include "basic_arith.hpp"
#include "mpfr.hpp"
#include "calc_func.hpp"
#include "calc_symbol.hpp"

#include "fixed_string.hpp"

namespace app {

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  calc cmd クラス
    */
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    class calc_cmd {

		static constexpr uint32_t ANS_NUM = 20;  			///< 表示桁
		// 内部演算を大きくしないと、最下位の表示が曖昧になる・・
		static constexpr uint32_t CALC_NUM = ANS_NUM * 4;	///< 内部演算

		// 数値クラス
		typedef mpfr::value<CALC_NUM> NVAL;

		typedef boost::unordered_map<utils::STR8, uint8_t> MAP;
		MAP		map_;

		typedef utils::calc_symbol<NVAL, MAP> SYMBOL;
		SYMBOL	symbol_;

		typedef utils::calc_func<NVAL, MAP> FUNC;
		FUNC	func_;

		typedef utils::basic_arith<NVAL, SYMBOL, FUNC> ARITH;
		ARITH	arith_;

		bool	disp_separate_;

		void zerosup_(char* org, char ech) noexcept
		{
			// 指数表示の場合スルー
			if(strrchr(org, ech) != nullptr) return;

			auto l = strlen(org);
			while(l > 0) {
				--l;
				if(org[l] != '0') break;
				else {
					org[l] = 0;
				}
			}
			if(l > 0) {
				if(org[l] == '.') {
					org[l] = 0;
				}
			}
		}

		void sep_(const char* src, int len, int mod, int spn, std::function<void (const char*)> out) noexcept
		{
			char sub[8];

			if(mod == 0) mod = spn;
			while(len > 0) {
				strncpy(sub, src, mod);
				if(len > spn) {
					sub[mod] = '\'';
					sub[mod + 1] = 0;
				} else {
					sub[mod] = 0;
				}
				out(sub);
				len -= mod;
				src += mod;
				mod = spn;
			}
		}

		void disp_(NVAL& ans, std::function<void (const char*)> out) noexcept
		{
			char cnv = 'f';
			char ech = 'e';
			uint32_t spn = 3;
			if(func_.get_vtype() == FUNC::VTYPE::Hex) {
				cnv = 'A';
				ech = 'P';
				spn = 4;
			} else if(func_.get_vtype() == FUNC::VTYPE::Bin) {
				cnv = 'b';
				ech = 'p';
				spn = 4;
			}
			char tmp[ANS_NUM * 4 + 8];  // 2 進表示用に 4 倍を確保 + α
			ans(ANS_NUM, tmp, sizeof(tmp), cnv);
			zerosup_(tmp, ech);

			if(disp_separate_) {
				const char* s = tmp;
				if(*s == '-') {
					out("-");
					++s;
				} else if(*s == '+') {
					out("+");
					++s;
				}
				if(cnv == 'A') {
					if(s[0] == '0' && s[1] == 'X') {
						s += 2;
						out("0x");
					}
				} else if(cnv == 'b') {
					out("0b");
				}

				const char* p = strchr(s, '.');
				int l;
				if(p != nullptr) {
					l = p - s;
				} else {
					l = strlen(s);
				}
				auto m = l % spn;
				sep_(s, l, m, spn, out);
				if(p != nullptr) {
					out(".");
					++p;
					int l = strlen(p);
					sep_(p, l, 0, spn, out);
				}
			} else {
				out(tmp);
			}
			out("\n");
		}

		void list_sym_(std::function<void (const char*)> out) noexcept
		{
			for(auto id = SYMBOL::NAME::V0; id < SYMBOL::NAME::V9; SYMBOL::next(id)) {
				out(symbol_.get_name(id).c_str());
				out(": ");
				NVAL val;
				symbol_(id, val);
				disp_(val, out);
			}
		}

		bool def_func_(const char* org, uint32_t n)
		{
			return true;
		}

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクタ
		*/
		//-------------------------------------------------------------//
		calc_cmd() noexcept :
			map_(), symbol_(map_), func_(map_), arith_(symbol_, func_),
			disp_separate_(true)
		{ }


		//-------------------------------------------------------------//
		/*!
			@brief  サービス
			@param[in] cmd	コマンドライン文字列
			@param[in] out	出力関数
		*/
		//-------------------------------------------------------------//
		void service(const char* cmd, std::function<void (const char*)> out) noexcept
		{
			if(strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
				out("  PI        constant\n");
				out("  LOG2      constant\n");
				out("  EULER     constant\n");
				out("  ANS       constant\n");
				out("  V[0-9]    Memory symbol 0..9\n");
				out("  Min[0-9]  Memory In 0..9\n");
				out("  ListSym   List symbol\n");
				out("  Rad       0 to 2*PI\n");
				out("  Grad      0 to 400\n");
				out("  Deg       0 to 360\n");
				out("  Dec       Decimal mode\n");
				out("  Hex       Hexadecimal mode\n");
				out("  Bin       Binary mode\n");
				out("  Sep       Separate mode\n");

				for(auto id = FUNC::NAME::org; id != FUNC::NAME::last; FUNC::next(id)) {
					out("  ");
					out(func_.get_name(static_cast<FUNC::NAME>(id)).c_str());
					out("(x)\n");
				}
				return;
			} else if(strncmp(cmd, "Min", 3) == 0) {
				if(cmd[3] >= '0' && cmd[3] <= '9') {
					NVAL val;
					symbol_(SYMBOL::NAME::ANS, val);
					auto vi = SYMBOL::NAME::V0;
					vi = static_cast<SYMBOL::NAME>(static_cast<uint8_t>(vi) + (cmd[3] - '0'));
					symbol_.set_value(vi, val);
				} else {
					out("Min number fail.\n");
				}
				return;
			} else if(strcmp(cmd, "ListSym") == 0) {
				list_sym_(out);
				return;
			} else if(strcmp(cmd, "Rad") == 0) {
				func_.set_atype(FUNC::ATYPE::Rad);
				return;
			} else if(strcmp(cmd, "Grad") == 0) {
				func_.set_atype(FUNC::ATYPE::Grad);
				return;
			} else if(strcmp(cmd, "Deg") == 0) {
				func_.set_atype(FUNC::ATYPE::Deg);
				return;
			} else if(strcmp(cmd, "Dec") == 0) {
				func_.set_vtype(FUNC::VTYPE::Dec);
				return;
			} else if(strcmp(cmd, "Hex") == 0) {
				func_.set_vtype(FUNC::VTYPE::Hex);
				return;
			} else if(strcmp(cmd, "Bin") == 0) {
				func_.set_vtype(FUNC::VTYPE::Bin);
				return;
			} else if(strcmp(cmd, "Sep") == 0) {
				disp_separate_ = !disp_separate_;
				out(disp_separate_ ? "Separate: ON" : "Separate: OFF");
				out("\n");
				return;
			}

			const auto p = strchr(cmd, ':');
			if(p != nullptr) {  // 関数定義
				def_func_(cmd, p - cmd);
			} else if(arith_.analize(cmd)) {
				auto ans = arith_();
				symbol_.set_value(SYMBOL::NAME::ANS, ans);
				disp_(ans, out);
			} else {
				char tmp[ANS_NUM + 16];
				utils::sformat("Fail: %s\n", tmp, sizeof(tmp)) % cmd;
				out(tmp);
			}
		}
	};
}
