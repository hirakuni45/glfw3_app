#pragma once
//=========================================================================//
/*!	@file
	@brief	OpenAL オーディオ入出力
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <unistd.h>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#endif

#if defined(IPHONE) || defined(IPAD) || defined(IPHONE_IPAD)
#include <OpenAL/alc.h>
#include <OpenAL/al.h>
#ifndef IPHONE_IPAD
#define IPHONE_IPAD
#endif
#endif

#include "utils/file_io.hpp"
#include "snd_io/i_audio.hpp"
#include "snd_io/pcm.hpp"
#include <iostream>
#include <boost/format.hpp>

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	audioio クラス（OpenAL のラッパークラス）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class audio_io {

	public:
		typedef ALuint	wave_handle;
		typedef ALuint	slot_handle;

	private:
		ALCdevice*	cap_device_;

		ALCdevice*	device_;
		ALCcontext*	context_;

		std::string	error_message_;

		int		queue_max_;
		bool	init_;
		bool	destroy_;

		bool set_buffer_(ALuint bh, const audio aif)
		{
			ALsizei num = aif->get_samples();
			ALenum format;
			switch(aif->get_type()) {
			case audio_format::PCM8_MONO:
				format = AL_FORMAT_MONO8;
				num *= 1;
				break;
			case audio_format::PCM8_STEREO:
				format = AL_FORMAT_STEREO8;
				num *= 2;
				break;
			case audio_format::PCM16_MONO:
				format = AL_FORMAT_MONO16;
				num *= 2;
				break;
			case audio_format::PCM16_STEREO:
				format = AL_FORMAT_STEREO16;
				num *= 4;
				break;
#if 0
			case audio_format::PCM24_MONO:
				format = AL_FORMAT_MONO_FLOAT32;
				num *= 4;
				break;
			case audio_format::PCM24_STEREO:
				format = AL_FORMAT_STEREO_FLOAT32;
				num *= 8;
				break;
#endif
			default:
				return false;
				break;
			}
			alBufferData(bh, format, aif->get_wave(), num, static_cast<ALsizei>(aif->get_rate()));
			return true;
		}

		static constexpr int indentation = 4;
		static constexpr int maxmimumWidth = 79;

	public:
		static void printChar(int c, int& width)
		{
			putchar(c);
			width = ((c == '\n') ? 0 : ((width) + 1));
		}

		static void indent(int& width)
		{
			for(int i = 0; i < indentation; i++) {
				printChar(' ', width);
			}
		}


		static void checkForErrors()
		{
			{
				ALCdevice* device = alcGetContextsDevice(alcGetCurrentContext());
				ALCenum error = alcGetError(device);
				if(error != ALC_NO_ERROR) {
					std::cout << boost::format("ALC error: '%s'\n")
						% static_cast<const char*>(alcGetString(device, error));
				}
			}
			{
				ALenum error = alGetError();
				if(error != AL_NO_ERROR) {
					std::cout << boost::format("AL error: '%s'\n")
						% static_cast<const char*>(alGetString(error));
				}
			}
		}


		static void printDevices(ALCenum which, const char* kind)
		{
			const char *s = alcGetString(NULL, which);
//			checkForErrors();

			std::cout << boost::format("Available %sdevices:\n") % kind;
			while(*s != '\0') {
				std::cout << boost::format("    %s\n") % s;
				while(*s++ != '\0') ;
			}
		}


		static void printExtensions(const char* header, char separator, const char* extensions)
		{
			std::cout << boost::format("%s:\n") % header;
			if(extensions == NULL || extensions[0] == '\0') return;

			int width = 0, start = 0, end = 0;
			indent(width);
			while(1) {
				if(extensions[end] == separator || extensions[end] == '\0') {
					if(width + end - start + 2 > maxmimumWidth) {
						printChar('\n', width);
						indent(width);
					}
					while(start < end) {
						printChar(extensions[start], width);
						start++;
					}
					if(extensions[end] == '\0') break;
					start++;
					end++;
					if(extensions[end] == '\0') break;
					printChar(',', width);
					printChar(' ', width);
				}
				end++;
			}
			printChar('\n', width);
		}

		static auto create_device_list(ALCenum which)
		{
			utils::strings ss;
			const char* s = alcGetString(NULL, which);
			while(*s != '\0') {
				ss.push_back(s);
				while(*s++ != '\0') ;
			}
			return ss;
		}

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		audio_io() noexcept :
			cap_device_(nullptr), device_(nullptr), context_(nullptr),
			queue_max_(0), init_(false), destroy_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~audio_io() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ標準出力デバイス名を取得
			@return オーディオ標準出力デバイス名
		*/
		//-----------------------------------------------------------------//
		std::string get_def_output_name() const noexcept
		{
			auto ss = create_device_list(ALC_ALL_DEVICES_SPECIFIER);
			return ss[0];
//			return alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ出力デバイス名を取得
			@return オーディオ出力デバイス名
		*/
		//-----------------------------------------------------------------//
		utils::strings get_output_name() const noexcept
		{
			return create_device_list(ALC_ALL_DEVICES_SPECIFIER);
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ標準入力デバイス名を取得
			@return オーディオ標準入力デバイス名
		*/
		//-----------------------------------------------------------------//
		std::string get_def_input_name() const noexcept
		{
			return alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ入力デバイス名を取得
			@return オーディオ入力デバイス名
		*/
		//-----------------------------------------------------------------//
		utils::strings get_input_name() const noexcept
		{
			return create_device_list(ALC_CAPTURE_DEVICE_SPECIFIER);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ドライバー・オープン
			@param[in]	input	入力デバイス名
			@param[in]	output	出力デバイス名
			@param[in]	srate	サンプリングレート
			@param[in]	capnum	キャプチャーリングバッファサイズ
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const char* input, const char* output, uint32_t srate, uint32_t capnum = 2048) noexcept
		{
			int err = 0;
			cap_device_ = alcCaptureOpenDevice(input, srate, AL_FORMAT_STEREO16, capnum);
			if(cap_device_ != nullptr) {
				// utils::format("CapDev: Open OK...\n");
			} else {
				++err;
				utils::format("Capture device: Open error: '%s'\n") % input;
			}

			device_ = alcOpenDevice(output);
			if(device_ == nullptr) {
				ALenum error = alGetError();
				if(error != AL_NO_ERROR) {
					++err;
					utils::format("Playback device: Open error: '%s'\n") % output;
				}
			}

			if(device_ != nullptr) {
				context_ = alcCreateContext(device_, NULL);
				if(context_ == nullptr) {
					ALCenum error = alcGetError(device_);
					if(error != ALC_NO_ERROR) {
						++err;
						utils::format("Create context error\n");
					}
				}
				alcMakeContextCurrent(context_);
			}

			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	qmax	キューバッファの最大数（通常６４）
		*/
		//-----------------------------------------------------------------//
		void initialize(int qmax = 64) noexcept
		{
			queue_max_ = qmax;

			if(init_) return;

			if(open(NULL, NULL, 48'000)) {
				init_ = true;
				destroy_ = false;
			}			
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コンテキストの情報を表示（OpenAL）
		*/
		//-----------------------------------------------------------------//
		void context_info() const noexcept
		{
			std::cout << boost::format("vendor string: %s\n") % alGetString(AL_VENDOR);
			std::cout << boost::format("renderer string: %s\n") % alGetString(AL_RENDERER);
			std::cout << boost::format("version string: %s\n") % alGetString(AL_VERSION);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ALC の情報を表示
		*/
		//-----------------------------------------------------------------//
		void alc_info() const noexcept
		{
			if(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE) {
				if(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATE_ALL_EXT") == AL_TRUE) {
					printDevices(ALC_ALL_DEVICES_SPECIFIER, "playback ");
				} else {
					printDevices(ALC_DEVICE_SPECIFIER, "playback ");
					printDevices(ALC_CAPTURE_DEVICE_SPECIFIER, "capture ");
				}
			} else {
				std::cout << boost::format("No device enumeration available\n");
			}

			ALCdevice* device = alcGetContextsDevice(alcGetCurrentContext());
			checkForErrors();

			std::cout << boost::format("Default device: %s\n")
				% alcGetString(device, ALC_DEFAULT_DEVICE_SPECIFIER);

			std::cout << boost::format("Default capture device: %s\n")
				% alcGetString(device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);

			ALCint major, minor;
			alcGetIntegerv(device, ALC_MAJOR_VERSION, 1, &major);
			alcGetIntegerv(device, ALC_MINOR_VERSION, 1, &minor);
			checkForErrors();
			std::cout << boost::format("ALC version: %d.%d\n") % static_cast<int>(major) % static_cast<int>(minor);

			printExtensions("ALC extensions", ' ',
				alcGetString(device, ALC_EXTENSIONS));
			checkForErrors();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・インターフェースから波形（buffer）を作成する
			@param[in]	aif	オーディオ・インターフェース・クラス
			@return	波形・ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		wave_handle create_wave(const audio aif) noexcept
		{
			if(aif == 0) return 0;

			wave_handle wh;
			alGenBuffers(1, &wh);
			set_buffer_(wh, aif);

			if(wh == AL_NONE) {
///				ALenum alerror = alGetError();
				return 0;
			}
			return wh;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	波形を廃棄する。
			@param[in]	wh	波形ハンドル
		*/
		//-----------------------------------------------------------------//
		void destroy_wave(wave_handle wh) noexcept
		{
			alDeleteBuffers(1, &wh);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロット（ソース）を作成する。
			@param[in]	wh	波形・ハンドル
			@return スロット・ハンドルを返す。
		*/
		//-----------------------------------------------------------------//
		slot_handle create_slot(wave_handle wh) noexcept
		{
			slot_handle sh;
			alGenSources(1, &sh);
			if(wh) {
				alSourcei(sh, AL_BUFFER, wh);
			}
			return sh;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットにループを設定する。
			@param[in]	sh	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void set_loop(slot_handle sh, bool flag = true) noexcept
		{
			if(flag) {
				alSourcei(sh, AL_LOOPING, AL_TRUE);
			} else {
				alSourcei(sh, AL_LOOPING, AL_FALSE);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットにゲインを設定する
			@param[in]	sh	スロット・ハンドル
			@param[in]	gain	ゲイン値（0.0 to 1.0)
		*/
		//-----------------------------------------------------------------//
		void set_gain(slot_handle sh, float gain) noexcept
		{
			alSourcef(sh, AL_GAIN, gain);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	再生
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool play(slot_handle sh) noexcept
		{
			bool ret = true;
			if(sh) {
				alSourcePlay(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	一時停止
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool pause(slot_handle sh) noexcept
		{
			bool ret = true;
			if(sh) {
				alSourcePause(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	停止
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool stop(slot_handle sh) noexcept
		{
			bool ret = true;
			if(sh) {
				alSourceStop(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	巻き戻し
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool rewind(slot_handle sh) noexcept
		{
			bool ret = true;
			if(sh) {
				alSourceRewind(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		bool get_slot_init(slot_handle sh) const noexcept
		{
			if(sh == 0) {
				return false;
			}
			ALint status;
			alGetSourcei(sh, AL_SOURCE_STATE, &status);
			return status == AL_INITIAL;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットの演奏状態を得る
			@param[in]	sh	スロット・ハンドル
			@return	「true」なら演奏中
		*/
		//-----------------------------------------------------------------//
		bool get_slot_status(slot_handle sh) const noexcept
		{
			if(sh == 0) {
				return false;
			}
			ALint status;
			alGetSourcei(sh, AL_SOURCE_STATE, &status);
			return status == AL_PLAYING;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットを廃棄する。
			@param[in]	h	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void destroy_slot(slot_handle h) noexcept
		{
			alDeleteSources(1, &h);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットに、波形をセットする
			@param[in]	sh	再生するスロット・ハンドル
			@param[in]	wh	波形ハンドル
			@return	成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_wave(slot_handle sh, wave_handle wh) noexcept
		{
			if(sh != 0 && wh != 0) {
				alSourcei(sh, AL_BUFFER, wh);
//				std::cout << boost::format("set_wave: slot: %d, wave: %d\n") % sh % wh;
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを浄化する
			@param[in]	ssh	ストリーム・スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void purge_stream(slot_handle ssh) noexcept
		{
			if(ssh) {
				alSourceStop(ssh);

				// キューイングされたバッファを取り除く～
				ALint n;
				do {
					ALuint bh;
					alSourceUnqueueBuffers(ssh, 1, &bh);
					alDeleteBuffers(1, &bh);
					alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);
				} while(n != 0) ;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームのバッファが消費されるまで同期する
			@param[in]	ssh	ストリーム・スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void sync_stream(slot_handle ssh) noexcept
		{
			if(ssh) {
				ALint n;
				alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);
				if(n == 0) return;
				ALint num;
				do {
					alGetSourcei(ssh, AL_BUFFERS_PROCESSED, &num);	// キューバッファ利用完了数を取得
					usleep(5000);
				} while(num < n) ;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生の空きバッファを返す。
			@param[in]	ssh	ストリーム・スロット・ハンドル
			@return 有効な、バッファがあれば、そのハンドルを返す。（「０」なら無効）
		*/
		//-----------------------------------------------------------------//
		wave_handle status_stream(slot_handle ssh) noexcept
		{
			ALint state;
			alGetSourcei(ssh, AL_SOURCE_STATE, &state);
			if(state == AL_PAUSED) {
				return 0;
			}
			wave_handle bh = 0;
			ALint n;
			static ALint qn = 0;
			alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);
			if(qn != n) {
				qn = n;
///				std::cout << "Queue buffer: " << qn << std::endl;
			}
			if(n == 0) {
				bh = 0;
				alGenBuffers(1, &bh);
				if(bh == 0) {
std::cout << "alGenBuffers NG!" << std::endl;
					return 0;
				}
			} else {
				alGetSourcei(ssh, AL_BUFFERS_PROCESSED, &n);
				if(n != 0) {
					alSourceUnqueueBuffers(ssh, 1, &bh);
///					std::cout << "Unqueue" << std::endl;
				} else {
					alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);
					if(n < queue_max_) {
						bh = 0;
						alGenBuffers(1, &bh);
						if(bh == 0) {
std::cout << "alGenBuffers NG!" << std::endl;
							return 0;
						}
					} else {
						static uint32_t cnt = 0;
///						std::cout << "No handle: " << cnt << std::endl;
						++cnt;
					}
				}
			}
			return bh;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生キューにバッファをセットする。
			@param[in]	ssh	ストリーム・スロット・ハンドル
			@param[in]	bh	有効なバッファ・ハンドル
			@param[in]	aif	バッファにセットするオーディオ・インターフェース@n
							※常に、同じ構成を与える必要がある。
		*/
		//-----------------------------------------------------------------//
		void queue_stream(slot_handle ssh, wave_handle bh, const audio aif) noexcept
		{
			if(ssh == 0 || bh == 0 || aif == 0) return;

			set_buffer_(bh, aif);
			alSourceQueueBuffers(ssh, 1, &bh);

			// ストリーム・ソースを「PLAY」
			ALint state;
			alGetSourcei(ssh, AL_SOURCE_STATE, &state);
			if(state == AL_PAUSED) ;
			else if(state != AL_PLAYING) {
				alSourcePlay(ssh);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをポーズする。
			@param[in]	ssh	ストリーム・スロット・ハンドル
			@param[in]	ena	「false」ならポーズ解除
		*/
		//-----------------------------------------------------------------//
		void pause_stream(slot_handle ssh, bool ena = true) noexcept
		{
			if(ssh == 0) return;

			if(ena) {
//				std::cout << "Stream: Pause enable\n";
				alSourcePause(ssh);
			} else {
//				std::cout << "Stream: Pause disable\n";
				alSourcePlay(ssh);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャプチャー開始
			@return 開始出来たら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool start_capture() noexcept
		{
			if(cap_device_ == nullptr) {
				return false;
			}
			alcCaptureStart(cap_device_);
			auto id = alcGetError(cap_device_);
			return id == ALC_NO_ERROR;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャプチャー停止
			@return 成功なら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool stop_capture() noexcept
		{
			if(cap_device_ == nullptr) {
				return false;
			}
			alcCaptureStop(cap_device_);
			auto id = alcGetError(cap_device_);
			return id == ALC_NO_ERROR;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャプチャー数の取得
			@return キャプチャー数（バイト数）
		*/
		//-----------------------------------------------------------------//
		uint32_t num_capture() noexcept
		{
			if(cap_device_ == nullptr) {
				return 0;
			}
			alcGetError(cap_device_);
			static ALint num = 0;
			alcGetIntegerv(cap_device_, ALC_CAPTURE_SAMPLES, 1, &num);
			if(alcGetError(cap_device_) != ALC_NO_ERROR) {
				utils::format("num_capture: NG...\n");
			}

			if(num < 0) return 0;

			return static_cast<uint32_t>(num);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キャプチャーの取得
			@param[in]	num		キャプチャーするバイト数
			@param[in]	wavs	波形バッファ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_capture(uint32_t num, pcm16_s_waves& wavs) noexcept
		{
			if(cap_device_ == nullptr) {
				return false;
			}

			ALshort buffer[num * 2];
			alcCaptureSamples(cap_device_, buffer, num);

			wavs.resize(num);
			for(uint32_t i = 0; i < num; ++i) {
				wavs[i].l = buffer[i * 2 + 0];
				wavs[i].r = buffer[i * 2 + 1];
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() noexcept
		{
			if(destroy_) return;

			purge_stream(0);

			alcDestroyContext(context_);
			alcCloseDevice(device_);

			if(cap_device_ != nullptr) {
				stop_capture();

				alcCaptureCloseDevice(cap_device_);
				cap_device_ = nullptr;
			}

			destroy_ = true;
			init_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	最後のエラーメッセージを取得
			@return	エラーメッセージ
		*/
		//-----------------------------------------------------------------//
		const std::string& get_error_message() const noexcept { return error_message_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットの状態を表示
			@param[in]	sh	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void print_slots(slot_handle sh) const noexcept
		{
			std::string stt;
			ALint status;
			alGetSourcei(sh, AL_SOURCE_STATE, &status);
			switch(status) {
			case AL_INITIAL:
				stt = "INITIAL";
				break;
			case AL_STOPPED:
				stt = "STOPPED";
				break;
			case AL_PLAYING:
				stt = "PLAYING";
				break;
			case AL_PAUSED:
				stt = "PAUSED";
				break;
			}

			std::cout << boost::format("%u: '%s', ") % static_cast<unsigned int>(sh) % stt;
		}

#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生の実験コード
		@param[in]	filename	ファイル名
	*/
	//-----------------------------------------------------------------//
	void audio_io::test_stream(const char* filename)
	{
		static const int numbuff = 16;
		static const int bufsize = 4096;

		utils::fileio fin;
		if(!fin.open(filename, "rb")) {
			return;
		}
		wavio wav;
		audio_info info;
		wav.open_stream(fin, bufsize, info);
		const i_audio* aif = wav.get_stream();
		if(aif == 0) {
			wav.close_stream();
			fin.close();
			return;
		}

		int cnt = 0;

		ALuint source;
		alGenSources(1, &source);
		size_t pos = 0;
		while(pos < info.samples) {
			ALint n;
			alGetSourcei(source, AL_BUFFERS_QUEUED, &n);
			ALuint buffer;
			if(n < numbuff) {
				alGenBuffers(1, &buffer);
				++cnt;
			} else {
				ALint state;
				alGetSourcei(source, AL_SOURCE_STATE, &state);
				if(state != AL_PLAYING) {
					alSourcePlay(source);
				}
				while(alGetSourcei(source, AL_BUFFERS_PROCESSED, &n), n == 0) {
					useconds_t usec = 1000 * 10;	//[ms]
					usleep(usec);
				}
				alSourceUnqueueBuffers(source, 1, &buffer);
			}
			pos += wav.read_stream(fin, pos, bufsize);
			set_buffer(buffer, aif);
			alSourceQueueBuffers(source, 1, &buffer);
		}
		alDeleteSources(1, &source);

		wav.close_stream();
		fin.close();

		std::cout << boost::format("buffer: %d\n") % cnt;
	}
#endif

#if 0
		ALuint no;
		alGenBuffers(1, &no);
		ALshort data[8192];
		for(int i = 0; i < 8192; ++i) {
			// 580Hz
			data[i] = (ALshort)(cosf(2.0f * 3.141592f * (50.0f * (float)i) / 8192.0f) * 32767.0f);
		}
		alBufferData(no, AL_FORMAT_MONO16, data, 8192 * 2, 48000);

		audio_buffer2_ = no;

		alGenSources(1, &audio_source2_);
		alSourcei(audio_source2_, AL_BUFFER, audio_buffer2_);
		alSourcei(audio_source2_, AL_LOOPING, AL_TRUE);
		alSourcePlay(audio_source2_);
#endif
	};
}	// al
