#pragma once
//=====================================================================//
/*!	@file
	@brief	FFmpeg Library/decoder クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <deque>
extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavfilter/avfilter.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
};
#include "utils/vtx.hpp"
#include "snd_io/i_audio.hpp"
#include "snd_io/pcm.hpp"

namespace av {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	decoder クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class decoder {

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	オーディオ・フォーマット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class audio_format {
			none,		///< 無効
			u8,			///< unsigned 8 bits
			s16,		///< signed 16 bits
			f32,		///< float 32 bits
			invalid		///< 未知のフォーマット
		};

		typedef std::deque<al::audio>  audio_deque;

	private:
		struct fb_t {
			AVFrame* 	image_;
			uint8_t*	buff_;
			fb_t() : image_(nullptr), buff_(nullptr) { }
		};

		std::string			path_;
		AVFormatContext*	format_ctx_;
		AVCodecContext*		video_ctx_;
		AVCodecContext*		audio_ctx_;
		int					video_idx_;
		int					audio_idx_;
		AVFrame* 			frame_;
		fb_t				fb_[2];
		uint32_t			fb_put_;
		uint32_t			fb_get_;
		vtx::ipos			size_;
		SwsContext*			sws_ctx_;
		uint32_t			vcount_;
		uint32_t			acount_;
		double				fps_;
		double				video_sum_;
		double				audio_sum_;
		audio_deque			audio_deque_;

		bool				init_;

		AVCodecContext* codec_context_(AVMediaType type, int& index) { 
			AVCodecContext* context = nullptr;
			index = av_find_best_stream(format_ctx_, type, -1, -1, NULL, 0);
			if(index >= 0) {
				context = format_ctx_->streams[index]->codec;
				if(context == nullptr) {
//					std::cerr << "Context: nullptr" << std::endl << std::flush;
					return nullptr;
				}
				AVCodec* codec = avcodec_find_decoder(context->codec_id);
				if(avcodec_open2(context, codec, NULL) < 0) {
//					std::cerr << "Error 'avcodec_open2()'" << std::endl;
					return nullptr;
				}
			} else {
//				std::cerr << "Video codec can't find." << std::endl << std::flush;
				return nullptr;
			}
			return context;
		}


		al::audio create_audio_() {
			al::audio aif = al::audio(new al::audio_sto16);
			if(aif) {
				aif->create(audio_ctx_->sample_rate, frame_->nb_samples);
				typedef std::function<void (al::audio aif, int idx, const void* right, const void* left) > 
					fill_func;
				fill_func func;
				if(audio_ctx_->sample_fmt == AV_SAMPLE_FMT_FLTP
				|| audio_ctx_->sample_fmt == AV_SAMPLE_FMT_FLT) {
					func = [this] (al::audio aif, int idx, const void* right, const void* left) {
						const float* r = static_cast<const float*>(right);
						int ir = static_cast<int>(r[idx] * 32767.0f);
						ir = std::max(ir, -32768);
						ir = std::min(ir,  32767);
						int il = ir;
						if(left) {
							const float* l = static_cast<const float*>(left);
							il = static_cast<int>(l[idx] * 32767.0f);
							il = std::max(il, -32768);
							il = std::min(il,  32767);
						}
						al::pcm16_s w(il, ir);
						aif->put(idx, w);
					};
				} else if(audio_ctx_->sample_fmt == AV_SAMPLE_FMT_S16P
						|| audio_ctx_->sample_fmt == AV_SAMPLE_FMT_S16) {
					func = [this] (al::audio aif, int idx, const void* left, const void* right) {
						const int16_t* r = static_cast<const int16_t*>(right);
						int16_t ir = r[idx];
						int16_t il = ir;
						if(left) {
							const int16_t* l = static_cast<const int16_t*>(left);
							il = l[idx];
						}
						al::pcm16_s w(il, ir);
						aif->put(idx, w);
					};
				} else if(audio_ctx_->sample_fmt == AV_SAMPLE_FMT_U8P
						|| audio_ctx_->sample_fmt == AV_SAMPLE_FMT_U8) {
					func = [this] (al::audio aif, int idx, const void* left, const void* right) {
						const uint8_t* r = static_cast<const uint8_t*>(right);
						int16_t ir = static_cast<int16_t>(r[idx]);
						ir |= ir << 8;
						ir -= 32768;
						int16_t il = ir;
						if(left) {
							const uint8_t* l = static_cast<const uint8_t*>(left);
							il = static_cast<int16_t>(l[idx]);
							il |= il << 8;
							il -= 32768;
						}
						al::pcm16_s w(il, ir);
						aif->put(idx, w);
					};
				}

				if(func) {
					if(audio_ctx_->channels == 1) {
						for(uint32_t i = 0; i < frame_->nb_samples; ++i) {
							func(aif, i, frame_->extended_data[0], nullptr);
						}
					} else {
						for(uint32_t i = 0; i < frame_->nb_samples; ++i) {
							func(aif, i, frame_->extended_data[0], frame_->extended_data[1]);
						}
					}
				}
			}
			return aif;
		}

#if 0
			const al::s8* s = (const al::s8*)frame_->extended_data[0];
			al::pcm8_s w(s[i * 2 + 0], s[i * 2 + 1]);
			aif->put(i, w);
#endif

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
					fb_(), fb_put_(0), fb_get_(0),
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

			// Audio コーデックの検索と取得
			audio_ctx_ = codec_context_(AVMEDIA_TYPE_AUDIO, audio_idx_);

			// ビデオも、オーディオ無かったらクローズする。
			if(video_ctx_ == nullptr && audio_ctx_ == nullptr) {
				close();
				return false;
			}

//			std::cout << "Video idx: " << video_idx_ << std::endl << std::flush;
//			std::cout << "Audio idx: " << audio_idx_ << std::endl << std::flush;
			fps_ = av_q2d(format_ctx_->streams[video_idx_]->avg_frame_rate);
//			std::cout << "FPS: " << fps << std::endl;

			if(0) {
				AVStream* avs = format_ctx_->streams[video_idx_];
				double d = avs->start_time * av_q2d(avs->time_base);
				std::cout << "Video st: " << d << std::endl;
			}
			if(0) {
				AVStream* avs = format_ctx_->streams[audio_idx_];
				double d = avs->start_time * av_q2d(avs->time_base);
				std::cout << "Audio st: " << d << std::endl;
			}

			// ログ・レベルの設定
			av_log_set_level(1);

			// フレームの確保
			frame_ = avcodec_alloc_frame();

			if(video_ctx_ != nullptr) {
				size_.x = video_ctx_->width;
				size_.y = video_ctx_->height;

				for(int i = 0; i < 2; ++i) {
					// イメージ格納バッファの確保
					fb_[i].image_ = avcodec_alloc_frame();
					// イメージ用バッファの確保
					fb_[i].buff_ = (unsigned char *)av_malloc(avpicture_get_size(PIX_FMT_RGB24, size_.x, size_.y));
					// バッファとフレームを関連付ける
					avpicture_fill((AVPicture*)fb_[i].image_, fb_[i].buff_, PIX_FMT_RGB24, size_.x, size_.y);
				}
				// スケーリング用コンテキストの取得
				sws_ctx_ = sws_getContext(size_.x, size_.y, video_ctx_->pix_fmt, size_.x, size_.y,
					PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
			}

			// double seconds= (dts - pStream->start_time) * av_q2d(pStream->time_base);

			if(audio_ctx_ != nullptr) {
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
			fb_put_ = 0;
			fb_get_ = 0;
			audio_deque_.clear();

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

			// フレーム読み込みループ
			AVPacket packet;
			av_init_packet(&packet);
//static int aa = 0;
//static bool start = false;
			while(av_read_frame(format_ctx_, &packet) >= 0) {

//				if(aa > packet.pts) {
//					start = true;
//				}
//				aa = packet.pts;
//				std::cout << (int)packet.pts << std::endl << std::flush;

				// デコード・ビデオ
				if(packet.stream_index == video_idx_) {
					int vstate;
					int ret = avcodec_decode_video2(video_ctx_, frame_, &vstate, &packet);
					if(ret >= 0 && vstate) {
						// フレームを切り出し
						fb_t& fb = fb_[fb_put_ & 1];
						++fb_put_;
						sws_scale(sws_ctx_, (const uint8_t **)frame_->data, frame_->linesize, 0,
							video_ctx_->height, fb.image_->data, fb.image_->linesize);
						++vcount_;
						video_sum_ += 1.0 / fps_;
						return false;
//						std::cout << 'V';
					}
				}

				// デコード・オーディオ
				if(packet.stream_index == audio_idx_) {
					int astate;
					int ret = avcodec_decode_audio4(audio_ctx_, frame_, &astate, &packet);
					if(ret >= 0 && astate) {
						if(audio_deque_.size() < 128) { 
							al::audio ai = create_audio_();
							audio_deque_.push_back(ai);
							++acount_;
							double at = static_cast<double>(frame_->nb_samples)
									  / static_cast<double>(audio_ctx_->sample_rate);
							audio_sum_ += at;
//							std::cout << 'A';
						}
					}
				}

				if((fb_put_ - fb_get_) == 2) {
///					std::cout << std::endl << std::flush;
//					return false;
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
			@brief	イメージを取得
			@return RGB24 イメージ
		*/
		//-----------------------------------------------------------------//
		const uint8_t* get_image() {
			if(format_ctx_ == nullptr || video_ctx_ == nullptr || video_idx_ < 0) return nullptr;
			const uint8_t* p = fb_[fb_get_ & 1].buff_;
			++fb_get_;
			return p; 
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・フォーマットを取得
			@return オーディオ・フォーマット
		*/
		//-----------------------------------------------------------------//
		audio_format get_audio_format() const {
			if(audio_ctx_ == nullptr || audio_idx_ < 0) return audio_format::none;
			if(audio_ctx_->sample_fmt == AV_SAMPLE_FMT_FLTP
			|| audio_ctx_->sample_fmt == AV_SAMPLE_FMT_FLT) {
				return audio_format::f32;
			} else if(audio_ctx_->sample_fmt == AV_SAMPLE_FMT_S16P
					|| audio_ctx_->sample_fmt == AV_SAMPLE_FMT_S16) {
				return audio_format::s16;
			} else if(audio_ctx_->sample_fmt == AV_SAMPLE_FMT_U8P
					|| audio_ctx_->sample_fmt == AV_SAMPLE_FMT_U8) {
				return audio_format::u8;
			} else {
				return audio_format::invalid;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・サンプル・レートを取得
			@return オーディオ・サンプル・レート
		*/
		//-----------------------------------------------------------------//
		uint32_t get_audio_rate() const {
			if(audio_ctx_ == nullptr || audio_idx_ < 0) return 0;
			return audio_ctx_->sample_rate;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・チャネルを取得
			@return オーディオ・チャネル
		*/
		//-----------------------------------------------------------------//
		uint32_t get_audio_chanel() const {
			if(audio_ctx_ == nullptr || audio_idx_ < 0) return 0;
			return audio_ctx_->channels;			
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・バッファを参照
			@return オーディオ・バッファ
		*/
		//-----------------------------------------------------------------//
		audio_deque& at_audio() {
			return audio_deque_;
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

			for(int i = 0; i < 2; ++i) {
				av_free(fb_[i].image_);
				fb_[i].image_ = nullptr;
				av_free(fb_[i].buff_);
				fb_[i].buff_ = nullptr;
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
