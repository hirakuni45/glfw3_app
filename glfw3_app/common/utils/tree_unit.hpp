#pragma once
//=====================================================================//
/*!	@file
	@brief	階層的構造を管理するテンプレート・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#ifndef NDEBUG
#include <iostream>
#endif
#include <algorithm>
#include <stack>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include "utils/string_utils.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ツリー・ユニット・クラス・テンプレート
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T>
	struct tree_unit {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ツリー・ユニット型@n
					「T」型のユーザー定義をアトリビュート情報として付加
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct unit_t {
			T				value;	///< ユーザー利用データ

			typedef boost::unordered_set<std::string>	childs;
			typedef childs::iterator		childs_it;
			typedef childs::const_iterator	childs_cit;

		private:
			uint32_t		id_;

			childs			childs_;

		public:
#ifndef NDEBUG
			void list_all() const {
				value.list_all();
			}
#endif
			unit_t() : id_(0), childs_() { }
			void set_id(uint32_t id) { id_ = id; }
			uint32_t get_id() const { return id_; }
			bool find_child(const std::string& key) {
				return childs_.find(key) != childs_.end();
			}
			void install_child(const std::string& key) {
				childs_.insert(key);
			}
			bool is_childs_empty() const { return childs_.empty(); }
			const childs& get_childs() const { return childs_; }
		};

		typedef boost::optional<const T&>	optional_const_ref;	//< オプショナル参照型(const)
		typedef boost::optional<T&>			optional_ref;		//< オプショナル参照型

		typedef std::pair<std::string, unit_t>	unit_pair;

		typedef boost::unordered_map<std::string, unit_t>	unit_map;
		typedef typename unit_map::const_iterator			unit_map_cit;
		typedef typename unit_map::iterator					unit_map_it;

		typedef std::vector<unit_map_cit>					unit_map_cits;
		typedef std::vector<unit_map_it>					unit_map_its;

	private:
		unit_map	   	unit_map_;

		uint32_t		serial_id_;

		std::string		current_path_;

		typedef std::stack<std::string>		string_stack;
		string_stack	stack_path_;

		bool install_(const std::string& key, const T& value)
		{
			std::string fpath;
			if(!create_full_path(key, fpath)) {
				return false;
			}

			bool f = false;
			utils::strings ss;
			split_text(fpath, "/", ss);
			std::string p;
			for(uint32_t i = 0; i < ss.size(); ++i) {
				p += '/';
				p += ss[i];
				unit_t u;
				u.value = value;
				u.set_id(serial_id_);
				std::pair<unit_map_it, bool> ret = unit_map_.insert(unit_pair(p, u));
				if(ret.second) {
					++serial_id_;
					std::string prev;
					if(utils::get_file_path(p, prev)) {
						unit_map_it it = unit_map_.find(prev);
						if(it != unit_map_.end()) {
							unit_t& t = it->second;
							t.install_child(utils::get_file_name(p));
						}
					}
					f = true;
				} else {
					f = false;
				}
			}
			return f;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		tree_unit() : serial_id_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~tree_unit() { clear(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	シリアルIDを取得
			@return シリアルID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_serial_id() const { return serial_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリア
		*/
		//-----------------------------------------------------------------//
		void clear() {
			unit_map_.clear();

			serial_id_ = 0;

			current_path_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フルパスを生成
			@param[in]	name	ベース名
			@param[in]	fullpath	フル・パスを受け取るコンテナ
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool create_full_path(const std::string& name, std::string& fullpath) const
		{
			if(name.empty()) return false;
			if(name[0] == '/') {
				fullpath = name;
				return true;
			}
			fullpath += current_path_;
			if(fullpath.empty() || fullpath[fullpath.size() - 1] != '/') fullpath += '/';
			fullpath += name;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットを登録
			@param[in]	key		ユニットを識別するキー
			@param[in]	value	値
			@return 成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool install(const std::string& key, const T& value)
		{
			return install_(key, value);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットを削除
			@param[in]	key		ユニットを識別するキー
			@return 成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool erase(const std::string& key)
		{
			std::string fpath;
			if(!create_full_path(key, fpath)) {
				return false;
			}

			unit_map_it it = unit_map_.find(fpath);
			if(it != unit_map_.end()) {
				const unit_t& t = it->second;
				if(t.keys.empty()) {
					unit_map_.erase(it);
				} else {
					unit_map_.erase(it);
					push_current_path();
					current_path_ = fpath;
					for(strings_cit cit = t.keys.begin(); cit != t.keys.end(); ++cit) {
						const std::string& key = *cit;
						std::string fpath;
						if(!create_full_path(key, fpath)) {
							pop_current_path();
							return false;
						}
						erase(fpath);
					}
					pop_current_path();
				}
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを退避
		*/
		//-----------------------------------------------------------------//
		void push_current_path()
		{
			stack_path_.push(current_path_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを復帰
		*/
		//-----------------------------------------------------------------//
		void pop_current_path()
		{
			if(!stack_path_.empty()) {
				current_path_ = stack_path_.top();
				stack_path_.pop();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスをクリア
		*/
		//-----------------------------------------------------------------//
		void clear_current_path()
		{
			current_path_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを設定
			@param[in]	path	パス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_current_path(const std::string& path)
		{
			if(path.empty()) {
				return false;
//			} else if(path == "..") {	// 一つ戻る
//				std::string s = currentPath_;
//				return previous_path(s, currentPath_);
			} else if(path[0] == '/') {	// 絶対パス
				current_path_ = path;
			} else {	// 相対パス
				if(current_path_[current_path_.size() - 1] != '/') current_path_ += '/';
				current_path_ += path;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレント・パスを得る
			@return カレントパス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_current_path() const { return current_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーを作成
			@param[in]	name	ディレクトリー名
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool make_directory(const std::string& name)
		{
			T value;
			return install_(name, value);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットを探す
			@param[in]	key	ユニット・キー
			@return ユニットがあれば「true」
		*/
		//-----------------------------------------------------------------//
		bool find(const std::string& key) const {
			std::string fullpath;
			if(!create_full_path(key, fullpath)) {
				return false;
			}

			unit_map_it it = unit_map_.find(fullpath);
			if(it == unit_map_.end()) {
				return false;
			} else {
				return true;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットを取得
			@param[in]	key	ユニット・キー
			@return ユニット・オプショナル型を返す
		*/
		//-----------------------------------------------------------------//
		optional_const_ref get(const std::string& key) const
		{
			std::string fpath;
			if(!create_full_path(key, fpath)) {
				return boost::none;
			}

			unit_map_cit cit = unit_map_.find(fpath);
			if(cit == unit_map_.end()) {
				return boost::none;
			} else {
				return optional_const_ref(cit->second.value);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーかどうか？
			@param[in]	key	ユニット・キー
			@return ディレクトリーなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_directory(const std::string& key) const
		{
			std::string fpath;
			if(!create_full_path(key, fpath)) {
				return false;
			}

			unit_map_it it = unit_map_.find(fpath);
			if(it != unit_map_.end()) {
				return !(it->second.is_childs_empty());
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リストを作成する
			@param[in]	root	起点となるルートパス
			@param[out]	list	リスト
			@param[in]	id_sort	ID 順（登録順）でソートする場合「true」
		*/
		//-----------------------------------------------------------------//
		void create_list(const std::string& root, unit_map_its& list, bool id_sort = false)
		{
			if(unit_map_.empty()) {
				list.clear();
				return;
			}

			if(root.empty()) {
				list.reserve(unit_map_.size());
				for(unit_map_it it = unit_map_.begin(); it != unit_map_.end(); ++it) {
					list.push_back(it);
				}
			} else {
				std::string fullpath;
				if(!create_full_path(root, fullpath)) {
					list.clear();
					return;
				}

				unit_map_it it = unit_map_.find(fullpath);
				if(it != unit_map_.end()) {
					const typename unit_t::childs& ch = it->second.get_childs();
					list.resize(ch.size());
					list.clear();
					BOOST_FOREACH(const std::string& s, ch) {
						std::string path;
						if(create_full_path(s, path)) {
							it = unit_map_.find(path);
							if(it != unit_map_.end()) {
								list.push_back(it);
							}
						}
					}
				} else {
					list.clear();
				}
			}

			if(id_sort) {
				std::sort(list.begin(), list.end(), [] (unit_map_it l, unit_map_it r) {
					return l->second.get_id() < r->second.get_id(); }
				);
			} else {
				std::sort(list.begin(), list.end(), [] (unit_map_it l, unit_map_it r) {
					return l->first < r->first; }
				);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	key の取得
			@param[in]	cit	イテレーター
			@return key の参照
		*/
		//-----------------------------------------------------------------//
		const std::string& get_key(unit_map_cit cit) const { return cit->first; }


		//-----------------------------------------------------------------//
		/*!
			@brief	unit の取得
			@param[in]	cit	イテレーター
			@return unit の参照
		*/
		//-----------------------------------------------------------------//
		const unit_t& get_unit(unit_map_cit cit) const { return cit->second; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーかどうか？
			@param[in]	cit	イテレーター
			@return ディレクトリーなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_directory(unit_map_cit cit) const
		{
			if(cit != unit_map_.end()) {
				return !(cit->second.is_childs_empty());
			} else {
				return false;
			}
		}


#ifndef NDEBUG
		//-----------------------------------------------------------------//
		/*!
			@brief	デバッグ用、全リスト表示
		*/
		//-----------------------------------------------------------------//
		void list(const std::string& root = "") const
		{
			unit_map_cits cits;
			create_list(root, cits);

			// key の最大長さを探す
			uint32_t ml = 0;
			BOOST_FOREACH(unit_map_cit cit, cits) {
				const std::string& key = cit->first;
				if(key.size() > ml) ml = key.size();				
			}

			BOOST_FOREACH(unit_map_cit cit, cits) {
				const unit_t& t = cit->second;
				if(t.is_childs_empty()) {
					std::cout << "- ";
				} else {
					std::cout << "d ";
				}

				const std::string& key = cit->first;
				std::cout << key;
				for(uint32_t i = key.size(); i < ml; ++i) {
					std::cout << " ";
				}
				std::cout << " ";
				t.list_all();
				std::cout << std::endl;
			}

			int n = cits.size();

			std::cout << "Total " << n << " file";
			std::cout << ((n > 1) ? "s" : "") << std::endl << std::endl;
		}
#endif

		//-----------------------------------------------------------------//
		/*!
			@brief	swap
		*/
		//-----------------------------------------------------------------//
		void swap(tree_unit& src) {
			unit_map_.swap(src.unit_map_);
			std::swap(serial_id_, src.serial_id_);
			current_path_.swap(src.current_path_);
			stack_path_.swap(src.stack_path_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	＝オペレーター
		*/
		//-----------------------------------------------------------------//
		tree_unit& operator = (const tree_unit& src) {
			unit_map_ = src.unit_map_;
			serial_id_ = src.serial_id_;
			current_path_ = src.current_path_;
			stack_path_ = src.stack_path_;
			return *this;
		} 
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	標準的な文字情報を扱える tree_unit クラスの型設定
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	typedef tree_unit<std::string> tree_unit_string;

}
