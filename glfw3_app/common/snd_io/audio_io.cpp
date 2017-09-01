//=====================================================================//
/*!	@file
	@brief	OpenAL オーディオ入出力
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <unistd.h>
#include <ctime>
#include "snd_io/audio_io.hpp"
#include "snd_io/pcm.hpp"
#ifdef WIN32
#include <windows.h>
#endif

namespace al {

	using namespace utils;

	static const int indentation = 4;
	static const int maxmimumWidth = 79;

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
		checkForErrors();

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


	bool audio_io::set_buffer_(ALuint bh, const audio aif)
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


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		@param[in]	qmax	キューバッファの最大数（通常６４）
	*/
	//-----------------------------------------------------------------//
	void audio_io::initialize(int qmax)
	{
		queue_max_ = qmax;

		if(init_) return;

		device_ = alcOpenDevice(nullptr);
		if(device_ == nullptr) {
			ALenum error = alGetError();
			if(error != AL_NO_ERROR) {
				std::cout << "OpenAL error: alcOpenDevice" << std::endl;
			// die("AL", (const char*)alGetString(error));
			}
			return;
		}

		context_ = alcCreateContext(device_, nullptr);
		if(context_ == nullptr) {
			ALCenum error = alcGetError(device_);
			if(error != ALC_NO_ERROR) {
			// die("ALC", (const char*)alcGetString(device, error));
			}
			return;
		}
		alcMakeContextCurrent(context_);
		init_ = true;
		destroy_ = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ALC の情報を表示
	*/
	//-----------------------------------------------------------------//
	void audio_io::alc_info() const
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


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void audio_io::destroy()
	{
		if(destroy_) return;

		purge_stream(0);

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context_);
		alcCloseDevice(device_);
		destroy_ = true;
		init_ = true;
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	スロットの状態を表示
		@param[in]	sh	スロット・ハンドル
	*/
	//-----------------------------------------------------------------//
	void audio_io::print_slots(slot_handle sh) const
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
}
