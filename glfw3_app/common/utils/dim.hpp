#pragma once
//=====================================================================//
/*!	@file
	@brief	配列クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include <vector>
#include <boost/unordered_map.hpp>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	配列クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dim {

		uint32_t				unit_size_;
		std::vector<uint8_t>	array_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		dim() : unit_size_(1) { }

		void set_unit_size(uint32_t us) { unit_size_ = us; }

		uint32_t get_unit_size() const { return unit_size_; }

		void create(uint32_t size) { array_.resize(size); }

		void* get_ptr(uint32_t idx) { return &array_[idx * unit_size_]; } 

		uint32_t get(uint32_t idx) {
			if(unit_size_ == 2) {
				uint16_t* p = reinterpret_cast<uint16_t*>(&array_[0]);
				return p[idx];
			} else if(unit_size_ == 4) {
				uint32_t* p = reinterpret_cast<uint32_t*>(&array_[0]);
				return p[idx];
			} else {
				return array_[idx];
			}
		}

		void put(uint32_t idx, uint32_t data) {
			if(unit_size_ == 2) {
				uint16_t* p = reinterpret_cast<uint16_t*>(&array_[0]);
				p[idx] = data;
			} else if(unit_size_ == 4) {
				uint32_t* p = reinterpret_cast<uint32_t*>(&array_[0]);
				p[idx] = data;
			} else {
				array_[idx] = data;
			}
		}
	};
}
