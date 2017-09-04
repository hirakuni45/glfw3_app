#pragma once
//=====================================================================//
/*!	@file
	@brief	汎用ペイント・クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
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

		void make_round_tables_();
		void h_line_(const img::rgba8& c, const vtx::spos& pos, short len);
		void h_line_gray_(const img::rgba8& c, const vtx::spos& pos, short len);
		void make_scan_line_(const vtx::spos& st, const vtx::spos& en, int ofs, lines& lns);

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
		bool plot(const vtx::spos& p, const rgba8& c);


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
		void line(short x0, short y0, short x1, short y1);


		//-----------------------------------------------------------------//
		/*!
			@brief	「fore」カラーで線を描画
			@param[in]	xy0	始点
			@param[in]	xy1	終点
		*/
		//-----------------------------------------------------------------//
		void line(const vtx::spos& xy0, const vtx::spos& xy1) {
			line(xy0.x, xy0.y, xy1.x, xy1.y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	円を描画
			@param[in]	center	中心座標
			@param[in]	radius	半径
		*/
		//-----------------------------------------------------------------//
		void fill_circle(const vtx::spos& center, int radius);


		//-----------------------------------------------------------------//
		/*!
			@brief	ポリゴンを描画
			@param[in]	points	点集合
		*/
		//-----------------------------------------------------------------//
		void fill_polygon(const vtx::sposs& points);


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
		void fill_rect(short x, short y, short w, short h, bool i = false);


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
			@brief	フォントの幅を取得
			@param[in]	lc	文字コード
			@return 幅
		*/
		//-----------------------------------------------------------------//
		int get_font_width(uint32_t lc);


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
			@param[in]	lc	文字コード
			@return 描画幅
		*/
		//-----------------------------------------------------------------//
		int draw_font(const vtx::spos& pos, uint32_t lc);


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

