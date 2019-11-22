#pragma once
//=====================================================================//
/*! @file
    @brief  C++ ソース関係ユーティリティークラス
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "utils/file_io.hpp"
#include <string>
#include <vector>
#include <map>

#include "utils/string_utils.hpp"
#include "utils/format.hpp"

namespace text {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  compose クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class compose {


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  method_t クラス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct method_t {

			enum class EXT {
				PRIVATE,
				PUBLIC,
				PROTECTED,
			};
			EXT				ext_;

			enum class TYPE {
				CONSTRUCTOR,
				DESTRACTOR,
				NORMAL,
				VIRTUAL,
				OVERRIDE,
			};
			TYPE			type_;

			std::string		def_;	///< 定義

			typedef std::vector<std::string> IMPL;	///< implementation
			IMPL			impl_;

			method_t() : ext_(EXT::PRIVATE), type_(TYPE::NORMAL) { }

			void clear(TYPE type = TYPE::NORMAL) {
				ext_ = EXT::PRIVATE;
				type_ = type;
				def_.clear();
				impl_.clear();
			}
		};
		method_t	method_t_;
		typedef std::vector<method_t> METHODS;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  class_t クラス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct class_t {

			std::string		def_;	///< クラス定義
			std::string		name_;	///< クラス名

			METHODS			methods_;

			typedef std::map<std::string, int> MAP;
			MAP				map_;


			class_t() { }

			void clear() {
				def_.clear();
				name_.clear();
				methods_.clear();
				map_.clear();
			}


			void make_name() {

				name_.clear();
				const char* p = def_.c_str();
				char ch;
				while((ch = *p) != 0) {
					if(ch == ' ' || ch == '\t') {
						++p;
						continue;
					} else break;
				}

				while((ch = *p++) != 0) {
					if(ch == ' ' || ch == '\t') break;
					else if(ch == ':') break;
					name_ += ch;
				}
/// utils::format("Class name: '%s'\n") % name_.c_str();
			}


			void list() {
				utils::format("class %s {\n\n") % def_.c_str();
				for(auto& t : methods_) {
					utils::format("\t");
					if(t.ext_ == method_t::EXT::PUBLIC) {
						utils::format("public ");
					}
					utils::format("%s\n") % t.def_.c_str();
					for(auto& m : t.impl_) {
						utils::format("\t%s\n") % m.c_str();
					}
					utils::format("\n");
				}
				utils::format("}\n");
			}
		};


		typedef std::vector<std::string> LINES;
		LINES			header_;

		class_t			class_t_;

		typedef std::vector<class_t> CLASS_VEC;
		CLASS_VEC		class_vec_;
		typedef std::map<std::string, int> CLASS_MAP;
		CLASS_MAP		class_map_;

		LINES			source_;

		uint32_t		empty_line_;
		uint32_t		space_tab_;
		uint32_t		comment_;

		enum class TASK {
			Normal,
			ClassDef,	// Header pass
			ClassIn,	// Header pass

			ImplDef,	// Source pass
			ImplIn,		// Source pass
		};
		TASK			task_;

		method_t::EXT	current_ext_;
		int				class_nest_;
		uint32_t		class_name_err_;

		std::string		impl_name_;
		int				class_idx_;
		int				impl_idx_;


		void analize_nest_(const char* src)
		{
			char ch;
			while((ch = *src) != 0) {
				if(ch == '{') ++class_nest_;
				else if(ch == '}') class_nest_--;
				++src;
			}
		}


		void class_def_(const char* org)
		{
			std::string str;

			char ch;
			while((ch = *org++) != 0) {
				if(ch == '{') {  // クラス定義完了
					class_t_.def_ = str;
					class_t_.make_name();
					++class_nest_;
					task_ = TASK::ClassIn;
					return;
				}
				str += ch;
			}

			if(!str.empty()) {
				class_t_.def_ = str;
				task_ = TASK::ClassDef;  // クラス定義が終わっていない
			}
		}


		bool is_space_tab_(const std::string& str) {
			for(auto ch : str) {
				if(ch == ' ' || ch == '\t') continue;
				else return false;
			}
			++space_tab_;
			return true;
		}


		bool is_comment_(const std::string& str) {
			const char*p = str.c_str();
			char ch;
			while((ch = *p++) != 0) {
				if(ch == ' ' || ch == '\t') continue;
				else if(ch == '/' && p[0] == '/') {
					++comment_;
					return true;
				} else {
					return false;
				}
			}
			return false;
		}


		std::string get_func_name_(const char* p) {
			std::string s;
			std::string tmp;
			char ch;
			while((ch = *p++) != 0) {
				if(ch == ' ' || ch == '\t') {
					tmp = s;
					s.clear();
					continue;
				} else if(ch == '(') {
					if(s.empty()) s = tmp;
					break;
				}
				s += ch;
			}
			return s;
		}


		void end_def_() {
			if(method_t_.def_.empty()) return;
			auto& tmp = method_t_.def_;
			// 関数宣言で、最後が[;]なら取り除く
			if(tmp.find("(") != std::string::npos && tmp.back() == ';') {
				tmp.pop_back();
				// method の map を作成
				auto fn = get_func_name_(tmp.c_str());
// utils::format("Ins: %s::%s\n") % class_t_.name_.c_str() % fn.c_str();
				auto ret = class_t_.map_.emplace(fn, class_t_.methods_.size());
				if(!ret.second) {  // method 名が「重なった」場合
					utils::format("Can't insert method name: '%s'\n") % tmp.c_str();
				}
			}
			class_t_.methods_.push_back(method_t_);
			method_t_.clear();
			method_t_.ext_ = current_ext_;
		}


		void end_class_() {
			task_ = TASK::Normal;
			auto ret = class_map_.emplace(class_t_.name_, class_vec_.size());
			if(!ret.second) {  // class 名が「重なった」場合
				utils::format("Can't insert class name: '%s'\n") % class_t_.name_.c_str();
				++class_name_err_;
			}
			class_vec_.push_back(class_t_);
			current_ext_ = method_t::EXT::PRIVATE;
		}


		void analize_class_sub_(const std::string& s) {
			if(s == "public:") {
				current_ext_ = method_t::EXT::PUBLIC;
			} else if(s == "private:") {
				current_ext_ = method_t::EXT::PRIVATE;
			} else if(s == "protected:") {
				current_ext_ = method_t::EXT::PROTECTED;
			} else {
				if(class_nest_ == 1) {  // 定義
					if(method_t_.def_.empty()) method_t_.def_ = s;
					else {
						method_t_.def_ += ' ';
						method_t_.def_ += s;
					}
					if(s.back() == ';' || s == ";") {
						end_def_();
					}
				} else if(class_nest_ > 1) {  // 実装
					bool set = true;
					// 実装内容が、";" だけの場合は除外
					if(method_t_.impl_.size() > 0 && method_t_.impl_.back() == "{") {
						if(s == ";") set = false;
					}
					if(set) {
						method_t_.impl_.push_back(s);
					}
				}
			}
		}


		// クラス内解析
		void analize_class_(const char* p) {

			std::string word;
			char bch = 0;
			char ch;
			while((ch = *p++) != 0) {
				if(ch == ' ' || ch == '\t') {
					if(!word.empty()) {
						analize_class_sub_(word);
					}
					word.clear();
					continue;
				} else if(ch == '/' && p[0] == '/') {  // ここから先はコメント行
					break;										
				} else if(ch == '{') {
					if(class_nest_ >= 0) {
						method_t_.impl_.push_back("{");
					}
					if(!word.empty()) {
						analize_class_sub_(word);
					}
					word.clear();
					++class_nest_;
				} else if(ch == '}') {
					if(!word.empty()) {
						analize_class_sub_(word);
					}
					word.clear();
					class_nest_--;
					if(class_nest_ >= 1) {
						method_t_.impl_.push_back("}");
					}
					if(class_nest_ == 1) {  // 定義終了
						end_def_();
					}
				} else {
					if(class_nest_ == 0 && ch == ';') continue;
					word += ch;
					bch = ch;
				}
			}
			if(!word.empty()) {
				analize_class_sub_(word);
			}

			if(class_nest_ == 0) {
				end_class_();
			}
		}


		void analize_source_(const char* p)
		{
			if(class_idx_ < 0) return;
			if(impl_idx_ < 0) return;

			const char* org = p;

			std::string word;
			char ch;
			while((ch = *p++) != 0) {
				if(ch == ' ' || ch == '\t') {
					if(!word.empty()) {
///						analize_class_sub_(word);
					}
					word.clear();
					continue;
				} else if(ch == '/' && p[0] == '/') {  // ここから先はコメント行
					break;										
				} else if(ch == '{') {
					if(class_nest_ >= 0) {
///						method_t_.impl_.push_back("{");
					}
					if(!word.empty()) {
///						analize_class_sub_(word);
					}
					word.clear();
					++class_nest_;
				} else if(ch == '}') {
					if(!word.empty()) {
///						analize_class_sub_(word);
					}
					word.clear();
					class_nest_--;
					if(class_nest_ >= 1) {
///						method_t_.impl_.push_back("}");
					}
					if(class_nest_ == 1) {  // 定義終了
///						end_def_();
					}
				} else {
					if(class_nest_ == 0 && ch == ';') continue;
					word += ch;
				}
			}
			if(!word.empty()) {
///				analize_class_sub_(word);
			}


//			utils::format("  %s\n") % org;
			auto& cls = class_vec_[class_idx_];
			auto& mth = cls.methods_[impl_idx_];

			mth.impl_.push_back(org);

			if(class_nest_ == 0) {
				task_ = TASK::Normal;
			}
		}


	public:
		//---------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//---------------------------------------------------------------//
		compose() : empty_line_(0), space_tab_(0), comment_(0),
			task_(TASK::Normal), current_ext_(method_t::EXT::PRIVATE), class_nest_(0),
			class_name_err_(0), class_idx_(-1), impl_idx_(-1)
		{ }


		//---------------------------------------------------------------//
		/*!
			@brief  ヘッダー・リード
		*/
		//---------------------------------------------------------------//
		bool load_header(const std::string path)
		{
			utils::file_io fio;
			if(!fio.open(path, "rb")) {
				return false;
			}

			while(!fio.eof()) {
				auto l = fio.get_line();
				header_.push_back(l);
			}
			fio.close();

			return true;
		}


		//---------------------------------------------------------------//
		/*!
			@brief  ヘッダー・解析
		*/
		//---------------------------------------------------------------//
		void analize_header()
		{
			// クラス解析
			task_ = TASK::Normal;
			bool comment = false;
			for(auto t : header_) {

				if(t.empty()) {  // 空行
					++empty_line_;
					continue;  
				}
				if(!comment) {
					if(t.find("/*") != std::string::npos) {
						comment = true;
					}
				} else if(comment) {
					if(t.find("*/") != std::string::npos) {
						comment = false;
					} else {
						continue;
					}
				}

				if(is_space_tab_(t)) continue;
				if(is_comment_(t)) continue;
				

				switch(task_) {
				case TASK::Normal:
					{
						auto cp = t.find("class");
						if(cp != std::string::npos) {
							class_t_.clear();
							class_def_(t.c_str() + cp + 5);
						}
					}
					break;

				case TASK::ClassDef:
					{
						char ch;
						const char* p = t.c_str();
						while((ch = *p++) != 0) {
							if(ch == '{') {
								class_t_.make_name();
								++class_nest_;
								analize_class_(p);
								task_ = TASK::ClassIn;
								break;
							} else {
								class_t_.def_ += ch;
							}
						}
					}
					break;

				case TASK::ClassIn:
					analize_class_(t.c_str());
					break;

				default:
					break;
				}
			}

			// 各クラス内のメソッド名のマップを作成

		}


		//---------------------------------------------------------------//
		/*!
			@brief  ソース・リード
		*/
		//---------------------------------------------------------------//
		bool load_source(const std::string path)
		{
			utils::file_io fio;
			if(!fio.open(path, "rb")) {
				return false;
			}

			while(!fio.eof()) {
				auto l = fio.get_line();
				source_.push_back(l);
			}
			fio.close();

			return true;
		}


		//---------------------------------------------------------------//
		/*!
			@brief  ソース・解析
		*/
		//---------------------------------------------------------------//
		void analize_source()
		{
			// クラス解析
			task_ = TASK::Normal;
			bool comment = false;
			uint32_t err_count = 0;
			CLASS_MAP::iterator mit = class_map_.end();
			for(auto t : source_) {

				if(t.empty()) {  // 空行
					++empty_line_;
					continue;  
				}
				if(!comment) {
					if(t.find("/*") != std::string::npos) {
						comment = true;
					}
				} else if(comment) {
					if(t.find("*/") != std::string::npos) {
						comment = false;
					} else {
						continue;
					}
				}

				if(is_space_tab_(t)) continue;
				if(is_comment_(t)) continue;

				switch(task_) {
				case TASK::Normal:
					{
						auto cp = t.find("::");
						if(cp != std::string::npos) {
							auto ss = utils::split_text(t.substr(0, cp), " \t");
							auto key = ss.back();
							const auto cit = class_map_.find(key);
							if(cit != class_map_.end()) {
								class_idx_ = cit->second;
								const char* p = &t[cp + 2];
								char ch;
								impl_name_.clear();
								// とりあえず、型名だけしか見ない・・
								bool def = false;
								while((ch = *p++) != 0) {
									if(impl_name_.empty() && (ch == ' ' || ch == '\t')) {
										continue;
									}
									if(ch == ' ' || ch == '\t') continue;
									if(ch == '(') {
										def = true;
									} else if(ch == ')') {
										break;
									}
									if(!def) impl_name_ += ch;
								}
								if(def) {  // 「(」が無い場合、変数定義なのでとりあえず無視
									impl_idx_ = -1;
									task_ = TASK::ImplDef;
								}
							} else {
								utils::format("Can't find class: '%s'\n") % key.c_str();
								++err_count;
								class_idx_ = -1;
							}
						}
					}
					break;

				case TASK::ImplDef:
					{
						char ch;
						const char*p = t.c_str();
						while((ch = *p++) != 0) {
							if(ch == ' ' || ch == '\t') continue;
							if(ch == '{') {
								auto& cls = class_vec_[class_idx_];
								auto cit = cls.map_.find(impl_name_);
								if(cit != cls.map_.end()) {
									impl_idx_ = cit->second;
// utils::format("%s::%s\n") % cls.name_.c_str() % impl_name_.c_str();
									analize_source_(p - 1);
									task_ = TASK::ImplIn;
								} else {
									utils::format("Can't find method: '%s::%s'\n")
										% cls.name_.c_str() % impl_name_.c_str();
									++err_count;
									task_ = TASK::Normal;
								}
							}
						}
					}
					break;

				case TASK::ImplIn:
					analize_source_(t.c_str());
					break;

				default:
					break;
				}
			}
			if(err_count > 0) {
				utils::format("Fail analize source: %u\n") % err_count;
			}
		}


		//---------------------------------------------------------------//
		/*!
			@brief  全体リスト
		*/
		//---------------------------------------------------------------//
		void list_all()
		{
			utils::format("Header line: %u\n") % header_.size();
			utils::format("Source line: %u\n") % source_.size();
			utils::format("Empty line: %u\n") % empty_line_;
			utils::format("Space, Tab: %u\n") % space_tab_;
			utils::format("Comment-line: %u\n") % comment_;
			utils::format("class num: %u\n") % class_vec_.size();
			for(auto& t : class_vec_) {
				t.list();
				utils::format("\n");
			}
		}
	};
}
