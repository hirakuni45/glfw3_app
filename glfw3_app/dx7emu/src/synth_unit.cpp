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

#ifdef VERBOSE
#include <iostream>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "synth", __VA_ARGS__)
#endif

#include <cstring>

#include "synth.h"
#include "freqlut.h"
#include "sin.h"
#include "exp2.h"
#include "pitchenv.h"
#include "patch.h"
#include "synth_unit.h"
#include "aligned_buf.h"


void SynthUnit::Init(double sample_rate) {
  Freqlut::init(sample_rate);
  Exp2::init();
  Tanh::init();
  Sin::init();
  Lfo::init(sample_rate);
  PitchEnv::init(sample_rate);
}

// Transfer as many bytes as possible from ring buffer to input buffer.
// Note that this implementation has a fair amount of copying - we'd probably
// do it a bit differently if it were bulk data, but in this case we're
// optimizing for simplicity of implementation.
void SynthUnit::TransferInput() {
  size_t bytes_available = ring_buffer_.BytesAvailable();
  int bytes_to_read = min(bytes_available,
      sizeof(input_buffer_) - input_buffer_index_);
  if (bytes_to_read > 0) {
    ring_buffer_.Read(bytes_to_read, input_buffer_ + input_buffer_index_);
    input_buffer_index_ += bytes_to_read;
  }
}

void SynthUnit::ConsumeInput(int n_input_bytes) {
  if (static_cast<uint32_t>(n_input_bytes) < input_buffer_index_) {
    memmove(input_buffer_, input_buffer_ + n_input_bytes,
        input_buffer_index_ - n_input_bytes);
  }
  input_buffer_index_ -= n_input_bytes;
}

int SynthUnit::AllocateNote() {
  int note = current_note_;
  for (int i = 0; i < max_active_notes; i++) {
    if (!active_note_[note].keydown) {
      current_note_ = (note + 1) % max_active_notes;
      return note;
    }
    note = (note + 1) % max_active_notes;
  }
  return -1;
}

void SynthUnit::ProgramChange(int p) {
  current_patch_ = p;
  const uint8_t *patch = patch_data_ + 128 * current_patch_;
  UnpackPatch((const char *)patch, unpacked_patch_);
  lfo_.reset(unpacked_patch_ + 137);
}

void SynthUnit::SetController(int controller, int value) {
  controllers_.values_[controller] = value;
}

int SynthUnit::ProcessMidiMessage(const uint8_t *buf, int buf_size) {
  uint8_t cmd = buf[0];
  uint8_t cmd_type = cmd & 0xf0;
  //LOGI("got %d midi: %02x %02x %02x", buf_size, buf[0], buf[1], buf[2]);
  if (cmd_type == 0x80 || (cmd_type == 0x90 && buf[2] == 0)) {
    if (buf_size >= 3) {
      // note off
      for (int note = 0; note < max_active_notes; ++note) {
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
      int note_ix = AllocateNote();
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
          for (int note = 0; note < max_active_notes; note++) {
            if (active_note_[note].sustained && !active_note_[note].keydown) {
              active_note_[note].dx7_note.keyup();
              active_note_[note].sustained = false;
            }
          }
        }
      }
      return 3;
    } return 0;
  } else if (cmd_type == 0xc0) {
    if (buf_size >= 2) {
      // program change
      int program_number = buf[1];
      ProgramChange(min(program_number, 31));
      char name[11];
      memcpy(name, unpacked_patch_ + 145, 10);
      name[10] = 0;
#ifdef VERBOSE
      std::cout << "Loaded patch " << current_patch_ << ": " << name << "\r";
      std::cout.flush();
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
/// utils::format("Load Patch Data\n");
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
  std::cout << "Unknown message " << std::hex << (int)cmd <<
    ", skipping " << std::dec << buf_size << " bytes" << std::endl;
#endif
  return buf_size;
}

void SynthUnit::GetSamples(int n_samples, int16_t *buffer) {
  TransferInput();
  size_t input_offset;
  for (input_offset = 0; input_offset < input_buffer_index_; ) {
    int bytes_available = input_buffer_index_ - input_offset;
    int bytes_consumed = ProcessMidiMessage(input_buffer_ + input_offset,
        bytes_available);
    if (bytes_consumed == 0) {
      break;
    }
    input_offset += bytes_consumed;
  }
  ConsumeInput(input_offset);

  int i;
  for (i = 0; i < n_samples && i < extra_buf_size_; i++) {
    buffer[i] = extra_buf_[i];
  }
  if (extra_buf_size_ > n_samples) {
    for (int j = 0; j < extra_buf_size_ - n_samples; j++) {
      extra_buf_[j] = extra_buf_[j + n_samples];
    }
    extra_buf_size_ -= n_samples;
    return;
  }

  for (; i < n_samples; i += SYNTH_N) {
    AlignedBuf<int32_t, SYNTH_N> audiobuf;
    AlignedBuf<int32_t, SYNTH_N> audiobuf2;
    for (int j = 0; j < SYNTH_N; ++j) {
      audiobuf.get()[j] = 0;
    }
    int32_t lfovalue = lfo_.getsample();
    int32_t lfodelay = lfo_.getdelay();
    for (int note = 0; note < max_active_notes; ++note) {
      if (active_note_[note].live) {
        active_note_[note].dx7_note.compute(audiobuf.get(), lfovalue, lfodelay,
          &controllers_);
      }
    }
    const int32_t *bufs[] = { audiobuf.get() };
    int32_t *bufs2[] = { audiobuf2.get() };
    filter_.process(bufs, filter_control_, filter_control_, bufs2);
    int jmax = n_samples - i;
    for (int j = 0; j < SYNTH_N; ++j) {
      int32_t val = audiobuf2.get()[j] >> 4;
      int clip_val = val < -(1 << 24) ? 0x8000 : val >= (1 << 24) ? 0x7fff :
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
