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
#include "utils/i_scene.hpp"

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

		const float	abc_[12];

		struct chanel_t {
			float	frq_;
			float	angle_[8];

			al::sound::waves16 wav_;

			void render(uint32_t len, float gain)
			{
				static const float sqlvt[] = {
///					1.0f, 0.6f, 0.7f, 0.6f, 0.68f, 0.68f, 0.55f, 0.55f 
					1.0f, 0.72f, 0.55f, 0.49f, 0.73f, 0.52f, 0.2f, 0.15f 
				};

				wav_.resize(len);
				for(int i = 0; i < len; ++i) {
					float l = 0.0f;
					for(int j = 0; j < 8; ++j) {
						l += sinf(angle_[j]) * sqlvt[j];
						angle_[j] += frq_ * static_cast<float>(j + 1);
					}

					l *= gain;
					wav_[i] = static_cast<int16_t>(l * 10000.0f);
				}
			}
		};

		chanel_t	ch_[ch_num];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano() {
			// １２平均音階テーブルの作成
			float a = 27.5f;
			for(int i = 0; i < 12; ++i) {
				abc_[i] = a * 2.0f * 3.14159265f / static_cast<float>(sample_rate);
				a *= 1.059463094f;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			for(uint32_t n = 0; n < ch_num; ++n) {
				ch_[n].render();
			} 
		}

	};
}
