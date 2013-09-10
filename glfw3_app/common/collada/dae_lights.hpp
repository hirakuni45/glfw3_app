#pragma once
//=====================================================================//
/*!	@file
	@brief	collada library_lights のパーサー（ヘッダー）
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
		@brief	collada scene のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dae_lights {



		int		error_;

	public:

		dae_lights() : error_(0) { }

		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@return エラー数（「０」なら成功）
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);


	};

}
