//=====================================================================//
/*!	@file
	@brief	collada scene のパーサー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_scene.hpp"
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
	int dae_scene::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		error_ = 0;

		const std::string& s = element.first.data();
		if(s != "scene") {
			return error_;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		const ptree& pt = element.second;

		if(boost::optional<string> s = pt.get_optional<string>("instance_visual_scene.<xmlattr>.url")) {
			source_ = s.get();
			if(v()) {
				v.nest_down();
				v.nest_out();
				cout << boost::format("visual_scene: '%s'") % source_ << endl;
				v.nest_up();
			}
		} else {
			++error_;
		}

		return error_;
	}

}
