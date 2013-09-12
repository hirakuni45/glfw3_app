#pragma once
//=====================================================================//
/*!	@file
	@brief	音楽タグ
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include "img_io/i_img.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	タグ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct tag {
		uint32_t		serial_;

		std::string		title_;
		std::string		artist_;
		std::string		writer_;
		std::string		album_;
		std::string		track_;
		std::string		total_tracks_;
		std::string		disc_;
		std::string		total_discs_;
		std::string		date_;

		img::i_img*		image_;

		tag() : serial_(0), image_(0) { }

		void clear() {
			serial_ = 0;
			title_.clear();
			artist_.clear();
			writer_.clear();
			album_.clear();
			track_.clear();
			total_tracks_.clear();
			disc_.clear();
			total_discs_.clear();
			date_.clear();
		}
		void update() { ++serial_; }
	};

}


