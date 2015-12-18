//=====================================================================//
/*!	@file
	@brief	WAV 音声ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "wav_io.hpp"
#include "pcm.hpp"

namespace al {

/// #define LOAD_INFO_

	using namespace utils;

	enum {
		WAVE_FORMAT_1M08 = 1,
		WAVE_FORMAT_1S08 = 2,
		WAVE_FORMAT_1M16 = 4,
		WAVE_FORMAT_1S16 = 8,
		WAVE_FORMAT_2M08 = 16,
		WAVE_FORMAT_2S08 = 32,
		WAVE_FORMAT_2M16 = 64,
		WAVE_FORMAT_2S16 = 128,
		WAVE_FORMAT_4M08 = 256,
		WAVE_FORMAT_4S08 = 512,
		WAVE_FORMAT_4M16 = 1024,
		WAVE_FORMAT_4S16 = 2048,
	};

	static const uint16_t WAVE_FORMAT_PCM = 0x0001;
	static const uint16_t WAVE_FORMAT_EXTENSIBLE = 0xFFFE;

	struct WAVEFILEHEADER {
		char	   	szRIFF[4];
		uint32_t	ulRIFFSize;
		char	   	szWAVE[4];
	};

	struct RIFFCHUNK {
		char   		szChunkName[4];
		uint32_t	ulChunkSize;
	};

	struct WAVEFMT {
		uint16_t	usFormatTag;
		uint16_t	usChannels;
		uint32_t	ulSamplesPerSec;
		uint32_t	ulAvgBytesPerSec;
		uint16_t	usBlockAlign;
		uint16_t	usBitsPerSample;
		uint16_t	usSize;
		uint16_t	usReserved;
		uint32_t	ulChannelMask;
		uint32_t	guidSubFormat;
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	WAV ファイルをパースする
		@param[in]	fin	ファイル入力コンテキスト
	*/
	//-----------------------------------------------------------------//
	bool wav_io::parse_header_(file_io& fin)
	{
		tag_.clear();
		tag_.title_ = utils::get_file_base(fin.get_path());

		type_ = wf_null;
		memset(&ext_, 0, sizeof(wave_format_extensible));
		data_size_ = 0;
		data_offset_ = 0;
		riff_num_ = 0;

		WAVEFILEHEADER wh;
		if(fin.read(&wh, 1, sizeof(WAVEFILEHEADER)) != sizeof(WAVEFILEHEADER)) {
			return false;
		}

		if(strncmp(wh.szRIFF, "RIFF", 4) == 0 && strncmp(wh.szWAVE, "WAVE", 4) == 0) ;
		else {
			return false;
		}

		RIFFCHUNK rc;
		while(fin.read(&rc, 1, sizeof(RIFFCHUNK)) == sizeof(RIFFCHUNK)) {
			size_t ofs = rc.ulChunkSize;
			if(strncmp(rc.szChunkName, "fmt ", 4) == 0) {
				if(rc.ulChunkSize <= sizeof(WAVEFMT)) {
					ofs = 0;
					WAVEFMT wf;
					if(fin.read(&wf, 1, rc.ulChunkSize) != rc.ulChunkSize) {
						return false;
					}
					// Determine if this is a WAVEFORMATEX or WAVEFORMATEXTENSIBLE wave file
					if(wf.usFormatTag == WAVE_FORMAT_PCM) {
						type_ = wf_ex;
						memcpy(&ext_.format, &wf, sizeof(wave_format_ex));
					} else if(wf.usFormatTag == WAVE_FORMAT_EXTENSIBLE) {
						type_ = wf_ext;
						memcpy(&ext_, &wf, sizeof(wave_format_extensible));
					}
				}
			} else if(strncmp(rc.szChunkName, "data", 4) == 0) {
				data_size_ = rc.ulChunkSize;
				data_offset_ = fin.tell();
				break;
			} else {

			}
			riff_num_++;
			if(ofs) {
				if(ofs & 1) ofs++;
				fin.seek(ofs, file_io::seek::cur);
			}
		}

		if(data_size_ > 0 && data_offset_ != 0 && ((type_ == wf_ex) || (type_ == wf_ext))) {
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	WAV ファイル作成
		@param[in]	fout	ファイル入力コンテキスト
		@param[in]	src		オーディオ・インターフェース
	*/
	//-----------------------------------------------------------------//
	bool wav_io::create_wav_(file_io& fout, const audio src)
	{
		int align;
		int bits;
		if(src->get_type() == audio_format::PCM8_MONO) { align = 1; bits = 8; }
		else if(src->get_type() == audio_format::PCM8_STEREO) { align = 2; bits = 8; }
		else if(src->get_type() == audio_format::PCM16_MONO) { align = 2; bits = 16; }
		else if(src->get_type() == audio_format::PCM16_STEREO) { align = 4; bits = 16; }
		else {
			return false;
		}
		int rate = static_cast<int>(src->get_rate());

		WAVEFILEHEADER wh;
		wh.szRIFF[0] = 'R';
		wh.szRIFF[1] = 'I';
		wh.szRIFF[2] = 'F';
		wh.szRIFF[3] = 'F';
		wh.ulRIFFSize = sizeof(RIFFCHUNK) * 2 + sizeof(wave_format_ex) + align * src->get_samples();
		wh.szWAVE[0] = 'W';
		wh.szWAVE[1] = 'A';
		wh.szWAVE[2] = 'V';
		wh.szWAVE[3] = 'E';
		if(fout.write(&wh, 1, sizeof(WAVEFILEHEADER)) != sizeof(WAVEFILEHEADER)) {
			return false;
		}

		RIFFCHUNK rc;
		rc.szChunkName[0] = 'f';
		rc.szChunkName[1] = 'm';
		rc.szChunkName[2] = 't';
		rc.szChunkName[3] = ' ';
		rc.ulChunkSize = sizeof(wave_format_ex);
		if(fout.write(&rc, 1, sizeof(RIFFCHUNK)) != sizeof(RIFFCHUNK)) {
			return false;
		}

		wave_format_ex ex;
		ex.format_tag        = WAVE_FORMAT_PCM;
		ex.channels          = src->get_chanel();
		ex.samples_per_sec   = rate;
		ex.avg_bytes_per_sec = rate * align;
		ex.block_align       = align;
		ex.bits_per_sample   = bits;
		if(fout.write(&ex, 1, sizeof(wave_format_ex)) != sizeof(wave_format_ex)) {
			return false;
		}

		rc.szChunkName[0] = 'd';
		rc.szChunkName[1] = 'a';
		rc.szChunkName[2] = 't';
		rc.szChunkName[3] = 'a';
		rc.ulChunkSize = src->get_samples() * align;
		if(fout.write(&rc, 1, sizeof(RIFFCHUNK)) != sizeof(RIFFCHUNK)) {
			return false;
		}

		if(fout.write(src->get_wave(), align, src->get_samples()) != src->get_samples()) {
			return false;
		}

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void wav_io::initialize()
	{
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
	bool wav_io::info(utils::file_io& fin, audio_info& info, info_state st)
	{
		long start_offset = fin.tell();
		bool f = parse_header_(fin);
		info.header_size = fin.tell() - start_offset;
		fin.seek(start_offset, utils::file_io::seek::set);
		if(f) {
			info.type = audio_format::NONE;
			if(ext_.format.channels == 1) {
				if(ext_.format.bits_per_sample == 8) {
					info.type = audio_format::PCM8_MONO;
				} else if(ext_.format.bits_per_sample == 16) {
					info.type = audio_format::PCM16_MONO;
				} else if(ext_.format.bits_per_sample == 24) {
					info.type = audio_format::PCM24_MONO;
				}
			} else if(ext_.format.channels == 2) {
				if(ext_.format.bits_per_sample == 8) {
					info.type = audio_format::PCM8_STEREO;
				} else if(ext_.format.bits_per_sample == 16) {
					info.type = audio_format::PCM16_STEREO;
				} else if(ext_.format.bits_per_sample == 24) {
					info.type = audio_format::PCM24_STEREO;
				}
			}
			info.samples     = data_size_ / ext_.format.block_align;
			info.chanels     = ext_.format.channels;
			info.bits        = ext_.format.bits_per_sample;
			info.frequency   = ext_.format.samples_per_sec;
			info.block_align = ext_.format.block_align;
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロードする
		@param[in]	fin	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@param[in]	成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool wav_io::load(utils::file_io& fin, const std::string& opt)
	{
		bool f = parse_header_(fin);
		if(f == false) {
#ifdef LOAD_INFO_
			std::cout << "parse error!" << std::endl;
#endif
		} else {
#ifdef LOAD_INFO_
			if(type_ == wf_ex) {
				std::cout << "Form: ex" << std::endl;
			} else {
				std::cout << "Form: ext" << std::endl;
			}
			std::cout << "Chanel:        " << static_cast<int>(ext_.format.channels) << std::endl;
			std::cout << "Sample / Sec:  " << static_cast<int>(ext_.format.samples_per_sec) << std::endl;
			std::cout << "Bytes / Sec:   " << static_cast<int>(ext_.format.avg_bytes_per_sec) << std::endl;
			std::cout << "Bits / Sample: " << static_cast<int>(ext_.format.bits_per_sample) << std::endl;
			std::cout << "Block Align:   " << static_cast<int>(ext_.format.block_align) << std::endl;
#endif
			audio aif;
			if(ext_.format.channels == 1) {		// mono
				if(ext_.format.bits_per_sample == 8) {
					aif = audio(new audio_mno8);
				} else if(ext_.format.bits_per_sample == 16) {
					aif = audio(new audio_mno16);
				} else if(ext_.format.bits_per_sample == 24) {
					aif = audio(new audio_mno24);
				} else {
					std::cerr << "Can't decode bit length: " << static_cast<int>(ext_.format.bits_per_sample) << std::endl;
					return false;
				}
			} else if(ext_.format.channels == 2) {	// stereo
				if(ext_.format.bits_per_sample == 8) {
					aif = audio(new audio_sto8);
				} else if(ext_.format.bits_per_sample == 16) {
					aif = audio(new audio_sto16);
				} else if(ext_.format.bits_per_sample == 24) {
					aif = audio(new audio_sto24);
				} else {
					std::cerr << "Can't decode bit length: " << static_cast<int>(ext_.format.bits_per_sample) << std::endl;
					return false;					
				}
			} else {
			}
			if(aif) {
				if(!fin.seek(data_offset_, utils::file_io::seek::set)) {
					return false;
				}
				aif->create(ext_.format.samples_per_sec,
							data_size_ / ext_.format.block_align);
				f = aif->read(fin);
				if(f) {
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
		@brief	ストリーム・オープンする
		@param[in]	fi		file_io クラス
		@param[in]	size	バッファサイズ
		@param[in]	inf		オーディオ情報を受け取る
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool wav_io::open_stream(utils::file_io& fi, int size, audio_info& inf)
	{
		stream_blocks_ = 0;

		if(info(fi, inf, info_state::all)) {
			if(inf.chanels == 1) {
				if(inf.bits == 8) {
					stream_ = audio(new audio_mno8);
					stream_blocks_ = 1;
				} else if(inf.bits == 16) {
					stream_ = audio(new audio_mno16);
					stream_blocks_ = 2;
				} else if(inf.bits == 24) {
					stream_ = audio(new audio_mno24);
					stream_blocks_ = 3;
				}
			} else if(inf.chanels == 2) {
				if(inf.bits == 8) {
					stream_ = audio(new audio_sto8);
					stream_blocks_ = 2;
				} else if(inf.bits == 16) {
					stream_ = audio(new audio_sto16);
					stream_blocks_ = 4;
				} else if(inf.bits == 16) {
					stream_ = audio(new audio_sto24);
					stream_blocks_ = 6;
				}
			}

			if(stream_) {
				stream_->create(inf.frequency, size);
				fi.seek(inf.header_size, utils::file_io::seek::set);
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
	size_t wav_io::read_stream(utils::file_io& fin, size_t offset, size_t samples)
	{
		if(stream_ == 0) return 0;

		if(fin.get_file_size() <= (data_offset_ + offset * stream_blocks_)) {
			return 0;
		}

		if(!fin.seek(data_offset_ + offset * stream_blocks_, utils::file_io::seek::set)) {
			return 0;
		}

		size_t n = 0;
		if(stream_blocks_ == 3 || stream_blocks_ == 6) {
			for(size_t i = 0; i < samples; ++i) {
				n += fin.read(stream_->at_wave(i), stream_blocks_);
			}
		} else {
			n = fin.read(stream_->at_wave(), 1, samples * stream_blocks_);
		}
		return n / stream_blocks_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームをクローズ
	*/
	//-----------------------------------------------------------------//
	void wav_io::close_stream()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	セーブする
		@param[in]	fout	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return 成功したら「true」が返る。
	*/
	//-----------------------------------------------------------------//
	bool wav_io::save(utils::file_io& fout, const std::string& opt)
	{
		if(!audio_source_) {
			return false;
		}

		if(audio_source_->get_samples() == 0) {
			return false;
		}

		if(!create_wav_(fout, audio_source_)) {
			return false;
		}

		return true;
	}



	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void wav_io::destroy()
	{
		close_stream();
	}

}
