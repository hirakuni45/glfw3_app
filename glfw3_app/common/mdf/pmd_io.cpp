//=====================================================================//
/*!	@file
	@brief	PMD ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "mdf/pmd_io.hpp"
#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"
#include "gl_fw/gl_info.hpp"
#include "img_io/img_files.hpp"

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

		vertex_.reserve(n);
		vertex_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_vertex v;
			if(!v.get(fio)) {
				return false;
			}
			if(i == 0) vertex_min_ = vertex_max_ = v.pos;
			vtx::set_min(v.pos, vertex_min_);
			vtx::set_max(v.pos, vertex_max_);
			vertex_.push_back(v);
		}
		return true;
	}

	bool pmd_io::parse_face_vertex_(utils::file_io& fio)
	{
		uint32_t n;
		if(!fio.get(n)) {
			return false;
		}

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

		material_.reserve(n);
		material_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_material m;
			if(!m.get(fio)) {
				return false;
			}
			material_.push_back(m);
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

		current_path_.clear();
		utils::get_file_path(fn, current_path_);

		destroy_();

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

		{
			std::string s;
			if(fio.get(s, 20) != 20) {
				return false;
			}
			utils::sjis_to_utf8(s, model_name_);
		}
		{
			std::string s;
			if(fio.get(s, 256) != 256) {
				return false;
			}
			utils::sjis_to_utf8(s, comment_);
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

		return true;
	}


	void pmd_io::destroy_()
	{
		if(vtx_id_) {
			glDeleteBuffers(1, &vtx_id_);
			vtx_id_ = 0;
		}

		if(!idx_id_.empty()) {
			glDeleteBuffers(idx_id_.size(), &idx_id_[0]);
			idx_id_.clear();
		}

		if(sphere_) {
			gluDeleteQuadric(sphere_);
			sphere_ = 0;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング・セットアップ
	*/
	//-----------------------------------------------------------------//
	void pmd_io::render_setup()
	{
		if(sphere_ == 0) {
			sphere_ = gluNewQuadric();
			gluQuadricDrawStyle(sphere_, GLU_FILL);
		}

		if(vertex_.empty()) return;
		if(face_index_.empty()) return;

		{
			std::vector<vbo_t> vbos;
			vbos.reserve(vertex_.size());
			vbos.clear();
			BOOST_FOREACH(pmd_vertex& v, vertex_) {
				vbo_t vbo;
				vbo.uv = v.uv;
				vbo.n = v.normal;			
				vbo.v.set(v.pos.x, v.pos.y, v.pos.z);
				vbos.push_back(vbo);
			}

			glGenBuffers(1, &vtx_id_);
			glBindBuffer(GL_ARRAY_BUFFER, vtx_id_);
			glBufferData(GL_ARRAY_BUFFER, vbos.size() * sizeof(vbo_t), &vbos[0],
				GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		std::vector<uint16_t> idxes;
		idxes.reserve(face_index_.size());
		idxes.clear();
		for(uint32_t i = 0; i < (face_index_.size() / 3); ++i) {
			idxes.push_back(face_index_[i * 3 + 0]);
			idxes.push_back(face_index_[i * 3 + 2]);
			idxes.push_back(face_index_[i * 3 + 1]);
		}

		idx_id_.resize(material_.size());
		glGenBuffers(material_.size(), &idx_id_[0]);

		img::img_files imf;
		imf.initialize();

		uint32_t n = 0;
		uint32_t in = 0;
		BOOST_FOREACH(pmd_material& m, material_) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id_[n]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.face_vert_count * sizeof(uint16_t),
				&idxes[in], GL_STATIC_DRAW);
			in += m.face_vert_count;
			++n;

			m.tex_id_ = 0;
			std::string mats;
			get_text_(m.texture_file_name, 20, mats);
// std::cout << mats << ", " << static_cast<int>(m.edge_flag) << std::endl;
			if(!mats.empty()) {
				std::string tfn;
	   			size_t pos = mats.find_first_of('*');
				if(std::string::npos != pos) {
					tfn = mats.substr(0, pos);
				} else {
					tfn = mats;
				}
				if(imf.load(current_path_ + '/' + tfn)) {
					const img::i_img* img = imf.get_image_if();
					if(img == 0) continue;
					glGenTextures(1, &m.tex_id_);
					glBindTexture(GL_TEXTURE_2D, m.tex_id_);
					int level = 0;
					int border = 0;
					glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA,
						img->get_size().x, img->get_size().y, border,
						GL_RGBA, GL_UNSIGNED_BYTE, img->get_image());
// std::cout << m.tex_id_ << ", " << img->get_size().x << ", " << img->get_size().y << std::endl;
				}
			}
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void pmd_io::render_surface()
	{
		if(vertex_.empty()) return;
		if(face_index_.empty()) return;

		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glScalef(-1.0f, 1.0f, 1.0f);

		glPushMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_DEPTH_TEST);
///		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
///		glEnable(GL_POLYGON_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 頂点情報をバインド
		glBindBuffer(GL_ARRAY_BUFFER, vtx_id_);
		glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);

		uint32_t n = 0;
		BOOST_FOREACH(const pmd_material& m, material_) {
			glColor4f(m.diffuse_color[0], m.diffuse_color[1], m.diffuse_color[2], m.alpha);
			if(m.tex_id_) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, m.tex_id_);
				GLenum edge;
				if(m.edge_flag) {
					edge = GL_CLAMP_TO_EDGE;
				} else {
					edge = GL_REPEAT;
				}
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edge);
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edge);
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			} else {
				glDisable(GL_TEXTURE_2D);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id_[n]);
			glDrawElements(GL_TRIANGLES, m.face_vert_count, GL_UNSIGNED_SHORT, 0);
			++n;
		}
		glDisable(GL_TEXTURE_2D);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

///		glDisable(GL_POLYGON_SMOOTH);

		glDisable(GL_CULL_FACE);

		glPopMatrix();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ボーンのレンダリング
	*/
	//-----------------------------------------------------------------//
	void pmd_io::render_bone()
	{
		if(bone_.empty()) return;

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glColor3f(1.0f, 1.0f, 1.0f);
		BOOST_FOREACH(const pmd_bone& bone, bone_) {
			glPushMatrix();
			glTranslatef(bone.head_pos.x, bone.head_pos.y, bone.head_pos.z);
			gluSphere(sphere_, 0.1f, 10.0f, 10.0f);
			glPopMatrix();
		}
	}

}
