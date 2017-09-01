#pragma once
//=====================================================================//
/*!	@file
	@brief	各種サウンドファイル統合的に扱う（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <memory>
#include "i_snd_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像ファイルを汎用的に扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class snd_files {

		struct snd_file {
			typedef std::shared_ptr<i_snd_io>  snd_io;
			snd_io		sio;
			std::string	ext;
		};
		typedef std::vector<snd_file>	sndios;
		sndios			sndios_;

		std::string		exts_;

		audio			aif_;

		snd_file::snd_io	stream_;
		tag				tag_;

		static uint32_t	tag_serial_;

		void add_sound_fileio_context_(snd_file::snd_io sio, const std::string& exts);
		void initialize_(const std::string& etxs);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	exts	拡張子
		*/
		//-----------------------------------------------------------------//
		snd_files(const std::string& exts = "wav,mp3,aac,m4a") :
			aif_(0), stream_(0)
			{ initialize_(exts); }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~snd_files() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしている画像フォーマットの数を返す
			@return フォーマット数
		*/
		//-----------------------------------------------------------------//
		int get_file_num() const { return static_cast<int>(sndios_.size()); }


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしている画像フォーマットの拡張子を返す
			@param[in]	n	ｎ番目のファイルフォーマットの拡張子
			@return 拡張子（小文字）
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext(size_t n) const {
			if(n < sndios_.size()) {
				return sndios_[n].ext.c_str();
			} else {
				return 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしている画像フォーマットの拡張子を返す
			@param[in]	n	ｎ番目のファイルフォーマットの拡張子
			@return 拡張子（小文字）
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file_exts() const { return exts_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サウンド・ファイルの種類を判定
			@param[in]	fin	ファイル入力コンテキスト
			@param[in]	ext	拡張子（無くても可）
			@return サウンド・ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin, const std::string& ext = "") const;


		//-----------------------------------------------------------------//
		/*!
			@brief	サウンド・ファイルの種類を判定
			@param[in]	filename	ファイル名
			@return サウンド・ファイルとして認識出来ない場合は「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(const std::string& filename) const {
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
			@brief	音楽ファイルの情報を取得する
			@param[in]	fin	file_io クラス
			@param[in]	fo	情報を受け取る構造体
			@param[in]	st	情報ステート
			@param[in]	ext	拡張子（無くても可）
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, audio_info& fo,
				  i_snd_io::info_state st = i_snd_io::info_state::all,
				  const std::string& ext = 0);


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルの情報を取得する
			@param[in]	filename	ファイル名
			@param[in]	fo	情報を受け取る構造体
			@param[in]	st	情報ステート
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(const std::string& filename, audio_info& fo,
				  i_snd_io::info_state st = i_snd_io::info_state::all) {
			bool f = false;
			utils::file_io fin;
			if(fin.open(filename, "rb")) {
				f = info(fin, fo, st, utils::get_file_ext(filename));
				fin.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルロードして展開する
			@param[in]	fin	file_io クラス
			@param[in]	ext	拡張子
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& ext, const std::string& opt = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルロードして展開する
			@param[in]	filename	ファイル名
			@param[in]	opt			フォーマット固有の設定文字列
			@return エラーなら「false」を返す
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
			@brief	音楽ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	ext	拡張子
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& ext, const std::string& opt = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルをセーブする
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& filename, const std::string& opt) {
			bool f = false;
			utils::file_io fout;
			if(fout.open(filename, "wb")) {
				f = save(fout, utils::get_file_ext(filename), opt);
				fout.close();
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・オープンする
			@param[in]	fin		file_io クラス
			@param[in]	size	バッファサイズ
			@param[in]	inf		オーディオ情報を受け取る
			@param[in]	ext		拡張子
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open_stream(utils::file_io& fin, int size, audio_info& inf, const std::string& ext = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルのタグを取得（事前に info, open_stream を実行）
			@return タグを返す
		*/
		//-----------------------------------------------------------------//
		const tag& get_tag() const { return tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを取得する
			@return ストリーム用オーディオ
		*/
		//-----------------------------------------------------------------//
		const audio get_stream();


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・リード
			@param[in]	fin		ファイルI/O
			@param[in]	offset	開始位置
			@param[in]	samples	読み込むサンプル数
			@return 読み込んだサンプル数
		*/
		//-----------------------------------------------------------------//
		size_t read_stream(utils::file_io& fin, size_t offset, size_t samples);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをクローズ
		*/
		//-----------------------------------------------------------------//
		void close_stream();


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースを取得
			@return	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		const audio get_audio() const { return aif_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオを設定
			@param[in]	aif	オーディオ
		*/
		//-----------------------------------------------------------------//
		void set_audio(const audio aif) { aif_ = aif; }

	};

}
