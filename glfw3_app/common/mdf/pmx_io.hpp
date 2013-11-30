#pragma once
//=====================================================================//
/*!	@file
	@brief	PMX ファイルを扱うクラス（ヘッダー）
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
#include "utils/dim.hpp"

namespace mdf {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PMX クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pmx_io {

		float	version_;

		struct reading_info {
			uint8_t		text_encode_type;
			uint8_t		appendix_uv;
			uint8_t		vertex_index_size;
			uint8_t		texture_index_size;
			uint8_t		material_index_size;
			uint8_t		bone_index_size;
			uint8_t		morph_index_size;
			uint8_t		rigid_body_index_size;

			bool get(utils::file_io& fio) {
				uint8_t len;
				if(!fio.get(len)) return false;
				if(!fio.get(text_encode_type)) return false;
				if(!fio.get(appendix_uv)) return false;
				if(!fio.get(vertex_index_size)) return false;
				if(!fio.get(texture_index_size)) return false;
				if(!fio.get(material_index_size)) return false;
				if(!fio.get(bone_index_size)) return false;
				if(!fio.get(morph_index_size)) return false;
				if(!fio.get(rigid_body_index_size)) return false;
				if(len > 8) {
					return false;
				}
				return true;
			}
		};
		reading_info	reading_info_;

		static bool get_text_(utils::file_io& fio, std::string& t, bool utf16) {
			uint32_t len;
			if(!fio.get(len)) return false;
			t.clear();
			if(utf16) {
				utils::wstring tmp;
				if(!fio.get(tmp, len / 2)) {
					return false;
				}
				utils::utf16_to_utf8(tmp, t);
			} else {
				if(!fio.get(t, len)) {
					return false;
				}
			}
			return true;
		}

		struct model_info {
			std::string		name;
			std::string		name_en;
			std::string		comment;
			std::string		comment_en;

			bool get(utils::file_io& fio, bool utf16 = false) {
				if(!get_text_(fio, name, utf16)) return false;
				if(!get_text_(fio, name_en, utf16)) return false;
				if(!get_text_(fio, comment, utf16)) return false;
				if(!get_text_(fio, comment_en, utf16)) return false;
				return true;
			}
		};
		model_info		model_info_;

		struct BDEF2 {
			uint32_t	index[2];
			float		weight[2];
		};
		struct BDEF4 {
			uint32_t	index[4];
			float		weight[4];
		};
		struct SDEF {
			uint32_t	index[2];
			float		weight;
			vtx::fvtx	c;
			vtx::fvtx	r0;
			vtx::fvtx	r1;
		};
		struct share_t {
			std::vector<vtx::fvtx4> uv;
			std::vector<BDEF2> bdef2;
			std::vector<BDEF4> bdef4;
			std::vector<SDEF>  sdef;
		};
		static share_t share_;

		struct pmx_vertex {
			vtx::fvtx	position_;
			vtx::fvtx	normal_;
			vtx::fpos	uv_;
			uint32_t	append_uv_;

			struct weight {
				enum type {
					BDEF1,
					BDEF2,
					BDEF4,
					SDEF
				};
			};
			weight::type	weight_;
			uint32_t		weight_index_;

			bool get(utils::file_io& fio, const reading_info& info) {
				if(!fio.get(position_)) return false;
				if(!fio.get(normal_)) return false;
				if(!fio.get(uv_)) return false;
				if(info.appendix_uv) {
					append_uv_ = share_.uv.size();
					for(uint8_t i = 0; i < info.appendix_uv; ++i) {
						vtx::fvtx4 v; 
						if(!fio.get(v)) return false;
						share_.uv.push_back(v);
					}
				}
				{
					uint8_t type;
					if(!fio.get(type)) return false;
					weight_ = static_cast<weight::type>(type);
				}
				switch(weight_) {
				case weight::BDEF1:
//					bone_index_size;
					break;
				case weight::BDEF2:

					break;
				case weight::BDEF4:

					break;
				case weight::SDEF:

					break;
				default:
					return false;
				}
				return true;
			}
		};
		std::vector<pmx_vertex>	pmx_vertexes_;


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
		~pmx_io() { }


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

	};
}
