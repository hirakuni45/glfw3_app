#pragma once
//=====================================================================//
/*!	@file
	@brief	MP3 ファイルを扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <cmath>
#include <mad.h>
#include "i_snd_io.hpp"
#include "snd_io/pcm.hpp"
#include "snd_io/id3_mgr.hpp"
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

		static constexpr int INPUT_BUFFER_SIZE = (5 * 8192);
		static constexpr int STREAM_NUM = 8;

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

		sound::tag_t	tag_;

		std::vector<uint32_t>	seek_points_;

		// デコード済みテンポラリー、５０フレーム分確保（44.1KHz で１．３秒）
		static const int output_buffer_size_ = 50;

		static const short SHRT_MAX_ = 32767;

		/****************************************************************************
		 * Converts a sample from mad's fixed point number format to a signed		*
		 * short (16 bits).															*
		 ****************************************************************************/
		static short MadFixedToSshort(mad_fixed_t v)
		{
			/* A fixed point number is formed of the following bit pattern:
			 *
			 * SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
			 * MSB                          LSB
			 * S ==> Sign (0 is positive, 1 is negative)
			 * W ==> Whole part bits
			 * F ==> Fractional part bits
			 *
			 * This pattern contains MAD_F_FRACBITS fractional bits, one
			 * should alway use this macro when working on the bits of a fixed
			 * point number. It is not guaranteed to be constant over the
			 * different platforms supported by libmad.
			 *
			 * The signed short value is formed, after clipping, by the least
			 * significant whole part bit, followed by the 15 most significant
			 * fractional part bits. Warning: this is a quick and dirty way to
			 * compute the 16-bit number, madplay includes much better
			 * algorithms.
			 */

			// Clipping..
			if(v >= MAD_F_ONE) {
				return SHRT_MAX_;
			}
			if(v <= -MAD_F_ONE) {
				return -SHRT_MAX_;
			}

			return (signed short)(v >> (MAD_F_FRACBITS - 15));
		}

		void apply_filter_(mad_frame& frame)
		{
			/* There is two application loops, each optimized for the number
			 * of audio channels to process. The first alternative is for
			 * two-channel frames, the second is for mono-audio.
			 */
			int num = MAD_NSBSAMPLES(&frame.header);
			if(frame.header.mode != MAD_MODE_SINGLE_CHANNEL) {
				for(int ch = 0; ch < 2; ++ch) {
					for(int s = 0; s < num; ++s) {
						for(int sb = 0; sb < 32; ++sb) {
							frame.sbsample[ch][s][sb] =
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
								mad_f_mul(frame.sbsample[ch][s][sb], subband_filter_[sb]);
#pragma clang diagnostic pop
						}
					}
				}
			} else {
				for(int s = 0; s < num; ++s) {
					for(int sb = 0; sb < 32; ++sb) {
						frame.sbsample[0][s][sb] =
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
							mad_f_mul(frame.sbsample[0][s][sb], subband_filter_[sb]);
#pragma clang diagnostic pop
					}
				}
			}
		}


		int fill_read_buffer_(utils::file_io& fin, mad_stream& strm)
		{
			/* The input bucket must be filled if it becomes empty or if
			 * it's the first execution of the loop.
			 */
			if(strm.buffer == NULL || strm.error == MAD_ERROR_BUFLEN) {
				size_t size;
				size_t remaining;
				unsigned char* ptr;

				/* {2} libmad may not consume all bytes of the input
				 * buffer. If the last frame in the buffer is not wholly
				 * contained by it, then that frame's start is pointed by
				 * the next_frame member of the Stream structure. This
				 * common situation occurs when mad_frame_decode() fails,
				 * sets the stream error code to MAD_ERROR_BUFLEN, and
				 * sets the next_frame pointer to a non NULL value. (See
				 * also the comment marked {4} bellow.)
				 *
				 * When this occurs, the remaining unused bytes must be
				 * put back at the beginning of the buffer and taken in
				 * account before refilling the buffer. This means that
				 * the input buffer must be large enough to hold a whole
				 * frame at the highest observable bit-rate (currently 448
				 * kb/s). XXX=XXX Is 2016 bytes the size of the largest
				 * frame? (448000*(1152/32000))/8
				 */
				if(strm.next_frame != NULL) {
					remaining = strm.bufend - strm.next_frame;
					memmove(&input_buffer_[0], strm.next_frame, remaining);
					ptr  = &input_buffer_[remaining];
					size = INPUT_BUFFER_SIZE - remaining;
				} else {
					size = INPUT_BUFFER_SIZE;
					ptr  = &input_buffer_[0];
					remaining = 0;
				}

				/* Fill-in the buffer. If an error occurs print a message
				 * and leave the decoding loop. If the end of stream is
				 * reached we also leave the loop but the return status is
				 * left untouched.
				 */
				// ReadSize = BstdRead(ReadStart, 1, ReadSize, BstdFile);
				size_t req = size;
				size_t rs = fin.read(ptr, 1, req);
				if(id3v1_) {
					if(fin.tell() >= (fin.get_file_size() - 128)) return -1;
				} else {
					if(fin.eof()) return -1;
				}
				size = rs;
				if(rs < req) {
					memset(&ptr[rs], 0, MAD_BUFFER_GUARD);
					size += MAD_BUFFER_GUARD;
				}

				/* Pipe the new buffer content to libmad's stream decoder
				 * facility.
				 */
				mad_stream_buffer(&strm, &input_buffer_[0], size + remaining);
				strm.error = MAD_ERROR_NONE;
				return 0;
			} else {
				return 1;
			}
		}

		bool decode_(utils::file_io& fin, audio out)
		{
			mad_stream_init(&mad_stream_);
			mad_frame_init(&mad_frame_);
			mad_synth_init(&mad_synth_);
			mad_timer_reset(&mad_timer_);

			int pos = 0;
			int frame_count = 0;
			bool status = true;
			while(fill_read_buffer_(fin, mad_stream_) >= 0) {

				if(mad_frame_decode(&mad_frame_, &mad_stream_)) {
					if(MAD_RECOVERABLE(mad_stream_.error)) {
						continue;
					} else {
						if(mad_stream_.error == MAD_ERROR_BUFLEN) {
							continue;
						} else {
							status = false;
							break;
						}
					}
				}

				frame_count++;
				mad_timer_add(&mad_timer_, mad_frame_.header.duration);

				if(subband_filter_enable_) {
					apply_filter_(mad_frame_);
				}

				mad_synth_frame(&mad_synth_, &mad_frame_);

				for(int i = 0; i < mad_synth_.pcm.length; ++i) {
					if(MAD_NCHANNELS(&mad_frame_.header) == 1) {
						pcm16_m pcm;
						pcm.w = MadFixedToSshort(mad_synth_.pcm.samples[0][i]);
						out->put(pos, pcm);
					} else {
						pcm16_s pcm;
						pcm.l = MadFixedToSshort(mad_synth_.pcm.samples[0][i]);
						pcm.r = MadFixedToSshort(mad_synth_.pcm.samples[1][i]);
						out->put(pos, pcm);
					}
					++pos;
				}
			}

			mad_synth_finish(&mad_synth_);
			mad_frame_finish(&mad_frame_);
			mad_stream_finish(&mad_stream_);

			return status;
		}

		bool analize_frame_(utils::file_io& fin, audio_info& info, mp3_info& mp3info, info_state st)
		{
			if(!fin.is_open()) {
				return false;
			}

			// 現在のファイル位置を覚えておく
			uint32_t pos = fin.tell();
			tag_.clear();

			sound::id3_mgr id3;
			if(!id3.parse(fin)) {
				fin.seek(pos, utils::file_io::SEEK::SET);
				return false;
			}
			auto ser = tag_.serial_;
			tag_ = id3.get_tag();
			tag_.serial_ = ser + 1;

			if(tag_.get_title().empty()) {
				tag_.at_title() = utils::get_file_base(utils::get_file_name(fin.get_path()));
			}

			if((static_cast<uint8_t>(st) & static_cast<uint8_t>(info_state::time)) == 0) {
				return true;
			}

			// ３フレーム以下はエラーとする・・
			uint32_t limit_frame = 3;
			mp3info.reset();
			info.header_size = mp3info.skip_head = fin.tell();
			mad_stream stream;
			mad_stream_init(&stream);
			mad_frame frame;
			mad_frame_init(&frame);

			int frame_cnt = 0;
//			int samples = 0;
			int ch = 0;
			seek_points_.clear();
			uint32_t error = 0;
			while(error == 0) {
				seek_points_.push_back(fin.tell());
				int ret = fill_read_buffer_(fin, stream);
				if(ret < 0) break;

				if(mad_frame_decode(&frame, &stream)) {
					if(MAD_RECOVERABLE(stream.error)) {
						++mp3info.recover_frame_error;
						continue;
					} else {
						if(stream.error == MAD_ERROR_BUFLEN) {
							continue;
						} else {
							++mp3info.unrecover_frame_error;
							break;
						}
					}
				}

				// 異なるチャネルがある場合エラー
				if(MAD_NCHANNELS(&frame.header) == 1) {
					if(ch == 2) {
						++error;
						break;
					}
					ch = 1;
				} else if(MAD_NCHANNELS(&frame.header) == 2) {
					if(ch == 1) {
						++error;
						break;
					}
					ch = 2;
				} else { ++error; break; }

				// 異なるレイヤーがある場合エラー
				switch(frame.header.layer) {
				case MAD_LAYER_I:
					if(mp3info.layer_2 > 0 || mp3info.layer_3 > 0) {
						++error;
					}
					++mp3info.layer_1;
					break;
				case MAD_LAYER_II:
					if(mp3info.layer_1 > 0 || mp3info.layer_3 > 0) {
						++error;
					}
					++mp3info.layer_2;
					break;
				case MAD_LAYER_III:
					if(mp3info.layer_1 > 0 || mp3info.layer_2 > 0) {
						++error;
					}
					++mp3info.layer_3;
					break;
				default:
					++error;
					break;
				}
				if(error) break;

				switch(frame.header.mode) {
				case MAD_MODE_SINGLE_CHANNEL:
					++mp3info.single_chanel;
					break;
				case MAD_MODE_DUAL_CHANNEL:
					++mp3info.dual_chanel;
					break;
				case MAD_MODE_JOINT_STEREO:
					++mp3info.dual_chanel;
					break;
				case MAD_MODE_STEREO:
					++mp3info.dual_chanel;
					break;
				default:
					++error;
					break;
				}

				switch(frame.header.emphasis) {
				case MAD_EMPHASIS_NONE:
					break;
				case MAD_EMPHASIS_50_15_US:
					break;
				case MAD_EMPHASIS_CCITT_J_17:
					break;
#if (MAD_VERSION_MAJOR>=1) || \
	((MAD_VERSION_MAJOR==0) && (MAD_VERSION_MINOR>=15))
				case MAD_EMPHASIS_RESERVED:
					break;
#endif
				default:
					++error;
					break;
				}

				++frame_cnt;
			}

// std::cout << boost::format("Frame: %d\n") % count;
// std::cout << boost::format("Recoverable frame level error: %d\n") % mp3info.recover_frame_error;
// std::cout << boost::format("Unrecoverable frame level error: %d\n") % mp3info.unrecover_frame_error;

			if(error == 0 && frame_cnt > limit_frame && mp3info.unrecover_frame_error == 0) {
				if(ch == 1) {
					info.type = audio_format::PCM16_MONO;
				} else {
					info.type = audio_format::PCM16_STEREO;
				}
				info.samples = frame_cnt * 1152;
				info.chanels = ch;
				info.bits = 16;
				info.frequency = frame.header.samplerate;
				info.block_align = ch * 2;
			}

			mad_frame_finish(&frame);
			mad_stream_finish(&stream);

			fin.seek(pos, utils::file_io::SEEK::SET);

// std::cout << boost::format("error: %d\n") % error;
// std::cout << boost::format("frame: %d\n") % frame_cnt;

			if(error == 0 && frame_cnt > limit_frame && mp3info.unrecover_frame_error == 0) {
				mp3info.frame_count = frame_cnt;
				return true;
			} else {
				return false;
			}
		}

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
		void initialize() override { }


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
		const sound::tag_t& get_tag() const override { return tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ロードする
			@param[in]	fin	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override
		{
			audio_info info;
			mp3_info_.reset();
			bool f = analize_frame_(fin, info, mp3_info_, info_state::all);
			if(f) {
				fin.seek(info.header_size, utils::file_io::SEEK::SET);

				audio aif;
				if(info.chanels == 1) {
					aif = audio(new audio_mno16);
				} else if(info.chanels == 2) {
					aif = audio(new audio_sto16);
				}
				if(aif) {
//					std::cout << boost::format("Sampling: %d [Hz]\n") % info.frequency;
//					std::cout << boost::format("Samples:  %d\n") % info.samples;
					aif->create(info.frequency, info.samples);

					if(decode_(fin, aif)) {
						audio_ = aif;
					}
				} else {
					f = false;
				}
			}
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override
		{
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・オープンする
			@param[in]	fi		file_io クラス
			@param[in]	size	バッファサイズ
			@param[in]	inf		オーディオ情報を受け取る
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open_stream(utils::file_io& fi, int size, audio_info& inf) override
		{
			close_stream();

			output_max_ = 1152 * output_buffer_size_;

			offset_ = 0;

			if(info(fi, inf)) {
				fi.seek(inf.header_size, utils::file_io::SEEK::SET);
				start_pos_ = fi.tell();

				mad_stream_init(&mad_stream_);
				mad_frame_init(&mad_frame_);
				mad_synth_init(&mad_synth_);
				mad_timer_reset(&mad_timer_);

				output_pos_ = 0;

				if(inf.chanels == 1) {
					if(inf.bits == 8) {
						stream_ = audio(new audio_mno8);
						output_buffer_ = audio(new audio_mno8);
					} else if(inf.bits == 16) {
						stream_ = audio(new audio_mno16);
						output_buffer_ = audio(new audio_mno16);
					}
				} else if(inf.chanels == 2) {
					if(inf.bits == 8) {
						stream_ = audio(new audio_sto8);
						output_buffer_ = audio(new audio_sto8);
					} else if(inf.bits == 16) {
						stream_ = audio(new audio_sto16);
						output_buffer_ = audio(new audio_sto16);
					}
				}

				if(stream_) {
					stream_->create(inf.frequency, size);
					stream_->zero();
					output_all_ = static_cast<int>(inf.samples);
					output_buffer_->create(inf.frequency, output_max_);
					output_buffer_->zero();
//					std::cout << boost::format("Stream Sampling: %d [Hz]\n") % inf.frequency;
//					std::cout << boost::format("Stream Samples:  %d\n") % inf.samples;
					return true;
				}
			}
			return false;
		}


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
		size_t read_stream(utils::file_io& fin, size_t offset, size_t samples) override
		{
			if(stream_ == 0) return 0;

			int f = 0;
			bool status = true;
			size_t end_pos = offset + samples;

			if(offset != 0 && (offset_ + samples) != offset) {	// seek を検出
				uint32_t n = offset / 1152;
				if(n < seek_points_.size()) {
					fin.seek(seek_points_[n], utils::file_io::SEEK::SET);
					output_pos_ = 1152 * n;
				} else {
					// seek error...
					return 0;
				}
			}
			offset_ = offset;
			while(output_pos_ < end_pos) {
				f = fill_read_buffer_(fin, mad_stream_);
				if(f < 0) {
					status = false;
					break;
				}

				if(mad_frame_decode(&mad_frame_, &mad_stream_)) {
					if(MAD_RECOVERABLE(mad_stream_.error)) {
						continue;
					} else {
						if(mad_stream_.error == MAD_ERROR_BUFLEN) {
							continue;
						} else {
							status = false;
							break;
						}
					}
				}

				mad_timer_add(&mad_timer_, mad_frame_.header.duration);

				if(subband_filter_enable_) {
					apply_filter_(mad_frame_);
				}

				mad_synth_frame(&mad_synth_, &mad_frame_);

				for(int i = 0; i < mad_synth_.pcm.length; ++i) {
					if(MAD_NCHANNELS(&mad_frame_.header) == 2) {
						pcm16_s pcm;
						pcm.l = MadFixedToSshort(mad_synth_.pcm.samples[0][i]);
						pcm.r = MadFixedToSshort(mad_synth_.pcm.samples[1][i]);
						output_buffer_->put(output_pos_ % output_max_, pcm);
					} else {
						pcm16_m pcm;
						pcm.w = MadFixedToSshort(mad_synth_.pcm.samples[0][i]);
						output_buffer_->put(output_pos_ % output_max_, pcm);
					}
					++output_pos_;
				}
				if(f < 0) break;	// to be EOF
			}

			if(output_buffer_->get_chanel() == 2) {
				for(size_t i = 0; i < stream_->get_samples(); ++i) {
					pcm16_s pcm;
					if((offset + i) >= output_all_) {
						pcm.l = pcm.r = 0;
					} else {
						output_buffer_->get((offset + i) % output_max_, pcm);
					}
					stream_->put(i, pcm);
				}
			} else {
				for(size_t i = 0; i < stream_->get_samples(); ++i) {
					pcm16_m pcm;
					if((offset + i) >= output_all_) {
						pcm.w = 0;
					} else {
						output_buffer_->get((offset + i) % output_max_, pcm);
					}
					stream_->put(i, pcm);
				}
			}

			if(status) return samples;
			else return 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをクローズ
		*/
		//-----------------------------------------------------------------//
		void close_stream() override
		{
			if(stream_) {
				stream_ = 0;
				output_buffer_ = 0;
				mad_synth_finish(&mad_synth_);
				mad_frame_finish(&mad_frame_);
				mad_stream_finish(&mad_stream_);
			}
		}


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
		void destroy() override
		{
			close_stream();
		}
	};
}
