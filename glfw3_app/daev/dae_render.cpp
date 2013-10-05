//=====================================================================//
/*!	@file
	@brief	メニュー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "dae_render.hpp"
#include "utils/preference.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_zero.hpp"

namespace core {

	using namespace gl;

	static collada::dae_io::triangle_meshes tms_;
	static vtx::fvtxs		bone_vertex_list_;
	static vtx::fvtxs		bone_normal_list_;

	void dae_render::service_light_()
	{
		int mno = light_.lookup_material("red_plastic");
//		int mno = light_.lookup_material("emerald");
//		int mno = light_.lookup_material("gold");
//		int mno = light_.lookup_material("chrome");
		light_.set_material(mno);
		light_.service();
		light_.enable();
	}


	static void create_bone_model_(float radius, float ratio, float length, vtx::fvtxs& vs, vtx::fvtxs& ns)
	{
		static vtx::fvtx rig[4];
		float r = radius * 0.707f;
		float w = length * ratio;
		rig[0].set( r,  r,  w);
		rig[1].set( r, -r,  w);
		rig[2].set(-r, -r,  w);
		rig[3].set(-r,  r,  w);
		vtx::fvtx end(0.0f, 0.0f, length);
		for(int i = 0; i < 4; ++i) {
			vs.push_back(vtx::fvtx(0.0f));
			vs.push_back(rig[i]);
			vs.push_back(rig[(i + 1) & 3]);
			{
				vtx::fvtx n;
				outer_product(rig[i], rig[(i + 1) & 3], n);
				ns.push_back(n);
				ns.push_back(n);
				ns.push_back(n);
			}
			vs.push_back(rig[(i + 1) & 3]);
			vs.push_back(rig[i]);
			vs.push_back(end);
			{
				vtx::fvtx n;
				outer_product(rig[i + 1] - end, rig[i] - end, n);
				ns.push_back(n);
				ns.push_back(n);
				ns.push_back(n);
			}
		}
	}


	static void draw_triangles_(const vtx::fvtxs& vs)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &vs[0]);
		::glDrawArrays(GL_TRIANGLES, 0, vs.size());
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	static void draw_triangles_(const vtx::fvtxs& vs, const vtx::fvtxs& ns)
	{
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glVertexPointer(3, GL_FLOAT, 0, &vs[0]);
		::glNormalPointer(GL_FLOAT, 0, &ns[0]);
		::glDrawArrays(GL_TRIANGLES, 0, vs.size());
		::glDisableClientState(GL_NORMAL_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	static void draw_mesh_(const collada::dae_io::triangle_mesh& mesh, bool color = false)
	{
		::glMultMatrixf(mesh.matrix_());
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glEnableClientState(GL_NORMAL_ARRAY);
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if(!mesh.color_.empty() && color) {
			::glEnableClientState(GL_COLOR_ARRAY);
			::glColorPointer(mesh.color_stride_, GL_FLOAT, 0, &mesh.color_[0]);
		}
		::glVertexPointer(mesh.vertex_stride_, GL_FLOAT, 0, &mesh.vertex_[0]);
		::glNormalPointer(GL_FLOAT, 0, &mesh.normal_[0]);
		::glTexCoordPointer(mesh.texcoord_stride_, GL_FLOAT, 0, &mesh.texcoord_[0]);
		::glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_.size() / 3);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if(!mesh.color_.empty() && color) {
			::glDisableClientState(GL_COLOR_ARRAY);
		}
		::glDisableClientState(GL_NORMAL_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}


	void dae_render::render_skeleton_(const vtx::fvtx& s, const collada::dae_visual_scenes::skeleton& sk)
	{
		const vtx::fvtx& j = sk.joint_;

//		glBegin(GL_LINES);
//		glVertex3f(s.x, s.y, s.z);
//		glVertex3f(j.x, j.y, j.z);
//		glEnd();

		vtx::fvtx dir;
		vtx::fvtx upv(0.0f, 0.0f, 1.0f);
		normalize(j - s, dir);
		float sc = vtx::distance(s, j) / 1.0f;

		::glPushMatrix();
		::glTranslatef(s.x, s.y, s.z);
		::glScalef(sc, sc, sc);
		qtx::fquat q;
		q.look_rotation(vtx::fvtx(dir.x, dir.y, dir.z), upv);
		mtx::fmat4 qm;
		q.create_rotate_matrix(qm);
		::glMultMatrixf(qm());
		draw_triangles_(bone_vertex_list_, bone_normal_list_);
///		gl::draw_line_cylinder(1.0f, 0.15f, 8);
		::glPopMatrix();

		::glPushMatrix();
		glTranslatef(j.x, j.y, j.z);
		glutSolidSphere(0.015f, 12, 12);
		::glPopMatrix();
		BOOST_FOREACH(const collada::dae_visual_scenes::skeleton& sk_, sk.skeletons_) {
			render_skeleton_(j, sk_);
		}
	}


	static void print_skeleton_(const vtx::fvtx& s, const collada::dae_visual_scenes::skeleton& sk)
	{
		const vtx::fvtx& j = sk.joint_;
		std::cout << boost::format("%f, %f, %f") % j.x % j.y % j.z << std::endl;
		BOOST_FOREACH(const collada::dae_visual_scenes::skeleton& sk_, sk.skeletons_) {
			print_skeleton_(j, sk_);
		}
	}


	void dae_render::physics_skeleton_(const vtx::fvtx& s, const collada::dae_visual_scenes::skeleton& sk)
	{
		const vtx::fvtx& j = sk.joint_;

// std::cout << "physics: " << boost::format("%g, %g, %g") % j.x % j.y % j.z << std::endl;

		capsule_body cap;
		cap.org_ = s * model_scale_;
		cap.end_ = j * model_scale_;
		float radius = 0.15f;
		if(s.len() == 0.0f) {
			physics_.set_current_mass(0.0f);
		} else {
			physics_.set_current_mass(1.0f);
		}
		if(j.len() != 0.0f) {
			vtx::fvtx l = cap.end_ - cap.org_;
			if(l.len() > 0.0f) {
				cap.body_ = physics_.create_capsule(cap.org_, cap.end_, radius);
				capsules_.push_back(cap);
			}
		}

		BOOST_FOREACH(const collada::dae_visual_scenes::skeleton& sk_, sk.skeletons_) {
			physics_skeleton_(j, sk_);
		}
	}


	void dae_render::create_material_()
	{
		BOOST_FOREACH(const collada::dae_io::triangle_mesh& mesh, tms_) {
			const collada::dae_io::material& mate = mesh.material_;



		}
	}


	void dae_render::destroy_material_()
	{
		BOOST_FOREACH(const collada::dae_io::triangle_mesh& mesh, tms_) {
			const collada::dae_io::material& mate = mesh.material_;
			if(mate.tex_id_) {

			}
		}
	}

	void dae_render::open_dae_(const std::string& fn)
	{
		if(!fn.empty()) {
			destroy_material_();
			dae_.destroy();
			dae_.parse(fn);
			dae_.create_triangle_mesh(tms_, model_scale_);
			create_material_();

#if 1
			const collada::dae_visual_scenes& vs = dae_.get_visual_scenes();
			vs.create_skeleton("head-node", skeletons_);

			capsules_.clear();
			BOOST_FOREACH(const collada::dae_visual_scenes::skeleton& sk, skeletons_) {
				physics_skeleton_(vtx::fvtx(0.0f), sk);
			}

			std::cout << "Capsule: " << capsules_.size() << std::endl;

			for(int i = 0; i < 2; ++i) {
				int idx = i * 2;
				vtx::fvtx a = capsules_[idx + 0].end_ - ((capsules_[idx + 0].org_ + capsules_[idx + 0].end_) * 0.5f);
				vtx::fvtx b = capsules_[idx + 1].end_ - ((capsules_[idx + 1].org_ + capsules_[idx + 1].end_) * 0.5f);
				physics_.create_spring_joint(capsules_[idx + 0].body_, capsules_[idx + 1].body_, a.len(), -b.len());
			}
#endif
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	 */
	//-----------------------------------------------------------------//
	void dae_render::initialize()
	{
		IGLcore* igl = get_glcore();
		igl->set_title("DAE Render");

		igl->set_cursor(arrow);

		sys::preference& pre = sys::preference::get_instance();
		pre.set_current_path("/dae_render");
		std::string s;
		pre.get_text("file_path", s);

		inten_.initialize(60.0f);
		filer_.initialize();
		filer_.set_root_path(s);

		// 詳細表示の設定
		dae_.at_verbose().set_level(utils::verbose::level::max);

//		dae_.parse("d:/MMD/LatMiku_2_3/LatNormal.dae");
//		dae_.parse("d:/Collada/hair_f20_a_secondly/hair_f20_a_secondly.DAE");
//		dae_.create_triangle_mesh(tms_);
//		const collada::dae_visual_scenes& vs = dae_.get_visual_scenes();
//		vs.create_skeleton("head-node", skeletons_);

#if 0
		BOOST_FOREACH(const collada::dae_visual_scenes::skeleton& sk, skeletons_) {
			print_skeleton_(vtx::fvtx(0.0f), sk);
		}
#endif
		// ボーン・モデル作成
		create_bone_model_(0.11f, 0.15f, 1.0f, bone_vertex_list_, bone_normal_list_);

		physics_.initialize();
		physics_.create_ground_body();

		// ライトの設定など
		{
			light::handle h = light_.create();
			light_.set_position(h, vtx::fvtx(70.0f,  70.0f, 100.0f));
			light_.set_ambient_color(h, img::rgbaf(0.2f, 0.2f, 0.2f));
			light_.set_diffuse_color(h, img::rgbaf(0.8f, 0.6f, 0.6f));
			light_.set_specular_color(h, img::rgbaf(1.0f, 1.0f, 1.0f));

			h = light_.create();
			light_.set_position(h, vtx::fvtx(-100.0f, -70.0f, -110.0f));
			light_.set_ambient_color(h, img::rgbaf(0.2f, 0.2f, 0.2f));
			light_.set_diffuse_color(h, img::rgbaf(0.6f, 0.6f, 0.8f));
			light_.set_specular_color(h, img::rgbaf(1.0f, 1.0f, 1.0f));
		}

		// gui の設定
		gui::widget_director& wd = director_.at_widget_director();
		{
			gui::widget::param param(vtx::spos(0, 0), vtx::spos(500, 50));
			gui::widget_frame::param sub_param;
			tools_palette_ = wd.add_widget<gui::widget_frame>(param, sub_param);
		}
		{
			gui::widget::param param(vtx::spos(5, 5), vtx::spos(100, 40), tools_palette_);
			gui::widget_button::param sub_param;
			sub_param.text_ = "ファイル";
			filer_button_ = wd.add_widget<gui::widget_button>(param, sub_param);
		}
		{
			gui::widget::param param(vtx::spos(110, 5), vtx::spos(100, 40), tools_palette_);
			gui::widget_button::param sub_param;
			sub_param.text_ = "物理";
			physics_button_ = wd.add_widget<gui::widget_button>(param, sub_param);
		}
		{
			gui::widget::param param(vtx::spos(215, 5), vtx::spos(100, 40), tools_palette_);
			gui::widget_button::param sub_param;
			sub_param.text_ = "あああ";
			wd.add_widget<gui::widget_button>(param, sub_param);
		}
		{
			gui::widget::param param(vtx::spos(350, 15), vtx::spos(20, 20), tools_palette_);
			gui::widget_check::param sub_param;
			wd.add_widget<gui::widget_check>(param, sub_param);
		}
		{
			gui::widget::param param(vtx::spos(400, 15), vtx::spos(20, 20), tools_palette_);
			gui::widget_zero::param sub_param;
			wd.add_widget<gui::widget_zero>(param, sub_param);
		}



	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	 */
	//-----------------------------------------------------------------//
	void dae_render::update()
	{
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		const gldev& dev = igl->get_device();

		int scw = igl->get_width();
		int sch = igl->get_height();

		camera_.update();

		if(dev.get_key_positive(KEY_NORMAL + 'w') || dev.get_key_positive(KEY_NORMAL + 'W')) {
			wire_frame_ = !wire_frame_;
		}
		if(dev.get_key_positive(KEY_NORMAL + 'g') || dev.get_key_positive(KEY_NORMAL + 'G')) {
			grid_enable_ = !grid_enable_;
		}
		if(dev.get_key_positive(KEY_NORMAL + 'j') || dev.get_key_positive(KEY_NORMAL + 'J')) {
			joint_enable_ = !joint_enable_;
		}
		if(dev.get_key_positive(KEY_NORMAL + 'm') || dev.get_key_positive(KEY_NORMAL + 'M')) {
			model_enable_ = !model_enable_;
		}
		if(dev.get_key_positive(KEY_NORMAL + 'l') || dev.get_key_positive(KEY_NORMAL + 'L')) {
			light_enable_ = !light_enable_;
		}

//		if(dev.get_key_positive(KEY_NORMAL + 0x1b)) {
//			physics_enable_ = !physics_enable_;
//			if(physics_enable_) physics_time_ = 0.0f;
//		}

		bool filer = false;

		// CTRL-F
		if(dev.get_key_positive(KEY_NORMAL + 'F' - 0x40)) {
			filer = true;
		}

		if(physics_enable_) {
			physics_.update(camera_);
			physics_time_ += 1.0f / 60.0f;
		}

		unsigned int cnt_f = filer_button_->get_count();
		unsigned int cnt_p = physics_button_->get_count();
		gui::widget_director& wd = director_.at_widget_director();
		wd.update();
		if(filer_button_->get_count() != cnt_f) {
			filer = true;
		}
		if(physics_button_->get_count() != cnt_p) {
			physics_enable_ = !physics_enable_;
			if(physics_enable_) physics_time_ = 0.0f;
		}

		if(filer) {
			filer_.set_ctrl(glfiler::enable);
			filer_.set_ctrl(glfiler::visible);
			filer_.set_ctrl(glfiler::filepath);
			filer_.set_ctrl(glfiler::mouse_select);
			filer_.set_ctrl(glfiler::drive_letter);
		}

		tools_palette_->at_position().set(0);
		tools_palette_->at_size().x = scw;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	 */
	//-----------------------------------------------------------------//
	void dae_render::render()
	{
		IGLcore* igl = get_glcore();
		if(igl == 0) return;

		int scw = igl->get_width();
		int sch = igl->get_height();

//		resource* res = get_resource();
//		if(res == 0) return;

		inten_.service();

		::glEnable(GL_DEPTH_TEST);

		camera_.service();

		::glPushMatrix();
		physics_.render();
		::glPopMatrix();

		::glDisable(GL_TEXTURE_GEN_S);
		::glDisable(GL_TEXTURE_GEN_T);
		::glDisable(GL_TEXTURE_GEN_R);

		if(light_enable_) {
			::glEnable(GL_LIGHTING);
			service_light_();
		} else {
			::glDisable(GL_LIGHTING);
		}

		if(joint_enable_) {
			::glDisable(GL_TEXTURE_2D);
			::glPushMatrix();
			::glScalef(model_scale_.x, model_scale_.y, model_scale_.z);
			BOOST_FOREACH(const collada::dae_visual_scenes::skeleton& sk, skeletons_) {
				render_skeleton_(vtx::fvtx(0.0f), sk);
			}
			::glPopMatrix();
		}

		if(model_enable_) {
			::glDisable(GL_TEXTURE_2D);
			::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if(wire_frame_) {
				::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			::glPushMatrix();
//			::glScalef(0.05f, 0.05f, 0.05f);
//			::glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			BOOST_FOREACH(const collada::dae_io::triangle_mesh& mesh, tms_) {
				draw_mesh_(mesh);
			}
			::glPopMatrix();
		}

		if(grid_enable_) {
			::glDisable(GL_TEXTURE_2D);
			::glColor3f(1.0f, 1.0f, 1.0f);
			::glDisable(GL_LIGHTING);
			::glEnable(GL_LINE_SMOOTH);
			::glEnable(GL_BLEND);
			::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			::glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
			draw_grid(-10.0f, -10.0f, 10.0f, 10.0f, 1.0f, 1.0f);
		}

		if(0) {
			::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			::glDisable(GL_TEXTURE_GEN_S);
			::glDisable(GL_TEXTURE_GEN_T);
			::glDisable(GL_TEXTURE_GEN_R);
			::glDisable(GL_LIGHTING);
			::glDisable(GL_STENCIL_TEST);
			::glCullFace(GL_BACK);
			::glDisable(GL_CULL_FACE);
			::glColor4ub(255, 255, 255, 255);
			::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			::glEnable(GL_TEXTURE_2D);
			::glEnable(GL_BLEND);
			::glDisable(GL_DEPTH_TEST);
			::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glfonts& fo = igl->at_fonts();
			fo.setup_matrix(0, 0, scw, sch);
			fo.set_clip_org(vtx::spos(0, 0));
			fo.set_clip_size(vtx::spos(scw, sch));
			fo.set_fore_color(img::rgba8(255));
			fo.set_back_color(img::rgba8(0));
			std::string s = (boost::format("Physics: %3.1f") % physics_time_).str();
			fo.draw(0, 0, s);
			fo.restore_matrix();
		}

		{
			::glDisable(GL_LIGHTING);
			gui::widget_director& wd = director_.at_widget_director();
			wd.render();
		}

		{

			::glEnable(GL_TEXTURE_2D);
			glfonts& fo = igl->at_fonts();
			fo.setup_matrix(0, 0, scw, sch);
//			vtx::spos s = fo.get_size();
			filer_.set_size(scw, sch);
			bool file_select = filer_.service(inten_);
			if(file_select) {
				open_dae_(filer_.get_selected_filename());
			}
			fo.restore_matrix();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void dae_render::destroy()
	{
		// ファイラー、カレントパスの記録
		sys::preference& pre = sys::preference::get_instance();
		pre.set_current_path("/dae_render");
		pre.put_text("file_path", filer_.get_root_path());

		std::cout << "DAE Render - destroy" << std::endl;
	}

}
