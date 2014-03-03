#pragma once
//=====================================================================//
/*!	@file
	@brief	PMX ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include <bitset>
#include "gl_fw/IGLcore.hpp"
#include "gl_fw/gllight.hpp"
#include "utils/vtx.hpp"
#include "utils/mtx.hpp"
#include "utils/quat.hpp"
#include "utils/dim.hpp"
#include "utils/file_io.hpp"

namespace mdf {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PMX クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pmx_io {

		static bool get_(utils::file_io& fio, uint8_t index_sizeof, int32_t& data) {
			if(index_sizeof == 1) {
				int8_t v;
				if(!fio.get(v)) return false;
				data = v;
			} else if(index_sizeof == 2) {
				int16_t v;
				if(!fio.get(v)) return false;
				data = v;
			} else if(index_sizeof == 4) {
				int32_t v;
				if(!fio.get(v)) return false;
				data = v;
			} else {
				return false;
			}
			return true;
		}

		static bool get_(utils::file_io& fio, uint8_t index_sizeof, uint32_t& data) {
			if(index_sizeof == 1) {
				uint8_t v;
				if(!fio.get(v)) return false;
				data = v;
			} else if(index_sizeof == 2) {
				uint16_t v;
				if(!fio.get(v)) return false;
				data = v;
			} else if(index_sizeof == 4) {
				uint32_t v;
				if(!fio.get(v)) return false;
				data = v;
			} else {
				return false;
			}
			return true;
		}

		static bool get_text_(utils::file_io& fio, std::string& t, uint8_t utf8) {
			int32_t len;
			if(!fio.get(len)) return false;
			t.clear();
			if(len <= 0) return true;
			if(utf8) {
				if(!fio.get(t, len)) return false;
			} else {
				utils::wstring tmp;
				if(!fio.get(tmp, len / 2)) return false;
				utils::utf16_to_utf8(tmp, t);
			}
			return true;
		}

		static bool put_text_(utils::file_io& fio, const std::string& t, uint8_t utf8) {
			if(utf8) {
				if(!fio.put(static_cast<uint32_t>(t.size()))) return false;
				if(!fio.put(t)) return false;
			} else {
				utils::wstring ws;
				utils::utf8_to_utf16(t, ws);
				if(!fio.put(static_cast<uint32_t>(ws.size()))) return false;
				if(!fio.put(ws)) return false;
			}
			return true;
		}


		float	version_;

		struct reading_info {
			uint8_t		text_encode_type;
			uint8_t		appendix_uv;
			uint8_t		vertex_index_sizeof;
			uint8_t		texture_index_sizeof;
			uint8_t		material_index_sizeof;
			uint8_t		bone_index_sizeof;
			uint8_t		morph_index_sizeof;
			uint8_t		rigid_body_index_sizeof;

			bool get(utils::file_io& fio) {
				uint8_t len;
				if(!fio.get(len)) return false;
				if(len > 8) {
					return false;
				}
				if(!fio.get(text_encode_type)) return false;
				if(!fio.get(appendix_uv)) return false;
				if(!fio.get(vertex_index_sizeof)) return false;
				if(!fio.get(texture_index_sizeof)) return false;
				if(!fio.get(material_index_sizeof)) return false;
				if(!fio.get(bone_index_sizeof)) return false;
				if(!fio.get(morph_index_sizeof)) return false;
				if(!fio.get(rigid_body_index_sizeof)) return false;
				return true;
			}
		};
		reading_info	reading_info_;

		struct model_info {
			std::string		name;
			std::string		name_en;
			std::string		comment;
			std::string		comment_en;

			bool get(utils::file_io& fio, uint8_t utf8) {
				if(!get_text_(fio, name, utf8)) return false;
				if(!get_text_(fio, name_en, utf8)) return false;
				if(!get_text_(fio, comment, utf8)) return false;
				if(!get_text_(fio, comment_en, utf8)) return false;
				return true;
			}
			bool put(utils::file_io& fio, uint8_t utf8) {
				if(!put_text_(fio, name, utf8)) return false;
				if(!put_text_(fio, name_en, utf8)) return false;
				if(!put_text_(fio, comment, utf8)) return false;
				if(!put_text_(fio, comment_en, utf8)) return false;
				return true;
			}
		};
		model_info		model_info_;

		struct pmx_vertex {
 			vtx::fvtx	position_;
			vtx::fvtx	normal_;
			vtx::fpos	uv_;
			std::vector<vtx::fvtx4> appendix_uv_;

			struct weight {
				enum type {
					BDEF1,
					BDEF2,
					BDEF4,
					SDEF
				};
			};
			weight::type	weight_type_;

			struct i_weight {
				virtual ~i_weight() { }
				virtual bool get(utils::file_io& fio, uint8_t index_sizeof) = 0;
			};

			struct BDEF1 : public i_weight {
				int32_t		index;
				virtual ~BDEF1() { }
				bool get(utils::file_io& fio, uint8_t index_sizeof) {
					return get_(fio, index_sizeof, index);
				}
			};
			struct BDEF2 : public i_weight {
				int32_t		index[2];
				float		weight;
				virtual ~BDEF2() { }
				bool get(utils::file_io& fio, uint8_t index_sizeof) {
					if(!get_(fio, index_sizeof, index[0])) return false;
					if(!get_(fio, index_sizeof, index[1])) return false;
					if(!fio.get(weight)) return false;
					return true;
				}
			};
			struct BDEF4 : public i_weight {
				int32_t		index[4];
				float		weight[4];
				virtual ~BDEF4() { }
				bool get(utils::file_io& fio, uint8_t index_sizeof) {
					if(!get_(fio, index_sizeof, index[0])) return false;
					if(!get_(fio, index_sizeof, index[1])) return false;
					if(!get_(fio, index_sizeof, index[2])) return false;
					if(!get_(fio, index_sizeof, index[3])) return false;
					if(!fio.get(weight[0])) return false;
					if(!fio.get(weight[1])) return false;
					if(!fio.get(weight[2])) return false;
					if(!fio.get(weight[3])) return false;
					return true;
				};
			};
			struct SDEF : public i_weight {
				int32_t		index[2];
				float		weight;
				vtx::fvtx	C;
				vtx::fvtx	R0;
				vtx::fvtx	R1;
				virtual ~SDEF() { }
				bool get(utils::file_io& fio, uint8_t index_sizeof) {
					if(!get_(fio, index_sizeof, index[0])) return false;
					if(!get_(fio, index_sizeof, index[1])) return false;
					if(!fio.get(weight)) return false;
					if(!fio.get(C)) return false;
					if(!fio.get(R0)) return false;
					if(!fio.get(R1)) return false;
					return true;
				};
			};
			i_weight*	weight_;

			float		edge_scale_;

			bool get(utils::file_io& fio, const reading_info& info) {
				if(!fio.get(position_)) return false;
				if(!fio.get(normal_)) return false;
				if(!fio.get(uv_)) return false;
				if(info.appendix_uv) {
					appendix_uv_.reserve(info.appendix_uv);
					appendix_uv_.clear();
					for(uint8_t i = 0; i < info.appendix_uv; ++i) {
						vtx::fvtx4 v; 
						if(!fio.get(v)) return false;
						appendix_uv_.push_back(v);
					}
				}
				uint8_t wt;
				if(!fio.get(wt)) return false;
				weight_type_ = static_cast<weight::type>(wt);
				if(weight_type_ == weight::BDEF1) {
					weight_ = new BDEF1;
				} else if(weight_type_ == weight::BDEF2) {
					weight_ = new BDEF2;
				} else if(weight_type_ == weight::BDEF4) {
					weight_ = new BDEF4;
				} else if(weight_type_ == weight::SDEF) {
					weight_ = new SDEF;
				} else {
					return false;
				}
				if(!weight_->get(fio, info.bone_index_sizeof)) return false;
				if(!fio.get(edge_scale_)) return false;
   				return true;
			}
			pmx_vertex() : weight_(0) { }
			~pmx_vertex() { delete weight_; }
		};
		typedef std::vector<pmx_vertex>	pmx_vertexes;
		pmx_vertexes	vertexes_;

		utils::dim		faces_;

		utils::strings	textures_;

		struct pmx_material {
			std::string		name_;
			std::string		name_en_;

			img::rgbaf		diffuse_;
			img::rgbaf		specular_;		///< RGB
			float			speculark_;
			img::rgbaf		ambient_;		///< RGB

			struct draw_flags {
				enum type {
					TWO_SIDE,			///< 両面描画
					GROUND_SHADOW,		///< 地面影
					SELF_SHADOW_MAP,	///< セルフ・シャドウ・マップへの描画
					SELF_SHADOW,		///< セルフ・シャドウへの描画
					EDGE,				///< エッジ描画
				};
			};
			uint8_t			draw_flags_;

			img::rgbaf		edge_color_;
			float			edge_size_;

			int32_t			normal_texture_index_;
			int32_t			sphere_texture_index_;
			uint8_t			sphere_mode_;

			uint8_t			share_toon_flags_;
			int32_t			toon_texture_index_;
			uint8_t			toon_texture_no_;

			std::string		memo_;

			int32_t			face_num_;

			bool get(utils::file_io& fio, const reading_info& info) {
				if(!get_text_(fio, name_, info.text_encode_type)) return false;
				if(!get_text_(fio, name_en_, info.text_encode_type)) return false;
				if(!fio.get(diffuse_)) return false;
				float rgb[3];
				if(!fio.get(rgb)) return false;
				specular_.set(rgb[0], rgb[1], rgb[2]);
				if(!fio.get(speculark_)) return false;
				if(!fio.get(rgb)) return false;
				ambient_.set(rgb[0], rgb[1], rgb[2]);
				if(!fio.get(draw_flags_)) return false;
				if(!fio.get(edge_color_)) return false;
				if(!fio.get(edge_size_)) return false;
				if(!get_(fio, info.texture_index_sizeof, normal_texture_index_)) return false;
				if(!get_(fio, info.texture_index_sizeof, sphere_texture_index_)) return false;
				if(!fio.get(sphere_mode_)) return false;
				if(!fio.get(share_toon_flags_)) return false;
				if(share_toon_flags_ == 0) {
					if(!get_(fio, info.texture_index_sizeof, toon_texture_index_)) return false;
				} else {
					if(!fio.get(toon_texture_no_)) return false;
				}
				if(!get_text_(fio, memo_, info.text_encode_type)) return false;
				if(!fio.get(face_num_)) return false;
				return true;
			}
		};
		typedef std::vector<pmx_material>	pmx_materials;
		pmx_materials	materials_;

		struct pmx_bone {
			std::string		name_;
			std::string		name_en_;

			vtx::fvtx		position_;

			int32_t			parent_index_;

			int32_t	   		level_;

			struct flags {
				enum type {
					CONECTION,	///< 0x0001: 接続先(PMD子ボーン指定)表示方法 0:座標オフセットで指定 1:ボーンで指定
					ROTATE,		///< 0x0002: 回転可能
					MOVE,		///< 0x0004: 移動可能
 					DISP,		///< 0x0008: 表示
 					OPERATION,	///< 0x0010: 操作可
 					IK,			///< 0x0020: IK
					none_,		///< 0x0040: 空き
					LOCAL,		///< 0x0080: ローカル付与 | 付与対象 0:ユーザー変形値／IKリンク／多重付与 1:親のローカル変形量
					ROTATE_,	///< 0x0100: 回転付与
					MOVE_,		///< 0x0200: 移動付与
					AXIS_LOCK,	///< 0x0400: 軸固定
					AXIS_LOCAL,	///< 0x0800: ローカル軸
					MODIFY_PHY,	///< 0x1000: 物理後変形
					MODIFY_EXT,	///< 0x2000: 外部親変形
				};
				uint16_t	bits_;
				bool test(type t) const { return bits_ & (1 << t); }
				bool get(utils::file_io& fio) {
					uint8_t	v[2];
					if(!fio.get(v)) return false;
					bits_ = (v[1] << 8) | v[0];
					return true;
				}
				flags() : bits_(0) { }
			};
			flags		flags_;

			vtx::fvtx	bone_offset_;
			int32_t		bone_index_;

			int32_t		parent_bone_index_;
			float		parent_bone_gain_;

			vtx::fvtx	axis_direction_;

			vtx::fvtx	axis_x_directioon_;
			vtx::fvtx	axis_z_directioon_;

			int32_t		modify_ext_key_;

			struct ik_data {
				int32_t		ik_target_;
				int32_t		ik_loop_count_;
				float		ik_loop_radian_;

				int			ik_link_count_;

				struct ik_link {
					int32_t		bone_index_;
					uint8_t		angle_limit_;
					vtx::fvtx	lower_;
					vtx::fvtx	upper_;
					ik_link() : bone_index_(0), angle_limit_(0), lower_(0.0f), upper_(0.0f) { }
					bool get(utils::file_io& fio, uint8_t is) {
						if(!get_(fio, is, bone_index_)) return false;
						if(!fio.get(angle_limit_)) return false;
						if(angle_limit_) {
							if(!fio.get(lower_)) return false;
							if(!fio.get(upper_)) return false;
						}
						return true;
					}
				};
				std::vector<ik_link>	ik_links_;

				ik_data() : ik_target_(-1), ik_loop_count_(0), ik_loop_radian_(0.0f),
					ik_link_count_(0), ik_links_() { }

				bool get(utils::file_io& fio, uint8_t is) {
					if(!get_(fio, is, ik_target_)) return false;
					if(!fio.get(ik_loop_count_)) return false;
					if(!fio.get(ik_loop_radian_)) return false;
					if(!fio.get(ik_link_count_)) return false;
					ik_links_.resize(ik_link_count_);
					for(int i = 0; i < ik_link_count_; ++i) {
						if(!ik_links_[i].get(fio, is)) return false;
					}
					return true;
				}
			};
			ik_data		ik_data_;

			bool get(utils::file_io& fio, const reading_info& info) {
				if(!get_text_(fio, name_, info.text_encode_type)) return false;
				if(!get_text_(fio, name_en_, info.text_encode_type)) return false;
				if(!fio.get(position_)) return false;
				if(!get_(fio, info.bone_index_sizeof, parent_index_)) return false;
				if(!fio.get(level_)) return false;
				if(!flags_.get(fio)) return false;
				if(flags_.test(flags::CONECTION)) {
					if(!get_(fio, info.bone_index_sizeof, bone_index_)) return false;
					bone_offset_.set(0.0f);
				} else {
					if(!fio.get(bone_offset_)) return false;
					bone_index_ = -1;
				}
				if(flags_.test(flags::ROTATE_) || flags_.test(flags::MOVE_)) {
					if(!get_(fio, info.bone_index_sizeof, parent_bone_index_)) return false;
					if(!fio.get(parent_bone_gain_)) return false;
				} else {
					parent_bone_index_ = -1;
					parent_bone_gain_ = 0.0f;
				}
				if(flags_.test(flags::AXIS_LOCK)) {
					if(!fio.get(axis_direction_)) return false;
				} else {
					axis_direction_.set(0.0f);
				}
				if(flags_.test(flags::AXIS_LOCAL)) {
					if(!fio.get(axis_x_directioon_)) return false;
					if(!fio.get(axis_z_directioon_)) return false;
				} else {
					axis_x_directioon_.set(0.0f);
					axis_z_directioon_.set(0.0f);
				}
				if(flags_.test(flags::MODIFY_EXT)) {
					if(!fio.get(modify_ext_key_)) return false;
				} else {
					modify_ext_key_ = 0;
				}
				if(flags_.test(flags::IK)) {
					if(!ik_data_.get(fio, info.bone_index_sizeof)) return false;
				}
				return true;
			}

			pmx_bone() : bone_offset_(0.0f), bone_index_(-1), parent_bone_index_(-1), parent_bone_gain_(0.0f),
				axis_direction_(0.0f), axis_x_directioon_(0.0f), axis_z_directioon_(0.0f),
				modify_ext_key_(0), ik_data_()
			{ }
		};
		typedef std::vector<pmx_bone>	pmx_bones;
		pmx_bones		bones_;

		struct pmx_morph {


			bool get(utils::file_io& fio, const reading_info& info) {
				return true;
			}
		};
		typedef std::vector<pmx_morph>	pmx_morphs;
		pmx_morphs		morphs_;

		std::string		current_path_;

		struct vbo_t {
			vtx::fpos	uv;
			vtx::fvtx	n;
			vtx::fvtx	v;
		};

		GLuint	vtx_id_;
		std::vector<GLuint>	idx_id_;

		std::vector<GLuint>	tex_id_;

		void initialize_();
		void destroy_();

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pmx_io() : version_(0.0f) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~pmx_io() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	バージョンを取得
			@return バージョン
		*/
		//-----------------------------------------------------------------//
		float get_version() const { return version_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルが有効か検査
			@return 有効なら「true」
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fio);


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
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render_surface();


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーンの参照
			@return ボーン
		*/
		//-----------------------------------------------------------------//
		const pmx_bones& get_bones() const { return bones_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーンのフルパスを生成
			@param[in]	index	ボーンのインデックス
			@param[out]	path	フル・パスを受け取る参照	
		*/
		//-----------------------------------------------------------------//
		bool create_bone_path(uint32_t index, std::string& path);

	};
}
