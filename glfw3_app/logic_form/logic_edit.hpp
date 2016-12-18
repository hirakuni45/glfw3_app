#pragma once
//=====================================================================//
/*! @file
	@brief  Logic Edit クラス @n
			コマンド形式の波形編集
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "logic.hpp"
#include "utils/string_utils.hpp"
#include <functional>

namespace tools {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ロジック編集クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class logic_edit {

		typedef std::function< void(const std::string& s) > output_func_type;

		logic&	logic_;

		output_func_type	output_;

		uint32_t	ch_;  // カレント・チャネル
		std::vector<uint32_t>	bus_;	// バス
		bool		bus_enable_;

		static const uint32_t ch_limit_ = 24;		///< チャネルの最大値
		static const uint32_t pos_limit_ = 2046;	///< 波形位置の最大値

		int32_t get_dec_(uint32_t idx, const utils::strings& ss)
		{
			int32_t val = -1;
			if(idx < ss.size() && ss.size() >= (idx + 1)) {
				val = std::stoi(ss[idx]);
				if(val >= 0 && val < pos_limit_) {
				} else {
					val = -1;
				}				
			}
			return val;
		}


		// カレント・チャネル
		bool cur_ch_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Current Chanel: [chanel-no]\n");
				return true;
			}

			if(ss.size() == 1) {
				output(std::to_string(ch_) + "\n");
				bus_enable_ = false;
				return true;
			} else if(ss.size() == 2) {
				auto n = std::stoi(ss[1]);
				if(n >= 0 && n < ch_limit_) {
					ch_ = n;
					bus_enable_ = false;
					return true;
				}
			}

			return false;
		}


		// バス定義
		bool cur_bus_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Current Bus Config: [chanel-no] ... (LSB ... MSB)\n");
				return true;
			}

			if(ss.size() == 1) {
				if(bus_.empty()) return true;

				for(auto ch : bus_) {
					output(std::to_string(ch) + " ");
				}
				output("\n");
				bus_enable_ = true;
				return true;
			} else {
				bus_.clear();
				for(uint32_t i = 1; i < ss.size(); ++i) {
					auto ch = std::stoi(ss[i]);
					if(ch >= 0 && ch < ch_limit_) {
						bus_.push_back(ch);
					} else {
						return false;
					}
				}
				bus_enable_ = true;
				return true;
			}

			return false;
		}


		// 値設定
		bool set_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Set value: position [level = 1]\n");
				return true;
			}

			auto pos = get_dec_(1, ss);
			if(pos < 0) return false;

			int lvl = 1;
			int limit = 1;
			if(bus_enable_) {
				lvl = 0;
				limit = (1 << bus_.size()) - 1;
			}
			if(ss.size() >= 3) {
				lvl = std::stoi(ss[2]);
				if(lvl >= 0 && lvl <= limit) {
				} else {
					return false;
				}
			}

			if(bus_enable_) {
				uint32_t idx = 0;
				for(auto ch : bus_) {
					logic_.set_logic(ch, pos, (lvl >> idx) & 1);
					++idx;
				}
			} else {
				logic_.set_logic(ch_, pos, lvl & 1);
			}
			return true;
		}


		// クロック生成
		bool clock_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Build Clock: start-position length [low-count=1] [high-count=1]\n");
				return true;
			}

			auto pos = get_dec_(1, ss);
			if(pos < 0) return false;

			auto len = get_dec_(2, ss);
			if(len < 0) return false;

			int32_t lc = 1;
			if(ss.size() >= 4) {
				lc = std::stoi(ss[3]);
				if(lc >= 0) {
				} else {
					return false;
				}
			}

			int32_t hc = 1;
			if(ss.size() >= 5) {
				hc = std::stoi(ss[4]);
				if(hc >= 0) {
				} else {
					return false;
				}
			}

			logic_.build_clock(ch_, pos, len, lc, hc);

			return true;
		}


		// フィル
		bool fill_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Fill logic level: start-position length [level=1]\n");
				return true;
			}

			auto pos = get_dec_(1, ss);
			if(pos < 0) return false;

			auto len = get_dec_(2, ss);
			if(len < 0) return false;

			int32_t lvl = 1;
			if(ss.size() >= 4) {
				lvl = std::stoi(ss[3]);
				if(lvl >= 0 && lvl <= 1) {
				} else {
					return false;
				}
			}

			logic_.fill(ch_, pos, len, lvl);

			return true;
		}

		// 反転
		bool flip_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Flip logic level: start-position length\n");
				return true;
			}

			auto pos = get_dec_(1, ss);
			if(pos < 0) return false;

			auto len = get_dec_(2, ss);
			if(len < 0) return false;

			logic_.flip(ch_, pos, len);

			return true;
		}


		// コピー
		bool copy_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Copy logic level: src-position length dst-position\n");
				return true;
			}

			auto src = get_dec_(1, ss);
			if(src < 0) return false;

			auto len = get_dec_(2, ss);
			if(len < 0) return false;

			auto dst = get_dec_(3, ss);
			if(dst < 0) return false;



			return true;
		}


		// コピー・チャネル
		bool copy_chanel_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Copy chanel: dst-chanel [src-position] [src-length]\n");
				return true;
			}

			auto dst_ch = get_dec_(1, ss);
			if(dst_ch < 0) return false;

			auto src_pos = get_dec_(2, ss);
			if(src_pos < 0) return false;

			auto src_len = get_dec_(3, ss);
			if(src_len < 0) return false;

			logic_.copy_chanel(ch_, dst_ch, src_pos, src_len);

			return true;
		}

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		logic_edit(logic& lg) : logic_(lg), ch_(0), bus_(), bus_enable_(false) { }


		//-------------------------------------------------------------//
		/*!
			@brief  出力関数設定
			@param[in]	func	出力関数
		*/
		//-------------------------------------------------------------//
		void set_output(output_func_type func) { output_ = func; }


		//-------------------------------------------------------------//
		/*!
			@brief  文字列を出力
			@param[in]	s	文字列
		*/
		//-------------------------------------------------------------//
		void output(const std::string& s) { output_(s); }


		//-------------------------------------------------------------//
		/*!
			@brief  コマンド実行
			@param[in]	line	命令
			@return エラー無ければ「true」
		*/
		//-------------------------------------------------------------//
		bool command(const std::string& line)
		{
			if(line.empty()) return true;

			auto ss = utils::split_text(line, " ");
			if(ss.empty()) return true;

			auto cmd = ss[0];

			bool ret = false;
			if(cmd == "help") {
				ret = true;;
				if(ss.size() == 1) {
					ss[0] = "help";
					cur_ch_(ss);
					cur_bus_(ss);
					set_(ss);
					clock_(ss);
					fill_(ss);
					flip_(ss);
					copy_(ss);
					copy_chanel_(ss);
				} else if(ss.size() == 2) {
					cmd = ss[1];
				}
			}

			if(cmd == "ch") ret = cur_ch_(ss);
			else if(cmd == "bus") ret = cur_bus_(ss);
			else if(cmd == "set") ret = set_(ss);
			else if(cmd == "clock") ret = clock_(ss);
			else if(cmd == "fill") ret = fill_(ss);
			else if(cmd == "flip") ret = flip_(ss);
			else if(cmd == "copy") ret = copy_(ss);
			else if(cmd == "copy_chanel") ret = copy_chanel_(ss);

			if(!ret) {  // error
				output("Command error: " + line + "\n");
			}

			return ret;
		}

	};

}
