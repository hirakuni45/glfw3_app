#pragma once
//=====================================================================//
/*!	@file
	@brief	汎用ペイント・クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
/// #include <unistd.h>
#include "img.hpp"
#include "i_img.hpp"
#include "img_rgba8.hpp"
#include "img_utils.hpp"
#include <stack>
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"
#include "core/ftimg.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ペイント・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class paint : public img_rgba8 {

	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	頂点輝度
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct intensity_rect {
			uint8_t	left_top,    center_top,    right_top;
			uint8_t	left_center, center_center, right_center;
			uint8_t	left_bottom, center_bottom, right_bottom;
			intensity_rect(uint8_t i = 255) :
				left_top(i),    center_top(i),    right_top(i),
				left_center(i), center_center(i), right_center(i),
				left_bottom(i), center_bottom(i), right_bottom(i) { }

			void set(uint8_t a0, uint8_t b0, uint8_t c0,
					 uint8_t a1, uint8_t b1, uint8_t c1,
					 uint8_t a2, uint8_t b2, uint8_t c2) {
				left_top    = a0; center_top    = b0; right_top    = c0;
				left_center = a1; center_center = b1; right_center = c1;
				left_bottom = a2; center_bottom = b2; right_bottom = c2;
			}

			size_t hash() const {
				size_t h = 0;
				boost::hash_combine(h, left_top);
				boost::hash_combine(h, center_top);
				boost::hash_combine(h, right_top);
				boost::hash_combine(h, left_center);
				boost::hash_combine(h, center_center);
				boost::hash_combine(h, right_center);
				boost::hash_combine(h, left_bottom);
				boost::hash_combine(h, center_bottom);
				boost::hash_combine(h, right_bottom);
				return h;
			}

			bool operator == (const intensity_rect& ir) const {
				return ir.left_top == left_top &&
				ir.center_top == center_top &&
				ir.right_top == right_top &&
				ir.left_center == left_center &&
				ir.center_center == center_center &&
				ir.right_center == right_center &&
				ir.left_bottom == left_bottom &&
				ir.center_bottom == center_bottom &&
				ir.right_bottom == right_bottom;
			}
		};

	private:
		struct line_t {
			short	min;
			short	max;
			void init(short v) { min = max = v; }
			void set(short v) {
				if(v < min) min = v;
				if(max < v) max = v;
			}
			short len() const { return max - min; }
		};
		typedef std::vector<line_t> lines;

		rgba8				fore_color_;
		rgba8				back_color_;
		std::stack<rgba8>	stack_color_;

		intensity_rect		inten_rect_;

		int					round_radius_;
		std::vector<int>	round_offset_;

		short				font_size_;
		short				font_space_;

		bool				alpha_blend_;

		inline uint8_t distance_(float d)
		{
			float m = 1.0f - (d * 2.0f / 3.0f);
			return 255.0f * m * m;
		}

		// 明るさの勾配を計算する
		typedef std::vector<uint8_t>	slopeT;
		typedef slopeT::iterator	   	slopeT_it;
		typedef slopeT::const_iterator 	slopeT_cit;
		static void make_slope_(int top, int center, int end, slopeT& slope)
		{
			float a = static_cast<float>(top);
			float hf = static_cast<float>(slope.size()) * 0.5f;
			float div0 = static_cast<float>(center - top) / hf;
			float div1 = static_cast<float>(end - center) / hf;
			for(uint32_t i = 0; i < slope.size(); ++i) {
				if(a > 255.0f) slope[i] = 255;
				else if(a < 0) slope[i] = 0;
				else slope[i] = static_cast<uint8_t>(a);
				if(i < (slope.size() / 4)) {
					a += div0 * 0.65f;
				} else if(i < (slope.size() / 2)) {
					a += div0 * 0.35f;
				} else if(i < (slope.size() * 4 / 3)) {
					a += div1 * 0.35f;
				} else {
					a += div1 * 0.65f;
				}
			}
		}


		void make_round_tables_()
		{
			round_offset_.resize(round_radius_);
			round_offset_.clear();
			for(int i = 0; i < round_radius_; ++i) {
				float fr = static_cast<float>(round_radius_);
				float fi = static_cast<float>(i) + 0.5f;
				float a = std::sqrt(fr * fr - fi * fi);
				round_offset_[round_radius_ - 1 - i] = static_cast<int>((fr - a) * 256.0f);
			}
		}


		void h_line_(const img::rgba8& c, const vtx::spos& pos, short len)
		{
			short l = 0;
			for(short i = 0; i < std::abs(len); ++i) {
				plot(vtx::spos(pos.x + l, pos.y), c);
				if(len < 0) --l; else ++l;
			}
		}


		void h_line_gray_(const img::rgba8& c, const vtx::spos& pos, short len)
		{
			img::rgba8 cc = c;
			short l = 0;
			short d = std::abs(len);
			for(short i = 0; i < d; ++i) {
				int a = static_cast<int>(d - i) * static_cast<int>(c.a) / d;
				cc.a = a;
				plot(vtx::spos(pos.x + l, pos.y), cc);
				if(len < 0) --l; else ++l;
			}
		}


		void make_scan_line_(const vtx::spos& st, const vtx::spos& en, int min, lines& lns)
		{
			int divx = en.x - st.x;
			int divy = std::abs(en.y - st.y);
			int idx = st.y;
			for(int i = 0; i < divy; ++i) {
				int v = st.x + divx * i / divy;
				uint32_t pos = idx - min;
				if(pos < lns.size()) {
					lns[pos].set(v);
				}
				if(st.y < en.y) ++idx;
				else --idx;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		paint() : fore_color_(255, 255, 255, 255), back_color_(0, 0, 0, 255),
			inten_rect_(255),
			round_radius_(0),
			font_size_(24), font_space_(2),
			alpha_blend_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~paint() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ・ブレンドの設定
			@param[in]	f	「false」なら無効
		*/
		//-----------------------------------------------------------------//
		void alpha_blend(bool f = true) { alpha_blend_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラウンドの設定
		*/
		//-----------------------------------------------------------------//
		void set_round(int radius) {
			round_radius_ = radius;
			make_round_tables_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーを設定
			@param[in]	c	カラー
		*/
		//-----------------------------------------------------------------//
		void set_fore_color(const rgba8& c) { fore_color_ = c; }


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーを取得
			@return 「fore」カラー
		*/
		//-----------------------------------------------------------------//
		const rgba8& get_fore_color() const { return fore_color_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	「back」カラーを設定
			@param[in]	c	カラー
		*/
		//-----------------------------------------------------------------//
		void set_back_color(const rgba8& c) { back_color_ = c; }


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーと「back」カラーを交換
		*/
		//-----------------------------------------------------------------//
		void swap_color() { fore_color_.swap(back_color_); }


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーを退避
		*/
		//-----------------------------------------------------------------//
		void push_fore_color() { stack_color_.push(fore_color_); }


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーを復帰
		*/
		//-----------------------------------------------------------------//
		void pop_fore_color() { fore_color_ = stack_color_.top(); stack_color_.pop(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	頂点輝度の設定
			@param[in]	ir	頂点輝度
		*/
		//-----------------------------------------------------------------//
		void set_intensity_rect(const intensity_rect& ir) { inten_rect_ = ir; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのサイズを指定
			@param[in]	size	サイズ
		*/
		//-----------------------------------------------------------------//
		void set_font_size(int size) { font_size_ = size; }


		//-----------------------------------------------------------------//
		/*!
			@brief	点を描画
			@param[in]	p	位置
			@param[in]	c	カラー
			@return 領域外なら「false」
		*/
		//-----------------------------------------------------------------//
		bool plot(const vtx::spos& p, const rgba8& c)
		{
			if(alpha_blend_) {
				rgba8 bc;
				if(get_pixel(p, bc)) {
					u16 a = static_cast<u16>(bc.a) + 1;
					a *= 256 - static_cast<u16>(c.a);
					a >>= 8;
					u16 r = static_cast<u16>(bc.r) * a;
					u16 g = static_cast<u16>(bc.g) * a;
					u16 b = static_cast<u16>(bc.b) * a;
					a = static_cast<u16>(c.a) + 1;
					r += static_cast<u16>(c.r) * a;
					g += static_cast<u16>(c.g) * a;
					b += static_cast<u16>(c.b) * a;
					put_pixel(p, rgba8(r >> 8, g >> 8, b >> 8, bc.a));
					return true;
				} else {
					return false;
				}
			} else {
				return put_pixel(p, c);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	点を描画
			@param[in]	x	位置X
			@param[in]	y	位置Y
			@param[in]	c	カラー
			@return 領域外なら「false」
		*/
		//-----------------------------------------------------------------//
		inline bool plot(short x, short y, const rgba8& c) {
			return plot(vtx::spos(x, y), c);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで線を描画
			@param[in]	x0	X 始点
			@param[in]	y0	Y 始点
			@param[in]	x1	X 終点
			@param[in]	y1	Y 終点
		*/
		//-----------------------------------------------------------------//
		void line(short x0, short y0, short x1, short y1)
		{
			vtx::spos adr(x0, y0);
			short dx = x1 - x0;
			short dy = y1 - y0;
			short du, dv, u, v;
			vtx::spos uincr, vincr;
			if(std::abs(dx) > std::abs(dy)) {
				du = std::abs(dx);
				dv = std::abs(dy);
				u = x1;
				v = y1;
				if(dx < 0) uincr.set(-1, 0);
				else uincr.set(1, 0);
				if(dy < 0) vincr.set(0, -1);
				else vincr.set(0, 1);
			} else {
				du = std::abs(dy);
				dv = std::abs(dx);
				u = y1;
				v = x1;
				if(dx < 0) uincr.set(0, -1);
				else uincr.set(0, 1);
				if(dy < 0) vincr.set(-1, 0);
				else vincr.set(1, 0);
			}

			short uend = u + 2 * du + 1;
			short d = (2 * dv) - du;
			short incrS = 2 * dv;
			short incrD = 2 * (dv - du);
			short twovdu = 0;
			float invD = 1.0f / (2.0f * sqrtf(du * du + dv * dv));
			float invD2du = 2.0f * (du * invD);
			do {
				rgba8 c = fore_color_;
/// c.r = c.g = c.b = 255;
				c.a = distance_(twovdu * invD);
/// std::cout << static_cast<int>(c.a) << ", ";
				plot(adr, c);
				c.a = distance_(invD2du - twovdu * invD);
/// std::cout << static_cast<int>(c.a) << ", ";
				plot(adr + vincr, c);
				c.a = distance_(invD2du + twovdu * invD);
/// std::cout << static_cast<int>(c.a) << std::endl;
				plot(adr - vincr, c);
				if (d < 0) {
					twovdu = d + du;
					d += incrS;
				} else {
					twovdu = d - du;
					d += incrD;
					++v;
					adr += vincr;
				}
				u += 2;
				adr += uincr;
			} while(u < uend) ;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで線を描画
			@param[in]	org	始点
			@param[in]	end	終点
		*/
		//-----------------------------------------------------------------//
		void line(const vtx::spos& org, const vtx::spos& end)
		{
			line(org.x, org.y, end.x, end.y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで水平線を描画
			@param[in]	org		始点
			@param[in]	len		長さ
			@param[in]	lw		線幅
		*/
		//-----------------------------------------------------------------//
		void line_holizontal(const vtx::spos& org, uint16_t len, uint16_t lw = 1)
		{
			auto pos = org;
			for(uint16_t l = 0; l < lw; ++l) {
				pos.x = org.x;
				for(uint16_t i = 0; i < len; ++i) {
					plot(pos, fore_color_);
					++pos.x;
				}
				++pos.y;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで水垂直線を描画
			@param[in]	org		始点
			@param[in]	len		長さ
			@param[in]	lw		線幅
		*/
		//-----------------------------------------------------------------//
		void line_vertical(const vtx::spos& org, uint16_t len, uint16_t lw = 1)
		{
			auto pos = org;
			for(uint16_t l = 0; l < lw; ++l) {
				pos.y = org.y;
				for(uint16_t i = 0; i < len; ++i) {
					plot(pos, fore_color_);
					++pos.y;
				}
				++pos.x;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	円を描画
			@param[in]	center	中心座標
			@param[in]	radius	半径
		*/
		//-----------------------------------------------------------------//
		void fill_circle(const vtx::spos& center, int radius)
		{
			using vtx::spos;
			short ln;
			for(int dy = 0; dy < radius; ++dy) {
				int r = radius - 1;
				int dx = static_cast<int>(sqrtf(r * r - dy * dy) * 256.0f);
				short l = dx >> 8;
				{
					short len = l * 2;
					++len;
					if(dy) h_line_(fore_color_, spos(center.x - l, center.y - dy), len);
					h_line_(fore_color_, spos(center.x - l, center.y + dy), len);
				}
				{
					u16 gain = dx & 255;
					short len = ln - l;
					if(len == 0) len = 1;
					if(gain || len > 1) {
						if(gain == 0) gain = 255;
						rgba8 c = fore_color_;
						c.a = (static_cast<u16>(fore_color_.a) * (gain + 1)) >> 8;
						if(dy) {
							h_line_gray_(c, spos(center.x - l - 1, center.y - dy), -len);
							h_line_gray_(c, spos(center.x + l + 1, center.y - dy),  len);
						}
						h_line_gray_(c, spos(center.x - l - 1, center.y + dy), -len);
						h_line_gray_(c, spos(center.x + l + 1, center.y + dy),  len);
					}
				}
				ln = l;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ポリゴンを描画
			@param[in]	points	点集合
		*/
		//-----------------------------------------------------------------//
		void fill_polygon(const vtx::sposs& points)
		{
			if(points.size() < 3) return;

			using vtx::spos;

			line_t lx;
			lx.init(points[0].x);
			line_t ly;
			ly.init(points[0].y);
			for(uint32_t i = 1; i < points.size(); ++i) {
				lx.set(points[i].x);
				ly.set(points[i].y);
			}

			lines ls;
			line_t iv;
			iv.min = lx.max;
			iv.max = lx.min;
			ls.resize(ly.len(), iv);
			for(uint32_t i = 0; i < points.size(); ++i) {
				const spos& t = points[i];
				uint32_t j = i + 1;
				if(j >= points.size()) j = 0;
				const spos& b = points[j];
				make_scan_line_(t, b, ly.min, ls);
			}

			for(uint32_t i = 0; i < ls.size(); ++i) {
				const line_t& t = ls[i];
				short y = ly.min + i;
				if(y >= 0 && y < get_size().y) {
					h_line_(fore_color_, spos(t.min, y), t.len());
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで矩形領域を描画
			@param[in]	x	X 開始位置
			@param[in]	y	Y 開始位置
			@param[in]	w	描画幅
			@param[in]	h	描画高さ
			@param[in]	i	「Intensity」を考慮する場合「true」
		*/
		//-----------------------------------------------------------------//
		void fill_rect(short x, short y, short w, short h, bool i = false)
		{
			short ww = get_size().x;
			if((ww - x) < w) w = ww - x;
			short hh = get_size().y;
			if((hh - y) < h) h = hh - y;
			short rw = round_radius_;
			if(w < (rw + rw)) rw = w / 2;
			short rh = round_radius_;
			if(h < (rh + rh)) rh = h / 2;
			rgba8* p = at_image(y * ww);

			// 縦の勾配テーブル作成
			slopeT left;
			slopeT right;
			slopeT center;
			slopeT line;
			if(i) {
				left.resize(h);
				make_slope_(inten_rect_.left_top, inten_rect_.left_center, inten_rect_.left_bottom, left);
				center.resize(h);
				make_slope_(inten_rect_.center_top, inten_rect_.center_center, inten_rect_.center_bottom, center);
				right.resize(h);
				make_slope_(inten_rect_.right_top, inten_rect_.right_center, inten_rect_.right_bottom, right);

				line.resize(w);		// 横の勾配テーブル確保
			}

			for(int yy = 0; yy < h; ++yy) {
				int ox = 0;
				int ln = w;
				int al = 255;
				bool round = false;
				if(yy < rh) {
					ox  = round_offset_[yy] >> 8;
					ln -= ox;
					al = 255 - (round_offset_[yy] & 0xff);
					round = true;
				} else if(yy > (h - rh)) {
					ox  = round_offset_[h - yy - 1] >> 8;
					ln -= ox;
					al = 255 - (round_offset_[h - yy - 1] & 0xff);
					round = true;
				}
				if(i) {
					make_slope_(left[yy], center[yy], right[yy], line);
				}

				rgba8* dst = p + x + ox;
				for(int xx = ox; xx < ln; ++xx) {
					rgba8 c = fore_color_;
					if(i) {
						c.mod(line[xx]);
					}
					if(round) {
						if(xx == ox || xx == (ln - 1)) {
							c.a = al;
							uint16_t a = static_cast<uint16_t>(dst->a) * (256 - c.a);
							a += static_cast<uint16_t>(c.a) * (c.a + 1);
							a >>= 8;
							uint16_t r = static_cast<uint16_t>(dst->r) * (256 - a);
							uint16_t g = static_cast<uint16_t>(dst->g) * (256 - a);
							uint16_t b = static_cast<uint16_t>(dst->b) * (256 - a);
							r += static_cast<uint16_t>(c.r) * (a + 1);
							g += static_cast<uint16_t>(c.g) * (a + 1);
							b += static_cast<uint16_t>(c.b) * (a + 1);
							c.a = al;
							c.r = r >> 8;
							c.g = g >> 8;
							c.b = b >> 8;
						}
					}
					*dst++ = c;
				}
				p += ww;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで矩形領域を描画
			@param[in]	area	描画エリア
			@param[in]	i		「Intensity」を考慮する場合「true」
		*/
		//-----------------------------------------------------------------//
		void fill_rect(const vtx::srect& area, bool i = false)
		{
			fill_rect(area.org.x, area.org.y, area.end_x(), area.end_y(), i);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで矩形領域を描画
			@param[in]	i	「Intensity」を考慮する場合「true」
		*/
		//-----------------------------------------------------------------//
		void fill_rect(bool i = false) {
			fill_rect(0, 0, get_size().x, get_size().y, i);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーから「back」カラーで矩形領域を描画（水平）
			@param[in]	rect	描画領域
		*/
		//-----------------------------------------------------------------//
		void fill_rect_scale_h(const vtx::srect& rect)
		{
			auto fc = fore_color_;
			auto bc = back_color_;
			float gain = 1.0f;
			float gainadd = 1.0f / static_cast<float>(rect.size.x - 1);
			auto pos = rect.org;
			push_fore_color();
			for(int x = 0; x < rect.size.x; ++x) {
				uint8_t s = static_cast<uint8_t>(gain * 256.0f);
				fore_color_.r = ((fc.r * (256 - s)) + bc.r * s) >> 8;
				fore_color_.g = ((fc.g * (256 - s)) + bc.g * s) >> 8;
				fore_color_.b = ((fc.b * (256 - s)) + bc.b * s) >> 8;
				line_vertical(pos, rect.size.y);
				++pos.x;
				gain += gainadd;
			}
			pop_fore_color();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	グリッドを描画
			@param[in]	org		開始位置
			@param[in]	gs		グリッドサイズ
			@param[in]	num		グリッド個数
			@param[in]	lw		線幅
		*/
		//-----------------------------------------------------------------//
		void draw_grid(const vtx::spos& org, const vtx::spos& gs, const vtx::spos& num, const vtx::spos& lw)
		{
			vtx::spos size(gs.x * num.x + lw.x, gs.y * num.y + lw.y);
			{
				auto pos = org;
				for(short x = 0; x < num.x; ++x) {
					line_vertical(pos, size.y, lw.x);
					pos.x += gs.x;
				}
				line_vertical(pos, size.y, lw.x);
			}
			{
				auto pos = org;
				for(short y = 0; y < num.y; ++y) {
					line_holizontal(pos, size.x, lw.y);
					pos.y += gs.y;
				}
				line_holizontal(pos, size.x, lw.y);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの幅を取得
			@param[in]	wc	文字コード
			@return 幅
		*/
		//-----------------------------------------------------------------//
		int get_font_width(uint32_t wc)
		{
			img::ftimg::get_instance().create_bitmap(font_size_, wc);
			const img::ftimg::metrics met = img::ftimg::get_instance().get_metrics();
			return static_cast<int>(met.width + met.hori_x + font_space_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画
			@param[in]	txt	文字列
			@param[in]	x	X 開始位置
			@param[in]	y	Y 開始位置
			@return 描画幅
		*/
		//-----------------------------------------------------------------//
		int get_text_width(const std::string& txt) {
			utils::lstring ls;
			utils::utf8_to_utf32(txt, ls);
			int w = 0;
			BOOST_FOREACH(uint32_t lc, ls) {
				w += get_font_width(lc);
			}
			return w;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画
			@param[in]	pos	開始位置
			@param[in]	ch	文字コード
			@return 描画幅
		*/
		//-----------------------------------------------------------------//
		int draw_font(const vtx::spos& pos, uint32_t ch)
		{
			img::ftimg& fti = img::ftimg::get_instance();
			fti.create_bitmap(font_size_, ch);
			const img_gray8& gray = fti.get_img();

			const img::ftimg::metrics& met = fti.get_metrics();

			short w = static_cast<short>(met.width + met.hori_x);
			vtx::spos p;
			for(p.y = 0; p.y < gray.get_size().y; ++p.y) {
				for(p.x = 0; p.x < w; ++p.x) {
					gray8 g;
					gray.get_pixel(p, g);
					if(g.g > 0) {
						rgba8 c = fore_color_;
						c.a = (fore_color_.a * (g.g + 1)) >> 8;
						alpha_pixel(pos + p, c);
					}
				}
			}
			return w + font_space_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを描画
			@param[in]	pos	開始位置
			@param[in]	txt	文字列
			@return 描画幅
		*/
		//-----------------------------------------------------------------//
		int draw_text(const vtx::spos& pos, const std::string& txt) {
			utils::lstring ls;
			utils::utf8_to_utf32(txt, ls);
			int w = 0;
			vtx::spos p = pos;
			BOOST_FOREACH(uint32_t lc, ls) {
				int chw = draw_font(p, lc);
				w += chw;
				p.x += chw;
			}
			return w;
		}
	};
}

