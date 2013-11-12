#pragma once
//=====================================================================//
/*!	@file
	@brief	プリファレンス・クラス（ヘッダー）@n
			・アプリケーションの状態を記録する（Maybe レジストリー）@n
			・テキストファイルとして記録されるので汎用性が高い
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <stack>
#include <map>
#include <boost/format.hpp>
#include "utils/vtx.hpp"
#include "utils/file_io.hpp"

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
		struct value {
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			/*!
				@brief	タイプ
			*/
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			enum type {
				invalid = -1,
				int32 = 0,
				float32,
				text,
				position_int32,
				position_float32,
				boolean,
			};
		};

	private:
		struct value_t {
			value::type		type_;
			std::string		value_;
			value_t() { }
			value_t(value::type t, const std::string& v) : type_(t), value_(v) { }
		};

		typedef std::pair<std::string, value_t>					item_pair;
		typedef std::map<std::string, value_t>::iterator		item_it;
		typedef std::map<std::string, value_t>::const_iterator	item_cit;
		typedef std::pair<item_it, bool>						item_ret;
		std::map<std::string, value_t>	map_;

		std::string						path_;
		std::stack<std::string>			stack_path_;

		void get_full_path_(const std::string& name, std::string& out) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	アイテムを追加
			@param[in]	key	キーワード
			@param[in]	vt	値の構造体
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_(const std::string& key, const value_t& vt);


		//-----------------------------------------------------------------//
		/*!
			@brief	アイテムを取得
			@param[in]	key	キーワード
			@param[in]	vt	値の構造体
			@return 取得されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool get_(const std::string& key, value_t& vt) const;

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
		void set_current_path(const std::string& path);


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
		void create_current_list(utils::strings& ss);


		//-----------------------------------------------------------------//
		/*!
			@brief	キーワードを検索
			@param[in]	key	キーワード
			@return キーワードがあれば「true」
		*/
		//-----------------------------------------------------------------//
		bool find(const std::string& key) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	キーワードのアイテムを消去
			@param[in]	key	キーワード
			@return 消去が成功すれば「true」
		*/
		//-----------------------------------------------------------------//
		bool erase(const std::string& key);


		//-----------------------------------------------------------------//
		/*!
			@brief	キーワードのタイプを取得
			@param[in]	key	キーワード
			@return キーワードのタイプ
		*/
		//-----------------------------------------------------------------//
		value::type get_type(const std::string& key) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	テキスト・アイテムを追加
			@param[in]	key	キーワード
			@param[in]	v	テキスト
			@return 追加されれば「true」
		*/
		//-----------------------------------------------------------------//
		bool put_text(const std::string& key, const std::string& v) {
			value_t vt(value::text, v);
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
			value_t vt(value::boolean, t);
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
			value_t vt(value::int32, t);
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
			std::string t = boost::io::str( boost::format("%1%") % v );
			value_t vt(value::float32, t);
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
			value_t vt(value::position_int32, t);
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
			value_t vt(value::position_float32, t);
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
				if(vt.type_ == value::text) {
					v = vt.value_;
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
				if(vt.type_ == value::boolean) {
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
				if(vt.type_ == value::int32) {
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
				if(vt.type_ == value::float32) {
					float i;
					if(sscanf(vt.value_.c_str(), "%g", &i) == 1) {
						v = i;
						return true;
					}
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
				if(vt.type_ == value::position_int32) {
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
				if(vt.type_ == value::position_float32) {
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
			@brief	設定をロード
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& filename);


		//-----------------------------------------------------------------//
		/*!
			@brief	設定をセーブ
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& filename);

	};

}

