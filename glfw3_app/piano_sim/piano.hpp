#pragma once
//=====================================================================//
/*! @file
    @brief  ピアノ・クラス @n
			※少ない手順で、ピアノぽぃ音を生成するクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <cmath>
#include <cstdint>
#include "utils/format.hpp"


namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  音楽クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class music {


	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  １２平均音階のキー @n
					(0)27.5, (1)55, (2)110, (3)220, (4)440, (5)880, (6)1760
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class key {
			A,		///< A  ラ
			As,		///< A#
			B,		///< B  シ
			C,		///< C  ド
			Cs,		///< C#
			D,		///< D  レ
			Ds,		///< D#
			E,		///< E  ミ
			F,		///< F  ファ
			Fs,		///< F#
			G,		///< G  ソ
			Gs		///< G#
		};

	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ピアノ・クラス
		@param[in]	SRATE	サンプリング・レート
		@param[in]	SLOT	同時発音数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint32_t SRATE, uint32_t SLOT>
	class piano {

		double	abc_[12];

		float	env_limit_;

		class channel_t {

			double			frq_;
			double			angle_[8];

			float			envelope_;
			float			decay_;
		public:
			al::sound::waves16 wav_;

			channel_t() : frq_(0.0), angle_{ 0.0f },
				envelope_(0.0f), decay_(0.965f) { }

			void set_decay(float v) { decay_ = v; }

			void ring(double frq, float envelope) {
				frq_ = frq;
				for(int i = 0; i < 8; ++i) angle_[i] = 0.0;
				envelope_ = envelope;
			}

			float get_envelope() const { return envelope_; }

			void render(uint32_t len, float gain, float limit, const double* ovtn)
			{
				wav_.resize(len);

				if(envelope_ < limit) {
					for(int i = 0; i < len; ++i) {
						wav_[i] = 0;
					}
					return;
				}

				for(int i = 0; i < len; ++i) {
					double l = 0.0;
					int n = 1;
					for(int j = 0; j < n; ++j) {
						l += std::sin(angle_[j]) * ovtn[j];
						angle_[j] += frq_ * static_cast<double>(j + 1);
					}
					l *= 1.0f / static_cast<float>(n);
					l *= envelope_ * gain;
					wav_[i] = static_cast<int16_t>(l * 32767.0f);
				}
				envelope_ *= decay_;
			}
		};

		channel_t	ch_[SLOT];

		al::sound::waves16 wav_;

		const double* get_overtone_() const {
			static const double ovtn[] = {
				1.0f, 0.72f, 0.55f, 0.49f, 0.73f, 0.52f, 0.2f, 0.15f 
			};
			return ovtn;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano() : env_limit_(1.0f / 200.0f), ch_{ }, wav_() {
			// １２平均音階テーブルの作成
			double a = 27.5 * 2.0 * 3.141592653589793 / static_cast<double>(SRATE);
			for(int i = 0; i < 12; ++i) {
				abc_[i] = a;
				a *= pow(2.0, 1.0 / 12.0);  // １２乗すると２になる定数
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  発音
			@param[in]	key	音階
			@return 発音したら「true」
		*/
		//-----------------------------------------------------------------//
		bool ring(uint32_t key)
		{
			// 無音チャネルを探す
			for(uint32_t i = 0; i < SLOT; ++i) {
				if(ch_[i].get_envelope() > env_limit_) continue;
				double oct = static_cast<double>(1 << (key / 12));
				uint32_t idx = key % 12;
				double k = abc_[idx] * oct;
				ch_[i].ring(k, 1.0f);
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
			@param[in]	wlen	波形バッファ長
			@param[in]	gain	全体ゲイン
		*/
		//-----------------------------------------------------------------//
		void service(uint32_t wlen, float gain)
		{
			for(uint32_t n = 0; n < SLOT; ++n) {
				ch_[n].render(wlen, gain, env_limit_, get_overtone_());
			} 

			// 全チャネル合成
			wav_.resize(wlen);
			for(uint32_t i = 0; i < wlen; ++i) {
				int32_t sum = 0;
				int32_t num = 0;
				for(uint32_t n = 0; n < SLOT; ++n) {
					if(ch_[n].get_envelope() > env_limit_) {
						sum += static_cast<int32_t>(ch_[n].wav_[i]);
						++num;
					}
				}
				if(num > 1) {
					sum /= num;
				}
				if(sum > 32767) sum = 32767;
				else if(sum < -32768) sum = -32768;
				wav_[i] = sum;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形参照
			@return 波形
		*/
		//-----------------------------------------------------------------//
		const al::sound::waves16& get_wav() const { return wav_; }
	};
}
