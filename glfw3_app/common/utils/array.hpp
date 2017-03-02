#pragma once
//=====================================================================//
/*!	@file
	@brief	アレイ @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <memory>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	array クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct array {
		typedef T  value_type;

	private:
		T*			ptr_;
		uint32_t	size_;

	public:
		array() : ptr_(nullptr), size_(0) { }

		explicit array(const T* ptr, uint32_t size) {
			ptr_ = new T[size];
			memcpy(ptr_, ptr, size);
		}

		~array() { delete[] ptr_; }

		void build(uint32_t size) {
			delete[] ptr_;
			size_ = size;
			ptr_ = new T[size];
		}

		uint32_t size() const { return size_; }

		const T* get() const { return ptr_; }

		T* at() { return ptr_; }

		void copy(const void* src, uint32_t size) {
			delete[] ptr_;
			size_ = size;
			ptr_ = new T[size];
			memcpy(ptr_, src, size);
		}

		array& operator = (const array& src) {
			copy(src.get(), src.size());
			return *this;
		}
	};

	typedef array<uint8_t>   array_u8;
	typedef array<uint16_t>  array_u16;
	typedef array<uint32_t>  array_u32;

	typedef std::shared_ptr<array_u8>   shared_array_u8;
	typedef std::shared_ptr<array_u16>  shared_array_u16;
	typedef std::shared_ptr<array_u32>  shared_array_u32;
}
