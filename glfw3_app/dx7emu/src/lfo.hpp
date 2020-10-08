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

// Low frequency oscillator, compatible with DX7

#include "synth.h"
#include "sin.hpp"

namespace synth {

	class Lfo {

		uint32_t	unit_;

		uint32_t	phase_;  // Q32
		uint32_t	delta_;
		uint8_t		waveform_;
		uint8_t		randstate_;
		bool		sync_;

		uint32_t	delaystate_;
		uint32_t	delayinc_;
		uint32_t	delayinc2_;

	public:
		Lfo() : unit_(0),
			phase_(0), delta_(0), waveform_(0), randstate_(0), sync_(false),
			delaystate_(0), delayinc_(0), delayinc2_(0)
		{ }


		void init(double sample_rate)
		{
			// constant is 1 << 32 / 15.5s / 11
			unit_ = (int32_t)(SYNTH_N * 25190424 / sample_rate + 0.5);
		}


		void reset(const char params[6])
		{
  int rate = params[0];  // 0..99
  int sr = rate == 0 ? 1 : (165 * rate) >> 6;
  sr *= sr < 160 ? 11 : (11 + ((sr - 160) >> 4));
  delta_ = unit_ * sr;
  int a = 99 - params[1];  // LFO delay
  if (a == 99) {
    delayinc_ = ~0u;
    delayinc2_ = ~0u;
  } else {
  a = (16 + (a & 15)) << (1 + (a >> 4));
    delayinc_ = unit_ * a;
    a &= 0xff80;
    a = max(0x80, a);
    delayinc2_ = unit_ * a;
  }
  waveform_ = params[5];
  sync_ = params[4] != 0;
		}

		// result is 0..1 in Q24
		int32_t getsample()
		{
  phase_ += delta_;
  int32_t x;
  switch (waveform_) {
  case 0:  // triangle
    x = phase_ >> 7;
    x ^= -(phase_ >> 31);
    x &= (1 << 24) - 1;
    return x;
  case 1:  // sawtooth down
    return (~phase_ ^ (1U << 31)) >> 8;
  case 2:  // sawtooth up
    return (phase_ ^ (1U << 31)) >> 8;
  case 3:  // square
    return ((~phase_) >> 7) & (1 << 24);
  case 4:  // sine
    return (1 << 23) + (Sin::lookup(phase_ >> 8) >> 1);
  case 5:  // s&h
    if (phase_ < delta_) {
      randstate_ = (randstate_ * 179 + 17) & 0xff;
    }
    x = randstate_ ^ 0x80;
    return (x + 1) << 16;
  }
  return 1 << 23;
		}


		// result is 0..1 in Q24
		int32_t getdelay()
		{
  uint32_t delta = delaystate_ < (1U << 31) ? delayinc_ : delayinc2_;
  uint32_t d = delaystate_ + delta;
  if (d < delayinc_) {
    return 1 << 24;
  }
  delaystate_ = d;
  if (d < (1U << 31)) {
    return 0;
  } else {
    return (d >> 7) & ((1 << 24) - 1);
  }
		}


		void keydown()
		{
  if (sync_) {
    phase_ = (1U << 31) - 1;
  }
  delaystate_ = 0;
		}

	};
}
