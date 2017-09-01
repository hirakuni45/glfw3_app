#pragma once
//=====================================================================//
/*!	@file
	@brief	オーディオの定義
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <vector>
#include <ctime>

namespace al {

	typedef int8_t	s8;
	typedef int16_t	s16;
	typedef int32_t	s32;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	オーディオのタイプ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	enum class audio_format {
		NONE,			///< 不明なタイプ
		PCM8_MONO,		///< PCM  8 ビット、モノラル
		PCM8_STEREO,	///< PCM  8 ビット、ステレオ
		PCM16_MONO,		///< PCM 16 ビット、モノラル
		PCM16_STEREO,	///< PCM 16 ビット、ステレオ
		PCM24_MONO,		///< PCM 24 ビット、モノラル
		PCM24_STEREO,	///< PCM 24 ビット、ステレオ
		PCM32_MONO,		///< PCM 32 ビット、モノラル
		PCM32_STEREO,	///< PCM 32 ビット、モノラル
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PCM 情報構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct audio_info {
		audio_format	type;			///< オーディオ・タイプ
		uint32_t		samples;		///< サンプル数
		uint32_t		chanels;		///< チャンネル数
		uint32_t		bits;			///< チャネル辺りの量子化ビット数
		uint32_t		frequency;		///< サンプリング周波数[Hz]
		uint32_t		block_align;	///< ブロック辺りのバイト数
		uint32_t		header_size;	///< インフォメーション・ヘッダーのサイズ

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		audio_info() : type(audio_format::NONE), samples(0), chanels(0), bits(0), frequency(0),
			block_align(0), header_size(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サンプル位置を時間に変換
			@param[in]	spos	サンプル位置
			@param[in]	t		秒単位の時間
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool sample_to_time(uint32_t spos, time_t& t) {
			if(frequency <= 0) return false;
			t = spos / frequency;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サンプル情報を出力
		*/
		//-----------------------------------------------------------------//
		void infomation() const {
			std::cout << "Audio infomations:" << std::endl;
			std::cout << "    Samples:     " << static_cast<int>(samples) << std::endl;
			std::cout << "    Chanels:     " << chanels << std::endl;
			std::cout << "    Bits/Chanel: " << bits << std::endl;
			std::cout << "    Sample Rate: " << frequency << " [Hz]" << std::endl; 
		}
	};

	struct pcm8_m;
	struct pcm8_s;
	struct pcm16_m;
	struct pcm16_s;
	struct pcm24_m;
	struct pcm24_s;
	struct pcm32_m;
	struct pcm32_s;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm8_m {
		s8	w;
		pcm8_m() { }
		pcm8_m(s8 _w) : w(_w) { }

		void set(int v) { w = v; }
		audio_format type() const { return audio_format::PCM8_MONO; }
		int chanel() const { return 1; }
		int bits() const { return 8; }
		size_t size() const { return 1; }

		pcm8_m& operator= (const pcm8_s& a);
		pcm8_m& operator= (const pcm16_m& a);
		pcm8_m& operator= (const pcm16_s& a);
		pcm8_m& operator= (const pcm24_m& a);
		pcm8_m& operator= (const pcm24_s& a);
		pcm8_m& operator= (const pcm32_m& a);
		pcm8_m& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm8_m>					pcm8_m_waves;
	typedef std::vector<pcm8_m>::iterator		pcm8_m_waves_it;
	typedef std::vector<pcm8_m>::const_iterator	pcm8_m_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm8_s {
		s8	l, r;
		pcm8_s() { }
		pcm8_s(s8 _w) : l(_w), r(_w) { }
		pcm8_s(s8 _l, s8 _r) : l(_l), r(_r) { }

		void set(int v) { l = r = v; }
		void set(int _l, int _r) { l = _l; r = _r; }
		audio_format type() const { return audio_format::PCM8_STEREO; }
		int chanel() const { return 2; }
		int bits() const { return 8; }
		size_t size() const { return 1; }

		pcm8_s& operator= (const pcm8_m& a);
		pcm8_s& operator= (const pcm16_m& a);
		pcm8_s& operator= (const pcm16_s& a);
		pcm8_s& operator= (const pcm24_m& a);
		pcm8_s& operator= (const pcm24_s& a);
		pcm8_s& operator= (const pcm32_m& a);
		pcm8_s& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm8_s>					pcm8_s_waves;
	typedef std::vector<pcm8_s>::iterator		pcm8_s_waves_it;
	typedef std::vector<pcm8_s>::const_iterator	pcm8_s_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm16_m {
		s16	w;
		pcm16_m() { }
		pcm16_m(s16 _w) : w(_w) { }

		void set(int v) { w = v; }
		audio_format type() const { return audio_format::PCM16_MONO; }
		int chanel() const { return 1; }
		int bits() const { return 16; }
		size_t size() const { return 2; }

		pcm16_m& operator= (const pcm8_m& a);
		pcm16_m& operator= (const pcm8_s& a);
		pcm16_m& operator= (const pcm16_s& a);
		pcm16_m& operator= (const pcm24_m& a);
		pcm16_m& operator= (const pcm24_s& a);
		pcm16_m& operator= (const pcm32_m& a);
		pcm16_m& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm16_m>					pcm16_m_waves;
	typedef std::vector<pcm16_m>::iterator			pcm16_m_waves_it;
	typedef std::vector<pcm16_m>::const_iterator	pcm16_m_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm16_s {
		s16	l, r;
		pcm16_s() { }
		pcm16_s(s16 _w) : l(_w), r(_w) { }
		pcm16_s(s16 _l, s16 _r) : l(_l), r(_r) { }

		void set(int v) { l = r = v; }
		void set(int _l, int _r) { l = _l; r = _r; }
		audio_format type() const { return audio_format::PCM16_STEREO; }
		int chanel() const { return 2; }
		int bits() const { return 16; }
		size_t size() const { return 4; }

		pcm16_s& operator= (const pcm8_m& a);
		pcm16_s& operator= (const pcm8_s& a);
		pcm16_s& operator= (const pcm16_m& a);
		pcm16_s& operator= (const pcm24_m& a);
		pcm16_s& operator= (const pcm24_s& a);
		pcm16_s& operator= (const pcm32_m& a);
		pcm16_s& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm16_s>					pcm16_s_waves;
	typedef std::vector<pcm16_s>::iterator			pcm16_s_waves_it;
	typedef std::vector<pcm16_s>::const_iterator	pcm16_s_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm24_m {
		s32	w;
		pcm24_m() { }
		pcm24_m(s32 _w) : w(_w) { }

		void set(int v) { w = v; }
		audio_format type() const { return audio_format::PCM24_MONO; }
		int chanel() const { return 1; }
		int bits() const { return 24; }
		size_t size() const { return 3; }

		pcm24_m& operator= (const pcm8_m& a);
		pcm24_m& operator= (const pcm8_s& a);
		pcm24_m& operator= (const pcm24_s& a);
		pcm24_m& operator= (const pcm16_m& a);
		pcm24_m& operator= (const pcm16_s& a);
		pcm24_m& operator= (const pcm32_m& a);
		pcm24_m& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm24_m>					pcm24_m_waves;
	typedef std::vector<pcm24_m>::iterator			pcm24_m_waves_it;
	typedef std::vector<pcm24_m>::const_iterator	pcm24_m_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm24_s {
		s32	l, r;
		pcm24_s() { }
		pcm24_s(s32 _w) : l(_w), r(_w) { }
		pcm24_s(s32 _l, s32 _r) : l(_l), r(_r) { }

		void set(int v) { l = r = v; }
		void set(int _l, int _r) { l = _l; r = _r; }
		audio_format type() const { return audio_format::PCM24_STEREO; }
		int chanel() const { return 2; }
		int bits() const { return 24; }
		size_t size() const { return 3; }

		pcm24_s& operator= (const pcm8_m& a);
		pcm24_s& operator= (const pcm8_s& a);
		pcm24_s& operator= (const pcm16_m& a);
		pcm24_s& operator= (const pcm16_s& a);
		pcm24_s& operator= (const pcm24_m& a);
		pcm24_s& operator= (const pcm32_m& a);
		pcm24_s& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm24_s>					pcm24_s_waves;
	typedef std::vector<pcm24_s>::iterator			pcm24_s_waves_it;
	typedef std::vector<pcm24_s>::const_iterator	pcm24_s_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm32_m {
		s32	w;
		pcm32_m() { }
		pcm32_m(s32 _w) : w(_w) { }

		void set(int v) { w = v; }
		audio_format type() const { return audio_format::PCM32_MONO; }
		int chanel() const { return 1; }
		int bits() const { return 32; }
		size_t size() const { return 4; }

		pcm32_m& operator= (const pcm8_m& a);
		pcm32_m& operator= (const pcm8_s& a);
		pcm32_m& operator= (const pcm16_m& a);
		pcm32_m& operator= (const pcm16_s& a);
		pcm32_m& operator= (const pcm24_m& a);
		pcm32_m& operator= (const pcm24_s& a);
		pcm32_m& operator= (const pcm32_s& a);
	};
	typedef std::vector<pcm32_m>					pcm32_m_waves;
	typedef std::vector<pcm32_m>::iterator			pcm32_m_waves_it;
	typedef std::vector<pcm32_m>::const_iterator	pcm32_m_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct pcm32_s {
		s32	l, r;
		pcm32_s() { }
		pcm32_s(s32 _w) : l(_w), r(_w) { }
		pcm32_s(s32 _l, s32 _r) : l(_l), r(_r) { }

		void set(int v) { l = r = v; }
		void set(int _l, int _r) { l = _l; r = _r; }
		audio_format type() const { return audio_format::PCM32_STEREO; }
		int chanel() const { return 2; }
		int bits() const { return 32; }
		size_t size() const { return 8; }

		pcm32_s& operator= (const pcm8_m& a);
		pcm32_s& operator= (const pcm8_s& a);
		pcm32_s& operator= (const pcm16_m& a);
		pcm32_s& operator= (const pcm16_s& a);
		pcm32_s& operator= (const pcm24_m& a);
		pcm32_s& operator= (const pcm24_s& a);
		pcm32_s& operator= (const pcm32_m& a);
	};
	typedef std::vector<pcm32_s>					pcm32_s_waves;
	typedef std::vector<pcm32_s>::iterator			pcm32_s_waves_it;
	typedef std::vector<pcm32_s>::const_iterator	pcm32_s_waves_cit;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm8_s& a) {
		w = (a.r >> 1) + (a.l >> 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm16_m& a) {
		w = a.w >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm16_s& a) {
		w = (a.l >> 1) + (a.r >> 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm24_m& a) {
		w = a.w >> 16;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm24_s& a) {
		w = (a.l >> 17) + (a.r >> 17);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm32_m& a) {
		w = a.w >> 24;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_m& pcm8_m::operator= (const pcm32_s& a) {
		w = ((a.l >> 1) + (a.r >> 1)) >> 24;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm8_m& a) {
		l = r = a.w;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm16_m& a) {
		l = r = a.w >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm16_s& a) {
		l = a.l >> 8;
		r = a.r >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm24_m& a) {
		l = r = a.w >> 16;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm24_s& a) {
		l = a.l >> 16;
		r = a.r >> 16;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm32_m& a) {
		l = r = a.w >> 24;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm8_s& pcm8_s::operator= (const pcm32_s& a) {
		l = a.l >> 24;
		r = a.r >> 24;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm8_m& a) {
		w = (a.w << 8) | ((a.w & 0x7f) << 1) | ((a.w >> 6) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm8_s& a) {
		int t = (a.l + a.r) >> 1;
		w = (t << 8) | ((t & 0x7f) << 1) | ((t >> 6) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm16_s& a) {
		w = (a.l >> 1) + (a.r >> 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm24_m& a) {
		w = a.w >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm24_s& a) {
		w = (a.l >> 9) + (a.r >> 9);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm32_m& a) {
		w = a.w >> 16;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_m& pcm16_m::operator= (const pcm32_s& a) {
		w = (a.l >> 17) + (a.r >> 17);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm8_m& a) {
		l = r = (a.w << 8) | ((a.w & 0x7f) << 1) | ((a.w >> 6) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm8_s& a) {
		l = (a.l << 8) | ((a.l & 0x7f) << 1) | ((a.l >> 6) & 1);
		r = (a.r << 8) | ((a.r & 0x7f) << 1) | ((a.r >> 6) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm16_m& a) {
		l = r = a.w;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm24_m& a) {
		l = r = a.w >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm24_s& a) {
		l = a.l >> 8;
		r = a.r >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm32_m& a) {
		l = r = a.w >> 16;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm16_s& pcm16_s::operator= (const pcm32_s& a) {
		l = a.l >> 16;
		r = a.r >> 16;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm8_m& a) {
		w = (a.w << 16) | ((a.w & 0x7f) << 9) | ((a.w & 0x7f) << 2) | ((a.w >> 5) & 3);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm8_s& a) {
		int t = (a.l + a.r) >> 1;
		w = (t << 16) | ((t & 0x7f) << 9) | ((t & 0x7f) << 2) | ((t >> 5) & 3);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm16_m& a) {
		w = (a.w << 8) | ((a.w & 0x7fff) >> 7);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm16_s& a) {
		int t = (a.l >> 1) + (a.r >> 1);
		w = (t << 8) | ((t & 0x7fff) >> 7);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm24_s& a) {
		w = (a.l >> 1) + (a.r >> 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm32_m& a) {
		w = a.w >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_m& pcm24_m::operator= (const pcm32_s& a) {
		w = (a.l >> 9) + (a.r >> 9);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm8_m& a) {
		l = r = (a.w << 16) | ((a.w & 0x7f) << 9) | ((a.w & 0x7f) << 2) | ((a.w & 0x7f) >> 5);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm8_s& a) {
		l = (a.l << 16) | ((a.l & 0x7f) << 9) | ((a.l & 0x7f) << 2) | ((a.l & 0x7f) >> 5);
		r = (a.r << 16) | ((a.r & 0x7f) << 9) | ((a.r & 0x7f) << 2) | ((a.r & 0x7f) >> 5);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm16_m& a) {
		l = r = (a.w << 8) | ((a.w & 0x7fff) >> 7);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm16_s& a) {
		l = (a.l << 8) | ((a.l & 0x7fff) >> 7);
		r = (a.r << 8) | ((a.r & 0x7fff) >> 7);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm24_m& a) {
		l = r = a.w;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm32_m& a) {
		l = r = a.w >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	24 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm24_s& pcm24_s::operator= (const pcm32_s& a) {
		l = a.l >> 8;
		r = a.r >> 8;
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm8_m& a) {
		w = (a.w << 24) | ((a.w & 0x7f) << 17) | ((a.w & 0x7f) << 10) | ((a.w & 0x7f) << 3) | ((a.w & 0x70) >> 4);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm8_s& a) {
		s8 w = (a.l + a.r) >> 1;
		w = (w << 24) | ((w & 0x7f) << 17) | ((w & 0x7f) << 10) | ((w & 0x7f) << 3) | ((w & 0x70) >> 4);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm16_m& a) {
		w = (a.w << 16) | ((a.w & 0x7fff) << 1) | ((a.w >> 14) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm16_s& a) {
		w = (a.l + a.r) >> 1;
		w = (w << 16) | ((w & 0x7fff) << 1) | ((w >> 14) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm24_m& a) {
		w = (a.w << 8) | ((a.w & 0x7fffff) >> 15);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm24_s& a) {
		int t = (a.l >> 1) + (a.r >> 1); 
		w = (t << 8) | ((t & 0x7fffff) >> 15);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、モノラル、＝オペレーター
		@param[in]	a	pcm32_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_m& pcm32_m::operator= (const pcm32_s& a) {
		w = (a.l >> 1) + (a.r >> 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm8_m& a) {
		l = r = (a.w << 24) | ((a.w & 0x7f) << 17) | ((a.w & 0x7f) << 10) | ((a.w & 0x7f) << 3)
			  | ((a.w & 0x70) >> 4);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm8_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm8_s& a) {
		l = (a.l << 24) | ((a.l & 0x7f) << 17) | ((a.l & 0x7f) << 10) | ((a.l & 0x7f) << 3) | ((a.l & 0x70) >> 4);
		r = (a.r << 24) | ((a.r & 0x7f) << 17) | ((a.r & 0x7f) << 10) | ((a.r & 0x7f) << 3) | ((a.r & 0x70) >> 4);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm16_m& a) {
		l = r = (a.w << 16) | ((a.w & 0x7fff) << 1) | ((a.w >> 14) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm16_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm16_s& a) {
		l = (a.l << 16) | ((a.l & 0x7fff) << 1) | ((a.l >> 14) & 1);
		r = (a.r << 16) | ((a.r & 0x7fff) << 1) | ((a.r >> 14) & 1);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm24_m& a) {
		l = r = (a.w << 8) | ((a.w & 0x7fffff) >> 15);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm24_s 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm24_s& a) {
		l = (a.l << 8) | ((a.l & 0x7fffff) >> 15);
		r = (a.r << 8) | ((a.r & 0x7fffff) >> 15);
		return *this;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	32 ビット PCM、ステレオ、＝オペレーター
		@param[in]	a	pcm32_m 入力
		@return 自分自身
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline pcm32_s& pcm32_s::operator= (const pcm32_m& a) {
		l = r = a.w;
		return *this;
	}
}
