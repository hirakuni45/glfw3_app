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

		enum class analize_type {
			none,
			error,
			first,
			base_in,
			base_main,
		};

		bool				verbose_;
		utils::text_edit	te_;
		analize_type		analize_type_ = analize_type::none;
		std::string			last_error_;

		std::string make_error_(uint32_t pos, const std::string& line) {
			auto ret = (boost::format("(%u)%s") % pos % line).str();
			return ret;
		}

		void analize_base_(const std::string& line, const utils::strings& ss) {
			if(ss.empty()) return;

			std::cout << ss[0] << std::endl;
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

			analize_type_ = analize_type::first;
			te_.loop([this](uint32_t pos, const std::string& line) {
				if(!line.empty() && line[0] == '#') return;

				if(analize_type_ == analize_type::none |
				   analize_type_ == analize_type::error) return;

				auto ss = utils::split_text(line, " \t", "\"'");
				if(ss.empty()) return;

				switch(analize_type_) {
				case analize_type::first:
					if(ss[0] == "base") {
						analize_type_ = analize_type::base_in;
					} else {
						last_error_ = make_error_(pos, line);
						last_error_ += (boost::format(", error: '%s'") % ss[0]).str();
						if(verbose_) {
							std::cerr << last_error_ << std::endl;
						}
						analize_type_ = analize_type::error;
					}
					break;
				case analize_type::base_in:
					if(ss[0] == "{") analize_type_ = analize_type::base_main;
					break;
				case analize_type::base_main:
					if(ss[0] == "}") analize_type_ = analize_type::first;
					else {
						analize_base_(line, ss);
					}
					break;
				default:
					break;
				}
			} );

			return analize_type_ != analize_type::error;
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
