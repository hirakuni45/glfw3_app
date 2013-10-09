#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL カメラ・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/gl_info.hpp"
#include "utils/quat.hpp"
#include "gl_fw/glmatrix.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	gl_camera クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class camera {

		device::key::type	translate_key_;
		device::key::type	rotate_key_;
		device::key::type	zoom_key_;

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
		camera() :
			translate_key_(device::key::CONTROL),
			rotate_key_(device::key::SHIFT),
			zoom_key_(device::key::ALT),
			mouse_pos_(0), mouse_left_first_pos_(0),
			quat_handle_(0.0f), quat_handle_first_(0.0f), quat_(), glmat_(),
			size_(0.0f), aspect_(0.0f), fov_(45.0f), z_near_(5.0f), z_far_(5000.0f),
			eye_(0.0f, -15.0f, 6.0f),
			target_(0.0f, 0.0f, 0.0f),
			up_(0.0f, 1.0f, 0.0f), touch_(false) { glmat_.initialize(); }

		void update();

		void service();

		const vtx::fpos& get_size() const { return size_; }
		float get_aspect() const { return aspect_; }
		float get_fov() const { return  fov_; }
		float get_near() const { return z_near_; }
		float get_far() const { return z_far_; }
		const vtx::fvtx& get_eye() const { return eye_; }
		const vtx::fvtx& get_target() const { return target_; }
		const vtx::fvtx& get_up() const { return up_; }
		bool get_touch() const { return touch_; }
	};

}
