//=====================================================================//
/*!	@file
	@brief	unzip クラス（ヘッダー）@n
			zlib minizip/unzip.c を利用
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/unzip.hpp"
#include <boost/foreach.hpp>
#include "utils/string_utils.hpp"

namespace utils {

	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブを開く
		@param[in]	arcive	アーカイブ・ファイル名
		@return エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	bool unzip::open(const std::string& archive)
	{
		close();

		std::string s;
#ifdef WIN32
		// ※ Windows では、アーカイブ名は、sjis にする必要があるようだ・・
		utils::utf8_to_sjis(archive, s);
#else
		s = archive;
#endif
		hnd_ = unzOpen(s.c_str());
		if(hnd_ == 0) {
			return false;
		}

		do {
			file_info fi;
			char fn[2048];
			fn[0] = 0;
			if(unzGetCurrentFileInfo(hnd_, &fi.info_, fn, sizeof(fn), NULL, 0, NULL, 0) != UNZ_OK) {
				break;
			}
			const char* p = strrchr(fn, '/');
			if(p != 0 && p[1] == 0) {	// 最後の文字が「/」で終わる場合はアーカイブに含めない
				dirs_.push_back(fn);
			} else {
				fi.ofs_ = unzGetOffset(hnd_);
				fi.path_ = fn;

				zmap_it it = map_.find(fn);
				if(it == map_.end()) {
					std::pair<std::string, size_t> stb(fn, files_.size());
					map_.insert(stb);
					files_.push_back(fi);
				} else {
					files_[it->second] = fi;
				}
			}
		} while(unzGoToNextFile(hnd_) != UNZ_END_OF_LIST_OF_FILE) ;

		zname_ = archive;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブ内のファイル取得
		@param[in]	index	ファイル番号
		@param[in]	buff	ファイルのイメージを受け取るポインター
		@return エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	bool unzip::get_file(int index, char* buff)
	{
		if(index >= 0 && index < static_cast<int>(files_.size())) {
			unzSetOffset(hnd_, files_[index].ofs_);

			if(unzOpenCurrentFile(hnd_) != UNZ_OK) return false;

			size_t sz = get_filesize(index);
			unzReadCurrentFile(hnd_, buff, sz);
			unzCloseCurrentFile(hnd_);

			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブ内のファイル書き出し
		@param[in]	index	ファイル番号
		@param[in]	filename	ファイル名
		@return エラーが無ければ「true」
	*/
	//-----------------------------------------------------------------//
	bool unzip::create_file(int index, const std::string& filename)
	{
		if(index >= 0 && index < static_cast<int>(files_.size())) {
			unzSetOffset(hnd_, files_[index].ofs_);

			if(unzOpenCurrentFile(hnd_) != UNZ_OK) return false;

			utils::file_io fout;
			fout.open(filename, "wb");

			char buff[4096];
			uLong sz;
			while((sz = unzReadCurrentFile(hnd_, buff, sizeof(buff))) > 0) {
				fout.write(buff, 1, sz);
			}
			unzCloseCurrentFile(hnd_);
			fout.close();

			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブ数を取得
		@return ファイル数を返す
	*/
	//-----------------------------------------------------------------//
	int unzip::file_count() const
	{
		if(hnd_) {
			return static_cast<int>(files_.size());
		} else {
			return 0;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブ内の名前をスキャン
		@param[in]	key	スキャンするキー
		@return 見つからない場合、負の値
	*/
	//-----------------------------------------------------------------//
	int unzip::find(const std::string& key)
	{
		zmap_cit cit = map_.find(key);
		if(cit == map_.end()) return -1;
		return static_cast<int>(cit->second);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブ内の名前をスキャン（大文字小文字を評価しない）
		@param[in]	key	スキャンするキー
		@return 見つからない場合、負の値
	*/
	//-----------------------------------------------------------------//
	int unzip::find_no_capital(const std::string& key)
	{
		int i = 0;
		BOOST_FOREACH(const file_info& info, files_) {
			if(utils::no_capital_strcmp(info.path_, key) == 0) {
				return i;
			}
			++i;
		}
		return -1;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アーカイブを閉じる
	*/
	//-----------------------------------------------------------------//
	void unzip::close()
	{
		if(hnd_) {
			unzClose(hnd_);
			hnd_ = 0;
		}
		dirs_.clear();
		files_.clear();
		zname_.clear();
		map_.clear();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void unzip::destroy()
	{
		close();
	}


};
