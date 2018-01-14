//=====================================================================//
/*!	@file
	@brief	線形代数的数学ユーティリティー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "utils/vmath.hpp"

namespace vtx {

#if 0
	static bool check_in_P_to_ABC(const fvtx& P, const fvtx& A, const fvtx& B, const fvtx& C)
	{
		//----平面ベクトル"OAB,OBC,OCA"を"U,V,W"とする。
		//----ベクトル P と U, V, W の内積及び判定
		fvtx U((A.y * B.z - A.z * B.y), (A.z * B.x - A.x * B.z), (A.x * B.y - A.y * B.x));
		float PU = (P.x * U.x + P.y * U.y + P.z * U.z);
		if(PU < 0.0f) return false;

		fvtx V((B.y * C.z - B.z * C.y), (B.z * C.x - B.x * C.z), (B.x * C.y - B.y * C.x));
		float PV = (P.x * V.x + P.y * V.y + P.z * V.z);
		if(PV < 0.0f) return false;

		fvtx W((C.y * A.z - C.z * A.y), (C.z * A.x - C.x * A.z), (C.x * A.y - C.y * A.x));
		float PW = (P.x * W.x + P.y * W.y + P.z * W.z);
		if(PW < 0.0f) return false;

		//①：まず⊿ABCの平面ベクトルＨ(Hx,Hy,Hz)を算出
		fvtx AB = B - A;
		fvtx AC = C - A;
		fvtx H;
		H.x = (AB.y * AC.z - AB.z * AC.y);
		H.y = (AB.z * AC.x - AB.x * AC.z);
		H.z = (AB.x * AC.y - AB.y * AC.x);

		if((H.x * (P.x - A.x) + H.y * (P.y - A.y) + H.z * (P.z - A.z) >= 0.0f)) {
			return true;
		} else {
			return false;
		}
	}
#endif

#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	直線と三角の当たり判定
		@param[in]	p	開始座標ｐ
		@param[in]	q	終端座標q
		@param[in]	v	三角の座標列
		@param[out]	cp	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool line_hit_triangle(const fvtx& p, const fvtx& q, const fvtx v[3], fvtx& cp)
	{
		fvtx P, A, B, C;
		normalization(q - p, P);
		normalization(v[0] - p, A);
		normalization(v[1] - p, B);
		normalization(v[2] - p, C);

		//----平面ベクトル"OAB,OBC,OCA"を"U,V,W"とする。
		//----ベクトル P と U, V, W の内積及び判定
		fvtx U((A.y * B.z - A.z * B.y), (A.z * B.x - A.x * B.z), (A.x * B.y - A.y * B.x));
		float PU = (P.x * U.x + P.y * U.y + P.z * U.z);
		if(PU < 0.0f) return false;

		fvtx V((B.y * C.z - B.z * C.y), (B.z * C.x - B.x * C.z), (B.x * C.y - B.y * C.x));
		float PV = (P.x * V.x + P.y * V.y + P.z * V.z);
		if(PV < 0.0f) return false;

		fvtx W((C.y * A.z - C.z * A.y), (C.z * A.x - C.x * A.z), (C.x * A.y - C.y * A.x));
		float PW = (P.x * W.x + P.y * W.y + P.z * W.z);
		if(PW < 0.0f) return false;

		// まず⊿ABCの平面ベクトルＨ(Hx,Hy,Hz)を算出
		fvtx AB = B - A;
		fvtx AC = C - A;
		fvtx H;
		H.x = (AB.y * AC.z - AB.z * AC.y);
		H.y = (AB.z * AC.x - AB.x * AC.z);
		H.z = (AB.x * AC.y - AB.y * AC.x);

		if((H.x * (P.x - A.x) + H.y * (P.y - A.y) + H.z * (P.z - A.z) >= 0.0f)) {
			// 条件が成立したら、交点を求める。
			float ha = H.x * A.x + H.y * A.y + H.z * A.z;
			float hp = H.x * P.x + H.y * P.y + H.z * P.z;
			float t = 1.0f;
			if(hp > FLT_MIN) {
				t = ha / hp;
				// 念の為クリッピング
				// if(t < 0.0f) t = 0.0f; else if(t > 1.0f) t = 1.0f;
			}
			t = 0.5f;
			cp = ((q - p) * t) + p;
			return true;
		} else {
			return false;
		}
	}
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	三角形内に点が存在するか
		@param[in]	pos	点の座標
		@param[in]	vv	三角の座標列
		@return あれば「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool triangle_in_point(const fpos& pos, const fvtx& v0, const fvtx& v1, const fvtx& v2)
	{
		fvtx org(pos.x, pos.y, 1.0f);
		fvtx dir(0.0f, 0.0f, -1.0f);
		fvtx vv(v0.x, v0.y, 0.0f);
		fvtx e1 = v1 - v0;
		e1.z = 0.0f;
		fvtx e2 = v2 - v0;
		e2.z = 0.0f;

		float u, v;
		fvtx pvec, qvec;
		cross(dir, e2, pvec);
		float det = dot(e1, pvec);
		if(det > (1e-3)) {
			fvtx tvec = org - vv;
			u = dot(tvec, pvec);
			if(u < 0.0f || u > det) return false;
			cross(tvec, e1, qvec);
			v = dot(dir, qvec);
			if(v < 0.0f || (u + v) > det) return false;
		} else if(det < -(1e-3)) {
			fvtx tvec = org - vv;
			u = dot(tvec, pvec);
			if(u > 0.0f || u < det) return false;
			cross(tvec, e1, qvec);
			v = dot(dir, qvec);
			if(v > 0.0f || (u + v) < det) return false;
		} else {
			return false;
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	多角形内に点が存在するか
		@param[in]	pos	点の座標
		@param[in]	src	頂点の座標列
		@return あれば「true」を返す
	*/
	//-----------------------------------------------------------------//
