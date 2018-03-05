#pragma once
//=====================================================================//
/*! @file
    @brief  ツールス・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "widgets/widget_chip.hpp"

#include "interlock.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ツールス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct tools {

		static std::string limitf(const std::string& str, float min, float max, const char* form)
		{
			std::string newtext;
			float v;
			if((utils::input("%f", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}


		static std::string limiti(const std::string& str, int min, int max, const char* form)
		{
			std::string newtext;
			int v;
			if((utils::input("%d", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}


		static void set_help(gui::widget_chip* chip, gui::widget* src, const std::string& text)
		{
			chip->set_offset(src);
			chip->set_text(text);
		}


		static interlock::module get_module(int swn)
		{
			interlock::module md = interlock::module::N;
			switch(swn) {
			case 1:
				md = interlock::module::CRM;
				break;
			case 15:
				md = interlock::module::DC2;
				break;
			case 29:
				md = interlock::module::WDM;
				break;
			case 34:
				md = interlock::module::ICM;
				break;
			case 40:
				md = interlock::module::DC1;
				break;
			case 44:
				md = interlock::module::WGM;
				break;
			default:
				break;
			}
			return md;
		}


		static void init_sw(gui::widget_director& wd, gui::widget* root, interlock& ilc,
			int ofsx, int ofsy, gui::widget_check* out[], int num, int swn)
		{
			auto md = get_module(swn);
			for(int i = 0; i < num; ++i) {
				gui::widget::param wp(vtx::irect(ofsx, ofsy, 60, 40), root);
				gui::widget_check::param wp_((boost::format("%d") % swn).str());
				out[i] = wd.add_widget<gui::widget_check>(wp, wp_);
				ofsx += 60;
				ilc.install(md, static_cast<interlock::swtype>(swn), out[i]);
				++swn;
			}
		}


		static void load_sw(sys::preference& pre, gui::widget_check* sw[], uint32_t n)
		{
			for(uint32_t i = 0; i < n; ++i) {
				sw[i]->load(pre);
			}
		}


		static void save_sw(sys::preference& pre, gui::widget_check* sw[], uint32_t n)
		{
			for(uint32_t i = 0; i < n; ++i) {
				sw[i]->save(pre);
			}
		}


		static std::string double_to_str(double a)
		{
			std::string str;
			if(a >= 0.001) {
				str = (boost::format("%4.3f") % a).str(); 
			} else {
				str = (boost::format("%e") % a).str();
			}
			return str;
		}
	};
}
