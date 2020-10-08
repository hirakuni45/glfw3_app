#pragma once
/*
 * Copyright 2012 Google Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cmath>

#include "synth.h"

namespace synth {

	#define EXP2_INLINE

	template <class _>
	class Exp2_ {

		#ifdef EXP2_INLINE
		#define INLINE inline
		#else
		#define INLINE
		#endif

		static const uint32_t EXP2_LG_N_SAMPLES = 10;
		static const uint32_t EXP2_N_SAMPLES    = 1 << EXP2_LG_N_SAMPLES;

		static int32_t exp2tab_[EXP2_N_SAMPLES << 1];

	public:

		static void init()
		{
			double inc = exp2(1.0 / EXP2_N_SAMPLES);
			double y = 1 << 30;
			for (int i = 0; i < EXP2_N_SAMPLES; i++) {
				exp2tab_[(i << 1) + 1] = (int32_t)floor(y + 0.5);
				y *= inc;
			}
			for (int i = 0; i < EXP2_N_SAMPLES - 1; i++) {
				exp2tab_[i << 1] = exp2tab_[(i << 1) + 3] - exp2tab_[(i << 1) + 1];
			}
			exp2tab_[(EXP2_N_SAMPLES << 1) - 2] = (1U << 31) - exp2tab_[(EXP2_N_SAMPLES << 1) - 1];
		}


		// Q24 in, Q24 out
		static INLINE int32_t lookup(int32_t x)
		{
			const int SHIFT = 24 - EXP2_LG_N_SAMPLES;
			int lowbits = x & ((1 << SHIFT) - 1);
			int x_int = (x >> (SHIFT - 1)) & ((EXP2_N_SAMPLES - 1) << 1);
			int dy = exp2tab_[x_int];
			int y0 = exp2tab_[x_int + 1];

			int y = y0 + (((int64_t)dy * (int64_t)lowbits) >> SHIFT);
			return y >> (6 - (x >> 24));
		}
	};
	typedef Exp2_<void> Exp2;
	template <class _> int32_t Exp2_<_>::exp2tab_[Exp2_<_>::EXP2_N_SAMPLES << 1];


	template <class _>
	class Tanh_ {

		static const uint32_t TANH_LG_N_SAMPLES = 10;
		static const uint32_t TANH_N_SAMPLES    = 1 << TANH_LG_N_SAMPLES;

		static int32_t tanhtab_[TANH_N_SAMPLES << 1];

		static double dtanh(double y) {
			return 1 - y * y;
		}

	public:
		static void init()
		{
			double step = 4.0 / TANH_N_SAMPLES;
			double y = 0;
			for (int i = 0; i < TANH_N_SAMPLES; i++) {
				tanhtab_[(i << 1) + 1] = (1 << 24) * y + 0.5;
				// printf("%d\n", tanhtab[(i << 1) + 1]);
				// Use a basic 4th order Runge-Kutte to compute tanh from its
				// differential equation.
				double k1 = dtanh(y);
				double k2 = dtanh(y + 0.5 * step * k1);
				double k3 = dtanh(y + 0.5 * step * k2);
				double k4 = dtanh(y + step * k3);
				double dy = (step / 6) * (k1 + k4 + 2 * (k2 + k3));
				y += dy;
			}
			for (int i = 0; i < TANH_N_SAMPLES - 1; i++) {
				tanhtab_[i << 1] = tanhtab_[(i << 1) + 3] - tanhtab_[(i << 1) + 1];
			}
			int32_t lasty = (1 << 24) * y + 0.5;
			tanhtab_[(TANH_N_SAMPLES << 1) - 2] = lasty - tanhtab_[(TANH_N_SAMPLES << 1) - 1];
		}


		// Q24 in, Q24 out
		static int32_t lookup(int32_t x)
		{
			int32_t signum = x >> 31;
			x ^= signum;
			if (x >= (4 << 24)) {
				if (x >= (17 << 23)) {
					return signum ^ (1 << 24);
				}
				int32_t sx = ((int64_t)-48408812 * (int64_t)x) >> 24;
				return signum ^ ((1 << 24) - 2 * Exp2::lookup(sx));
			} else {
				const int SHIFT = 26 - TANH_LG_N_SAMPLES;
				int lowbits = x & ((1 << SHIFT) - 1);
				int x_int = (x >> (SHIFT - 1)) & ((TANH_N_SAMPLES - 1) << 1);
				int dy = tanhtab_[x_int];
				int y0 = tanhtab_[x_int + 1];
				int y = y0 + (((int64_t)dy * (int64_t)lowbits) >> SHIFT);
				return y ^ signum;
			}
		}
	};
	typedef Tanh_<void> Tanh;
	template<class _> int32_t Tanh_<_>::tanhtab_[Tanh_<_>::TANH_N_SAMPLES << 1];
}
