#pragma once
//=====================================================================//
/*!	@file
	@brief	階層的構造を管理するクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#ifndef NDEBUG
#define DEBUG_TREE_UNIT_
#endif
#define DEBUG_TREE_UNIT_

#ifdef DEBUG_TREE_UNIT_
#include <iostream>
#endif

#include <algorithm>
#include <stack>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "handle_set.hpp"
#include "string_utils.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ツリー・ユニット・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct tree_unit {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	unit_t 型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		class unit_t {
			uint32_t	id_;

		public:
			typedef std::unordered_set<std::string>	childs;
			typedef childs::iterator		childs_it;
			typedef childs::const_iterator	childs_cit;

		private:
			childs		childs_;

		public:
#ifdef DEBUG_TREE_UNIT_
			void list_all() const {
				std::cout << '(' << static_cast<unsigned int>(id_) << ") ";
				std::cout << static_cast<unsigned int>(childs_.size()) << ' ';
				for(auto s : childs_) {
					std::cout << s << ", ";
				}
			}
#endif
			unit_t() : id_(0), childs_() { }

			uint32_t get_id() const { return id_; }

			void set_id(uint32_t id) { id_ = id; }

			bool find_child(const std::string& key) {
				return childs_.find(key) != childs_.end();
			}

			void erase_child(const std::string& key) {
				childs_.erase(key);
			}
			
			void install_child(const std::string& key) {
				childs_.insert(key);
			}

			bool is_childs_empty() const { return childs_.empty(); }

			const childs& get_childs() const { return childs_; }
		};


		typedef std::pair<std::string, unit_t>	unit_pair;

		typedef std::unordered_map<std::string, unit_t>	unit_map;
		typedef typename unit_map::const_iterator		unit_map_cit;
		typedef typename unit_map::iterator				unit_map_it;

		typedef std::vector<unit_map_cit>				unit_map_cits;
		typedef std::vector<unit_map_it>				unit_map_its;

	private:
		unit_map	   	unit_map_;

		handle_set<uint32_t>	handle_set_;

		typedef std::unordered_map<uint32_t, std::string>	hnd_map;
		typedef typename hnd_map::const_iterator			hnd_map_cit;
		typedef typename hnd_map::iterator					hnd_map_it;
		hnd_map			hnd_map_;

		std::string		current_path_;

		typedef std::stack<std::string>		string_stack;
		string_stack	stack_path_;


		uint32_t install_(const std::string& fpath)
		{
			unit_map_cit cit = unit_map_.find(fpath);
			if(cit != unit_map_.end()) {  // find it !, To be not install.
				return 0;
			}

			auto tpath = strip_last_of_delimita_path(fpath);
			auto bpath = get_file_path(tpath);
			bpath += '/';

			unit_map_it it = unit_map_.find(bpath);
			if(it != unit_map_.end()) {
				unit_t& t = it->second;
				std::string name = get_file_name(tpath);
				if(fpath.back() == '/') name += '/';
				t.install_child(name);
			}

			uint32_t hnd = handle_set_.create();
			unit_t u;
			u.set_id(hnd);
			unit_map_.emplace(fpath, u);
			hnd_map_.emplace(hnd, fpath);
			return hnd;
		}


		bool erase_(const std::string& fpath)
		{
			unit_map_it it = unit_map_.find(fpath);
			if(it == unit_map_.end()) return false;

			auto tpath = strip_last_of_delimita_path(fpath);
			auto bpath = get_file_path(tpath);
			bpath += '/';
			if(fpath.back() == '/') {  // is directory
				unit_t::childs chs = it->second.get_childs();
				for(auto s : chs) {
					std::cout << "Loop: " << (fpath + s) << std::endl;
					erase_(fpath + s);
				}
			}

			{
				unit_map_it it = unit_map_.find(fpath);
				if(it == unit_map_.end()) return false;
				uint32_t hnd = it->second.get_id();
				handle_set_.erase(hnd);
				unit_map_.erase(it);
				hnd_map_.erase(hnd);
			}

			{  // previous directory
				unit_map_it it = unit_map_.find(bpath);
				if(it != unit_map_.end()) {
					std::string s = get_file_name(tpath);
					if(fpath.back() == '/') s += '/';
					it->second.erase_child(s);
				} else {
					return false;
				}
			}

			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	max_handle	最大ハンドル数（標準は６５５３６個）
		*/
		//-----------------------------------------------------------------//
		tree_unit(uint32_t max_handle = 0x10000) : handle_set_(max_handle), current_path_("/") {
			install_("/");
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~tree_unit() { clear(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリア
		*/
		//-----------------------------------------------------------------//
		void clear()
		{
			unit_map_.clear();
			handle_set_.clear();
			hnd_map_.clear();
			current_path_ = "/";
			string_stack a;
			a.swap(stack_path_);
			make_directory("");
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フルパスを生成
			@param[in]	path	パス
			@return フルパス
		*/
		//-----------------------------------------------------------------//
		std::string create_full_path(const std::string& name) const {
			std::string fpath;
			if(name.empty()) return std::move(fpath);
			if(name[0] == '/') {
				return std::move(name);
			}
			fpath += current_path_;
			if(fpath.empty() || fpath.back() != '/') fpath += '/';
			fpath += name;
			return std::move(fpath);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	パスを登録
			@param[in]	path	パス
			@return 成功したらハンドルを返す
		*/
		//-----------------------------------------------------------------//
		uint32_t install(const std::string& path)
		{
			auto fpath = create_full_path(path);
			if(fpath.empty()) {
				return false;
			}

			return install_(strip_last_of_delimita_path(fpath));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	パスを削除（ディレクトリーを指定するとエラー）
			@param[in]	path	パス
			@return 成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool erase(const std::string& path)
		{
			auto fpath = create_full_path(path);
			if(fpath.empty()) {
				return false;
			}

			if(fpath.back() == '/') return false;

			return erase_(fpath);
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
			@brief	カレントのパスを設定
			@param[in]	path	パス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_current_path(const std::string& path)
		{
			if(path.empty()) {
				return false;
			}

			auto tpath = strip_last_of_delimita_path(path);
			if(tpath.empty()) {
				current_path_ = '/';  // root
			} else if(tpath[0] == '/') {  // 絶対パス
				current_path_ = tpath;
			} else {  // 相対パス
				if(current_path_.empty()) current_path_ = '/';
				if(current_path_.back() != '/') current_path_ += '/';
				current_path_ += tpath;
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
			@param[in]	path	ディレクトリー名
			@return 正常なら「０」以上の値
		*/
		//-----------------------------------------------------------------//
		uint32_t make_directory(const std::string& path)
		{
			auto fpath = create_full_path(path);
			if(fpath.empty()) {
				return false;
			}

			if(fpath.back() != '/') fpath += '/';

			return install_(fpath);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーを削除
			@param[in]	path	ディレクトリー名
			@return 正常なら「０」以上の値
		*/
		//-----------------------------------------------------------------//
		uint32_t remove_directory(const std::string& path)
		{
			auto fpath = create_full_path(path);
			if(fpath.empty()) {
				return 0;
			}

			if(fpath.back() != '/') fpath += '/';

			std::cout << "Remove dir: " << fpath << std::endl;

			return erase_(fpath);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	パスを探す
			@param[in]	path	パス
			@return パスのハンドルを返す（無い場合「０」）
		*/
		//-----------------------------------------------------------------//
		uint32_t find(const std::string& path) const {
			auto fpath = create_full_path(path);
			if(fpath.empty()) {
				return 0;
			}

			unit_map_cit cit = unit_map_.find(fpath);
			if(cit != unit_map_.end()) {
				return cit->second.get_id();
			} else {
				return 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ハンドルからパスを得る
			@param[in]	hnd	ハンドル
			@return パスを返す
		*/
		//-----------------------------------------------------------------//
		const std::string& find(uint32_t hnd) const {
			hnd_map_cit cit = hnd_map_.find(hnd);
			if(cit == hnd_map_.end()) {
				static std::string empty;
				return empty;
			}
			return cit->second;
		}


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
				return cit->first.back() == '/';
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーかどうか？
			@param[in]	path	ユニット・キー
			@return ディレクトリーなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_directory(const std::string& path) const
		{
			auto fpath = create_full_path(path);
			if(fpath.empty()) {
				return false;
			}

			if(fpath.back() != '/') fpath += '/';

			return is_directory(unit_map_.find(fpath));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リストを作成する
			@param[in]	root	起点となるルートパス
			@return	リスト
		*/
		//-----------------------------------------------------------------//
		unit_map_its create_list(const std::string& root)
		{
			unit_map_its list;
			if(unit_map_.empty()) {
				list.clear();
				return list;
			}

			if(root.empty()) {
				list.resize(unit_map_.size());
				list.clear();
				for(unit_map_it it = unit_map_.begin(); it != unit_map_.end(); ++it) {
					list.push_back(it);
				}
			} else {
				auto fpath = create_full_path(root);
				if(fpath.empty()) {
					list.clear();
					return list;
				}

				unit_map_it it = unit_map_.find(fpath);
				if(it != unit_map_.end()) {
					const typename unit_t::childs& ch = it->second.get_childs();
					list.resize(ch.size());
					list.clear();
					for(auto s : ch) {
						auto path = create_full_path(s);
						if(!path.empty()) {
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

			std::sort(list.begin(), list.end(), [] (unit_map_it l, unit_map_it r) {
				return l->first < r->first; }
			);

			return list;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	path の取得
			@param[in]	cit	イテレーター
			@return key の参照
		*/
		//-----------------------------------------------------------------//
		const std::string& get_path(unit_map_cit cit) const { return cit->first; }


		//-----------------------------------------------------------------//
		/*!
			@brief	unit の取得
			@param[in]	cit	イテレーター
			@return unit の参照
		*/
		//-----------------------------------------------------------------//
		const unit_t& get_unit(unit_map_cit cit) const { return cit->second; }


#ifdef DEBUG_TREE_UNIT_
		//-----------------------------------------------------------------//
		/*!
			@brief	デバッグ用、全リスト表示
		*/
		//-----------------------------------------------------------------//
		void list(const std::string& root = "")
		{
			unit_map_its its = create_list(root);

			// key の最大長さを探す
			uint32_t ml = 0;
			for(auto it : its) {
				const std::string& key = it->first;
				if(key.size() > ml) ml = key.size();
			}

			for(auto it : its) {
				if(is_directory(it)) {
					std::cout << "d ";
				} else {
					std::cout << "- ";
				}

				const std::string& key = it->first;
				std::cout << key;
				for(uint32_t i = key.size(); i < ml; ++i) {
					std::cout << " ";
				}
				std::cout << " ";
				it->second.list_all();
				std::cout << std::endl;
			}

			int n = its.size();
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
			handle_set_.swap(src.handle_set_);
			hnd_map_.swap(src.hnd_map_);
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
			handle_set_ = src.handle_set_;
			hnd_map_ = src.hnd_map_;
			current_path_ = src.current_path_;
			stack_path_ = src.stack_path_;
			return *this;
		}
	};
}
