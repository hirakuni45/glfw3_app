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
#include "controllers.h"
#include "lfo.hpp"
#include "ringbuffer.hpp"
#include "resofilter.h"
#include "dx7note.hpp"
#include "freqlut.hpp"
#include "sin.hpp"
#include "exp2.hpp"
#include "pitchenv.hpp"
#include "patch.hpp"

namespace synth {

	class DX7 {

#ifdef WIN32
		static const int max_active_notes = 16;
#else
#if defined(SIG_RX65N)
		static const int max_active_notes = 8;
#elif defined(SIG_RX72N)
		static const int max_active_notes = 16;
#endif
#endif

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

		RingBuffer	ring_buffer_;

		ActiveNote	active_note_[max_active_notes];
		int			current_note_;
		uint8_t		input_buffer_[8192];
		uint32_t	input_buffer_index_;

		uint8_t		patch_data_[4096];
		uint32_t	current_patch_;

		char		unpacked_patch_[156];

		// The original DX7 had one single LFO. Later units had an LFO per note.
		Lfo			lfo_;

		// in MIDI units (0x4000 is neutral)
		Controllers	controllers_;

		ResoFilter	filter_;
		int32_t		filter_control_[3];
		bool		sustain_;

		// Extra buffering for when GetSamples wants a buffer not a multiple of N
		int16_t		extra_buf_[SYNTH_N];
		uint32_t	extra_buf_size_;

		PitchEnv	pitch_env_;

	public:
		DX7() :
			ring_buffer_(), active_note_{}, current_note_(0),
			input_buffer_{ 0 }, input_buffer_index_(0),
			patch_data_{ 0 }, current_patch_(0), unpacked_patch_{ 0 },
			lfo_(),
//			controllers_(),
			filter_(), filter_control_{ 258847126, 0, 0 }, sustain_(false),

			pitch_env_()
		{
			memcpy(patch_data_, epiano_, sizeof(epiano_));
			ProgramChange(0);
			controllers_.values_[kControllerPitch] = 0x2000;
//			filter_control_[0] = 258847126;
//			filter_control_[1] = 0;
//			filter_control_[2] = 0;
//			sustain_ = false;
			extra_buf_size_ = 0;
		}


		void Init(double sample_rate)
		{
			Freqlut::init(sample_rate);
			Exp2::init();
			Tanh::init();
			Sin::init();
			lfo_.init(sample_rate);
			PitchEnv::init(sample_rate);
		}


		auto& at_msg() { return ring_buffer_; }


		void GetSamples(int n_samples, int16_t *buffer)
		{
			TransferInput();
			uint32_t input_offset;
			for (input_offset = 0; input_offset < input_buffer_index_; ) {
				auto bytes_available = input_buffer_index_ - input_offset;
				auto bytes_consumed = ProcessMidiMessage(input_buffer_ + input_offset, bytes_available);
				if (bytes_consumed == 0) {
					break;
				}
				input_offset += bytes_consumed;
			}
			ConsumeInput(input_offset);

			uint32_t i;
			for (i = 0; i < n_samples && i < extra_buf_size_; i++) {
				buffer[i] = extra_buf_[i];
			}
			if (extra_buf_size_ > n_samples) {
				for (uint32_t j = 0; j < extra_buf_size_ - n_samples; j++) {
					extra_buf_[j] = extra_buf_[j + n_samples];
				}
				extra_buf_size_ -= n_samples;
				return;
			}

			for (; i < n_samples; i += SYNTH_N) {
				AlignedBuf<int32_t, SYNTH_N> audiobuf;
				AlignedBuf<int32_t, SYNTH_N> audiobuf2;
				for (uint32_t j = 0; j < SYNTH_N; ++j) {
					audiobuf.get()[j] = 0;
				}
				int32_t lfovalue = lfo_.getsample();
				int32_t lfodelay = lfo_.getdelay();
				for (uint32_t note = 0; note < max_active_notes; ++note) {
					if (active_note_[note].live) {
						active_note_[note].dx7_note.compute(audiobuf.get(), lfovalue, lfodelay, 
							&controllers_);
					}
				}
				const int32_t* bufs[] = { audiobuf.get() };
				int32_t* bufs2[] = { audiobuf2.get() };
				filter_.process(bufs, filter_control_, filter_control_, bufs2);
				auto jmax = n_samples - i;
				for (uint32_t j = 0; j < SYNTH_N; ++j) {
					int32_t val = audiobuf2.get()[j] >> 4;
					auto clip_val = val < -(1 << 24) ? 0x8000 : val >= (1 << 24) ? 0x7fff :
						val >> 9;
					// TODO: maybe some dithering?
					if (j < jmax) {
						buffer[i + j] = clip_val;
					} else {
						extra_buf_[j - jmax] = clip_val;
					}
				}
			}
			extra_buf_size_ = i - n_samples;
		}


		bool get_patch_name(uint32_t pno, char* dst, uint32_t len) const {
			if(dst == nullptr || len == 0) return false;
			dst[0] = 0;
			if(pno >= 32 || len < 11) return false;
			memcpy(dst, patch_data_ + (pno * 128) + 118, 10);
			dst[10] = 0;
			return true;
		} 

