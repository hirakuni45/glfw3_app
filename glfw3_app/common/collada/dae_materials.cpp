#include "dae_materials.hpp"
#include <boost/format.hpp>

namespace collada {

	using namespace boost::property_tree;
	using namespace std;

	//-----------------------------------------------------------------//
	/*!
		@brief	パース
		@return エラー数（「０」なら成功）
	*/
	//-----------------------------------------------------------------//
	int dae_materials::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		error_ = 0;

		const std::string& s = element.first.data();
		if(s != "library_materials") {
			return error_;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			const std::string& s = child.first.data();
			const ptree& pt = child.second;
			if(s == "material") {
				utils::str_str_pair tab;
				if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.id")) {
					tab.first = s.get();
				} else {
					// 'id'が無いのはエラー
					++error_;
					break;
				}
				BOOST_FOREACH(const ptree::value_type& child, pt) {
					const std::string& s = child.first.data();
					if(s == "instance_effect") {
						if(boost::optional<string> s = child.second.get_optional<string>("<xmlattr>.url")) {
							tab.second = s.get();
						}
					}
				}

				if(v()) {
					v.nest_out();
					cout << boost::format("material: id: '%s', url: '%s'") % tab.first % tab.second << endl;
				}

				material_map_.insert(tab);
			}
		}
		v.nest_up();

		return error_;
	}

}
