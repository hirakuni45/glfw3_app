//=====================================================================//
/*!	@file
	@brief	OpenGL カメラ・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/IGLcore.hpp"
#include "gl_fw/glcamera.hpp"
#include "gl_fw/glmatrix.hpp"

namespace gl {

	void camera::update()
	{
		using namespace vtx;

		IGLcore* glc = get_glcore();
		const device& dev = glc->get_device();

		const spos& mspos = dev.get_cursor();
		const spos msdiff = mspos - mouse_pos_;
		mouse_pos_ = mspos;

		bool touch = false;

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
			}
		}

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
				touch = true;
			}

			// 回転
			if(rotate) {
				fpos d;
				d.x = static_cast<float>(diff_pos.x);
				d.y = static_cast<float>(diff_pos.y);
				vtx::fpos qh = d * -0.005f;
				qtx::fquat q;
				if(q.rot_xz(qh.x, qh.y, 1.0f)) {
					q.create_rotate_matrix(glmat_.at_current_matrix());
					vtx::fvtx4 fv;
					matrixf::vertex_world(glmat_.get_current_matrix(), up_first_, fv);
					up_.set(fv.x,  fv.y,  fv.z);
					matrixf::vertex_world(glmat_.get_current_matrix(), eye_first_ - target_first_, fv);
					eye_ = target_first_ + vtx::fvtx(fv.x, fv.y, fv.z);
					touch = true;
				}
			}

			// ズーム
			if(zoom) {
				vtx::fpos d(static_cast<float>(diff_pos.x), static_cast<float>(diff_pos.y));
				vtx::fvtx n;
				if(vtx::normalize(target_ - eye_, n)) {
					eye_ = eye_first_ + n * d.y * 0.01f;
					touch = true;
				}
			}
		}
		touch_ = touch;
	}


	void camera::service()
	{
		IGLcore* igl = get_glcore();
		const device& dev = igl->get_device();

		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		size_ = igl->get_size();
		aspect_ = size_.x / size_.y;
		::gluPerspective(fov_, aspect_, z_near_, z_far_);
		::gluLookAt(eye_.x, eye_.y, eye_.z,
			target_.x, target_.y, target_.z,
			up_.x, up_.y, up_.z);
//		const mtx::fmat4& m = glmat_.get_current_matrix();
//		glMultMatrixf(m());

		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();
	}

}

