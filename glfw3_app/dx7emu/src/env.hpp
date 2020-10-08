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

#include "synth.h"

// DX7 envelope generation

namespace synth {

	class Env {

		int rates_[4];
		int levels_[4];
		int outlevel_;
		int rate_scaling_;

		// Level is stored so that 2^24 is one doubling, ie 16 more bits than
		// the DX7 itself (fraction is stored in level rather than separate
		// counter)
		int32_t level_;
		int targetlevel_;
		bool rising_;
		int ix_;
		int inc_;

		bool down_;

		void advance_(int newix)
		{
			ix_ = newix;
			if (ix_ < 4) {
				int newlevel = levels_[ix_];
				int actuallevel = scaleoutlevel(newlevel) >> 1;
				actuallevel = (actuallevel << 6) + outlevel_ - 4256;
				actuallevel = actuallevel < 16 ? 16 : actuallevel;
				// level here is same as Java impl
				targetlevel_ = actuallevel << 16;
				rising_ = (targetlevel_ > level_);

				// rate
				int qrate = (rates_[ix_] * 41) >> 6;
				qrate += rate_scaling_;
				qrate = min(qrate, 63);
				inc_ = (4 + (qrate & 3)) << (2 + SYNTH_LG_N + (qrate >> 2));
			}
		}

		static constexpr int levellut_[] = {
			0, 5, 9, 13, 17, 20, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 42, 43, 45, 46
		};

	public:
		// The rates and levels arrays are calibrated to match the Dx7 parameters
		// (ie, value 0..99). The outlevel parameter is calibrated in microsteps
		// (ie units of approx .023 dB), with 99 * 32 = nominal full scale. The
		// rate_scaling parameter is in qRate units (ie 0..63).
		void init(const int r[4], const int l[4], int ol, int rate_scaling)
		{
			for (int i = 0; i < 4; i++) {
				rates_[i] = r[i];
				levels_[i] = l[i];
			}
			outlevel_ = ol;
			rate_scaling_ = rate_scaling;
			level_ = 0;
			down_ = true;
			advance_(0);
		}


		// Result is in Q24/doubling log format. Also, result is subsampled
		// for every N samples.
		// A couple more things need to happen for this to be used as a gain
		// value. First, the # of outputs scaling needs to be applied. Also,
		// modulation.
		// Then, of course, log to linear.
		int32_t getsample()
		{
			if (ix_ < 3 || ((ix_ < 4) && !down_)) {
				if (rising_) {
					const int jumptarget = 1716;
					if (level_ < (jumptarget << 16)) {
						level_ = jumptarget << 16;
					}
					level_ += (((17 << 24) - level_) >> 24) * inc_;
					// TODO: should probably be more accurate when inc is large
					if (level_ >= targetlevel_) {
						level_ = targetlevel_;
						advance_(ix_ + 1);
					}
				} else {  // !rising
					level_ -= inc_;
					if (level_ <= targetlevel_) {
						level_ = targetlevel_;
						advance_(ix_ + 1);
					}
				}
			}
			// TODO: this would be a good place to set level to 0 when under threshold
			return level_;
		}


		void keydown(bool down)
		{
			if (down_ != down) {
				down_ = down;
				advance_(down ? 0 : 3);
			}
		}


 		void setparam(int param, int value)
		{
			if (param < 4) {
				rates_[param] = value;
			} else if (param < 8) {
				levels_[param - 4] = value;
			}
			// Unknown parameter, ignore for now
		}

		static int scaleoutlevel(int outlevel)
		{
			return outlevel >= 20 ? 28 + outlevel : levellut_[outlevel];
		}
	};
}

