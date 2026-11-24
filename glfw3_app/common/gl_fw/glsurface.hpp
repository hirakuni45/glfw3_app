#pragma once
//=========================================================================//
/*!	@file
	@brief	OpenGL サーフェース・クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
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

		void destroy_() noexcept
		{
			if(vertex_id_) {
				glDeleteBuffers(1, &vertex_id_);
				vertex_id_ = 0;
			}
			if(index_id_) {
				glDeleteBuffers(1, &index_id_);
				index_id_ = 0;
			}
		}

		void create_dome_vertex_(const vtx::fvtx& rad, uint32_t div) noexcept
		{
			divide_ = div;
			radius_ = rad;

			uint32_t qd = div / 4;
			vertices_.resize(div * qd + 1);
			vertices_.clear();

			float a = 0.0f;
			float b = 0.0f;
			float d = vtx::get_pi<float>() * 2.0f / static_cast<float>(div);
			for(uint32_t j = 0; j <= qd; ++j) {
				for(uint32_t i = 0; i < div; ++i) {
					vertex v;
					v.position.x = std::cos(a) * std::cos(b) * rad.x;
					v.position.y = std::sin(a) * std::cos(b) * rad.y;
					v.position.z = std::sin(b) * rad.z;
					v.normal = vtx::normalize(v.position);
					vtx::fpos uv(std::cos(a), std::sin(a));
					uv *= static_cast<float>(qd - j) / static_cast<float>(qd);
					uv += vtx::fpos(1.0f);
					v.uv = uv * 0.5f;
					vertices_.push_back(v);
					if(j == qd) {
						break;
					}
					a += d;
				}
				b += d;
			}
		}

		void make_triangle_dome_() noexcept
		{
			uint32_t qdv = divide_ / 4;
			indices_.clear();
			for(uint32_t j = 0; j < qdv; ++j) {
				uint32_t nj = j + 1;
				for(uint32_t i = 0; i < divide_; ++i) {
					uint32_t ni = (i + 1) % divide_;
					if(nj < qdv) {
						indices_.push_back( j * divide_ +  i);
						indices_.push_back( j * divide_ + ni);
						indices_.push_back(nj * divide_ + ni);

						indices_.push_back(nj * divide_ + ni);
						indices_.push_back(nj * divide_ +  i);
						indices_.push_back( j * divide_ +  i);
					} else {
						indices_.push_back( j * divide_ +  i);
						indices_.push_back( j * divide_ + ni);
						indices_.push_back(nj * divide_);
					}
				}
			}
		}

		void make_triangle_sphere_() noexcept
		{
		}

		void gen_glbuffer_() noexcept
		{
			destroy_();

			// 頂点バッファの作成
			glGenBuffers(1, &vertex_id_);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_id_);
			glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(vertex), &vertices_[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// インデックス・バッファの作成
			glGenBuffers(1, &index_id_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(indices::value_type),
				&indices_[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		surface() noexcept :
			divide_(0), radius_(0.0f), vertices_(), indices_(),
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
		void create_sphere(const vtx::fvtx& rad, uint32_t div = 16) noexcept
		{
			if(rad.x <= 0.0f || rad.y <= 0.0f || rad.z <= 0.0f) return;

			if(div < 8) div = 8;

			create_dome_vertex_(rad, div);
			for(uint32_t i = div; i < vertices_.size(); ++i) {
				auto v = vertices_[i];
				v.position.z = -v.position.z;
				v.normal = vtx::normalize(v.position);
				vertices_.push_back(v);
			}
			make_triangle_sphere_();
			gen_glbuffer_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ドームの作成
			@param[in]	rad		半径（高さ：Z）
			@param[in]	div		分割数
		 */
		//-----------------------------------------------------------------//
		void create_dome(const vtx::fvtx& rad, uint32_t div = 16) noexcept
		{
			if(div == 0 || rad.x <= 0.0f || rad.y <= 0.0f || rad.z <= 0.0f) return;

			if(div < 8) div = 8;

			create_dome_vertex_(rad, div);
			make_triangle_dome_();
			gen_glbuffer_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	頂点コンテナを取得
			@return 頂点コンテナ
		 */
		//-----------------------------------------------------------------//
		const vertices& get_vertices() noexcept { return vertices_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	描画
			@param[in]	type	描画タイプ
		 */
		//-----------------------------------------------------------------//
		void draw(draw_type type) const noexcept
		{
			switch(type) {
			case draw_type::line:
			{
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
				uint32_t vcn = divide_ / 4;
				for(uint32_t j = 0; j < vcn; ++j) {
					for(uint32_t i = 0; i < divide_; ++i) {
						{
							const vertex& a = vertices_[j * divide_ + i];
							const vertex& b = vertices_[j * divide_ + ((i + 1) % divide_)];
							draw_line(a.position, b.position);				
						}
						const vertex& a = vertices_[j * divide_ + i];
						uint32_t o = 0;
						if(j < (vcn - 1)) o = i;
						const vertex& b = vertices_[(j + 1) * divide_ + o];
						draw_line(a.position, b.position);
					}
				}
				glDisable(GL_LINE_SMOOTH);
			}
			break;

			case draw_type::fill:
				{
					glBindBuffer(GL_ARRAY_BUFFER, vertex_id_);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id_);
					glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);
					glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				}
				break;
			default:
				break;
			}
		}
	};
}
