//=====================================================================//
/*!	@file
	@brief	MP3 ファイルを扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "mp3_io.hpp"
#include "pcm.hpp"
#include "img_io/img_utils.hpp"
#include <boost/format.hpp>

#include <iostream>

#include <mpegfile.h>
#include <id3v2tag.h>
#include <id3v2header.h>
#include <id3v1tag.h>
#include <apetag.h>

namespace al {

	using namespace utils;

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


	/****************************************************************************
	 * Applies a frequency-domain filter to audio data in the subband-domain.	*
	 ****************************************************************************/
	void mp3_io::apply_filter_(mad_frame& frame)
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


	int mp3_io::fill_read_buffer_(file_io& fin, mad_stream& strm)
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


	bool mp3_io::decode_(file_io& fin, audio out)
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

#if 0
	static int mpeg_audio_decoder(file_io& fin, FILE *OutputFp)
	{
		unsigned char InputBuffer[INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
		unsigned char OutputBuffer[OUTPUT_BUFFER_SIZE];
		unsigned char* OutputPtr = OutputBuffer;
		const unsigned char* OutputBufferEnd = OutputBuffer + OUTPUT_BUFFER_SIZE;

		int status = 0;
		unsigned int frame_count = 0;
		unsigned int output_count = 0;

		// First the structures used by libmad must be initialized.
		struct mad_stream Stream;
		mad_stream_init(&Stream);
		struct mad_frame Frame;
		mad_frame_init(&Frame);
		struct mad_synth Synth;
		mad_synth_init(&Synth);
		mad_timer_t Timer;
		mad_timer_reset(&Timer);

		/* This is the decoding loop. */
		while(fill_read_buffer(fin, Stream, InputBuffer) >= 0) {

			/* Decode the next mpeg frame. The streams is read from the
			 * buffer, its constituents are break down and stored the the
			 * Frame structure, ready for examination/alteration or PCM
			 * synthesis. Decoding options are carried in the Frame
			 * structure from the Stream structure.
			 *
			 * Error handling: mad_frame_decode() returns a non zero value
			 * when an error occurs. The error condition can be checked in
			 * the error member of the Stream structure. A mad error is
			 * recoverable or fatal, the error status is checked with the
			 * MAD_RECOVERABLE macro.
			 *
			 * {4} When a fatal error is encountered all decoding
			 * activities shall be stopped, except when a MAD_ERROR_BUFLEN
			 * is signaled. This condition means that the
			 * mad_frame_decode() function needs more input to achieve
			 * it's work. One shall refill the buffer and repeat the
			 * mad_frame_decode() call. Some bytes may be left unused at
			 * the end of the buffer if those bytes forms an incomplete
			 * frame. Before refilling, the remainign bytes must be moved
			 * to the begining of the buffer and used for input for the
			 * next mad_frame_decode() invocation. (See the comments marked
			 * {2} earlier for more details.)
			 *
			 * Recoverable errors are caused by malformed bit-streams, in
			 * this case one can call again mad_frame_decode() in order to
			 * skip the faulty part and re-sync to the next frame.
			 */
			if(mad_frame_decode(&Frame, &Stream)) {
				if(MAD_RECOVERABLE(Stream.error)) {
//					std::cout << boost::format("Recoverable frame level error (%s)\n")
//						% MadErrorString(&Stream);
					continue;
				} else {
					if(Stream.error == MAD_ERROR_BUFLEN) {
						continue;
					} else {
//						std::cout << boost::format("Unrecoverable frame level error (%s).\n")
//							% MadErrorString(&Stream);
						status = 1;
						break;
					}
				}
			}

			/* The characteristics of the stream's first frame is printed
			 * on stderr. The first frame is representative of the entire
			 * stream.
			 */
			if(frame_count == 0) {
				if(PrintFrameInfo(stderr, &Frame.header)) {
					status = 1;
					break;
				}
			}

			/* Accounting. The computed frame duration is in the frame
			 * header structure. It is expressed as a fixed point number
			 * whole data type is mad_timer_t. It is different from the
			 * samples fixed point format and unlike it, it can't directly
			 * be added or substracted. The timer module provides several
			 * functions to operate on such numbers. Be careful there, as
			 * some functions of mad's timer module receive some of their
			 * mad_timer_t arguments by value!
			 */
			frame_count++;
			mad_timer_add(&Timer, Frame.header.duration);

			/* Between the frame decoding and samples synthesis we can
			 * perform some operations on the audio data. We do this only
			 * if some processing was required. Detailed explanations are
			 * given in the ApplyFilter() function.
			 */
			if(g_subband_filter_enable) {
				ApplyFilter(Frame);
			}

			/* Once decoded the frame is synthesized to PCM samples. No errors
			 * are reported by mad_synth_frame();
			 */
			mad_synth_frame(&Synth, &Frame);

			/* Synthesized samples must be converted from mad's fixed
			 * point number to the consumer format. Here we use unsigned
			 * 16 bit big endian integers on two channels. Integer samples
			 * are temporarily stored in a buffer that is flushed when
			 * full.
			 */
			for(int i = 0; i < Synth.pcm.length; ++i) {
				signed short	Sample;

				// Left channel
				Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
				*(OutputPtr++) = Sample >> 8;
				*(OutputPtr++) = Sample & 0xff;
				++output_count;

				// Right channel. If the decoded stream is monophonic then
				// the right output channel is the same as the left one.
				if(MAD_NCHANNELS(&Frame.header) == 2) {
					Sample = MadFixedToSshort(Synth.pcm.samples[1][i]);
					*(OutputPtr++) = Sample >> 8;
					*(OutputPtr++) = Sample & 0xff;
					++output_count;
				}

				// Flush the output buffer if it is full.
				if(OutputPtr == OutputBufferEnd) {
					if(fwrite(OutputBuffer, 1, OUTPUT_BUFFER_SIZE, OutputFp) != OUTPUT_BUFFER_SIZE) {
						std::cout << boost::format("PCM write error (%s).\n")
							% strerror(errno);
						status = 2;
						break;
					}
					OutputPtr = OutputBuffer;
				}
			}
		}

		// If the output buffer is not empty and no error occured during
		// the last write, then flush it.
		if(OutputPtr != OutputBuffer && status != 2) {
			size_t BufferSize = OutputPtr - OutputBuffer;

			if(fwrite(OutputBuffer, 1, BufferSize, OutputFp) != BufferSize) {
				std::cout << boost::format("PCM write error (%s).\n")
					% strerror(errno);
				status = 2;
			}
		}

		// Mad is no longer used, the structures that were initialized must
		// now be cleared.
		mad_synth_finish(&Synth);
		mad_frame_finish(&Frame);
		mad_stream_finish(&Stream);

		/* Accounting report if no error occured. */
		if(!status) {
			char Buffer[80];

		/* The duration timer is converted to a human readable string
		 * with the versatile, but still constrained mad_timer_string()
		 * function, in a fashion not unlike strftime(). The main
		 * difference is that the timer is broken into several
		 * values according some of it's arguments. The units and
		 * fracunits arguments specify the intended conversion to be
		 * executed.
		 *
		 * The conversion unit (MAD_UNIT_MINUTES in our example) also
		 * specify the order and kind of conversion specifications
		 * that can be used in the format string.
		 *
		 * It is best to examine mad's timer.c source-code for details
		 * of the available units, fraction of units, their meanings,
		 * the format arguments, etc.
		 */
			mad_timer_string(Timer,Buffer,"%lu:%02lu.%03u",
						 MAD_UNITS_MINUTES,MAD_UNITS_MILLISECONDS,0);
			std::cout << boost::format("%lu frames decoded (%s).\n")
				% frame_count % Buffer;
		}

		/* That's the end of the world (in the H. G. Wells way). */
		return status;
	}


	//=================================================================//
	/*!
		@brief	テスト的デコード（実験コード）
	*/
	//=================================================================//
	void mp3_io::test_decode(utils::file_io& fin)
	{
		FILE* ofp;
		ofp = fopen("aaa.snd", "wb");
		mpeg_audio_decoder(fin, ofp);
		fclose(ofp);
	}
#endif


	static std::string convert_string_(const TagLib::String& s)
	{
		std::string tmp;
	   	if(s.isLatin1()) {
			utils::sjis_to_utf8(s.toCString(), tmp);
		} else {
			tmp = s.toCString(true);
		}
		std::string out;
		for(char ch : tmp) {
			if(ch < 0x20 && ch >= 0) ;
			else out += ch; 
		}
		return out;
	}


	static std::string convert_string_(const TagLib::uint n)
	{
		return (boost::format("%d") % static_cast<int>(n)).str();
	}

#if 0
	static std::string convert_string_(const TagLib::StringList& list)
	{
		std::string out;
		for(TagLib::StringList::ConstIterator i = list.begin(); i != list.end(); ++i) {
			out += convert_string_( *i );
		}
		return out;
	}
#endif

	//=================================================================//
	/*!
		@brief	フレームの解析
		@param[in]	fin	ファイル入力コンテキスト
		@param[in]	info	オーデイオ情報
		@param[in]	mp3info	MP3 情報
		@param[in]	apic	画像情報取得なら［true」
		@return エラーなら「false」
	*/
	//=================================================================//
	bool mp3_io::analize_frame_(utils::file_io& fin, audio_info& info, mp3_info& mp3info, info_state st)
	{
		if(!fin.is_open()) {
			return false;
		}

		// 現在のファイル位置を覚えておく
		uint32_t pos = fin.tell();
		uint32_t ofs = 0;
		tag_.clear();
		// 一旦クローズ
		fin.close();

		using namespace TagLib;
#ifdef WIN32
		utils::wstring ws;
		utils::utf8_to_utf16(fin.get_path(), ws);
		MPEG::File f(reinterpret_cast<const wchar_t*>(ws.c_str()));
#else
		MPEG::File f(fin.get_path().c_str());
#endif
		ID3v1::Tag* v1 = f.ID3v1Tag();
	   	if(v1) {
   			tag_.title_  = convert_string_(v1->title()); 
   			tag_.artist_ = convert_string_(v1->artist());
	   		tag_.album_  = convert_string_(v1->album());
			tag_.track_ = convert_string_(v1->track());
			tag_.date_ = convert_string_(v1->year());
			id3v1_ = true;
		}
		ID3v2::Tag* v2 = f.ID3v2Tag();
		if(v2) {
			ofs = v2->header()->tagSize();
			{
				std::string s = convert_string_(v2->title());
				if(!s.empty()) tag_.title_ = s;
			} 

			{
				std::string s = convert_string_(v2->artist());
				if(!s.empty()) tag_.artist_ = s; 
			}

			{
				std::string s = convert_string_(v2->album());
				if(!s.empty()) tag_.album_ = s;
			}

			const ID3v2::FrameListMap& map = v2->frameListMap();
			typedef ID3v2::FrameListMap::ConstIterator const_it;
			for(const_it cit = map.begin(); cit != map.end(); ++cit) {
				std::string key = convert_string_(cit->first);
				const ID3v2::FrameList& list = cit->second;
				typedef ID3v2::FrameList::ConstIterator const_it;
				std::string tmp;
				for(const_it cit = list.begin(); cit != list.end(); ++cit) {
					std::string s = convert_string_((*cit)->toString());
///					std::cout << key << ": '" << s << "'" << std::endl;
					tmp += s;
				}
				if(key == "TDRC") tag_.date_ = tmp;
				else if(key == "TPE2") tag_.writer_ = tmp;
				else if(key == "TPOS") tag_.disc_ = tmp;
				else if(key == "TRCK") tag_.track_ = tmp;
				else if(key == "TIT3") {
					if(tag_.title_.empty()) tag_.title_ = tmp;
				}
			}

			// ジャケット画像
			const_it acit = map.find("APIC");
			if(static_cast<uint8_t>(st) & static_cast<uint8_t>(info_state::apic) && acit != map.end()) {
				const ID3v2::FrameList& list = acit->second;
				typedef ID3v2::FrameList::ConstIterator const_it;
				for(const_it cit = list.begin(); cit != list.end(); ++cit) {
//					size_t fsize = (*cit)->size();
//					std::cout << (*cit)->toString() << std::endl;
//					std::cout << "Frame size: " << static_cast<int>(fsize) << std::endl;
					const ByteVector& image = (*cit)->render();
//					std::cout << "Render size: " << static_cast<int>(image.size()) << std::endl;
					int skip = 11;
					const char* p = image.data();
					// mime type
					std::string mime;
					while(p[skip] != 0) {
						mime += p[skip];
						++skip;
					}
					++skip;
					tag_.image_mime_ = mime;

					tag_.image_cover_ = p[skip];
					skip += 1;	// picture type (cover)

					// discription
					std::string dscrp;
					while(p[skip] != 0) {
						dscrp += p[skip];
						++skip;
					}
					++skip;
					tag_.image_dscrp_ = dscrp;

					int len = image.size();
					if(len > skip) {
						len -= skip;
						p += skip;		
						tag_.image_ = utils::shared_array_u8(new utils::array_u8);
						tag_.image_->copy(p, len);
						break;
					}
				}
			}
		}
		// tag 情報が無い場合、ファイル名を曲名としておく
		if(tag_.title_.empty()) {
			tag_.title_ = utils::get_file_base(utils::get_file_name(fin.get_path()));
		}
		tag_.update();
		fin.re_open();

		if(!fin.is_open()) {
			return false;
		}

		if(ofs) {
			fin.seek(ofs, file_io::seek::set);
		} else {
			fin.seek(pos, file_io::seek::set);
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
//		int samples = 0;
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

		fin.seek(pos, file_io::seek::set);

// std::cout << boost::format("error: %d\n") % error;
// std::cout << boost::format("frame: %d\n") % frame_cnt;

		if(error == 0 && frame_cnt > limit_frame && mp3info.unrecover_frame_error == 0) {
			mp3info.frame_count = frame_cnt;
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void mp3_io::initialize()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロードする
		@param[in]	fin	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool mp3_io::load(utils::file_io& fin, const std::string& opt)
	{
		audio_info info;
		mp3_info_.reset();
		bool f = analize_frame_(fin, info, mp3_info_, info_state::all);
		if(f) {
			fin.seek(info.header_size, file_io::seek::set);

			audio aif;
			if(info.chanels == 1) {
				aif = audio(new audio_mno16);
			} else if(info.chanels == 2) {
				aif = audio(new audio_sto16);
			}
			if(aif) {
//				std::cout << boost::format("Sampling: %d [Hz]\n") % info.frequency;
//				std::cout << boost::format("Samples:  %d\n") % info.samples;
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
	bool mp3_io::save(utils::file_io& fout, const std::string& opt)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム・オープンする
		@param[in]	fi		file_io クラス
		@param[in]	size	バッファサイズ
		@param[in]	inf		オーディオ情報を受け取る
		@param[in]	sseek	スタート・シーク
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool mp3_io::open_stream(utils::file_io& fi, int size, audio_info& inf)
	{
		close_stream();

		output_max_ = 1152 * output_buffer_size_;

		offset_ = 0;

		if(info(fi, inf)) {
			fi.seek(inf.header_size, file_io::seek::set);
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
//				std::cout << boost::format("Stream Sampling: %d [Hz]\n") % inf.frequency;
//				std::cout << boost::format("Stream Samples:  %d\n") % inf.samples;
				return true;
			}
		}
		return false;
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
	size_t mp3_io::read_stream(utils::file_io& fin, size_t offset, size_t samples)
	{
		if(stream_ == 0) return 0;

		int f = 0;
		bool status = true;
		size_t end_pos = offset + samples;

		if(offset != 0 && (offset_ + samples) != offset) {	// seek を検出
			uint32_t n = offset / 1152;
			if(n < seek_points_.size()) {
				fin.seek(seek_points_[n], utils::file_io::seek::set);
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
	void mp3_io::close_stream()
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
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void mp3_io::destroy()
	{
		close_stream();
	}

}
