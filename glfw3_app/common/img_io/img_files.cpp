//=====================================================================//
/*!	@file
	@brief	各種画像ファイル統合的に扱う
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/img_files.hpp"
#include "img_io/jpeg_io.hpp"
#include "img_io/png_io.hpp"
#include "img_io/bmp_io.hpp"
#include "img_io/tga_io.hpp"
/// #include "img_io/dds_io.hpp"
#include "img_io/openjpeg_io.hpp"
#include <boost/foreach.hpp>
/// #include <iostream>

namespace img {

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


	void img_files::add_image_file_io_context_(img_file::img_io imi, const std::string& exts)
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


	void img_files::initialize_(const std::string& exts)
	{
		add_image_file_io_context_(img_file::img_io(new bmp_io), exts);
		add_image_file_io_context_(img_file::img_io(new png_io), exts);
		add_image_file_io_context_(img_file::img_io(new jpeg_io), exts);
		add_image_file_io_context_(img_file::img_io(new openjpeg_io), exts);
		// TGA フォーマットはシグネチュアが無いので、最後に評価する事
		add_image_file_io_context_(img_file::img_io(new tga_io), exts);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画像ファイルの種類を判定
		@param[in]	fin	ファイル入力コンテキスト
		@param[in]	ext	拡張子（無くても可）
		@return 画像ファイルとして認識出来ない場合は「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool img_files::probe(utils::file_io& fin, const std::string& ext)
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
		@brief	画像ファイルの情報を取得する
		@param[in]	fin	ファイル入力コンテキスト
		@param[in]	fo	情報を受け取る構造体
		@param[in]	ext	拡張子、「０」の場合、全てのファイルで検査される
		@return 画像ファイルとして認識出来ない場合は「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool img_files::info(utils::file_io& fin, img::img_info& fo, const std::string& ext)
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
///				std::cout << io.ext << std::endl;
				if(io.igf->info(fin, fo)) return true;
			}
		}

		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・ロード
		@param[in]	fin	ファイル入力コンテキスト
		@param[in]	ext	ファイルタイプ（拡張子）
		@param[in]	opt	フォーマット固有の設定文字列
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool img_files::load(utils::file_io& fin, const std::string& ext, const std::string& opt)
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
		@brief	ファイル・セーブ
		@param[in]	fout	ファイル出力コンテキスト
		@param[in]	ext		ファイルタイプ（拡張子）
		@param[in]	opt		フォーマット固有の設定文字列
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool img_files::save(utils::file_io& fout, const std::string& ext, const std::string& opt)
	{
		if(!ext.empty() && img_) {
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

}
