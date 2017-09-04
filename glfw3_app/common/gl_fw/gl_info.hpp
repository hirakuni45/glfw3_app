#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL/ES 依存を吸収する為のヘッダー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <boost/assert.hpp>

namespace gl {

#ifndef OPENGL_ES
	inline void glOrthof(float l, float r, float b, float t, float aa, float bb) {
		glOrtho(l, r, b, t, aa, bb);
	}

	inline void glFrustumf(float l, float r, float b, float t, float aa, float bb) {
		glFrustum(l, r, b, t, aa, bb);
	}

	inline void gluPerspectivef(float ang, float aspect, float ne, float fa) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
		gluPerspective(ang, aspect, ne, fa);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
	}

	inline void gluLookAtf(float eye_x, float eye_y, float eye_z, float tgt_x, float tgt_y, float tgt_z, float up_x, float up_y, float up_z) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
		gluLookAt(eye_x, eye_y, eye_z, tgt_x, tgt_y, tgt_z, up_x, up_y, up_z);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
	}

	inline void glOrtho(double l, double r, double b, double t, double aa, double bb) {
		glOrthof(static_cast<float>(l), static_cast<float>(r),
				   static_cast<float>(b), static_cast<float>(t),
				   static_cast<float>(aa), static_cast<float>(bb));
	}

	inline void glFrustum(double l, double r, double b, double t, double aa, double bb) {
		glFrustumf(static_cast<float>(l), static_cast<float>(r),
				 static_cast<float>(b), static_cast<float>(t),
				 static_cast<float>(aa), static_cast<float>(bb));
	}

	inline void glColor3ub(GLubyte r, GLubyte g, GLubyte b) {
		glColor4ub(r, g, b, 255);
	}

	inline void glColor3f(float r, float g, float b) {
		glColor4ub(static_cast<unsigned char>(r * 255.0f),
					 static_cast<unsigned char>(g * 255.0f),
					 static_cast<unsigned char>(b * 255.0f),
					 255);
	}

	inline void glColor4f(float r, float g, float b, float a) {
		glColor4ub(static_cast<unsigned char>(r * 255.0f),
					 static_cast<unsigned char>(g * 255.0f),
					 static_cast<unsigned char>(b * 255.0f),
					 static_cast<unsigned char>(a * 255.0f));
	}
#endif

	inline void glTranslatei(int x, int y) {
		glTranslatef(static_cast<float>(x), static_cast<float>(y), 0.0f);
	}

	inline void glTranslatei(int x, int y, int z) {
		glTranslatef(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	}

}
