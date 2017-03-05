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

		uint16_t	disasm_org_;
		uint16_t	dump_org_;

		uint16_t	adr_;
		uint16_t 	ncnt_;

		bool		enable_;

		void dump_() {
			if(ncnt_ > 0) { dump_org_ = adr_; adr_ = 0; ncnt_ = 0; }
			std::string list = dis_.dump(dump_org_);
			terminal_->output(list + "\n"); 
			++dump_org_;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		tools() : disasm_org_(0), dump_org_(0), adr_(0), ncnt_(0), enable_(false) { }


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
			if(cmd == "state") {
				if(enable_) {
					terminal_->output("ROM active\n");
				} else {
					terminal_->output("ROM not active\n"); 
				}
				return;
			}

			if(!enable_) return;

			adr_ = 0;
			ncnt_ = 0;
			bool dump = true;
			for(char ch : cmd) {
				if(ch == ' ') {
					dump_();
				} else if(ch >= '0' && ch <= '9') {
					adr_ <<= 4;
					adr_ += ch - '0';
					++ncnt_;
				} else if(ch >= 'A' && ch <= 'F') {
					adr_ <<= 4;
					adr_ += ch - 'A' + 10;
					++ncnt_;
				} else if(ch >= 'a' && ch <= 'f') {
					adr_ <<= 4;
					adr_ += ch - 'a' + 10;
					++ncnt_;
				} else {
					if(ch == 'l' || ch == 'L') {
						if(ncnt_ > 0) { disasm_org_ = adr_; adr_ = 0; ncnt_ = 0; }
						for(int i = 0; i < 16; ++i) {
							std::string list = dis_.disasm(disasm_org_);
							terminal_->output(list + "\n"); 
							disasm_org_ = dis_.get_pc();
						}
					} else {
						std::string s;
						s = "Command error: '";
						terminal_->output(s + ch + "'\n"); 
					}
					dump = false;
				}
			}
			if(dump) {
				dump_();
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
