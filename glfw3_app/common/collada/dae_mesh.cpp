#include "dae_mesh.hpp"
#include "utils/string_utils.hpp"
#include <boost/format.hpp>

namespace collada {

	using namespace boost::property_tree;
	using namespace std;

	bool dae_mesh::parse_input_(const boost::property_tree::ptree::value_type& element, input& inp)
	{
		const std::string& s = element.first.data();
		if(s != "input") return false;

		const ptree& pt = element.second;
		if(boost::optional<string> opt = pt.get_optional<string>("<xmlattr>.semantic")) {
			const std::string& s = opt.get();
			if(s == "POSITION") {
				inp.semantic_ = input::semantic::POSITION;
			} else if(s == "VERTEX") {
				inp.semantic_ = input::semantic::VERTEX;
			} else if(s == "NORMAL") {
				inp.semantic_ = input::semantic::NORMAL;
			} else if(s == "COLOR") {
				inp.semantic_ = input::semantic::COLOR;
			} else if(s == "TEXCOORD") {
				inp.semantic_ = input::semantic::TEXCOORD;
			} else {
				++error_;
				return false;
			}
			inp.source_.clear();
			inp.offset_ = 0;
			inp.set_ = 0;
		}

		if(boost::optional<string> os = pt.get_optional<string>("<xmlattr>.source")) {
			inp.source_ = os.get();
		}

		if(boost::optional<string> os = pt.get_optional<string>("<xmlattr>.offset")) {
			utils::int_vector iv;
			if(utils::string_to_int(os.get(), iv) && iv.size() >= 1) {
				inp.offset_ = iv[0];
			}
		}

		if(boost::optional<string> os = pt.get_optional<string>("<xmlattr>.set")) {
			utils::int_vector iv;
			if(utils::string_to_int(os.get(), iv) && iv.size() >= 1) {
				inp.set_ = iv[0];
			}
		}

		return true;
	}


	void dae_mesh::parse_source_(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "source") return;

		source src;

		const ptree& pt = element.second;
		if(boost::optional<string> id = pt.get_optional<string>("<xmlattr>.id")) {
			src.id_ = id.get();
		} else {
			++error_;
			return;
		}

		int count = 0;
		BOOST_FOREACH(const ptree::value_type& child, pt) {
			const string& s = child.first.data();
			if(s == "float_array") {
				utils::string_to_float(child.second.data(), src.array_);
			} else if(s == "technique_common") {
				const ptree& pt = child.second;
				if(boost::optional<string> ops = pt.get_optional<string>("accessor.<xmlattr>.count")) {
					utils::int_vector iv;
					if(utils::string_to_int(ops.get(), iv) && !iv.empty()) {
						count = iv[0];
					}
				}
				if(boost::optional<string> ops = pt.get_optional<string>("accessor.<xmlattr>.stride")) {
					utils::int_vector iv;
					if(utils::string_to_int(ops.get(), iv) && !iv.empty()) {
						src.stride_ = iv[0];
					}
				}
			}
		}
		if(src.stride_ == 0 || count == 0) {
			++error_;
			return;
		}

		if(v()) {
			v.nest_out();
			cout << boost::format("source: '%s', stride: %d") % src.id_ % src.stride_;
			cout << boost::format(", float_array: (%d)") % src.array_.size() << endl;
		}

		sourceies_.push_back(src);
	}


	void dae_mesh::parse_vertices_(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "vertices") return;

		vertice vert;

		const ptree& pt = element.second;
		if(boost::optional<string> id = pt.get_optional<string>("<xmlattr>.id")) {
			vert.id_ = id.get();
		} else {
			++error_;
			return;
		}

		BOOST_FOREACH(const ptree::value_type& child, pt) {
			input inp;
			if(parse_input_(child, inp)) {
				vert.input_ = inp;
				if(v()) {
					v.nest_out();
					cout << boost::format("vertices: id: '%s', source: '%s'") %
						vert.id_ % vert.input_.source_ << endl;
				}
			}
		}
		vertices_.push_back(vert);
	}


	void dae_mesh::parse_triangles_(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "triangles") return;

		triangle tri;

		const ptree& pt = element.second;
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.material")) {
			tri.material_ = s.get();
		} else {
			++error_;
			return;
		}

		int count;
		if(boost::optional<string> os = pt.get_optional<string>("<xmlattr>.count")) {
			utils::int_vector iv;
			if(utils::string_to_int(os.get(), iv) && iv.size() >= 1) {
				count = iv[0];
			}
		} else {
			++error_;
			return;
		}

		if(v()) {
			v.nest_out();
			cout << boost::format("triangle: material: '%s', count: %d") % tri.material_ % count << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, pt) {
			input inp;
			if(parse_input_(child, inp)) {
				if(v()) {
					v.nest_out();
					if(inp.semantic_ == input::semantic::VERTEX) {
						cout << boost::format("VERTEX: source: '%s', offset: %d")
							% inp.source_ % inp.offset_ << endl;
					} else if(inp.semantic_ == input::semantic::NORMAL) {
						cout << boost::format("NORMAL: source: '%s', offset: %d")
							% inp.source_ % inp.offset_ << endl;
					} else if(inp.semantic_ == input::semantic::COLOR) {
						cout << boost::format("COLOR: source: '%s', offset: %d, set: %d")
							% inp.source_ % inp.offset_ % inp.set_ << endl;
					} else if(inp.semantic_ == input::semantic::TEXCOORD) {
						cout << boost::format("TEXCOORD: source: '%s', offset: %d, set: %d")
							% inp.source_ % inp.offset_ % inp.set_ << endl;
					} else {
						cout << "semantic error..." << endl;
					}
				}
				tri.inputs_.push_back(inp);
			}
		}

		if(boost::optional<string> ops = pt.get_optional<string>("p")) {
			utils::string_to_int(ops.get(), tri.pointer_);
			if(v()) {
				v.nest_out();
				cout << boost::format("pointer: (%d)") % tri.pointer_.size() << endl;
			}
		} else {
			++error_;
			return;
		}
		v.nest_up();

		triangles_.push_back(tri);
	}


	int dae_mesh::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		error_ = 0;

		const std::string& s = element.first.data();
		if(s != "mesh") {
			++error_;
			return error_;
		}

		if(v()) {
			v.nest_out();
			cout << "mesh:" << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			parse_source_(v, child);
			parse_vertices_(v, child);
			parse_triangles_(v, child);
		}
		v.nest_up();

		return error_;
	}

}

