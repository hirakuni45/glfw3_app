#pragma once
//=====================================================================//
/*!	@file
	@brief	AAC ファイルを扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <neaacdec.h>
// #include <mp4ff.h>
#include <faad.h>
#include "i_snd_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	AAC 音声ファイルクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class aac_io : public i_snd_io {

		struct decode_mp4_t {
			NeAACDecHandle	h_decoder;
			mp4ff_callback_t mp4cb;
			mp4ff_t*		infile;
			NeAACDecConfigurationPtr config;
			NeAACDecFrameInfo frame_info;
			mp4AudioSpecificConfig	mp4ASC;
			int			track;
			unsigned char*	tmp;
			uint32_t	samples;
			uint32_t	useAacLength;
			uint32_t	timescale;
			uint32_t	channels;
			uint32_t	samplerate;
			uint32_t	framesize;
			uint32_t	count;
			uint32_t	delay;
			void* 		sample;
			bool		no_gapless;
			bool		initial;
			decode_mp4_t() : h_decoder(0),
				infile(0), config(0), frame_info(), track(-1), tmp(0), samples(0),
				useAacLength(0), timescale(0), channels(0), samplerate(0), framesize(0),
				count(0), delay(0), sample(0),
				no_gapless(false), initial(false) { }
		};

		decode_mp4_t	mp4_t_;

		audio			audio_;
		audio			stream_;
		audio			buffer_;
		uint32_t		index_;
		uint32_t		stream_cnt_;
		uint32_t		buffer_cnt_;

		tag				tag_;

		void destroy_mp4_(decode_mp4_t& dt);
		bool decode_audio_mp4_(decode_mp4_t& dt, uint32_t idx);
		bool decode_track_mp4_(utils::file_io& fin, decode_mp4_t& dt);
		bool decode_mp4_param_(decode_mp4_t& dt);
		audio decode_mp4_file_(utils::file_io& fin);
		bool create_mp4_info_(utils::file_io& fin, decode_mp4_t& dt, audio_info& info, info_state st);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		aac_io() : index_(0), stream_cnt_(0), buffer_cnt_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~aac_io() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "aac,m4a"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	AAC ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	AAC ファイルの情報を取得する
			@param[in]	fin		file_io クラス
			@param[in]	info	情報を受け取る構造体
			@param[in]	st		情報ステート
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, audio_info& info, info_state st = info_state::all) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルのタグを取得
			@return タグを返す
		*/
		//-----------------------------------------------------------------//
		const tag& get_tag() const override {
			return tag_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ロードする
			@param[in]	fin	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・オープンする
			@param[in]	fi		file_io クラス
			@param[in]	size	バッファサイズ
			@param[in]	info	オーディオ情報を受け取る
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open_stream(utils::file_io& fi, int size, audio_info& info) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム読み込みバッファを取得する
			@return ストリーム用オーディオ・インターフェース
		*/
		//-----------------------------------------------------------------//
		const audio get_stream() const override { return stream_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・リード
			@param[in]	fin		ファイルI/O
			@param[in]	offset	開始位置
			@param[in]	samples	読み込むサンプル数
			@return 読み込んだサンプル数
		*/
		//-----------------------------------------------------------------//
		size_t read_stream(utils::file_io& fin, size_t offset, size_t samples) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをクローズ
		*/
		//-----------------------------------------------------------------//
		void close_stream() override;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・インターフェースを取得する
			@return オーディオ・インターフェースクラス
		*/
		//-----------------------------------------------------------------//
		const audio get_audio() const override { return audio_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオの登録
			@param[in]	aif	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_audio(const audio aif) override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override;

	};

}
