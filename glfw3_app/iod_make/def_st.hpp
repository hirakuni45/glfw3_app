#pragma once
//=====================================================================//
/*!	@file
	@brief	設定ファイル構造体 @n
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <unordered_map>
#include <vector>
#include "utils/string_utils.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	基本・解析クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class analize {

	public:
		typedef std::unordered_map<std::string, utils::strings> opr_map;
		typedef opr_map::iterator opr_map_it;

	private:
		opr_map		opr_map_;
		opr_map_it	map_it_;
		opr_map_it	back_it_;
		std::vector<opr_map_it>	list_;

		std::string text_filter_(const std::string& s) {
			if(s.empty()) return "";
			auto t = utils::strip_char(s, ",");
			if(t.size() > 2) {
				auto ch = t.front();
				if((ch == '"' || ch == '\'') && ch == t.back()) {
					return t.substr(1, t.size() - 2);
				}
			}
			return t;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	tbl	登録コマンド列
		*/
		//-----------------------------------------------------------------//
		analize(const std::string& tbl) {
			if(!tbl.empty()) {
				auto ss = utils::split_text(tbl, ", \t");
				for(const auto& s : ss) {
					auto ret = opr_map_.emplace(s, utils::strings());
					list_.push_back(ret.first);
				}
			}
			map_it_ = opr_map_.end();
			back_it_ = opr_map_.end();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コマンド数の取得
			@return コマンド数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_num() const { return list_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列を注入
			@param[in]	s	文字列
			@return エラーの場合、エラー文字列
		*/
		//-----------------------------------------------------------------//
		std::string injection(const std::string& s)
		{
			if(s.empty()) return "";

			if(map_it_ == opr_map_.end()) {
				if(s.front() == ',' && back_it_ != opr_map_.end()) {
					map_it_ = back_it_;
					return injection(s.substr(1));
				}
				auto it = opr_map_.find(s);
				if(it == opr_map_.end()) {
					return "Invalid error: '" + s + "'";
				}
				it->second.clear();
				map_it_ = it;
				back_it_ = opr_map_.end();
			} else {
				if(s == ",") {
					if(map_it_->second.empty()) return "Parameter feed error.";
					else return "";
				}
				bool next = (s.back() == ',');
				std::string param;
				if(next) {
					param = s.substr(0, s.size() - 1);
				} else {
					param = s;
				}
				map_it_->second.push_back(text_filter_(param));
				if(!next) {
					back_it_ = map_it_;
					map_it_ = opr_map_.end();
				}
			}

			return "";
		}
		

		//-----------------------------------------------------------------//
		/*!
			@brief	パラメーターの取得
			@param[in]	idx	登録順のインデックス
			@return 登録されたパラメーター
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get(uint32_t idx) const {
			if(idx < list_.size()) {
				return list_[idx]->second;
			} else {
				static utils::strings dummy;
				return dummy;
			}
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief	リスト表示
			@param[in]	key	キー文字列
		*/
		//-----------------------------------------------------------------//
		void list(const std::string& key) const
		{
			for(auto it : list_) {
				std::string para;
				for(const auto& s : it->second) {
					if(!para.empty()) para += ", ";
					para += s;
				}
				std::cout << key << ": (" << it->first << ") " << para << std::endl;
			}
		}
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	base 構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct base_t {
		enum class type {
			title,		///< タイトル
			author,		///< オーサリング・ユーザー
			file,		///< インクルード・ファイル
			space,		///< 名前空間
		};

	private:
		analize	analize_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		base_t() : analize_(".title,.author,.file,.space") { }


		//-----------------------------------------------------------------//
		/*!
			@brief	解析
			@param[in]	s	解析文字列
			@return エラーの場合、エラー文字列
		*/
		//-----------------------------------------------------------------//
		std::string analize(const std::string& s) { return analize_.injection(s); }


		//-----------------------------------------------------------------//
		/*!
			@brief	パラメーターの取得
			@param[in]	t	コマンド・タイプ
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get(type t) const {
			return analize_.get(static_cast<uint32_t>(t));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リスト表示
		*/
		//-----------------------------------------------------------------//
		void list() const { analize_.list("base"); }
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	reg 構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct reg_t {
		enum class type {
			title,
			base,
			address,
			name,
		};

	private:
		analize	analize_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		reg_t() : analize_(".title,.base,.address,.name") { }


		//-----------------------------------------------------------------//
		/*!
			@brief	解析
			@param[in]	s	解析文字列
			@return エラーの場合、エラー文字列
		*/
		//-----------------------------------------------------------------//
		std::string analize(const std::string& s) { return analize_.injection(s); }


		//-----------------------------------------------------------------//
		/*!
			@brief	パラメーターの取得
			@param[in]	t	コマンド・タイプ
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get(type t) const {
			return analize_.get(static_cast<uint32_t>(t));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リスト表示
		*/
		//-----------------------------------------------------------------//
		void list() const { analize_.list("reg"); }
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	bit 構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct bit_t {
		enum class type {
			title,
			def,
			name,
		};

	private:
		analize	analize_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		bit_t() : analize_(".title,.def,.name") { }


		//-----------------------------------------------------------------//
		/*!
			@brief	解析
			@param[in]	s	解析文字列
			@return エラーの場合、エラー文字列
		*/
		//-----------------------------------------------------------------//
		std::string analize(const std::string& s) { return analize_.injection(s); }


		//-----------------------------------------------------------------//
		/*!
			@brief	パラメーターの取得
			@param[in]	t	コマンド・タイプ
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get(type t) const {
			return analize_.get(static_cast<uint32_t>(t));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リスト表示
		*/
		//-----------------------------------------------------------------//
		void list() const { analize_.list("bit"); }
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	クラス構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct class_t {
		reg_t	reg_;
		typedef std::vector<bit_t> bits;
		bits	bits_;
	};
	typedef std::vector<class_t> classies;
}
