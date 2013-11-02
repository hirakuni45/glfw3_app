#pragma once
//=====================================================================//
/*!	@file
	@breif	BDF フォント・ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include <string>
#include <vector>
#include "utils/bit_array.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	BDF クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bdf_io {

		std::vector<char>	codemap_;
		std::vector<uint8_t>	bitmaps_;

		unsigned short	jis_code_;
		bool	bitmap_;

		int		map_max_;

		int		bbx_width_;
		int		bbx_height_;

		utils::bit_array	bit_array_;

		bool	alignment_bits_;

	public:
		bdf_io() : jis_code_(0), bitmap_(false), map_max_(0),
			bbx_width_(0), bbx_height_(0), alignment_bits_(8) { }

		~bdf_io() { }

		void initialize();

		size_t size() const { return bitmaps_.size(); }

		const unsigned char* get_array() const { return &bitmaps_[0]; }

		bool load(const std::string& filename);

		bool save(const std::string& filename);

	};
}
