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

		// 10進、又は１６進数の変換、「-1」はエラー
		int32_t get_value_(const std::string& s)
		{
			if(s.find("0x") == 0 || s.find("0X") == 0) {
				try {
					return std::stoi(s, nullptr, 16);
				} catch(const std::invalid_argument& er) {
					return -1;
				}
			}
			try {
				return std::stoi(s);
			} catch(const std::invalid_argument& er) {
				return -1;
			}
		}


		int32_t get_pos_(uint32_t idx, const utils::strings& ss)
		{
			int32_t val = -1;
			if(idx < ss.size()) {
				val = get_value_(ss[idx]);
				if(val >= 0 && val < pos_limit_) {
				} else {
					val = -1;
				}				
			}
			return val;
		}


		int32_t get_len_(uint32_t idx, const utils::strings& ss)
		{
			int32_t val = -1;
			if(idx < ss.size()) {
				val = get_value_(ss[idx]);
				if(val >= 0 && val < pos_limit_) {
				} else {
					val = -1;
				}				
			}
			return val;
		}


		// ステータス
		bool status_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("status\n");
				return true;
			}

			output("Size: " + std::to_string(logic_.size()) + "\n");

			for(uint32_t i = 0; i < 24; ++i) {
				auto n = logic_.count1(i);
				if(n == 0) continue;
				output("CH" + std::to_string(i) + ": "
					+ std::to_string(n) + "/" + std::to_string(logic_.size()) + "\n");
			}

			return true;
		}


		// カレント・チャネル
		bool cur_ch_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("ch [Chanel-No]\n");
				return true;
			}

			if(ss.size() == 1) {
				output(std::to_string(ch_) + "\n");
				bus_enable_ = false;
				return true;
			} else if(ss.size() == 2) {
				auto n = get_value_(ss[1]);
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
				output("bus [Chanel-No] ... (LSB ... MSB)\n");
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
					auto ch = get_value_(ss[i]);
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


		// クリア
		bool clear_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("clear\n");
				return true;
			}

			logic_.clear();

			return true;
		}


		// クリエイト
		bool create_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("create Size\n");
				return true;
			}

			auto size = get_pos_(1, ss);
			if(size < 0) return false;

			logic_.create(size);

			return true;
		}


		void set_value_(uint32_t pos, uint32_t value)
		{
			if(bus_enable_) {
				uint32_t idx = 0;
				for(auto ch : bus_) {
					logic_.set_logic(ch, pos, (value >> idx) & 1);
					++idx;
				}
			} else {
				logic_.set_logic(ch_, pos, value);
			}
		}


		// 値設定
		bool set_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("set [Value] ...\n");
				return true;
			}

			auto pos = get_pos_(1, ss);
			if(pos < 0 || pos >= logic_.size()) return false;

			int lvl = 1;
			int limit = 1;
			if(bus_enable_) {
				lvl = 0;
				limit = (1 << bus_.size()) - 1;
			}

			if(ss.size() <= 2) {
				set_value_(pos, lvl);
				return true;
			}

			for(uint32_t i = 2; i < ss.size(); ++i) {
				auto s = ss[i];
				if(s.front() == '"' && s.back() == '"') {
					if(bus_enable_) {
						return false;
					}
					for(auto ch : s) {
						if(ch == '0') {
							logic_.set_logic(ch_, pos, 0);
							++pos;
						} else if(ch == '1') {
							logic_.set_logic(ch_, pos, 1);
							++pos;
						} else if(ch == '"') {
						} else {
							return false;
						}
					}
				} else {
					int32_t lvl = get_value_(s);
					if(lvl >= 0 && lvl <= limit) {
					} else {
						return false;
					}
					set_value_(pos, lvl);
					++pos;
				}
			}
			return true;
		}


		// クロック生成
		bool clock_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("clock Start-Position Length [Low-Count=1] [High-Count=1]\n");
				return true;
			}

			auto pos = get_pos_(1, ss);
			if(pos < 0) return false;

			auto len = get_len_(2, ss);
			if(len < 0) return false;

			int32_t lc = 1;
			if(ss.size() >= 4) {
				lc = get_value_(ss[3]);
				if(lc >= 0) {
				} else {
					return false;
				}
			}

			int32_t hc = 1;
			if(ss.size() >= 5) {
				hc = get_value_(ss[4]);
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
				output("fill Start-Position Length [Value=1]\n");
				return true;
			}

			auto pos = get_pos_(1, ss);
			if(pos < 0) return false;

			auto len = get_len_(2, ss);
			if(len < 0) return false;

			int lvl = 1;
			int limit = 1;
			if(bus_enable_) {
				lvl = limit = (1 << bus_.size()) - 1;
			}
			if(ss.size() >= 4) {
				lvl = get_value_(ss[3]);
				if(lvl >= 0 && lvl <= limit) {
				} else {
					return false;
				}
			}

			if(bus_enable_) {
				uint32_t idx = 0;
				for(auto ch : bus_) {
					logic_.fill(ch, pos, (lvl >> idx) & 1);
					++idx;
				}
			} else {
				logic_.fill(ch_, pos, len, lvl & 1);
			}

			return true;
		}

		// 反転
		bool flip_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("flip Start-Position Length\n");
				return true;
			}

			auto pos = get_pos_(1, ss);
			if(pos < 0) return false;

			auto len = get_len_(2, ss);
			if(len < 0) return false;

			if(bus_enable_) {
				for(auto ch : bus_) {
					logic_.flip(ch, pos, len);
				}
			} else {
				logic_.flip(ch_, pos, len);
			}

			return true;
		}


		// コピー
		bool copy_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("copy Src-Position Copy-Length Dst-Position\n");
				return true;
			}

			auto src = get_pos_(1, ss);
			if(src < 0 || src >= logic_.size()) return false;

			auto len = get_len_(2, ss);
			if(len < 0) return false;

			auto dst = get_pos_(3, ss);
			if(dst < 0 && dst >= logic_.size()) return false;

			if(bus_enable_) {
				uint32_t idx = 0;
				for(auto ch : bus_) {
					logic_.copy(ch, src, len, dst);
					++idx;
				}
			} else {
				logic_.copy(ch_, src, len, dst);
			}

			return true;
		}


		// コピー・チャネル
		bool copy_chanel_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("copy-chanel Dst-Chanel [Src-Position] [Src-Length]\n");
				return true;
			}

			if(bus_enable_) return false;  // バスモードの場合は、エラー

			auto dst_ch = get_pos_(1, ss);
			if(dst_ch < 0 || dst_ch >= 24) return false;

			int32_t src_pos = 0;
			int32_t src_len = logic_.size();
			if(ss.size() >= 4) {
				src_pos = get_pos_(2, ss);
				if(src_pos < 0) return false;

				src_len = get_len_(3, ss);
				if(src_len < 0) return false;
			}

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
				ss[0] = "help";
				if(ss.size() == 1) {
					status_(ss);
					cur_ch_(ss);
					cur_bus_(ss);
					clear_(ss);
					create_(ss);
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

			if(cmd == "status") ret = status_(ss);
			else if(cmd == "ch") ret = cur_ch_(ss);
			else if(cmd == "bus") ret = cur_bus_(ss);
			else if(cmd == "clear") ret = clear_(ss);
			else if(cmd == "create") ret = create_(ss);
			else if(cmd == "set") ret = set_(ss);
			else if(cmd == "clock") ret = clock_(ss);
			else if(cmd == "fill") ret = fill_(ss);
			else if(cmd == "flip") ret = flip_(ss);
			else if(cmd == "copy") ret = copy_(ss);
			else if(cmd == "copy-chanel") ret = copy_chanel_(ss);

			if(!ret) {  // error
				output("Command error: " + line + "\n");
			}

			return ret;
		}


		//-------------------------------------------------------------//
		/*!
			@brief  ファイル実行
			@param[in]	name	ファイル名
			@return エラー無ければ「true」
		*/
		//-------------------------------------------------------------//
		bool injection(const std::string& name)
		{
			utils::file_io fio;
			if(!fio.open(name, "rb")) {
				output("Can't open script file: " + name); 
				return false;
			}

			uint32_t lno = 1;
			while(!fio.eof()) {
				auto s = fio.get_line();
				if(s.empty()) continue;
				if(s.front() == '#') continue;
				if(!command(s)) {
					return false;
				}
				++lno;
			}

			return true;
		}

	};

}
