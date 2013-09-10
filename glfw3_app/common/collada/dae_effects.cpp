//=====================================================================//
/*!	@file
	@brief	collada library_effects のパーサー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_effects.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace collada {

	using namespace boost::property_tree;
	using namespace std;

	bool dae_effects::parse_surface_(utils::verbose& v,
		const boost::property_tree::ptree::value_type& element, surface& surf)
	{
		const std::string& s = element.first.data();
		if(s != "surface") {
			return false;
		}
		const ptree& pt = element.second;
		if(boost::optional<string> ops = pt.get_optional<string>("<xmlattr>.type")) {
			surf.type_ = ops.get();
		}
		if(v()) {
			v.nest_out();
			cout << s << boost::format(": type: '%s'") % surf.type_ << endl;
		}
		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			const std::string& s = child.first.data();
			const std::string& t = child.second.data();
			bool f = false;
			if(s == "init_from") {
				surf.init_from_ = t;
				f = true;
			} else if(s == "format") {
				if(t == "A8R8G8B8") {
					surf.color_type_ = surface::A8R8G8B8;
					f = true;
				} else if(t == "R8G8B8") {
					surf.color_type_ = surface::R8G8B8;
					f = true;
				}
			}
			if(v() && f) {
				v.nest_out();
				std::cout << boost::format("%s: '%s'") % s % t << std::endl;
			}
		}
		v.nest_up();
		surf.enable_ = true;
		return true;
	}


	bool dae_effects::parse_sampler_(utils::verbose& v,
		const boost::property_tree::ptree::value_type& element, sampler& samp)
	{
		const std::string& s = element.first.data();
		if(s != "sampler2D") {
			return false;
		}
		if(v()) {
			v.nest_out();
			cout << s << boost::format(":") << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			const std::string& s = child.first.data();
			const std::string& t = child.second.data();
			bool f = false;
			if(s == "source") {
				samp.source_ = t;
				f = true;
			} else if(s == "minfilter") {
				if(t == "LINEAR") {
					samp.minfilter_ = sampler::minfilter::LINEAR;
					f = true;
				} else if(t == "NEAREST") {
					samp.minfilter_ = sampler::minfilter::NEAREST;
					f = true;
				} else if(t == "LINEAR_MIPMAP_LINEAR") {
					samp.minfilter_ = sampler::minfilter::LINEAR_MIPMAP_LINEAR;
					f = true;
				} else if(t == "NEAREST_MIPMAP_LINEAR") {
					samp.minfilter_ = sampler::minfilter::NEAREST_MIPMAP_LINEAR;
					f = true;
				} else if(t == "LINEAR_MIPMAP_NEAREST") {
					samp.minfilter_ = sampler::minfilter::LINEAR_MIPMAP_NEAREST;
					f = true;
				} else if(t == "NEAREST_MIPMAP_NEAREST") {
					samp.minfilter_ = sampler::minfilter::NEAREST_MIPMAP_NEAREST;
					f = true;
				}
			} else if(s == "magfilter") {
				if(t == "LINEAR") {
					samp.magfilter_ = sampler::magfilter::LINEAR;
					f = true;
				} else if(t == "NEAREST") {
					samp.magfilter_ = sampler::magfilter::NEAREST;
					f = true;
				}
			}
			if(v() && f) {
				v.nest_out();
				std::cout << boost::format("%s: '%s'") % s % t << std::endl;
			}
		}
		v.nest_up();
		samp.enable_ = true;
		return true;
	}


	bool dae_effects::parse_newparam_(utils::verbose& v,
		const boost::property_tree::ptree::value_type& element, newparam& param)
	{
		const std::string& s = element.first.data();
		if(s != "newparam") {
			return false;
		}
		const ptree& pt = element.second;
		if(boost::optional<string> ops = pt.get_optional<string>("<xmlattr>.sid")) {
			param.sid_ = ops.get();
		}
		if(v()) {
			v.nest_out();
			cout << s << boost::format(": sid: '%s'") % param.sid_ << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			if(parse_surface_(v, child, param.surface_)) continue;
			if(parse_sampler_(v, child, param.sampler_)) continue;
		}
		v.nest_up();

		return true;
	}


	bool dae_effects::parse_technique_(utils::verbose& v,
		const boost::property_tree::ptree::value_type& element, technique& teq)
	{
		const std::string& s = element.first.data();
		if(s != "technique") {
			return false;
		}
		const ptree& pt = element.second;
		if(boost::optional<string> ops = pt.get_optional<string>("<xmlattr>.sid")) {
			teq.sid_ = ops.get();
		}
		if(v()) {
			v.nest_out();
			cout << s << boost::format(": sid: '%s'") % teq.sid_ << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			
		}
		v.nest_up();

		return true;
	}


	bool dae_effects::parse_profile_(utils::verbose& v,
		const boost::property_tree::ptree::value_type& element, profile_COM& procom)
	{
		const std::string& s = element.first.data();
		if(s != "profile_COMMON") {
			return false;
		}
		if(v()) {
			v.nest_out();
			cout << s << ":" << endl;
		}
		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			newparam param;
			if(parse_newparam_(v, child, param)) {
				procom.newparams_.push_back(param);
				continue;
			}
			if(parse_technique_(v, child, procom.technique_)) {
				continue;
			} 
		}
		v.nest_up();

		return true;
	}


	void dae_effects::parse_effect_(utils::verbose& v,
		const boost::property_tree::ptree::value_type& element, effect& eff)
	{
		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			if(parse_profile_(v, child, eff.profile_com_)) continue;
//			if(parse_extra_(v, child, eff.extra_)) continue;
		}
		v.nest_up();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パース
		@return エラー数（「０」なら成功）
	*/
	//-----------------------------------------------------------------//
	int dae_effects::parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element)
	{
		error_ = 0;

		const std::string& s = element.first.data();
		if(s != "library_effects") {
			return error_;
		}
		if(v()) {
			cout << s << ":" << endl;
		}

		v.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			const std::string& s = child.first.data();
			const ptree& pt = child.second;
			if(s == "effect") {
				std::string id;
				if(boost::optional<string> s = pt.get_optional<string>("<xmlattr>.id")) {
					id = s.get();
				} else {
					// 'id'が無いのはエラー
					++error_;
					break;
				}
				if(effect_map_.find(id) == effect_map_.end()) {
					if(v()) {
						v.nest_out();
						cout << boost::format("effect: id: '%s'") % id << endl;
					}
					effect eff;
					parse_effect_(v, child, eff);
					effect_map_.insert(effect_map::value_type(id, eff));
				}
			}
		}
		v.nest_up();

		return error_;
	}

}
