//=====================================================================//
/*!	@file
	@brief	プリファレンス・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/preference.hpp"
#include <boost/foreach.hpp>
#include <iostream>
#include "utils/string_utils.hpp"

namespace sys {

	using namespace std;

	void preference::get_full_path_(const std::string& name, std::string& out) const
	{
		if(name.empty()) return;

		string s;
		if(name[0] == '/') {
			s = name;
		} else {
			s = path_;
			s += name;
		}
		utils::code_conv(s, ' ', '_', out);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アイテムを取得
		@param[in]	key	キーワード
		@param[in]	vt	値の構造体
		@return 取得されれば「true」
	*/
	//-----------------------------------------------------------------//
	bool preference::get_(const std::string& key, value_t& vt) const
	{
		string s;
		get_full_path_(key, s);
		item_map::const_iterator cit = map_.find(s);
		if(cit == map_.end()) {
			return false;
		} else {
			vt = cit->second;
			return true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アイテムを追加
		@param[in]	key	キーワード
		@param[in]	vt	値の構造体
		@return 追加されれば「true」
	*/
	//-----------------------------------------------------------------//
	bool preference::put_(const std::string& key, const value_t& vt)
	{
		string s;
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
		@brief	カレントパスを設定
		@param[in]	path	パス
	*/
	//-----------------------------------------------------------------//
	void preference::set_current_path(const std::string& path)
	{
		if(path.empty()) return;

		string s;
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
		@brief	カレントのアイテムリストを作成
		@param[in]	ss	キーリストを受け取るコンテナ
	*/
	//-----------------------------------------------------------------//
	void preference::create_current_list(utils::strings& ss)
	{
		BOOST_FOREACH(const item_map::value_type& t, map_) {
			const string& s = t.first;
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
	bool preference::find(const std::string& key) const
	{
		string s;
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
	bool preference::erase(const std::string& key)
	{
		string s;
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
	preference::vtype preference::get_type(const std::string& key) const
	{
		vtype t = vtype::invalid;
		string s;
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
		@brief	標準的な rect 構造の取得
		@param[in]	key	ベースとなるキーワード
		@return エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	bool preference::load_rect(const std::string& key, vtx::srect& rect)
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
	bool preference::save_rect(const std::string& key, const vtx::srect& rect)
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
	bool preference::load(const std::string& filename)
	{
		utils::file_io inp;
		if(!inp.open(filename, "rb")) {
			return false;
		}

		uint32_t err = 0;
		string s;
		while(inp.get_line(s) == true) {
			if(s.empty()) continue;
			if(s[0] == '#') ;  // コメント行は無視
			else {
				utils::strings ss = utils::split_text(s, " \t", 3);
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
			s.clear();
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
	bool preference::save(const std::string& filename)
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
