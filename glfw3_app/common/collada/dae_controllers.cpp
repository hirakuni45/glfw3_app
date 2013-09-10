//=====================================================================//
/*!	@file
	@brief	collada library_controllers のパーサー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_controllers.hpp"
#include <boost/format.hpp>

namespace collada {

	using namespace boost::property_tree;
	using namespace std;

#if 0
	bool dae_controllers::parse_bind_shape_matrix_(utils::verbose& v, const ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "bind_shape_matrix") {
			return false;
		}
		utils::string_to_matrix4x4(element.second.data(), mat);		
		if(v()) {
			v.nest_out();
			cout << boost::format("%s: ") % s;
			for(int i = 0; i < 16; ++i) {
				if(i != 0) cout << ", ";
				cout << boost::format("%1.1f") % mat.m[i];
			}
			cout << endl;
		}

	}
#endif

	void dae_controllers::parse_controller_(utils::verbose& v, const ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "controller") {
			return;
		}

		const ptree& pt = element.second;
		controller ctrl;
		if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.id")) {
			ctrl.id_ = s.get();
		} else {
			// 'id'が無いのはエラー
			++error_;
		}

		if(v()) {
			v.nest_out();
			cout << boost::format("controller: id: '%s'") % ctrl.id_ << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, pt) {
//			parse_bind_shape_matrix_(v, child);
#if 0
			node nd;
			if(parse_node_(v, child, nd)) {
				vs.nodes_.push_back(nd);
			}
			extra ex;
			if(parse_extra_(v, child, ex)) {
				vs.extras_.push_back(ex);
			}
#endif
		}
		v.nest_up();

		if(error_ == 0) {
			controllers_.push_back(ctrl);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パース
		@return エラー数（「０」なら成功）
	*/
	//-----------------------------------------------------------------//
	int dae_controllers::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "library_controllers") {
			return 0;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		error_ = 0;

		const ptree& pt = element.second;
		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			parse_controller_(v, child);
		}
		v.nest_up();

		return error_;
	}

}
