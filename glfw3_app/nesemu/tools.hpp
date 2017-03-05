#pragma once
//=====================================================================//
/*! @file
	@brief  Emulator Tools クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_terminal.hpp"

#include "emu/cpu/nes6502.h"

#include "emu/cpu/dis6502.hpp"

namespace emu {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	エミュレーター Tools クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class tools {

		static gui::widget_terminal*	terminal_;

		cpu::dis6502	dis_;

		uint32_t	address_;
		bool		enable_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		tools() : address_(0), enable_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  ターミナル・コンテキストを設定
			@param[in]	term	ターミナル・コンテキスト
		*/
		//-----------------------------------------------------------------//
		static void set_terminal(gui::widget_terminal* t) {
			terminal_ = t;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  出力
		*/
		//-----------------------------------------------------------------//
		static void put(const char* text) {
			if(terminal_ == nullptr) return;
			terminal_->output(text);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void init()
		{
			dis_.at_getbyte() = nes6502_getbyte;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  許可
			@param[in]	f	不許可なら「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { enable_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief  コマンド
			@param[in]	cmd	コマンド
		*/
		//-----------------------------------------------------------------//
		void command(const std::string& cmd)
		{
			if(cmd.empty()) return;

			if(cmd == "state") {
				if(enable_) {
					terminal_->output("ROM active\n");
				} else {
					terminal_->output("ROM not active\n"); 
				}
				return;
			}

//			std::cout << cmd << std::endl;
			bool err = false;
			for(char ch : cmd) {
				if(ch >= '0' && ch <= '9') {
					address_ <<= 4;
					address_ += ch - '0';
				} else if(ch >= 'A' && ch <= 'F') {
					address_ <<= 4;
					address_ += ch - 'A' + 10;
				} else if(ch >= 'a' && ch <= 'f') {
					address_ <<= 4;
					address_ += ch - 'a' + 10;
				} else {
					if(ch == 'l' || ch == 'L') {
						for(int i = 0; i < 16; ++i) {
							std::string list = dis_.disasm(address_ & 0xffff);
							terminal_->output(list + "\n"); 
							address_ = dis_.get_pc();
						}
					} else {
						err = true;
					}
				}
			}

			if(err) {
				terminal_->output("Command error: '" + cmd + "'\n"); 
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ツール・サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
		}
	};
}
