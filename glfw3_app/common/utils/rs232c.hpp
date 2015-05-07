#pragma once
//=====================================================================//
/*!	@file
	@brief	RS232C 操作クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	RS232C クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class rs232c {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	パリティの設定
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class parity {
			none,
			even,
			odd
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	キャラクター長
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class char_len {
			bits7,
			bits8
		};


	private:
		int    fd_;

		termios    attr_back_;

		void close_();

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		rs232c() : fd_(-1) { }


		//-----------------------------------------------------------------//
		/*!
			@brief オープン
			@param[in]	path	シリアルポートパス
			@param[in]	brate	接続ボーレート
			@param[in]	clen	キャラクター長
			@param[in]	par		パリティ・ビット
		*/
		//-----------------------------------------------------------------//
		bool open(std::string& path, speed_t brate, char_len clen = char_len::bits8, parity par = parity::none);


		bool close();


		bool out_sync() const {
			if(fd_ < 0) return false;

			tcdrain(fd_);
			return true;
		}


		size_t read(char* dst, size_t len) {
			if(fd_ < 0) return 0;

			return ::read(fd_, dst, len);
		}


		size_t read(char* dst, size_t len, timeval tv) {
			if(fd_ < 0) return 0;

			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd_, &fds);

			int ret = select(fd_, &fds, NULL, NULL, &tv);
			if(ret == -1) {
				// error..
				return 0;
			} else if(ret > 0) {
				return ::read(fd_, dst, static_cast<size_t>(ret));
			}

			return 0;
		}


		size_t write(const char* src, size_t len) {
			if(fd_ < 0) return 0;

			return ::write(fd_, src, len);
		}


		bool get_DCD() const;


		bool get_CTS() const;


		bool get_DSR() const;


		bool enable_DTR(bool ena = true);


		bool enable_RTS(bool ena = true);

	};
}

