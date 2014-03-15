#pragma once
//=====================================================================//
/*!	@file
	@brief	WAV 音声ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
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

		typedef short			WORD;	///< 2 bytes
		typedef long			DWORD;	///< 4 bytes
		typedef unsigned long	GUID;	///< 4 bytes

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
			} samples;
			DWORD			channel_mask;      /* which channels are */
			GUID			sub_format;
		};

		wavefile_type			type_;
		wave_format_extensible	ext_;
		size_t					data_size_;
		size_t					data_offset_;
		int						riff_num_;

		i_audio*		audio_;

		i_audio*		stream_;
		int				stream_blocks_;

		const i_audio*	audio_source_;

		tag				tag_;

		bool parse_header_(utils::file_io& fin);
		bool create_wav_(utils::file_io& fout, const i_audio* src);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		wav_io() : audio_(0), stream_(0), stream_blocks_(0), audio_source_(0) { }


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
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, audio_info& info) override;


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
			@brief	ストリーム読み込みバッファを取得する
			@return ストリーム用オーディオ・インターフェース
		*/
		//-----------------------------------------------------------------//
		const i_audio* get_stream() const override { return stream_; }


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
		const i_audio* get_audio_if() const override { return audio_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・インターフェースを設定する
			@param[in]	aif	オーディオ・インターフェースクラス
		*/
		//-----------------------------------------------------------------//
		void set_audio_if(const i_audio* aif) override { audio_source_ = aif; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override;

	};

}
