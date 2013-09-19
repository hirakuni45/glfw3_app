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
#include <stack>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
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
			std::string		name;	///< ユニット名
			T				value;	///< ユーザー利用データ
		private:
			uint32_t		id_;

			typedef boost::unordered_set<std::string>	childs;
			typedef childs::iterator		childs_it;
			childs			childs_;	///< 子ユニットの key

		public:
#ifndef NDEBUG
			void list_all() const
			{
				if(!name.empty()) {
					std::cout << "(" << name << "), ";
				} else {
					std::cout << "(), ";
				}
				value.list_all();
			}
#endif
			unit_t() : id_(0) { }
			void set_id(uint32_t id) { id_ = id; }
			uint32_t get_id() const { return id_; }
			void install_child(const std::string& key) {
				childs_.insert(key);
			}
			bool is_childs_empty() const { return childs_.empty(); }
		};

		typedef boost::optional<const T&>	optional_const_ref;	//< オプショナル参照型(const)
		typedef boost::optional<T&>			optional_ref;		//< オプショナル参照型

	private:
		typedef std::pair<std::string, unit_t>	unit_pair;
		typedef boost::unordered_map<std::string, unit_t> unit_map;
		typedef typename unit_map::iterator			unit_map_it;
		typedef typename unit_map::const_iterator	unit_map_cit;

		unit_map	   	unit_map_;

		uint32_t		serial_id_;
		uint32_t		units_;
		uint32_t		directory_;

		std::string		current_path_;

		typedef std::stack<std::string>		string_stack;
		string_stack	stack_path_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		tree_unit() : serial_id_(0), units_(0), directory_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~tree_unit() { }


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
			@param[in]	same_name	キーと名前を同一にする場合「true」
			@return 成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool install(const std::string& key, const T& value, bool same_name = false)
		{
			std::string fullpath;
			if(!create_full_path(key, fullpath)) {
				return false;
			}

			unit_t u;
			u.value = value;
			if(same_name) {
				u.name = get_file_name(key);
			}
			u.set_id(serial_id_);

			std::pair<unit_map_it, bool> ret = unit_map_.insert(unit_pair(fullpath, u));
			if(ret.second) {
				++serial_id_;
				++units_;
				// ディレクトリー情報にユニット名を追加
				std::string d;
				if(get_file_path(fullpath, d)) {
					unit_map_it it = unit_map_.find(d);
					if(it == unit_map_.end()) {
						return false;
					} else {
						unit_t& t = it->second;
						t.install_child(key);
					}
				}
			}
			return ret.second;
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
			std::string fullpath;
			if(!create_full_path(key, fullpath)) {
				return false;
			}

			unit_map_it it = unit_map_.find(fullpath);
			if(it != unit_map_.end()) {
				const unit_t& t = it->second;
				if(t.keys.empty()) {
					unit_map_.erase(it);
					--units_;
				} else {
					unit_map_.erase(it);
					push_current_path();
					current_path_ = fullpath;
					for(strings_cit cit = t.keys.begin(); cit != t.keys.end(); ++cit) {
						const std::string& key = *cit;
						std::string fullpath;
						if(!create_full_path(key, fullpath)) {
							pop_current_path();
							return false;
						}
						erase(fullpath);
					}
					pop_current_path();
					--directory_;
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
			current_path_ = stack_path_.top();
			stack_path_.pop();
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
			@param[in]	same_name	名前にコピーする場合「true」
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool make_directory(const std::string& name, bool same_name = false)
		{
			std::string fullpath;
			if(!create_full_path(name, fullpath)) {
				return false;
			}

			unit_t t;
			if(same_name) {
				t.name = get_file_name(name);
			}
			t.set_id(serial_id_);

			std::pair<unit_map_it, bool> ret = unit_map_.insert(unit_pair(fullpath, t));
			if(ret.second) {
				++serial_id_;
				++directory_;
				return true;
			} else {
				// 既に同じディレクトリーが存在した。
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ユニットを取得
			@param[in]	key	ユニット・キー
			@return ユニット・オプショナル型を返す
		*/
		//-----------------------------------------------------------------//
		optional_const_ref get(const std::string& key)
		{
			std::string fullpath;
			if(!create_full_path(key, fullpath)) {
				return boost::none;
			}

			unit_map_it it = unit_map_.find(fullpath);
			if(it == unit_map_.end()) {
				return boost::none;
			} else {
				return optional_const_ref(it->second);
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
			std::string fullpath;
			if(!create_full_path(key, fullpath)) {
				return false;
			}

			unit_map_it it = unit_map_.find(fullpath);
			if(it != unit_map_.end()) {
				return !(it->second.is_childs_empty());
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キーリストを作成する
			@param[in]	root	起点となるルート
			@param[out]	ss		キーリストを受け取るコンテナ
		*/
		//-----------------------------------------------------------------//
		void create_key_list(const std::string& root, strings& ss)
		{
			ss.resize(unit_map_.size());
			ss.clear();
			for(unit_map_cit cit = unit_map_.begin(); cit != unit_map_.end(); ++cit) {
				const std::string& key = cit->first;
				if(root.empty()) {
					ss.push_back(key);
				} else if(strncmp(root.c_str(), key.c_str(), root.size()) == 0) {
					ss.push_back(key);
				}
			}			
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			unit_map_.clear();

			serial_id_ = 0;
			units_ = 0;
			directory_ = 0;

			current_path_.clear();
		}


#ifndef NDEBUG
		//-----------------------------------------------------------------//
		/*!
			@brief	デバッグ用、全リスト表示
		*/
		//-----------------------------------------------------------------//
		void list_all() const
		{
			// key の最大長さを探す
			unsigned int ml = 0;
			for(unit_map_cit cit = unit_map_.begin(); cit != unit_map_.end(); ++cit) {
				const std::string& key = cit->first;
				if(key.size() > ml) ml = key.size();
			}

			for(unit_map_cit cit = unit_map_.begin(); cit != unit_map_.end(); ++cit) {
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
			int n = unit_map_.size();

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
			std::swap(units_, src.units_);
			std::swap(directory_, src.directory_);
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
			units_ = src.units_;
			directory_ = src.directory_;
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
