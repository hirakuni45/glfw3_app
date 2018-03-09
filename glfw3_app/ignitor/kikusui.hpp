#pragma once
//=====================================================================//
/*! @file
    @brief  菊水電源インターフェース・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <boost/format.hpp>
#include "utils/serial_win32.hpp"
#include "utils/fixed_fifo.hpp"
#include "utils/input.hpp"
#include "utils/string_utils.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  菊水電源インターフェース・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class kikusui {

		device::serial_win32&	serial_;

		typedef utils::fixed_fifo<char, 1024> FIFO;
		FIFO		fifo_;

		enum class task {
			idle,
			state,		///< 菊水の電源が接続されているか？

			refc,		///< 下駄を履かせた分の基準電流の測定
			refc_wait,
			refc_sync,

			volt,
			curr,
		};
		task		task_;

		float		volt_;
		float		curr_;

		uint32_t	volt_id_;
		uint32_t	curr_id_;

		uint32_t	loop_;
		bool		term_;
		bool		timeout_;
		uint32_t	refc_loop_;
		uint32_t	refc_id_;
		bool		refc_;
		float		ref_curr_;

		std::string	idn_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		kikusui(device::serial_win32& serial) : serial_(serial),
			fifo_(), task_(task::idle), volt_(0.0f), curr_(0.0f),
			volt_id_(0), curr_id_(0), loop_(0), term_(false), timeout_(false),
			refc_loop_(0), refc_id_(0), refc_(false), ref_curr_(0.0f), idn_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start()
		{
			std::string s = "INST:NSEL 6;*IDN?\r\n";
			serial_.write(s.c_str(), s.size());
			task_ = task::state;
			loop_ = 60;  // time out 1 sec
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  IDN の取得
			@return IDN
		*/
		//-----------------------------------------------------------------//
		const std::string& get_idn() const { return idn_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  停止
		*/
		//-----------------------------------------------------------------//
		void stop()
		{
			std::string s = "CURR 0 A\r\n";
			serial_.write(s.c_str(), s.size());
			s = "VOLT 0 V\r\n";
			serial_.write(s.c_str(), s.size());
			s = "OUTP 0\r\n";
			serial_.write(s.c_str(), s.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  出力設定
			@param[in]	ena		許可の場合「true」
		*/
		//-----------------------------------------------------------------//
		void set_output(bool ena)
		{
			std::string s;
			if(ena) {
				s = "OUTP 1\r\n";
			} else {
				s = "OUTP 0\r\n";
			}
			serial_.write(s.c_str(), s.size());
// std::cout << s << std::flush;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧設定
			@param[in]	volt	設定電圧
			@param[in]	curr	最大電流
		*/
		//-----------------------------------------------------------------//
		void set_volt(float volt, float curr)
		{
			auto s = (boost::format("CURR %5.4f A;VOLT %5.4f V\r\n") % curr % volt).str();
			serial_.write(s.c_str(), s.size());
// std::cout << "VOLT: " << s << std::flush;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電流設定
			@param[in]	curr	設定電流
			@param[in]	volt	最大電圧
		*/
		//-----------------------------------------------------------------//
		void set_curr(float curr, float volt)
		{
			auto s = (boost::format("VOLT %5.4f V;CURR %5.4f A\r\n") % volt % curr).str();
			serial_.write(s.c_str(), s.size());
// std::cout << "CURR: " << s << std::flush;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧測定要求
		*/
		//-----------------------------------------------------------------//
		void req_volt()
		{
			std::string s = "MEAS:VOLT:DC?\r\n";
			serial_.write(s.c_str(), s.size());
			task_ = task::volt;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電流測定要求
		*/
		//-----------------------------------------------------------------//
		void req_curr()
		{
			std::string s = "MEAS:CURR:DC?\r\n";
			serial_.write(s.c_str(), s.size());
			timeout_ = false;
			task_ = task::curr;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電流 ID を取得
			@return 電流 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_curr_id() const { return curr_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧 ID を取得
			@return 電圧 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_volt_id() const { return volt_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧を取得
			@return 電圧
		*/
		//-----------------------------------------------------------------//
		float get_volt() const { return volt_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  基準電流を取得
			@return 基準電流
		*/
		//-----------------------------------------------------------------//
		float get_ref_curr() const { return ref_curr_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  電流を取得
			@return 電流
		*/
		//-----------------------------------------------------------------//
		float get_curr() const { return curr_ - ref_curr_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			char tmp[256];
			auto rl = serial_.read(tmp, sizeof(tmp));
			for(uint32_t i = 0; i < rl; ++i) {
				char ch = tmp[i];
				fifo_.put(ch);
				if(ch == '\n') {
					term_ = true;
					break;
				}
			}

			switch(task_) {
			case task::state:
				while(fifo_.length() > 0) {
					auto ch = fifo_.get();
					if(ch == '\r') {
						ch = 0;
					} else if(ch == '\n') {
//						task_ = task::refc;
						task_ = task::idle;
						break;
					}
					idn_ += ch;
				}
				if(loop_ > 0) {
					--loop_;
				} else {
					timeout_ = true;
					task_ = task::idle;
				}
				break;

			case task::refc:
				set_output(1);
				refc_loop_ = 90;
				task_ = task::refc_wait;
				break;
			case task::refc_wait:
				if(refc_loop_ > 0) {
					refc_loop_--;
				} else {
					std::string s = "MEAS:CURR:DC?\r\n";
					serial_.write(s.c_str(), s.size());					
					task_ = task::refc_sync;
				}
				break;
			case task::refc_sync:
#if 0
				if(fifo_.length() > 0) {
					std::string s;
					while(fifo_.length() > 0) {
						auto ch = fifo_.get();
						if(ch == '\r') {
							ch = 0;
						} else if(ch == '\n') {
							break;
						}
						s += ch;
					}
					if((utils::input("%f", s.c_str()) % curr_).status()) {
						++curr_id_;
					}
					set_output(0);
					task_ = task::idle;
				}
#endif
				break;

			case task::volt:
				if(fifo_.length() > 0 && term_) {
					std::string s;
					while(fifo_.length() > 0) {
						auto ch = fifo_.get();
						if(ch == '\r') {
							ch = 0;
						} else if(ch == '\n') {
							break;
						}
						s += ch;
					}
					if((utils::input("%f", s.c_str()) % volt_).status()) {
						++volt_id_;
					}
					term_ = false;
					task_ = task::idle;
				}
				break;

			case task::curr:
				if(fifo_.length() > 0 && term_) {
					std::string s;
					while(fifo_.length() > 0) {
						auto ch = fifo_.get();
						if(ch == '\r') {
							ch = 0;
						} else if(ch == '\n') {
							break;
						}
						s += ch;
					}
					if((utils::input("%f", s.c_str()) % curr_).status()) {
						++curr_id_;
					}
					term_ = true;
					task_ = task::idle;
				}
				break;

			case task::idle:
			default:
				while(fifo_.length() > 0) {
					fifo_.get();
				}
				break;
			}
		}
	};
}
