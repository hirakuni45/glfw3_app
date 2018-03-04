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
			volt,
			curr,
		};
		task		task_;

		float		volt_;
		float		curr_;

	   uint32_t		id_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		kikusui(device::serial_win32& serial) : serial_(serial),
			fifo_(), task_(task::idle), volt_(0.0f), curr_(0.0f), id_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start()
		{
			std::string s = "INST:NSEL 6\r\n";
			serial_.write(s.c_str(), s.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧設定
			@param[in]	volt	設定電圧
		*/
		//-----------------------------------------------------------------//
		void set_volt(float volt)
		{
			auto s = (boost::format("VOLT:LEV %5.4f\r\n") % volt).str();
			serial_.write(s.c_str(), s.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電流設定
			@param[in]	curr	設定電流
		*/
		//-----------------------------------------------------------------//
		void set_curr(float curr)
		{
			auto s = (boost::format("CURR:LEV %5.4f\r\n") % curr).str();
			serial_.write(s.c_str(), s.size());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧測定要求
		*/
		//-----------------------------------------------------------------//
		void req_volt()
		{
			std::string s = "MEAS:VOLT?\r\n";
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
			std::string s = "MEAS:CURR?\r\n";
			serial_.write(s.c_str(), s.size());
			task_ = task::curr;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  タスク ID を取得
			@return タスク ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_id() const { return id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧を取得
			@return 電圧
		*/
		//-----------------------------------------------------------------//
		float get_volt() const { return volt_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  電流を取得
			@return 電流
		*/
		//-----------------------------------------------------------------//
		float get_curr() const { return curr_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			char tmp[256];
			auto rl = serial_.read(tmp, sizeof(tmp));
			for(uint32_t i = 0; i < rl; ++i) {
				fifo_.put(tmp[i]);
			}
			switch(task_) {
			case task::volt:
				if(fifo_.length() >= 14) {
					std::string s;
					for(uint32_t i = 0; i < 14; ++i) {
						auto ch = fifo_.get();
						if(ch == '\r' || ch == '\n') ch = 0;
						s += ch;
					}
					utils::string_to_float(s, volt_);
					++id_;
					task_ = task::idle;
				}
				break;

			case task::curr:
				if(fifo_.length() >= 14) {
					std::string s;
					for(uint32_t i = 0; i < 14; ++i) {
						auto ch = fifo_.get();
						if(ch == '\r' || ch == '\n') ch = 0;
						s += ch;
					}
					utils::string_to_float(s, curr_);
					++id_;
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
