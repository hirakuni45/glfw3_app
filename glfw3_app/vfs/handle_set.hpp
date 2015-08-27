#pragma once
//=====================================================================//
/*!	@file
	@brief	handle_set テンプレートクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <set>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ハンドル管理テンプレート・クラス
		@param[in]	HANDLE	ハンドルの型
		@param[in]	SET		管理用 set コンテナ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename HANDLE, class SET = std::set<HANDLE> >
	struct handle_set {
		typedef HANDLE	handle_type;

	private:
		HANDLE			start_;
		HANDLE			current_;
		HANDLE			limit_;

		typedef SET		set_type;
		set_type		erase_set_;
		typedef typename set_type::iterator	set_it;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	limit	ハンドル最終値
			@param[in]	start	ハンドル開始値（０番は常に無効とするので、０以外の値）
		*/
		//-----------------------------------------------------------------//
		handle_set(HANDLE limit, HANDLE start = 1) : start_(start), current_(start), limit_(limit),
			erase_set_() {
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~handle_set() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルの最大数
			@return 最大数
		*/
		//-----------------------------------------------------------------//
		handle_type max_size() const { return limit_ - 1; }


		//-----------------------------------------------------------------//
		/*!
			@brief	使用サイズを返す
			@return 使用サイズ
		*/
		//-----------------------------------------------------------------//
		handle_type size() const { return current_ - 1 - erase_set_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルを消去
		*/
		//-----------------------------------------------------------------//
		void clear() {
			erase_set_.clear();
			current_ = start_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルが有効か調べる
			@param[in]	h	ハンドル
			@return 有効なら「true」が返る
		*/
		//-----------------------------------------------------------------//
		bool probe(handle_type h) const {
			if(h > 0 && h < limit_) {
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
			if(h > 0 && h < limit_) {
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
			@brief	新規ハンドルを返す
			@return ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		handle_type create() {
			handle_type h;
			if(erase_set_.empty()) {
				if(current_ < limit_) {
					h = current_;
					++current_;
				} else {
					current_ = limit_ - 1;
					h = 0;
				}
			} else {
				set_it it = erase_set_.begin();
				h = *it;
				erase_set_.erase(it);
			}
			return h;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	swap
		*/
		//-----------------------------------------------------------------//
		void swap(handle_set& src) {
			std::swap(src.start_, start_);
			std::swap(src.current_, current_);
			std::swap(src.limit_, limit_);
			src.erase_set_.swap(erase_set_);
		}

	};
}
