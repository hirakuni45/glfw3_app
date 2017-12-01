#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・サーバー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>

#define ASIO_STANDALONE
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace asio = boost::asio;
namespace ip = asio::ip;

namespace net {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor Server クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ign_server {


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
//		ign_server(asio::io_service& ios) : io_service_(ios), socket_(ios), connect_(false)


	};
}
