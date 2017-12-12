#pragma once
//=====================================================================//
/*!	@file
	@brief	プリファレンス・クラス（ヘッダー）@n
			・アプリケーションの状態を記録する（Maybe レジストリー）@n
			・テキストファイルとして記録されるので汎用性が高い @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <stack>
#include <map>
#include "utils/vtx.hpp"
#include "utils/file_io.hpp"
#include <boost/format.hpp>

namespace sys {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	プリファレンス・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct preference {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	プリファレンス値型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class vtype {
			invalid = -1,
			int32 = 0,
			float32,
			text,
			position_int32,
			position_float32,
			boolean,
		};

	private:
		struct value_t {
			vtype			vtype_;
			std::string		value_;
			value_t() { }
			explicit value_t(vtype t, const std::string& v) : vtype_(t), value_(v) { }
		};

		typedef std::map<std::string, value_t>	item_map;
		typedef std::pair<item_map::iterator, bool>  item_ret;
		item_map					map_;

		std::string					path_;
		std::stack<std::string>		stack_path_;

		void get_full_path_(const std::string& name, std::string& out) const
		{
			if(name.empty()) return;

			std::string s;
			if(name[0] == '/') {
				s = name;
			} else {
				s = path_;
				s += name;
			}
			utils::code_conv(s, ' ', '_', out);
		}


		const std::string uint_to_hexs_(uint32_t val) const
		{
			char tmp[9];
			int i = sizeof(tmp) - 1;
			tmp[i] = 0;
			do {
				--i;
				uint8_t nib = val & 15;
				val >>= 4;
				if(nib < 10) tmp[i] = '0' + nib;
				else tmp[i] = 'A' + nib - 10;
			} while(val != 0) ;
			std::string s = &tmp[i];
			return s;
		}

		uint32_t hexs_to_uint_(const std::string& s) const {
			uint32_t v = 0;
			for(char ch : s) {
				v <<= 4;
				if(ch >= '0' && ch <= '9') v |= (ch - '0');
				else if(ch >= 'A' && ch <= 'F') v |= (ch - 'A') + 10;
				else if(ch >= 'a' && ch <= 'f') v |= (ch - 'a') + 10;
			}
			return v;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アイテムを追加
			@param[in]	key	キーワード
			@param[in]	vt	値の構造体
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_(const std::string& key, const value_t& vt)
		{
			std::string s;
			get_full_path_(key, s);
			item_map::iterator it = map_.find(s);
			if(it == map_.end()) {
				item_map::value_type pa(s, vt);
				item_ret r = map_.insert(pa);
				return r.second;
			} else {
				it->second = vt;
				return true;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アイテムを取得
			@param[in]	key	キーワード
			@param[in]	vt	値の構造体
			@return 取得されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_(const std::string& key, value_t& vt) const
		{
			std::string s;
			get_full_path_(key, s);
			item_map::const_iterator cit = map_.find(s);
			if(cit == map_.end()) {
				return false;
			} else {
				vt = cit->second;
				return true;
			}
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		preference() : path_("/") { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~preference() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントパスを設定
			@param[in]	path	パス
		*/
		//-----------------------------------------------------------------//
		void set_current_path(const std::string& path)
		{
			if(path.empty()) return;

			std::string s;
			utils::code_conv(path, ' ', '_', s);

			if(s[0] == '/') {
				path_ = s;
			} else {
				path_ += s;
			}
			if(path_[path_.size() - 1] != '/') {
				path_ += '/';
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントパスを取得
			@return カレント・パス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_current_path() const { return path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを退避
		*/
		//-----------------------------------------------------------------//
		void push_current_path() { stack_path_.push(path_); }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのパスを復帰
		*/
		//-----------------------------------------------------------------//
		void pop_current_path() { path_ = stack_path_.top(); stack_path_.pop(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのアイテムリストを作成
			@param[in]	ss	キーリストを受け取るコンテナ
		*/
		//-----------------------------------------------------------------//
		void create_current_list(utils::strings& ss)
		{
			for(const item_map::value_type& t : map_) {
				const std::string& s = t.first;
				if(utils::string_strncmp(s, path_, path_.size()) == 0) {
					ss.push_back(s);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キーワードを検索
			@param[in]	key	キーワード
			@return キーワードがあれば「true」
		*/
		//-----------------------------------------------------------------//
		bool find(const std::string& key) const
		{
			std::string s;
			get_full_path_(key, s);
			item_map::const_iterator cit = map_.find(s);
			if(cit == map_.end()) {
				return false;
			} else {
				return true;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キーワードのアイテムを消去
			@param[in]	key	キーワード
			@return 消去が成功すれば「true」
		*/
		//-----------------------------------------------------------------//
		bool erase(const std::string& key)
		{
			std::string s;
			get_full_path_(key, s);
			size_t sz = map_.size();
			size_t n = map_.erase(s);
			if(sz == n) return false;
			else return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キーワードのタイプを取得
			@param[in]	key	キーワード
			@return キーワードのタイプ
		*/
		//-----------------------------------------------------------------//
		vtype get_type(const std::string& key) const
		{
			vtype t = vtype::invalid;
			std::string s;
			get_full_path_(key, s);
			item_map::const_iterator cit = map_.find(s);
			if(cit != map_.end()) {
				const value_t& vt = cit->second;
				t = vt.vtype_;
			}
			return t;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキスト・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	v	テキスト
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_text(const std::string& key, const std::string& v) {
			std::string s;
			s += '"';
			s += v;
			s += '"';
			value_t vt(vtype::text, s);
			return put_(key, vt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ブーリアン・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	v	「true」か「false」
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_boolean(const std::string& key, bool v) {
			std::string t;
			if(v) t = boost::io::str( boost::format("%1%") % "1" );
			else t = boost::io::str( boost::format("%1%") % "0" );
			value_t vt(vtype::boolean, t);
			return put_(key, vt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	整数・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	v	整数
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_integer(const std::string& key, int v) {
			std::string t = boost::io::str( boost::format("%1%") % v );
			value_t vt(vtype::int32, t);
			return put_(key, vt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	浮動小数点・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	v	整数
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_real(const std::string& key, float v) {
			const uint32_t* p = reinterpret_cast<const uint32_t*>(&v);
			value_t vt(vtype::float32, uint_to_hexs_(*p));
			return put_(key, vt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	整数位置・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	pos	整数位置
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_position(const std::string& key, const vtx::ipos& pos) {
			std::string t = boost::io::str( boost::format("%d,%d") % pos.x % pos.y );
			value_t vt(vtype::position_int32, t);
			return put_(key, vt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	浮動小数点位置・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	pos	浮動小数点位置
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_position(const std::string& key, const vtx::fpos& pos) {
			std::string t = boost::io::str( boost::format("%f,%f") % pos.x % pos.y );
			value_t vt(vtype::position_float32, t);
			return put_(key, vt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキスト・アイテムを取得
			@param[in]	key	キーワード
			@param[in]	v	テキスト
			@return 取得できれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_text(const std::string& key, std::string& v) const {
			value_t vt;
			if(get_(key, vt)) {
				if(vt.vtype_ == vtype::text) {
					if(vt.value_.size() >= 2 && vt.value_[0] == '"' && vt.value_.back() == '"') {
						v = vt.value_.substr(1, vt.value_.size() - 2);
					} else {
						v = vt.value_;
					}
					return true;
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ブーリアン・アイテムを取得
			@param[in]	key	キーワード
			@param[in]	v	「true」か「false」
			@return 取得できれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_boolean(const std::string& key, bool& v) const {
			value_t vt;
			if(get_(key, vt)) {
				if(vt.vtype_ == vtype::boolean) {
					int i;
					if(sscanf(vt.value_.c_str(), "%d", &i) == 1) {
						if(i) v = true; else v = false;
						return true;
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	整数・アイテムを取得
			@param[in]	key	キーワード
			@param[in]	v	整数
			@return 取得できれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_integer(const std::string& key, int& v) const {
			value_t vt;
			if(get_(key, vt)) {
				if(vt.vtype_ == vtype::int32) {
					int i;
					if(sscanf(vt.value_.c_str(), "%d", &i) == 1) {
						v = i;
						return true;
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	浮動小数点・アイテムを取得
			@param[in]	key	キーワード
			@param[in]	v	整数
			@return 取得できれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_real(const std::string& key, float& v) const {
			value_t vt;
			if(get_(key, vt)) {
				if(vt.vtype_ == vtype::float32) {
					const uint32_t iv = hexs_to_uint_(vt.value_);
					const float* p = reinterpret_cast<const float*>(&iv);
					v = *p;
					return true;
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	整数位置・アイテムを取得
			@param[in]	key	キーワード
			@param[in]	pos	整数位置
			@return 取得できれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_position(const std::string& key, vtx::ipos& pos) const {
			value_t vt;
			if(get_(key, vt)) {
				if(vt.vtype_ == vtype::position_int32) {
					int x, y;
					if(sscanf(vt.value_.c_str(), "%d,%d", &x, &y) == 2) {
						pos.set(x, y);
						return true;
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	浮動小数点位置・アイテムを取得
			@param[in]	key	キーワード
			@param[in]	pos	浮動小数点位置
			@return 取得できれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_position(const std::string& key, vtx::fpos& pos) {
			value_t vt;
			if(get_(key, vt)) {
				if(vt.vtype_ == vtype::position_float32) {
					float x, y;
					if(sscanf(vt.value_.c_str(), "%g,%g", &x, &y) == 2) {
						pos.set(x, y);
						return true;
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	標準的な rect 構造の取得
			@param[in]	key	ベースとなるキーワード
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool load_rect(const std::string& key, vtx::srect& rect)
		{
			vtx::ipos org(-1);
			if(!get_position(key + "/locate", org)) {
				return false;
			}
			if(org.x < 0 || org.y < 0) {
				return false;
			}

			vtx::ipos size(0);
			if(!get_position(key + "/size", size)) {
				return false;
			}
			if(size.x <= 0 || size.y <= 0) {
				return false;
			}

			rect.org = org;
			rect.size = size;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	標準的な rect 構造の取得
			@param[in]	key	ベースとなるキーワード
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool save_rect(const std::string& key, const vtx::srect& rect)
		{
			if(rect.org.x < 0 || rect.org.y < 0) {
				return false;
			}
			vtx::ipos org = rect.org;
			if(rect.size.x <= 0 || rect.size.y <= 0) {
				return false;
			}
			vtx::ipos size = rect.size;
			put_position(key + "/locate", org);
			put_position(key + "/size", size);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	設定をロード
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& filename)
		{
			utils::file_io inp;
			if(!inp.open(filename, "rb")) {
				return false;
			}

			uint32_t err = 0;
			while(!inp.eof()) {
				auto s = inp.get_line();
				if(s.empty()) continue;
				if(s[0] == '#') ;  // コメント行は無視
				else {
					utils::strings ss = utils::split_text(s, " \t", "", 3);
					if(ss.size() == 3) {
						int n;
						if(utils::string_to_int(ss[1], n)) {
							value_t vt(static_cast<vtype>(n), ss[2]);
							put_(ss[0], vt);
						} else {
							++err;
						}
					}
				}
			}
			inp.close();

			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	設定をセーブ
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& filename)
		{
			utils::file_io out;
			if(!out.open(filename, "wb")) {
				return false;
			}

			for(item_map::const_iterator cit = map_.begin(); cit != map_.end(); ++cit) {
				out.put(cit->first);
				out.put_char(' ');
				out.put_char(0x30 + static_cast<int>(cit->second.vtype_));
				out.put_char(' ');
				out.put(cit->second.value_);
				out.put_char('\n');
			}
			out.close();

			return true;
		}
	};
}

