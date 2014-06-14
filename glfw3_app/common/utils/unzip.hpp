#pragma once
//=====================================================================//
/*!	@file
	@brief	unzip クラス（ヘッダー）@n
			zlib minizip/unzip.c を利用
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <boost/unordered_map.hpp>
#include "minizip/unzip.h"
#include "utils/file_io.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	zlib minizip-unzip クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class unzip {

		std::string	zname_;
		unzFile		hnd_;

		struct file_info {
			uLong			ofs_;
			unz_file_info	info_;
			std::string		path_;
		};

		std::vector<file_info>	files_;

		std::vector<std::string>	dirs_;

		typedef boost::unordered_map<std::string, size_t>	zmap;
		zmap	zmap_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		unzip() : hnd_(0) { }

		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~unzip() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブを開く
			@param[in]	arcive	アーカイブ・ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& archive);


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ名を取得
			@return アーカイブ名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_archive_name() const { return zname_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ数を取得
			@return ファイル数を返す
		*/
		//-----------------------------------------------------------------//
		uint32_t file_count() const;


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリー数を取得
			@return ディレクトリー数を返す
		*/
		//-----------------------------------------------------------------//
		uint32_t dir_count() const { return static_cast<uint32_t>(dirs_.size()); }


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内のファイル名取得
			@param[in]	index	ファイル番号
			@return ファイル名を返す
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file_name(uint32_t index) const {
			if(index < static_cast<uint32_t>(files_.size())) {
				return files_[index].path_;
			} else {
				static std::string null_string_;
				return null_string_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内のディレクトリー名取得
			@param[in]	index	ファイル番号
			@return ディレクトリー名を返す
		*/
		//-----------------------------------------------------------------//
		const std::string& get_dir_name(uint32_t index) const {
			if(index < static_cast<uint32_t>(dirs_.size())) {
				return dirs_[index];
			} else {
				static std::string null_string_;
				return null_string_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内のファイルサイズを返す
			@param[in]	index	ファイル番号
			@return ファイルサイズを返す
		*/
		//-----------------------------------------------------------------//
		size_t get_filesize(uint32_t index) const {
			if(index < static_cast<uint32_t>(files_.size())) {
				return static_cast<size_t>(files_[index].info_.uncompressed_size);
			} else {
				return 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内のファイル作成日時を取得
			@param[in]	index	ファイル番号
			@return 作成日時
		*/
		//-----------------------------------------------------------------//
		time_t get_date(uint32_t index) const {
			if(index < static_cast<uint32_t>(files_.size())) {
				const tm_unz& tm_unz_t = files_[index].info_.tmu_date;
				struct tm tm_t;
				tm_t.tm_sec = tm_unz_t.tm_sec;
				tm_t.tm_min = tm_unz_t.tm_min;
				tm_t.tm_hour = tm_unz_t.tm_hour;
				tm_t.tm_mday = tm_unz_t.tm_mday;
				tm_t.tm_mon = tm_unz_t.tm_mon;
				tm_t.tm_year = tm_unz_t.tm_year;
				tm_t.tm_wday = 0;
				tm_t.tm_yday = 0;
				tm_t.tm_isdst = 0;
				return mktime(&tm_t);
			} else {
				return 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内のファイル取得
			@param[in]	index	ファイル番号
			@param[in]	buff	ファイルを受け取るポインター@n
								※あらかじめ「get_filesize」で必要なメモリーを確保しておく事
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool get_file(uint32_t index, char* buff);


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内のファイル書き出し
			@param[in]	index	ファイル番号
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool create_file(uint32_t index, const std::string& filename);


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内の名前をスキャン
			@param[in]	key	スキャンするキー
			@return 見つからない場合、負の値
		*/
		//-----------------------------------------------------------------//
		int32_t find(const std::string& key);


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブ内の名前をリスト
		*/
		//-----------------------------------------------------------------//
		void list_name() {
			for(size_t i = 0; i < file_count(); ++i) {
				const std::string& s = get_file_name(i);
				std::cout << s << std::endl;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アーカイブを閉じる
		*/
		//-----------------------------------------------------------------//
		void close();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
