#pragma once
//=====================================================================//
/*!	@file
	@brief	各種サウンドファイル統合的に扱う
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <memory>
#include "i_snd_io.hpp"
#include "wav_io.hpp"
#include "mp3_io.hpp"
#include "aac_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	音楽ファイルを汎用的に扱うクラス
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

		typename snd_file::snd_io	stream_;
		sound::tag_t		tag_;

		static inline uint32_t	tag_serial_;

		static bool check_file_exts_(const std::string& exts, const std::string& ext)
		{
			utils::strings ss = utils::split_text(exts, ",");
			for(const std::string& s : ss) {
				if(utils::no_capital_strcmp(ext, s) == 0) {
					return true;
				}
			}
			return false;
		}


		void add_sound_fileio_context_(typename snd_file::snd_io sio, const std::string& exts)
		{
			if(sio) {
				sio->initialize();
				utils::strings ss = utils::split_text(exts, ",");
				for(const std::string& s : ss) {
					if(check_file_exts_(sio->get_file_ext(), s)) {
						snd_file sd;
						sd.sio = sio;
						sd.ext = sio->get_file_ext();
						sndios_.push_back(sd);
						return;
					}
				}
			}
		}


		void initialize_(const std::string& exts)
		{
			exts_ = exts;

			add_sound_fileio_context_(typename snd_file::snd_io(new wav_io), exts);

			// +AAC codec
			add_sound_fileio_context_(typename snd_file::snd_io(new aac_io), exts);

			// MP3 はタグが、前、後、にあるのか不明な為、検出が難しい為、最後に調べる。
			add_sound_fileio_context_(typename snd_file::snd_io(new mp3_io), exts);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	exts	拡張子
		*/
		//-----------------------------------------------------------------//
		snd_files(const std::string& exts = "wav,mp3,aac,m4a") :
			aif_(0), stream_(0), tag_()
			{ initialize_(exts); }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~snd_files() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしているフォーマットの数を返す
			@return フォーマット数
		*/
		//-----------------------------------------------------------------//
		int get_file_num() const { return static_cast<int>(sndios_.size()); }


		//-----------------------------------------------------------------//
		/*!
			@brief	サポートしているフォーマットの拡張子を返す
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
			@brief	サポートしているフォーマットの拡張子を返す
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
		bool probe(utils::file_io& fin, const std::string& ext = "") const
		{
			size_t n = sndios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < n; ++i) {
					const snd_file& io = sndios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.sio->probe(fin)) return true;
						else n = i;
						break;
					}
				}
			}
			for(size_t i = 0; i < n; ++i) {
				if(n != i) {
					const snd_file& io = sndios_[i];
					if(io.sio->probe(fin)) {
						return true;
					}
				}
			}

			return false;
		}


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
				  const std::string& ext = 0)
		{
			size_t n = sndios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < sndios_.size(); ++i) {
					snd_file& io = sndios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.sio->info(fin, fo, st)) {
							tag_ = io.sio->get_tag();
							// snd_files のタグ更新シリアルを上書き
							++tag_serial_;
							tag_.serial_ = tag_serial_;
							return true;
						} else n = i;
						break;
					}
				}
			}
			for(size_t i = 0; i < sndios_.size(); ++i) {
				if(n != i) {
					snd_file& io = sndios_[i];
					if(io.sio->info(fin, fo, st)) {
						tag_ = io.sio->get_tag();
						// snd_files のタグ更新シリアルを上書き
						++tag_serial_;
						tag_.serial_ = tag_serial_;
						return true;
					}
				}
			}
			return false;
		}


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
		bool load(utils::file_io& fin, const std::string& ext, const std::string& opt = "")
		{
			aif_ = 0;
			size_t n = sndios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < sndios_.size(); ++i) {
					snd_file& io = sndios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.sio->load(fin, opt)) {
							aif_ = io.sio->get_audio();
							return true;
						}
						n = i;
						break;
					}
				}
			}
			for(size_t i = 0; i < sndios_.size(); ++i) {
				if(n != i) {
					snd_file& io = sndios_[i];
					if(io.sio->load(fin, opt)) {
						aif_ = io.sio->get_audio();
						return true;
					}
				}
			}
			return false;
		}


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
		bool save(utils::file_io& fout, const std::string& ext, const std::string& opt = "")
		{
			if(!ext.empty() && aif_) {
				for(size_t i = 0; i < sndios_.size(); ++i) {
					snd_file& io = sndios_[i];
					if(check_file_exts_(io.ext, ext)) {
						io.sio->set_audio(aif_);
						if(io.sio->save(fout, opt)) {
							return true;
						}
					}
				}
			}
			return false;
		}


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
		bool open_stream(utils::file_io& fin, int size, audio_info& inf, const std::string& ext = "")
		{
			stream_ = 0;
			size_t n = sndios_.size();
			if(!ext.empty()) {
				for(size_t i = 0; i < sndios_.size(); ++i) {
					snd_file& io = sndios_[i];
					if(check_file_exts_(io.ext, ext)) {
						if(io.sio->open_stream(fin, size, inf)) {
							tag_ = io.sio->get_tag();
							// snd_files のタグ更新シリアルを上書き
							++tag_serial_;
							tag_.serial_ = tag_serial_;
							stream_ = io.sio;
							return true;
						}
						n = i;
						break;
					}
				}
			}

			for(size_t i = 0; i < sndios_.size(); ++i) {
				if(n != i) {
					snd_file& io = sndios_[i];
					if(io.sio->open_stream(fin, size, inf)) {
						tag_ = io.sio->get_tag();
						// snd_files のタグ更新シリアルを上書き
						++tag_serial_;
						tag_.serial_ = tag_serial_;
						stream_ = io.sio;
						return true;
					}
				}
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルのタグを取得（事前に info, open_stream を実行）
			@return タグを返す
		*/
		//-----------------------------------------------------------------//
		const auto& get_tag() const { return tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを取得する
			@return ストリーム用オーディオ
		*/
		//-----------------------------------------------------------------//
		const audio get_stream()
		{
			if(stream_) {
				return stream_->get_stream();
			} else {
				return 0;
			}
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
		size_t read_stream(utils::file_io& fin, size_t offset, size_t samples)
		{
			if(stream_) {
				return stream_->read_stream(fin, offset, samples);
			} else {
				return 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをクローズ
		*/
		//-----------------------------------------------------------------//
		void close_stream()
		{
			if(stream_) {
				stream_->close_stream();
				stream_ = 0;
			}
		}


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
