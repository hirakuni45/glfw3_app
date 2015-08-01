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
		@brief	quaternion (四元数)テンプレート・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename T>
	class quaternion {
	public:
		T		x;
		T		y;
		T		z;
		T		w;

		typedef T	value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	quaternion コンストラクター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline quaternion() : x(0), y(0), z(0), w(1) { }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	quaternion コンストラクター
			@param[in]	xx	X	要素
			@param[in]	yy	Y	要素
			@param[in]	zz	Z	要素
			@param[in]	ww	w	要素
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline quaternion(T xx, T yy, T zz, T ww = static_cast<T>(1)) : x(xx), y(yy), z(zz), w(ww) { }

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	quaternion 要素を設定
			@param[in]	xx	X	要素
			@param[in]	yy	Y	要素
			@param[in]	zz	Z	要素
			@param[in]	ww	w	要素
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void set(T xx, T yy, T zz, T ww = static_cast<T>(1)) { x = xx; y = yy; z = zz; w = ww; }


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	normalise @n
			normalising a quaternion works similar to a vector. This method will
			not do anything
			if the quaternion is close enough to being unit-length.
			define TOLERANCE as something small like 0.00001f to get accurate results
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		void normalise() {
			T TOLERANCE = static_cast<T>(0.000001);
			// Don't normalize if we don't have to
			T mag2 = w * w + x * x + y * y + z * z;
			if(std::abs(mag2) > TOLERANCE && fabs(mag2 - static_cast<T>(1)) > TOLERANCE) {
				T mag = std::sqrt(mag2);
				w /= mag;
				x /= mag;
				y /= mag;
				z /= mag;
			}
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	We need to get the inverse of a quaternion to properly
					apply a quaternion-rotation to a vector
					The conjugate of a quaternion is the same as the inverse,
					as long as the quaternion is unit-length
			@return 答え
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		quaternion get_conjugate() const {
			return std::move(quaternion(-x, -y, -z, w));
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	quaternion 掛け算
			@param[in]	left	左辺
			@param[in]	right	右辺
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		void mult(const quaternion& left, const quaternion& right) {
			T d1, d2, d3, d4;
			d1 =  left.w * right.w;
			d2 = -left.x * right.x;
			d3 = -left.y * right.y;
			d4 = -left.z * right.z;
			T ww = d1 + d2 + d3 + d4;

			d1 =   left.w * right.x;
			d2 =  right.w *  left.x;
			d3 =   left.y * right.z;
			d4 =  -left.z * right.y;
			T xx =  d1 + d2 + d3 + d4;

			d1 =   left.w * right.y;
			d2 =  right.w *  left.y;
			d3 =   left.z * right.x;
			d4 =  -left.x * right.z;
			T yy =  d1 + d2 + d3 + d4;

			d1 =   left.w * right.z;
			d2 =  right.w *  left.z;
			d3 =   left.x * right.y;
			d4 =  -left.y * right.x;
			T zz =  d1 + d2 + d3 + d4;
			w = ww;
			x = xx;
			y = yy;
			z = zz;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	quaternion 掛け算(右辺）
			@param[in]	left	左辺
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		inline void mult_left(const quaternion& left) {
			mult(left, *this);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	quaternion 掛け算(右辺）
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
				w = std::cos(ar);
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
				w = std::cos(ar);
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
			w = x = y = z = static_cast<T>(0);
			T n = AxisX * AxisX + AxisY * AxisY + AxisZ * AxisZ;
			if(n <= static_cast<T>(0)) return;

			n = static_cast<T>(1) / std::sqrt(n);

			T si, co;
			mtx::deg_sin_cos_(deg / static_cast<T>(2), si, co);
			w = co;
			si *= n;
			x = si * AxisX;
			y = si * AxisY;
			z = si * AxisZ;
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ベクトルの回転
			@param[in]	vec	ベクトル
			@return 出力
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		vtx::vertex3<T> rotate(const vtx::vertex3<T>& vec) const {
			vtx::vertex3<T> vn;
			vtx::normalize<T>(vec, vn);
 
			quaternion vecQ(vn.x, vn.y, vn.z, 0);
			quaternion resQ = vecQ * get_conjugate();
			resQ = *this * resQ;
 
			vtx::vertex3<T> out(resQ.x, resQ.y, resQ.z);
			return std::move(out);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	AXIS angle
			@param[in]	v	回転軸
			@param[in]	angle	角度
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		void from_axis(const vtx::vertex3<T>& v, T angle) {
			T sinAngle;
			angle *= static_cast<T>(0.5);
			vtx::vertex3<T> vn;
			vtx::normalize(v, vn);
 
			T si = std::sin(angle);
 
			x = vn.x * si;
			y = vn.y * si;
			z = vn.z * si;
			w = std::cos(angle);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Convert to Axis/Angles
			@param[in]	v	回転軸
			@param[in]	angle	角度
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		vtx::vertex4<T> get_axis_angle() const {
			T scale = sqrt(x * x + y * y + z * z);
			vtx::vertex4<T> ans;
			ans.x = x / scale;
			ans.y = y / scale;
			ans.z = z / scale;
			ans.w = std::acos(w) * static_cast<T>(2);
			return std::move(ans);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	指定方向に向ける
			@param[in]	lookat	方向
			@param[in]	up_dir	上方向
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		void look_rotation(const vtx::vertex3<T>& lookat, const vtx::vertex3<T>& up_dir)
		{
#if 0
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
				w = num * 0.5f;
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
				w = (m12 - m21) * num4;
				return;
			}
			if(m11 > m22) {
				T num6 = std::sqrt(((1.0f + m11) - m00) - m22);
				T num3 = 0.5f / num6;
				x = (m10+ m01) * num3;
				y = 0.5f * num6;
				z = (m21 + m12) * num3;
				w = (m20 - m02) * num3;
				return;
			}
			T num5 = std::sqrt(((1.0f + m22) - m00) - m11);
			T num2 = 0.5f / num5;
			x = (m20 + m02) * num2;
			y = (m21 + m12) * num2;
			z = 0.5f * num5;
			w = (m01 - m10) * num2;
#endif
#if 1
			vtx::vertex3<T> forward;
			vtx::normalize(lookat, forward);
			vtx::vertex3<T> up;
			vtx::ortho_normalize(forward, up_dir, up);
			vtx::vertex3<T> right;
			vtx::cross(up, forward, right); 

			w = std::sqrt(static_cast<T>(1) + right.x + up.y + forward.z) / static_cast<T>(2);
			T w4_recip = static_cast<T>(1) / (static_cast<T>(4) * w);
			x = (     up.z - forward.y) * w4_recip;
			y = (forward.x -   right.z) * w4_recip;
			z = (  right.y -      up.x) * w4_recip;
//			x = (forward.y -      up.z) * w4_recip;
//			y = (right.z   - forward.x) * w4_recip;
//			z = (up.x      -   right.y) * w4_recip;
#endif
#if 0
			Quaternion Quaternion::LookRotation(Vector& lookAt, Vector& upDirection) {
				Vector forward = lookAt; Vector up = upDirection;
				Vector::OrthoNormalize(&forward, &up);
				Vector right = Vector::Cross(up, forward);

#define m00 right.x
#define m01 up.x
#define m02 forward.x
#define m10 right.y
#define m11 up.y
#define m12 forward.y
#define m20 right.z
#define m21 up.z
#define m22 forward.z

				Quaternion ret;
				ret.w = sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
				float w4_recip = 1.0f / (4.0f * ret.w);
				ret.x = (m21 - m12) * w4_recip;
				ret.y = (m02 - m20) * w4_recip;
				ret.z = (m10 - m01) * w4_recip;
#endif
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	マトリックス(OpenGL 系）へ変換
			@return	マトリックス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		mtx::matrix4<T> create_matrix() const {
			T x2 = x * x;
			T y2 = y * y;
			T z2 = z * z;
			T xy = x * y;
			T xz = x * z;
			T yz = y * z;
			T wx = w * x;
			T wy = w * y;
			T wz = w * z;
			mtx::matrix4<T> m;
			T c1 = static_cast<T>(1);
			T c2 = static_cast<T>(2);
			m[ 0] = c1 - c2 * (y2 + z2);
			m[ 4] = c2 * (xy - wz);
			m[ 8] = c2 * (xz + wy);
			m[12] = 0;

			m[ 1] = c2 * (xy + wz);
			m[ 5] = c1 - c2 * (x2 + z2);
			m[ 9] = c2 * (yz - wx);
			m[13] = 0;

			m[ 2] = c2 * (xz - wy);
			m[ 6] = c2 * (yz + wx);
			m[10] = c1 - c2 * (x2 + y2);
			m[14] = 0;

			m[ 3] = 0;
			m[ 7] = 0;
			m[11] = 0;
			m[15] = c1;

			return std::move(m);
		}


		// Multiplying q1 with q2 applies the rotation q2 to q1
		quaternion operator * (const quaternion& rq) const {
			// the constructor takes its arguments as (x, y, z, w)
		   	return quaternion(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
		   					  w * rq.y + y * rq.w + z * rq.x - x * rq.z,
		   					  w * rq.z + z * rq.w + x * rq.y - y * rq.x,
		   					  w * rq.w - x * rq.x - y * rq.y - z * rq.z);
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
