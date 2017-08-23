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
		@param[in]	sample_rate	サンプリング・レート
		@param[in]	ch_num		同時発音数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint32_t sample_rate, uint32_t ch_num>
	class piano {

		float	abc_[12];

		float	env_limit_;

		class channel_t {

			float			frq_;
			const float*	ovtn_gains_;
			float			angle_[8];

			float			envelope_;
			float			decay_;
		public:
			al::sound::waves16 wav_;

			channel_t() : frq_(0.0f), ovtn_gains_(nullptr), angle_{ 0.0f },
				envelope_(0.0f), decay_(0.965f) { }

			void set_overtone_gains(const float* gains) { ovtn_gains_ = gains; }

			void set_decay(float v) { decay_ = v; }

			void ring(float frq, float envelope) {
				frq_ = frq;
				for(int i = 0; i < 8; ++i) angle_[i] = 0.0f;
				envelope_ = envelope;
			}

			float get_envelope() const { return envelope_; }

			void render(uint32_t len, float gain, float limit)
			{
				wav_.resize(len);

				if(envelope_ < limit) {
					for(int i = 0; i < len; ++i) {
						wav_[i] = 0;
					}
					return;
				}

				for(int i = 0; i < len; ++i) {
					float l = 0.0f;
					for(int j = 0; j < 8; ++j) {
						l += sinf(angle_[j]) * ovtn_gains_[j];
						angle_[j] += frq_ * static_cast<float>(j + 1);
					}
					l /= 8.0f;
					l *= envelope_ * gain;
					wav_[i] = static_cast<int16_t>(l * 32767.0f);
				}
				envelope_ *= decay_;
			}
		};

		channel_t	ch_[ch_num];

		al::sound::waves16 wav_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano() : env_limit_(0.01f), ch_{ }, wav_() {
			// １２平均音階テーブルの作成
			float a = 27.5f;
			for(int i = 0; i < 12; ++i) {
				abc_[i] = a * 2.0f * 3.14159265f / static_cast<float>(sample_rate);
				a *= 1.059463094f;
			}

			static const float ovtn_gains[] = {
				1.0f, 0.72f, 0.55f, 0.49f, 0.73f, 0.52f, 0.2f, 0.15f 
			};
			for(int i = 0; i < 8; ++i) {
				ch_[i].set_overtone_gains(ovtn_gains);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  発音
			@param[in]	key	音階
			@param[in]	env	エンベロープ
			@return 発音したら「true」
		*/
		//-----------------------------------------------------------------//
		bool ring(uint32_t key, float env = 1.0f)
		{
			// 無音チャネルを探す
			for(uint32_t i = 0; i < ch_num; ++i) {
				if(ch_[i].get_envelope() > env_limit_) continue;
// utils::format("Key: %d\n") % key;
// utils::format("CH: %d\n") % i;
				float oct = (key / 12) + 1;
				ch_[i].ring(abc_[key % 12] * oct, env);
				return true;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
			@param[in]	len		波形バッファ長
			@param[in]	gain	全体ゲイン
		*/
		//-----------------------------------------------------------------//
		void service(uint32_t len, float gain)
		{
			for(uint32_t n = 0; n < ch_num; ++n) {
				ch_[n].render(len, gain, env_limit_);
			} 

			// 全チャネル合成
			wav_.resize(len);
			for(uint32_t i = 0; i < len; ++i) {
				int32_t sum = 0;
				int32_t num = 0;
				for(uint32_t n = 0; n < ch_num; ++n) {
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
