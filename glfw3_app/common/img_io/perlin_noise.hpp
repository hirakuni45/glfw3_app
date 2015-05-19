#pragma once
//=====================================================================//
/*!	@file
	@brief	パーリンノイズ生成
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cmath>
#include <random>
#include <algorithm>
#include "img_io/img_gray8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	パーリンノイズ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename REAL>
	class perlin_noise {

		uint32_t p_[512];

		REAL fade_(REAL t) const {
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		REAL lerp_(REAL t, REAL a, REAL b) const {
			return a + t * (b - a);
		}

		REAL grad_(uint32_t hash, REAL x, REAL y, REAL z) const {
			uint32_t h = hash & 15;
			REAL u = h < 8 ? x : y, v = h < 4 ? y : h==12 || h==14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

		REAL clamp_(REAL x, REAL min, REAL max) {
			return (x < min) ? min : ((max < x) ? max : x);
		}

		float floor_(float a) const { return floorf(a); }
		double floor_(double a) const { return floor(a); }

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		explicit perlin_noise(uint32_t seed = 1) {
			if(seed == 0) {
				seed = std::mt19937::default_seed;
			}

			std::iota(std::begin(p_), std::begin(p_) + 256, 0);
			std::shuffle(std::begin(p_), std::begin(p_) + 256, std::mt19937(seed));

			for(uint32_t i = 0; i < 256; ++i) {
				p_[256 + i] = p_[i];
			}
		}


		REAL noise(REAL x) const {
			return noise(x, 0, 0);
		}


		REAL noise(REAL x, REAL y) const {
			return noise(x, y, 0);
		}


		REAL noise(REAL x, REAL y, REAL z) const {
			int X = static_cast<int>(floor_(x)) & 255;
			int Y = static_cast<int>(floor_(y)) & 255;
			int Z = static_cast<int>(floor_(z)) & 255;

			x -= floor_(x);
			y -= floor_(y);
			z -= floor_(z);

			REAL u = fade_(x);
			REAL v = fade_(y);
			REAL w = fade_(z);

			uint32_t A = p_[X  ]+Y, AA = p_[A]+Z, AB = p_[A+1]+Z;
			uint32_t B = p_[X+1]+Y, BA = p_[B]+Z, BB = p_[B+1]+Z;

			return lerp_(w, lerp_(v, lerp_(u, grad_(p_[AA  ], x  , y  , z   ),
										grad_(p_[BA  ], x-1, y  , z   )),
								lerp_(u, grad_(p_[AB  ], x  , y-1, z   ),
									 grad_(p_[BB  ], x-1, y-1, z   ))),
						lerp_(v, lerp_(u, grad_(p_[AA+1], x  , y  , z-1 ),
									 grad_(p_[BA+1], x-1, y  , z-1 )),
							 lerp_(u, grad_(p_[AB+1], x  , y-1, z-1 ),
								  grad_(p_[BB+1], x-1, y-1, z-1 ))));
		}


		REAL octave_noise(REAL x, uint32_t octaves) const {
			REAL result = static_cast<REAL>(0);
			REAL amp = static_cast<REAL>(1);

			for(uint32_t i = 0; i < octaves; ++i) {
				result += noise(x) * amp;
				x   *= static_cast<REAL>(2);
				amp /= static_cast<REAL>(2);
			}
			return result;
		}


		REAL octave_noise(REAL x, REAL y, uint32_t octaves) const {
			REAL result = static_cast<REAL>(0);
			REAL amp = static_cast<REAL>(1);

			for(uint32_t i = 0; i < octaves; ++i) {
				result += noise(x, y) * amp;
				x   *= static_cast<REAL>(2);
				y   *= static_cast<REAL>(2);
				amp /= static_cast<REAL>(2);
			}
			return result;
		}


		REAL octave_noise(REAL x, REAL y, REAL z, int octaves) const {
			REAL result = static_cast<REAL>(0);
			REAL amp = static_cast<REAL>(1);

			for(uint32_t i = 0; i < octaves; ++i) {
				result += noise(x, y, z) * amp;
				x   *= static_cast<REAL>(2);
				y   *= static_cast<REAL>(2);
				z   *= static_cast<REAL>(2);
				amp /= static_cast<REAL>(2);
			}
			return result;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	パーリンノイズでグレースケール画像の作成
			@param[out]	dst	リサイズイメージ
			@param[in]	octave	オクターブパラメーター
			@param[in]	frequemncy	周波数パラメーター
		*/
		//-----------------------------------------------------------------//
		void create_perlin_noize(img_gray8& dst, REAL octave, REAL frequency) {
			REAL fx = static_cast<REAL>(dst.get_size().x) / frequency;
			REAL fy = static_cast<REAL>(dst.get_size().y) / frequency;
			REAL fh = 1 / static_cast<REAL>(2);

			vtx::spos pos;
			for(pos.y = 0; pos.y < dst.get_size().y; ++pos.y) {
				for(pos.x = 0; pos.x < dst.get_size().x; ++pos.x) {
					REAL n = octave_noise(static_cast<REAL>(pos.x) / fx,
										  static_cast<REAL>(pos.y) / fy, octave);
					n = clamp_(n * fh + fh, 0.0, 1.0);
					gray8 g(static_cast<uint8_t>(n * static_cast<REAL>(255)));
					dst.put_pixel(pos, g);
				}
			}
		}
	};
}
