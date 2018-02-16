#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・クライアント・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <winsock2.h>

#include <string>
#include <queue>

#include "utils/format.hpp"
#include "utils/string_utils.hpp"

namespace net {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor Client クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ign_client_tcp {
	public:
		static const uint32_t WAVE_BUFF_SIZE = 2048;

	private:
		bool		startup_;

		SOCKET		sock_;

		bool		connect_;

		std::string	line_;

		typedef std::queue<std::string> SQUEUE;
		SQUEUE		rmsg_;

		uint32_t	crcd_;
		uint32_t	crrd_;

		uint32_t	wdm_ch_;
		uint32_t	wdm_pos_;
		uint32_t	wdm_st_[4];
		uint16_t	wdm_buff_[WAVE_BUFF_SIZE * 4];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ign_client_tcp() : startup_(false),
			sock_(0),
			connect_(false), crcd_(0), crrd_(0),
			wdm_ch_(0), wdm_pos_(0), wdm_st_{ 0 }
		{
		}


		~ign_client_tcp()
		{
			if(startup_) {
				WSACleanup();
			}
		}


		uint32_t get_crcd() const { return crcd_; }

		uint32_t get_crrd() const { return crrd_; }

		const uint16_t* get_wdm(uint32_t ch) const {
			return &wdm_buff_[WAVE_BUFF_SIZE * (ch & 3)];
		}

		uint32_t get_wdm_st(uint32_t ch) const { return wdm_st_[ch & 3]; }

		//-----------------------------------------------------------------//
		/*!
			@brief  接続状態の確認
			@return 接続なら「true」
		*/
		//-----------------------------------------------------------------//
		bool probe() const { return connect_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
			@param[in]	ip	IP アドレス
			@param[in]	pn	ポート番号
		*/
		//-----------------------------------------------------------------//
		bool start(const std::string& ip, uint16_t pn)
		{
			if(!startup_) {
				WSADATA wsaData;
				int res = WSAStartup(MAKEWORD(2,0), &wsaData);
				if(res != NO_ERROR) {
					std::cout << "WSAStartup function failed with error:" << std::endl;
					return false;
				}
			}

			// クライアントソケット作成
			sock_ = socket(AF_INET, SOCK_STREAM, 0);
			if(sock_ == INVALID_SOCKET) {
				std::cout << "socket failed" << std::endl;
				return false;
			}

			// struct sockaddr_in 作成
			struct sockaddr_in cl;
			cl.sin_family = PF_INET;
			cl.sin_port = htons(pn);
			// 接続先のアドレスを指定（指定しない事も可能）
			cl.sin_addr.s_addr = inet_addr(ip.c_str());

			// クライアントの接続を待つ
			int ret = connect(sock_, (struct sockaddr *)&cl, sizeof(cl));
			if(ret == SOCKET_ERROR) {
				closesocket(sock_);
				perror("TCP connect fail...");
				return false;
			}

			connect_ = true;
			utils::format("TCP Client connect (%d)\n") % sock_;

			u_long val = 1;
			ioctlsocket(sock_, FIONBIO, &val);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			if(!connect_) return;

			char tmp[8192];
			int n = recv(sock_, tmp, sizeof(tmp) - 1, 0);
			if(n < 1) {
				if(WSAGetLastError() == WSAEWOULDBLOCK) {
					// まだ来ない。
				} else {
					std::cout << "recv error..." << std::endl; 
				}
			} else {
				for(int i = 0; i < n; ++i) {
					auto ch = tmp[i];
					line_ += ch;
					if(ch == '\n') {
						rmsg_.push(line_);
						line_.clear();			
					}
				}
			}

			// 読み込みデータ処理
			while(!rmsg_.empty()) {
				auto s = rmsg_.front();
				rmsg_.pop();

				if(s.find("CRCD") == 0) {
					auto t = s.substr(4, 4);
					int v = 0;
					if((utils::input("%x", t.c_str()) % v).status()) {
						crcd_ = v;
					}
				} else if(s.find("CRRD") == 0) {
					auto t = s.substr(4, 4);
					int v = 0;
					if((utils::input("%x", t.c_str()) % v).status()) {
						crrd_ = v;
					}			
				} else if(s.find("WDCH") == 0) {  // WDM チャネル
					auto t = s.substr(4);
					int v = 0;
					utils::input("%d", t.c_str()) % v;
					wdm_ch_ = v;
					wdm_pos_ = 0;
// std::cout << "WDM capture CH: " << wdm_ch_ << std::endl;
				} else if(s.find("WDMW") == 0) {  // WDM 波形
					auto t = s.substr(4);
					while(t.size() > 4) {
						auto d = t.substr(4);
						t[4] = 0;
						int v;
						utils::input("%x", t.c_str()) % v;
						auto pos = wdm_pos_ % WAVE_BUFF_SIZE;
						wdm_buff_[(wdm_ch_ & 3) * WAVE_BUFF_SIZE + pos] = v;
						++wdm_pos_;
						if(wdm_pos_ >= WAVE_BUFF_SIZE) {
// std::cout << "WDM capture END" << std::endl;
							++wdm_st_[wdm_ch_ & 3];
						}
						t = d;
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信
			@param[in]	text	送信文字列
		*/
		//-----------------------------------------------------------------//
		void send_data(const std::string& text)
		{
			if(send(sock_, text.c_str(), text.size(), 0) == SOCKET_ERROR) {
				shutdown(sock_, 2);
				closesocket(sock_);
				connect_ = false;
			}
		}
	};
}
