#pragma once
//=====================================================================//
/*!	@file
	@brief	collada DAE ファイルの入出力（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "utils/verbose.hpp"
#include "dae_asset.hpp"
#include "dae_lights.hpp"
#include "dae_images.hpp"
#include "dae_materials.hpp"
#include "dae_effects.hpp"
#include "dae_mesh.hpp"
#include "dae_controllers.hpp"
#include "dae_visual_scenes.hpp"
#include "dae_scene.hpp"

#include "gl_fw/vtx.hpp"
#include "gl_fw/mtx.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada ファイル In/Out クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct dae_io {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	geometry 構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct geometry {
			std::string	id_;
			std::string	name_;
			dae_mesh	mesh_;
		};
		typedef std::vector<geometry>	geometries;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	マテリアル構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct material {
			unsigned int			tex_id_;
			dae_effects::surface	surface_;
			dae_effects::sampler	sampler_;
			material() : tex_id_(0) { }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	三角形メッシュ構造体（簡易レンダリング用）
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct triangle_mesh {
			mtx::fmat4			matrix_;
			material			material_;
			vtx::fvtx			min_;
			vtx::fvtx			max_;
			size_t				vertex_stride_;
			std::vector<float>	vertex_;
			size_t				normal_stride_;
			std::vector<float>	normal_;
			size_t				texcoord_stride_;
			std::vector<float>	texcoord_;
			size_t				color_stride_;
			std::vector<float>	color_;
			triangle_mesh() : min_(0.0f), max_(0.0f),
							  vertex_stride_(0), texcoord_stride_(0), color_stride_(0) { matrix_.identity(); }
			void list(const std::string& tab = "") const {
				std::cout << tab << boost::format("vertex: %d, (stride: %d)")
						% vertex_.size() % vertex_stride_ << std::endl; 
				std::cout << tab << "  " <<
					boost::format("normal: %d, (stride: %d)")
						% normal_.size() % normal_stride_ << std::endl; 
				std::cout << tab << "  " <<
					boost::format("texcoord: %d, (stride: %d)")
						% texcoord_.size() % texcoord_stride_ << std::endl; 
				std::cout << tab << "  " <<
					boost::format("color: %d, (stride: %d)")
						% color_.size() % color_stride_ << std::endl; 
			}
		};
		typedef std::vector<triangle_mesh>	triangle_meshes;

	private:
		std::string		filename_;
		std::string		version_;

		boost::property_tree::ptree	pt_;

		dae_asset			asset_;
		dae_lights			lights_;
		dae_images			images_;
		dae_effects			effects_;
		dae_materials		materials_;
		geometries			geometries_;
		dae_controllers		controllers_;
		dae_visual_scenes	visual_scenes_;
		dae_scene			scene_;

		utils::verbose		verbose_;

		void setup_material_(const std::string& name, material& mate);
		void parse_geometry_(const boost::property_tree::ptree::value_type& ele, geometry& gt);
		void parse_geometries_(const boost::property_tree::ptree::value_type& element);

	public:
		dae_io() { verbose_.set_level(utils::verbose::level::none); }


		utils::verbose& at_verbose() { return verbose_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@param[in]	filename	ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool parse(const std::string& filename);


		//-----------------------------------------------------------------//
		/*!
			@brief	三角形メッシュを作成する
			@param[out]	tms	三角メッシュ構造体
			@param[in]	scale	スケール
		*/
		//-----------------------------------------------------------------//
		void create_triangle_mesh(triangle_meshes& tms, const vtx::fvtx& scale = vtx::fvtx(1.0f));


		//-----------------------------------------------------------------//
		/*!
			@brief	dae_scene を取得する
			@return dae_scene のインスタンス
		*/
		//-----------------------------------------------------------------//
		const dae_scene& get_scene() const { return scene_; }


		const dae_visual_scenes& get_visual_scenes() const { return visual_scenes_; }


		void destroy() {
			filename_.clear();
			version_.clear();
			pt_.clear();
		}

	};
}
