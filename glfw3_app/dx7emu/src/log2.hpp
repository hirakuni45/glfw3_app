#pragma once
/*
 * Copyright 2013 Google Inc.
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

namespace synth {

	template <class _>
	class Log2_ {

		#define LOG2_INLINE

		#ifdef LOG2_INLINE
		#define INLINE inline
		#else
		#define INLINE
		#endif

		static const uint32_t LOG2_LG_N_SAMPLES = 9;
		static const uint32_t LOG2_N_SAMPLES    = 1 << LOG2_LG_N_SAMPLES;

		static int32_t log2tab_[LOG2_N_SAMPLES << 1];

#ifdef __GNUC__
		static int clz(unsigned int x) {
			return __builtin_clz(x);
		}
		// TODO: other implementations (including ANSI C)
#endif

	public:
		Log2();

		static void init()
		{
			const double mul = 1 / log(2);
			for (int i = 0; i < LOG2_N_SAMPLES; i++) {
				double y = mul * log(i + (LOG2_N_SAMPLES)) + (7 - LOG2_LG_N_SAMPLES);
				log2tab_[(i << 1) + 1] = (int32_t)floor(y * (1 << 24) + 0.5);
			}
			for (int i = 0; i < LOG2_N_SAMPLES - 1; i++) {
				log2tab_[i << 1] = log2tab_[(i << 1) + 3] - log2tab_[(i << 1) + 1];
			}
			log2tab_[(LOG2_N_SAMPLES << 1) - 2] = (8 << 24) - log2tab_[(LOG2_N_SAMPLES << 1) - 1];
		}

		// Q24 in, Q24 out
		static INLINE int32_t lookup(uint32_t x)
		{
			int exp = clz(x | 1);
			unsigned int y = x << exp;

			const int SHIFT = 31 - LOG2_LG_N_SAMPLES;
			int lowbits = y & ((1 << SHIFT) - 1);
			int y_int = (y >> (SHIFT - 1)) & ((LOG2_N_SAMPLES - 1) << 1);
			int dz = log2tab[y_int];
			int z0 = log2tab[y_int + 1];

			int z = z0 + (((int64_t)dz * (int64_t)lowbits) >> SHIFT);
			return z - (exp << 24);
		}
	};
	typedef Log2_<void> Log2;

	template <class _> int32_t Log2_<_>::log2tab_[LOG2_N_SAMPLES << 1];
}
