#pragma once
//=====================================================================//
/*!	@file
	@brief	BDF フォント・ファイルを扱うクラス（ヘッダー）
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
		@brief	BDF フォントファイルの入力
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bdf_io {

		uint32_t				lin_code_max_;
		std::vector<uint8_t>	sjis_pad_;

		uint16_t	jis_code_;
		bool		bitmap_;

		int		map_max_;

		uint32_t	jis_count_;

		uint32_t	bbx_width_;
		uint32_t	bbx_height_;

		utils::bit_array	bit_array_;

		bool	alignment_bits_;

	public:
		bdf_io() : lin_code_max_(0), jis_code_(0), bitmap_(false), map_max_(0),
			jis_count_(0),
			bbx_width_(0), bbx_height_(0), alignment_bits_(8) { }

		~bdf_io() { }

		uint32_t get_width() const { return bbx_width_; }

		uint32_t get_height() const { return bbx_height_; }

		uint32_t byte_size() const { return (bbx_width_ * bbx_height_ + 7) / 8; }

		void initialize();

		bool load(const std::string& filename);

		bool save(const std::string& filename);

		uint32_t size() const { return sjis_pad_.size(); }

		uint8_t get_byte(uint32_t idx) const { return sjis_pad_[idx]; }

	};
}
