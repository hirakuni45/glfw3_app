#pragma once
//=====================================================================//
/*!	@file
	@brief	音楽ファイルを扱うインターフェースクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "utils/file_io.hpp"
#include "i_audio.hpp"
#include "audio.hpp"
#include "tag.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	音楽ファイル・インターフェース・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct i_snd_io {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	情報取得フラグ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class info_state : uint8_t {
			none = 0,	///< 取得しない
			apic = 1,	///< 画像情報
			time = 2,	///< フレームスキャン（時間）
			all  = apic | time	///< 全て取得
		};


		//-----------------------------------------------------------------//
		/*!
			@brief	仮想デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~i_snd_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化プロセス
		*/
		//-----------------------------------------------------------------//
		virtual void initialize() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		virtual const char* get_file_ext() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルの適正を確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		virtual bool probe(utils::file_io& fin) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルの情報を取得する
			@param[in]	fin	file_io クラス
			@param[in]	fo	情報を受け取る構造体
			@param[in]	apic	画像情報を受けたらない場合「false」
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		virtual bool info(utils::file_io& fin, audio_info& fo, info_state st = info_state::all) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルのタグを取得
			@return タグを返す
		*/
		//-----------------------------------------------------------------//
		virtual const sound::tag_t& get_tag() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルロードして展開する
			@param[in]	fin	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		virtual bool load(utils::file_io& fin, const std::string& opt = "") = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		virtual bool save(utils::file_io& fout, const std::string& opt = "") = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・オープンする
			@param[in]	fin		file_io クラス
			@param[in]	size	バッファサイズ
			@param[in]	inf		オーディオ情報を受け取る
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool open_stream(utils::file_io& fin, int size, audio_info& inf) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを取得する
			@return ストリーム用オーディオ
		*/
		//-----------------------------------------------------------------//
		virtual const audio get_stream() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・リード
			@param[in]	fin		ファイルI/O
			@param[in]	offset	開始位置
			@param[in]	samples	読み込むサンプル数
			@return 読み込んだサンプル数
		*/
		//-----------------------------------------------------------------//
		virtual size_t read_stream(utils::file_io& fin, size_t offset, size_t samples) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをクローズ
		*/
		//-----------------------------------------------------------------//
		virtual void close_stream() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオを取得
			@return	オーディオ
		*/
		//-----------------------------------------------------------------//
		virtual const audio get_audio() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオの登録
			@param[in]	au オーディオ
		*/
		//-----------------------------------------------------------------//
		virtual void set_audio(const audio au) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄プロセス
		*/
		//-----------------------------------------------------------------//
		virtual void destroy() = 0;

	};
}
