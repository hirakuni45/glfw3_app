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
				std::string s = asio::buffer_cast<const char*>(recv_.data());
				rmsg_.push(s);
				recv_.consume(recv_.size());
			}
		}


		void async_recv_()
		{
			asio::async_read_until(socket_, recv_, '\n', boost::bind(&ign_client::recv_end_, this, _1));
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
			connect_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~ign_client()
		{
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  接続状態の確認
			@return 接続なら「true」
		*/
		//-----------------------------------------------------------------//
		bool connect() const { return connect_; }


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
			if(connect_) {
				async_recv_();
			}
//			io_service_.reset();
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

		// 受信データがあれば「true」
		bool recv_probe() const { return !rmsg_.empty(); }


		std::string recv()
		{
			if(rmsg_.empty()) return "";

			auto s = rmsg_.front();
			rmsg_.pop();
			return s;
		}
	};
}


