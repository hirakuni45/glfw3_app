#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL カメラ・クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "gl_fw/gl_info.hpp"
#include "gl_fw/glmatrix.hpp"
#include "gl_fw/glutils.hpp"
#include "utils/quat.hpp"
#include "core/device.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	glcamera クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class camera {

		device::key	translate_key_;
		device::key	rotate_key_;
		device::key	zoom_key_;

		vtx::spos	mouse_pos_;
		vtx::spos	mouse_left_first_pos_;
		vtx::fvtx	eye_first_;
		vtx::fvtx	target_first_;
		vtx::fvtx	up_first_;

		vtx::fpos	quat_handle_;
		vtx::fpos	quat_handle_first_;
		qtx::fquat	quat_;

		matrixf		glmat_;

		vtx::fpos	size_;
		float		aspect_;
		float		fov_;
		float		z_near_;
		float		z_far_;
		vtx::fvtx	eye_;
		vtx::fvtx	target_;
		vtx::fvtx	up_;

		bool		touch_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		camera() :
			translate_key_(device::key::CONTROL),
			rotate_key_(device::key::SHIFT),
			zoom_key_(device::key::ALT),
			mouse_pos_(0), mouse_left_first_pos_(0),
			quat_handle_(0.0f), quat_handle_first_(0.0f), quat_(), glmat_(),
			size_(0.0f), aspect_(1.0f), fov_(30.0f), z_near_(10.0f), z_far_(50000.0f),
			eye_(0.0f, -15.0f, 6.0f),
			target_(0.0f, 0.0f, 0.0f),
			up_(0.0f, 1.0f, 0.0f), touch_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	update
		 */
		//-----------------------------------------------------------------//
		void update()
		{
			using namespace vtx;

			core& core = core::get_instance();
			const device& dev = core.get_device();

			const spos& mspos = dev.get_locator().get_cursor();
///			const spos msdiff = mspos - mouse_pos_;
			mouse_pos_ = mspos;

			bool trans = dev.get_level(translate_key_);
			bool rotate = dev.get_level(rotate_key_);
			bool zoom = dev.get_level(zoom_key_);

			if(trans || rotate || zoom) {
				if(dev.get_positive(device::key::MOUSE_LEFT)) {
					mouse_left_first_pos_ = mspos;
					eye_first_ = eye_;
					target_first_ = target_;
					up_first_ = up_;
					quat_handle_first_ = quat_handle_;
					touch_ = true;
				}
			}

			if(touch_) {
				if(dev.get_level(device::key::MOUSE_LEFT)) {
					// 並行移動
					spos diff_pos = mspos - mouse_left_first_pos_;
					if(trans) {
						fvtx d;
						d.x = static_cast<float>(-diff_pos.x);
						d.y = 0.0f;
						d.z = static_cast<float>(diff_pos.y);
						d *= 0.01f;
						eye_ = eye_first_ + d;
						target_ = target_first_ + d;
					}

					// 回転
					if(rotate) {
						fpos d;
						d.x = static_cast<float>(diff_pos.x);
						d.y = static_cast<float>(diff_pos.y);
						vtx::fpos qh = d * -0.0025f;
						if(quat_.rot_xz(qh.x, qh.y, 1.0f)) {
							glmat_.at_current_matrix() = quat_.create_matrix();
							vtx::fvtx4 av;
							matrixf::vertex_world(glmat_.get_current_matrix(), up_first_, av);
							up_.set(av.x,  av.y,  av.z);
							auto tv = eye_first_ - target_first_;
							matrixf::vertex_world(glmat_.get_current_matrix(), tv, av);
							eye_ = target_first_ + vtx::fvtx(av.x, av.y, av.z);
						}
					}

				// ズーム
					if(zoom) {
						vtx::fpos d(static_cast<float>(diff_pos.x), static_cast<float>(diff_pos.y));
						vtx::fvtx n;
						if(vtx::normalize(target_ - eye_, n)) {
							eye_ = eye_first_ + n * d.y * 0.01f;
						}
					}
				}
			} else {
				touch_ = false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		 */
		//-----------------------------------------------------------------//
		void service()
		{
			core& core = core::get_instance();
///			const device& dev = core.get_device();

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			size_ = core.get_rect().size;
			aspect_ = static_cast<float>(size_.x) / static_cast<float>(size_.y);
			gl::gluPerspectivef(fov_, aspect_, z_near_, z_far_);
			gl::gluLookAtf(eye_.x, eye_.y, eye_.z,
				target_.x, target_.y, target_.z,
				up_.x, up_.y, up_.z);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズ取得
			@return サイズ
		 */
		//-----------------------------------------------------------------//
		const vtx::fpos& get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アスペクト取得
			@return アスペクト
		 */
		//-----------------------------------------------------------------//
		float get_aspect() const { return aspect_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	fov 取得
			@return fov
		 */
		//-----------------------------------------------------------------//
		float get_fov() const { return  fov_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	near 取得
			@return near
		 */
		//-----------------------------------------------------------------//
		float get_near() const { return z_near_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	far 取得
			@return far
		 */
		//-----------------------------------------------------------------//
		float get_far() const { return z_far_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	eye 設定
			@param[in]	eye	位置
		 */
		//-----------------------------------------------------------------//
		void set_eye(const vtx::fvtx& eye) { eye_ = eye; }


		//-----------------------------------------------------------------//
		/*!
			@brief	eye 取得
			@return eye
		 */
		//-----------------------------------------------------------------//
		const vtx::fvtx& get_eye() const { return eye_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	target 設定
			@param[in]	target	位置
		 */
		//-----------------------------------------------------------------//
		void set_target(const vtx::fvtx& target) { target_ = target; }


		//-----------------------------------------------------------------//
		/*!
			@brief	target 取得
			@return target
		 */
		//-----------------------------------------------------------------//
		const vtx::fvtx& get_target() const { return target_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カメラ上向き設定
			@param[in]	up	方向
		 */
		//-----------------------------------------------------------------//
		void set_up(const vtx::fvtx& up) { up_ = up; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カメラ上向き取得
			@return カメラ上方向
		 */
		//-----------------------------------------------------------------//
		const vtx::fvtx& get_up() const { return up_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート中にあった操作
			@return 「true」なら操作有り
		 */
		//-----------------------------------------------------------------//
		bool get_touch() const { return touch_; }
	};

}
