#pragma once
//=====================================================================//
/*!	@file
	@brief	Quaternion (四元数)クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <cmath>
#include "utils/vtx.hpp"
#include "utils/mtx.hpp"

namespace qtx {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Quaternion (四元数)テンプレート・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	class quaternion {
	public:
		T		x;
		T		y;
		T		z;
		T		t;

		typedef T	value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Quaternion コンストラクター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline quaternion() : t(1), x(0), y(0), z(0) { }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Quaternion コンストラクター
			@param[in]	xx	X	要素
			@param[in]	yy	Y	要素
			@param[in]	zz	Z	要素
			@param[in]	tt	t	要素
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline quaternion(T xx, T yy, T zz, T tt = static_cast<T>(1)) : t(tt), x(xx), y(yy), z(zz) { }

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Quaternion 要素を設定
			@param[in]	xx	X	要素
			@param[in]	yy	Y	要素
			@param[in]	zz	Z	要素
			@param[in]	tt	t	要素
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void set(T xx, T yy, T zz, T tt = static_cast<T>(1)) { x = xx; y = yy; z = zz; t = tt; }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Quaternion 掛け算
			@param[in]	left	左辺
			@param[in]	right	右辺
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void mult(const quaternion& left, const quaternion& right) {
			T d1, d2, d3, d4;
			d1 =  left.t * right.t;
			d2 = -left.x * right.x;
			d3 = -left.y * right.y;
			d4 = -left.z * right.z;
			T tt = d1 + d2 + d3 + d4;

			d1 =   left.t * right.x;
			d2 =  right.t *  left.x;
			d3 =   left.y * right.z;
			d4 =  -left.z * right.y;
			T xx =  d1 + d2 + d3 + d4;

			d1 =   left.t * right.y;
			d2 =  right.t *  left.y;
			d3 =   left.z * right.x;
			d4 =  -left.x * right.z;
			T yy =  d1 + d2 + d3 + d4;

			d1 =   left.t * right.z;
			d2 =  right.t *  left.z;
			d3 =   left.x * right.y;
			d4 =  -left.y * right.x;
			T zz =  d1 + d2 + d3 + d4;
			t = tt;
			x = xx;
			y = yy;
			z = zz;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Quaternion 掛け算(右辺）
			@param[in]	left	左辺
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void mult_left(const quaternion& left) {
			mult(left, *this);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Quaternion 掛け算(右辺）
			@param[in]	right	右辺
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void mult_right(const quaternion& right) {
			mult(*this, right);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	X,Y の距離を使って回転変換
			@param[in]	dx	X 軸方向回転角度
			@param[in]	dy	Y 軸方向回転角度
			@param[in]	scale	スケール（腕の長さ）
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline bool rot_xy(T dx, T dy, T scale) {
			T a = std::sqrt(dx * dx + dy * dy);
		// 回転のクォータニオン dq を求める
			T min;
			vtx::min_level(min);
			if(a > min) {
				T ar = a * scale * 0.5f;
				T as = std::sin(ar) / a;
				t = std::cos(ar);
				x = dy * as;
				y = dx * as;
				z = static_cast<T>(0);
				return true;
			} else {
				return false;
			}
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	X,Y の距離を使って回転変換
			@param[in]	dx	X 軸方向回転角度
			@param[in]	dy	Y 軸方向回転角度
			@param[in]	scale	スケール（腕の長さ）
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline bool rot_xz(T dx, T dy, T scale) {
			T a = std::sqrt(dx * dx + dy * dy);
		// 回転のクォータニオン dq を求める
			T min;
			vtx::min_level(min);
			if(a > min) {
				T ar = a * scale * 0.5f;
				T as = std::sin(ar) / a;
				t = std::cos(ar);
				x = dy * as;
				y = static_cast<T>(0);
				z = dx * as;
				return true;
			} else {
				return false;
			}
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	回転軸を指定して回転変換
			@param[in]	deg	回転角度
			@param[in]	AxisX	回転軸の X 座標
			@param[in]	AxisY	回転軸の Y 座標
			@param[in]	AxisZ	回転軸の Z 座標
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void rotate(T deg, T AxisX, T AxisY, T AxisZ) {
			t = x = y = z = static_cast<T>(0);
			T n = AxisX * AxisX + AxisY * AxisY + AxisZ * AxisZ;
			if(n <= static_cast<T>(0)) return;

			n = static_cast<T>(1) / std::sqrt(n);

			T si, co;
			mtx::deg_sin_cos_(deg / static_cast<T>(2), si, co);
			t = co;
			si *= n;
			x = si * AxisX;
			y = si * AxisY;
			z = si * AxisZ;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	指定方向に向ける
			@param[in]	lookat	回転方向
			@param[in]	up_dir	上方向
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		void look_rotation(const vtx::fvtx& lookat, const vtx::fvtx& up_dir)
		{
			vtx::fvtx forward;
			vtx::normalize(lookat, forward);
			vtx::fvtx up;
			vtx::ortho_normalize(up_dir, forward, up);
			vtx::fvtx right;
			vtx::cross(up, forward, right); 

			t = std::sqrt(1.0f + right.x + up.y + forward.z) * 0.5f;
			T w4_recip = 1.0f / (4.0f * t);
			x = (     up.z - forward.y) * w4_recip;
			y = (forward.x -   right.z) * w4_recip;
			z = (  right.y -      up.x) * w4_recip;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	マトリックス(OpenGL 系）へ変換
			@param[out]	m	マトリックス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void create_rotate_matrix(mtx::matrix4<T>& m) {
			T cc = static_cast<T>(2);
			T x2 = x * x * cc;
			T y2 = y * y * cc;
			T z2 = z * z * cc;
			T xy = x * y * cc;
			T yz = y * z * cc;
			T zx = z * x * cc;
			T xt = x * t * cc;
			T yt = y * t * cc;
			T zt = z * t * cc;

			m.m[ 0] = static_cast<T>(1) - y2 - z2;
			m.m[ 1] = xy + zt;
			m.m[ 2] = zx - yt;
			m.m[ 3] = static_cast<T>(0);

			m.m[ 4] = xy - zt;
			m.m[ 5] = static_cast<T>(1) - z2 - x2;
			m.m[ 6] = yz + xt;
			m.m[ 7] = static_cast<T>(0);

			m.m[ 8] = zx + yt;
			m.m[ 9] = yz - xt;
			m.m[10] = static_cast<T>(1) - x2 - y2;
			m.m[11] = static_cast<T>(0);

			m.m[12] = static_cast<T>(0);
			m.m[13] = static_cast<T>(0);
			m.m[14] = static_cast<T>(0);
			m.m[15] = static_cast<T>(1);
		}

	};

	//---------------------------------------------------------------------//
	/*!
		@brief	float 型 Quaternion (四元数) クラス
	*/
	//---------------------------------------------------------------------//
	typedef quaternion<float>	fquat;


	//---------------------------------------------------------------------//
	/*!
		@brief	double 型 Quaternion (四元数) クラス
	*/
	//---------------------------------------------------------------------//
	typedef quaternion<double>	dquat;

}

