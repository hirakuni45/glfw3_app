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

#include "utils/format.hpp"

namespace text {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  utils クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class compose {

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

		struct class_t {

			std::string		def_;	///< クラス定義
			std::string		name_;	///< クラス名

			METHODS			methods_;

			class_t() { }

			void clear() {
				def_.clear();
				name_.clear();
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
utils::format("Class name: '%s'\n") % name_.c_str();
			}
		};


		typedef std::vector<std::string> LINES;
		LINES			header_;

		class_t			class_t_;

		typedef std::vector<class_t> CLASS_TS;
		CLASS_TS		class_ts_;


		LINES			source_;


		uint32_t		empty_line_;
		uint32_t		space_tab_;
		uint32_t		comment_;

		enum class TASK {
			Normal,
			ClassDef,
			ClassIn,
		};
		TASK			task_;

		method_t::EXT	current_ext_;
		int				class_nest_;

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


		void end_def_() {
			if(method_t_.def_.empty()) return;
utils::format("  %s\n") % method_t_.def_.c_str();

			class_t_.methods_.push_back(method_t_);
			method_t_.clear();
			method_t_.ext_ = current_ext_;
		}


		void end_class_() {
			task_ = TASK::Normal;
			class_ts_.push_back(class_t_);
			current_ext_ = method_t::EXT::PRIVATE;
utils::format("\n");
		}


		void analize_sub_(const std::string& s) {
			if(s == "public:") {
				current_ext_ = method_t::EXT::PUBLIC;
			} else if(s == "private:") {
				current_ext_ = method_t::EXT::PRIVATE;
			} else if(s == "protected:") {
				current_ext_ = method_t::EXT::PROTECTED;
			} else {
/// utils::format("%s\n") % s.c_str();
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
/// utils::format("Impl: '%s'\n") % s.c_str();
///					if(impl_.empty()) impl_ = s;
///					else {
///					}
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
						analize_sub_(word);
					}
					word.clear();
					continue;
				} else if(ch == '/' && p[0] == '/') {  // ここから先はコメント行
					break;										
				} else if(ch == '{') {
					if(!word.empty()) {
						analize_sub_(word);
					}
					word.clear();
					++class_nest_;
				} else if(ch == '}') {
					if(!word.empty()) {
						analize_sub_(word);
					}
					word.clear();
					class_nest_--;
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
				analize_sub_(word);
			}

			if(class_nest_ == 0) {
				end_class_();
			}
		}

	public:
		//---------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//---------------------------------------------------------------//
		compose() : empty_line_(0), space_tab_(0), comment_(0),
			task_(TASK::Normal), current_ext_(method_t::EXT::PRIVATE), class_nest_(0)
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

/// utils::format("Header: %u\n") % header_.size();

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
				}
			}

			utils::format("Total-line: %u\n") % header_.size();
			utils::format("Empty-line: %u\n") % empty_line_;
			utils::format("Space-Tab: %u\n") % space_tab_;
			utils::format("Comment-line: %u\n") % comment_;
			utils::format("class num: %u\n") % class_ts_.size(); 
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

/// utils::format("Source: %u\n") % source_.size();

			return true;
		}

	};
}
