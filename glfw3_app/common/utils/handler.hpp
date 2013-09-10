// -*-tab-width:4-*-
#pragma once
//=====================================================================//
/*!	@file
	@brief	ハンドルを使ってリソース管理を行うテンプレート・クラス@n
			・ベクターを使い、小分けにして管理する事で、再配置による@n
			パフォーマンス低下を防ぐ。@n
			・削除したハンドルは、「set」により別管理する事で、再利用@n
			が発生した場合のコストを軽減する。
			・以上の仕組みで、新規ハンドルの作成、削除を大量に行った@n
			場合のパフォーマンス低下を最小限にする。
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#ifndef NDEBUG
#include <iostream>
#endif
#include <vector>
#include <boost/optional.hpp>
#include <boost/unordered_set.hpp>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ハンドル管理テンプレート・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T>
	struct Handler {

		typedef unsigned int	handle;		//< ハンドル

		typedef boost::optional<const T&>	optional_const_ref;	//< オプショナル参照型(const)
		typedef boost::optional<T&>			optional_ref;		//< オプショナル参照型

	private:
		typedef std::vector<T>					containeres_unit;
		typedef std::vector<containeres_unit>	containeres;
		typedef typename containeres::iterator	containeres_it;
		containeres								containeres_;

		typedef boost::unordered_set<handle>	delete_handles;
		typedef delete_handles::iterator		delete_handles_it;
		delete_handles		delete_handles_;

		size_t				unit_limit_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		Handler() : unit_limit_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~Handler() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	unit_n	単位配列のリミットサイズ@n
								※１以上の正の数（パフォーマンスに影響する為慎重に選ぶ事）
		*/
		//-----------------------------------------------------------------//
		void initialize(size_t unit_n) {
			unit_limit_ = unit_n;
			containeres_unit u;
			containeres_.push_back(u);
			containeres_unit& unit = containeres_.back();
			unit.reserve(unit_limit_);
			delete_handles_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	使用サイズを返す
			@return 使用サイズ
		*/
		//-----------------------------------------------------------------//
		inline size_t size() const {
			const containeres_unit& unit = containeres_.back();
			size_t n = unit.size();
			if(containeres_.size() > 1) {
				n += unit_limit_ * (containeres_.size() - 1);
			}
			return n - delete_handles_.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルが有効か調べる
			@param[in]	h	ハンドル
			@return 有効なら「true」が返る
		*/
		//-----------------------------------------------------------------//
		inline bool probe(handle h) const {
			if(h < size()) {
				return (delete_handles_.find(h) == delete_handles_.end());
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	保持可能な大きさを返す
			@return 保持可能サイズ
		*/
		//-----------------------------------------------------------------//
		size_t capacity() const {
			return containeres_.size() * unit_limit_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	インスタンスを取得（読み出し専用）
			@param[in]	h	ハンドル
			@return type_optional 型で返す
		*/
		//-----------------------------------------------------------------//
		inline optional_const_ref get(handle h) const {
			if(probe(h)) {
				const containeres_unit& u = containeres_[h / unit_limit_];
				return optional_const_ref(u[h % unit_limit_]);
			} else {
				return boost::none;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	インスタンスを取得
			@param[in]	h	ハンドル
			@return type_optional 型で返す
		*/
		//-----------------------------------------------------------------//
		inline optional_ref at(handle h) {
			if(probe(h)) {
				containeres_unit& u = containeres_[h / unit_limit_];
				return optional_ref(u[h % unit_limit_]);
			} else {
				return boost::none;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルを消去
			@param[in]	h	ハンドル
			@return 消去できたら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool erase(handle h) {
			if(h < size()) {
				std::pair<delete_handles_it, bool> ret = delete_handles_.insert(h);
				return ret.second;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	インスタンスを挿入してハンドルを返す
			@param[in]	v	インスタンス
			@return ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		handle insert(const T& v) {
			if(delete_handles_.empty()) {
				containeres_unit& unit = containeres_.back();
				if(unit.size() < unit_limit_) {
					handle h = (containeres_.size() - 1) * unit_limit_ + unit.size();
					unit.push_back(v);
					return h;
				} else {
					containeres_unit u;
					containeres_.push_back(u);
					containeres_unit& unit = containeres_.back();
					unit.reserve(unit_limit_);
					handle h = (containeres_.size() - 1) * unit_limit_ + unit.size();
					unit.push_back(v);
					return h;
				}
			} else {
				delete_handles_it it = delete_handles_.begin();
				handle h = *it;
				containeres_unit& unit = containeres_[h / unit_limit_];
				unit[h % unit_limit_] = v;
				delete_handles_.erase(it);
				return h;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	明示的な廃棄@n
					※初期化を呼ぶ必要がある
		*/
		//-----------------------------------------------------------------//
		void destroy() {
			for(containeres_it it = containeres_.begin(); it != containeres_.end(); ++it) {
				containeres_unit& unit = *it;
				containeres_unit().swap(unit);
			}
			containeres().swap(containeres_);
			delete_handles_.clear();
		}


#ifndef NDEBUG
		//-----------------------------------------------------------------//
		/*!
			@brief	管理状態をリストする
		*/
		//-----------------------------------------------------------------//
		void list() {
			using namespace std;
			cout << "Handler class size: " << sizeof(*this) << endl;
			cout << "Handler container size: " << sizeof(T) << endl;
			cout << endl;
			cout << "Handler size: " << size() << endl;
			cout << "Handler capacity: " << capacity() << endl;
			int i = 0;
			for(handle h = 0; h < size(); ++h) {
				if(probe(h)) {
					cout << "Active handle: " << h << endl;
					++i;
				}
			}
			cout << "Total active handle: " << i << endl;
		}
#endif

	};

}
