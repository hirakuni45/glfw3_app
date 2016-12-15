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

		static const uint32_t ch_limit_ = 24;		///< チャネルの最大値
		static const uint32_t pos_limit_ = 2046;	///< 波形位置の最大値


		// カレント・チャネル
		bool cur_ch_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Current Chanel: [chanel-no]\n");
				return true;
			}

			if(ss.size() == 1) {
				output(std::to_string(ch_) + "\n");
				return true;
			} else if(ss.size() == 2) {
				auto n = std::stoi(ss[1]);
				if(n >= 0 && n < ch_limit_) {
					ch_ = n;
					return true;
				}
			}

			return false;
		}


		// レベル設定
		bool set_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Set logic level: position [level = 1]\n");
				return true;
			}

			uint32_t pos = 0;
			if(ss.size() >= 2) {
				pos = std::stoi(ss[1]);
				if(pos < pos_limit_) {
				} else {
					return false;
				}
			}

			int lvl = 1;
			if(ss.size() >= 3) {
				lvl = std::stoi(ss[2]);
				if(lvl >= 0 && lvl <= 1) {
				} else {
					return false;
				}
			}

			logic_.set_logic(ch_, pos, lvl);

			return true;
		}


		// クロック生成
		bool clock_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Clock Build: start-position length [low-count=1] [high-count=1]\n");
				return true;
			}

			if(ss.size() > 1) return false;



			return true;
		}


		// フィル
		bool fill_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				output("Fill logic: start-position length [level=1]\n");
				return true;
			}

			return true;
		}

		// 反転
		bool flip_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				return true;
			}

			return true;
		}

		// コピー
		bool copy_(const utils::strings& ss)
		{
			if(ss[0] == "help") {
				return true;
			}

			return true;
		}

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		logic_edit(logic& lg) : logic_(lg), ch_(0) { }


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
				if(ss.size() == 1) {
					ss[0] = "help";
					cur_ch_(ss);
					set_(ss);
					clock_(ss);
					fill_(ss);
					flip_(ss);
					copy_(ss);
				} else if(ss.size() == 2) {
					cmd = ss[1];
				}
			}

			if(cmd == "ch") ret = cur_ch_(ss);
			else if(cmd == "set") ret = set_(ss);
			else if(cmd == "clock") ret = clock_(ss);
			else if(cmd == "fill") ret = fill_(ss);
			else if(cmd == "flip") ret = flip_(ss);
			else if(cmd == "copy") ret = copy_(ss);

			if(!ret) {  // error
				output("Command error: " + line + "\n");
			}

			return ret;
		}
	};

}
