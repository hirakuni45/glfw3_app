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
// for glfw3_app
#ifdef WIN32
#include <time.h>
#else
// for RX C++ framework
#include "common/delay.hpp"
#endif

#include <cstring>

#include "synth.h"

namespace synth {

	class RingBuffer {

		static const uint32_t kBufSize = 65536;
		uint8_t buf_[kBufSize];
		volatile uint32_t rd_ix_;
		volatile uint32_t wr_ix_;

	public:
		RingBuffer() : rd_ix_(0), wr_ix_(0) { }

		// Returns number of bytes available for reading.
		uint32_t BytesAvailable() const
		{
			return (wr_ix_ - rd_ix_) & (kBufSize - 1);
		}

		// Returns number of bytes that can be written without blocking.
		uint32_t WriteBytesAvailable()
		{
			return (rd_ix_ - wr_ix_ - 1) & (kBufSize - 1);
		}

		// Reads bytes. It is the caller's responsibility to make sure that
		// size <= a previous value of BytesAvailable().
		uint32_t Read(uint32_t size, uint8_t* bytes)
		{
			auto rd_ix = rd_ix_;
			SynthMemoryBarrier();  // read barrier, make sure data is committed before ix
			auto fragment_size = min(size, (kBufSize - rd_ix));
			memcpy(bytes, buf_ + rd_ix, fragment_size);
			if (static_cast<uint32_t>(size) > fragment_size) {
				memcpy(bytes + fragment_size, buf_, size - fragment_size);
			}
			SynthMemoryBarrier();  // full barrier, make sure read commits before updating
			rd_ix_ = (rd_ix + size) & (kBufSize - 1);
			return size;
		}


		// Writes bytes into the buffer. If the buffer is full, the method will
		// block until space is available.
		void Write(const uint8_t *bytes, uint32_t size)
		{
			auto remaining = size;
			while (remaining > 0) {
				auto rd_ix = rd_ix_;
				auto wr_ix = wr_ix_;
				auto space_available = (rd_ix - wr_ix - 1) & (kBufSize - 1);
				if (space_available == 0) {
#ifdef WIN32
					struct timespec sleepTime;
					sleepTime.tv_sec = 0;
					sleepTime.tv_nsec = 1000'000;
					nanosleep(&sleepTime, nullptr);
#else
					utils::delay::micro_second(1000);
#endif
				} else {
					auto wr_size = min(remaining, space_available);
					auto fragment_size = min(wr_size, kBufSize - wr_ix);
					memcpy(buf_ + wr_ix, bytes, fragment_size);
					if (wr_size > fragment_size) {
						memcpy(buf_, bytes + fragment_size, wr_size - fragment_size);
					}
					SynthMemoryBarrier();  // write barrier, make sure data commits
					wr_ix_ = (wr_ix + wr_size) & (kBufSize - 1);
					remaining -= wr_size;
					bytes += wr_size;
				}
			}
		}
	};
}
