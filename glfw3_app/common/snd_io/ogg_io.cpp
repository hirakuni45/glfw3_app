//=====================================================================//
/*!	@file
	@brief	OGG ファイルを扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "snd_io/ogg_io.hpp"

namespace al {

	//-----------------------------------------------------------------//
	/*!
		@brief	OGG ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool ogg_io::probe(utils::file_io& fin)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	AAC ファイルの情報を取得する
		@param[in]	fin		file_io クラス
		@param[in]	info	情報を受け取る構造体
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool ogg_io::info(utils::file_io& fin, audio_info& info)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロードする
		@param[in]	fin	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool ogg_io::load(utils::file_io& fin, const std::string& opt)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	音楽ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool ogg_io::save(utils::file_io& fout, const std::string& opt)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム・オープンする
		@param[in]	fi		file_io クラス
		@param[in]	size	バッファサイズ
		@param[in]	info	オーディオ情報を受け取る
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool ogg_io::open_stream(utils::file_io& fi, int size, audio_info& info)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム・リード
		@param[in]	fin		ファイルI/O
		@param[in]	offset	開始位置
		@param[in]	samples	読み込むサンプル数
		@return 読み込んだサンプル数
	*/
	//-----------------------------------------------------------------//
	size_t ogg_io::read_stream(utils::file_io& fin, size_t offset, size_t samples)
	{
		return 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームをクローズ
	*/
	//-----------------------------------------------------------------//
	void ogg_io::close_stream()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void ogg_io::destroy()
	{
	}
}
