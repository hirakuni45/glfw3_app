#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・クライアント・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <winsock2.h>

#include <string>
#include <queue>

#include "utils/format.hpp"
#include "utils/string_utils.hpp"

// デバッグ・エミュレーションを行う場合有効にする
// #define DEBUG_EMU

namespace net {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor Client クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ign_client_tcp {
	public:
		static const uint32_t WAVE_BUFF_SIZE = 2048;

		//=================================================================//
		/*!
			@brief  モジュール・ステータス構造体
		*/
		//=================================================================//
		struct mod_status {
			uint32_t	crdd_;
			uint32_t	crdd_id_;

			uint32_t	crcd_;
			uint32_t	crcd_id_;

			uint32_t	crrd_;
			uint32_t	crrd_id_;

			uint32_t	d2md_;
			uint32_t	d2md_id_;

			uint32_t	wdm_id_[4];

			uint32_t	treg_id_[2];

			mod_status() :
				crdd_(0), crdd_id_(0),
				crcd_(0), crcd_id_(0),
				crrd_(0), crrd_id_(0),
				d2md_(0), d2md_id_(0),
				wdm_id_{ 0 }, treg_id_{ 0 }
			{ }
		};

	private:
		bool		startup_;

		SOCKET		sock_;

		bool		connect_;

		std::string	line_;

		typedef std::queue<std::string> SQUEUE;
		SQUEUE		rmsg_;

		mod_status	mod_status_;

		uint32_t	wdm_ch_;
		uint32_t	wdm_pos_;
		uint16_t	wdm_buff_[WAVE_BUFF_SIZE * 4];

		uint32_t	treg_ch_;
		uint32_t	treg_pos_;
		uint16_t	treg_buff_[WAVE_BUFF_SIZE * 2];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ign_client_tcp() : startup_(false),
			sock_(0),
			connect_(false), mod_status_(),
			wdm_ch_(0), wdm_pos_(0), wdm_buff_{ 0 },
			treg_ch_(0), treg_pos_(0), treg_buff_{ 0 }
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~ign_client_tcp()
		{
			if(startup_) {
				WSACleanup();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  モジュール・ステータスの取得
			@return モジュール・ステータス
		*/
		//-----------------------------------------------------------------//
		const mod_status& get_mod_status() const { return mod_status_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  波形の取得
			@param[in]	ch	チャネル
			@return 波形
		*/
		//-----------------------------------------------------------------//
		const uint16_t* get_wdm(uint32_t ch) const {
			return &wdm_buff_[WAVE_BUFF_SIZE * (ch & 3)];
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  熱抵抗波形の取得
			@param[in]	ch	チャネル（前半:0、後半:1）
			@return 熱抵抗波形
		*/
		//-----------------------------------------------------------------//
		const uint16_t* get_treg(uint32_t ch) const {
			return &treg_buff_[WAVE_BUFF_SIZE * (ch & 1)];
		}


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
#ifdef DEBUG_EMU
			connect_ = true;
			return true;
#endif
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
//			utils::format("TCP Client connect (%d)\n") % sock_;

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
#ifdef DEBUG_EMU


#else
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
std::cout << s << std::endl;

				if(s.find("CRCD") == 0) {
					auto t = s.substr(4, 8);
std::cout << t << std::endl;
					int v = 0;
					if((utils::input("%x", t.c_str()) % v).status()) {
						mod_status_.crcd_ = v;
						++mod_status_.crcd_id_;
					}
				} else if(s.find("CRRD") == 0) {
					auto t = s.substr(4, 8);
std::cout << t << std::endl;
					int v = 0;
					if((utils::input("%x", t.c_str()) % v).status()) {
						mod_status_.crrd_ = v;
						++mod_status_.crrd_id_;
					}			
				} else if(s.find("CRDD") == 0) {
					auto t = s.substr(4, 8);
std::cout << t << std::endl;
					int v = 0;
					if((utils::input("%x", t.c_str()) % v).status()) {
						mod_status_.crdd_ = v;
						++mod_status_.crdd_id_;
					}			
				} else if(s.find("D2MD") == 0) {
					auto t = s.substr(4, 5);
					int v = 0;
					if((utils::input("%x", t.c_str()) % v).status()) {
						mod_status_.d2md_ = v;
						++mod_status_.d2md_id_;
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
							++mod_status_.wdm_id_[wdm_ch_ & 3];
						}
						t = d;
					}
				} else if(s.find("TRCH") == 0) {  // 熱抵抗チャネル (0, 1)
					auto t = s.substr(4);
					int v = 0;
					utils::input("%d", t.c_str()) % v;
					treg_ch_ = v;
					treg_pos_ = 0;
// std::cout << "TRCH: " << treg_ch_ << std::endl;
				} else if(s.find("TRMW") == 0) {  // 熱抵抗波形 (0, 1)
					auto t = s.substr(4);
					while(t.size() > 4) {
						auto d = t.substr(4);
						t[4] = 0;
						int v;
						utils::input("%x", t.c_str()) % v;
						auto pos = treg_pos_ % WAVE_BUFF_SIZE;
						treg_buff_[(treg_ch_ & 1) * WAVE_BUFF_SIZE + pos] = v;
						++treg_pos_;
						if(treg_pos_ >= WAVE_BUFF_SIZE) {
							++mod_status_.treg_id_[treg_ch_ & 1];
// std::cout << "TRMW: " << treg_ch_ << std::endl;
						}
						t = d;
					}
				}
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信
			@param[in]	text	送信文字列
		*/
		//-----------------------------------------------------------------//
		void send_data(const std::string& text)
		{
// std::cout << text << std::endl;
#ifdef DEBUG_EMU
			if(text.find("CRR?1") != std::string::npos) {
				static double aaa = 0.0123;
				double a = aaa;  // オーム
				aaa += 0.0001;
				a *= 200.0;  // 200mA
				a /= 778.2;
				a *= 50.0;
				a *= static_cast<double>(0x7FFFF) / 1.570798233;
				mod_status_.crrd_ = 50 * 0x7ffff + static_cast<uint32_t>(a);
				++mod_status_.crrd_id_;
			} else if(text.find("CRC?1") != std::string::npos) {
				double a = 0.33;
				a /= 1e6;
				a = 1.0 / (2.0 * 3.141592654 * 1000.0 * a);
				a *= 2.0;
				a /= 778.2;  // 778.2 mV P-P
				a *= 50.0;
				a *= static_cast<double>(0x7FFFF) / 1.570798233;
				mod_status_.crcd_ = 50 * 0x7ffff + static_cast<uint32_t>(a);
				++mod_status_.crcd_id_;
			} else if(text.find("wdm 20") != std::string::npos) {
				++mod_status_.wdm_id_[2];
			}
///			std::cout << text << std::endl;
			return;
#else
			if(send(sock_, text.c_str(), text.size(), 0) == SOCKET_ERROR) {
				shutdown(sock_, 2);
				closesocket(sock_);
				connect_ = false;
			}
#endif
		}
	};
}
