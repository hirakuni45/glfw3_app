#pragma once
//=====================================================================//
/*!	@file
	@brief	ディレクトリー情報取得クラス（ヘッダー）@n
			ディレクトリー情報取得をスレッドにて並行して行う
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <thread>
#include <future>
#include <tuple>
#include "utils/drive_info.hpp"
#include "utils/file_info.hpp"
#include "utils/string_utils.hpp"
#include <iostream>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ディレクトリー情報取得クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class files {
		typedef std::tuple<std::string, std::string> inp; 

		static file_infos task_(inp in) {
			if(std::get<1>(in).empty()) {
				file_infos fis;
				create_file_list(std::get<0>(in), fis);
				return fis;
			} else {
				file_infos tmp;
				create_file_list(std::get<0>(in), tmp);
				file_infos fis;
				filter_file_infos(tmp, std::get<1>(in), fis);
				return fis;
			}
		};

		std::string		root_path_;
		std::string		ext_;

		std::future<file_infos>	future_;

		utils::file_infos	file_infos_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	path	パス
		*/
		//-----------------------------------------------------------------//
		files() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~files() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ルートパスを設定
			@param[in]	path	パス
			@param[in]	ext		拡張子フィルター
		*/
		//-----------------------------------------------------------------//
		void set_path(const std::string& path, const std::string& ext = "") {
			root_path_ = path;
			ext_ = ext;
			inp in = std::make_tuple(path, ext);
			future_ = std::async(std::launch::async, task_, in);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ルートパスを取得
			@return ルートパス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_path() const { return root_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	拡張子を取得
			@return 拡張子
		*/
		//-----------------------------------------------------------------//
		const std::string& get_exts() const { return ext_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報取得かどうか？
			@return ファイル情報取得なら「true」
		*/
		//-----------------------------------------------------------------//
		bool probe() {
			return future_.valid();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル情報郡を取得
			@return ファイル情報郡
		*/
		//-----------------------------------------------------------------//
		const file_infos& get() {
			file_infos_ = future_.get();
			return file_infos_;
		}
	};

}
