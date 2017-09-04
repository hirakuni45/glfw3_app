//=====================================================================//
/*!	@file
	@brief	サーフェース・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "mdf/surface.hpp"

namespace mdf {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	球を描画
		@param[in]	radius	半径
		@param[in]	lats	分割数
		@param[in]	longs	分割数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void draw_sphere(float radius, int lats, int longs)
	{
		for(int i = 0; i <= lats; ++i) {
			float lat0 = vtx::get_pi<float>() * (-0.5f + static_cast<float>(i - 1) / lats);
			float z0  = radius * std::sin(lat0);
			float zr0 = radius * std::cos(lat0);

			float lat1 = vtx::get_pi<float>() * (-0.5f + static_cast<float>(i) / lats);
			float z1  = radius * std::sin(lat1);
			float zr1 = radius * std::cos(lat1);

			glBegin(GL_QUAD_STRIP);
			for(int j = 0; j <= longs; ++j) {
				float lng = 2 * vtx::get_pi<float>() * static_cast<float>(j - 1) / longs;
				float x = std::cos(lng);
				float y = std::sin(lng);
				glNormal3f(x * zr1, y * zr1, z1);
				glVertex3f(x * zr1, y * zr1, z1);
				glNormal3f(x * zr0, y * zr0, z0);
				glVertex3f(x * zr0, y * zr0, z0);
			}
			glEnd();
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シリンダーを描画
		@param[in]	radius_org	半径（開始）
		@param[in]	radius_end	半径（終点）
		@param[in]	length		長さ
		@param[in]	lats	分割数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void draw_cylinder(float radius_org, float radius_len, float length, int lats)
	{
		int divide = 12;
		float a = 0.0f;
		float d = 2.0f * vtx::get_pi<float>() / static_cast<float>(divide);
		glBegin(GL_TRIANGLE_STRIP);
		for(int i = 0; i < divide; ++i) {
			float x = std::sin(a);
			float y = std::cos(a);
			a += d;
			glVertex3f(x * radius_org, 0.0f,   y * radius_org);
			glVertex3f(x * radius_len, length, y * radius_len);
		}
		{
			a = 0.0f;
			float x = std::sin(a);
			float y = std::cos(a);
			glVertex3f(x * radius_org, 0.0f,   y * radius_org);
			glVertex3f(x * radius_len, length, y * radius_len);
		}
		glEnd();
	}


#if 0
	void surface::destroy_vertex_()
	{
		if(vertex_id_) {
			glDeleteBuffers(1, &vertex_id_);
			vertex_id_ = 0;
		}
	}

	void surface::destroy_element_()
	{
		if(!elements_.empty()) {
			glDeleteBuffers(elements_.size(), &elements_[0]);
			elements_.clear();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	頂点を開始
		@param[in]	n	頂点数を指定する場合指定
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool surface::begin_vertex(uint32_t n)
	{
		destroy_vertex_();

		glGenBuffers(1, &vertex_id_);

		if(n) {
			vertexes_.reserve(n);
		}
		vertexes_.clear();

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	頂点を追加
		@param[in]	n	数を指定する場合
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool surface::add_vertex(const vertex& v)
	{
		vertexes_.push_back(v);
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	頂点を終了
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool surface::end_vertex()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertex_id_);
		glBufferData(GL_ARRAY_BUFFER, vertexes_.size() * sizeof(vertex),
			&vertexes_[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		vertexes tmp;
		tmp.swap(vertexes_);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
		@param[in]	h	ハンドル（０なら全てをレンダリング）
	*/
	//-----------------------------------------------------------------//
	void surface::render(handle h)
	{
		if(h == 0) {

		} else if((h - 1) < elements_.size()) {

		}
	}
#endif
}
