#pragma once
//=====================================================================//
/*!	@file
	@brief	@brief	OpenGL 関係、描画を行う関数など（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <map>
#include "gl_fw/gl_info.hpp"
#include "utils/vtx.hpp"
#include "img_io/i_img.hpp"
#include "img_io/img_rgba8.hpp"

namespace gl {

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
	inline uint32_t polygon_to_triangle_strip(const vtx::fvtxs& src, vtx::fvtxs& dst) {
		return polygon2triangleStrip<vtx::fvtxs>(src, dst);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（short）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	void draw_line(const vtx::spos& a, const vtx::spos& b);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（float）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	void draw_line(const vtx::fpos& a, const vtx::fpos& b);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（float）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	void draw_line(const vtx::fvtx& a, const vtx::fvtx& b);


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画（short）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_lines(const vtx::sposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画（float）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_lines(const vtx::fposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画（float）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_lines(const vtx::fvtxs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画（short）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_loop(const vtx::sposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画（float）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_loop(const vtx::fposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（short）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_strip(const vtx::sposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_strip(const vtx::iposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（float）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_strip(const vtx::fposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画（float）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_loop(const vtx::fvtxs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（float）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_strip(const vtx::fvtxs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ファンの描画（short）
		@param[in]	list	２頂点列（最初が中心）
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_fan(const vtx::sposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ファンの描画（float）
		@param[in]	list	２頂点列（最初が中心）
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_fan(const vtx::fposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（short）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_strip(const vtx::sposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_strip(const vtx::fvtxs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_strip(const vtx::fposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	vlist	３頂点列
		@param[in]	tlist	テクスチャーコーディネート列
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fposs& tlist);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	vlist	３頂点列
		@param[in]	nlist	法線列
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングルストリップの描画（float）
		@param[in]	vlist	３頂点列
		@param[in]	nlist	法線列
		@param[in]	tlist	テクスチャーコーディネート列
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist, const vtx::fposs& tlist);


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画（short）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_polygon(const vtx::sposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画（float）
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_polygon(const vtx::fposs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画（float）
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_polygon(const vtx::fvtxs& list);


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画（float）
		@param[in]	vlist		３頂点列
		@param[in]	nlist		法線リスト
	*/
	//-----------------------------------------------------------------//
	void draw_polygon(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist);


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画（float）
		@param[in]	vlist		３頂点列
		@param[in]	tlist		テクスチャーコーディネートリスト
	*/
	//-----------------------------------------------------------------//
	void draw_polygon(const vtx::fvtxs& vlist, const vtx::fposs& tlist);


	//-----------------------------------------------------------------//
	/*!
		@brief	ポリゴンの描画（float）
		@param[in]	vlist		３頂点列
		@param[in]	nlist		法線リスト
		@param[in]	tlist		テクスチャーコーディネートリスト
	*/
	//-----------------------------------------------------------------//
	void draw_polygon(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist, const vtx::fposs& tlist);


	//-----------------------------------------------------------------//
	/*!
		@brief	円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_line_circle(const vtx::spos& pos, int rad, int step = 8);


	//-----------------------------------------------------------------//
	/*!
		@brief	円を描画する（float）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_line_circle(const vtx::fpos& pos, float rad, int step = 8);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされ円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_filled_circle(const vtx::spos& pos, int rad, int step = 8);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた円を描画する（float）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_filled_circle(const vtx::fpos& pos, float rad, int step = 8);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされ円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	orad	外径
		@param[in]	irad	内径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_filled_ring(const vtx::spos& pos, int orad, int irad, int step = 8);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた円を描画する（float）
		@param[in]	pos		中心座標
		@param[in]	orad	外径
		@param[in]	irad	内径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_filled_ring(const vtx::fpos& pos, float orad, float irad, int step = 8);


	//-----------------------------------------------------------------//
	/*!
		@brief	三角を描画する
		@param[in]	rect	位置、大きさ
		@param[in]	dir		角度（0, 90, 180, 270)
	*/
	//-----------------------------------------------------------------//
	void draw_filled_arrow(const vtx::srect& rect, short dir);


	//-----------------------------------------------------------------//
	/*!
		@brief	四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
	*/
	//-----------------------------------------------------------------//
	void draw_line_rectangle(const vtx::fpos& pos, const vtx::fpos& size);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、四角を描画する
		@param[in]	rect	短形
	*/
	//-----------------------------------------------------------------//
	void draw_filled_rectangle(const vtx::srect& rect);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
	*/
	//-----------------------------------------------------------------//
	void draw_filled_rectangle(const vtx::fpos& pos, const vtx::fpos& size);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
		@param[in]	hflip	テクスチャー左右反転
		@param[in]	vflip	テクスチャー上下反転
	*/
	//-----------------------------------------------------------------//
	void draw_filled_rectangle(const vtx::fpos& pos, const vtx::fpos& size, bool hflip, bool vflip);


	//-----------------------------------------------------------------//
	/*!
		@brief	エッジ付き短型を描画
		@param[in]	rect	短型
		@param[in]	ew		エッジの幅
		@param[in]	c		基本カラー
	 */
	//-----------------------------------------------------------------//
	void draw_filled_rectangle_with_edge(const vtx::srect& rect, int ew, const img::rgbaf& c);


	//-----------------------------------------------------------------//
	/*!
		@brief	ワイヤーフレームシリンダーの描画
		@param[in]	length		長さ
		@param[in]	radius	半径
		@param[in]	rstep	半径の分割数
	*/
	//-----------------------------------------------------------------//
	void draw_line_cylinder(float length, float radius, int rstep);


	//-----------------------------------------------------------------//
	/*!
		@brief	塗りつぶされた、角が丸い四角を描画する
		@param[in]	pos		基準座標
		@param[in]	size	大きさ
		@param[in]	radian	ラウンド半径
		@param[in]	rstep	ラウンドの分割数（１回転に対する分割数）
	*/
	//-----------------------------------------------------------------//
	void draw_filled_round_rectangle(const vtx::fpos& pos, const vtx::fpos& size, float radian, int rstep);


	//-----------------------------------------------------------------//
	/*!
		@brief	正立体の描画
		@param[in]	s	サイズ
	*/
	//-----------------------------------------------------------------//
	void draw_square(float s);


	//-----------------------------------------------------------------//
	/*!
		@brief	グリッドを描画
		@param[in]	start	スタート
		@param[in]	end		エンド
		@param[in]	d	変化量
	*/
	//-----------------------------------------------------------------//
	void draw_grid(const vtx::fpos& start, const vtx::fpos& end, const vtx::fpos& d);


	void subdivision_line(const vtx::fvtxs& src, float factor, vtx::fvtxs& dst);


	inline void subdivision_line(const vtx::fvtxs& src, float factor, vtx::fvtxs& dst, int n) {
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


	void draw_bounding_box(const vtx::fvtx& min, const vtx::fvtx& max);
	



	//-----------------------------------------------------------------//
	/*!
		@brief	position 列から vertex 列への変換
		@param[in]	src	ソース・リスト
		@param[in]	z	変換の際設定する z の値
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void convert_position_to_vertex(const vtx::fposs& src, float z, vtx::fvtxs& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	vertex 列から position 列への変換（ Z メンバーは無視される）
		@param[in]	src	ソース・リスト
		@param[in]	offset	オフセット
		@param[in]	scale	スケール
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void convert_vertex_to_position(const vtx::fvtxs& src, const vtx::fpos& offset, const vtx::fpos& scale, vtx::fposs& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	長方形座標列を生成する（時計周り）
		@param[in]	pos		開始点
		@param[in]	size	大きさ
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void create_rectangle(const vtx::fpos& pos, const vtx::fpos& size, vtx::fposs& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	長方形座標列を生成する（時計周り）
		@param[in]	pos		開始点
		@param[in]	size	大きさ
		@param[in]	z		Z 値
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void create_rectangle(const vtx::fpos& pos, const vtx::fpos& size, float z, vtx::fvtxs& dst);


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
	void double_line_to_triangle_strip(const vtx::fposs& inn, float inn_z, const vtx::fposs& out, float out_z, vtx::fvtxs& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・マップの為のuvコーディネートを求める（円）
		@param[in]	srcn	頂点法線座標列
		@param[out]	uvs		UV 座標列
	*/
	//-----------------------------------------------------------------//
	void create_circle_uv(const vtx::fvtxs& srcn, vtx::fposs& uvs);


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・マップの為のuvコーディネートを求める（リフレクション）
		@param[in]	eye		視線ベクトル
		@param[in]	srcn	頂点法線座標列
		@param[out]	uvs		UV 座標列
	*/
	//-----------------------------------------------------------------//
	void create_reflection_uv(const vtx::fvtx& eye, const vtx::fvtxs& srcn, vtx::fposs& uvs);


	//-----------------------------------------------------------------//
	/*!
		@brief iPhone 用マトリックス・スケールの設定
		@param[in]	scale	スケール・ファクター
	 */
	//-----------------------------------------------------------------//
	void set_iphone_matrix_scale(float scale);

	
	//-----------------------------------------------------------------//
	/*!
		@brief iPhone 用マトリックスの設定
		@param[in]	width	論理横幅
		@param[in]	height	論理高さ
		@param[in]	zn		Z near
		@param[in]	zf		Z far
	 */
	//-----------------------------------------------------------------//
	void setup_iphone_matrix(int width, int height, float zn = -1.0f, float zf = 1.0f);


#ifdef IPHONE_IPAD
	enum {
		IPHONE_GRAVITY_NONE,
		IPHONE_GRAVITY_TOP,
		IPHONE_GRAVITY_LEFT,
		IPHONE_GRAVITY_RIGHT,
		IPHONE_GRAVITY_BOTTOM,
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	iPhone の重力タイプを返す
		@return	重力タイプ
	 */
	//-----------------------------------------------------------------//	
	int get_iphone_gravity_type();
	
	
	//-----------------------------------------------------------------//
	/*!
		@brief	重力ロックを設定する
		@param[in]	重力応答をロックする場合「true」
	 */
	//-----------------------------------------------------------------//	
	void set_gravity_lock(bool f = true);
#endif

#ifndef OPENGL_ES
	//-----------------------------------------------------------------//
	/*!
		@brief	フレーム・バッファを画像を取得する。
		@param[in]	x	開始位置 X
		@param[in]	y	開始位置 Y
		@param[in]	w	幅
		@param[in]	h	高さ
		@return 画像
	 */
	//-----------------------------------------------------------------//
	img::shared_img get_frame_buffer(int x, int y, int w, int h);


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
	void get_frame_bufferRGB(unsigned char* dst, int x, int y, int w, int h);


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
	void get_frame_bufferRGBA(unsigned char* dst, int x, int y, int w, int h);
#endif

	//-----------------------------------------------------------------//
	/*!
		@brief	指定幅だけ小さいライン列を求める
		@param[in]	src	ソース・リスト
		@param[in]	width	小さくする幅
		@param[out]	dst	ラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void create_inner_line(const vtx::fposs& src, float width, vtx::fposs& dst);


	//-----------------------------------------------------------------//
	/*!
		@brief	ラウンドしたアウト・ラインを求める
		@param[in]	src	ソース・リスト
		@param[in]	radian	ラウンド半径
		@param[in]	rstep	ラウンドの分割数（１回転に対する分割数）
		@param[out]	dst	ラウンドしたラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void create_round_line(const vtx::fposs& src, float radian, int rstep, vtx::fposs& dst);


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
	void create_frame_line_inn_out(const vtx::fposs& src, float radius, float width, vtx::fposs& inn, vtx::fposs& out);


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
	void create_frame_wire(const vtx::fposs& src, float radius, float width, float inz, float outz,
						   vtx::fvtxs& idst, vtx::fvtxs& odst);


	//-----------------------------------------------------------------//
	/*!
		@brief	最小値と最大値を探す (fpos)
		@param[in]	src		ソース列
		@param[out]	min		最小値
		@param[out]	max		最大値
	*/
	//-----------------------------------------------------------------//
	void scan_min_max(const vtx::fposs& src, vtx::fpos& min, vtx::fpos& max);


	//-----------------------------------------------------------------//
	/*!
		@brief	最小値と最大値を探す (fvtx)
		@param[in]	src		ソース列
		@param[out]	min		最小値
		@param[out]	max		最大値
	*/
	//-----------------------------------------------------------------//
	void scan_min_max(const vtx::fvtxs& src, vtx::fvtx& min, vtx::fvtx& max);


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ストリップの列から、各頂点の法線を求める
		@param[in]	vert	ソース・リスト
		@param[out]	norm	法線リスト
	*/
	//-----------------------------------------------------------------//
	void create_normal(const vtx::fvtxs& vert, vtx::fvtxs& norm);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	二つのベクトルの交点を求める
		@param[in]	a	ベクトル A
		@param[in]	b	ベクトル B
		@param[out]	pos	交点
		@return	「解」が無い場合、「false」が返る。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	bool check_cross_line(const vtx::fpos_line& a, const vtx::fpos_line& b, vtx::fpos& pos);


//	void draw_quad(GLuint tex_id, const quad& q);
//	void initial_quad(quad& q, float xt, float xe, float yt, float ye, float z);
//	void convert_position_flip(const vtx::fposs& src, bool h_flip, bool v_flip, vtx::fposs& dst);

	inline void set_const_value(vtx::fvtx& value, vtx::fvtxs& dst) {
		for(vtx::fvtxs_it it = dst.begin(); it != dst.end(); ++it) {
			*it = value;
		}
	}

}