	private:
		// Transfer as many bytes as possible from ring buffer to input buffer.
		// Note that this implementation has a fair amount of copying - we'd probably
		// do it a bit differently if it were bulk data, but in this case we're
		// optimizing for simplicity of implementation.
		void TransferInput()
		{
			auto bytes_available = ring_buffer_.BytesAvailable();
			auto bytes_to_read = min(bytes_available,
				static_cast<uint32_t>(sizeof(input_buffer_) - input_buffer_index_));
			if (bytes_to_read > 0) {
				ring_buffer_.Read(bytes_to_read, input_buffer_ + input_buffer_index_);
				input_buffer_index_ += bytes_to_read;
			}
		}


		void ConsumeInput(int n_input_bytes)
		{
			if (static_cast<uint32_t>(n_input_bytes) < input_buffer_index_) {
				memmove(input_buffer_, input_buffer_ + n_input_bytes,
				input_buffer_index_ - n_input_bytes);
			}
			input_buffer_index_ -= n_input_bytes;
		}


		// Choose a note for a new key-down, returns note number, or -1 if
		// none available.
		int AllocateNote()
		{
			auto note = current_note_;
			for (uint32_t i = 0; i < max_active_notes; i++) {
				if (!active_note_[note].keydown) {
					current_note_ = (note + 1) % max_active_notes;
					return note;
				}
				note = (note + 1) % max_active_notes;
			}
			return -1;
		}


		// zero-based
		void ProgramChange(int p)
		{
			current_patch_ = p;
			const uint8_t *patch = patch_data_ + 128 * current_patch_;
			Patch::Unpack((const char *)patch, unpacked_patch_);
			lfo_.reset(unpacked_patch_ + 137);
		}


		void SetController(int controller, int value)
		{
			controllers_.values_[controller] = value;
		}


		int ProcessMidiMessage(const uint8_t* buf, int buf_size)
		{
			uint8_t cmd = buf[0];
			uint8_t cmd_type = cmd & 0xf0;
			// LOGI("got %d midi: %02x %02x %02x", buf_size, buf[0], buf[1], buf[2]);
			if (cmd_type == 0x80 || (cmd_type == 0x90 && buf[2] == 0)) {
				if (buf_size >= 3) {
					// note off
					for (uint32_t note = 0; note < max_active_notes; ++note) {
						if (active_note_[note].midi_note == buf[1] && 
							active_note_[note].keydown) {
							if (sustain_) {
								active_note_[note].sustained = true;
							} else {
								active_note_[note].dx7_note.keyup();
							}
							active_note_[note].keydown = false;
						}
					}
					return 3;
				}
				return 0;
			} else if (cmd_type == 0x90) {
				if (buf_size >= 3) {
					// note on
					auto note_ix = AllocateNote();
					if (note_ix >= 0) {
						lfo_.keydown();  // TODO: should only do this if # keys down was 0
						active_note_[note_ix].midi_note = buf[1];
						active_note_[note_ix].keydown = true;
						active_note_[note_ix].sustained = sustain_;
						active_note_[note_ix].live = true;
						active_note_[note_ix].dx7_note.init(unpacked_patch_, buf[1], buf[2]);
					}
					return 3;
				}
				return 0;
			} else if (cmd_type == 0xb0) {
				if (buf_size >= 3) {
					// controller
					// TODO: move more logic into SetController
					int controller = buf[1];
					int value = buf[2];
					if (controller == 1) {
						filter_control_[0] = 142365917 + value * 917175;
					} else if (controller == 2) {
						filter_control_[1] = value * 528416;
					} else if (controller == 3) {
						filter_control_[2] = value * 528416;
					} else if (controller == 64) {
						sustain_ = value != 0;
						if (!sustain_) {
							for (int32_t note = 0; note < max_active_notes; note++) {
								if (active_note_[note].sustained && !active_note_[note].keydown) {
									active_note_[note].dx7_note.keyup();
									active_note_[note].sustained = false;
								}
							}
						}
					}
					return 3;
				}
				return 0;
			} else if (cmd_type == 0xc0) {
				if (buf_size >= 2) {
					// program change
					int program_number = buf[1];
					ProgramChange(min(program_number, 31));
					char name[11];
					memcpy(name, unpacked_patch_ + 145, 10);
					name[10] = 0;
#ifdef VERBOSE
					utils::format("Loaded patch %d: %s\n") % current_patch_ % name;
#endif
					return 2;
				}
				return 0;
			} else if (cmd == 0xe0) {
				// pitch bend
				SetController(kControllerPitch, buf[1] | (buf[2] << 7));
				return 3;
			} else if (cmd == 0xf0) {
				// sysex
				if (buf_size >= 6 && buf[1] == 0x43 && buf[2] == 0x00 && buf[3] == 0x09 &&
					buf[4] == 0x20 && buf[5] == 0x00) {
					if (buf_size >= 4104) {
						// TODO: check checksum?
						memcpy(patch_data_, buf + 6, 4096);
						ProgramChange(current_patch_);
#if 0
						for(int i = 0; i < 32; ++i) {
							char tmp[11];
							memcpy(tmp, patch_data_ + i * 128 + 128 - 10, 10);
							tmp[10] = 0;
							utils::format("%2d: '%s'\n") % i % tmp;
						}
#endif
						return 4104;
					}
				return 0;
				}
			}

			// TODO: more robust handling
#ifdef VERBOSE
			utils::format("Unknown message %02X, skipping %d bytes\n")
				% static_cast<uint16_t>(cmd) % buf_size; 
#endif
			return buf_size;
		}
	};
}
