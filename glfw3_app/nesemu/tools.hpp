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
#include "emu/cpu/nes6502.hpp"

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
		uint16_t	write_org_;

		uint16_t	hex_;
		uint16_t 	ncnt_;

		bool		enable_;

		void dump_() {
			if(ncnt_ > 0) { dump_org_ = hex_; hex_ = 0; ncnt_ = 0; }
			std::string list = dis_.dump(dump_org_);
			terminal_->output(list + "\n"); 
			++dump_org_;
		}

		void write_() {
			if(ncnt_ > 0) {
				dis_.write_byte(write_org_, hex_);
				++write_org_;
				ncnt_ = 0;
				hex_ = 0;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		tools() : disasm_org_(0), dump_org_(0), write_org_(0),
				  hex_(0), ncnt_(0), enable_(false) { }


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
			dis_.at_putbyte() = nes6502_putbyte;
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

			std::string tmp = cmd;
			tmp += ';';

			hex_ = 0;
			ncnt_ = 0;
			bool dump = true;
			bool write = false;
			for(char ch : tmp) {
				if(ch == ' ' || ch == ';') {
					if(write) {
						write_();
					} else {
						dump_();
					}
				} else if(ch == ':') {
					write_org_ = hex_;
					hex_ = 0;
					ncnt_ = 0;
					write = true;
				} else if(ch >= '0' && ch <= '9') {
					hex_ <<= 4;
					hex_ += ch - '0';
					++ncnt_;
				} else if(ch >= 'A' && ch <= 'F') {
					hex_ <<= 4;
					hex_ += ch - 'A' + 10;
					++ncnt_;
				} else if(ch >= 'a' && ch <= 'f') {
					hex_ <<= 4;
					hex_ += ch - 'a' + 10;
					++ncnt_;
				} else {
					write = false;
					if(ch == 'l' || ch == 'L') {
						if(ncnt_ > 0) { disasm_org_ = hex_; hex_ = 0; ncnt_ = 0; }
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
