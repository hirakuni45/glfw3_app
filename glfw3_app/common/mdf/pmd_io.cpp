//=====================================================================//
/*!	@file
	@brief	PMD ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "mdf/pmd_io.hpp"
#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"
#include <boost/format.hpp>

#include <cstdio>

namespace mdf {

	bool pmd_io::parse_vertex_(utils::file_io& fio)
	{
		int32_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Vertex :" << n << std::endl;
		vertex_.reserve(n);
		vertex_.clear();
		vtx::fvtx min;
		vtx::fvtx max;
		for(uint32_t i = 0; i < n; ++i) {
			pmd_vertex v;
			if(!v.get(fio)) {
				return false;
			}
			if(i == 0) min = max = v.pos;
			vtx::set_min(v.pos, min);
			vtx::set_max(v.pos, max);
			vertex_.push_back(v);
		}
		model_min_ = min;
		model_max_ = max;
///		std::cout << (boost::format("Min: %1.3f, %1.3f, %1.3f\n") % min.x % min.y % min.z);
///		std::cout << (boost::format("Max: %1.3f, %1.3f, %1.3f\n") % max.x % max.y % max.z);
		return true;
	}

	bool pmd_io::parse_face_vertex_(utils::file_io& fio)
	{
		int32_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Face Vertex: " << n << std::endl;
		face_index_.reserve(n);
		face_index_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			uint16_t v;
			if(!fio.get(v)) {
				return false;
			}
			face_index_.push_back(v);
		}
		return true;
	}


	bool pmd_io::parse_material_(utils::file_io& fio)
	{
		int32_t n;
		if(!fio.get(n)) {
			return false;
		}
		std::cout << "Material: " << n << std::endl;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・オープン
		@param[in]	fn	ファイル名
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmd_io::open(const std::string& fn)
	{
		utils::file_io fio;
		if(!fio.open(fn, "rb")) {
			return false;
		}

		{
			std::string s;
			if(fio.get(s, 3) != 3) {
				return false;
			}
			if(s != "Pmd") {
				return false;
			}
		}
		if(!fio.get(version_)) {
			return false;
		}
///		std::cout << "Version: " << (boost::format("%1.2f") % version_) << std::endl;
		{
			std::string s;
			if(fio.get(s, 20) != 20) {
				return false;
			}
			utils::sjis_to_utf8(s, model_name_);
///			std::cout << "Model name: '" << model_name_ << "'" << std::endl; 
		}
		{
			std::string s;
			if(fio.get(s, 256) != 256) {
				return false;
			}
			utils::sjis_to_utf8(s, comment_);
///			std::cout << "Comment: '" << comment_ << "'" << std::endl; 
		}

		// 頂点リスト取得
		if(!parse_vertex_(fio)) {
			return false;
		}

		// 面頂点リスト取得
		if(!parse_face_vertex_(fio)) {
			return false;
		}

		// マテリアル取得
		if(!parse_material_(fio)) {
			return false;
		}


		fio.close();

///		std::cout << fn << std::endl;

		return true;
	}

}
