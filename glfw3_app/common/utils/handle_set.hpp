#pragma once
//=====================================================================//
/*!	@file
	@brief	ハンドルによるリソース管理
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <set>
#include <vector>
#include <boost/optional.hpp>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ハンドル管理テンプレート・クラス
		@param[in]	HANDLE	ハンドルの型
		@param[in]	SET		管理用 set コンテナ
		@param[in]	ARRAY	アレイコンテナ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T,
			  class HANDLE = uint32_t,
			  class SET = std::set<HANDLE>,
			  class ARRAY = std::vector<T> > 
	struct handle_set {
		typedef T		value_type;
		typedef HANDLE	handle_type;
		typedef ARRAY	array_type;

		typedef boost::optional<T&>			optional_value_type;
		typedef boost::optional<const T&>	optional_const_value_type;

	private:
		array_type		array_;

		typedef SET		set_type;
		set_type		erase_set_;
		typedef typename set_type::iterator	set_it;

		bool			zero_handle_enable_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	zhe	０番ハンドルを無効にする場合「false」	
			@param[in]	fas	初期アレイサイズ
		*/
		//-----------------------------------------------------------------//
		handle_set(bool zhe = true, uint32_t fas = 0) : array_(), erase_set_(),
														zero_handle_enable_(zhe) {
			if(fas) {
				array_.reserve(fas);
				array_.clear();
			}
			if(zero_handle_enable_) array_.push_back(T());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~handle_set() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	使用サイズを返す
			@return 使用サイズ
		*/
		//-----------------------------------------------------------------//
		handle_type size() const { return static_cast<handle_type>(array_.size()); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルを消去
		*/
		//-----------------------------------------------------------------//
		void clear() {
			erase_set_.clear();
			array_.clear();
			if(zero_handle_enable_) array_.push_back(T());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルが有効か調べる
			@param[in]	h	ハンドル
			@return 有効なら「true」が返る
		*/
		//-----------------------------------------------------------------//
		inline bool probe(handle_type h) const {
			if(static_cast<size_t>(h) < size()) {
				return erase_set_.find(h) == erase_set_.end();
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルを消去
			@param[in]	h	ハンドル
			@return 消去できたら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool erase(handle_type h) {
			if(static_cast<size_t>(h) < size()) {
				set_it it = erase_set_.find(h);
				if(it == erase_set_.end()) {
					erase_set_.insert(h);
					return true;
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	インスタンスを挿入してハンドルを返す
			@param[in]	st	インスタンス
			@return ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		handle_type insert(const T& st) {
			handle_type h;
			if(erase_set_.empty()) {
				h = static_cast<handle_type>(array_.size());
				array_.push_back(st);
			} else {
				set_it it = erase_set_.begin();
				h = *it;
				array_[h] = st;
				erase_set_.erase(it);
			}
			return h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	値の参照（RW）
			@param[in]	h	ハンドル
			@return 有効なら「true」を返す。
		*/
		//-----------------------------------------------------------------//
		optional_value_type at(handle_type h) {
			if(probe(h)) {
				return optional_value_type(array_[h]);
			} else {
				return optional_value_type();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	値の参照（RO）
			@param[in]	h	ハンドル
			@return 有効なら「true」を返す。
		*/
		//-----------------------------------------------------------------//
		optional_const_value_type get(handle_type h) const {
			if(probe(h)) {
				return optional_const_value_type(array_[h]);
			} else {
				return optional_const_value_type();
			}
		}
	};
}
