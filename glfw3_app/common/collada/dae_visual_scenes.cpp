//=====================================================================//
/*!	@file
	@brief	collada visual_scenes のパーサー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_visual_scenes.hpp"
#include <boost/format.hpp>

namespace collada {

	using namespace boost::property_tree;
	using namespace std;

	bool dae_visual_scenes::parse_controller_(utils::verbose& v,
		const ptree::value_type& element, controller& cont)
	{
		const std::string& s = element.first.data();
		if(s != "instance_controller") {
			return false;
		}

		const ptree& pt = element.second;
		if(boost::optional<string> os = pt.get_optional<string>("<xmlattr>.url")) {
			cont.url_ = os.get();
			if(v()) {
				v.nest_out();
				cout << boost::format("%s: url: '%s'") % s % os.get() << endl;
			}
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, pt) {
			const std::string& s = child.first.data();
			if(s == "skeleton") {
				cont.skeletons_.push_back(child.second.data());
				if(v()) {
					v.nest_out();
					cout << boost::format("skeleton: '%s'") % cont.skeletons_.back() << endl;
				}
			}
		}
		v.nest_up();

		return true;
	}


	bool dae_visual_scenes::parse_geometry_(utils::verbose& v,
		const ptree::value_type& element, geometry& geom)
	{
		const std::string& s = element.first.data();
		if(s != "instance_geometry") {
			return false;
		}
		if(boost::optional<string> os = element.second.get_optional<string>("<xmlattr>.url")) {
			geom.url_ = os.get();
			if(v()) {
				v.nest_out();
				cout << boost::format("%s: url: '%s'") % s % os.get() << endl;
			}
		}
		return true;
	}


	bool dae_visual_scenes::parse_light_(utils::verbose& v,
		const ptree::value_type& element, light& lig)
	{
		const std::string& s = element.first.data();
		if(s != "instance_light") {
			return false;
		}
		if(boost::optional<string> os = element.second.get_optional<string>("<xmlattr>.url")) {
			lig.url_ = os.get();
			if(v()) {
				v.nest_out();
				cout << boost::format("%s: url: '%s'") % s % os.get() << endl;
			}
		}
		return true;
	}


	bool dae_visual_scenes::parse_matrix_(utils::verbose& v,
		const ptree::value_type& element, mtx::fmat4& mat)
	{
		const std::string& s = element.first.data();
		if(s != "matrix") {
			return false;
		}
		utils::string_to_matrix4x4(element.second.data(), mat);				
		if(v()) {
			v.nest_out();
			cout << boost::format("%s: ") % s;
			for(int i = 0; i < 16; ++i) {
				if(i != 0) cout << ", ";
				cout << boost::format("%g") % mat.m[i];
			}
			cout << endl;
		}
		return true;
	}


	bool dae_visual_scenes::parse_node_(utils::verbose& v, const ptree::value_type& element, node& nd)
	{
		const std::string& s = element.first.data();
		if(s != "node") return false;

		const ptree& pt = element.second;
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.id")) {
			nd.id_ = s.get();
		}
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.name")) {
			nd.name_ = s.get();
		}
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.type")) {
			if(s.get() == "NODE") {
				nd.type_ = node::NODE;
			} else if(s.get() == "JOINT") {
				nd.type_ = node::JOINT;
				if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.sid")) {
					nd.sid_ = s.get();
				}
			}
		}

		if(v()) {
			string t;
			if(nd.type_ == node::NODE) t = "NODE";
			else if(nd.type_ == node::JOINT) t = "JOINT";
			v.nest_out();
			cout << boost::format("node: id: '%s'") % nd.id_;
			if(!nd.name_.empty()) cout << boost::format(", name: '%s'") % nd.name_;
			if(!nd.sid_.empty()) cout << boost::format(", sid: '%s'") % nd.sid_;
			cout << boost::format(", type: '%s'") % nd.type_ << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, pt) {
			if(parse_controller_(v, child, nd.controller_)) continue;
			if(parse_geometry_(v, child, nd.geometry_)) continue;
			if(parse_light_(v, child, nd.light_)) continue;
			node newnode;
			if(parse_node_(v, child, newnode)) {
				nd.nodes_.push_back(newnode);
				continue;
			}
			if(parse_matrix_(v, child, nd.matrix_)) continue;
			extra ex;
			if(parse_extra_(v, child, ex)) {
				nd.extras_.push_back(ex);
				continue;
			}
		}
		v.nest_up();

		return true;
	}


	bool dae_visual_scenes::parse_extra_(utils::verbose& v, const ptree::value_type& element, extra& ex)
	{
		const std::string& s = element.first.data();
		if(s != "extra") return false;
		if(v()) {
			v.nest_out();
			cout << boost::format("%s:") % s << endl;
		}

		const ptree& pt = element.second;
		if(boost::optional<string> s = pt.get_optional<string>("technique.<xmlattr>.profile")) {

		}

		return true;
	}


	void dae_visual_scenes::parse_visual_scene_(utils::verbose& v, const ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "visual_scene") {
			return;
		}

		const ptree& pt = element.second;
		visual_scene vs;
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.id")) {
			vs.id_ = s.get();
		} else {
			// 'id'が無いのはエラー
			++error_;
		}
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.name")) {
			vs.name_ = s.get();
		}

		if(v()) {
			v.nest_out();
			cout << boost::format("visual_scene: id: '%s', name: '%s'") % vs.id_ % vs.name_ << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, pt) {
			node nd;
			if(parse_node_(v, child, nd)) {
				vs.nodes_.push_back(nd);
				continue;
			}
			extra ex;
			if(parse_extra_(v, child, ex)) {
				vs.extras_.push_back(ex);
				continue;
			}
		}
		v.nest_up();

		if(error_ == 0) {
			visual_scenes_.push_back(vs);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パース
		@return エラー数（「０」なら成功）
	*/
	//-----------------------------------------------------------------//
	int dae_visual_scenes::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		error_ = 0;

		const std::string& s = element.first.data();
		if(s != "library_visual_scenes") {
			return error_;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		const ptree& pt = element.second;
		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			parse_visual_scene_(v, child);
		}
		v.nest_up();

		return error_;
	}


	void dae_visual_scenes::joint_vertex_(const node& nd, gl::glmatf& glmat, skeleton& sk) const
	{
		glmat.mult(nd.matrix_);
		vtx::fvtx4 v;
		gl::glmatf::vertex_world(glmat.get_current_matrix(), vtx::fvtx(0.0f), v);
		sk.joint_.set(v.x, v.y, v.z);
// std::cout << "joint: " << boost::format("%g, %g, %g") % v.x % v.y % v.z << std::endl;
		BOOST_FOREACH(const node& nd, nd.nodes_) {
			glmat.push();
			skeleton newsks;
			joint_vertex_(nd, glmat, newsks);
			sk.skeletons_.push_back(newsks);
			glmat.pop();
		}
	}


	void dae_visual_scenes::create_skeleton(const std::string& id, skeletons& sks) const
	{
		BOOST_FOREACH(const visual_scene& vc, visual_scenes_) {
			BOOST_FOREACH(const node& nd, vc.nodes_) {
				if(nd.id_ == id) {
					gl::glmatf glmat;
					glmat.initialize();
					skeleton sk;
					joint_vertex_(nd, glmat, sk);
					sks.push_back(sk);
					return;
				}
			}
		}
	}

}
