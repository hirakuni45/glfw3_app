//=====================================================================//
/*!	@file
	@brief	collada asset のパーサー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_asset.hpp"
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
	int dae_asset::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "asset") {
			return 0;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		error_ = 0;

		created_.clear();
		modified_.clear();
		unit_meter_ = 0.0f;
		up_axis_ = up_axis::none;

		const ptree& pt = element.second;
		if(boost::optional<string> s = pt.get_optional<string>("created")) {
			created_ = s.get();
		}
		if(boost::optional<string> s = pt.get_optional<string>("modified")) {
			modified_ = s.get();
		}
		if(boost::optional<string> s = pt.get_optional<string>("unit.<xmlattr>.meter")) {
			utils::int_vector iv;
			if(utils::string_to_int(s.get(), iv) && !iv.empty()) {
				unit_meter_ = iv[0];
			}
		}
		if(boost::optional<string> s = pt.get_optional<string>("up_axis")) {
			if(s.get() == "X_UP") up_axis_ = up_axis::X_UP;
			else if(s.get() == "Y_UP") up_axis_ = up_axis::Y_UP;
			else if(s.get() == "Z_UP") up_axis_ = up_axis::Z_UP;
			else if(s.get() == "X_DOWN") up_axis_ = up_axis::X_DOWN;
			else if(s.get() == "Y_DOWN") up_axis_ = up_axis::Y_DOWN;
			else if(s.get() == "Z_DOWN") up_axis_ = up_axis::Z_DOWN;
			else up_axis::none;
		}

		if(v()) {
			v.nest_down();
			const string& nest = v.get_nest();
			cout << nest << boost::format("created: '%s'") % created_ << endl;
			cout << nest << boost::format("modified: '%s'") % modified_ << endl;
			cout << nest <<boost::format("unit_meter: %f [m]") % unit_meter_ << endl;
			string f;
			if(up_axis_ == up_axis::X_UP) f = "X UP";
			else if(up_axis_ == up_axis::Y_UP) f = "Y UP";
			else if(up_axis_ == up_axis::Z_UP) f = "Z UP";
			else if(up_axis_ == up_axis::X_DOWN) f = "X DOWN";
			else if(up_axis_ == up_axis::Y_DOWN) f = "Y DOWN";
			else if(up_axis_ == up_axis::Z_DOWN) f = "Z DOWN";
			cout << nest << "up_axis: " << f << endl;
			v.nest_up();
		}

		return error_;
	}

}
