#pragma once
//=====================================================================//
/*!	@file
	@brief	collada geometry-mesh ファイルの入出力（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "utils/verbose.hpp"
#include "utils/vtx.hpp"
#include "utils/mtx.hpp"

namespace collada {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	collada mesh クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct dae_mesh {

		struct source {
			std::string	id_;
			std::vector<float>		array_;
			int			stride_;
			source() : stride_(0) { }
		};
		typedef std::vector<source>	sourceies;

		struct input {
			struct semantic {
				enum type {
					NONE = -1,
					POSITION,
					VERTEX,
					NORMAL,
					COLOR,
					TEXCOORD,
				};			
			};

			semantic::type	semantic_;
			std::string		source_;
			int				offset_;
			int				set_;
			input() : semantic_(semantic::NONE), offset_(0), set_(0) { }
		};
		typedef std::vector<input>	inputs;

		struct vertice {
			std::string	id_;
			input		input_;
		};
		typedef std::vector<vertice>	vertices;

		struct triangle {
			mtx::fmat4	matrix_;
			std::string	material_;
			inputs		inputs_;
			std::vector<int>	pointer_;
			triangle() { matrix_.identity(); }
		};
		typedef std::vector<triangle>	triangles;

	private:
		sourceies	sourceies_;
		vertices	vertices_;
		triangles	triangles_;

		int		error_;

		bool parse_input_(const boost::property_tree::ptree::value_type& element, input& inp);

		void parse_source_(utils::verbose& v, const boost::property_tree::ptree::value_type& element);
		void parse_vertices_(utils::verbose& v, const boost::property_tree::ptree::value_type& element);
		void parse_triangles_(utils::verbose& v, const boost::property_tree::ptree::value_type& element);
	public:
		dae_mesh() : error_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	パース
			@param[in]	v	verbose のインスタンス
			@param[in]	element	ルート・エレメント
			@return 成功なら「０」
		*/
		//-----------------------------------------------------------------//
		int parse(utils::verbose& v, const boost::property_tree::ptree::value_type& element);


		const triangles& get_triangles() const { return triangles_; }


		const boost::optional<const source&> get_source(const std::string& id) const {
			if(!id.empty()) {
				int ofs = 0;
				if(id[0] == '#') ofs = 1;
				BOOST_FOREACH(const source& src, sourceies_) {
					if(src.id_ == id.substr(ofs)) {
						return boost::optional<const source&>(src);
					} 
				}
			}
			return boost::optional<const source&>();
		}


		const boost::optional<const source&> get_vertex_source(const std::string& id) const {
			if(!id.empty()) {
				int ofs = 0;
				if(id[0] == '#') ofs = 1;
				BOOST_FOREACH(const vertice& ver, vertices_) {
					if(ver.id_ == id.substr(ofs)) {
						return get_source(ver.input_.source_);
					}
				}
			}
			return boost::optional<const source&>();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	最小値、最大値のスキャン
			@param[in]	src	ソース
			@param[out]	min	最小値を受け取る参照
			@param[out]	max	最大値を受け取る参照
		*/
		//-----------------------------------------------------------------//
		static void scan_min_max(const source& src, vtx::fvtx& min, vtx::fvtx& max) {
			vtx::fvtx v(0.0f);
			for(int i = 0; i < src.array_.size(); ++i) {
				int m = i % src.stride_;
				if(m == 0) v.x = src.array_[i];
				else if(m == 1) v.y = src.array_[i];
				else if(m == 2) v.z = src.array_[i];
				if(i == 0) {
					min = max = v;
				} else {
					if(min.x > v.x) min.x = v.x;
					if(min.y > v.y) min.y = v.y;
					if(min.z > v.z) min.z = v.z;
					if(max.x < v.x) max.x = v.x;
					if(max.y < v.y) max.y = v.y;
					if(max.z < v.z) max.z = v.z;
				}
			}
		}

	};
}
