#pragma once
//=====================================================================//
/*!	@file
	@brief	ソフトウェア SPI I/O 制御
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "common/renesas.hpp"

/// F_ICLK はボーレートパラメーター計算で必要で、設定が無いとエラーにします。
#ifndef F_ICLK
#  error "spi_io.hpp requires F_ICLK to be defined"
#endif

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ソフト SPI 制御モード
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	enum class soft_spi_mode : uint8_t {
		CK10,	///< CLK 1 to 0
		CK01,	///< CLK 0 to 1
		LTC,	///< LTC SPI モード
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ソフト SPI 制御クラス
		@param[in]	MISO	Master In Slave Out
		@param[in]	MOSI	Master Out Slave In
		@param[in]	SPCK	Clock
		@param[in]	MODE	soft_spi_mode
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class MISO, class MOSI, class SPCK, soft_spi_mode MODE>
	class spi_io {

		uint8_t	delay_;

		void clock_()
		{
			if(MODE == soft_spi_mode::CK10) {
				uint8_t n = delay_;
				while(n > 3) { --n; asm("nop"); }
				SPCK::P = 1;
				n = delay_;
				while(n > 0) { --n; asm("nop"); }
				SPCK::P = 0;
			} else {
				uint8_t n = delay_;
				while(n > 3) { --n; asm("nop"); }
				SPCK::P = 0;
				n = delay_;
				while(n > 0) { --n; asm("nop"); }
				SPCK::P = 1;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		spi_io() : delay_(255) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  設定可能な最大速度を返す
			@return 速度
		*/
		//-----------------------------------------------------------------//
		uint32_t get_max_speed() const { return 120000000; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ＳＤカード用設定を有効にする
			@param[in]	speed	通信速度
			@return エラー（速度設定範囲外）なら「false」
		*/
		//-----------------------------------------------------------------//
		bool start_sdc(uint32_t speed)
		{
			MISO::DIR = 0;
			MISO::PU  = 1;
			MOSI::DIR = 1;
			SPCK::DIR = 1;

			uint32_t n = F_ICLK / speed;
			if(n > 511) n = 511;
			delay_ = n / 2;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
			@param[in]	speed	通信速度
			@return エラー（速度設定範囲外）なら「false」
		*/
		//-----------------------------------------------------------------//
		bool start(uint32_t speed)
		{
			MISO::PU  = 1;
			MISO::DIR = 0;
			MOSI::DIR = 1;
			SPCK::DIR = 1;

			if(MODE == soft_spi_mode::CK10) {
				SPCK::P = 0;
			} else {
				SPCK::P = 1;
			}

			uint32_t n = F_ICLK / speed;
			if(n > 511) n = 511;
			delay_ = n / 2;

			return true;
		}


		//----------------------------------------------------------------//
		/*!
			@brief	リード・ライト
			@param[in]	data	書き込みデータ
			@return 読み出しデータ
		*/
		//----------------------------------------------------------------//
		uint8_t xchg(uint8_t data = 0xff)
		{
			if(MODE == soft_spi_mode::LTC) {
				uint8_t r = 0;

				if(data & 0x80) MOSI::P = 1; else MOSI::P = 0;	// bit7
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x40) MOSI::P = 1; else MOSI::P = 0;	// bit6
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x20) MOSI::P = 1; else MOSI::P = 0;	// bit5
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x10) MOSI::P = 1; else MOSI::P = 0;	// bit4
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x08) MOSI::P = 1; else MOSI::P = 0;	// bit3
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x04) MOSI::P = 1; else MOSI::P = 0;	// bit2
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x02) MOSI::P = 1; else MOSI::P = 0;	// bit1
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				r <<= 1;
				if(data & 0x01) MOSI::P = 1; else MOSI::P = 0;	// bit0
				SPCK::P = 0;
				if(MISO::P()) ++r;
				SPCK::P = 1;

				return r;
			} else if(MODE == soft_spi_mode::CK10) {
				uint8_t r = 0;
				if(MISO::P()) ++r;  // bit7
				if(data & 0x80) MOSI::P = 1; else MOSI::P = 0;	// bit7
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit6
				if(data & 0x40) MOSI::P = 1; else MOSI::P = 0;	// bit6
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit5
				if(data & 0x20) MOSI::P = 1; else MOSI::P = 0;	// bit5
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit4
				if(data & 0x10) MOSI::P = 1; else MOSI::P = 0;	// bit4
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit3
				if(data & 0x08) MOSI::P = 1; else MOSI::P = 0;	// bit3
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit2
				if(data & 0x04) MOSI::P = 1; else MOSI::P = 0;	// bit2
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit1
				if(data & 0x02) MOSI::P = 1; else MOSI::P = 0;	// bit1
				clock_();

				r <<= 1;
				if(MISO::P()) ++r;  // bit0
				if(data & 0x01) MOSI::P = 1; else MOSI::P = 0;	// bit0
				clock_();

				return r;
			} else {

				uint8_t r = 0;
				if(data & 0x80) MOSI::P = 1; else MOSI::P = 0;	// bit7
				clock_();
				if(MISO::P()) ++r;  // bit7

				r <<= 1;
				if(data & 0x40) MOSI::P = 1; else MOSI::P = 0;	// bit6
				clock_();
				if(MISO::P()) ++r;  // bit6

				r <<= 1;
				if(data & 0x20) MOSI::P = 1; else MOSI::P = 0;	// bit5
				clock_();
				if(MISO::P()) ++r;  // bit5

				r <<= 1;
				if(data & 0x10) MOSI::P = 1; else MOSI::P = 0;	// bit4
				clock_();
				if(MISO::P()) ++r;  // bit4

				r <<= 1;
				if(data & 0x08) MOSI::P = 1; else MOSI::P = 0;	// bit3
				clock_();
				if(MISO::P()) ++r;  // bit3

				r <<= 1;
				if(data & 0x04) MOSI::P = 1; else MOSI::P = 0;	// bit2
				clock_();
				if(MISO::P()) ++r;  // bit2

				r <<= 1;
				if(data & 0x02) MOSI::P = 1; else MOSI::P = 0;	// bit1
				clock_();
				if(MISO::P()) ++r;  // bit1

				r <<= 1;
				if(data & 0x01) MOSI::P = 1; else MOSI::P = 0;	// bit0
				clock_();
				if(MISO::P()) ++r;  // bit0

				return r;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  シリアル送信
			@param[in]	src	送信ソース
			@param[in]	cnt	送信サイズ
		*/
		//-----------------------------------------------------------------//
		void send(const void* src, uint32_t size)
		{
			auto ptr = static_cast<const uint8_t*>(src);
			auto end = ptr + size;
			while(ptr < end) {
				xchg(*ptr);
				++ptr;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  シリアル受信
			@param[out]	dst	受信先
			@param[in]	cnt	受信サイズ
		*/
		//-----------------------------------------------------------------//
		void recv(void* dst, uint32_t size)
		{
			uint8_t* ptr = static_cast<uint8_t*>(dst);
			uint32_t pos = 0;
			while(pos < size) {
				*ptr = xchg();
				++ptr;
				++pos;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  RSPIを無効にして、パワーダウンする
			@param[in]	power パワーダウンをしない場合「false」
		*/
		//-----------------------------------------------------------------//
		void destroy(bool power = true)
		{
			MISO::PU = 0;
			MOSI::DIR = 0;
			SPCK::DIR = 0;
		}
	};
}
