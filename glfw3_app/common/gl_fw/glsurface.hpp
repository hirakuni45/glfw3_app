#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL サーフェース・クラス（ヘッダー）@n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <iostream>
#include "utils/vtx.hpp"
#include "gl_fw/gl_info.hpp"
#include "gl_fw/glutils.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	surface クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct surface {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	描画タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class draw_type {
			line,
			fill,
		};

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	サーフェース頂点・クラス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct vertex {
			vtx::fpos	uv;
			vtx::fvtx	normal;
			vtx::fvtx	position;
			vertex() : uv(0), normal(0), position(0) { }
		};

		typedef std::vector<vertex> vertices;
		typedef std::vector<uint16_t> indices;

	private:
		uint32_t	divide_;
		vtx::fvtx	radius_;
		vertices	vertices_;
 		indices		indices_;

		uint32_t	vertex_id_;
		uint32_t	index_id_;

		bool		sphere_;

		void create_dome_vertex_(const vtx::fvtx& rad, uint32_t div);
		void make_triangle_dome_();
		void make_triangle_sphere_();
		void gen_glbuffer_();
		void destroy_() {
			if(vertex_id_) {
				glDeleteBuffers(1, &vertex_id_);
				vertex_id_ = 0;
			}
			if(index_id_) {
				glDeleteBuffers(1, &index_id_);
				index_id_ = 0;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		surface() : divide_(0), radius_(0.0f), vertices_(), indices_(),
					vertex_id_(0), index_id_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		 */
		//-----------------------------------------------------------------//
		~surface() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	球の作成
			@param[in]	rad		半径（高さ：Z）
			@param[in]	div		分割数
		 */
		//-----------------------------------------------------------------//
		void create_sphere(const vtx::fvtx& rad, uint32_t div = 16);


		//-----------------------------------------------------------------//
		/*!
			@brief	ドームの作成
			@param[in]	rad		半径（高さ：Z）
			@param[in]	div		分割数
		 */
		//-----------------------------------------------------------------//
		void create_dome(const vtx::fvtx& rad, uint32_t div = 16);


		//-----------------------------------------------------------------//
		/*!
			@brief	頂点コンテナを取得
			@return 頂点コンテナ
		 */
		//-----------------------------------------------------------------//
		const vertices& get_vertices() { return vertices_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	描画
			@param[in]	type	描画タイプ
		 */
		//-----------------------------------------------------------------//
		void draw(draw_type type) const;
	};
}
