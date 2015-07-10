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

		hnd_ = unzOpen(archive.c_str());
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

				zmap::iterator it = zmap_.find(fn);
				if(it == zmap_.end()) {
					zmap::value_type stb(fn, files_.size());
					zmap_.insert(stb);
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
	bool unzip::get_file(uint32_t index, char* buff)
	{
		if(index < static_cast<uint32_t>(files_.size())) {
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
	bool unzip::create_file(uint32_t index, const std::string& filename)
	{
		if(index < static_cast<uint32_t>(files_.size())) {
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
	uint32_t unzip::file_count() const
	{
		if(hnd_) {
			return static_cast<uint32_t>(files_.size());
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
	int32_t unzip::find(const std::string& key)
	{
		zmap::const_iterator cit = zmap_.find(key);
		if(cit == zmap_.end()) return -1;
		return static_cast<int32_t>(cit->second);
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
		zmap_.clear();
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
