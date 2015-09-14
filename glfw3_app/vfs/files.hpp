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

#ifndef NDEBUG
#define DEBUG_FILES_
#else
#ifndef __psp2__
// リリース版でも、デバッグ出力
#define DEBUG_FILES_
#endif
#endif

#ifdef DEBUG_FILES_
#include <iomanip>
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

		fio		fio_;

		void resize_infos_(uint32_t n) {
			if(n >= finfos_.size()) {
				finfos_.resize(n + 1);
			}
		}


		void remove_(uint32_t hnd) {
			std::unordered_set<uint16_t> rm_list;
			if(finfos_[hnd].path_.back() == '/') { // directory
				finfos_[hnd].reset();
				return;
			} else {
				for(const finfo::block& bk : finfos_[hnd].blocks_) {
					rm_list.insert(bk.fileno_);
				}
			}

			for(finfo& fi : finfos_) {
				if(fi.handle_ == hnd) continue;
				for(const finfo::block& bk : fi.blocks_) {
					rm_list.erase(bk.fileno_);
				}
			}

			finfos_[hnd].reset();

			for(uint16_t h : rm_list) {
				fio_.remove_file(h);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	base	ベース・パス
		*/
		//-----------------------------------------------------------------//
		files(const std::string& base) : tree_unit_(), finfos_(), fio_(finfos_, base) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	開始
			@param[in]	read	ファイル構造を読み込む場合「true」
		*/
		//-----------------------------------------------------------------//
		void start(bool read = false) {
			fio_.init();

			tree_unit_.clear();
			finfos_.clear();

			// ディレクトリー情報を読み込み
			finfos fos;
			if(read) {
				fio_.read_dir(fos);
				if(!fos.empty()) {
					resize_infos_(fos.size());
					for(const auto& fi : fos) {
						if(fi.path_.empty()) continue;
						finfo f;
						f = fi;
						if(fi.path_.back() == '/') {
							f.handle_ = tree_unit_.make_directory(fi.path_);
						} else {
							f.handle_ = tree_unit_.install(fi.path_);
						}
						finfos_[f.handle_] = f;
					}
				}
			}
			if(fos.empty()) {
				mkdir("/");
			}
			cd("/");
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
			if(hnd) {
				resize_infos_(hnd);
				finfos_[hnd].handle_ = hnd;
				finfos_[hnd].path_ = tree_unit_.create_full_path(path);
				if(finfos_[hnd].path_.back() != '/') finfos_[hnd].path_ += '/';
			}
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
				remove_(hnd);
			}
			return hnds.size();
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
				remove_(hnd);
			}
			return hnds.size();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルサイズを取得
			@param[in]	path	パス
			@return 「-1」なら失敗
		*/
		//-----------------------------------------------------------------//
		int file_size(const std::string& path) {
			if(path.empty()) return -1;
			return fio_.file_size(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクトリーのファイルリストを作成
			@param[in]	path	ルート・パス
			@param[in]	full	フル・パスの場合「true」
			@return ファイルリスト
		*/
		//-----------------------------------------------------------------//
		utils::strings create_directory_list(const std::string& root, bool full) {
			return tree_unit_.get_sub_directory(root, full);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをコピー
			@param[in]	src	ソース・パス
			@param[in]	dst	ディストネーション・パス
			@param[in]	ovw	オーバーライトの場合「true」
			@return 失敗なら「false」
		*/
		//-----------------------------------------------------------------//
		bool copy(const std::string& src, const std::string& dst, bool ovw = false) {
			int srchnd = open(src, open_mode::read);
			if(srchnd <= 0) return false;
			if(ovw) {
				remove(dst);
			}
			int dsthnd = open(dst, open_mode::write);
			if(dsthnd <= 0) return false;

			std::array<uint8_t, 512> buffer;
			int rl = 0;
			do {
				rl = read(srchnd, &buffer[0], buffer.size());
				write(dsthnd, &buffer[0], rl);
			} while(rl > 0) ;
			close(dsthnd);
			close(srchnd);

			return true;
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
				if(hnd == 0) return 0;
				resize_infos_(hnd);
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::none) return 0;
				fi.path_ = tree_unit_.create_full_path(path);
				fi.handle_ = hnd;
				fi.open_mode_ = opm;
				fi.fsize_ = 0;
				fi.create_cash();
				fi.cpos_ = 0;
				fi.fpos_ = 0;
				fi.blocks_.clear();
			} else if(opm == open_mode::read) {
				hnd = tree_unit_.find(path);
				if(hnd == 0) return 0;
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::none) return 0;
				fi.open_mode_ = opm;
				fi.create_cash();
				fi.cpos_ = 0;
				fi.fpos_ = 0;
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
			if(src == nullptr) return ret;
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::write) {
					return ret;
				}

				ret = 0;
				while(size > 0) {
					uint32_t cs;
					if(size >= (fi.cash_size() - fi.cpos_)) {
						cs = fi.cash_size() - fi.cpos_;
					} else {
						cs = size;
					}
					std::memcpy(fi.cash_ptr(fi.cpos_), src, cs);
					fi.cpos_ += cs;
					fi.fpos_ += cs;
					size -= cs;
					ret += static_cast<int>(cs);
					if(fi.cpos_ >= fi.cash_size()) {
						fio_.write_cash(fi, false);
					}
				}
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルへの読み込み
			@param[in]	hnd	ファイル・ハンドル
			@param[in]	dst	ディストネーション・ポインター
			@param[in]	size	サイズ
			@return 読み込んだバイト数
		*/
		//-----------------------------------------------------------------//
		int read(uint32_t hnd, void* dst, uint32_t size) {
			int ret = -1;
			if(dst == nullptr) return ret;
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::read) {
					return ret;
				}

				ret = 0;
				while(size > 0) {
					if(fi.cpos_ == 0) {
						if(!fio_.read_cash(fi)) {
							return -1;
						}
					}
					if(fi.fpos_ >= fi.fsize_) break;
					if(fi.cpos_ > 0) {
						uint32_t cps;
						if(size < fi.cpos_) cps = size;
						else cps = fi.cpos_;
						std::memcpy(dst, fi.cash_ptr(fi.fpos_ % fi.cash_size()), cps);
						fi.cpos_ -= cps;
						fi.fpos_ += cps;
						size -= cps;
						ret += static_cast<int>(cps);
					}
				}
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルポインターの移動
			@param[in]	hnd	ファイル・ハンドル
			@param[in]	offset	オフセット
			@param[in]	mode	シーク・モード
			@return 移動した位置（-1の場合エラー）
		*/
		//-----------------------------------------------------------------//
		int seek(uint32_t hnd, uint32_t offset, seek_mode mode) {
			int ret = -1;
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::read) {
					return ret;
				}

				uint32_t pos = 0;
				if(mode == seek_mode::set) {
					pos = offset;
				} else if(mode == seek_mode::cur) {
					pos = fi.fpos_ + offset;
				} else if(mode == seek_mode::end) {
					if(offset > fi.fsize_) pos = 0;
					else pos = fi.fsize_ - offset;
				} else {
					return ret;
				}
				if(pos > fi.fsize_) pos = fi.fsize_;
				fi.fpos_ = pos;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル位置を取得
			@param[in]	hnd	ファイル・ハンドル
			@return 成功なら「正」の値
		*/
		//-----------------------------------------------------------------//
		int tell(uint32_t hnd) const {
			int ret = -1;
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				const finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::none) {
					ret = static_cast<int>(fi.fpos_);
				}
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの終端を検査
			@param[in]	hnd	ファイル・ハンドル
			@return 成功なら「正」の値
		*/
		//-----------------------------------------------------------------//
		int eof(uint32_t hnd) const {
			int ret = -1;
			if(hnd < finfos_.size() && finfos_[hnd].handle_ == hnd) {
				const finfo& fi = finfos_[hnd];
				if(fi.open_mode_ != vfs::open_mode::none) {
					if(fi.fpos_ == fi.fsize_) ret = 1;
					else ret = 0;
				}				
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
					fi.destroy_cash();
				} else if(fi.open_mode_ == open_mode::read) {
					fio_.read_close(fi);
					fi.destroy_cash();
				} else {
					return false;
				}
				fi.open_mode_ = open_mode::none;
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ディレクリー情報の書き込み
		*/
		//-----------------------------------------------------------------//
		void final() {
			fio_.write_dir(finfos_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルリストを表示
		*/
		//-----------------------------------------------------------------//
		void ls() const {
#ifdef DEBUG_FILES_
			int fnmx = 14;  // file name max
			int n = 0;
			for(const auto& fi : finfos_) {
				if(fi.handle_ == 0) continue;

				const std::string& path = fi.path_;
				if(path.empty()) continue;

				if(is_dir(path)) std::cout << 'd';
				else std::cout << '-';
				std::cout << ' ';
				if(fi.open_mode_ == open_mode::none) std::cout << 'N';
				else if(fi.open_mode_ == open_mode::read) std::cout << 'R';
				else if(fi.open_mode_ == open_mode::write) std::cout << 'W';
				else std::cout << 'X';
				std::cout << " (";
				std::cout << std::setw(3) << static_cast<unsigned int>(fi.handle_) << ") ";

				std::cout << std::setw(9) << static_cast<unsigned int>(fi.fsize_) << ' ';
				std::cout << std::setw(fnmx) << std::left << path << std::right << " idx, ofs, bks ";
				for(const auto& bk : fi.blocks_) {
					std::cout << static_cast<int>(bk.fileno_ + 1) << ", " << static_cast<int>(bk.offset_) << ", " <<
						static_cast<int>(bk.blocks_) << " : ";
				}
				std::cout << std::endl;
				++n;
			}
			std::cout << "Total files: " << n << std::endl;
			std::cout << std::endl;
#endif
		}
	};
}
