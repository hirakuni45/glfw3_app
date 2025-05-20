#pragma once
//=========================================================================//
/*! @file
    @brief  トーン・クラス @n
			基本波形の精製、合成
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <cmath>
#include <cstdint>
#include "utils/vtx.hpp"
#include "utils/format.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  トーン・ベース・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class tone_base {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  波形型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class WAVE_TYPE {
			NONE,		///< none (stop generate wave)
			SIN_P,		///< sin wave (positive)
			SIN_N,		///< sin wave (negative)
			COS_P,		///< cos wave (positive)
			COS_N,		///< cos wave (negative)
			TRIANGLE_P,	///< triangle wave  (positive)
			TRIANGLE_N,	///< triangle wave  (negative)
			SAWTOOTH_P,	///< saw-tooth wave (positive)
			SAWTOOTH_N,	///< saw-tooth wave (negative)
			PLUS_P,		///< plus (positive)
			PLUS_N,		///< plus (negative)
		};


		enum class CHANNEL {
			MONO,	///< left, right channel
			LEFT,	///< left channel (stereo)
			RIGHT,	///< right channel (stereo)
		};
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  トーン・クラス
		@param[in]	SRT		サンプリング・レート
		@param[in]	SLOT	同時発音数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint32_t SRT, uint32_t SLOT>
	class tone : public tone_base {

		class slot_t {

			WAVE_TYPE	type_;

			double		frq_;
			double		angle_;
			double		step_;
			double		limit_;
		public:
			std::vector<float>	wav_;

			slot_t() :
				type_(WAVE_TYPE::NONE),
				frq_(0.0),
				angle_(0.0f),
				step_(0.0f), limit_(0.0)
			{ }

			void stop() noexcept
			{
				angle_ = 0.0;
				step_ = 0.0;
				type_ = WAVE_TYPE::NONE;
			}

			void ring(WAVE_TYPE type, double frq) noexcept
			{
				type_ = type;
				frq_ = frq;
				angle_ = 0.0;
				step_ = frq / static_cast<double>(SRT);
				if(type_ == WAVE_TYPE::SIN_P || type_ == WAVE_TYPE::COS_P || type_ == WAVE_TYPE::SIN_N || type_ == WAVE_TYPE::COS_N) {
					step_ *= vtx::radian_d_;
					limit_ = vtx::radian_d_;
				} else {
					limit_ = 1.0;
				}
			}

			auto get_type() const noexcept { return type_; }

			void render(uint32_t len) noexcept
			{
				if(len == 0 || step_ == 0.0 || type_ == WAVE_TYPE::NONE) return;

				wav_.resize(len);

				for(int i = 0; i < len; ++i) {
					float a = 0.0f;
					switch(type_) {
					case WAVE_TYPE::SIN_P:
						a = std::sin(angle_);
						break;
					case WAVE_TYPE::SIN_N:
						a = -std::sin(angle_);
						break;
					case WAVE_TYPE::COS_P:
						a = std::cos(angle_);
						break;
					case WAVE_TYPE::COS_N:
						a = -std::cos(angle_);
						break;
					case WAVE_TYPE::TRIANGLE_P:
					case WAVE_TYPE::TRIANGLE_N:
						if(angle_ <= 0.25) {  // 0 to 1.0
							a = angle_ * 4.0;
						} else if(angle_ <= 0.75) {  // 1 to -1
							a = (angle_ - 0.25) * -4.0 + 1.0;
						} else {  // -1 to 0
							a = (angle_ - 0.75) * 4.0 - 1.0;
						}
						if(type_ == WAVE_TYPE::TRIANGLE_N) {
							a *= -1.0;
						}
						break;
					case WAVE_TYPE::SAWTOOTH_P:
						a = angle_ * 2.0 - 1.0;
						break;
					case WAVE_TYPE::SAWTOOTH_N:
						a = (1.0 - angle_) * 2.0 - 1.0;
						break;
					case WAVE_TYPE::PLUS_P:
						if(angle_ <= 0.5) {
							a =  1.0f;
						} else {
							a = -1.0f;
						}
						break;
					case WAVE_TYPE::PLUS_N:
						if(angle_ <= 0.5) {
							a = -1.0f;
						} else {
							a =  1.0f;
						}
						break;
					default:
						break;
					}
					angle_ += step_;
					if(angle_ >= limit_) { angle_ -= limit_; }
					wav_[i] = a;
				}
			}
		};

		CHANNEL		ch_[SLOT];
		slot_t		slot_[SLOT];
		float		vol_[SLOT];

		al::audio	audio_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		tone() : ch_{ CHANNEL::MONO }, slot_{ }, vol_{ 1.0f }, audio_()
		{
			audio_ = al::audio(new al::audio_sto16);
			audio_->create(SRT, 1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  発音
			@param[in]	slot	スロット・インデックス
			@param[in]	type	波形タイプ
			@param[in]	ch		チャネル
			@param[in]	freq	周波数
			@return 発音したら「true」
		*/
		//-----------------------------------------------------------------//
		bool ring(uint32_t slot, WAVE_TYPE type, CHANNEL ch, double freq) noexcept
		{
			if(slot >= SLOT) return false;

			ch_[slot] = ch;
			slot_[slot].ring(type, freq);

			return true;
		}

#if 0
		//-----------------------------------------------------------------//
		/*!
			@brief  
			@param[in]	slot	スロット・インデックス
		*/
		//-----------------------------------------------------------------//
		void set_volume(uint32_t slot, float vol) noexcept
		{
			if(slot >= SLOT) return;

			vol_[slot] = vol;
		}
#endif

		//-----------------------------------------------------------------//
		/*!
			@brief  ボリュームの設定
			@param[in]	slot	スロット・インデックス
		*/
		//-----------------------------------------------------------------//
		void set_volume(uint32_t slot, float vol) noexcept
		{
			if(slot >= SLOT) return;

			vol_[slot] = vol;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  発音停止
		*/
		//-----------------------------------------------------------------//
		void stop(uint32_t slot) noexcept
		{
			if(slot >= SLOT) return;

			slot_[slot].stop();
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
				slot_[n].render(wlen);
			} 

			audio_->resize(wlen);

			for(uint32_t i = 0; i < wlen; ++i) {
				int32_t left = 0;
				int32_t right = 0;
				int32_t left_n = 0;
				int32_t right_n = 0;
				for(uint32_t n = 0; n < SLOT; ++n) {
					if(slot_[n].get_type() == WAVE_TYPE::NONE) continue;
					auto a = static_cast<int32_t>(slot_[n].wav_[i] * vol_[n] * 32767.0f);
					switch(ch_[n]) {
					case CHANNEL::MONO:
						left += a;
						++left_n;
						right += a;
						++right_n;
						break;
					case CHANNEL::LEFT:
						left += a;
						++left_n;
						break;
					case CHANNEL::RIGHT:
						right += a;
						++right_n;
						break;
					}
				}
				if(left_n > 1) {
					left /= left_n;
				}
				if(right_n > 1) {
					right /= right_n;
				}
				if(left > 32767) left = 32767;
				else if(left < -32768) left = -32768;
				if(right > 32767) right = 32767;
				else if(right < -32768) right = -32768;

				al::pcm16_s w(left, right);
				audio_->put(i, w);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  オーディオの参照
			@return オーディオ
		*/
		//-----------------------------------------------------------------//
		auto& get_audio() { return audio_; }
	};
}
