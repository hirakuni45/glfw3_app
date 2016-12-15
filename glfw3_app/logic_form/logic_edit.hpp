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

		bool cur_ch_(const utils::strings& ss)
		{
			if(ss.size() != 2) return false;

			return true;
		}

		bool clock_(const utils::strings& ss)
		{
			return true;
		}

		bool fill_(const utils::strings& ss)
		{
			return true;
		}

		bool flip_(const utils::strings& ss)
		{
			return true;
		}

		bool copy_(const utils::strings& ss)
		{
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
///			if(cmd == "help" || cmd == "?") ret = help_(ss);
			if(cmd == "ch") ret = cur_ch_(ss);
			else if(cmd == "clock") ret = clock_(ss);
			else if(cmd == "fill") ret = fill_(ss);
			else if(cmd == "flip") ret = flip_(ss);
			else if(cmd == "copy") ret = copy_(ss);
			else {  // error
				output("Command error: " + ss[0] + "\n");
			}

			return ret;
		}
	};

}
