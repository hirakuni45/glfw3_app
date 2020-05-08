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

// move to generic utility file?
void *malloc_aligned(size_t alignment, size_t nbytes);

// Abstract class
template <typename T, typename U>
class FirFilter {
public:
  // preconditions:
  // in + (nk - 1) is aligned to 128 bits
  // out is aligned to 128 bits
  virtual void process(const T *in, U *out, size_t n) = 0;
  virtual ~FirFilter() { }
};


class SimpleFirFilter : public FirFilter<float, float> {
	size_t nk;
	float* k;
public:
	SimpleFirFilter(const float *kernel, size_t nk) : nk(nk)
	{
		k = (float *)malloc(nk * sizeof(k[0]));
		for (size_t i = 0; i < nk; i++) {
			k[i] = kernel[nk - i - 1];
		}
	}

	~SimpleFirFilter() {
		free(k);
	}

	void process(const float *in, float *out, size_t n)
	{
		for (size_t i = 0; i < n; i++) {
			float y = 0;
			for (size_t j = 0; j < nk; j++) {
				y += k[j] * in[i + j];
			}
			out[i] = y;
		}
	}
};

class HalfRateFirFilter : public FirFilter<float, float> {
	static const int kMaxNk = 256;
	size_t nk;
	float* i0;
	float* i1;
	float* i2;
	float* y0;
	float* y1;
	float* y2;
	float* k2;
	FirFilter* f0;
	FirFilter* f1;
	FirFilter* f2;
public:
	HalfRateFirFilter(const float *kernel, size_t nk, size_t n) : nk(nk) {
  float k0[kMaxNk / 2];
  float k1[kMaxNk / 2];
  size_t n2 = n >> 1;
  size_t nk2 = nk >> 1;
  // probably better to do fewer allocations and just set up pointers...
  y0 = (float *)malloc_aligned(16, n2 * sizeof(y0[0]));
  y1 = (float *)malloc_aligned(16, n2 * sizeof(y1[0]));
  y2 = (float *)malloc_aligned(16, n2 * sizeof(y2[0]));
  i0 = (float *)malloc_aligned(16, (n2 + nk2) * sizeof(i0[0]));
  i1 = (float *)malloc_aligned(16, (n2 + nk2) * sizeof(i1[0]));
  i2 = (float *)malloc_aligned(16, (n2 + nk2) * sizeof(i2[0]));
  k2 = (float *)malloc_aligned(16, nk2 * sizeof(k2[0]));
  for (size_t i = 0; i < nk2; i++) {
    float b0 = kernel[i * 2];
    float b2 = kernel[i * 2 + 1];
    k0[i] = b0;
    k1[i] = b0 + b2;
    k2[i] = b2;
  }
  f0 = new SimpleFirFilter(k0, nk2);
  f1 = new SimpleFirFilter(k1, nk2);
  f2 = new SimpleFirFilter(k2, nk2);
}
	~HalfRateFirFilter() {
		free(k2);
		free(i0);
		free(i1);
		free(i2);
		free(y0);
		free(y1);
		free(y2);
		delete f0;
		delete f1;
		delete f2;
	}

	void process(const float *in, float *out, size_t n);

};

#ifdef HAVE_NEON

class NeonFirFilter : public FirFilter<float, float> {
 public:
  NeonFirFilter(const float *kernel, size_t nk);
  ~NeonFirFilter();
  void process(const float *in, float *out, size_t n);
 private:
  size_t nk;
  float *k;
};

class Neon16FirFilter : public FirFilter<float, float> {
 public:
  Neon16FirFilter(const float *kernel, size_t nk, bool mirror);
  ~Neon16FirFilter();
  void process(const float *in, float *out, size_t n);
 private:
  size_t nk;
  int16_t *k;
  bool mirror;
};

#endif  // HAVE_NEON

#ifdef __SSE2__

class SseFirFilter : public FirFilter<float, float> {
 public:
  SseFirFilter(const float *kernel, size_t nk);
  ~SseFirFilter();
  void process(const float *in, float *out, size_t n);
 private:
  size_t nk;
  float *k;
};

#endif  // __SSE2__
