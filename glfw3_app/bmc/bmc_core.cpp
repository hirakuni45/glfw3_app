//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "bmc_core.hpp"
#include <boost/foreach.hpp>

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  コマンドライン解析
		@param[in]	ss	コマンド郡
	*/
	//-----------------------------------------------------------------//
	bool bmc_core::analize(const utils::strings& ss)
	{
		bool noerr = true;
		BOOST_FOREACH(const std::string& s, ss) {
			if(s.empty()) continue;
			else if(s[0] == '-') {
				if(s == "-preview") option_.set(option::preview);
				else if(s == "-pre") option_.set(option::preview);
				else if(s == "-true-color") option_.set(option::true_color);
				else if(s == "-inverse") option_.set(option::inverse);
				else if(s == "-bdf") option_.set(option::bdf_type);
				else if(s == "-dither") option_.set(option::dither);
				else if(s == "-no-header") option_.set(option::header);
				else if(s == "-clipx") option_.set(option::clipx);
				else if(s == "-clipy") option_.set(option::clipy);
				else if(s == "-append") option_.set(option::append);
				else if(s == "-verbose") option_.set(option::verbose);
				else noerr = false;
			} else {
				if(option_[option::clipx]) {
//					area.x = area.w;
//					if(Arith((const char *)p, &area.w) != 0) {
//						clipx = false;
//					}
//					if(area.x >= 0 && area.w > 0) clipx = false;
					option_.reset(option::clipx);
				} else if(option_[option::clipy]) {
//					area.y = area.h;
//					if(Arith((const char *)p, &area.h) != 0) {
//						clipy = false;
//					}
//					if(area.y >= 0 && area.h > 0) clipy = false;
					option_.reset(option::clipy);
				} else {
					inp_fname_ = out_fname_;
					out_fname_ = s;
				}
			}
		}

		return noerr;
	}
}
