#pragma once
//=====================================================================//
/*!	@file
	@brief	WAV 音声ファイルを扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include "i_snd_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	WAV 音声ファイルクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wav_io : public i_snd_io {

		typedef int16_t	   	WORD;	///< 2 bytes
		typedef int32_t	   	DWORD;	///< 4 bytes
		typedef uint32_t	GUID;	///< 4 bytes

		enum wavefile_type {
			wf_null = 0,
			wf_ex  = 1,
			wf_ext = 2
		};

		struct wave_format_ex {
			WORD	format_tag;
			WORD	channels;
			DWORD	samples_per_sec;
			DWORD	avg_bytes_per_sec;
			WORD	block_align;
			WORD	bits_per_sample;
		};

		struct wave_format_extensible {
			wave_format_ex	format;
			WORD			size;
			union {
				WORD	valid_bits_per_sample;       /* bits of precision  */
				WORD	samples_per_block;          /* valid if wBitsPerSample==0 */
				WORD	reserved;                 /* If neither applies, set to zero. */
			};
			DWORD			channel_mask;      /* which channels are */
			GUID			sub_format;
		};

		wavefile_type			type_;
		wave_format_extensible	ext_;
		size_t					data_size_;
		size_t					data_offset_;
		int						riff_num_;

		audio			audio_;

		audio			stream_;
		int				stream_blocks_;

		audio		   	audio_source_;

		tag				tag_;

		bool parse_header_(utils::file_io& fin);
		bool create_wav_(utils::file_io& fout, const audio src);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		wav_io() : stream_blocks_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~wav_io() { destroy(); }


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
		const char* get_file_ext() const override { return "wav"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	WAV ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override {
			long pos = fin.tell();
			bool f = parse_header_(fin);
			fin.seek(pos, utils::file_io::seek::set);
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	WAV ファイルの情報を取得する
			@param[in]	fin		file_io クラス
			@param[in]	info	情報を受け取る構造体
			@param[in]	st		画像情報ステート
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
			@param[in]	成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	セーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return 成功したら「true」が返る。
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
			@brief	ストリームを取得する
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
			@brief	オーディオを設定する
			@param[in]	au	オーディオ
		*/
		//-----------------------------------------------------------------//
		void set_audio(const audio au) override { audio_source_ = au; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override;

	};

}
