#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenAL オーディオ入出力（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <vector>
#include <cmath>
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
#include "i_audio.hpp"
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
		ALCdevice*	device_;
		ALCcontext*	context_;

		std::string	error_message_;

		int		queue_max_;
		bool	init_;
		bool	destroy_;

		bool set_buffer_(ALuint bh, const audio aif);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		audio_io() : device_(nullptr), context_(nullptr),
					queue_max_(0), init_(false), destroy_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~audio_io() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
			@param[in]	qmax	キューバッファの最大数（通常６４）
		*/
		//-----------------------------------------------------------------//
		void initialize(int qmax = 64);


		//-----------------------------------------------------------------//
		/*!
			@brief	コンテキストの情報を表示（OpenAL）
		*/
		//-----------------------------------------------------------------//
		void context_info() const {
			std::cout << boost::format("vendor string: %s\n") % alGetString(AL_VENDOR);
			std::cout << boost::format("renderer string: %s\n") % alGetString(AL_RENDERER);
			std::cout << boost::format("version string: %s\n") % alGetString(AL_VERSION);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ALC の情報を表示
		*/
		//-----------------------------------------------------------------//
		void alc_info() const;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・インターフェースから波形（buffer）を作成する
			@param[in]	aif	オーディオ・インターフェース・クラス
			@return	波形・ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		wave_handle create_wave(const audio aif)
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
		void destroy_wave(wave_handle wh) {
			alDeleteBuffers(1, &wh);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロット（ソース）を作成する。
			@param[in]	wh	波形・ハンドル
			@return スロット・ハンドルを返す。
		*/
		//-----------------------------------------------------------------//
		slot_handle create_slot(wave_handle wh) {
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
		void set_loop(slot_handle sh, bool flag = true) {
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
		void set_gain(slot_handle sh, float gain) { alSourcef(sh, AL_GAIN, gain); }


		//-----------------------------------------------------------------//
		/*!
			@brief	再生
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool play(slot_handle sh) {
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
		bool pause(slot_handle sh) {
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
		bool stop(slot_handle sh) {
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
		bool rewind(slot_handle sh) {
			bool ret = true;
			if(sh) {
				alSourceRewind(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		bool get_slot_init(slot_handle sh) const {
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
		bool get_slot_status(slot_handle sh) const {
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
		void destroy_slot(slot_handle h) {
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
		bool set_wave(slot_handle sh, wave_handle wh) {
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
		void purge_stream(slot_handle ssh)
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
		void sync_stream(slot_handle ssh)
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
		wave_handle status_stream(slot_handle ssh)
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
				alGenBuffers(1, &bh);
			} else {
				alGetSourcei(ssh, AL_BUFFERS_PROCESSED, &n);
				if(n != 0) {
					alSourceUnqueueBuffers(ssh, 1, &bh);
///					std::cout << "Unqueue" << std::endl;
				} else {
					alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);
					if(n < queue_max_) {
						alGenBuffers(1, &bh);
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
		void queue_stream(slot_handle ssh, wave_handle bh, const audio aif) {
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
		void pause_stream(slot_handle ssh, bool ena = true) {
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
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	最後のエラーメッセージを取得
			@return	エラーメッセージ
		*/
		//-----------------------------------------------------------------//
		const std::string& get_error_message() const { return error_message_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットの状態を表示
			@param[in]	sh	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void print_slots(slot_handle sh) const;

	};

}	// al
