#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイル情報クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <string>
#include <sys/stat.h>
#include "utils/file_io.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル情報クラス@n
				ファイル名、サイズ、ディレクトリーの判定を格納
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class file_info {
		std::string		name_;
		bool			directory_;
		size_t			size_;
		time_t			time_;
		mode_t			mode_;
		bool			drive_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	標準コンストラクター
		*/
		//-----------------------------------------------------------------//
		file_info() : name_(), directory_(false), size_(0), time_(0), mode_(0), drive_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化コンストラクター
			@param[in]	name	ファイル名
			@param[in]	directory	ディレクトリーの場合に「true」
			@param[in]	size	ファイル・サイズ
			@param[in]	tm		時間
			@param[in]	mt		モード
			@param[in]	drv		ドライブ（省略すると「false」）
		*/
		//-----------------------------------------------------------------//
		file_info(const std::string& name, bool directory, size_t size, const time_t tm, const mode_t mt, bool drv = false) : name_(name), directory_(directory), size_(size), time_(tm), mode_(mt), drive_(drv) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	name	ファイル名
			@param[in]	directory	ディレクトリーの場合に「true」
			@param[in]	size	ファイル・サイズ
			@param[in]	tm		時間
			@param[in]	mt		モード
			@param[in]	drv		ドライブ（省略すると「false」）
		*/
		//-----------------------------------------------------------------//
		void initialize(const std::string& name, bool directory, size_t size, const time_t tm, const mode_t mt, bool drv = false) { file_info(name, directory, size, tm, mt, drv); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル名を得る
			@return ファイル名を返す
		*/
		//-----------------------------------------------------------------//
		const std::string& get_name() const { return name_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	デイレクトリーか検査
			@return ディレクトリーの場合は「true」
		*/
		//-----------------------------------------------------------------//
		bool is_directory() const { return directory_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・サイズを得る
			@return ファイルサイズを返す@n
					ディレクトリーの場合、常に０
		*/
		//-----------------------------------------------------------------//
		size_t get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	タイム・スタンプを得る
			@return ファイルサイズを返す
		*/
		//-----------------------------------------------------------------//
		time_t get_time() const { return time_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルモード得る
			@return ファイルモードを返す
		*/
		//-----------------------------------------------------------------//
		mode_t get_mode() const { return mode_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ドライブか検査
			@return ドライブの場合は「true」
		*/
		//-----------------------------------------------------------------//
		bool is_drive() const { return drive_; }

	};

	typedef std::vector<file_info>					file_infos;
	typedef std::vector<file_info>::iterator		file_infos_it;
	typedef std::vector<file_info>::const_iterator	file_infos_cit;

	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーのファイルリストを作成
		@param[in]	root	ルート・パス
		@param[out]	list	ファイルリストを受け取るクラス
		@return リストの取得に失敗した場合「false」
	*/
	//-----------------------------------------------------------------//
	bool create_file_list(const std::string& root, file_infos& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーのファイルリストを作成
		@param[in]	root	ルート・パス
		@return		ファイルリスト
	*/
	//-----------------------------------------------------------------//
	inline file_infos create_file_list(const std::string& root) {
		file_infos dst;
		create_file_list(root, dst);
		return std::move(dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報からフォーマットされた文字列を生成
		@param[in]	list	fileinfos クラス
		@param[in]	format	フォーマット（ディレクトリー出力専用形式）
		@param[in]	dst		出力先列
	*/
	//-----------------------------------------------------------------//
	void create_format_file_infos(const file_infos& list, const std::string& format, utils::strings& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報リストを正規表現フィルターで再構成
		@param[in]	src		fileinfos クラス
		@param[in]	filter	拡張子文字列
		@param[in]	cap	「false」なら大文字小文字を判定する
		@return		出力列
	*/
	//-----------------------------------------------------------------//
	file_infos filter_file_infos(const file_infos& src, const std::string& filter, bool cap = true);

}
