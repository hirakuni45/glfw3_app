#pragma once
//=====================================================================//
/*!	@file
	@brief	FFmpeg Library/decoder クラス
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
#include "snd_io/i_audio.hpp"
#include "snd_io/pcm.hpp"
#include "utils/fifo.hpp"

namespace av {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	decoder クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class decoder {

	public:
		struct fb_t {
			AVFrame* 	image_;
			uint8_t*	buff_;
			fb_t() : image_(nullptr), buff_(nullptr) { }
		};

	private:
		static const int fb_buff_size_ = 4;
		static const int au_buff_size_ = 32;

		std::string			path_;
		AVFormatContext*	format_ctx_;
		AVCodecContext*		video_ctx_;
		AVCodecContext*		audio_ctx_;
		int					video_idx_;
		int					audio_idx_;
		AVPacket			packet_;
		AVFrame* 			frame_;
		fb_t				fb_buff_[fb_buff_size_];
		uint32_t			fb_pos_;
		utils::fifo<fb_t, fb_buff_size_>		fb_;
		utils::fifo<al::audio, au_buff_size_>	au_;
		vtx::ipos			size_;
		SwsContext*			sws_ctx_;
		uint32_t			vcount_;
		uint32_t			acount_;
		double				fps_;
		double				video_sum_;
		double				audio_sum_;

		bool				init_;

		AVCodecContext* codec_context_(AVMediaType type, int& index) { 
			AVCodecContext* context = nullptr;
			index = av_find_best_stream(format_ctx_, type, -1, -1, NULL, 0);
			if(index >= 0) {
				context = format_ctx_->streams[index]->codec;
				if(context == nullptr) {
					std::cerr << "Context: nullptr" << std::endl << std::flush;
					return nullptr;
				}
				AVCodec* codec = avcodec_find_decoder(context->codec_id);
				if(avcodec_open2(context, codec, NULL) < 0) {
					std::cerr << "Error 'avcodec_open2()'" << std::endl;
					return nullptr;
				}
			} else {
				std::cerr << "Video codec can't find." << std::endl << std::flush;
				return nullptr;
			}
			return context;
		}


		al::audio create_audio_() {
			int ds = av_get_bytes_per_sample(audio_ctx_->sample_fmt);
//			int das = av_samples_get_buffer_size(nullptr, audio_ctx_->channels,
//						frame_->nb_samples, audio_ctx_->sample_fmt, 1);
//			std::cout << "Data size: " << das << std::endl << std::flush;

			al::audio aif = al::audio(new al::audio_sto16);
			if(aif) {
				aif->create(audio_ctx_->sample_rate, frame_->nb_samples);
				for(uint32_t i = 0; i < frame_->nb_samples; ++i) {
					if(audio_ctx_->channels == 1) {
						if(ds == 1) {
							const al::s8* m = (const al::s8*)frame_->extended_data[0];
							al::pcm8_m w(m[i]);
							aif->put(i, w);
						} else if(ds == 2) {
							const al::s16* m = (const al::s16*)frame_->extended_data[0];
							al::pcm16_m w(m[i]);
							aif->put(i, w);
						}
					} else if(audio_ctx_->channels == 2) {
						if(ds == 2) {
							const al::s8* s = (const al::s8*)frame_->extended_data[0];
							al::pcm8_s w(s[i * 2 + 0], s[i * 2 + 1]);
							aif->put(i, w);
						} else if(ds == 4) {
							const float* r = (const float*)frame_->extended_data[0];
							const float* l = (const float*)frame_->extended_data[1];
							al::pcm16_s w(static_cast<short>(l[i] * 32767.0f),
										  static_cast<short>(r[i] * 32767.0f));
							aif->put(i, w);
						}
					}
				}
			}
			return aif;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		decoder() : path_(), format_ctx_(nullptr),
					video_ctx_(nullptr), audio_ctx_(nullptr),
					video_idx_(-1), audio_idx_(-1),
					frame_(nullptr),
					fb_pos_(0),
					size_(0), sws_ctx_(nullptr),
					vcount_(0), acount_(0),
					fps_(0.0), video_sum_(0.0), audio_sum_(0.0),
					init_(false) { }


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
			if(init_) return;
			av_register_all();
			init_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム情報の表示
		*/
		//-----------------------------------------------------------------//
		void info() {
			av_dump_format(format_ctx_, 0, path_.c_str(), 1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@param[in]	path	ファイル名パス
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& path) {
			path_ = path;
			video_idx_ = -1;
			audio_idx_ = -1;
			video_ctx_ = nullptr;
			audio_ctx_ = nullptr;
			fps_ = 0.0;
			video_sum_ = 0.0;
			audio_sum_ = 0.0;

			if(path.empty()) return false;

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

			// Video コーデックの検索と取得
			video_ctx_ = codec_context_(AVMEDIA_TYPE_VIDEO, video_idx_);
			if(video_ctx_ == nullptr) {
				close();
				return false;
			}

			// Audio コーデックの検索と取得
			audio_ctx_ = codec_context_(AVMEDIA_TYPE_AUDIO, audio_idx_);
			if(audio_ctx_ == nullptr) {
				close();
				return false;
			}
//			std::cout << "Video idx: " << video_idx_ << std::endl << std::flush;
//			std::cout << "Audio idx: " << audio_idx_ << std::endl << std::flush;
			fps_ = av_q2d(format_ctx_->streams[video_idx_]->avg_frame_rate);
//			std::cout << "FPS: " << fps << std::endl;

			// ログ・レベルの設定
			av_log_set_level(1);

			// フレームの確保
			frame_ = avcodec_alloc_frame();

			if(video_ctx_ != nullptr) {
				size_.x = video_ctx_->width;
				size_.y = video_ctx_->height;

				// イメージ格納バッファの確保
				fb_.clear();
				for(int i = 0; i < fb_buff_size_; ++i) {
					fb_t& fb = fb_buff_[i];
					fb.image_ = avcodec_alloc_frame();
					// イメージ用バッファの確保
					fb.buff_ = (unsigned char *)av_malloc(avpicture_get_size(PIX_FMT_RGB24, size_.x, size_.y));
					// バッファとフレームを関連付ける
					avpicture_fill((AVPicture*)fb.image_, fb.buff_, PIX_FMT_RGB24, size_.x, size_.y);
				}

				// スケーリング用コンテキストの取得
				sws_ctx_ = sws_getContext(size_.x, size_.y, video_ctx_->pix_fmt, size_.x, size_.y,
					PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
			}

			if(audio_ctx_ != nullptr) {
				au_.clear();
//				std::cout << "Sample rate: " << (int)audio_ctx_->sample_rate << std::endl;
//				std::cout << "Audio chanel: " << (int)audio_ctx_->channels << std::endl;
//				std::cout << std::flush;
				// int ds = av_get_bytes_per_sample(audio_ctx_->sample_fmt);
				// audio_ctx_->sample_rate;
				// audio_ctx_->channels
//				std::cout << "Audio format: " << (int)audio_ctx_->sample_fmt << std::endl;
			}

			vcount_ = 0;
			acount_ = 0;

			av_init_packet(&packet_);
			packet_.data = NULL;
			packet_.size = 0;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
			@return フレーム終端なら「true」
		*/
		//-----------------------------------------------------------------//
		bool update() {
			if(format_ctx_ == nullptr || video_ctx_ == nullptr) return true;
 			if(video_idx_ < 0) return true;

			if(fb_.length() >= (fb_buff_size_ - 1)) return false;

			// フレーム読み込みループ
			while(av_read_frame(format_ctx_, &packet_) >= 0) {
				// デコード・ビデオ
				if(packet_.stream_index == video_idx_) {
					int vstate;
					int ret = avcodec_decode_video2(video_ctx_, frame_, &vstate, &packet_);
					if(ret >= 0 && vstate) {
						// フレームを切り出し
						fb_t& fb = fb_buff_[fb_pos_ % fb_buff_size_];
						++fb_pos_;
						sws_scale(sws_ctx_, (const uint8_t **)frame_->data, frame_->linesize, 0,
							video_ctx_->height, fb.image_->data, fb.image_->linesize);
						fb_.put(fb);
						++vcount_;
						video_sum_ += 1.0 / fps_;
					}
				}

				// デコード・オーディオ
				if(packet_.stream_index == audio_idx_) {
					int astate;
					int ret = avcodec_decode_audio4(audio_ctx_, frame_, &astate, &packet_);
					if(ret >= 0 && astate) {
						al::audio ai = create_audio_();
						au_.put(ai);
						++acount_;
						double at = static_cast<double>(frame_->nb_samples)
								  / static_cast<double>(audio_ctx_->sample_rate);
						audio_sum_ += at;
					}
				}

				if(fb_.length() >= (fb_.size() - 1)) {
//					std::cout << (int)au_.length() << std::endl << std::flush;
					return false;
				}
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フレームレートを取得
			@return フレームレート
		*/
		//-----------------------------------------------------------------//
		double get_frame_rate() const { return fps_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ビデオ時間を取得
			@return ビデオ時間
		*/
		//-----------------------------------------------------------------//
		double get_video_time() const { return video_sum_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ時間を取得
			@return オーディオ時間
		*/
		//-----------------------------------------------------------------//
		double get_audio_time() const { return audio_sum_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フレームサイズを取得
			@return フレームサイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_frame_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージ数を取得
			@return イメージ数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_image_num() const {
			return fb_.length();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを取得
			@return RGB24 イメージ
		*/
		//-----------------------------------------------------------------//
		const uint8_t* get_image() {
			if(format_ctx_ == nullptr || video_ctx_ == nullptr) return nullptr;
			if(fb_.length()) {
				const fb_t& fb = fb_.get();
				return fb.buff_;
			} else {
				return nullptr;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ数を取得
			@return オーディオ数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_audio_num() const {
			return au_.length();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオを取得
			@return オーディオ・インターフェース
		*/
		//-----------------------------------------------------------------//
		al::audio get_audio() {
			if(format_ctx_ == nullptr || audio_ctx_ == nullptr) return nullptr;
			if(au_.length()) {
				return au_.get();
			} else {
				return nullptr;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	現在のフレーム数を取得
		*/
		//-----------------------------------------------------------------//
		uint32_t get_frame_no() const { return vcount_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クローズ
		*/
		//-----------------------------------------------------------------//
		void close() {
//			std::cout << "Video: " << (int)vcount_ << std::endl;
//			std::cout << "Audio: " << (int)acount_ << std::endl;

			sws_freeContext(sws_ctx_);
			sws_ctx_ = nullptr;

			for(int i = 0; i < fb_buff_size_; ++i) {
				fb_t& fb = fb_buff_[i];
				av_free(fb.image_);
				fb.image_ = nullptr;
				av_free(fb.buff_);
				fb.buff_ = nullptr;
			}

			av_free(frame_);
			frame_ = nullptr;

			avcodec_close(audio_ctx_);
			audio_ctx_ = nullptr;

			avcodec_close(video_ctx_);
			video_ctx_ = nullptr;

			avformat_close_input(&format_ctx_);
			format_ctx_ = nullptr;
		}
	};
}
