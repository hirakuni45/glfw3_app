#pragma once
//=====================================================================//
/*!	@file
	@brief	collada visual_scenes のパーサー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "utils/string_utils.hpp"

#include "utils/verbose.hpp"

#include "gl_fw/mtx.hpp"
#include "gl_fw/gl_matrix.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada visual_scenes のパーサー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct dae_visual_scenes {

		struct controller {
			std::string		url_;
			utils::strings	skeletons_;
		};


		struct geometry {
			std::string		url_;
		};


		struct light {
			std::string		url_;
		};


		struct extra {
			std::string		tmp;
		};
		typedef std::vector<extra>	extras;


		struct node;
		typedef std::vector<node>	nodes;
		struct node {
			mtx::fmat4		matrix_;

			enum type {
				NODE,
				JOINT,
			};

			std::string		id_;
			std::string		name_;
			std::string		sid_;
			type			type_;
			controller		controller_;
			geometry		geometry_;
			light			light_;
			nodes			nodes_;
			extras			extras_;
			node() : type_(NODE) { matrix_.identity(); }
		};


		struct visual_scene {
			std::string		id_;
			std::string		name_;
			nodes			nodes_;
			extras			extras_;
		};
		typedef	std::vector<visual_scene>	visual_scenes;

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	スケルトン
		*/
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct skeleton;
		typedef std::vector<skeleton>	skeletons;		
		struct skeleton {
			vtx::fvtx	joint_;
			skeletons	skeletons_;
		};

	private:
		visual_scenes	visual_scenes_;

		int		error_;

		bool parse_controller_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, controller& cont);
		bool parse_geometry_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, geometry& cont);
		bool parse_light_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, light& cont);
		bool parse_node_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, node& nd);
		bool parse_extra_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, extra& ex);
		bool parse_matrix_(utils::verbose& v,
			const boost::property_tree::ptree::value_type& element, mtx::fmat4& mat);

		void parse_visual_scene_(utils::verbose& v, const boost::property_tree::ptree::value_type& element);

		void joint_vertex_(const node& nd, gl::glmatf& glmat, skeleton& sk) const;

	public:

		dae_visual_scenes() : error_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@return エラー数（「０」なら成功）
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);



		void create_skeleton(const std::string& id, skeletons& sks) const;

	};

}
