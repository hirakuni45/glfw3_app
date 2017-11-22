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
// #include <winsock2.h>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;


namespace net {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor Client クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ign_client {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		ign_client() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start()
		{
			asio::io_service io_service;
			tcp::socket socket(io_service);

			boost::system::error_code error;
			socket.connect(tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 31400), error);

			if(error) {
				std::cout << "connect failed : " << error.message() << std::endl;
			} else {
				std::cout << "connected" << std::endl;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{



		}
	};
}


