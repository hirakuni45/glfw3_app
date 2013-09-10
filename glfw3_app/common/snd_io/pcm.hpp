#pragma once
//=====================================================================//
/*!	@file
	@brief	PCM 波形を扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/foreach.hpp>
#include "i_audio.hpp"
#include "utils/file_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PCM 波形を扱うクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class _T>
	class pcm : public i_audio {
		float			sample_rate_;
		std::vector<_T>	waves_;

		_T				zero_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pcm() : sample_rate_(0.0f), zero_(0) { }


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
		void create(float rate, int samples) {
			sample_rate_ = rate;
			waves_.resize(samples);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バッファを埋める
			@param[in]	w	PCM データ
		*/
		//-----------------------------------------------------------------//
		void fill(const _T& w) { BOOST_FOREACH(_T& v, waves_) { v = w; } }


		//-----------------------------------------------------------------//
		/*!
			@brief	バッファを「０」クリアする
		*/
		//-----------------------------------------------------------------//
		void zero() { BOOST_FOREACH(_T& v, waves_) { v = zero_; } }


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
		audio_format::type get_type() const { return zero_.type(); }


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
		float get_rate() const { return sample_rate_; }


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
			if(fin.read(&waves_[0], zero_.size(), s) != s) return false;
			else return true;
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
			if(fout.write(&waves_[0], zero_.size(), s) != s) return false;
			else return true;
		}


		void get(size_t pos, pcm8_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm8_s& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm16_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm16_s& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm32_m& wave) const { wave = waves_[pos]; }
		void get(size_t pos, pcm32_s& wave) const { wave = waves_[pos]; }

		void put(size_t pos, const pcm8_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm8_s& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm16_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm16_s& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm32_m& wave) { waves_[pos] = wave; }
		void put(size_t pos, const pcm32_s& wave) { waves_[pos] = wave; }


		void* at_wave(size_t n = 0) { return &waves_[n]; }


		const void* get_wave(size_t n = 0) const { return &waves_[n]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() { std::vector<_T>().swap(waves_); }
	};

	typedef pcm<pcm8_m>		audio_mno8;
	typedef pcm<pcm8_s>		audio_sto8;
	typedef pcm<pcm16_m>	audio_mno16;
	typedef pcm<pcm16_s>	audio_sto16;
	typedef pcm<pcm32_m>	audio_mno32;
	typedef pcm<pcm32_s>	audio_sto32;

	template <class _T>
	void _copy_pcm(const pcm<_T>& src, pcm<_T>& dst);

	inline void copy_pcm(const audio_mno8& src, audio_mno8& dst) { _copy_pcm<pcm8_m>(src, dst); }
	inline void copy_pcm(const audio_sto8& src, audio_sto8& dst) { _copy_pcm<pcm8_s>(src, dst); }
	inline void copy_pcm(const audio_mno16& src, audio_mno16& dst) { _copy_pcm<pcm16_m>(src, dst); }
	inline void copy_pcm(const audio_sto16& src, audio_sto16& dst) { _copy_pcm<pcm16_s>(src, dst); }
	inline void copy_pcm(const audio_mno32& src, audio_mno32& dst) { _copy_pcm<pcm32_m>(src, dst); }
	inline void copy_pcm(const audio_sto32& src, audio_sto32& dst) { _copy_pcm<pcm32_s>(src, dst); }


	//-----------------------------------------------------------------//
	/*!
		@brief	オーディオ・インターフェースの複製を作成する
		@param[in]	src	オーディオ・インターフェースのソース
		@return 複製されたオーディオ・インターフェース
	*/
	//-----------------------------------------------------------------//
	i_audio* copy_audio(const i_audio* src);

}
