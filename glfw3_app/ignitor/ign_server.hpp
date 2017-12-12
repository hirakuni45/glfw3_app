#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・サーバー・クラス @n
			※エミュレーション、テスト用、実際のサーバーは、外部マイコン
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "utils/string_utils.hpp"

namespace asio = boost::asio;
namespace ip = asio::ip;

namespace net {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor Server クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ign_server {

		asio::io_service&	io_service_;
    	ip::tcp::acceptor	acceptor_;
    	ip::tcp::socket		socket_;

		asio::streambuf		recv_;

		bool				connect_;
		bool				read_trg_;

		void on_recv_(const boost::system::error_code& error, size_t bytes_transferred)
		{
			if(error && error != boost::asio::error::eof) {
				std::cout << "receive failed: " << error.message() << std::endl;
			} else {
				const char* data = asio::buffer_cast<const char*>(recv_.data());
				std::cout << "response(" << bytes_transferred << "): ";
				std::cout << data << std::endl;
				recv_.consume(recv_.size());
				read_trg_ = true;
			}
		}


		void on_accept_(const boost::system::error_code& error)
		{
			if(error) {
				auto out = utils::sjis_to_utf8(error.message());
				std::cout << "accept failed: " << out << std::endl;
			} else {
				std::cout << "accept correct!" << std::endl;
				connect_ = true;
				read_trg_ = true;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ign_server(asio::io_service& ios) : io_service_(ios),
			acceptor_(ios, ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 31400)),
			socket_(ios), recv_(), connect_(false), read_trg_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start()
		{
			connect_ = false;
			acceptor_.async_accept(socket_,
				boost::bind(&ign_server::on_accept_, this, asio::placeholders::error));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			if(connect_) {
//				boost::asio::async_read(socket_, recv_, asio::transfer_all(),
				if(read_trg_) {
					boost::asio::async_read(socket_, recv_, asio::transfer_at_least(4),
						boost::bind(&ign_server::on_recv_, this,
						asio::placeholders::error, asio::placeholders::bytes_transferred));
					read_trg_ = false;
				}
			}
		}
	};
}
