//=====================================================================//
/*!	@file
	@brief	TGA 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/tga_io.hpp"

namespace img {

	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::probe(utils::file_io& fin)
	{
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画像ファイルの情報を取得する
		@param[in]	fin	file_io クラス
		@param[in]	fo	情報を受け取る構造体
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::info(utils::file_io& fin, img::img_info& fo)
	{
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイル、ロード(utils::file_io)
		@param[in]	fin	ファイル I/O クラス
		@param[in]	ext	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::load(utils::file_io& fin, const std::string& ext)
	{
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイルをセーブする
		@param[in]	fout	ファイル I/O クラス
		@param[in]	ext		フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool tga_io::save(utils::file_io& fout, const std::string& ext)
	{
		return true;
	}
}
