#pragma once
//=====================================================================//
/*!	@file
	@brief	線形代数的数学ユーティリティー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/vtx.hpp"

namespace vtx {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	配置型
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct placement {
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	水平配置型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct holizontal {
			enum type {
				NONE,		///< 無効果
				LEFT,		///< 左
				CENTER,		///< 中央
				RIGHT,		///< 右
			};
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	垂直配置型
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct vertical {
			enum type {
				NONE,		///< 無効果
				TOP,		///< 上
				CENTER,		///< 中央
				BOTTOM,		///< 下
			};
		};


		holizontal::type	hpt;
		vertical::type		vpt;
		placement(holizontal::type hpt_ = holizontal::NONE,
				  vertical::type vpt_ = vertical::NONE) : hpt(hpt_), vpt(vpt_) { }
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	配置を作成
		@param[in]	area	エリア
		@param[in]	size	配置サイズ
		@param[in]	pt		配置型
		@param[out]	dst		位置
	*/
	//-----------------------------------------------------------------//
	template <typename T>
	void create_placement(const rectangle<T>& area, const vertex2<T>& size,
		const placement& pt, vertex2<T>& dst)
	{
		if(pt.hpt == placement::holizontal::LEFT) {
			dst.x = area.org.x;
		} else if(pt.hpt == placement::holizontal::CENTER) {
			dst.x = area.org.x + (area.size.x - size.x) / 2;
		} else if(pt.hpt == placement::holizontal::RIGHT) {
			dst.x = area.end_x() - size.x;
		}

		if(pt.vpt == placement::vertical::TOP) {
			dst.y = area.org.y;
		} else if(pt.vpt == placement::vertical::CENTER) {
			dst.y = area.org.y + (area.size.y - size.y) / 2;
		} else if(pt.vpt == placement::vertical::BOTTOM) {
			dst.y = area.end_y() - size.y;
		}
	}


#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	直線と三角の当たり判定
		@param[in]	p	直線座標ｐ
		@param[in]	q	直線座標q
		@param[in]	v	三角の座標列
		@param[out]	cp	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool line_hit_triangle(const fvtx& p, const fvtx& q, const fvtx v[3], fvtx& cp);
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	三角形内に点が存在するか（二次元空間）
		@param[in]	pos	点の座標
		@param[in]	v0	三角の座標 0
		@param[in]	v1	三角の座標 1
		@param[in]	v2	三角の座標 2
		@return あれば「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool triangle_in_point(const fpos& pos, const fvtx& v0, const fvtx& v1, const fvtx& v2);


	//-----------------------------------------------------------------//
	/*!
		@brief	多角形内に点が存在するか
		@param[in]	pos	点の座標
		@param[in]	src	頂点の座標列
		@return あれば「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool polygon_in_point(const fpos& pos, const fposs& src);


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と三角の当たり判定
		@param[in]	org	直線起点
		@param[in]	dir	直線方向
		@param[in]	tv	三角の座標列
		@param[out]	crs	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool triangle_intersect(const fvtx& org, const fvtx& dir, const fvtx tv[3], fvtx& crs);


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と三角の当たり判定２（有限直線）
		@param[in]	org	直線起点
		@param[in]	end	直線方向
		@param[in]	tv	三角の座標列
		@param[out]	crs	交点座標
		@return 当たりなら「true」を返す
	*/
	//-----------------------------------------------------------------//
	bool triangle_intersect2(const fvtx& org, const fvtx& end, const fvtx tv[3], fvtx& crs);


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と四角形の当たり判定
		@param[in]	org	直線起点
		@param[in]	dir	直線方向
		@param[in]	vlist	時計周り頂点リスト
		@param[out]	crs	交点座標
		@return 当たりなら四角形内三角形の番号 (1: 0-1-2, 2: 2-3-0)
	*/
	//-----------------------------------------------------------------//
	int quadrangle_intersect(const fvtx& org, const fvtx& dir, const fvtxs& vlist, fvtx& crs);


	//-----------------------------------------------------------------//
	/*!
		@brief	直線と四角形の当たり判定その２（有限直線）
		@param[in]	org	直線起点
		@param[in]	end	直線終点
		@param[in]	vlist	時計周り頂点リスト
		@param[out]	crs	交点座標
		@return 当たりなら四角形内三角形の番号 (1: 0-1-2, 2: 2-3-0)
	*/
	//-----------------------------------------------------------------//
	int quadrangle_intersect2(const fvtx& org, const fvtx& end, const fvtxs& vlist, fvtx& crs);


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
	bool sphere_line_collision(float r, const fvtx& cen, const fvtx& org, const fvtx dir, fvtx& crs);


	void surface_ratio(const fpos& scr, const fvtxs& suf, const fvtxs& in, fvtx& out);

}	// namespace vtx

