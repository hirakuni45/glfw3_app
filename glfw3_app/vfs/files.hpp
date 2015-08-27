#pragma once
//=====================================================================//
/*!	@file
	@brief	ファイルを管理するクラス
	@author	平松邦仁 (hira@bexide.co.jp)
*/
//=====================================================================//
#include <stdint.h>
#include <string>
#include <vector>
#include <cstring>
#include "tree_unit.hpp"
#include "fio.hpp"

#ifdef WIN32
#include <boost/format.hpp>
#endif

namespace vfs {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ファイル群・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class files {

		utils::tree_unit	tree_unit_;
		finfos				finfos_;

		void resize_infos_(uint32_t n) {
			finfo fi;
			while(n >= finfos_.size()) {
				finfos_.push_back(fi);
			}
		}

		fio		fio_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	base	ベース・パス
		*/
		//-----------------------------------------------------------------//
		files(const std::string& base) : tree_unit_(), finfos_(), fio_(base) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	read	ファイル構造を読み込む場合「true」
		*/
		//-----------------------------------------------------------------//
		void start(bool read = false) {
			finfo fi;
			finfos_.clear();
			finfos_.push_back(fi);

			// ディレクトリー情報を読み込み
			if(read) {
//				auto path = base_;
//				path += hex_to_string_(0);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルがあるか調べる
			@return ファイル・ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		uint32_t find(const std::string& path) const {
			auto hnd = tree_unit_.find(path);
			return hnd;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーか検査（子供があるか？）
			@return ディレクトリーなら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_dir(const std::string& path) const {
			return tree_unit_.is_directory(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレント・ディレクトリーの設定
			@param[in]	path	パス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool cd(const std::string& path) {
			return tree_unit_.set_current_path(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーの作成
			@param[in]	path	パス
			@return 成功ならハンドルを返す、「０」なら失敗
		*/
		//-----------------------------------------------------------------//
		uint32_t mkdir(const std::string& path) {
			auto hnd = tree_unit_.make_directory(path);
			resize_infos_(hnd);
			finfos_[hnd].handle_ = hnd;
			return hnd;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーの削除
			@param[in]	path	パス
			@return 削除した数、「０」なら失敗
		*/
		//-----------------------------------------------------------------//
		uint32_t rmdir(const std::string& path) {
			auto hnds = tree_unit_.remove_directory(path);
			for(auto hnd : hnds) {
				finfos_[hnd].handle_ = 0;
			}
			return hnds.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをオープンしてハンドルを返す
			@param[in]	path	パス
			@param[in]	opm		オープン・モード
			@return ハンドル、「０」なら失敗
		*/
		//-----------------------------------------------------------------//
		uint32_t open(const std::string& path, vfs::open_mode opm) {
			uint32_t hnd = 0;
			if(opm == vfs::open_mode::write) {
				hnd = tree_unit_.install(path);
				resize_infos_(hnd);
				if(finfos_[hnd].open_mode_ != vfs::open_mode::none) return 0;
				finfos_[hnd].handle_ = hnd;
				finfos_[hnd].open_mode_ = opm;
			} else if(opm == open_mode::read) {
				hnd = tree_unit_.find(path);
				if(hnd == 0) return 0;
				if(finfos_[hnd].open_mode_ != vfs::open_mode::none) return 0;
				finfos_[hnd].open_mode_ = opm;
			}
			return hnd;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルへの書き込み
			@param[in]	hnd	ファイル・ハンドル
			@param[in]	src	ソース・ポインター
			@param[in]	size	サイズ
			@return 書き込んだバイト数
		*/
		//-----------------------------------------------------------------//
		int write(uint32_t hnd, const void* src, uint32_t size) {
			int ret = -1;
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::write) {
					return ret;
				}

				if(src == nullptr) return 0;

				ret = 0;
				while(size > 0) {
					uint32_t cs = fi.cash_.size() - fi.cpos_;
					if(size <= cs) {
						std::memcpy(&fi.cash_[fi.cpos_], src, size);
						fi.cpos_ += size;
						fi.fpos_ += size;
						ret += static_cast<int>(size);
						size = 0;
					} else {
						std::memcpy(&fi.cash_[fi.cpos_], src, cs);
						fio_.write_cash(fi, false);
						fi.cpos_ = 0;
						fi.fpos_ += cs;
						size -= cs;
						ret += static_cast<int>(cs);
					}
				}
//				std::cout << "Write: " << ret << std::endl;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをクローズする
			@param[in]	hnd	ファイル・ハンドル
			@return 成功なら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool close(uint32_t hnd) {
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ == open_mode::write) {
					fio_.write_cash(fi, true);
					fi.fsize_ = fi.fpos_;
				} else if(fi.open_mode_ == open_mode::read) {
					
				}
				fi.open_mode_ = open_mode::none;
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルを削除する
			@param[in]	path	パス
			@return 成功なら「０」以外
		*/
		//-----------------------------------------------------------------//
		uint32_t remove(const std::string& path) {
			auto hnds = tree_unit_.erase(path);
			for(auto hnd : hnds) {
				finfos_[hnd].handle_ = 0;
			}
			return hnds.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクリー情報の書き込み
		*/
		//-----------------------------------------------------------------//
		void final() {
//			auto path = base_;
//			path += hex_to_string_(0);
		}


#ifdef WIN32
		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルリストを表示
		*/
		//-----------------------------------------------------------------//
		void list() {
			int n = 0;
			for(auto fi : finfos_) {
				if(fi.handle_ == 0) continue;

				auto path = tree_unit_.find(fi.handle_);
				if(path.back() == '/') std::cout << 'd';
				else std::cout << '-';
				std::cout << ' ';
				if(fi.open_mode_ == open_mode::none) std::cout << 'N';
				else if(fi.open_mode_ == open_mode::read) std::cout << 'R';
				else if(fi.open_mode_ == open_mode::write) std::cout << 'W';
				else std::cout << 'X';
				std::cout << ' ';
				std::cout << boost::format("%6d ") % static_cast<unsigned int>(fi.fsize_);
				std::cout << path << std::endl;
				++n;
			}
			std::cout << "Total files: " << n << std::endl;
			std::cout << std::endl;
		}
#endif
	};
}