#if 0
	bool polygon_in_point(const fpos& pos, const fpositions& src)
	{
		fpos vv[3];
		unsigned int num = src.size();
		for(unsigned int i = 0; i < num; ++i) {
			if(i < 2) {
				vv[i] = src[i];
			} else {
				int n = ((i - 2) & 65535) >> 1;
				if(i & 1) {
					vv[2] = src[num - 1 - n];
				} else {
					vv[2] = src[2 + n];
				}
				if(triangle_in_point(pos, vv[0], vv[1], vv[2])) {
					return true;
				}
				vv[0] = vv[1];
				vv[1] = vv[2];
			}
		}
		return false;
	}
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と三角形の当たり判定（無限直線）
		@param[in]	org	直線起点
		@param[in]	dir	直線方向
		@param[in]	tv	三角の座標列
		@param[out]	crs	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool triangle_intersect(const fvtx& org, const fvtx& dir, const fvtx tv[3], fvtx& crs)
	{
		fvtx e1 = tv[1] - tv[0];
		fvtx e2 = tv[2] - tv[0];

		float t, u, v;
		fvtx pvec, qvec;
		cross(dir, e2, pvec);
		float det = dot(e1, pvec);
		if(det > (1e-3)) {
			fvtx tvec = org - tv[0];
			u = dot(tvec, pvec);
			if(u < 0.0f || u > det) return false;
			cross(tvec, e1, qvec);
			v = dot(dir, qvec);
			if(v < 0.0f || (u + v) > det) return false;
		} else if(det < -(1e-3)) {
			fvtx tvec = org - tv[0];
			u = dot(tvec, pvec);
			if(u > 0.0f || u < det) return false;
			cross(tvec, e1, qvec);
			v = dot(dir, qvec);
			if(v > 0.0f || (u + v) < det) return false;
		} else {
			return false;
		}

		float inv_det = 1.0f / det;
		t = dot(e2, qvec);
		t *= inv_det;
////	u *= inv_det;
////	v *= inv_det;
//		tuv.x = t;
//		tuv.y = u;
//		tuv.z = v;

		crs.x = dir.x * t + org.x;
		crs.y = dir.y * t + org.y;
		crs.z = dir.z * t + org.z;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と三角形の当たり判定その２（有限直線）
		@param[in]	org	直線起点
		@param[in]	end	直線終点
		@param[in]	tv	三角の座標列
		@param[out]	crs	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool triangle_intersect2(const fvtx& org, const fvtx& end, const fvtx tv[3], fvtx& crs)
	{
		fvtx vec = end - org;
		float l = distance(org, end);
		fvtx dir;
		if(!normalize(vec, dir)) return false;

		fvtx e1 = tv[1] - tv[0];
		fvtx e2 = tv[2] - tv[0];

		float t, u, v;
		fvtx pvec, qvec;
		cross(dir, e2, pvec);
		float det = dot(e1, pvec);
		if(det > (1e-3)) {
			fvtx tvec = org - tv[0];
			u = dot(tvec, pvec);
			if(u < 0.0f || u > det) return false;
			cross(tvec, e1, qvec);
			v = dot(dir, qvec);
			if(v < 0.0f || (u + v) > det) return false;
		} else if(det < -(1e-3)) {
			fvtx tvec = org - tv[0];
			u = dot(tvec, pvec);
			if(u > 0.0f || u < det) return false;
			cross(tvec, e1, qvec);
			v = dot(dir, qvec);
			if(v > 0.0f || (u + v) < det) return false;
		} else {
			return false;
		}

		float inv_det = 1.0f / det;
		t = dot(e2, qvec);
		t *= inv_det;

		if(t > l) return false;

		crs.x = dir.x * t + org.x;
		crs.y = dir.y * t + org.y;
		crs.z = dir.z * t + org.z;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と四角形の当たり判定（無限直線）
		@param[in]	org	直線起点
		@param[in]	dir	直線方向
		@param[in]	vlist	時計周り頂点リスト
		@param[out]	crs	交点座標
		@return 当たりなら四角形内三角形の番号 (1: 0-1-2, 2: 2-3-0)
	*/
	//-----------------------------------------------------------------//
	int quadrangle_intersect(const fvtx& org, const fvtx& dir, const fvtxs& vlist, fvtx& crs)
	{
		if(triangle_intersect(org, dir, &vlist[0], crs)) {
			return 1;
		}
		fvtx v[3];
		v[0] = vlist[2];
		v[1] = vlist[3];
		v[2] = vlist[0];
		if(triangle_intersect(org, dir, v, crs)) {
			return 2;
		}
		return 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と四角形の当たり判定その２（有限直線）
		@param[in]	org	直線起点
		@param[in]	dir	直線方向
		@param[in]	vlist	時計周り頂点リスト
		@param[out]	crs	交点座標
		@return 当たりなら四角形内三角形の番号 (1: 0-1-2, 2: 2-3-0)
	*/
	//-----------------------------------------------------------------//
	int quadrangle_intersect2(const fvtx& org, const fvtx& end, const fvtxs& vlist, fvtx& crs)
	{
		if(triangle_intersect2(org, end, &vlist[0], crs)) {
			return 1;
		}
		fvtx v[3];
		v[0] = vlist[2];
		v[1] = vlist[3];
		v[2] = vlist[0];
		if(triangle_intersect2(org, end, v, crs)) {
			return 2;
		}
		return 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と球の当たり判定
		@param[in]	r	球の半径
		@param[in]	cen	球の中心
		@param[in]	org	直線起点
		@param[in]	dir	直線方向
		@param[out]	crs	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool sphere_line_collision(float r, const fvtx& cen, const fvtx& org, const fvtx dir, fvtx& crs)
	{
		static const float IKD_EPSIRON = 0.00001f;

		fvtx u = org - cen;

		float a = dot(dir, dir);
		float b = dot(dir, u);
		float c = dot(u, u) - r * r;

		if((a - IKD_EPSIRON) <= 0.0f) {
			// 誤差
			return false;
		}

		float q = b * b - a * c;
		if(q < 0.0f) {	// 衝突しない
			return false;
		}

		float t = ( -b - sqrtf(b * b - a * c) ) / a;
//		pOut_t = t;

		crs = org + (dir * t);

		return true;
	}


	void surface_ratio(const fpos& scr, const fvtxs& suf, const fvtxs& in, fvtx& out)
	{




		out = (in[0] + in[1] + in[2] + in[3]) * 0.25f;
	}

}
