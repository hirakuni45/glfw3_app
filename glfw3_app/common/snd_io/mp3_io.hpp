#pragma once
//=====================================================================//
/*!	@file
	@brief	MP3 ファイルを扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <cmath>
#include <mad.h>
#include "i_snd_io.hpp"
#include "img_io/img_files.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	MP3 音声ファイルクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class mp3_io : public i_snd_io {

	public:
		struct mp3_info {
			unsigned int	recover_frame_error;
			unsigned int	unrecover_frame_error;
			unsigned int	layer_1;
			unsigned int	layer_2;
			unsigned int	layer_3;
			unsigned int	single_chanel;
			unsigned int	dual_chanel;
			size_t			frame_count;
			long			skip_head;
			void reset() {
				recover_frame_error = 0;
				unrecover_frame_error = 0;
				layer_1 = layer_2 = layer_3 = 0;
				single_chanel = 0;
				dual_chanel = 0;
				frame_count = 0;
				skip_head = 0;
			}
		};

	private:

		static const int INPUT_BUFFER_SIZE = (5 * 8192);
		static const int STREAM_NUM = 8;

		mp3_info		mp3_info_;

		audio			audio_;

		audio			stream_;

		mad_stream		mad_stream_;
		mad_frame		mad_frame_;
		mad_synth		mad_synth_;
		mad_timer_t		mad_timer_;

		long			start_pos_;
		long			offset_;

		int				output_pos_;
		int				output_max_;
		int				output_all_;

		// サブバンド領域フィルター特性用。
		mad_fixed_t		subband_filter_[32];
		bool			subband_filter_enable_;
		bool			id3v1_;

		unsigned char	input_buffer_[INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
		audio			output_buffer_;

		img::img_files	img_files_;

		tag				tag_;

		std::vector<uint32_t>	seek_points_;

		void apply_filter_(mad_frame& frame);
		int fill_read_buffer_(utils::file_io& fin, mad_stream& strm);
		bool analize_frame_(utils::file_io& fin, audio_info& info, mp3_info& mp3info, info_state st);
		bool decode_(utils::file_io& fin, audio out);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		mp3_io() : subband_filter_enable_(false), id3v1_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~mp3_io() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "mp3"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	MP3 ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override {
			audio_info info;
			return analize_frame_(fin, info, mp3_info_, info_state::none);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MP3 ファイルの情報を取得する
			@param[in]	fin		file_io クラス
			@param[in]	info	情報を受け取る構造体
			@param[in]	st		画像情報ステート
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, audio_info& info, info_state st = info_state::all) override {
			return analize_frame_(fin, info, mp3_info_, st);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルのタグを取得
			@param[in]	fin	file_io クラス
			@return タグを返す
		*/
		//-----------------------------------------------------------------//
		const tag& get_tag() const override { return tag_; }


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
			@param[in]	inf		オーディオ情報を受け取る
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open_stream(utils::file_io& fi, int size, audio_info& inf) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム読み込みバッファを取得する
			@return ストリーム用オーディオ
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
			@brief	オーディオを取得する
			@return オーディオ
		*/
		//-----------------------------------------------------------------//
		const audio get_audio() const override { return audio_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオの登録
			@param[in]	au	オーディオ
		*/
		//-----------------------------------------------------------------//
		void set_audio(const audio au) override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override;

	};

}
