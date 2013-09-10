#pragma once
//=====================================================================//
/*!	@file
	@brief	collada library_materials のパーサー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "utils/string_utils.hpp"

#include "utils/verbose.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada library_materials のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dae_materials {

		utils::str_str_undmap	material_map_;

		int		error_;

	public:

		dae_materials() : error_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@return エラー数（「０」なら成功）
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);


		//-----------------------------------------------------------------//
		/*!
			@brief	id から from を取得
			@param[in]	id	id 文字列
			@return 
		*/
		//-----------------------------------------------------------------//
		const utils::optional_str_ref get_from(const std::string& id) const {
			if(!id.empty()) {
				utils::str_str_undmap_cit cit = material_map_.find(id);
				if(cit != material_map_.end()) {
					return utils::optional_str_ref(cit->second);
				}
			}
			return utils::optional_str_ref();
		}

	};

}
