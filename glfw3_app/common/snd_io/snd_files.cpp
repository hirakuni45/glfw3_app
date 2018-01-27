//=====================================================================//
/*!	@file
	@brief	各種サウンドファイル統合的に扱う
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "snd_files.hpp"
#include "wav_io.hpp"
#include "mp3_io.hpp"
// #include "aac_io.hpp"

namespace al {

	uint32_t snd_files::tag_serial_ = 0;


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


	void snd_files::add_sound_fileio_context_(snd_file::snd_io sio, const std::string& exts)
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


	void snd_files::initialize_(const std::string& exts)
	{
		exts_ = exts;

		add_sound_fileio_context_(snd_file::snd_io(new wav_io), exts);
//		add_sound_fileio_context_(snd_file::snd_io(new aac_io), exts);

		// MP3 はタグが、前、後、にあるのか不明な為、検出が難しい為、最後に調べる。
		add_sound_fileio_context_(snd_file::snd_io(new mp3_io), exts);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サウンド・ファイルの種類を判定
		@param[in]	fin	ファイル入力コンテキスト
		@param[in]	ext	拡張子（省略した場合、全ファイルと比べる）
		@return サウンド・ファイルとして認識出来ない場合は「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool snd_files::probe(utils::file_io& fin, const std::string& ext) const
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
		@brief	音楽ファイルの情報を取得する
		@param[in]	fin	file_io クラス
		@param[in]	fo	情報を受け取る構造体
		@param[in]	apic	画像情報を受けたらない場合「false」
		@param[in]	ext	拡張子（無くても可）
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool snd_files::info(utils::file_io& fin, audio_info& fo, i_snd_io::info_state st, const std::string& ext)
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
		@brief	音楽ファイルロードして展開する
		@param[in]	fin	file_io クラス
		@param[in]	ext	拡張子
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool snd_files::load(utils::file_io& fin, const std::string& ext, const std::string& opt)
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
		@brief	音楽ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	ext	拡張子
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool snd_files::save(utils::file_io& fout, const std::string& ext, const std::string& opt)
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
		@brief	ストリーム・オープンする
		@param[in]	fin		file_io クラス
		@param[in]	size	バッファサイズ
		@param[in]	inf		オーディオ情報を受け取る
		@param[in]	ext		拡張子
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool snd_files::open_stream(utils::file_io& fin, int size, audio_info& inf, const std::string& ext)
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
		@brief	ストリーム読み込みバッファを取得する
		@return ストリーム用オーディオ・インターフェース
	*/
	//-----------------------------------------------------------------//
	const audio snd_files::get_stream()
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
	size_t snd_files::read_stream(utils::file_io& fin, size_t offset, size_t samples)
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
	void snd_files::close_stream()
	{
		if(stream_) {
			stream_->close_stream();
			stream_ = 0;
		}
	}
}
