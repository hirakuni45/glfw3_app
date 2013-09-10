#pragma once
//=====================================================================//
/*!	@file
	@brief	画素の定義
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_set.hpp>

namespace img {

	typedef unsigned char	u8;
	typedef unsigned short	u16;
	typedef unsigned long	u32;

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
		typedef	u8				value_type;

		u8	i;

		idx8() { }
		idx8(u8 val) : i(val) { }
		void set(u8 val) { i = val; }
		void swap(idx8& idx) { std::swap(idx, *this); }
		inline size_t hash() const {
			size_t h = 0;
			boost::hash_combine(h, i);
			return h;
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

	inline size_t hash_value(const idx8& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット、インデックスド・カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct idx16 {
		typedef	u16				value_type;

		u16	i;

		idx16() { }
		idx16(u16 val) : i(val) { }
		void set(u16 val) { i = val; }
		void swap(idx16& idx) { std::swap(idx, *this); }
		inline size_t hash() const {
			size_t h = 0;
			boost::hash_combine(h, i);
			return h;
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

	inline size_t hash_value(const idx16& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット、グレー・スケール
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct gray8 {
		typedef	u8			value_type;

		u8	g;

		gray8() { }
		gray8(u8 val) : g(val) { }
		void set(u8 val) { g = val; }
		void swap(gray8& g_) { std::swap(g_, *this); }
		inline size_t hash() const {
			size_t h = 0;
			boost::hash_combine(h, g);
			return h;
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

	inline size_t hash_value(const gray8& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	8 ビット、RGBA カラー (32 Bits)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct rgba8 {
		typedef	u8			value_type;

		u8	r,g,b,a;

		rgba8() { }
		rgba8(u8 v, u8 a_ = 255) : r(v), g(v), b(v), a(a_) { }
		rgba8(u8 cr, u8 cg, u8 cb, u8 ca = 255) : r(cr), g(cg), b(cb), a(ca) { }
		void set(u8 cr, u8 cg, u8 cb, u8 ca = 255) { r = cr; g = cg; b = cb; a = ca; }
		void mod(u8 m) {
			u16 mm = static_cast<u16>(m) + 1;
			r = (static_cast<u16>(r) * mm) >> 8;
			g = (static_cast<u16>(g) * mm) >> 8;
			b = (static_cast<u16>(b) * mm) >> 8;
		}
		void swap(rgba8& rgba) { std::swap(rgba, *this); }

		inline size_t hash() const {
			size_t h = 0;
			boost::hash_combine(h, r);
			boost::hash_combine(h, g);
			boost::hash_combine(h, b);
			boost::hash_combine(h, a);
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
			c.r = static_cast<u8>(static_cast<float>(r) * s);
			c.g = static_cast<u8>(static_cast<float>(g) * s);
			c.b = static_cast<u8>(static_cast<float>(b) * s);
			c.a = a;
			return c;
		}

		rgba8& operator *= (float s) {
			r = static_cast<u8>(static_cast<float>(r) * s);
			g = static_cast<u8>(static_cast<float>(g) * s);
			b = static_cast<u8>(static_cast<float>(b) * s);
			return *this;
		}

		// NTSC 輝度: Y =                0.299*R + 0.587*G + 0.114*B
		// NTSC 色差: Cb= 0.564*(B-Y) = -0.169*R - 0.331*G + 0.500*B
		//            Cr= 0.713*(R-Y) =  0.500*R - 0.419*G - 0.081*B
		u8 getY() const { return (static_cast<u32>(r) * 19595
								+ static_cast<u32>(g) * 38470
								+ static_cast<u32>(b) * 7471) >> 16; }

		void alpha_mix(const rgba8& bc, u8 alpha) {
			u16 ii = 255 - alpha;
			u16 aa = alpha + 1;
			r = ((bc.r * ii) + (r * aa)) >> 8;
			g = ((bc.g * ii) + (g * aa)) >> 8;
			b = ((bc.b * ii) + (b * aa)) >> 8;
			a = 255;
		}

	};

	inline size_t hash_value(const rgba8& v) { return v.hash(); }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	16 ビット、RGBA カラー (64 Bits)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct rgba16 {
		typedef	u16				value_type;

		u16	r,g,b,a;

		rgba16() { }
		rgba16(u16 cr, u16 cg, u16 cb, u16 ca = 65535) : r(cr), g(cg), b(cb), a(ca) { }
		void set(u16 cr, u16 cg, u16 cb, u16 ca = 65535) { r = cr; g = cg; b = cb; a = ca; }
		inline size_t hash() const {
			size_t h = 0;
			boost::hash_combine(h, r);
			boost::hash_combine(h, g);
			boost::hash_combine(h, b);
			boost::hash_combine(h, a);
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
			c.r = static_cast<u16>(static_cast<float>(r) * s);
			c.g = static_cast<u16>(static_cast<float>(g) * s);
			c.b = static_cast<u16>(static_cast<float>(b) * s);
			c.a = a;
			return c;
		}

		rgba16& operator *= (float s) {
			r = static_cast<u16>(static_cast<float>(r) * s);
			g = static_cast<u16>(static_cast<float>(g) * s);
			b = static_cast<u16>(static_cast<float>(b) * s);
			return *this;
		}
	};

	inline size_t hash_value(const rgba16& v) { return v.hash(); }


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
		short		r_depth;		///< 赤深度
		short		g_depth;		///< 緑深度
		short		b_depth;		///< 青深度
		short		a_depth;		///< アルファ深度
		short		i_depth;		///< インデックスカラーのインデックス深度
		short		clut_num;		///< カラールックアップテーブルの数
		short		width;			///< 画像の横幅
		short		height;			///< 画像の高さ
		short		mipmap_level;	///< ミップマップ・レベル
		short		multi_level;	///< 複数画像数（キューブマップなど）
		bool		grayscale;		///< グレイ・スケール
	};

}	// namespace img

