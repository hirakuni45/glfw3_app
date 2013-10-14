#pragma once
//=====================================================================//
/*!	@file
	@brief	PMD ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include "gl_fw/IGLcore.hpp"
#include "utils/vtx.hpp"
#include "utils/mtx.hpp"
#include "utils/quat.hpp"

namespace mdf {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PMD クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pmd_io {

		float		version_;
		std::string	model_name_;
		std::string	comment_;

		struct pmd_vertex {
			vtx::fvtx	pos;
			vtx::fvtx	normal;
			vtx::fpos	uv;
			uint16_t	bone_num[2];
			uint8_t		bone_weight;
			uint8_t		edge_flag;

			bool get(utils::file_io& fio) {
				if(!fio.get(pos.x)) return false;
				if(!fio.get(pos.y)) return false;
				if(!fio.get(pos.z)) return false;
				if(!fio.get(normal.x)) return false;
				if(!fio.get(normal.y)) return false;
				if(!fio.get(normal.z)) return false;
				if(!fio.get(uv.x)) return false;
				if(!fio.get(uv.y)) return false;
				if(!fio.get(bone_num[0])) return false;
				if(!fio.get(bone_num[1])) return false;
				if(!fio.get(bone_weight)) return false;
				if(!fio.get(edge_flag)) return false;
				return true;
			}

			bool put(utils::file_io& fio) {
				if(!fio.put(pos.x)) return false;
				if(!fio.put(pos.y)) return false;
				if(!fio.put(pos.z)) return false;
				if(!fio.put(normal.x)) return false;
				if(!fio.put(normal.y)) return false;
				if(!fio.put(normal.z)) return false;
				if(!fio.put(uv.x)) return false;
				if(!fio.put(uv.y)) return false;
				if(!fio.put(bone_num[0])) return false;
				if(!fio.put(bone_num[1])) return false;
				if(!fio.put(bone_weight)) return false;
				if(!fio.put(edge_flag)) return false;
				return true;
			}
		};
		std::vector<pmd_vertex>	vertex_;
		vtx::fvtx		vertex_min_;
		vtx::fvtx		vertex_max_;

		std::vector<uint16_t>	face_index_;

		struct pmd_material {
			float	diffuse_color[3];
			float	alpha;
			float	specularity;
			float	specular_color[3];
			float	mirror_color[3];
			uint8_t	toon_index;
			uint8_t	edge_flag;
			uint32_t	face_vert_count;
			char	texture_file_name[20];

			bool get(utils::file_io& fio) {
				if(!fio.get(diffuse_color[0])) return false;
				if(!fio.get(diffuse_color[1])) return false;
				if(!fio.get(diffuse_color[2])) return false;
				if(!fio.get(alpha)) return false;
				if(!fio.get(specularity)) return false;
				if(!fio.get(specular_color[0])) return false;
				if(!fio.get(specular_color[1])) return false;
				if(!fio.get(specular_color[2])) return false;
				if(!fio.get(mirror_color[0])) return false;
				if(!fio.get(mirror_color[1])) return false;
				if(!fio.get(mirror_color[2])) return false;
				if(!fio.get(toon_index)) return false;
				if(!fio.get(edge_flag)) return false;
				if(!fio.get(face_vert_count)) return false;
				if(fio.read(texture_file_name, 20) != 20) return false;
				return true;
			}

			bool put(utils::file_io& fio) {
				if(!fio.put(diffuse_color[0])) return false;
				if(!fio.put(diffuse_color[1])) return false;
				if(!fio.put(diffuse_color[2])) return false;
				if(!fio.put(alpha)) return false;
				if(!fio.put(specularity)) return false;
				if(!fio.put(specular_color[0])) return false;
				if(!fio.put(specular_color[1])) return false;
				if(!fio.put(specular_color[2])) return false;
				if(!fio.put(mirror_color[0])) return false;
				if(!fio.put(mirror_color[1])) return false;
				if(!fio.put(mirror_color[2])) return false;
				if(!fio.put(toon_index)) return false;
				if(!fio.put(edge_flag)) return false;
				if(!fio.put(face_vert_count)) return false;
				if(fio.write(texture_file_name, 20) != 20) return false;
				return true;
			}

			GLuint	tex_id_;

			pmd_material() : tex_id_(0) { }
		};
		std::vector<pmd_material>	material_;

		struct pmd_bone {
			char		name[20];
			uint16_t	parent_index;
			uint16_t	tail_pos_index;
			uint8_t		type;
			uint16_t	ik_parent_index;
			vtx::fvtx	head_pos;

			bool get(utils::file_io& fio) {
				if(fio.read(name, 20) != 20) return false;
				if(!fio.get(parent_index)) return false;
				if(!fio.get(tail_pos_index)) return false;
				if(!fio.get(type)) return false;
				if(!fio.get(ik_parent_index)) return false;
				if(!fio.get(head_pos.x)) return false;
				if(!fio.get(head_pos.y)) return false;
				if(!fio.get(head_pos.z)) return false;
				return true;
			}
		};
		std::vector<pmd_bone>	bone_;

		struct pmd_ik {
			uint16_t	index;
			uint16_t	target_index;
			uint8_t		chain_length;
			uint16_t	iterations;
			float		control_weight;
			std::vector<uint16_t>	child_index;

			bool get(utils::file_io& fio) {
				if(!fio.get(index)) return false;
				if(!fio.get(target_index)) return false;
				if(!fio.get(chain_length)) return false;
				if(!fio.get(iterations)) return false;
				if(!fio.get(control_weight)) return false;
				child_index.reserve(chain_length);
				child_index.clear();
				for(uint16_t i = 0; i < static_cast<uint16_t>(chain_length); ++i) {
					uint16_t v;
					if(!fio.get(v)) return false;
					child_index.push_back(v);
				}
				return true;
			}			
		};
		std::vector<pmd_ik>		ik_;

		struct pmd_skin_vert {
			uint32_t	index;
			vtx::fvtx	pos;

			bool get(utils::file_io& fio) {
				if(!fio.get(index)) return false;
				if(!fio.get(pos.x)) return false;
				if(!fio.get(pos.y)) return false;
				if(!fio.get(pos.z)) return false;
				return true;
			}
		};

		struct pmd_skin {
			char		name[20];
			uint32_t	vert_count;
			uint8_t		type;
			std::vector<pmd_skin_vert>	vert;

			bool get(utils::file_io& fio) {
				if(fio.read(name, 20) != 20) return false;
				if(!fio.get(vert_count)) return false;
				if(!fio.get(type)) return false;
				vert.reserve(vert_count);
				vert.clear();
				for(uint32_t i = 0; i < vert_count; ++i) {
					pmd_skin_vert sv;
					if(!sv.get(fio)) {
						return false;
					}
					vert.push_back(sv);
				}
				return true;
			}
		};
		std::vector<pmd_skin>	skin_;

		std::vector<uint16_t>	skin_index_;

		struct pmd_bone_disp_list {
			char	name[50];

			bool get(utils::file_io& fio) {
				if(fio.read(name, 50) != 50) return false;
				return true;
			}			
		};
		std::vector<pmd_bone_disp_list>	bone_disp_list_;

		struct pmd_bone_disp {
			uint16_t	index;
			uint8_t		frame_index;

			bool get(utils::file_io& fio) {
				if(!fio.get(index)) return false;
				if(!fio.get(frame_index)) return false;
				return true;
			}
		};
		std::vector<pmd_bone_disp>	bone_disp_;

		bool parse_vertex_(utils::file_io& fio);
		bool parse_face_vertex_(utils::file_io& fio);
		bool parse_material_(utils::file_io& fio);
		bool parse_bone_(utils::file_io& fio);
		bool parse_ik_(utils::file_io& fio);
		bool parse_skin_(utils::file_io& fio);
		bool parse_skin_index_(utils::file_io& fio);
		bool parse_bone_disp_list_(utils::file_io& fio);
		bool parse_bone_disp_(utils::file_io& fio);
		void destroy_();

		std::string	current_path_;

		struct vbo_t {
			vtx::fpos	uv;
			vtx::fvtx	n;
			vtx::fvtx	v;
		};
		std::vector<vbo_t>	vbos_;

		GLuint	vtx_id_;
		std::vector<GLuint>	idx_id_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pmd_io() : version_(0.0f),
			vertex_min_(0.0f), vertex_max_(0.0f), vtx_id_(0), idx_id_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~pmd_io() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	バージョンを取得
			@return バージョン
		*/
		//-----------------------------------------------------------------//
		float get_version() const { return version_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	モデル名を取得
			@return モデル名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_model_name() const { return model_name_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	コメントを取得
			@return コメント
		*/
		//-----------------------------------------------------------------//
		const std::string& get_comment() const { return comment_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・オープン
			@param[in]	fn	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& fn);


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング・セットアップ
		*/
		//-----------------------------------------------------------------//
		void render_setup();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();

	};
}
