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

		bool				connect_;

		asio::streambuf		send_;
		asio::streambuf		recv_;

		void send_end_(const boost::system::error_code& error)
		{
			if(send_.size() > 0) {
				std::cout << "request : " << asio::buffer_cast<const char*>(send_.data()) << std::endl;
				send_.consume(send_.size());
			}
		}


		void async_send_()
		{
			const std::string request = "asdfg\n";

			std::ostream os(&send_);
			os << request;

			asio::async_write(socket_, send_.data(), boost::bind(&ign_client::send_end_, this, _1));
		}


		void recv_end_(const boost::system::error_code& error)
    	{
			if(recv_.size() > 0) {
				std::cout << "response : " << asio::buffer_cast<const char*>(recv_.data()) << std::endl;
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
        	} else {
				connect_ = true;
				std::cout << "connected" << std::endl;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ign_client(asio::io_service& ios) : io_service_(ios), socket_(ios), connect_(false)
		{
		}


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
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start()
		{
			connect_ = false;
			socket_.async_connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 31400),
				boost::bind(&ign_client::on_connect_, this, asio::placeholders::error));			connect_ = false;
//			socket_.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 31400));

			std::cout << "Connection start..." << std::endl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{


			if(connect_) {
//				io_service_.reset();
				static uint32_t cnt = 0;
				if(cnt >= 60) {
					async_send_();
					cnt = 0;
					gui::format("Send...\n");
				}
				++cnt;

				async_recv_();
			}
		}
	};
}


