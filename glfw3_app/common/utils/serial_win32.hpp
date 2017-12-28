#pragma once
//=====================================================================//
/*!	@file
	@brief	シリアルＩ／Ｏ（WIN32）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <windows.h>
#include <string>

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シリアル通信クラス（WIN32)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class serial_win32 {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	パリティー・タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class PARITY {
			NONE,	///< パリティー無し
			EVEN,	///< 偶数パリティー
			ODD,	///< 奇数パリティー
		};

	private:
		HANDLE		fd_;

		std::string	error_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		serial_win32() : fd_(INVALID_HANDLE_VALUE) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@param[in]	port	ポート名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& port)
		{
			std::string full("\\\\.\\");
			full += port;
///			snprintf(port_full_name, sizeof port_full_name - 2u, "\\\\.\\%s", port);
///			if(4 <= verbose_level) {
///				printf("\t\tOpen port: %s\n", port_full_name);
///			}
			fd_ = CreateFile(full.c_str(),
					GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0);

			bool ret = false;
			if(INVALID_HANDLE_VALUE == fd_) {
				int error_num = GetLastError();
				char error_string[1024];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					error_num,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					error_string,
					sizeof error_string,
					NULL);
///			fprintf(stderr, "Unable to open port: (%i) %s\n", error_num, error_string);
					error_ = error_string;
			} else {
				DCB dcbSerialParams;
				GetCommState(fd_, &dcbSerialParams);
				dcbSerialParams.BaudRate = CBR_115200;
				dcbSerialParams.ByteSize = 8;
				dcbSerialParams.StopBits = ONESTOPBIT;
				dcbSerialParams.Parity   = NOPARITY;
				SetCommState(fd_, &dcbSerialParams);

				COMMTIMEOUTS timeouts;
				timeouts.ReadIntervalTimeout=50;
				timeouts.ReadTotalTimeoutConstant=50;
				timeouts.ReadTotalTimeoutMultiplier=10;
				timeouts.WriteTotalTimeoutConstant=0;
				timeouts.WriteTotalTimeoutMultiplier=0;
				SetCommTimeouts(fd_, &timeouts);
				FlushFileBuffers(fd_);
				ret = true;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーレートの設定
			@param[in]	baud	ボーレート
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_baud(int baud)
		{
			DCB dcbSerialParams;
			GetCommState(fd_, &dcbSerialParams);
			dcbSerialParams.BaudRate = baud;
			return SetCommState(fd_, &dcbSerialParams) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	パリティーの設定
			@param[in]	parity	パリティー・タイプ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_parity(PARITY parity)
		{
			DCB dcbSerialParams;
			GetCommState(fd_, &dcbSerialParams);
			switch(parity) {
			case PARITY::EVEN:
				dcbSerialParams.Parity = EVENPARITY;
				break;
			case PARITY::ODD:
				dcbSerialParams.Parity = ODDPARITY;
				break;
			case PARITY::NONE:
			default:
				dcbSerialParams.Parity = NOPARITY;
				break;
			}
			return SetCommState(fd_, &dcbSerialParams) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DTR の設定（Data Terminal Ready)
			@param[in]	value	設定値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_dtr(bool value)
		{
			int command;
			if(value) {
				command = SETDTR;
			} else {
				command = CLRDTR;
			}
			return EscapeCommFunction(fd_, command) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	RTS の設定（Request To Send)
			@param[in]	value	設定値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_rts(bool value)
		{
			int command;
			if(value) {
				command = SETRTS;
			} else {
				command = CLRRTS;
			}
			return EscapeCommFunction(fd_, command) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	TXD の設定
			@param[in]	value	設定値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_txd(bool value)
		{
			int command;
			if(value) {
				command = SETBREAK;
			} else {
				command = CLRBREAK;
				
			}
			return EscapeCommFunction(fd_, command) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フラッシュ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool flush()
		{
//			if(4 <= verbose_level) {
//				printf("\t\tFlush IO buffers\n");
//			}
			return PurgeComm(fd_, PURGE_RXCLEAR | PURGE_TXCLEAR) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライト
			@param[in]	src	ソース
			@param[in]	len	長さ（バイト）
			@return 書き込んだサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t write(const void* src, uint32_t len)
		{
#if 0
			if(4 <= verbose_level) {
				unsigned char *p = (unsigned char*)buf;
				unsigned int i;
				printf("\t\tsend(%u): ", len);
				for(i = len; 0 < i; --i) {
					printf("%02X ", *p++);
				}
				printf("\n");
			}
#endif
			int bytes_left = len;
			DWORD bytes_written;
			const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
			do {
				if(0 == WriteFile(fd_, p, bytes_left, &bytes_written, NULL)) {
//					fprintf(stderr, "Failed to write to port.\n");
					return false;
				}
				p += bytes_written;
				bytes_left -= bytes_written;
			} while(0 < bytes_left) ;
			return len - bytes_left;
		}



	};
}


#if 0

int serial_read(port_handle_t fd, void *buf, int len)
{
    int bytes_left = len;
    DWORD bytes_read;
    unsigned char *pbuf = (unsigned char*)buf;
    do
    {
        if (0 == ReadFile(fd, pbuf, bytes_left, &bytes_read, NULL))
        {
            fprintf(stderr, "Failed to read from port.\n");
            return -1;
        }
        if (0 == bytes_read)
        {
            break;
        }
        pbuf += bytes_read;
        bytes_left -= bytes_read;
    }
    while (0 < bytes_left);
    const int nbytes = len - bytes_left;
    if (4 <= verbose_level)
    {
        pbuf = buf;
        unsigned int i;
        printf("\t\trecv(%u): ", nbytes);
        for (i = nbytes; 0 < i; --i)
        {
            printf("%02X ", *pbuf++);
        }
        printf("\n");
    }
    return nbytes;
}

int serial_close(port_handle_t fd)
{
    if (4 <= verbose_level)
    {
        printf("\t\tClose port\n");
    }
    return CloseHandle(fd) != 0 ? 0 : -1;
}
#endif
