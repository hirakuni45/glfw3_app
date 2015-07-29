#pragma once
//=====================================================================//
/*!	@file
	@brief	Quaternion (四元数)クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <utility>
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
		inline quaternion() : x(0), y(0), z(0), t(1) { }


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
			if(a > vtx::min_value<T>()) {
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
			@param[in]	forward	方向
			@param[in]	up		上方向
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		void look_rotation(const vtx::vertex3<T>& forward, const vtx::vertex3<T>& up)
		{
///			forward.Normalize();
			vtx::vertex3<T> vector1;
			vtx::normalize(forward, vector1);
			vtx::vertex3<T> vector2;
			vtx::vertex3<T> cross;
			vtx::vertex3<T>::cross(up, vector1, cross);
			vtx::normalize(cross, vector2);
			vtx::vertex3<T> vector3;
			vtx::vertex3<T>::cross(vector1, vector2, vector3);
			T m00 = vector2.x;
			T m01 = vector2.y;
			T m02 = vector2.z;
			T m10 = vector3.x;
			T m11 = vector3.y;
			T m12 = vector3.z;
			T m20 = vector1.x;
			T m21 = vector1.y;
			T m22 = vector1.z;

			T num8 = (m00 + m11) + m22;
			if (num8 > 0.0f) {
				T num = std::sqrt(num8 + 1.0f);
				t = num * 0.5f;
				num = 0.5f / num;
				x = (m12 - m21) * num;
				y = (m20 - m02) * num;
				z = (m01 - m10) * num;
				return;
			}
			if((m00 >= m11) && (m00 >= m22)) {
				T num7 = std::sqrt(((1.0f + m00) - m11) - m22);
				T num4 = 0.5f / num7;
				x = 0.5f * num7;
				y = (m01 + m10) * num4;
				z = (m02 + m20) * num4;
				t = (m12 - m21) * num4;
				return;
			}
			if(m11 > m22) {
				T num6 = std::sqrt(((1.0f + m11) - m00) - m22);
				T num3 = 0.5f / num6;
				x = (m10+ m01) * num3;
				y = 0.5f * num6;
				z = (m21 + m12) * num3;
				t = (m20 - m02) * num3;
				return;
			}
			T num5 = std::sqrt(((1.0f + m22) - m00) - m11);
			T num2 = 0.5f / num5;
			x = (m20 + m02) * num2;
			y = (m21 + m12) * num2;
			z = 0.5f * num5;
			t = (m01 - m10) * num2;
#if 0
			vtx::vertex3<T> forward;
			vtx::normalize(lookat, forward);
			vtx::vertex3<T> up;
			vtx::ortho_normalize(up_dir, forward, up);
			vtx::vertex3<T> right;
			vtx::cross(up, forward, right); 

			t = std::sqrt(static_cast<T>(1) + right.x + up.y + forward.z) / static_cast<T>(2);
			T w4_recip = static_cast<T>(1) / (static_cast<T>(4) * t);
//			x = (     up.z - forward.y) * w4_recip;
//			y = (forward.x -   right.z) * w4_recip;
//			z = (  right.y -      up.x) * w4_recip;
			x = (forward.y -      up.z) * w4_recip;
			y = (right.z   - forward.x) * w4_recip;
			z = (up.x      -   right.y) * w4_recip;
#endif
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	マトリックス(OpenGL 系）へ変換
			@return	マトリックス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		mtx::matrix4<T> create_rotate_matrix() const {
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

			mtx::matrix4<T> m;
			m[ 0] = static_cast<T>(1) - y2 - z2;
			m[ 1] = xy + zt;
			m[ 2] = zx - yt;
			m[ 3] = static_cast<T>(0);

			m[ 4] = xy - zt;
			m[ 5] = static_cast<T>(1) - z2 - x2;
			m[ 6] = yz + xt;
			m[ 7] = static_cast<T>(0);

			m[ 8] = zx + yt;
			m[ 9] = yz - xt;
			m[10] = static_cast<T>(1) - x2 - y2;
			m[11] = static_cast<T>(0);

			m[12] = static_cast<T>(0);
			m[13] = static_cast<T>(0);
			m[14] = static_cast<T>(0);
			m[15] = static_cast<T>(1);
			return m;
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
