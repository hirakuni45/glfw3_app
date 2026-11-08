#pragma once
//=========================================================================//
/*!	@file
	@brief	画素の定義
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <vector>
#include <cstdint>
#include <unordered_set>

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	イメージの形
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct IMG {
		enum type {
			NONE,
			INDEXED8,	///< インデックスカラー８ビット
			GRAY8,		///< グレースケール８ビット
			FULL8,		///< フルカラー８ビット
			INDEXED16,	///< インデックスカラー１６ビット
			GRAY16,		///< グレースケール１６ビット
			FULL16,		///< フルカラー１６ビット
		};
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット、インデックスド・カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct idx8 {
		typedef	uint8_t	value_type;
		static const IMG::type	type_ = IMG::INDEXED8;

		uint8_t	i;

		idx8() { }
		idx8(uint8_t val) : i(val) { }
		void set(uint8_t val) { i = val; }
		void swap(idx8& idx) { std::swap(idx, *this); }
		inline size_t hash() const {
			return std::hash<uint8_t>()(i);
		}

		bool operator == (const idx8& c) const {
			if(i == c.i) return true;
			else return false;
		}
		bool operator != (const idx8& c) const {
			if(i == c.i) return false;
			else return true;
		}
		bool operator < (const idx8& c) const {
			if(i < c.i) return true;
			else return false;
		}
		idx8& operator = (const idx8& c) {
			i = c.i;
			return *this;
		}
	};

//	inline size_t hash_value(const idx8& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット、インデックスド・カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct idx16 {
		typedef	uint16_t	value_type;
		static const IMG::type	type_ = IMG::INDEXED16;

		uint16_t	i;

		idx16() { }
		idx16(uint16_t val) : i(val) { }
		void set(uint16_t val) { i = val; }
		void swap(idx16& idx) { std::swap(idx, *this); }
		inline size_t hash() const {
			return std::hash<uint16_t>()(i);
		}
		bool operator == (const idx16& c) const {
			if(i == c.i) return true;
			else return false;
		}
		bool operator != (const idx16& c) const {
			if(i == c.i) return false;
			else return true;
		}
		bool operator < (const idx16& c) const {
			if(i < c.i) return true;
			else return false;
		}
		idx16& operator = (const idx16& c) {
			i = c.i;
			return *this;
		}
	};

//	inline size_t hash_value(const idx16& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	グレー・スケール
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct gray8 {
		typedef	uint8_t  value_type;
		static const IMG::type	type_ = IMG::GRAY8;

		uint8_t	g;

		gray8() { }
		gray8(uint8_t val) : g(val) { }
		void set(uint8_t val) { g = val; }
		void swap(gray8& g_) { std::swap(g_, *this); }
		inline size_t hash() const {
			return std::hash<uint8_t>()(g);
		}
		bool operator == (const gray8& c) const {
			if(g == c.g) return true;
			else return false;
		}
		bool operator != (const gray8& c) const {
			if(g == c.g) return false;
			else return true;
		}
		bool operator < (const gray8& c) const {
			if(g < c.g) return true;
			else return false;
		}
		gray8& operator = (const gray8& c) {
			g = c.g;
			return *this;
		}
	};

//	inline size_t hash_value(const gray8& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット、RGBA カラー (32 Bits)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct rgba8 {
		typedef	uint8_t	value_type;
		static const IMG::type	type_ = IMG::FULL8;

		uint8_t	r,g,b,a;

		rgba8() { }
		constexpr rgba8(uint8_t v, uint8_t a_ = 255) : r(v), g(v), b(v), a(a_) { }
		constexpr rgba8(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca = 255) : r(cr), g(cg), b(cb), a(ca) { }
		void set(uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca = 255) { r = cr; g = cg; b = cb; a = ca; }
		void mod(uint8_t m) {
			uint16_t mm = static_cast<uint16_t>(m) + 1;
			r = (static_cast<uint16_t>(r) * mm) >> 8;
			g = (static_cast<uint16_t>(g) * mm) >> 8;
			b = (static_cast<uint16_t>(b) * mm) >> 8;
		}
		void swap(rgba8& rgba) { std::swap(rgba, *this); }

		inline size_t hash() const {
			size_t h = std::hash<uint8_t>()(r);
			h ^= std::hash<uint8_t>()(g);
			h ^= std::hash<uint8_t>()(b);
			h ^= std::hash<uint8_t>()(a);
			return h;
		}
		bool operator == (const rgba8& c) const {
			if(r == c.r && g == c.g && b == c.b && a == c.a) return true;
			else return false;
		}
		bool operator != (const rgba8& c) const {
			if(r == c.r && g == c.g && b == c.b && a == c.a) return false;
			else return true;
		}
		bool operator < (const rgba8& c) const {
			if((r | (g << 8) | (b << 16) | (a << 24)) < (c.r | (c.g << 8) | (c.b << 16) | (c.a << 24))) return true;
			else return false;
		}
		rgba8& operator = (const rgba8& c) {
			r = c.r; g = c.g; b = c.b; a = c.a;
			return *this;
		}

		rgba8 operator * (float s) const {
			rgba8 c;
			c.r = static_cast<uint8_t>(static_cast<float>(r) * s);
			c.g = static_cast<uint8_t>(static_cast<float>(g) * s);
			c.b = static_cast<uint8_t>(static_cast<float>(b) * s);
			c.a = a;
			return c;
		}

		rgba8& operator *= (float s) {
			r = static_cast<uint8_t>(static_cast<float>(r) * s);
			g = static_cast<uint8_t>(static_cast<float>(g) * s);
			b = static_cast<uint8_t>(static_cast<float>(b) * s);
			return *this;
		}

		// NTSC 輝度: Y =                0.299*R + 0.587*G + 0.114*B
		// NTSC 色差: Cb= 0.564*(B-Y) = -0.169*R - 0.331*G + 0.500*B
		//            Cr= 0.713*(R-Y) =  0.500*R - 0.419*G - 0.081*B
		uint8_t getY() const { return (static_cast<uint32_t>(r) * 19595
								+ static_cast<uint32_t>(g) * 38470
								+ static_cast<uint32_t>(b) * 7471) >> 16; }

		void gainY(float gain) {
			float rr = r * (1.0f - 0.299f) + r * 0.299f * gain;
			float gg = g * (1.0f - 0.587f) + g * 0.587f * gain;
			float bb = b * (1.0f - 0.114f) + b * 0.114f * gain;
			if(rr > 255.0f) rr = 255.0f;
			if(gg > 255.0f) gg = 255.0f;
			if(bb > 255.0f) bb = 255.0f;
			r = static_cast<uint8_t>(rr);
			g = static_cast<uint8_t>(gg);
			b = static_cast<uint8_t>(bb);
		}


		void alpha_mix(const rgba8& bc, uint8_t alpha) {
			uint16_t ii = 255 - alpha;
			uint16_t aa = alpha + 1;
			r = ((bc.r * ii) + (r * aa)) >> 8;
			g = ((bc.g * ii) + (g * aa)) >> 8;
			b = ((bc.b * ii) + (b * aa)) >> 8;
			a = 255;
		}


		void alpha_scale(float scale) {
			float fa = static_cast<float>(a) * scale;
			if(fa < 0.0f) fa = 0.0f;
			else if(fa > 255.0f) fa = 255.0f; 
			a = static_cast<uint8_t>(fa);
		}

	};

//	inline size_t hash_value(const rgba8& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット、RGBA カラー (64 Bits)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct rgba16 {
		typedef	uint16_t	value_type;
		static const IMG::type	type_ = IMG::FULL16;

		uint16_t	r,g,b,a;

		rgba16() { }
		rgba16(uint16_t cr, uint16_t cg, uint16_t cb, uint16_t ca = 65535) : r(cr), g(cg), b(cb), a(ca) { }
		void set(uint16_t cr, uint16_t cg, uint16_t cb, uint16_t ca = 65535) { r = cr; g = cg; b = cb; a = ca; }

		inline size_t hash() const {
			size_t h = std::hash<uint16_t>()(r);
			h ^= std::hash<uint16_t>()(g);
			h ^= std::hash<uint16_t>()(b);
			h ^= std::hash<uint16_t>()(a);
			return h;
		}

		bool operator == (const rgba8& c) const {
			if(r == c.r && g == c.g && b == c.b && a == c.a) return true;
			else return false;
		}

		rgba16& operator = (const rgba16& c) {
			r = c.r; g = c.g; b = c.b; a = c.a;
			return *this;
		}

		rgba16 operator * (float s) const {
			rgba16 c;
			c.r = static_cast<uint16_t>(static_cast<float>(r) * s);
			c.g = static_cast<uint16_t>(static_cast<float>(g) * s);
			c.b = static_cast<uint16_t>(static_cast<float>(b) * s);
			c.a = a;
			return c;
		}

		rgba16& operator *= (float s) {
			r = static_cast<uint16_t>(static_cast<float>(r) * s);
			g = static_cast<uint16_t>(static_cast<float>(g) * s);
			b = static_cast<uint16_t>(static_cast<float>(b) * s);
			return *this;
		}
	};

//	inline size_t hash_value(const rgba16& v) { return v.hash(); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	RGBA カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct rgba_color {
		typedef	T	value_type;
		union {
			T	rgba[4];
			struct {
				T	r, g, b, a;
			};
		};

		rgba_color() : a(1) { }
		rgba_color(const rgba_color& c) : r(c.r), g(c.g), b(c.b), a(c.a) { }
		rgba_color(T v) : r(v), g(v), b(v), a(v) { }
		rgba_color(T c, T a_) : r(c), g(c), b(c), a(a_) { }
		rgba_color(T r_, T g_, T b_, T a_ = static_cast<T>(1)) : r(r_), g(g_), b(b_), a(a_) { }
		inline void set(const rgba_color& c) { r = c.r; g = c.g; b = c.b; a = c.a; }
		inline void set(T v) { r = v; g = v; b = v; a = v; }
		inline void set(T c, T a_) { r = c; g = c; b = c; a = a_; }
		inline void set(T r_, T g_, T b_, T a_ = static_cast<T>(1)) { r = r_; g = g_; b = b_; a = a_; }
		inline void swap(rgba_color& c) { std::swap(c, *this); }

		bool operator == (const rgba_color& c) const {
			if(r == c.r && g == c.g && b == c.b && a == c.a) return true;
			else return false;
		}

		rgba_color& operator = (const rgba8& c) {
			r = static_cast<T>(c.r) / 255.0f;
			g = static_cast<T>(c.g) / 255.0f;
			b = static_cast<T>(c.b) / 255.0f;
			a = static_cast<T>(c.a) / 255.0f;
			return *this;
		}

		rgba_color& operator = (const rgba16& c) {
			r = static_cast<T>(c.r) / 65535.0f;
			g = static_cast<T>(c.g) / 65535.0f;
			b = static_cast<T>(c.b) / 65535.0f;
			a = static_cast<T>(c.a) / 65535.0f;
			return *this;
		}

		rgba_color& operator = (const rgba_color& c) {
			r = c.r; g = c.g; b = c.b; a = c.a;
			return *this;
		}

		rgba_color operator * (float s) const {
			rgba_color c;
			c.r = r * s;
			c.g = g * s;
			c.b = b * s;
			c.a = a;
			return c;
		}

		rgba_color& operator *= (float s) {
			r = static_cast<T>(static_cast<float>(r) * s);
			g = static_cast<T>(static_cast<float>(g) * s);
			b = static_cast<T>(static_cast<float>(b) * s);
			return *this;
		}

	};

	typedef rgba_color<float>	rgbaf;


	//-----------------------------------------------------------------//
	/*!
		@brief	イメージ情報構造体
	*/
	//-----------------------------------------------------------------//
	struct img_info {
		int16_t		r_depth;		///< 赤深度
		int16_t		g_depth;		///< 緑深度
		int16_t		b_depth;		///< 青深度
		int16_t		a_depth;		///< アルファ深度
		int16_t		i_depth;		///< インデックスカラーのインデックス深度
		int16_t		clut_num;		///< カラールックアップテーブルの数
		int16_t		width;			///< 画像の横幅
		int16_t		height;			///< 画像の高さ
		int16_t		mipmap_level;	///< ミップマップ・レベル
		int16_t		multi_level;	///< 複数画像数（キューブマップなど）
		bool		grayscale;		///< グレイ・スケール
	};

}	// namespace img

namespace std {
    template <>
    struct hash<img::idx8> {
        size_t operator()(const img::idx8& p) const {
            return p.hash();
        }
    };

    template <>
    struct hash<img::idx16> {
        size_t operator()(const img::idx16& p) const {
            return p.hash();
        }
    };

    template <>
    struct hash<img::gray8> {
        size_t operator()(const img::gray8& p) const {
            return p.hash();
        }
    };

	template <>
    struct hash<img::rgba8> {
        size_t operator()(const img::rgba8& p) const {
            return p.hash();
        }
    };

	template <>
    struct hash<img::rgba16> {
        size_t operator()(const img::rgba16& p) const {
            return p.hash();
        }
    };
}
