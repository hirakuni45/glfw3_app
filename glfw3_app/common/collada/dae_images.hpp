#pragma once
//=====================================================================//
/*!	@file
	@brief	collada library_images のパーサー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "utils/string_utils.hpp"

#include "utils/verbose.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada library_images のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dae_images {

		utils::str_str_map	image_map_;

		int		error_;

	public:

		dae_images() : error_(0) { }

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
		utils::optional_str_ref get_from(const std::string& id) {
			if(!id.empty()) {
				utils::str_str_map_cit cit = image_map_.find(id);
				if(cit != image_map_.end()) {
					return utils::optional_str_ref(cit->second);
				}
			}
			return utils::optional_str_ref();
		}

	};

}
