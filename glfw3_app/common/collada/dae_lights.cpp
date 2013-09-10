//=====================================================================//
/*!	@file
	@brief	collada lights のパーサー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_lights.hpp"
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
	int dae_lights::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		error_ = 0;

		const std::string& s = element.first.data();
		if(s != "library_lights") {
			return error_;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		const ptree& pt = element.second;



		return error_;
	}

}
