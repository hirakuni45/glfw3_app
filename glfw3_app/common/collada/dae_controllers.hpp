#pragma once
//=====================================================================//
/*!	@file
	@brief	collada library_controllers のパーサー（ヘッダー）
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
		@brief	collada library_controllers のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct dae_controllers {

		struct vertex_weights {

		};

		struct joints {

		};

		struct source {
			utils::strings		name_array_;
			std::vector<float>	float_array_;
		};
		typedef std::vector<source>	sources;

		struct skin {
			std::string		source_;
			mtx::fmat4		bind_shape_matrix_;
			sources			sources_;
			joints			joints_;
			vertex_weights	vertex_weights_;
		};

		struct controller {
			std::string		id_;
			skin			skin_;
		};
		typedef std::vector<controller>	controllers;

	private:

		controllers		controllers_;

		int		error_;

		void parse_controller_(utils::verbose& v, const boost::property_tree::ptree::value_type& element);
	public:

		dae_controllers() : 
			error_(0) { }

		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@return エラー数（「０」なら成功）
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);


	};

}
