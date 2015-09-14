#pragma once
//=====================================================================//
/*!	@file
	@brief	FFmpeg Library/decoder クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavfilter/avfilter.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
};
#include "utils/vtx.hpp"

namespace av {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	decoder クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class decoder {

		std::string			path_;
		AVFormatContext*	format_ctx_;
		AVCodecContext*		codec_ctx_;
		AVFrame* 			frame_;
		AVFrame* 			image_;
		unsigned char*		buffer_;
		vtx::ipos			size_;
		struct SwsContext*	sws_ctx_;
		uint32_t			count_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		decoder() : path_(), format_ctx_(nullptr), codec_ctx_(nullptr),
					frame_(nullptr), image_(nullptr), buffer_(nullptr),
					size_(0), sws_ctx_(nullptr),
					count_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~decoder() {
			close();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() {
			avcodec_register_all();
			av_register_all();
			avfilter_register_all();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム情報の表示
		*/
		//-----------------------------------------------------------------//
		void info() {
			av_dump_format(format_ctx_, 0, path_.c_str(), 1);
//			fflush(stderr);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@param[in]	path	ファイル名パス
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& path) {
			if(path.empty()) return false;

			path_ = path;

			// ビデオファイルを開く
			if(avformat_open_input(&format_ctx_, path.c_str(), NULL, NULL) != 0) {
				// std::cerr << "ERROR: avformat_open_input(): '" << file_name << '\'' << std::endl;
				return false;
			}

			// ストリーム情報の取得
			if(avformat_find_stream_info(format_ctx_, NULL) < 0) {
				// std::cerr << "ERROR: avformat_find_stream_info(): '" << file_name << '\'' << std::endl;
				close();
				return false;
			}

			// コーデックの検索
			codec_ctx_ = format_ctx_->streams[0]->codec;
			AVCodec* codec = avcodec_find_decoder(codec_ctx_->codec_id);

			// コーデックを開く
			if(avcodec_open2(codec_ctx_, codec, NULL) < 0) {
				// std::cerr << "ERROR: avcodec_open2(): '" << file_name << '\'' << std::endl;
				close();
				return false;
			}

			// ログ・レベルの設定
			av_log_set_level(1);

			// フレームの確保
			frame_ = avcodec_alloc_frame();
			// イメージの確保
			image_ = avcodec_alloc_frame();

			size_.x = codec_ctx_->width;
			size_.y = codec_ctx_->height;
			// イメージ用バッファの確保
			buffer_ = (unsigned char *)av_malloc(avpicture_get_size(PIX_FMT_RGB24, size_.x, size_.y));
			// バッファとフレームを関連付ける
			avpicture_fill((AVPicture*)image_, buffer_, PIX_FMT_RGB24, size_.x, size_.y);

			// スケーリング用コンテキストの取得
			sws_ctx_ = sws_getContext(size_.x, size_.y, codec_ctx_->pix_fmt, size_.x, size_.y,
				PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);

			count_ = 0;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
			@return フレーム終端なら「true」
		*/
		//-----------------------------------------------------------------//
		bool update() {
			if(format_ctx_ == nullptr || codec_ctx_ == nullptr) return true;

			// フレーム読み込みループ
			AVPacket packet;
			while(av_read_frame(format_ctx_, &packet) >= 0) {

				// デコード
				int state;
				avcodec_decode_video2(codec_ctx_, frame_, &state, &packet);

				// 各フレーム、デコード完了
				if(state) {
					// フレームを切り出し
//					int bsize = w_ * h_ * 4;
					sws_scale(sws_ctx_, (const uint8_t **)frame_->data, frame_->linesize, 0,
						codec_ctx_->height, image_->data, image_->linesize);
					++count_;
					return false;
				}
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フレームサイズを取得
			@return フレームサイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_frame_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	現在のフレームを取得
			@return デコードされたフレームイメージ
		*/
		//-----------------------------------------------------------------//
		const unsigned char* get_frame() const {
			if(format_ctx_ == nullptr || codec_ctx_ == nullptr) return nullptr;
			return buffer_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	現在のフレーム数を取得
		*/
		//-----------------------------------------------------------------//
		uint32_t get_frame_no() const { return count_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クローズ
		*/
		//-----------------------------------------------------------------//
		void close() {
			sws_freeContext(sws_ctx_);
			sws_ctx_ = nullptr;

			av_free(buffer_);
			buffer_ = nullptr;

			av_free(image_);
			image_ = nullptr;

			av_free(frame_);
			frame_ = nullptr;

			avcodec_close(codec_ctx_);
			codec_ctx_ = nullptr;

			avformat_close_input(&format_ctx_);
			format_ctx_ = nullptr;
		}
	};
}