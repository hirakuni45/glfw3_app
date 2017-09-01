#pragma once
//=====================================================================//
/*!	@file
	@brief	オーディオのインターフェースクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <memory>
#include <vector>
#include "utils/file_io.hpp"
#include "audio.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	オーディオ・インターフェース・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class i_audio {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	仮想デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~i_audio() { };


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオをバッファを確保する
			@param[in]	rate	サンプルレートを指定
			@param[in]	sample 	サンプル数を指定
		*/
		//-----------------------------------------------------------------//
		virtual void create(uint32_t rate, int samples) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	バッファを「０」クリアする
		*/
		//-----------------------------------------------------------------//
		virtual void zero() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオのタイプを返す
			@return オーディオタイプ
		*/
		//-----------------------------------------------------------------//
		virtual audio_format get_type() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	チャンネル数を返す
			@return チャンネル数
		*/
		//-----------------------------------------------------------------//
		virtual int get_chanel() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	サンプリングレートを返す
			@return サンプリングレート（周波数[Hz]）
		*/
		//-----------------------------------------------------------------//
		virtual uint32_t get_rate() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	サンプル数を返す
			@return サンプル数
		*/
		//-----------------------------------------------------------------//
		virtual size_t get_samples() const = 0;

		virtual void resize(size_t newsize) = 0;

		virtual bool read(utils::file_io& fio) = 0;
		virtual bool write(utils::file_io& fout) = 0;

		virtual void get(size_t pos, pcm8_m& wave) const = 0;
		virtual void get(size_t pos, pcm8_s& wave) const = 0;
		virtual void get(size_t pos, pcm16_m& wave) const = 0;
		virtual void get(size_t pos, pcm16_s& wave) const = 0;
		virtual void get(size_t pos, pcm24_m& wave) const = 0;
		virtual void get(size_t pos, pcm24_s& wave) const = 0;
		virtual void get(size_t pos, pcm32_m& wave) const = 0;
		virtual void get(size_t pos, pcm32_s& wave) const = 0;

		virtual void put(size_t pos, const pcm8_m& wave) = 0;
		virtual void put(size_t pos, const pcm8_s& wave) = 0;
		virtual void put(size_t pos, const pcm16_m& wave) = 0;
		virtual void put(size_t pos, const pcm24_s& wave) = 0;
		virtual void put(size_t pos, const pcm24_m& wave) = 0;
		virtual void put(size_t pos, const pcm16_s& wave) = 0;
		virtual void put(size_t pos, const pcm32_m& wave) = 0;
		virtual void put(size_t pos, const pcm32_s& wave) = 0;

		virtual void* at_wave(size_t n = 0) = 0;

		virtual const void* get_wave(size_t n = 0) const = 0;
	};

	typedef std::shared_ptr<i_audio>  audio;
	typedef std::vector<audio>  audios;
}
