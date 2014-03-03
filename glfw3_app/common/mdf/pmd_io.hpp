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
#include "gl_fw/gllight.hpp"
#include "utils/vtx.hpp"
#include "utils/mtx.hpp"
#include "utils/quat.hpp"
#include "utils/file_io.hpp"

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

	public:
		typedef std::vector<uint16_t>	pmd_indexes;

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
		typedef std::vector<pmd_vertex>	pmd_vertexes;

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
		};
		typedef std::vector<pmd_material>	pmd_materials;


		struct pmd_bone {
			enum bone_type {
				ROTATE,			///< (0) 回転
				ROTATE_MOVE,	///< (1) 回転と移動
				IK,				///< (2) IK
				none_,			///< (3) 不明
				IK_,			///< (4) IK 影響下
				ROTATE_,		///< (5) 回転影響下
				IK_LINK,		///< (6) IK 接続先
				NO_DISP			///< (7) 非表示
			};
			char		name[20];			///< ボーン名
			uint16_t	parent_index;		///< 親ボーン番号（無い場合は 0xffff）
			uint16_t	tail_pos_index;		///< tail 位置のボーン番号（末端の場合０）
			uint8_t		type;				///< ボーン種
			uint16_t	ik_parent_index;	///< IK ボーン番号（無い場合０）
			vtx::fvtx	head_pos;			///< 基準位置

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

			bool put(utils::file_io& fio) {
				if(fio.write(name, 20) != 20) return false;
				if(!fio.put(parent_index)) return false;
				if(!fio.put(tail_pos_index)) return false;
				if(!fio.put(type)) return false;
				if(!fio.put(ik_parent_index)) return false;
				if(!fio.put(head_pos.x)) return false;
				if(!fio.put(head_pos.y)) return false;
				if(!fio.put(head_pos.z)) return false;
				return true;
			}
		};
		typedef std::vector<pmd_bone>	pmd_bones;


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
		typedef std::vector<pmd_ik>	pmd_iks;

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
		typedef std::vector<pmd_skin>	pmd_skins;

		struct pmd_bone_disp_name {
			char	name[50];

			bool get(utils::file_io& fio) {
				if(fio.read(name, 50) != 50) return false;
				return true;
			}			
		};
		typedef std::vector<pmd_bone_disp_name>	pmd_bone_disp_names;

		struct pmd_bone_disp {
			uint16_t	index;
			uint8_t		frame_index;

			bool get(utils::file_io& fio) {
				if(!fio.get(index)) return false;
				if(!fio.get(frame_index)) return false;
				return true;
			}
		};
		typedef std::vector<pmd_bone_disp>	pmd_bone_disps;

		static void get_text_(const char* src, uint32_t n, std::string& dst);

	private:
		pmd_vertexes		vertexes_;
		vtx::fvtx			vertex_min_;
		vtx::fvtx			vertex_max_;

		pmd_indexes			face_indexes_;

		pmd_materials		materials_;
		pmd_bones			bones_;
		pmd_iks				iks_;
		pmd_skins			skins_;
		pmd_indexes			skin_indexes_;

		pmd_bone_disp_names	bone_disp_names_;
		pmd_bone_disps		bone_disps_;

		bool parse_vertex_(utils::file_io& fio);
		bool parse_face_vertex_(utils::file_io& fio);
		bool parse_material_(utils::file_io& fio);
		bool parse_bone_(utils::file_io& fio);
		bool parse_ik_(utils::file_io& fio);
		bool parse_skin_(utils::file_io& fio);
		bool parse_skin_index_(utils::file_io& fio);
		bool parse_bone_disp_name_(utils::file_io& fio);
		bool parse_bone_disp_(utils::file_io& fio);
		void initialize_();
		void destroy_();

		std::string	current_path_;

		struct vbo_t {
			vtx::fpos	uv;
			vtx::fvtx	n;
			vtx::fvtx	v;
		};

		GLuint	vtx_id_;
		std::vector<GLuint>	idx_id_;

		std::vector<GLuint>	tex_id_;

		float	bone_joint_size_;
		GLuint	bone_list_id_;
		GLuint	joint_list_id_;

		bool	init_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pmd_io() : version_(1.0f),
			vertex_min_(0.0f), vertex_max_(0.0f), vtx_id_(0),
			bone_joint_size_(0.0f), bone_list_id_(0), joint_list_id_(0),
			init_(false)
		{ initialize_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~pmd_io() {
			destroy_();
			glDeleteLists(joint_list_id_, 1);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルが有効か検査
			@return 有効なら「true」
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fio);


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
			@brief	ロード
			@param[in]	fio	ファイル入出力クラス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fio);


		//-----------------------------------------------------------------//
		/*!
			@brief	ロード
			@param[in]	fn	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& fn) {
			utils::file_io fio;
			if(!fio.open(fn, "rb")) {
				return false;
			}
			bool f = load(fio);
			fio.close();
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	セーブ
			@param[in]	fio	ファイル入出力クラス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io fio);


		//-----------------------------------------------------------------//
		/*!
			@brief	セーブ
			@param[in]	fn	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& fn) {
			utils::file_io fio;
			if(!fio.open(fn, "wb")) {
				return false;
			}
			bool f = save(fio);
			fio.close();
			return f;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング・セットアップ
		*/
		//-----------------------------------------------------------------//
		void render_setup();


		//-----------------------------------------------------------------//
		/*!
			@brief	サーフェースのレンダリング
		*/
		//-----------------------------------------------------------------//
		void render_surface();


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーンのレンダリング
			@param[in]	lig	ライトのコンテキスト
		*/
		//-----------------------------------------------------------------//
		void render_bone(gl::light& lig);


		//-----------------------------------------------------------------//
		/*!
			@brief	マテリアルの参照
			@return マテリアル
		*/
		//-----------------------------------------------------------------//
		const pmd_materials& get_materials() const { return materials_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スキンの参照
			@return スキン
		*/
		//-----------------------------------------------------------------//
		const pmd_skins& get_skins() const { return skins_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーンの参照
			@return ボーン
		*/
		//-----------------------------------------------------------------//
		const pmd_bones& get_bones() const { return bones_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーン表示名の参照
			@return ボーン表示名
		*/
		//-----------------------------------------------------------------//
		const pmd_bone_disp_names& get_bone_disp_names() const { return bone_disp_names_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーンのフルパスを生成
			@param[in]	index	インデックス
			@param[out]	path	フル・パスを受け取る参照	
		*/
		//-----------------------------------------------------------------//
		bool create_bone_path(uint32_t index, std::string& path);

	};
}
