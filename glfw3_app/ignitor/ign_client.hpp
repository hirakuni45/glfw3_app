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
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <queue>
#include <string>

#include "utils/format.hpp"
#include "utils/string_utils.hpp"

// #include <boost/asio/detail/winsock_init.hpp>
// boost::asio::detail::winsock_init<>::manual manual_winsock_init
// __attribute__ ((init_priority (101)));

namespace asio = boost::asio;
namespace ip = asio::ip;

namespace net {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor Client クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ign_client {
		asio::io_service&	io_service_;
		ip::tcp::socket		socket_;
//		asio::deadline_timer connect_timer_;

		bool				connect_;

		asio::streambuf		send_;
		asio::streambuf		recv_;

		std::string			ips_;

		typedef std::queue<std::string> SQUEUE;
		SQUEUE				rmsg_;

		uint32_t			crcd_;
		uint32_t			crrd_;

		uint32_t			wdm_ch_;
		uint32_t			wdm_pos_;
		uint16_t			wdm_buff_[2048];

		std::string			back_;

		void send_end_(const boost::system::error_code& error)
		{
			if(send_.size() > 0) {
//				std::cout << "request : " << asio::buffer_cast<const char*>(send_.data()) << std::endl;
				send_.consume(send_.size());
			}
		}


		void async_send_(const std::string& request)
		{
			std::ostream os(&send_);
			os << request;
			asio::async_write(socket_, send_.data(),
				boost::bind(&ign_client::send_end_, this, _1));
		}


		void recv_end_(const boost::system::error_code& error)
    	{
			if(recv_.size() > 0) {
///				std::cout << "response : " << asio::buffer_cast<const char*>(recv_.data()) << std::endl;
				rmsg_.emplace(asio::buffer_cast<const char*>(recv_.data()));

				recv_.consume(recv_.size());

				async_recv_();
			}
		}


		void async_recv_()
		{
			asio::async_read_until(socket_, recv_, '\n',
				boost::bind(&ign_client::recv_end_, this, _1));
		}


		void on_connect_(const boost::system::error_code& error)
    	{
        	if(error) {
				const std::string& in = error.message();
				auto out = utils::sjis_to_utf8(in);
            	std::cout << "connect failed: " << out << std::endl;
				socket_.close();
        	} else {
//				connect_timer_.cancel();
				connect_ = true;
//				std::cout << "connected" << std::endl;
			}
		}


		void on_connect_timeout_(const boost::system::error_code& error)
		{
			socket_.close();
			// 接続タイムアウト
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ign_client(asio::io_service& ios) :
			io_service_(ios), socket_(ios), /* connect_timer_(ios), */
			connect_(false), crcd_(0), crrd_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~ign_client()
		{
		}


		uint32_t get_crcd() const { return crcd_; }

		uint32_t get_crrd() const { return crrd_; }

		const uint16_t* get_wdm(uint32_t ch) const { return wdm_buff_; }


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
		void start(const std::string& ip, uint16_t pn)
		{
			if(connect_) {
				socket_.close();
			}
			connect_ = false;

			ips_ = "127.0.0.1";
			if(!ip.empty()) ips_ = ip;
///			socket_.async_connect(ip::tcp::endpoint(ip::address::from_string(ips_), pn),
			socket_.async_connect(ip::tcp::endpoint(ip::address::from_string("192.168.0.20"), pn),
				boost::bind(&ign_client::on_connect_, this, _1));
//				boost::bind(&ign_client::on_connect_, this, asio::placeholders::error));

//			connect_timer_.expires_from_now(boost::posix_time::seconds(20));
//			connect_timer_.async_wait(boost::bind(&ign_client::on_connect_timeout_, this, _1));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			if(!connect_) return;

			async_recv_();

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
///std::cout << "WDM ch: %d" << v << std::endl;
				} else if(s.find("WDMW") == 0) {  // WDM 波形
					auto t = s.substr(4);
// std::cout << wdm_pos_ << ": ";
					while(t.size() > 4) {
						auto d = t.substr(4);
						t[4] = 0;
						int v;
						utils::input("%x", t.c_str()) % v;
						wdm_buff_[wdm_pos_ % 2048] = v;
// std::cout << v << ", ";
						++wdm_pos_;
						t = d;
					}
// std::cout << std::endl;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  送信
			@param[in]	text	送信文字列
		*/
		//-----------------------------------------------------------------//
		void send(const std::string& text)
		{
			if(text.empty()) return;

			async_send_(text);
		}

	};
}


