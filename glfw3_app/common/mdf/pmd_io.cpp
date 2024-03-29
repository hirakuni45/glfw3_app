//=====================================================================//
/*!	@file
	@brief	PMD ファイルを扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <iostream>
#include <boost/foreach.hpp>
#include "mdf/pmd_io.hpp"
#include "utils/string_utils.hpp"
#include "img_io/img_files.hpp"
#include "gl_fw/gl_info.hpp"
#include "gl_fw/glutils.hpp"
#include "utils/quat.hpp"

namespace mdf {

	void pmd_io::get_text_(const char* src, uint32_t n, std::string& dst)
	{
		std::string tmp;
		for(uint32_t i = 0; i < n; ++i) {
			char ch = *src++;
			if(ch == 0) break;
			if(static_cast<uint8_t>(ch) != 0xfd) {
///				uint32_t n = static_cast<uint8_t>(ch);
///				std::cout << boost::format("%02X, ") % n;
				tmp += ch;
			}
		}
		utils::sjis_to_utf8(tmp, dst);
///		std::cout << "'" << dst << "'" << std::endl;
	}


	bool pmd_io::parse_vertex_(utils::file_io& fio)
	{
		uint32_t n;
		if(!fio.get(n)) {
			return false;
		}

		vertices_.reserve(n);
		vertices_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_vertex v;
			if(!v.get(fio)) {
				return false;
			}
			if(i == 0) vertex_min_ = vertex_max_ = v.pos;
			vtx::set_min(v.pos, vertex_min_);
			vtx::set_max(v.pos, vertex_max_);
			vertices_.push_back(v);
		}
		return true;
	}


	bool pmd_io::parse_face_vertex_(utils::file_io& fio)
	{
		uint32_t n;
		if(!fio.get(n)) {
			return false;
		}

		face_indices_.reserve(n);
		face_indices_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			uint16_t v;
			if(!fio.get(v)) {
				return false;
			}
			face_indices_.push_back(v);
		}
		return true;
	}


	bool pmd_io::parse_material_(utils::file_io& fio)
	{
		uint32_t n;
		if(!fio.get(n)) {
			return false;
		}

		materials_.reserve(n);
		materials_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_material m;
			if(!m.get(fio)) {
				return false;
			}
			materials_.push_back(m);
		}
		return true;
	}


	bool pmd_io::parse_bone_(utils::file_io& fio)
	{
		uint16_t n;
		if(!fio.get(n)) {
			return false;
		}
//		std::cout << "Bone num: " << n << std::endl;

		bones_.reserve(n);
		bones_.clear();
		for(uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
			pmd_bone b;
			if(!b.get(fio)) {
				return false;
			}
			bones_.push_back(b);
//			std::string s;
//			get_text_(b.name, 20, s);
//			std::cout << "Bone: '" << s << "', type: " << static_cast<int>(b.type)
//				<< std::endl;
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

		iks_.reserve(n);
		iks_.clear();
		for(uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
			pmd_ik ik;
			if(!ik.get(fio)) {
				return false;
			}
			iks_.push_back(ik);
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

		skins_.reserve(n);
		skins_.clear();
		for(uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
			pmd_skin ps;
			if(!ps.get(fio)) {
				return false;
			}
			skins_.push_back(ps);
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

		skin_indices_.reserve(n);
		skin_indices_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			uint16_t v;
			if(!fio.get(v)) {
				return false;
			}
			skin_indices_.push_back(v);
		}

		return true;
	}


	bool pmd_io::parse_bone_disp_name_(utils::file_io& fio)
	{
		uint8_t n;
		if(!fio.get(n)) {
			return false;
		}
//		std::cout << "Bone disp list num: " << static_cast<int>(n) << std::endl;

		bone_disp_names_.reserve(n);
		bone_disp_names_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_bone_disp_name bdl;
			if(!bdl.get(fio)) {
				return false;
			}
			bone_disp_names_.push_back(bdl);
//			std::string s;
//			get_text_(bdl.name, 50, s);
//			std::cout << "Bone disp list: " << s << std::endl;
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

		bone_disps_.reserve(n);
		bone_disps_.clear();
		for(uint32_t i = 0; i < n; ++i) {
			pmd_bone_disp bd;
			if(!bd.get(fio)) {
				return false;
			}
			bone_disps_.push_back(bd);
		}
		return true;
	}


//	static void draw_joint_(const vtx::fvyx& t, const vtx::fvtx& e, float w, float ratio)
	static void draw_joint_(float w, float up, float down)
	{
//		vtx::fvtx n = vtx::normalize(e - t);

		vtx::fvtxs vs;

		vs.push_back(vtx::fvtx(0.0f, 0.0f, 0.0f));
		vs.push_back(vtx::fvtx(   w,    w, down));
		vs.push_back(vtx::fvtx(   w,   -w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, 0.0f));
		vs.push_back(vtx::fvtx(   w,   -w, down));
		vs.push_back(vtx::fvtx(  -w,   -w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, 0.0f));
		vs.push_back(vtx::fvtx(  -w,   -w, down));
		vs.push_back(vtx::fvtx(  -w,    w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, 0.0f));
		vs.push_back(vtx::fvtx(  -w,    w, down));
		vs.push_back(vtx::fvtx(   w,    w, down));

		vs.push_back(vtx::fvtx(   w,   -w, down));
		vs.push_back(vtx::fvtx(   w,    w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, up));
		vs.push_back(vtx::fvtx(  -w,   -w, down));
		vs.push_back(vtx::fvtx(   w,   -w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, up));
		vs.push_back(vtx::fvtx(  -w,    w, down));
		vs.push_back(vtx::fvtx(  -w,   -w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, up));
		vs.push_back(vtx::fvtx(   w,    w, down));
		vs.push_back(vtx::fvtx(  -w,    w, down));
		vs.push_back(vtx::fvtx(0.0f, 0.0f, up));

		vtx::fvtxs ns;
		for(uint32_t i = 0; i < 8; ++i) {
			vtx::fvtx n;
			vtx::fvtx::cross(vs[i * 3 + 0], vs[i * 3 + 2], n);
			vtx::fvtx out;
			vtx::normalize(n, out);
			ns.push_back(out);
			ns.push_back(out);
			ns.push_back(out);
		}

		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &ns[0]);
		glVertexPointer(3, GL_FLOAT, 0, &vs[0]);
		glDrawArrays(GL_TRIANGLES, 0, vs.size());
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}


	void pmd_io::initialize_()
	{
		// Joint オブジェクトの生成
		joint_list_id_ = glGenLists(1);
		glNewList(joint_list_id_, GL_COMPILE);		
		draw_joint_(0.1f, 1.0f, 0.3f);
		glEndList();
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

		if(!tex_id_.empty()) {
			glDeleteTextures(tex_id_.size(), &tex_id_[0]);
			tex_id_.clear();
		}

		glDeleteLists(bone_list_id_, 1);
	}


	static bool probe_(utils::file_io& fio)
	{
		std::string s;
		if(fio.get(s, 3) != 3) {
			return false;
		}
		if(s != "Pmd") {
			return false;
		}
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルが有効か検査
		@return 有効なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmd_io::probe(utils::file_io& fio)
	{
		size_t pos = fio.tell();
		bool f = probe_(fio);
		fio.seek(pos, utils::file_io::SEEK::SET);
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロード
		@param[in]	fn	ファイル名
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmd_io::load(utils::file_io& fio)
	{
		initialize_();

		current_path_ = utils::get_file_path(fio.get_path());

		destroy_();

		if(!probe_(fio)) {
			return false;
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
		if(!parse_bone_disp_name_(fio)) {
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
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	セーブ
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmd_io::save(utils::file_io& fio)
	{


		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング・セットアップ
	*/
	//-----------------------------------------------------------------//
	void pmd_io::render_setup()
	{
		if(vertices_.empty()) return;
		if(face_indices_.empty()) return;

		{	// 頂点バッファの作成
			std::vector<vbo_t> vbos;
			vbos.reserve(vertices_.size());
			vbos.clear();
			BOOST_FOREACH(pmd_vertex& v, vertices_) {
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

		{ // インデックス・バッファの作成（マテリアル別に作成）
			std::vector<uint16_t> ids;
			ids.reserve(face_indices_.size());
			ids.clear();
			for(uint32_t i = 0; i < (face_indices_.size() / 3); ++i) {
				ids.push_back(face_indices_[i * 3 + 0]);
				ids.push_back(face_indices_[i * 3 + 2]);
				ids.push_back(face_indices_[i * 3 + 1]);
			}

			idx_id_.resize(materials_.size());
			glGenBuffers(materials_.size(), &idx_id_[0]);

			uint32_t n = 0;
			uint32_t in = 0;
			BOOST_FOREACH(const pmd_material& m, materials_) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id_[n]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							 m.face_vert_count * sizeof(uint16_t),
							 &ids[in], GL_STATIC_DRAW);
				in += m.face_vert_count;
				++n;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		// マテリアル（テクスチャー）の作成と登録
		tex_id_.resize(materials_.size());
		glGenTextures(tex_id_.size(), &tex_id_[0]);
		
		img::img_files imf;
		uint32_t i = 0;
		BOOST_FOREACH(pmd_material& m, materials_) {
			std::string mats;
			get_text_(m.texture_file_name, 20, mats);
			if(!mats.empty()) {
				std::string tfn;
	   			size_t pos = mats.find_first_of('*');
				if(std::string::npos != pos) {
					tfn = mats.substr(0, pos);
				} else {
					tfn = mats;
				}
				if(imf.load(current_path_ + '/' + tfn)) {
					const img::i_img* img = imf.get_image().get();
					if(img == 0) continue;

					glBindTexture(GL_TEXTURE_2D, tex_id_[i]);
					int level = 0;
					int border = 0;
					int w = img->get_size().x;
					int h = img->get_size().y;
					glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
					glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, w, h, border,
								 GL_RGBA, GL_UNSIGNED_BYTE, (*img)());
				}
			}
			++i;
		}

		// Bone ジョイントの作成
		// bone joint size

		bone_joint_size_ = (vertex_max_ - vertex_min_).len() / 200.0f;
#if 0
		GLUquadricObj* sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		bone_list_id_ = glGenLists(1);
		glNewList(bone_list_id_, GL_COMPILE);		
		gluSphere(sphere, bone_joint_size_, 10.0f, 10.0f);
		glEndList();
		gluDeleteQuadric(sphere);
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void pmd_io::render_surface()
	{
		if(vertices_.empty()) return;
		if(face_indices_.empty()) return;

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glScalef(-1.0f, 1.0f, 1.0f);

		glPushMatrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 頂点情報をバインド
		glBindBuffer(GL_ARRAY_BUFFER, vtx_id_);
		glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);

		uint32_t n = 0;
		BOOST_FOREACH(const pmd_material& m, materials_) {
			glColor4f(m.diffuse_color[0], m.diffuse_color[1], m.diffuse_color[2], m.alpha);
			if(tex_id_[n]) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tex_id_[n]);
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
		if(bones_.empty()) return;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glScalef(-1.0f, 1.0f, 1.0f);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		glLineWidth(4.0f);

		BOOST_FOREACH(const pmd_bone& bone, bones_) {
///			glPushMatrix();
///			gl::glTranslate(bone.head_pos);
///			glCallList(bone_list_id_);
///			lig.set_material(gl::light::material::pearl);
///			glPopMatrix();

			uint16_t idx = bone.parent_index_;
			if(idx < bones_.size()) {
				glPushMatrix();
///				gl::glTranslate(bone.head_pos);
///				vtx::fvtx sc((bone.head_pos - bones_[idx].head_pos).len());
///				gl::glScale(sc);
///				glCallList(joint_list_id_);
				glColor3f(1.0f, 1.0f, 1.0f);
				gl::draw_line(bone.position_, bones_[idx].position_);
				glPopMatrix();
			}
		}

		glDisable(GL_CULL_FACE);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ボーンのフルパスを生成
		@param[in]	index	インデックス
		@param[out]	path	フル・パスを受け取る参照	
	*/
	//-----------------------------------------------------------------//
	bool pmd_io::create_bone_path(uint32_t index, std::string& path)
	{
		if(index >= bones_.size()) return false;

		using namespace mdf;
		std::vector<uint16_t> ids;
		do {
			const pmd_bone& bone = bones_[index];
			ids.push_back(index);
			index = bone.parent_index_;
		} while(index != 0xffff) ;

		BOOST_REVERSE_FOREACH(uint16_t idx, ids) {
			const pmd_bone& bone = bones_[idx];
			std::string s;
			pmd_io::get_text_(bone.name_, sizeof(bone.name_), s);
			if(!s.empty()) {
				path += '/';
				path += s;
			}
		}
		return true;
	}

}
