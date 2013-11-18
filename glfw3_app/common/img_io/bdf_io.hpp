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
		@brief	BDF クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bdf_io {

		uint32_t				lin_code_max_;
		std::vector<uint16_t>	codemap_;

		uint16_t	jis_code_;
		bool		bitmap_;

		int		map_max_;

		int		bbx_width_;
		int		bbx_height_;

		utils::bit_array	bit_array_;

		bool	alignment_bits_;

	public:
		bdf_io() : lin_code_max_(0), jis_code_(0), bitmap_(false), map_max_(0),
			bbx_width_(0), bbx_height_(0), alignment_bits_(8) { }

		~bdf_io() { }

		void initialize();

		bool load(const std::string& filename);

		bool save(const std::string& filename);

	};
}
