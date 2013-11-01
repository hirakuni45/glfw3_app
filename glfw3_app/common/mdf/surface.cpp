//=====================================================================//
/*!	@file
	@brief	サーフェース・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "mdf/surface.hpp"

namespace mdf {

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
