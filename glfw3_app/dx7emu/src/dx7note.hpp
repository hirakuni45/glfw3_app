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

// This is the logic to put together a note from the MIDI description
// and run the low-level modules.

// It will continue to evolve a bit, as note-stealing logic, scaling,
// and real-time control of parameters live here.
#ifdef VERBOSE
#include <iostream>
#endif
#include <cmath>
#include "synth.h"
#include "freqlut.hpp"
#include "exp2.hpp"
#include "controllers.h"
#include "env.hpp"
#include "pitchenv.hpp"
#include "fm_core.h"

namespace synth {

	class Dx7Note {

		FmCore core_;
		Env env_[6];
		FmOpParams params_[6];
		PitchEnv pitchenv_;
		int32_t basepitch_[6];
		int32_t fb_buf_[2];
		int32_t fb_shift_;

		int algorithm_;
		int pitchmoddepth_;
		int pitchmodsens_;


		static constexpr uint8_t exp_scale_data[] = {
		  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 14, 16, 19, 23, 27, 33, 39, 47, 56, 66,
		  80, 94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 250
		};


		static constexpr uint8_t pitchmodsenstab[] = {
			0, 10, 20, 33, 55, 92, 153, 255
		};


		static constexpr uint8_t velocity_data[64] = {
			0, 70, 86, 97, 106, 114, 121, 126, 132, 138, 142, 148, 152, 156, 160, 163,
			166, 170, 173, 174, 178, 181, 184, 186, 189, 190, 194, 196, 198, 200, 202,
			205, 206, 209, 211, 214, 216, 218, 220, 222, 224, 225, 227, 229, 230, 232,
			233, 235, 237, 238, 240, 241, 242, 243, 244, 246, 246, 248, 249, 250, 251,
			252, 253, 254
		};


		static constexpr int32_t coarsemul[] = {
			-16777216, 0, 16777216, 26591258, 33554432, 38955489, 43368474, 47099600,
			50331648, 53182516, 55732705, 58039632, 60145690, 62083076, 63876816,
			65546747, 67108864, 68576247, 69959732, 71268397, 72509921, 73690858,
			74816848, 75892776, 76922906, 77910978, 78860292, 79773775, 80654032,
			81503396, 82323963, 83117622
		};


		int32_t midinote_to_logfreq(int midinote) {
			const int base = 50857777;  // (1 << 24) * (log(440) / log(2) - 69/12)
			const int step = (1 << 24) / 12;
			return base + step * midinote;
		}


		int32_t osc_freq(int midinote, int mode, int coarse, int fine, int detune) {
			// TODO: pitch randomization
			int32_t logfreq;
			if (mode == 0) {
				logfreq = midinote_to_logfreq(midinote);
				logfreq += coarsemul[coarse & 31];
				if (fine) {
					// (1 << 24) / log(2)
					logfreq += (int32_t)floor(24204406.323123 * log(1 + 0.01 * fine) + 0.5);
				}
				// This was measured at 7.213Hz per count at 9600Hz, but the exact
				// value is somewhat dependent on midinote. Close enough for now.
				logfreq += 12606 * (detune - 7);
			} else {
				// ((1 << 24) * log(10) / log(2) * .01) << 3
				logfreq = (4458616 * ((coarse & 3) * 100 + fine)) >> 3;
				logfreq += detune > 7 ? 13457 * (detune - 7) : 0;
			}
			return logfreq;
		}


		// See "velocity" section of notes. Returns velocity delta in microsteps.
		int ScaleVelocity(int velocity, int sensitivity) {
			int clamped_vel = max(0, min(127, velocity));
			int vel_value = velocity_data[clamped_vel >> 1] - 239;
			int scaled_vel = ((sensitivity * vel_value + 7) >> 3) << 4;
			return scaled_vel;
		}


		int ScaleRate(int midinote, int sensitivity) {
			int x = min(31, max(0, midinote / 3 - 7));
			int qratedelta = (sensitivity * x) >> 3;
#ifdef SUPER_PRECISE
			int rem = x & 7;
			if (sensitivity == 3 && rem == 3) {
				qratedelta -= 1;
			} else if (sensitivity == 7 && rem > 0 && rem < 4) {
				qratedelta += 1;
			}
#endif
			return qratedelta;
		}


