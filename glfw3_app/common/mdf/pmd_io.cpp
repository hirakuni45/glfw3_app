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

	static void get_text_(const char* src, uint32_t n, std::string& dst)
	{
		std::string tmp;
		for(uint32_t i = 0; i < n; ++i) {
			char ch = *src++;
			if(ch == 0) break;
			if(ch != 0xfd) {
				tmp += ch;
			}
		}
		utils::sjis_to_utf8(tmp, dst);
	}

	bool pmd_io::parse_vertex_(utils::file_io& fio)
	{
		uint32_t n;
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
		uint32_t n;
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
		uint32_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Material num: " << n << std::endl;

		material_.reserve(n);
		material_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_material m;
			if(!m.get(fio)) {
				return false;
			}
			material_.push_back(m);

///			std::string s;
///			get_text_(m.texture_file_name, 20, s);
///			std::cout << "Material: " << s << std::endl;
		}
		return true;
	}


	bool pmd_io::parse_bone_(utils::file_io& fio)
	{
		uint16_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Bone num: " << n << std::endl;

		bone_.reserve(n);
		bone_.clear();
		for(uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
			pmd_bone b;
			if(!b.get(fio)) {
				return false;
			}
			bone_.push_back(b);

///			std::string s;
///			get_text_(b.name, 20, s);
///			std::cout << "Bone: " << s << std::endl;
		}


		return true;
	}


	bool pmd_io::parse_ik_(utils::file_io& fio)
	{
		uint16_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "IK num: " << n << std::endl;

		ik_.reserve(n);
		ik_.clear();
		for(uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
			pmd_ik ik;
			if(!ik.get(fio)) {
				return false;
			}
			ik_.push_back(ik);
		}
		return true;
	}


	bool pmd_io::parse_skin_(utils::file_io& fio)
	{
		uint16_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Skin num: " << n << std::endl;

		skin_.reserve(n);
		skin_.clear();
		for(uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
			pmd_skin ps;
			if(!ps.get(fio)) {
				return false;
			}
			skin_.push_back(ps);

///			std::string s;
///			get_text_(ps.name, 20, s);
///			std::cout << "Skin: " << s << std::endl;
		}

		return true;
	}


	bool pmd_io::parse_skin_index_(utils::file_io& fio)
	{
		uint8_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Skin index num: " << static_cast<int>(n) << std::endl;

		skin_index_.reserve(n);
		skin_index_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			uint16_t v;
			if(!fio.get(v)) {
				return false;
			}
			skin_index_.push_back(v);
		}

		return true;
	}


	bool pmd_io::parse_bone_disp_list_(utils::file_io& fio)
	{
		uint8_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Bone disp list num: " << static_cast<int>(n) << std::endl;

		bone_disp_list_.reserve(n);
		bone_disp_list_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_bone_disp_list bdl;
			if(!bdl.get(fio)) {
				return false;
			}
			bone_disp_list_.push_back(bdl);

///			std::string s;
///			get_text_(bdl.name, 50, s);
///			std::cout << "Bone disp list: " << s << std::endl;
		}
		return true;
	}


	bool pmd_io::parse_bone_disp_(utils::file_io& fio)
	{
		uint32_t n;
		if(!fio.get(n)) {
			return false;
		}
///		std::cout << "Bone disp num: " << n << std::endl;

		bone_disp_.reserve(n);
		bone_disp_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_bone_disp bd;
			if(!bd.get(fio)) {
				return false;
			}
			bone_disp_.push_back(bd);
		}
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

		// ボーン取得
		if(!parse_bone_(fio)) {
			return false;
		}

		// IK 取得
		if(!parse_ik_(fio)) {
			return false;
		}

		// スキン 取得
		if(!parse_skin_(fio)) {
			return false;
		}

		// スキン・インデックス 取得
		if(!parse_skin_index_(fio)) {
			return false;
		}

		// ボーン・ディスプレイ・リスト 取得
		if(!parse_bone_disp_list_(fio)) {
			return false;
		}

		// ボーン・ディスプレイ 取得
		if(!parse_bone_disp_(fio)) {
			return false;
		}

#if 0
		uint32_t len = fio.tell();
		uint32_t l = fio.get_file_size();
		std::cout << (l - len) << std::endl;
#endif
		fio.close();

///		std::cout << fn << std::endl;

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void pmd_io::render()
	{
		if(vertex_.empty()) return;




	}

}
