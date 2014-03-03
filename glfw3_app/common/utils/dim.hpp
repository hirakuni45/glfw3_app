#pragma once
//=====================================================================//
/*!	@file
	@brief	配列クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	配列クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dim {
		struct i_dim {
			virtual ~i_dim() { }
			virtual void resize(uint32_t s) = 0;
			virtual uint32_t size() const = 0;
			virtual uint32_t unit_size() const = 0;
			virtual uint32_t get(uint32_t idx) const = 0;
			virtual void put(uint32_t idx, uint32_t val) = 0;
			virtual void* ptr(uint32_t idx) = 0;
		};

		template <typename T>
		class dimx : public i_dim {
			std::vector<T>	array_;
		public:
			typedef T	value_type;
			virtual ~dimx() { }
			void resize(uint32_t s) { array_.resize(s); }
			uint32_t size() const { return array_.size(); }
			uint32_t unit_size() const { return sizeof(T); }
			uint32_t get(uint32_t idx) const { return array_[idx]; }
			void put(uint32_t idx, uint32_t val) { array_[idx] = val; }
			void* ptr(uint32_t idx) { return &array_[idx]; } 
		};

		i_dim*		array_;
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		dim() : array_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~dim() { delete array_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットサイズを選択
			@param[in]	n	ユニットサイズ（１、２，４）
		*/
		//-----------------------------------------------------------------//
		void select(uint32_t n) {
			delete array_;
			array_ = 0;
			if(n == 1) {
				array_ = new dimx<uint8_t>;
			} else if(n == 2) {
				array_ = new dimx<uint16_t>;
			} else if(n == 4) {
				array_ = new dimx<uint32_t>;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リサイズ
			@param[in]	s	サイズ
		*/
		//-----------------------------------------------------------------//
		void resize(uint32_t s) { array_->resize(s); }


		//-----------------------------------------------------------------//
		/*!
			@brief	要素サイズを返す
			@return 要素サイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t size() const { return array_->size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	要素が空か？
			@return 要素が空の場合「true」
		*/
		//-----------------------------------------------------------------//
		bool empty() const { return array_->size() == 0; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットサイズを返す
			@return ユニットサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t unit_size() const { return array_->unit_size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	要素を取得
			@param[in]	idx	要素位置
			@return 要素
		*/
		//-----------------------------------------------------------------//
		uint32_t get(uint32_t idx) const { return array_->get(idx); }


		//-----------------------------------------------------------------//
		/*!
			@brief	要素を設定
			@param[in]	idx	要素位置
			@param[in]	val	値
		*/
		//-----------------------------------------------------------------//
		void put(uint32_t idx, uint32_t val) { array_->put(idx, val); }


		//-----------------------------------------------------------------//
		/*!
			@brief	要素をポインターを取得
			@param[in]	idx	要素位置
			@return 要素ポインター
		*/
		//-----------------------------------------------------------------//
		void* ptr(uint32_t idx = 0) { return array_->ptr(idx); }
	};
}
