#pragma once
//=====================================================================//
/*!	@file
	@brief	PMD ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include "img_io/bmp_io.hpp"
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
		};
		std::vector<pmd_vertex>	vertex_;
		vtx::fvtx		model_min_;
		vtx::fvtx		model_max_;

		std::vector<uint16_t>	face_index_;




		bool parse_vertex_(utils::file_io& fio);
		bool parse_face_vertex_(utils::file_io& fio);
		bool parse_material_(utils::file_io& fio);
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		pmd_io() : version_(0.0f),
			model_min_(0.0f), model_max_(0.0f) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~pmd_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・オープン
			@param[in]	fn	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& fn);

	};
}
