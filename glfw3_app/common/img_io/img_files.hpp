#pragma once
//=====================================================================//
/*!	@file
	@brief	各種画像ファイル統合的に扱う（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <memory>
#include "img_io/i_img_io.hpp"
#include "utils/file_io.hpp"
#include "img_io/jpeg_io.hpp"
#include "img_io/png_io.hpp"
#include "img_io/bmp_io.hpp"
#include "img_io/tga_io.hpp"
#include "img_io/pvr_io.hpp"
/// #include "img_io/dds_io.hpp"
#include "img_io/openjpeg_io.hpp"
#include <boost/foreach.hpp>

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像ファイルを汎用的に扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class img_files {

		struct img_file {
			typedef std::shared_ptr<i_img_io>  img_io;
			img_io		igf;
			std::string	ext;
		};

		typedef std::vector<img_file>	imgios;
		imgios	   	imgios_;

		shared_img	img_;

		static bool check_file_exts_(const std::string& exts, const std::string& ext)
		{
			utils::strings ss = utils::split_text(exts, ",");
			BOOST_FOREACH(std::string& s, ss) {
				if(utils::no_capital_strcmp(ext, s) == 0) {
					return true;
				}
			}
			return false;
		}


		void add_image_file_io_context_(img_file::img_io imi, const std::string& exts)
		{
			if(imi) {
				utils::strings ss = utils::split_text(exts, ",");
				BOOST_FOREACH(std::string& s, ss) {
					if(check_file_exts_(imi->get_file_ext(), s)) {
						img_file io;
						io.igf = imi;
						io.ext = imi->get_file_ext();
						imgios_.push_back(io);
						return;
					}
				}
			}
		}


		void initialize_(const std::string& exts)
		{
			add_image_file_io_context_(img_file::img_io(new bmp_io), exts);
			add_image_file_io_context_(img_file::img_io(new png_io), exts);
			add_image_file_io_context_(img_file::img_io(new jpeg_io), exts);
			add_image_file_io_context_(img_file::img_io(new openjpeg_io), exts);
			add_image_file_io_context_(img_file::img_io(new pvr_io), exts);
			// TGA フォーマットはシグネチュアが無いので、最後に評価する事
			add_image_file_io_context_(img_file::img_io(new tga_io), exts);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_files(const std::string& exts = "bmp,png,jpg,jpeg,j2k,jp2,pvr,tga") {
			initialize_(exts);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~img_files() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしている画像フォーマットの数を返す
			@return フォーマット数
		*/
		//-----------------------------------------------------------------//
		int get_file_num() const { return static_cast<int>(imgios_.size()); }


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしている画像フォーマットの拡張子を返す
			@param[in]	n	ｎ番目のファイルフォーマットの拡張子
			@return 拡張子（小文字）
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file_ext(size_t n) const {
			if(n < imgios_.size()) {
				return imgios_[n].ext;
			} else {
				static std::string empty;
				return empty;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの種類を判定
			@param[in]	fin	ファイル入力コンテキスト
			@param[in]	ext	拡張子、「０」の場合、全てのファイルで検査される
			@return 画像ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin, const std::string& ext = "")
		{
			size_t n = imgios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < imgios_.size(); ++i) {
					img_file& io = imgios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.igf->probe(fin)) return true;
						else n = i;
						break;
					}
				}
			}
			for(size_t i = 0; i < imgios_.size(); ++i) {
				if(n != i) {
					img_file& io = imgios_[i];
					if(io.igf->probe(fin)) return true;
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの種類を判定
			@param[in]	filename	ファイル名
			@return 画像ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(const std::string& filename) {
			bool f = false;
			utils::file_io fin;
			if(fin.open(filename, "rb")) {
				f = probe(fin, utils::get_file_ext(filename));
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの種類を判定
			@param[in]	filename	ファイル名
			@return 画像ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(const utils::wstring& filename) {
			std::string s;
			utils::utf16_to_utf8(filename, s);
			bool f = false;
			utils::file_io fin;
			if(fin.open(s, "rb")) {
				f = probe(fin, utils::get_file_ext(s));
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの情報を取得する
			@param[in]	fin	ファイル入力コンテキスト
			@param[in]	fo	情報を受け取る構造体
			@param[in]	ext	拡張子、「０」の場合、全てのファイルで検査される
			@return 画像ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, img::img_info& fo, const std::string& ext = "")
		{
			size_t n = imgios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < imgios_.size(); ++i) {
					img_file& io = imgios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.igf->info(fin, fo)) return true;
						else n = i;
						break;
					}
				}
			}

			for(size_t i = 0; i < imgios_.size(); ++i) {
				if(n != i) {
					img_file& io = imgios_[i];
///					std::cout << io.ext << std::endl;
					if(io.igf->info(fin, fo)) return true;
				}
			}

			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの情報を取得する
			@param[in]	filename	ファイル名
			@param[in]	fo	情報を受け取る構造体
			@return 画像ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(const std::string& filename, img::img_info& fo) {
			bool f = false;
			utils::file_io fin;
			if(fin.open(filename, "rb")) {
				f = info(fin, fo, utils::get_file_ext(filename));
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの情報を取得する
			@param[in]	filename	ファイル名
			@param[in]	fo	情報を受け取る構造体
			@return 画像ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(const utils::wstring& filename, img::img_info& fo) {
			std::string s;
			utils::utf16_to_utf8(filename, s);
			bool f = false;
			utils::file_io fin;
			if(fin.open(s, "rb")) {
				f = info(fin, fo, utils::get_file_ext(s));
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・ロード
			@param[in]	fin	ファイル入力コンテキスト
			@param[in]	ext	ファイルタイプ（拡張子）
			@param[in]	opt	フォーマット固有の設定文字列
			@return オープン出来ない場合は、「false」
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& ext = "", const std::string& opt = "")
		{
			img_ = nullptr;
			size_t n = imgios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < imgios_.size(); ++i) {
					img_file& io = imgios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.igf->load(fin, opt)) {
							img_ = io.igf->get_image();
							return true;
						}
						n = i;
						break;
					}
				}
			}

			for(size_t i = 0; i < imgios_.size(); ++i) {
				if(n != i) {
					img_file& io = imgios_[i];
					if(io.igf->load(fin, opt)) {
						img_ = io.igf->get_image();
						return true;
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・ロード
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の設定文字列
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& filename, const std::string& opt = "") {
			bool f = false;
			utils::file_io fin;
			if(fin.open(filename, "rb")) {
				f = load(fin, utils::get_file_ext(filename), opt);
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・ロード（UTF-16)
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の設定文字列
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const utils::wstring& filename, const std::string& opt = "") {
			std::string s;
			utils::utf16_to_utf8(filename, s);
			bool f = false;
			utils::file_io fin;
			if(fin.open(s, "rb")) {
				f = load(fin, utils::get_file_ext(s), opt);
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・セーブ
			@param[in]	fin	ファイル出力コンテキスト
			@param[in]	ext	ファイルタイプ（拡張子）
			@param[in]	opt	フォーマット固有の設定文字列
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& ext, const std::string& opt = "")
		{
			if(!ext.empty() && img_ != nullptr) {
				for(size_t i = 0; i < imgios_.size(); ++i) {
					img_file& io = imgios_[i];
					if(check_file_exts_(io.ext, ext)) {
						io.igf->set_image(img_);
						if(io.igf->save(fout, opt)) {
							return true;
						} else {
							return false;
						}
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・セーブ
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の設定文字列
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& filename, const std::string& opt = "") {
			bool f = false;
   			utils::file_io fo;
			if(fo.open(filename, "wb")) {
   				f = save(fo, utils::get_file_ext(filename), opt);
				fo.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・セーブ(UTF-16)
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の設定文字列
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const utils::wstring& filename, const std::string& opt = "") {
			std::string s;
			utils::utf16_to_utf8(filename, s);
			return save(s, opt);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースを取得
			@return	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		const shared_img get_image() const { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースを設定
			@param[in]	img	イメージ・インターフェース
		*/
		//-----------------------------------------------------------------//
		void set_image(shared_img img) { img_ = img; }

	};

}

