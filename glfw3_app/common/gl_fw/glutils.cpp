//=====================================================================//
/*!	@file
	@brief	OpenGL 関係、描画を行う関数など
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cmath>
#include "gl_fw/glutils.hpp"
#include "gl_fw/IGLcore.hpp"

using namespace vtx;

namespace gl {
#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	byte2_t テーブルから、fposs
		@param[in]	src	byte3_t 型ソース
		@param[in]	n	数
		@param[in]	dst	vertex 出力列
	*/
	//-----------------------------------------------------------------//
	void byte2_to_fposs(const byte2_t* src, int n, vtx::fposs& dst)
	{
		for(int i = 0; i < n; ++i) {
			vtx::fpos v(src[i].x, src[i].y);
			dst.push_back(v);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	byte3_t テーブルから、vertex
		@param[in]	src	byte3_t 型ソース
		@param[in]	n	数
		@param[in]	dst	vertex 出力列
	*/
	//-----------------------------------------------------------------//
	void byte3_to_fvtxs(const byte3_t* src, int n, vtx::fvtxs& dst)
	{
		for(int i = 0; i < n; ++i) {
			vtx::fvtx v(src[i].x, src[i].y, src[i].z);
			dst.push_back(v);
		}
	}
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画（short）
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	void draw_line(const vtx::spos& a, const vtx::spos& b)
	{
		spos lines[2];
		lines[0] = a;
		lines[1] = b;
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_SHORT, 0, &lines[0]);
		::glDrawArrays(GL_LINES, 0, 2);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	void draw_line(const vtx::fpos& a, const vtx::fpos& b)
	{
		fpos lines[2];
		lines[0] = a;
		lines[1] = b;
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &lines[0]);
		::glDrawArrays(GL_LINES, 0, 2);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	a	ラインの開始
		@param[in]	b	ラインの終端
	*/
	//-----------------------------------------------------------------//
	void draw_line(const vtx::fvtx& a, const vtx::fvtx& b)
	{
		fvtx lines[2];
		lines[0] = a;
		lines[1] = b;
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &lines[0]);
		::glDrawArrays(GL_LINES, 0, 2);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	複数ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_lines(const vtx::sposs& list)
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
	void draw_lines(const vtx::fposs& list)
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
	void draw_lines(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINES, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_strip(const vtx::sposs& list)
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
	void draw_line_strip(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画
		@param[in]	list	２頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_loop(const vtx::sposs& list)
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
	void draw_line_loop(const vtx::fposs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(2, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_LOOP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ラインの描画
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_strip(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_STRIP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	閉じたラインの描画
		@param[in]	list	３頂点列
	*/
	//-----------------------------------------------------------------//
	void draw_line_loop(const vtx::fvtxs& list)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &list[0]);
		::glDrawArrays(GL_LINE_LOOP, 0, list.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トライアングル・ファンの描画（sort）
		@param[in]	list	２頂点列（最初が中心）
	*/
	//-----------------------------------------------------------------//
	void draw_triangle_fan(const vtx::sposs& list)
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
	void draw_triangle_fan(const vtx::fposs& list)
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
	void draw_triangle_strip(const vtx::sposs& list)
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
	void draw_triangle_strip(const vtx::fvtxs& list)
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
	void draw_triangle_strip(const vtx::fposs& list)
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
	void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fposs& tlist)
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
	void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist)
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
	void draw_triangle_strip(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist, const vtx::fposs& tlist)
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
	void draw_polygon(const vtx::sposs& list)
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
	void draw_polygon(const vtx::fposs& list)
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
	void draw_polygon(const vtx::fvtxs& list)
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
	void draw_polygon(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist)
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
	void draw_polygon(const vtx::fvtxs& vlist, const vtx::fposs& tlist)
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
	void draw_polygon(const vtx::fvtxs& vlist, const vtx::fvtxs& nlist, const vtx::fposs& tlist)
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
		@brief	グリッドを描画
		@param[in]	start	スタート
		@param[in]	end		エンド
		@param[in]	d	変化量
	*/
	//-----------------------------------------------------------------//
	void draw_grid(const vtx::fpos& start, const vtx::fpos& end, const vtx::fpos& d)
	{
		::glLineWidth(1.0f);

		fposs vs;
		{
			float step = start.y;
			int yy = static_cast<int>((end.y - start.y) / d.y);
			for(int y = 0; y <= yy; ++y) {
				vs.push_back(fpos(start.x, step));
				vs.push_back(fpos(end.x, step));
				step += d.y;
			}
		}
		{
			float step = start.x;
			int xx = static_cast<int>((end.x - start.x) / d.x);
			for(int x = 0; x <= xx; ++x) {
				vs.push_back(fpos(step, start.y));
				vs.push_back(fpos(step, end.y));
				step += d.x;
			}
		}
		draw_lines(vs);

		if(0) {
			::glLineWidth(2.0f);
			vs.clear();
			float i = (start.x + end.x) * 0.5f;
			float j = (start.y + end.y) * 0.5f;
			vs.push_back(fpos(i, j));
			vs.push_back(fpos(i + d.x, j));
			vs.push_back(fpos(i, j));
			vs.push_back(fpos(i, j + d.y));
		}
	}


	void draw_bounding_box(const vtx::fvtx& min, const vtx::fvtx& max)
	{
		fvtxs v;
		v.push_back(fvtx(min.x, max.y, max.z));
		v.push_back(fvtx(max.x, max.y, max.z));
		v.push_back(fvtx(max.x, min.y, max.z));
		v.push_back(fvtx(min.x, min.y, max.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(fvtx(min.x, max.y, min.z));
		v.push_back(fvtx(max.x, max.y, min.z));
		v.push_back(fvtx(max.x, max.y, max.z));
		v.push_back(fvtx(min.x, max.y, max.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(fvtx(min.x, max.y, min.z));
		v.push_back(fvtx(min.x, min.y, min.z));
		v.push_back(fvtx(max.x, min.y, min.z));
		v.push_back(fvtx(max.x, max.y, min.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(fvtx(min.x, min.y, min.z));
		v.push_back(fvtx(min.x, min.y, max.z));
		v.push_back(fvtx(max.x, min.y, max.z));
		v.push_back(fvtx(max.x, min.y, min.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(fvtx(max.x, min.y, max.z));
		v.push_back(fvtx(max.x, max.y, max.z));
		v.push_back(fvtx(max.x, max.y, min.z));
		v.push_back(fvtx(max.x, min.y, min.z));
		draw_line_loop(v);

		v.clear();
		v.push_back(fvtx(min.x, min.y, min.z));
		v.push_back(fvtx(min.x, max.y, min.z));
		v.push_back(fvtx(min.x, max.y, max.z));
		v.push_back(fvtx(min.x, min.y, max.z));
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
	void convert_position_to_vertex(const vtx::fposs& src, float z, vtx::fvtxs& dst)
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
	void convert_vertex_to_position(const vtx::fvtxs& src, const vtx::fpos& offset, const vtx::fpos& scale, vtx::fposs& dst)
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
	void create_rectangle(const vtx::fpos& pos, const vtx::fpos& size, vtx::fposs& dst)
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
	void create_rectangle(const vtx::fpos& pos, const vtx::fpos& size, float z, vtx::fvtxs& dst)
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
	void double_line_to_triangle_strip(const vtx::fposs& inn, float inn_z, const vtx::fposs& out, float out_z, vtx::fvtxs& dst)
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


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	二つの平面ベクトルの交点を求める
		@param[in]	a	ベクトル A
		@param[in]	b	ベクトル B
		@param[out]	pos	交点
		@return	「解」が無い場合、「false」が返る。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	bool check_cross_line(const vtx::fpos_line& a, const vtx::fpos_line& b, vtx::fpos& pos)
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


	void convert_position_flip(const vtx::fposs& src, bool h_flip, bool v_flip, vtx::fposs& dst)
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

#if 0
	void draw_quad(GLuint tex_id, const quad& q)
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


	void initial_quad(quad& q, float xt, float xe, float yt, float ye, float z)
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


	//-----------------------------------------------------------------//
	/*!
		@brief	円を描画する（short）
		@param[in]	pos		中心座標
		@param[in]	rad		半径
		@param[in]	step	分割数
	*/
	//-----------------------------------------------------------------//
	void draw_line_circle(const vtx::spos& pos, int rad, int step)
	{
		vtx::sposs list;
		float a = radian_f_ / static_cast<float>(step);
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
	void draw_line_circle(const vtx::fpos& pos, float rad, int step)
	{
		vtx::fposs list;
		float a = radian_f_ / static_cast<float>(step);
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
	void draw_filled_circle(const vtx::spos& pos, int rad, int step)
	{
		vtx::sposs list;
		list.push_back(pos);
		float a = radian_f_ / static_cast<float>(step);
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
	void draw_filled_circle(const vtx::fpos& pos, float rad, int step)
	{
		vtx::fposs list;
		float a = radian_f_ / static_cast<float>(step);
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
	void draw_filled_ring(const vtx::spos& pos, int orad, int irad, int step)
	{
		vtx::sposs list;
		float a = radian_f_ / static_cast<float>(step);
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
	void draw_filled_ring(const vtx::fpos& pos, float orad, float irad, int step)
	{
		vtx::fposs list;
		float a = radian_f_ / static_cast<float>(step);
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
	void draw_filled_arrow(const vtx::srect& rect, short dir)
	{
		sposs list;
		switch(dir) {
		case 0:
			list.push_back(spos(rect.center_x(), rect.org.y));
			list.push_back(spos(rect.org.x, rect.end_y()));
			list.push_back(rect.end());
			break;
		case 90:
			list.push_back(spos(rect.end_x(), rect.center_y()));
			list.push_back(rect.org);
			list.push_back(spos(rect.org.x, rect.end_y()));
			break;
		case 180:
			list.push_back(spos(rect.center_x(), rect.end_y()));
			list.push_back(spos(rect.end_x(), rect.org.y));
			list.push_back(rect.org);
			break;
		case 270:
			list.push_back(spos(rect.org.x, rect.center_y()));
			list.push_back(rect.end());
			list.push_back(spos(rect.end_x(), rect.org.y));
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
	void draw_line_rectangle(const vtx::fpos& pos, const vtx::fpos& size)
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
	void draw_filled_rectangle(const vtx::srect& rect)
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
	void draw_filled_rectangle(const vtx::fpos& pos, const vtx::fpos& size)
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
	void draw_filled_rectangle(const vtx::fpos& pos, const vtx::fpos& size, bool hflip, bool vflip)
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
	void draw_filled_rectangle_with_edge(const vtx::srect& rect, int ew, const img::rgbaf& c)
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
	void draw_line_cylinder(float length, float radius, int rstep)
	{
		vtx::fvtxs listm;
		vtx::fvtxs listp;
		float gain = radian_f_ / static_cast<float>(rstep);
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
	void draw_filled_round_rectangle(const vtx::fpos& pos, const vtx::fpos& size, float radian, int rstep)
	{


//		create_round_line(const vtx::fposs& src, float radian, int rstep, vtx::fposs& dst);


	}


	//-----------------------------------------------------------------//
	/*!
		@brief	指定幅だけ小さいライン列を求める
		@param[in]	src	ソース・リスト
		@param[in]	width	小さくする幅
		@param[out]	dst	ラインを受け取るリファレンス
	*/
	//-----------------------------------------------------------------//
	void create_inner_line(const vtx::fposs& src, float width, vtx::fposs& dst)
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
	void create_round_line(const vtx::fposs& src, float radian, int rstep, vtx::fposs& dst)
	{
		if(radian <= FLT_MIN) {
			dst = src;
			return;
		}

		unsigned int num = src.size();
		fpos_lines	outv;
		fpos_lines	innv;
		for(unsigned int i = 0; i < num; ++i) {
			unsigned int n = (i + 1) % num;
			fpos_line line(src[i], src[n]);
			outv.push_back(line);

			fpos_line lout;
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

			fpos d, dd;
			fpos_line tmp;
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
	void create_frame_line_inn_out(const vtx::fposs& src, float radius, float width, vtx::fposs& inn, vtx::fposs& out)
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
	void create_frame_wire(const vtx::fposs& src, float radius, float width, float inz, float outz,
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
	void scan_min_max(const vtx::fposs& src, vtx::fpos& min, vtx::fpos& max)
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
	void scan_min_max(const vtx::fvtxs& src, vtx::fvtx& min, vtx::fvtx& max)
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
	void create_normal(const vtx::fvtxs& vert, vtx::fvtxs& norm)
	{
		int num = vert.size();
		norm.clear();
		fvtx n;
		for(int i = 0; i < (num - 2); ++i) {
			fvtx a, c, b;
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


	//-----------------------------------------------------------------//
	/*!
		@brief	正立体の描画
		@param[in]	s	サイズ
	*/
	//-----------------------------------------------------------------//
	void draw_square(float s)
	{
		::glPushMatrix();
		::glScalef(s, s, s);

		fvtxs v;
		v.push_back(fvtx(-1.0f,  1.0f, -1.0f));
		v.push_back(fvtx( 1.0f,  1.0f, -1.0f));
		v.push_back(fvtx( 1.0f, -1.0f, -1.0f));
		v.push_back(fvtx(-1.0f, -1.0f, -1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(fvtx(-1.0f, -1.0f, -1.0f));
		v.push_back(fvtx( 1.0f, -1.0f, -1.0f));
		v.push_back(fvtx( 1.0f, -1.0f,  1.0f));
		v.push_back(fvtx(-1.0f, -1.0f,  1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(fvtx(-1.0f,  1.0f,  1.0f));
		v.push_back(fvtx(-1.0f, -1.0f,  1.0f));
		v.push_back(fvtx( 1.0f, -1.0f,  1.0f));
		v.push_back(fvtx( 1.0f,  1.0f,  1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(fvtx(-1.0f,  1.0f,  1.0f));
		v.push_back(fvtx( 1.0f,  1.0f,  1.0f));
		v.push_back(fvtx( 1.0f,  1.0f, -1.0f));
		v.push_back(fvtx(-1.0f,  1.0f, -1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(fvtx(-1.0f,  1.0f,  1.0f));
		v.push_back(fvtx(-1.0f,  1.0f, -1.0f));
		v.push_back(fvtx(-1.0f, -1.0f, -1.0f));
		v.push_back(fvtx(-1.0f, -1.0f,  1.0f));
		draw_polygon(v);

		v.clear();
		v.push_back(fvtx( 1.0f,  1.0f, -1.0f));
		v.push_back(fvtx( 1.0f,  1.0f,  1.0f));
		v.push_back(fvtx( 1.0f, -1.0f,  1.0f));
		v.push_back(fvtx( 1.0f, -1.0f, -1.0f));
		draw_polygon(v);

		::glPopMatrix();
	}


	void subdivision_line(const fvtxs& src, float factor, fvtxs& dst)
	{
		if(src.size() < 2) return;

		fvtxs tmp;
		for(unsigned int i = 0; i < (src.size() - 1); ++i) {
			fvtx c;
			factor_line(src[i], 0.5f, src[i + 1], c);
			tmp.push_back(c);
		}
		for(unsigned int i = 0; i < src.size(); ++i) {
			if(i > 0 && i < (src.size() - 1)) {
				fvtx c;
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


	//-----------------------------------------------------------------//
	/*!
		@brief	テクスチャー・マップの為のuvコーディネートを求める
		@param[in]	srcc	中心座標
		@param[in]	srcv	頂点法線座標列
		@param[out]	uvs		UV 座標列
	*/
	//-----------------------------------------------------------------//
	void create_sphere_uv(const vtx::fvtx& srcc, const vtx::fvtxs& srcv, vtx::fposs& uvs)
	{
		uvs.clear();

		for(fvtxs_cit cit = srcv.begin(); cit != srcv.end(); ++cit) {
			fvtx v = *cit;
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
	void create_circle_uv(const vtx::fvtxs& srcn, vtx::fposs& uvs)
	{
		uvs.clear();

		for(fvtxs_cit cit = srcn.begin(); cit != srcn.end(); ++cit) {
			const fvtx& n = *cit;
			float m = 2.0f * sqrtf(n.sqrY() + n.sqrY() + (n.z + 1.0f) * (n.z + 1.0f));
			fpos uv(n.x / m + 0.5f, n.y / m + 0.5f);
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
	void create_reflection_uv(const vtx::fvtx& eye, const vtx::fvtxs& srcn, vtx::fposs& uvs)
	{
		uvs.clear();

		fvtx ieye = eye * -1.0f;
		for(fvtxs_cit cit = srcn.begin(); cit != srcn.end(); ++cit) {
			const fvtx& n = *cit;
			float in = 2.0f * dot(ieye, n);
			fvtx nn = n * in;
			nn -= ieye;
			float m = 2.0f * sqrtf(nn.sqrX() + nn.sqrY() + (nn.z + 1.0f) * (nn.z + 1.0f));
			fpos uv(nn.x / m + 0.5f, nn.y / m + 0.5f);
			uvs.push_back(uv);
		}
	}


	static float g_matrix_scale = 1.0f;

	//-----------------------------------------------------------------//
	/*!
		@brief iPhone 用マトリックス・スケールの設定
		@param[in]	scale	スケール・ファクター
	 */
	//-----------------------------------------------------------------//
	void set_iphone_matrix_scale(float scale) { g_matrix_scale = scale; }


	//-----------------------------------------------------------------//
	/*!
		@brief iPhone 用マトリックスの設定
		@param[in]	width	論理横幅
		@param[in]	height	論理高さ
		@param[in]	zn		Z near
		@param[in]	zf		Z far
	 */
	//-----------------------------------------------------------------//
	void setup_iphone_matrix(int width, int height, float zn, float zf)
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
	static bool g_gravity_lock = true;
	static int g_gravity_type = IPHONE_GRAVITY_NONE;

	//-----------------------------------------------------------------//
	/*!
		@brief	iPhone の重力タイプを返す
		@return	重力タイプ
	 */
	//-----------------------------------------------------------------//	
	int get_iphone_gravity_type()
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
	void set_gravity_lock(bool f)
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
		@return 画像
	 */
	//-----------------------------------------------------------------//
	img::img_rgba8* get_frame_buffer(int x, int y, int w, int h)
	{
		IGLcore* glif = get_glcore();
		int fbh = glif->get_size().y;
		img::img_rgba8* im = new img::img_rgba8;
		im->create(vtx::spos(w, h), true);
		::glReadBuffer(GL_BACK);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		img::rgba8* p = im->at_image();
		for(int yy = 0; yy < h; ++yy) {
			::glReadPixels(x, fbh - 1 - y - yy, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, p);
			p += w;
		}
		return im;
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
	void get_frame_bufferRGB(unsigned char* dst, int x, int y, int w, int h)
	{
		IGLcore* glif = get_glcore();
		int fbh = glif->get_size().y;
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
	void get_frame_bufferRGBA(unsigned char* dst, int x, int y, int w, int h)
	{
		IGLcore* glif = get_glcore();
		int fbh = glif->get_size().y;
		::glReadBuffer(GL_BACK);
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		::glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, dst);
	}
#endif

}
