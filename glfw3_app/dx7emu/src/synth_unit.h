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
#include "controllers.h"
#include "dx7note.h"
#include "lfo.h"
#include "ringbuffer.h"
#include "resofilter.h"

class SynthUnit {

	struct ActiveNote {
		int		midi_note;
		bool	keydown;
		bool	sustained;
		bool	live;
		Dx7Note	dx7_note;
		ActiveNote() :
			midi_note(0),
			keydown(false), sustained(false), live(false),
			dx7_note() { }
	};

	// initial FM param for program 0
	static constexpr uint8_t epiano_[] = {
		95, 29, 20, 50, 99, 95,  0,  0, 41, 0, 19,  0, 115, 24, 79,  2, 0,
		95, 20, 20, 50, 99, 95,  0,  0,  0, 0,  0,  0,   3,  0, 99,  2, 0,
		95, 29, 20, 50, 99, 95,  0,  0,  0, 0,  0,  0,  59, 24, 89,  2, 0,
		95, 20, 20, 50, 99, 95,  0,  0,  0, 0,  0,  0,  59,  8, 99,  2, 0,
		95, 50, 35, 78, 99, 75,  0,  0,  0, 0,  0,  0,  59, 28, 58, 28, 0,
		96, 25, 25, 67, 99, 75,  0,  0,  0, 0,  0,  0,  83,  8, 99,  2, 0,
 
		94, 67, 95, 60, 50, 50, 50, 50,  4, 6, 34, 33,   0,  0, 56, 24,
		69, 46, 80, 73, 65, 78, 79, 32, 49, 32
	};

#ifdef WIN32
	static const int max_active_notes = 16;
#else
#if defined(SIG_RX65N)
	static const int max_active_notes = 8;
#elif defined(SIG_RX72N)
	static const int max_active_notes = 16;
#endif
#endif

	RingBuffer& ring_buffer_;

	ActiveNote active_note_[max_active_notes];
	int current_note_;
	uint8_t input_buffer_[8192];
	size_t input_buffer_index_;

	uint8_t patch_data_[4096];
	int current_patch_;

	char unpacked_patch_[156];

	// The original DX7 had one single LFO. Later units had an LFO per note.
	Lfo lfo_;

	// in MIDI units (0x4000 is neutral)
	Controllers controllers_;

	ResoFilter filter_;
	int32_t filter_control_[3];
	bool sustain_;

	// Extra buffering for when GetSamples wants a buffer not a multiple of N
	int16_t extra_buf_[SYNTH_N];
	int extra_buf_size_;

public:
	static void Init(double sample_rate);

	explicit SynthUnit(RingBuffer& ring_buffer) :
		ring_buffer_(ring_buffer), active_note_{}, current_note_(0),
		input_buffer_{ 0 }, input_buffer_index_(0),
		patch_data_{ 0 }, current_patch_(0), unpacked_patch_{ 0 },
		lfo_()
	{
		memcpy(patch_data_, epiano_, sizeof(epiano_));
		ProgramChange(0);
		filter_control_[0] = 258847126;
		filter_control_[1] = 0;
		filter_control_[2] = 0;
		controllers_.values_[kControllerPitch] = 0x2000;
		sustain_ = false;
		extra_buf_size_ = 0;
	}

  void GetSamples(int n_samples, int16_t *buffer);

	bool get_patch_name(uint32_t pno, char* dst, uint32_t len) const {
		if(dst == nullptr || len == 0) return false;
		dst[0] = 0;
		if(pno >= 32 || len < 11) return false;
		memcpy(dst, patch_data_ + (pno * 128) + 118, 10);
		dst[10] = 0;
		return true;
	} 
 private:
  void TransferInput();

  void ConsumeInput(int n_input_bytes);

  // Choose a note for a new key-down, returns note number, or -1 if
  // none available.
  int AllocateNote();

  // zero-based
  void ProgramChange(int p);

  void SetController(int controller, int value);

  int ProcessMidiMessage(const uint8_t *buf, int buf_size);
};
