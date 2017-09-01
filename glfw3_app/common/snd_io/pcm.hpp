#pragma once
//=====================================================================//
/*!	@file
	@brief	PCM 波形を扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include "i_audio.hpp"
#include "utils/file_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PCM 波形を扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T>
	class pcm : public i_audio {
		uint32_t		sample_rate_;
		std::vector<T>	waves_;

		T				zero_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pcm() : sample_rate_(0), zero_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~pcm() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	波形バッファを作成する
			@param[in]	rate	サンプリング・レート
			@param[in]	samples	サンプル数
		*/
		//-----------------------------------------------------------------//
		void create(uint32_t rate, int samples) {
			sample_rate_ = rate;
			waves_.resize(samples);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バッファを埋める
			@param[in]	w	PCM データ
		*/
		//-----------------------------------------------------------------//
		void fill(const T& w) { for(T& v : waves_) { v = w; } }


		//-----------------------------------------------------------------//
		/*!
			@brief	バッファを「０」クリアする
		*/
		//-----------------------------------------------------------------//
		void zero() { for(T& v : waves_) { v = zero_; } }


		//-----------------------------------------------------------------//
		/*!
			@brief	バッファをリサイズする
			@param[in]	newsize	新しいサイズ
		*/
		//-----------------------------------------------------------------//
		void resize(size_t newsize) { waves_.resize(newsize); }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・タイプを返す
			@return	オーディオ・タイプ
		*/
		//-----------------------------------------------------------------//
		audio_format get_type() const { return zero_.type(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	チャネル数を返す
			@return	チャネル数
		*/
		//-----------------------------------------------------------------//
		int get_chanel() const { return zero_.chanel(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	サンプリング・レートを返す
			@return	サンプリング・レート「Hz」
		*/
		//-----------------------------------------------------------------//
		uint32_t get_rate() const { return sample_rate_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サンプル数を返す
			@return	サンプル数
		*/
		//-----------------------------------------------------------------//
		size_t get_samples() const { return waves_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルからの読み込み
			@param[in]	fin	ファイル入力コンテキスト
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool read(utils::file_io& fin) {
			size_t s = waves_.size();
			if(zero_.bits() == 24) {
				for(size_t i = 0; i < s; ++i) {
					if(fin.read(&waves_[i], zero_.size()) != zero_.size()) return false;
				}
			} else {
				if(fin.read(&waves_[0], zero_.size(), s) != s) return false;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルへ出力
			@param[in]	fout	ファイル出力コンテキスト
			@return	正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool write(utils::file_io& fout) {
			size_t s = waves_.size();
			if(zero_.bits() == 24) {
				for(auto w : waves_) {
					if(fout.write(&w, zero_.size()) != zero_.size()) return false;
				}
			} else {
				if(fout.write(&waves_[0], zero_.size(), s) != s) return false;
			}
			return true;
		}


		void get(size_t pos, pcm8_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm8_s& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm16_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm16_s& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm24_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm24_s& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm32_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm32_s& wave) const { wave = waves_[pos]; }

		void put(size_t pos, const pcm8_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm8_s& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm16_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm16_s& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm24_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm24_s& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm32_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm32_s& wave) { waves_[pos] = wave; }


		void* at_wave(size_t n = 0) { return &waves_[n]; }


		const void* get_wave(size_t n = 0) const { return &waves_[n]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() { std::vector<T>().swap(waves_); }
	};

	typedef pcm<pcm8_m>		audio_mno8;
	typedef pcm<pcm8_s>		audio_sto8;
	typedef pcm<pcm16_m>	audio_mno16;
	typedef pcm<pcm16_s>	audio_sto16;
	typedef pcm<pcm24_m>	audio_mno24;
	typedef pcm<pcm24_s>	audio_sto24;
	typedef pcm<pcm32_m>	audio_mno32;
	typedef pcm<pcm32_s>	audio_sto32;

	template <class T>
	void copy_pcm_(const pcm<T>& src, pcm<T>& dst);

	inline void copy_pcm(const audio_mno8& src, audio_mno8& dst) {
		copy_pcm_<pcm8_m>(src, dst);
	}
	inline void copy_pcm(const audio_sto8& src, audio_sto8& dst) {
		copy_pcm_<pcm8_s>(src, dst);
	}
	inline void copy_pcm(const audio_mno16& src, audio_mno16& dst) {
		copy_pcm_<pcm16_m>(src, dst);
	}
	inline void copy_pcm(const audio_sto16& src, audio_sto16& dst) {
		copy_pcm_<pcm16_s>(src, dst);
	}
	inline void copy_pcm(const audio_mno24& src, audio_mno24& dst) {
		copy_pcm_<pcm24_m>(src, dst);
	}
	inline void copy_pcm(const audio_sto24& src, audio_sto24& dst) {
		copy_pcm_<pcm24_s>(src, dst);
	}
	inline void copy_pcm(const audio_mno32& src, audio_mno32& dst) {
		copy_pcm_<pcm32_m>(src, dst);
	}
	inline void copy_pcm(const audio_sto32& src, audio_sto32& dst) {
		copy_pcm_<pcm32_s>(src, dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	オーディオ・インターフェースの複製を作成する
		@param[in]	src	オーディオ・インターフェースのソース
		@return 複製されたオーディオ・インターフェース
	*/
	//-----------------------------------------------------------------//
	i_audio* copy_audio(const i_audio* src);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	オーディオフォーマットから波形生成
		@return 波形を返す
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline audio create_audio(audio_format aform) {
		audio a;
		switch(aform) {
		case audio_format::NONE:
			break;
		case audio_format::PCM8_MONO:
			a = audio(new audio_mno8);
			break;
		case audio_format::PCM8_STEREO:
			a = audio(new audio_sto8);
			break;
		case audio_format::PCM16_MONO:
			a = audio(new audio_mno16);
			break;
		case audio_format::PCM16_STEREO:
			a = audio(new audio_sto16);
			break;
		case audio_format::PCM24_MONO:
			a = audio(new audio_mno24);
			break;
		case audio_format::PCM24_STEREO:
			a = audio(new audio_sto24);
			break;
		case audio_format::PCM32_MONO:
			a = audio(new audio_mno32);
			break;
		case audio_format::PCM32_STEREO:
			a = audio(new audio_sto32);
			break;
		}
		return a;
	}
}
