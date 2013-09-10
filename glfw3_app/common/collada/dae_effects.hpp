#pragma once
//=====================================================================//
/*!	@file
	@brief	collada library_effects のパーサー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>

#include "utils/verbose.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada library_effects のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct dae_effects {

		struct surface {
			enum color_type {
				R8G8B8,
				A8R8G8B8,
			};
			std::string		type_;
			std::string		init_from_;
			color_type		color_type_;
			bool			enable_;
			surface() : enable_(false) { }
		};

		struct sampler {
			std::string		source_;
			struct minfilter {
				enum type {
    				NEAREST_MIPMAP_NEAREST,
					LINEAR_MIPMAP_NEAREST,
					NEAREST_MIPMAP_LINEAR,
					LINEAR_MIPMAP_LINEAR,
					NEAREST,
					LINEAR,
				};
			};
			minfilter::type	minfilter_;
			struct magfilter {
				enum type {
					LINEAR,
					NEAREST,
				};
			};
			magfilter::type	magfilter_;
			bool			enable_;
			sampler() : enable_(false) { }
		};

		struct newparam {
			std::string		sid_;
			surface			surface_;
			sampler			sampler_;
		};
		typedef std::vector<newparam>	newparams;

		struct technique {
			std::string		sid_;

		};

		struct profile_COM {
			newparams		newparams_;
			technique		technique_;
		};

		struct effect {
			profile_COM		profile_com_;
		};

		typedef boost::optional<effect&>		optional_effect_ref;
		typedef boost::optional<const effect&>	optional_effect_cref;

	private:

		typedef boost::unordered_map<std::string, effect> effect_map;
		typedef effect_map::const_iterator		effect_map_cit;
		effect_map		effect_map_;

		int				error_;

		bool parse_surface_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, surface& surf);
		bool parse_sampler_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, sampler& samp);
		bool parse_newparam_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, newparam& param);
		bool parse_technique_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, technique& teq);
		bool parse_profile_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, profile_COM& procom);
		void parse_effect_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, effect& eff);

	public:

		dae_effects() : error_(0) { }

		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@return エラー数（「０」なら成功）
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);


		//-----------------------------------------------------------------//
		/*!
			@brief	id から effect を取得
			@param[in]	id	id 文字列
			@return effect
		*/
		//-----------------------------------------------------------------//
		optional_effect_cref get_effect(const std::string& id) const {
			if(!id.empty()) {
				std::string s;
				if(id[0] == '#') s = id.substr(1);
				else s = id;
				effect_map_cit cit = effect_map_.find(s);
				if(cit != effect_map_.end()) {
					return optional_effect_cref(cit->second);
				}
			}
			return optional_effect_cref();
		}
	};

}
