#pragma once
//=====================================================================//
/*!	@file
	@brief	@brief	OpenGL 関係、描画を行う関数など（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <map>
#include <cmath>
#include "core/glcore.hpp"
#include "gl_fw/gl_info.hpp"
#include "utils/vtx.hpp"
#include "img_io/i_img.hpp"
#include "img_io/img_rgba8.hpp"

namespace gl {

	// ab ベクトルの法泉方向に、r だけ内側のベクトルを計算する。
	static void inner_even_vector(const vtx::fpos_line& src, float rad, vtx::fpos_line& dst)
	{
		vtx::fpos dlt = src.t - src.s;
		float len = distance(src.s, src.t);
		float co  =  dlt.y / len;
		float si  = -dlt.x / len;
		vtx::fpos d(co * rad, si * rad);
		dst.s = src.s + d;
		dst.t = src.t + d;
	}


	static void convert_position_flip(const vtx::fposs& src, bool h_flip, bool v_flip, vtx::fposs& dst)
	{
		dst.clear();
		for(vtx::fposs_cit cit = src.begin(); cit != src.end(); ++cit) {
			vtx::fpos pos = *cit;
			if(h_flip) {
				pos.x = (1.0f - pos.x);
			}
			if(v_flip) {
				pos.y = (1.0f - pos.y);
			}
			dst.push_back(pos);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	二つの平面ベクトルの交点を求める
		@param[in]	a	ベクトル A
		@param[in]	b	ベクトル B
		@param[out]	pos	交点
		@return	「解」が無い場合、「false」が返る。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static bool check_cross_line(const vtx::fpos_line& a, const vtx::fpos_line& b, vtx::fpos& pos)
	{
		vtx::fpos ad = a.t - a.s;
		vtx::fpos bd = b.t - b.s;

		float det = bd.x * ad.y - bd.y * ad.x;
		if(det <= FLT_MIN && det >= -FLT_MIN) return false;

		vtx::fpos d = b.s - a.s;
		float t1 = (bd.x * d.y - bd.y * d.x) / det;
		float t2 = (ad.x * d.y - ad.y * d.x) / det;

		// この範囲を超えた場合は、線分の延長線上にある。
		if(0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f) ;
		else return false;

		pos = a.s + ad * t1;

		return true;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（カラー関係）
		@param[in]	c	rgba8 形式カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glColor(const img::rgba8& c) { glColor4ub(c.r, c.g, c.b, c.a); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（カラー関係）
		@param[in]	c	rgbaf 形式カラー
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glColor(const img::rgbaf& c) { glColor4f(c.r, c.g, c.b, c.a); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（移動関係）
		@param[in]	pos	二元位置
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glTranslate(const vtx::ipos& pos) { glTranslatei(pos.x, pos.y, 0); }
	inline void glTranslate(const vtx::spos& pos) { glTranslatei(pos.x, pos.y, 0); }
	inline void glTranslate(const vtx::fpos& pos) { glTranslatef(pos.x, pos.y, 0.0f); }
	inline void glTranslate(const vtx::dpos& pos) { glTranslated(pos.x, pos.y, 0.0); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（移動関係）
		@param[in]	pos	三元位置
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glTranslate(const vtx::ivtx& pos) { glTranslatei(pos.x, pos.y, pos.z); }
	inline void glTranslate(const vtx::svtx& pos) { glTranslatei(pos.x, pos.y, pos.z); }
	inline void glTranslate(const vtx::fvtx& pos) { glTranslatef(pos.x, pos.y, pos.z); }
	inline void glTranslate(const vtx::dvtx& pos) { glTranslated(pos.x, pos.y, pos.z); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（二元移動）
		@param[in]	x	X 位置
		@param[in]	y	Y 位置
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glTranslate(short x, short y) { glTranslatei(x, y, 0); }
	inline void glTranslate(int x, int y) { glTranslatei(x, y, 0); }
	inline void glTranslate(float x, float y) { glTranslatef(x, y, 0); }
	inline void glTranslate(double x, double y) { glTranslated(x, y, 0); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（三元移動）
		@param[in]	x	X 位置
		@param[in]	y	Y 位置
		@param[in]	z	Z 位置
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glTranslate(short x, short y, short z) { glTranslatei(x, y, z); }
	inline void glTranslate(int x, int y, int z) { glTranslatei(x, y, z); }
	inline void glTranslate(float x, float y, float z) { glTranslatef(x, y, z); }
	inline void glTranslate(double x, double y, double z) { glTranslated(x, y, z); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OpenGL サポート関数（スケーリング関係）
		@param[in]	s	スケールファクター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	inline void glScale(float s) { glScalef(s, s, s); }
	inline void glScale(double s) { glScaled(s, s, s); }
	inline void glScale(const vtx::fpos& s) { glScalef(s.x, s.y, 1.0f); }
	inline void glScale(const vtx::dpos& s) { glScaled(s.x, s.y, 1.0f); }
	inline void glScale(const vtx::fvtx& s) { glScalef(s.x, s.y, s.z); }
	inline void glScale(const vtx::dvtx& s) { glScaled(s.x, s.y, s.z); }


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	２次元単純構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct v2_t {
		T	x, y;
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	３次元単純構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct v3_t {
		T	x, y, z;
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	４次元単純構造体
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	struct v4_t {
		T	x, y, z, w;
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	v2_t テーブルから、fposs へ初期化
		@param[in]	src	v2_t 型ソース
		@param[in]	n	数
		@param[in]	dst	pos 出力列
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T>
	void init_fposs(const v2_t<T>* src, int n, vtx::fposs& dst) {
		for(int i = 0; i < n; ++i) {
			dst.push_back(vtx::fpos(static_cast<float>(src->x), static_cast<float>(src->y)));
			src++;
		}
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	v3_t テーブルから、fvtxs へ初期化
		@param[in]	src	v3_t 型ソース
		@param[in]	n	数
		@param[in]	dst	pos 出力列
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T>
	void init_fvtxs(const v3_t<T>* src, int n, vtx::fvtxs& dst) {
		for(int i = 0; i < n; ++i) {
			dst.push_back(vtx::fvtx(static_cast<float>(src->x), static_cast<float>(src->y), static_cast<float>(src->z)));
			src++;
		}
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ポリゴン列から、トライアングル・ストリップ列に変換する@n
				テンプレート関数
		@param[in]	src	入力列（STL vector）
		@param[in]	dst	出力列（STL vector）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T>
	uint32_t polygon2triangleStrip(const T& src, T& dst) {
		dst.clear();

		uint32_t num = src.size();
		for(uint32_t i = 0; i < num; ++i) {
			if(i == 0) {
				dst.push_back(src[1]);
			} else if(i == 1) {
				dst.push_back(src[0]);
			} else {
				uint32_t n = ((i - 2) & 65535) >> 1;
				if(i & 1) {
					dst.push_back(src[num - 1 - n]);
				} else {
					dst.push_back(src[2 + n]);
				}
			}
		}
		return num;
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴン列から、トライアングル・ストリップ列に変換する
		@param[in]	src	入力列
		@param[out]	dst	出力列
	*/
	//-----------------------------------------------------------------//
	inline uint32_t polygon_to_triangle_strip(const vtx::sposs& src, vtx::sposs& dst) {
		return polygon2triangleStrip<vtx::sposs>(src, dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴン列から、トライアングル・ストリップ列に変換する
		@param[in]	src	入力列
		@param[out]	dst	出力列
	*/
	//-----------------------------------------------------------------//
	inline uint32_t polygon_to_triangle_strip(const vtx::fposs& src, vtx::fposs& dst) {
		return polygon2triangleStrip<vtx::fposs>(src, dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴン列から、トライアングル・ストリップ列に変換する
		@param[in]	src	入力列
		@param[out]	dst	出力列
	*/
	//-----------------------------------------------------------------//
	inline uint32_t polygon_to_triangle_strip(const vtx::fvtxs& src, vtx::fvtxs& dst)
	{
		return polygon2triangleStrip<vtx::fvtxs>(src, dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（short）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	static void draw_line(const vtx::spos& a, const vtx::spos& b)
	{
		vtx::spos lines[2];
		lines[0] = a;
		lines[1] = b;
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &lines[0]);
		::glDrawArrays(GL_LINES, 0, 2);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（float）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	static void draw_line(const vtx::fpos& a, const vtx::fpos& b)
	{
		vtx::fpos lines[2];
		lines[0] = a;
		lines[1] = b;
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &lines[0]);
		::glDrawArrays(GL_LINES, 0, 2);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（float）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	static void draw_line(const vtx::fvtx& a, const vtx::fvtx& b)
	{
		vtx::fvtx lines[2];
		lines[0] = a;
		lines[1] = b;
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &lines[0]);
		::glDrawArrays(GL_LINES, 0, 2);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画（short）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_lines(const vtx::sposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &list[0]);
		::glDrawArrays(GL_LINES, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_lines(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINES, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_lines(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINES, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_loop(const vtx::sposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &list[0]);
		::glDrawArrays(GL_LINE_LOOP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_loop(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_LOOP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_strip(const vtx::sposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &list[0]);
		::glDrawArrays(GL_LINE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_strip(const vtx::iposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_INT, 0, &list[0]);
		::glDrawArrays(GL_LINE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_strip(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_loop(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_LOOP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_line_strip(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ファンの描画（sort）
		@param[in]	list	２頂点列（最初が中心）
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_fan(const vtx::sposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &list[0]);
		::glDrawArrays(GL_TRIANGLE_FAN, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ファンの描画（float）
		@param[in]	list	２頂点列（最初が中心）
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_fan(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_TRIANGLE_FAN, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（short）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_strip(const vtx::sposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &list[0]);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_strip(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_strip(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	vlist	３頂点列
		@param[in]	tlist	テクスチャーコーディネート列
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fposs& tlist)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glTexCoordPointer(2, GL_FLOAT, 0, &tlist[0]);
		::glVertexPointer(3, GL_FLOAT, 0, &vlist[0]);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, vlist.size());
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	vlist	３頂点列
		@param[in]	nlist	法線列
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist)
	{
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glNormalPointer(GL_FLOAT, 0, &nlist[0]);
		::glVertexPointer(3, GL_FLOAT, 0, &vlist[0]);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, vlist.size());
		::glDisableClientState(GL_NORMAL_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	vlist	３頂点列
		@param[in]	nlist	法線列
		@param[in]	tlist	テクスチャーコーディネート列
	*/
	//-----------------------------------------------------------------//
	static void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist, const vtx::fposs& tlist)
	{
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glNormalPointer(GL_FLOAT, 0, &nlist[0]);
		::glTexCoordPointer(2, GL_FLOAT, 0, &tlist[0]);
		::glVertexPointer(3, GL_FLOAT, 0, &vlist[0]);
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, vlist.size());
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDisableClientState(GL_NORMAL_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_polygon(const vtx::sposs& list)
	{
		vtx::sposs	out;
		polygon_to_triangle_strip(list, out);
		draw_triangle_strip(out);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_polygon(const vtx::fposs& list)
	{
		vtx::fposs	out;
		polygon_to_triangle_strip(list, out);
		draw_triangle_strip(out);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	static void draw_polygon(const vtx::fvtxs& list)
	{
		vtx::fvtxs out;
		polygon_to_triangle_strip(list, out);
		draw_triangle_strip(out);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画
		@param[in]	vlist		３頂点列
		@param[in]	nlist		法線リスト
	*/
	//-----------------------------------------------------------------//
	static void draw_polygon(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist)
	{
		vtx::fvtxs vout;
		polygon_to_triangle_strip(vlist, vout);
		vtx::fvtxs nout;
		polygon_to_triangle_strip(nlist, nout);
		draw_triangle_strip(vout, nout);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画
		@param[in]	vlist		３頂点列
		@param[in]	tlist		テクスチャーコーディネートリスト
	*/
	//-----------------------------------------------------------------//
	static void draw_polygon(const vtx::fvtxs& vlist, const vtx::fposs& tlist)
	{
		vtx::fvtxs vout;
		polygon_to_triangle_strip(vlist, vout);
		vtx::fposs tout;
		polygon_to_triangle_strip(tlist, tout);
		draw_triangle_strip(vout, tout);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画
		@param[in]	vlist		３頂点列
		@param[in]	nlist		法線リスト
		@param[in]	tlist		テクスチャーコーディネートリスト
	*/
	//-----------------------------------------------------------------//
	static void draw_polygon(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist, const vtx::fposs& tlist)
	{
		vtx::fvtxs vout;
		polygon_to_triangle_strip(vlist, vout);
		vtx::fvtxs nout;
		polygon_to_triangle_strip(nlist, nout);
		vtx::fposs tout;
		polygon_to_triangle_strip(tlist, tout);
		draw_triangle_strip(vout, nout, tout);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_line_circle(const vtx::spos& pos, int rad, int step = 8)
	{
		vtx::sposs list;
		float a = vtx::radian_f_ / static_cast<float>(step);
		for(int i = 0; i < step; ++i) {
			short si = static_cast<short>(std::sin(a * static_cast<float>(i)) * static_cast<float>(rad));
			short co = static_cast<short>(std::cos(a * static_cast<float>(i)) * static_cast<float>(rad));
			vtx::spos p(pos.x + co, pos.y - si);
			list.push_back(p);
		}
		draw_line_loop(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	円を描画する
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_line_circle(const vtx::fpos& pos, float rad, int step = 8)
	{
		vtx::fposs list;
		float a = vtx::radian_f_ / static_cast<float>(step);
		for(int i = 0; i < step; ++i) {
			float si = std::sin(a * static_cast<float>(i)) * rad;
			float co = std::cos(a * static_cast<float>(i)) * rad;
			vtx::fpos p(pos.x + co, pos.y - si);
			list.push_back(p);
		}
		draw_line_loop(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_circle(const vtx::spos& pos, int rad, int step = 8)
	{
		vtx::sposs list;
		list.push_back(pos);
		float a = vtx::radian_f_ / static_cast<float>(step);
		for(int i = 0; i < step; ++i) {
			short si = static_cast<short>(std::sin(a * static_cast<float>(i)) * static_cast<float>(rad));
			short co = static_cast<short>(std::cos(a * static_cast<float>(i)) * static_cast<float>(rad));
			vtx::spos p(pos.x + co, pos.y - si);
			list.push_back(p);
		}
		list.push_back(list[1]);
		draw_triangle_fan(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	円（塗りつぶされた）を描画する（float）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_circle(const vtx::fpos& pos, float rad, int step = 8)
	{
		vtx::fposs list;
		float a = vtx::radian_f_ / static_cast<float>(step);
		list.push_back(pos);
		for(int i = 0; i < step; ++i) {
			float si = std::sin(a * static_cast<float>(i)) * rad;
			float co = std::cos(a * static_cast<float>(i)) * rad;
			vtx::fpos p(pos.x + co, pos.y - si);
			list.push_back(p);
		}
		list.push_back(list[1]);
		draw_triangle_fan(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされ円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	orad	外径
		@param[in]	irad	内径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_ring(const vtx::spos& pos, int orad, int irad, int step = 8)
	{
		vtx::sposs list;
		float a = vtx::radian_f_ / static_cast<float>(step);
		for(int i = 0; i < step; ++i) {
			float si = std::sin(a * static_cast<float>(i));
			float co = std::cos(a * static_cast<float>(i));
			float r = static_cast<float>(irad);
			vtx::spos p(pos.x + static_cast<short>(co * r), pos.y - static_cast<short>(si * r));
			list.push_back(p);
			r = static_cast<float>(orad);
			p.set(pos.x + static_cast<short>(co * r), pos.y - static_cast<short>(si * r));
			list.push_back(p);
		}
		list.push_back(list[0]);
		list.push_back(list[1]);
		draw_triangle_strip(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた円を描画する（float）
		@param[in]	pos		中心座標
		@param[in]	orad	外径
		@param[in]	irad	内径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_ring(const vtx::fpos& pos, float orad, float irad, int step = 8)
	{
		vtx::fposs list;
		float a = vtx::radian_f_ / static_cast<float>(step);
		for(int i = 0; i < step; ++i) {
			float si = std::sin(a * static_cast<float>(i));
			float co = std::cos(a * static_cast<float>(i));
			vtx::fpos p(pos.x + co * irad, pos.y - si * irad);
			list.push_back(p);
			p.set(pos.x + co * orad, pos.y - si * orad);
			list.push_back(p);
		}
		list.push_back(list[0]);
		list.push_back(list[1]);
		draw_triangle_strip(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	三角を描画する
		@param[in]	rect	位置、大きさ
		@param[in]	dir		角度（0, 90, 180, 270)
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_arrow(const vtx::srect& rect, short dir)
	{
		vtx::sposs list;
		switch(dir) {
		case 0:
			list.push_back(vtx::spos(rect.center_x(), rect.org.y));
			list.push_back(vtx::spos(rect.org.x, rect.end_y()));
			list.push_back(rect.end());
			break;
		case 90:
			list.push_back(vtx::spos(rect.end_x(), rect.center_y()));
			list.push_back(rect.org);
			list.push_back(vtx::spos(rect.org.x, rect.end_y()));
			break;
		case 180:
			list.push_back(vtx::spos(rect.center_x(), rect.end_y()));
			list.push_back(vtx::spos(rect.end_x(), rect.org.y));
			list.push_back(rect.org);
			break;
		case 270:
			list.push_back(vtx::spos(rect.org.x, rect.center_y()));
			list.push_back(rect.end());
			list.push_back(vtx::spos(rect.end_x(), rect.org.y));
			break;
		default:
			break;
		}
		draw_triangle_strip(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
	*/
	//-----------------------------------------------------------------//
	static void draw_line_rectangle(const vtx::fpos& pos, const vtx::fpos& size)
	{
		vtx::fposs list;
		vtx::fpos ofs;

		ofs.set(0.0f, 0.0f);
		list.push_back(pos + ofs);
		ofs.set(0.0f, size.y);
		list.push_back(pos + ofs);
		ofs.set(size.x, size.y);
		list.push_back(pos + ofs);
		ofs.set(size.x, 0.0f);
		list.push_back(pos + ofs);
		draw_line_loop(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、四角を描画する
		@param[in]	rect	短形
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_rectangle(const vtx::srect& rect)
	{
		vtx::spos vec[4];
		vec[0].set(rect.org.x, rect.end_y());
		vec[1].set(rect.org.x, rect.org.y);
		vec[2].set(rect.end_x(), rect.end_y());
		vec[3].set(rect.end_x(), rect.org.y);

		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, vec[0].getXY());
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_rectangle(const vtx::fpos& pos, const vtx::fpos& size)
	{
		vtx::fpos vec[4];
		vec[0].set(pos.x, pos.y + size.y);
		vec[1].set(pos.x, pos.y);
		vec[2].set(pos.x + size.x, pos.y + size.y);
		vec[3].set(pos.x + size.x, pos.y);

		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, vec[0].getXY());
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
		@param[in]	hflip	テクスチャー左右反転
		@param[in]	vflip	テクスチャー上下反転
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_rectangle(const vtx::fpos& pos, const vtx::fpos& size, bool hflip, bool vflip)
	{
		float	tu, tv, bu, bv;
		if(hflip) {
			tu = 1.0f; bu = 0.0f;
		} else {
			tu = 0.0f; bu = 1.0f;
		}
		if(vflip) {
			tv = 1.0f; bv = 0.0f;
		} else {
			tv = 0.0f; bv = 1.0f;
		}
		vtx::fpos tex[4];
		vtx::fpos vec[4];
		tex[0].set(tu, bv);
		vec[0].set(pos.x, pos.y + size.y);
		tex[1].set(tu, tv);
		vec[1].set(pos.x, pos.y);
		tex[2].set(bu, bv);
		vec[2].set(pos.x + size.x, pos.y + size.y);
		tex[3].set(bu, tv);
		vec[3].set(pos.x + size.x, pos.y);

		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glTexCoordPointer(2, GL_FLOAT, 0, tex[0].getXY());
		::glVertexPointer(2, GL_FLOAT, 0, vec[0].getXY());
		::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	エッジ付き短型を描画
		@param[in]	rect	短型
		@param[in]	ew		エッジの幅
		@param[in]	c		基本カラー
	 */
	//-----------------------------------------------------------------//
	static void draw_filled_rectangle_with_edge(const vtx::srect& rect, int ew, const img::rgbaf& c)
	{
		float left_i;
		float right_i;
		if(ew < 0) {
			left_i = 1.0f;
			right_i = 0.5f;
			ew = -ew;
		} else {
			left_i = 0.5f;
			right_i = 1.0f;
		}

		glColor(c * 0.75f);
		draw_filled_rectangle(vtx::srect(rect.org + 2, rect.size - 4));

		vtx::spos l_b(rect.org.x, rect.end_y());
		vtx::spos r_t(rect.end_x(), rect.org.y);
		vtx::spos r_b(rect.end_x(), rect.end_y());
		vtx::sposs list;

		glColor(c * left_i);
		list.push_back(rect.org);
		list.push_back(l_b);
		list.push_back(vtx::spos(rect.org.x + ew, l_b.y - ew));
		list.push_back(vtx::spos(rect.org.x + ew, rect.org.y + ew));
		draw_polygon(list);

		list.clear();
		list.push_back(l_b);
		list.push_back(r_b);
		list.push_back(vtx::spos(r_b.x - ew, r_b.y - ew));
		list.push_back(vtx::spos(l_b.x + ew, l_b.y - ew));
		draw_polygon(list);

		glColor(c * right_i);

		list.clear();
		list.push_back(r_b);
		list.push_back(r_t);
		list.push_back(vtx::spos(r_t.x - ew, r_t.y + ew));
		list.push_back(vtx::spos(r_b.x - ew, r_b.y - ew));
		draw_polygon(list);

		list.clear();
		list.push_back(r_t);
		list.push_back(rect.org);
		list.push_back(vtx::spos(rect.org.x + ew, rect.org.y + ew));
		list.push_back(vtx::spos(r_t.x - ew, r_t.y + ew));
		draw_polygon(list);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ワイヤーフレームシリンダーの描画
		@param[in]	length		長さ
		@param[in]	radius	半径
		@param[in]	rstep	半径の分割数
	*/
	//-----------------------------------------------------------------//
	static void draw_line_cylinder(float length, float radius, int rstep)
	{
		vtx::fvtxs listm;
		vtx::fvtxs listp;
		float gain = vtx::radian_f_ / static_cast<float>(rstep);
		for(int i = 0; i < rstep; ++i) {
			float si = std::sin(gain * static_cast<float>(i)) * radius;
			float co = std::cos(gain * static_cast<float>(i)) * radius;
			vtx::fvtx pm(co, si, -length);
			listm.push_back(pm);
			vtx::fvtx pp(co, si,  length);
			listp.push_back(pp);
			draw_line(pm, pp);
		}
		draw_line_loop(listm);
		draw_line_loop(listp);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、角が丸い四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
		@param[in]	radian	ラウンド半径
		@param[in]	rstep	ラウンドの分割数（１回転に対する分割数）
	*/
	//-----------------------------------------------------------------//
	static void draw_filled_round_rectangle(const vtx::fpos& pos, const vtx::fpos& size, float radian, int rstep)
	{


//		create_round_line(const vtx::fposs& src, float radian, int rstep, vtx::fposs& dst);


	}


	//-----------------------------------------------------------------//
	/*!
		@brief	正立体の描画
		@param[in]	s	サイズ
	*/
	//-----------------------------------------------------------------//
	static void draw_square(float s)
	{
		::glPushMatrix();
		::glScalef(s, s, s);

		vtx::fvtxs v;
		v.push_back(vtx::fvtx(-1.0f,  1.0f, -1.0f));
		v.push_back(vtx::fvtx( 1.0f,  1.0f, -1.0f));
		v.push_back(vtx::fvtx( 1.0f, -1.0f, -1.0f));
		v.push_back(vtx::fvtx(-1.0f, -1.0f, -1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(vtx::fvtx(-1.0f, -1.0f, -1.0f));
		v.push_back(vtx::fvtx( 1.0f, -1.0f, -1.0f));
		v.push_back(vtx::fvtx( 1.0f, -1.0f,  1.0f));
		v.push_back(vtx::fvtx(-1.0f, -1.0f,  1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(vtx::fvtx(-1.0f,  1.0f,  1.0f));
		v.push_back(vtx::fvtx(-1.0f, -1.0f,  1.0f));
		v.push_back(vtx::fvtx( 1.0f, -1.0f,  1.0f));
		v.push_back(vtx::fvtx( 1.0f,  1.0f,  1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(vtx::fvtx(-1.0f,  1.0f,  1.0f));
		v.push_back(vtx::fvtx( 1.0f,  1.0f,  1.0f));
		v.push_back(vtx::fvtx( 1.0f,  1.0f, -1.0f));
		v.push_back(vtx::fvtx(-1.0f,  1.0f, -1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(vtx::fvtx(-1.0f,  1.0f,  1.0f));
		v.push_back(vtx::fvtx(-1.0f,  1.0f, -1.0f));
		v.push_back(vtx::fvtx(-1.0f, -1.0f, -1.0f));
		v.push_back(vtx::fvtx(-1.0f, -1.0f,  1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(vtx::fvtx( 1.0f,  1.0f, -1.0f));
		v.push_back(vtx::fvtx( 1.0f,  1.0f,  1.0f));
		v.push_back(vtx::fvtx( 1.0f, -1.0f,  1.0f));
		v.push_back(vtx::fvtx( 1.0f, -1.0f, -1.0f));
		draw_polygon(v);

		::glPopMatrix();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	グリッドを描画
		@param[in]	start	スタート
		@param[in]	end		エンド
		@param[in]	d	変化量
	*/
	//-----------------------------------------------------------------//
	static void draw_grid(const vtx::fpos& start, const vtx::fpos& end, const vtx::fpos& d)
	{
		::glLineWidth(1.0f);

		vtx::fposs vs;
		{
			float step = start.y;
			int yy = static_cast<int>((end.y - start.y) / d.y);
			for(int y = 0; y <= yy; ++y) {
				vs.push_back(vtx::fpos(start.x, step));
				vs.push_back(vtx::fpos(end.x, step));
				step += d.y;
			}
		}
		{
			float step = start.x;
			int xx = static_cast<int>((end.x - start.x) / d.x);
			for(int x = 0; x <= xx; ++x) {
				vs.push_back(vtx::fpos(step, start.y));
				vs.push_back(vtx::fpos(step, end.y));
				step += d.x;
			}
		}
		draw_lines(vs);

		if(0) {
			::glLineWidth(2.0f);
			vs.clear();
			float i = (start.x + end.x) * 0.5f;
			float j = (start.y + end.y) * 0.5f;
			vs.push_back(vtx::fpos(i, j));
			vs.push_back(vtx::fpos(i + d.x, j));
			vs.push_back(vtx::fpos(i, j));
			vs.push_back(vtx::fpos(i, j + d.y));
		}
	}


	static void subdivision_line(const vtx::fvtxs& src, float factor, vtx::fvtxs& dst)
	{
		if(src.size() < 2) return;

		vtx::fvtxs tmp;
		for(unsigned int i = 0; i < (src.size() - 1); ++i) {
			vtx::fvtx c;
			factor_line(src[i], 0.5f, src[i + 1], c);
			tmp.push_back(c);
		}
		for(unsigned int i = 0; i < src.size(); ++i) {
			if(i > 0 && i < (src.size() - 1)) {
				vtx::fvtx c;
				factor_line(tmp[i - 1], 0.5f, tmp[i], c);
				factor_line(c, 0.5f, src[i], c);
				dst.push_back(c);
			} else {
				dst.push_back(src[i]);
			}
			if(i < (src.size() - 1)) {
				dst.push_back(tmp[i]);
			}
		}
	}


	static void subdivision_line(const vtx::fvtxs& src, float factor, vtx::fvtxs& dst, int n)
	{
		if(n == 0) {
			dst = src;
			return;
		}
		vtx::fvtxs tmp = src;
		for(int i = 0; i < n; ++i) {
			if(i) tmp = dst;
			dst.clear();
			subdivision_line(tmp, factor, dst);
		}
	}


	static void draw_bounding_box(const vtx::fvtx& min, const vtx::fvtx& max)
	{
		vtx::fvtxs v;
		v.push_back(vtx::fvtx(min.x, max.y, max.z));
		v.push_back(vtx::fvtx(max.x, max.y, max.z));
		v.push_back(vtx::fvtx(max.x, min.y, max.z));
		v.push_back(vtx::fvtx(min.x, min.y, max.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(vtx::fvtx(min.x, max.y, min.z));
		v.push_back(vtx::fvtx(max.x, max.y, min.z));
		v.push_back(vtx::fvtx(max.x, max.y, max.z));
		v.push_back(vtx::fvtx(min.x, max.y, max.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(vtx::fvtx(min.x, max.y, min.z));
		v.push_back(vtx::fvtx(min.x, min.y, min.z));
		v.push_back(vtx::fvtx(max.x, min.y, min.z));
		v.push_back(vtx::fvtx(max.x, max.y, min.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(vtx::fvtx(min.x, min.y, min.z));
		v.push_back(vtx::fvtx(min.x, min.y, max.z));
		v.push_back(vtx::fvtx(max.x, min.y, max.z));
		v.push_back(vtx::fvtx(max.x, min.y, min.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(vtx::fvtx(max.x, min.y, max.z));
		v.push_back(vtx::fvtx(max.x, max.y, max.z));
		v.push_back(vtx::fvtx(max.x, max.y, min.z));
		v.push_back(vtx::fvtx(max.x, min.y, min.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(vtx::fvtx(min.x, min.y, min.z));
		v.push_back(vtx::fvtx(min.x, max.y, min.z));
		v.push_back(vtx::fvtx(min.x, max.y, max.z));
		v.push_back(vtx::fvtx(min.x, min.y, max.z));
		draw_line_loop(v);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	position 列から vertex 列への変換
		@param[in]	src	ソース・リスト
		@param[in]	z	変換の際設定する z の値
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void convert_position_to_vertex(const vtx::fposs& src, float z, vtx::fvtxs& dst)
	{
		for(vtx::fposs_cit cit = src.begin(); cit != src.end(); ++cit) {
			vtx::fvtx v((*cit).x, (*cit).y, z);
			dst.push_back(v);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	vertex 列から position 列への変換（ Z メンバーは無視される）
		@param[in]	src	ソース・リスト
		@param[in]	offset	オフセット
		@param[in]	scale	スケール
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void convert_vertex_to_position(const vtx::fvtxs& src, const vtx::fpos& offset, const vtx::fpos& scale, vtx::fposs& dst)
	{
		for(vtx::fvtxs_cit cit = src.begin(); cit != src.end(); ++cit) {
			vtx::fpos v((*cit).x, (*cit).y);
			v -= offset;
			v *= scale;
			dst.push_back(v);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	長方形座標列を生成する（時計周り）
		@param[in]	pos		開始点
		@param[in]	size	大きさ
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void create_rectangle(const vtx::fpos& pos, const vtx::fpos& size, vtx::fposs& dst)
	{
		vtx::fpos min;
		vtx::fpos max;
		min = pos;
		max = pos + size;
		dst.push_back(vtx::fpos(min.x, max.y));
		dst.push_back(vtx::fpos(max.x, max.y));
		dst.push_back(vtx::fpos(max.x, min.y));
		dst.push_back(vtx::fpos(min.x, min.y));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	長方形座標列を生成する（時計周り）
		@param[in]	pos		開始点
		@param[in]	size	大きさ
		@param[in]	z		Z 値
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void create_rectangle(const vtx::fpos& pos, const vtx::fpos& size, float z, vtx::fvtxs& dst)
	{
		vtx::fpos min;
		vtx::fpos max;
		min = pos;
		max = pos + size;
		dst.push_back(vtx::fvtx(min.x, max.y, z));
		dst.push_back(vtx::fvtx(max.x, max.y, z));
		dst.push_back(vtx::fvtx(max.x, min.y, z));
		dst.push_back(vtx::fvtx(min.x, min.y, z));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	二つのラインから、トライアングル・ストリップ列に変換する
		@param[in]	inn		内側のライン
		@param[in]	inn_z	内側のラインの Z 値
		@param[in]	out		外側のライン
		@param[in]	out_z	外側のラインの Z 値
		@param[out]	dst	出力列
	*/
	//-----------------------------------------------------------------//
	static void double_line_to_triangle_strip(const vtx::fposs& inn, float inn_z, const vtx::fposs& out, float out_z, vtx::fvtxs& dst)
	{
		vtx::fposs_cit inn_cit = inn.begin();
		vtx::fposs_cit out_cit = out.begin();
		vtx::fvtx v;
		for(int i = 0; i < (int)inn.size(); ++i) {
			v.set((*out_cit).x, (*out_cit).y, out_z);
			dst.push_back(v);
			v.set((*inn_cit).x, (*inn_cit).y, inn_z);
			dst.push_back(v);
			++out_cit;
			++inn_cit;
		}
		out_cit = out.begin();
		v.set((*out_cit).x, (*out_cit).y, out_z);
		dst.push_back(v);
		inn_cit = inn.begin();
		v.set((*inn_cit).x, (*inn_cit).y, inn_z);
		dst.push_back(v);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・マップの為のuvコーディネートを求める
		@param[in]	srcc	中心座標
		@param[in]	srcv	頂点法線座標列
		@param[out]	uvs		UV 座標列
	*/
	//-----------------------------------------------------------------//
	static void create_sphere_uv(const vtx::fvtx& srcc, const vtx::fvtxs& srcv, vtx::fposs& uvs)
	{
		uvs.clear();

		for(auto cit = srcv.begin(); cit != srcv.end(); ++cit) {
			vtx::fvtx v = *cit;
			v -= srcc;
			vtx::fpos uv(0);
			uvs.push_back(uv);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・マップの為のuvコーディネートを求める
		@param[in]	srcn	頂点法線座標列
		@param[out]	uvs		UV 座標列
	*/
	//-----------------------------------------------------------------//
	static void create_circle_uv(const vtx::fvtxs& srcn, vtx::fposs& uvs)
	{
		uvs.clear();

		for(auto cit = srcn.begin(); cit != srcn.end(); ++cit) {
			const vtx::fvtx& n = *cit;
			float m = 2.0f * sqrtf(n.sqrY() + n.sqrY() + (n.z + 1.0f) * (n.z + 1.0f));
			vtx::fpos uv(n.x / m + 0.5f, n.y / m + 0.5f);
			uvs.push_back(uv);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・マップの為のuvコーディネートを求める（リフレクション）
		@param[in]	eye		視線ベクトル
		@param[in]	srcn	頂点法線座標列
		@param[out]	uvs		UV 座標列
	*/
	//-----------------------------------------------------------------//
	static void create_reflection_uv(const vtx::fvtx& eye, const vtx::fvtxs& srcn, vtx::fposs& uvs)
	{
		uvs.clear();

		vtx::fvtx ieye = eye * -1.0f;
		for(auto cit = srcn.begin(); cit != srcn.end(); ++cit) {
			const vtx::fvtx& n = *cit;
			float in = 2.0f * dot(ieye, n);
			vtx::fvtx nn = n * in;
			nn -= ieye;
			float m = 2.0f * sqrtf(nn.sqrX() + nn.sqrY() + (nn.z + 1.0f) * (nn.z + 1.0f));
			vtx::fpos uv(nn.x / m + 0.5f, nn.y / m + 0.5f);
			uvs.push_back(uv);
		}
	}


#if defined(IPHONE_EMU) || defined(IPAD_EMU)
	static float g_matrix_scale = 1.0f;

	//-----------------------------------------------------------------//
	/*!
		@brief iPhone 用マトリックス・スケールの設定
		@param[in]	scale	スケール・ファクター
	 */
	//-----------------------------------------------------------------//
	static void set_iphone_matrix_scale(float scale)
	{
		g_matrix_scale = scale;
	}
#endif

	//-----------------------------------------------------------------//
	/*!
		@brief iPhone 用マトリックスの設定
		@param[in]	width	論理横幅
		@param[in]	height	論理高さ
		@param[in]	zn		Z near
		@param[in]	zf		Z far
	 */
	//-----------------------------------------------------------------//
	static void setup_iphone_matrix(int width, int height, float zn, float zf)
	{
#ifdef IPHONE_IPAD
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		GLfloat ww = static_cast<GLfloat>(width / 2);
		GLfloat hh = static_cast<GLfloat>(height / 2);
		IGLcore* igl = get_glcore(); 
		GLfloat scww = static_cast<float>(igl->get_width())  * 0.5f;
		GLfloat schh = static_cast<float>(igl->get_height()) * 0.5f;
		glOrthof(-scww, scww, schh, -schh, zn, zf);
		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();

		static float g_current_angle = 0.0f;

		if(get_iphone_gravity_type() == IPHONE_GRAVITY_LEFT) {
			if(g_current_angle < 90.0f) {
				g_current_angle += 9.0f;
			} else {
				g_current_angle = 90.0f;
			}
		} else if(get_iphone_gravity_type() == IPHONE_GRAVITY_RIGHT) {
			if(g_current_angle > -90.0f) {
				g_current_angle -= 9.0f;
			} else {
				g_current_angle = -90.0f;
			}
		} else {
			if(g_current_angle < 90.0f) {
				g_current_angle += 9.0f;
			} else {
				g_current_angle = 90.0f;
			}
		}
		::glRotatef(g_current_angle, 0.0f, 0.0f, 1.0f);
		::glScalef(g_matrix_scale, g_matrix_scale, 1.0f);
		::glTranslatef(-ww, -hh, 0.0f);
#endif

#ifdef IPHONE_EMU
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		GLfloat ww = static_cast<GLfloat>(width / 2);
		GLfloat hh = static_cast<GLfloat>(height / 2);
		GLfloat scww = 480.0f * 0.5f;
		GLfloat schh = 320.0f * 0.5f;
		glOrthof(-scww, scww, schh, -schh, zn, zf);
		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();

		::glScalef(g_matrix_scale, g_matrix_scale, 1.0f);
		::glTranslatef(-ww, -hh, 0.0f);
#endif

#ifdef IPAD_EMU
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		GLfloat ww = static_cast<GLfloat>(width / 2);
		GLfloat hh = static_cast<GLfloat>(height / 2);
		GLfloat scww = 1024.0f * 0.5f;
		GLfloat schh = 768.0f * 0.5f;
		glOrthof(-scww, scww, schh, -schh, zn, zf);
		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();

		::glScalef(g_matrix_scale, g_matrix_scale, 1.0f);
		::glTranslatef(-ww, -hh, 0.0f);
#endif
	}


#ifdef IPHONE_IPAD
	enum {
		IPHONE_GRAVITY_NONE,
		IPHONE_GRAVITY_TOP,
		IPHONE_GRAVITY_LEFT,
		IPHONE_GRAVITY_RIGHT,
		IPHONE_GRAVITY_BOTTOM,
	};


	static bool g_gravity_lock = true;
	static int g_gravity_type = IPHONE_GRAVITY_NONE;

	//-----------------------------------------------------------------//
	/*!
		@brief	iPhone の重力タイプを返す
		@return	重力タイプ
	 */
	//-----------------------------------------------------------------//	
	static int get_iphone_gravity_type()
	{
		IGLcore* igl = get_glcore();
		if(igl) {
			if(g_gravity_lock) {
				return g_gravity_type;
			}
			const gldev& dev = igl->get_device();
			const fvtx& v = dev.get_gravity();
			if(v.x < -0.3f) {
				g_gravity_type = IPHONE_GRAVITY_LEFT;
				return IPHONE_GRAVITY_LEFT;
			}
			else if(v.x > 0.3f) {
				g_gravity_type = IPHONE_GRAVITY_RIGHT;
				return IPHONE_GRAVITY_RIGHT;
			} else {
				return g_gravity_type;
			}
		}
		return IPHONE_GRAVITY_NONE;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	重力ロックを設定する
		@param[in]	重力応答をロックする場合「true」
	 */
	//-----------------------------------------------------------------//
	static void set_gravity_lock(bool f = true)
	{
		g_gravity_lock = f;
	}
#endif

#ifndef OPENGL_ES
	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・バッファを画像を取得する。
		@param[in]	x	開始位置 X
		@param[in]	y	開始位置 Y
		@param[in]	w	幅
		@param[in]	h	高さ
		@return 共有画像
	 */
	//-----------------------------------------------------------------//
	static img::shared_img get_frame_buffer(int x, int y, int w, int h)
	{
		gl::core& core = gl::core::get_instance();
		int fbh = core.get_size().y;
		auto sim = img::shared_img(new img::img_rgba8);
		sim->create(vtx::spos(w, h), false);
//		::glReadBuffer(GL_BACK);
		::glReadBuffer(GL_FRONT);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for(int yy = 0; yy < h; ++yy) {
			img::rgba8 tmp[w];
			::glReadPixels(x, fbh - 1 - y - yy, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
			for(int xx = 0; xx < w; ++xx) {
				tmp[xx].a = 255;
				sim->put_pixel(vtx::spos(xx, yy), tmp[xx]);
			}
		}
		return sim;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・バッファを画像を取得する。(RGB RAW)
		@param[in]	dst	転送先
		@param[in]	x	開始位置 X
		@param[in]	y	開始位置 Y
		@param[in]	w	幅
		@param[in]	h	高さ
		@return 画像
	 */
	//-----------------------------------------------------------------//
	static void get_frame_bufferRGB(unsigned char* dst, int x, int y, int w, int h)
	{
//		gl::core& core = gl::core::get_instance();
//		int fbh = core.get_size().y;
		::glReadBuffer(GL_BACK);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		::glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・バッファを画像を取得する。(RGBA RAW)
		@param[in]	dst	転送先
		@param[in]	x	開始位置 X
		@param[in]	y	開始位置 Y
		@param[in]	w	幅
		@param[in]	h	高さ
		@return 画像
	 */
	//-----------------------------------------------------------------//
	static void get_frame_bufferRGBA(unsigned char* dst, int x, int y, int w, int h)
	{
///		gl::core& core = gl::core::get_instance();
///		int fbh = core.get_size().y;
		::glReadBuffer(GL_BACK);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		::glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, dst);
	}
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	指定幅だけ小さいライン列を求める
		@param[in]	src	ソース・リスト
		@param[in]	width	小さくする幅
		@param[out]	dst	ラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void create_inner_line(const vtx::fposs& src, float width, vtx::fposs& dst)
	{
		unsigned int num = src.size();
		vtx::fpos_lines	innv;
		for(unsigned int i = 0; i < num; ++i) {
			unsigned int n = (i + 1) % num;
			vtx::fpos_line line(src[i], src[n]);
			vtx::fpos_line lout;
			inner_even_vector(line, width, lout);
			innv.push_back(lout);
		}

		dst.clear();
		for(unsigned int i = 0; i < num; ++i) {
			unsigned int m = (i - 1 + num) % num;
			vtx::fpos_line a = innv[m];
			vtx::fpos_line b = innv[i];
	
			vtx::fpos pc;
			if(check_cross_line(a, b, pc)) {
				dst.push_back(pc);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラウンドしたアウト・ラインを求める
		@param[in]	src	ソース・リスト
		@param[in]	radian	ラウンド半径
		@param[in]	rstep	ラウンドの分割数（１回転に対する分割数）
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void create_round_line(const vtx::fposs& src, float radian, int rstep, vtx::fposs& dst)
	{
		if(radian <= FLT_MIN) {
			dst = src;
			return;
		}

		unsigned int num = src.size();
		vtx::fpos_lines	outv;
		vtx::fpos_lines	innv;
		for(unsigned int i = 0; i < num; ++i) {
			unsigned int n = (i + 1) % num;
			vtx::fpos_line line(src[i], src[n]);
			outv.push_back(line);

			vtx::fpos_line lout;
			inner_even_vector(line, radian, lout);
			innv.push_back(lout);
		}

		dst.clear();
		for(unsigned int i = 0; i < num; ++i) {
			vtx::fpos_line aa = outv[i];
			vtx::fpos_line a = innv[i];
			unsigned int n = (i + 1) % num;
			vtx::fpos_line bb = outv[n];
			vtx::fpos_line b = innv[n];

			int ec = 0;
			vtx::fpos pc;
			if(check_cross_line(a, b, pc)) {
				ec++;
			}

			vtx::fpos d;
			vtx::fpos dd;
			vtx::fpos_line tmp;
			normalize(a.t - a.s, d);
			dd.set(-d.y * radian * 1.1f, d.x * radian * 1.1f);
			tmp.set(pc, pc + dd);
			vtx::fpos pa;
			if(check_cross_line(aa, tmp, pa)) {
				ec++;
			}

			normalize(b.t - b.s, d);
			dd.set(-d.y * radian * 1.1f, d.x * radian * 1.1f);
			tmp.set(pc, pc + dd);
			vtx::fpos pb;
			if(check_cross_line(tmp, bb, pb)) {
				ec++;
			}

			if(ec == 3) {
				float ico = vtx::inner_product(pa - pc, pb - pc);
				float ang = acosf(ico) / static_cast<float>(rstep);
				vtx::fpos src = pa - pc;
				int rs = static_cast<int>(rstep);
				for(int j = 0; j <= rs; ++j) {
					vtx::fpos ans;
					rotate_cw(src, sinf(ang * static_cast<float>(j)), cosf(ang * static_cast<float>(j)), ans);
					ans += pc;
					dst.push_back(ans);
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・ワイヤー・リストの作成
		@param[in]	src		ソース・リスト
		@param[in]	radius	半径
		@param[in]	width	ワイヤーの幅
		@param[in]	inn		内側のリスト
		@param[in]	out		外側のリスト
	 */
	//-----------------------------------------------------------------//
	static void create_frame_line_inn_out(const vtx::fposs& src, float radius, float width, vtx::fposs& inn, vtx::fposs& out)
	{
		create_round_line(src, radius, 6, out);

		vtx::fposs innw;
		create_inner_line(src, width, innw);
		create_round_line(innw, radius - width, 6, inn);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・ワイヤー・リストの作成
		@param[in]	src		ソース・リスト
		@param[in]	radius	ラウンドの半径
		@param[in]	width	ワイヤーの幅
		@param[in]	inz		内側の Z 値
		@param[in]	outz	外側の Z 値
		@param[out]	idst	作成後のインナーリストを受け取るリファレンス
		@param[out]	odst	作成後のアウターリストを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	static void create_frame_wire(const vtx::fposs& src, float radius, float width, float inz, float outz,
						   vtx::fvtxs& idst, vtx::fvtxs& odst)
	{
		vtx::fposs inn, out;
		create_frame_line_inn_out(src, radius, width, inn, out);

		double_line_to_triangle_strip(inn, inz, out, outz, odst);

		vtx::fvtxs v;
		convert_position_to_vertex(inn, inz, v);
		polygon_to_triangle_strip(v, idst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	最小値と最大値を探す
		@param[in]	src		ソース・リスト
		@param[out]	min		最小値
		@param[out]	max		最大値
	*/
	//-----------------------------------------------------------------//
	static void scan_min_max(const vtx::fposs& src, vtx::fpos& min, vtx::fpos& max)
	{
		bool init = false;
		for(vtx::fposs_cit cit = src.begin(); cit != src.end(); cit++) {
			vtx::fpos pos = *cit;
			if(init == false) {
				min = pos;
				max = pos;
				init = true;
			} else {
				if(min.x > pos.x) min.x = pos.x;
				if(min.y > pos.y) min.y = pos.y;
				if(max.x < pos.x) max.x = pos.x;
				if(max.y < pos.y) max.y = pos.y;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	最小値と最大値を探す
		@param[in]	src		ソース・リスト
		@param[out]	min		最小値
		@param[out]	max		最大値
	*/
	//-----------------------------------------------------------------//
	static void scan_min_max(const vtx::fvtxs& src, vtx::fvtx& min, vtx::fvtx& max)
	{
		bool init = false;
		for(vtx::fvtxs_cit cit = src.begin(); cit != src.end(); cit++) {
			vtx::fvtx pos = *cit;
			if(init == false) {
				min = pos;
				max = pos;
				init = true;
			} else {
				if(min.x > pos.x) min.x = pos.x;
				if(min.y > pos.y) min.y = pos.y;
				if(min.z > pos.z) min.z = pos.z;
				if(max.x < pos.x) max.x = pos.x;
				if(max.y < pos.y) max.y = pos.y;
				if(max.z < pos.z) max.z = pos.z;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ストリップの列から、各頂点の法線を求める
		@param[in]	vert	ソース・リスト
		@param[out]	norm	法線リスト
	*/
	//-----------------------------------------------------------------//
	static void create_normal(const vtx::fvtxs& vert, vtx::fvtxs& norm)
	{
		int num = vert.size();
		norm.clear();
		vtx::fvtx n;
		for(int i = 0; i < (num - 2); ++i) {
			vtx::fvtx a, c, b;
			a = vert[i];
			c = vert[i + 1];
			b = vert[i + 2];
			a -= c;
			b -= c;
			if(i & 1) {
				outer_product(a, b, n);
			} else {
				outer_product(b, a, n);
			}
			norm.push_back(n);
		}
		norm.push_back(n);
		norm.push_back(n);
	}


#if 0
	static void draw_quad(GLuint tex_id, const quad& q)
	{
		::glEnable(GL_TEXTURE_2D);

		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glEnableClientState(GL_VERTEX_ARRAY);

		::glBindTexture(GL_TEXTURE_2D, tex_id);
		::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		::glTexCoordPointer(2, GL_FLOAT, 0, q.coord);
		::glVertexPointer(3, GL_FLOAT, 0, q.vertex);

		::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	static void initial_quad(quad& q, float xt, float xe, float yt, float ye, float z)
	{
		q.coord[0].u = 0.0f; q.coord[0].v = 0.0f;
		q.vertex[0].x = xt;  q.vertex[0].y = yt;  q.vertex[0].z = z;
		q.coord[1].u = 0.0f; q.coord[1].v = 1.0f;
		q.vertex[1].x = xt;  q.vertex[1].y = ye;  q.vertex[1].z = z;
		q.coord[3].u = 1.0f; q.coord[3].v = 1.0f;
		q.vertex[3].x = xe;  q.vertex[3].y = ye;  q.vertex[3].z = z;
		q.coord[2].u = 1.0f; q.coord[2].v = 0.0f;
		q.vertex[2].x = xe;  q.vertex[2].y = yt;  q.vertex[2].z = z;
	}
#endif
//	void initial_quad(quad& q, float xt, float xe, float yt, float ye, float z);
//	void convert_position_flip(const vtx::fposs& src, bool h_flip, bool v_flip, vtx::fposs& dst);

	static void set_const_value(vtx::fvtx& value, vtx::fvtxs& dst)
	{
		for(vtx::fvtxs_it it = dst.begin(); it != dst.end(); ++it) {
			*it = value;
		}
	}

}