		int ScaleCurve(int group, int depth, int curve) {
			int scale;
			if (curve == 0 || curve == 3) {
				// linear
				scale = (group * depth * 329) >> 12;
			} else {
				// exponential
				int n_scale_data = sizeof(exp_scale_data);
				int raw_exp = exp_scale_data[min(group, n_scale_data - 1)];
				scale = (raw_exp * depth * 329) >> 15;
			}
			if (curve < 2) {
				scale = -scale;
			}
			return scale;
		}


		int ScaleLevel(int midinote, int break_pt, int left_depth, int right_depth,
			int left_curve, int right_curve)
		{
			int offset = midinote - break_pt - 17;
			if (offset >= 0) {
				return ScaleCurve(offset / 3, right_depth, right_curve);
			} else {
				return ScaleCurve((-offset) / 3, left_depth, left_curve);
			}
		}

	public:
		Dx7Note() { }

		void init(const char patch[128], int midinote, int velocity)
		{
			int rates[4];
			int levels[4];
			for (int op = 0; op < 6; op++) {
				int off = op * 21;
				for (int i = 0; i < 4; i++) {
					rates[i] = patch[off + i];
					levels[i] = patch[off + 4 + i];
				}
				int outlevel = patch[off + 16];
				outlevel = Env::scaleoutlevel(outlevel);
#ifdef VERBOSE
				for (int j = 8; j < 12; j++) {
					cout << (int)patch[off + j] << " ";
				}
#endif
				int level_scaling = ScaleLevel(midinote, patch[off + 8], patch[off + 9],
					patch[off + 10], patch[off + 11], patch[off + 12]);
				outlevel += level_scaling;
				outlevel = min(127, outlevel);
#ifdef VERBOSE
				cout << op << ": " << level_scaling << " " << outlevel << endl;
#endif
				outlevel = outlevel << 5;
				outlevel += ScaleVelocity(velocity, patch[off + 15]);
				outlevel = max(0, outlevel);
				int rate_scaling = ScaleRate(midinote, patch[off + 13]);
				env_[op].init(rates, levels, outlevel, rate_scaling);

				int mode = patch[off + 17];
				int coarse = patch[off + 18];
				int fine = patch[off + 19];
				int detune = patch[off + 20];
				int32_t freq = osc_freq(midinote, mode, coarse, fine, detune);
				basepitch_[op] = freq;
				// cout << op << " freq: " << freq << endl;
				params_[op].phase = 0;
				params_[op].gain[1] = 0;
			}
			for (int i = 0; i < 4; i++) {
				rates[i] = patch[126 + i];
				levels[i] = patch[130 + i];
			}
			pitchenv_.set(rates, levels);
			algorithm_ = patch[134];
			int feedback = patch[135];
			fb_shift_ = feedback != 0 ? 8 - feedback : 16;
			pitchmoddepth_ = (patch[139] * 165) >> 6;
			pitchmodsens_ = pitchmodsenstab[patch[143] & 7];
		}

		// Note: this _adds_ to the buffer. Interesting question whether it's
		// worth it...
		void compute(int32_t *buf, int32_t lfo_val, int32_t lfo_delay, const Controllers* ctrls)
		{
			int32_t pitchmod = pitchenv_.getsample();
			uint32_t pmd = pitchmoddepth_ * lfo_delay;  // Q32
			// TODO: add modulation sources (mod wheel, etc)
			int32_t senslfo = pitchmodsens_ * (lfo_val - (1 << 23));
			pitchmod += (((int64_t)pmd) * (int64_t)senslfo) >> 39;

			// hardcodes a pitchbend range of 3 semitones, TODO make configurable
			int pitchbend = ctrls->values_[kControllerPitch];
			int32_t pb = (pitchbend - 0x2000) << 9;
			pitchmod += pb;
			for (int op = 0; op < 6; op++) {
				params_[op].gain[0] = params_[op].gain[1];
				int32_t level = env_[op].getsample();
				int32_t gain = Exp2::lookup(level - (14 * (1 << 24)));
				// int32_t gain = pow(2, 10 + level * (1.0 / (1 << 24)));
				params_[op].freq = synth::Freqlut::lookup(basepitch_[op] + pitchmod);
				params_[op].gain[1] = gain;
			}
			core_.compute(buf, params_, algorithm_, fb_buf_, fb_shift_);
		}


		void keyup()
		{
			for (int op = 0; op < 6; op++) {
				env_[op].keydown(false);
				pitchenv_.keydown(false);
			}
		}

		// TODO: parameter changes

		// TODO: some way of indicating end-of-note. Maybe should be a return
		// value from the compute method? (Having a count return from keyup
		// is also tempting, but if there's a dynamic parameter change after
		// keyup, that won't work.
	};
}

