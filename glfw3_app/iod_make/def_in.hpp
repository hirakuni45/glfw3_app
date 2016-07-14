#pragma once
//=====================================================================//
/*!	@file
	@brief	設定ファイル入力 @n
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/format.hpp>
#include "utils/string_utils.hpp"
#include "utils/text_edit.hpp"
#include "def_st.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Def　input クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct def_in {


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	レジスター定義
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

	private:

		enum class main_type {
			none,
			error,
			first,
			base_in,
			base_core,
			class_in,
			class_core,
		};

		bool				verbose_;
		std::string			last_error_;
		utils::text_edit	te_;
		main_type			main_type_ = main_type::none;
		base_t				base_;
		class_t				class_;


		std::string make_error_(uint32_t pos, const std::string& line) {
			auto ret = (boost::format("(%u)%s") % pos % line).str();
			return ret;
		}


		std::string analize_main_(const utils::strings& ss) {
			std::string err;
			for(const auto& s : ss) {
				switch(main_type_) {
				case main_type::first:
					if(s == "base") {
						main_type_ = main_type::base_in;
					} else if(s == "class") {
						main_type_ = main_type::class_in;
					} else {
						err = (boost::format(", invalid #main: '%s'") % s).str();
						main_type_ = main_type::error;
					}
					break;
				case main_type::base_in:
				case main_type::class_in:
					if(s == "{") {
						main_type_ = static_cast<main_type>(static_cast<int>(main_type_) + 1);

					}
					break;
				case main_type::base_core:
				case main_type::class_core:
					if(s == "}") {
						main_type_ = main_type::first;
					} else {
						if(main_type_ == main_type::base_core) {
							auto e = base_.analize(s);
							if(!e.empty()) {
								err = ", " + e;
							}
						} else if(main_type_ == main_type::class_core) {
							auto e = class_.analize(s);
							if(!e.empty()) {
								err = ", " + e;
							}
						}
					}
					break;
				default:
					break;
				}
				if(!err.empty()) break;
			}
			return err;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	verbose	詳細時「true」
		*/
		//-----------------------------------------------------------------//
		def_in(bool verbose = false) : verbose_(verbose) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	設定書式をロード
			@param[in]	file	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& file)
		{
			auto ret = te_.load(file);
			if(ret && verbose_) {
				std::cout << "Input: '" << file << "'" << std::endl;
				std::cout << "lines: " << te_.get_lines() << std::endl;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	解析
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool analize()
 		{
			last_error_.clear();

			main_type_ = main_type::first;
			te_.loop([this](uint32_t pos, const std::string& line) {
				if(line.empty()) return;
				if(line[0] == '#') return;

				if(main_type_ == main_type::none |
				   main_type_ == main_type::error) return;


				auto ss = utils::split_text(line, " \t", "\"'");
				if(ss.empty()) return;
				auto ret = analize_main_(ss);
				if(!ret.empty()) {
					last_error_ = make_error_(pos, line);
					last_error_ += ret;
					main_type_ = main_type::error;
		   			if(verbose_) {
		   				std::cerr << last_error_ << std::endl;
		   			}
				}
			} );


			base_.list();


			return main_type_ != main_type::error;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	解析エラーを取得
			@return 解析エラー文字列
		*/
		//-----------------------------------------------------------------//
		std::string get_last_error() const { return last_error_; }

	};

}
