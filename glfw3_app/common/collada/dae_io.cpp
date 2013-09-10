//=====================================================================//
/*!	@file
	@brief	collada DAE ファイルの入出力
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "dae_io.hpp"
#include <boost/format.hpp>
#include <boost/foreach.hpp>

namespace collada {

	using namespace boost::property_tree;
	using namespace std;

	void dae_io::setup_material_(const std::string& name, material& mate)
	{
		const dae_effects::optional_effect_cref& cref = effects_.get_effect(name);
		if(cref) {
			const dae_effects::effect& eff = cref.get();
			const dae_effects::profile_COM& com = eff.profile_com_;
			BOOST_FOREACH(const dae_effects::newparam& param, com.newparams_) {
				if(param.surface_.enable_) {
					mate.surface_ = param.surface_;
				} else if(param.sampler_.enable_) {
					mate.sampler_ = param.sampler_;
				}
			}
		}
	}


	void dae_io::parse_geometry_(const ptree::value_type& element, geometry& gt)
	{
		const std::string& s = element.first.data();
		if(s != "geometry") return;

		const ptree& pt = element.second;
		if(boost::optional<string> id = pt.get_optional<string>("<xmlattr>.id")) {
			gt.id_ = id.get();
		}
		if(boost::optional<string> name = pt.get_optional<string>("<xmlattr>.name")) {
			gt.name_ = name.get();
		}
		if(verbose_()) {
			verbose_.nest_out();
			cout << boost::format("id: '%s', name: '%s'") % gt.id_ % gt.name_ << endl;
		}
		verbose_.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, pt) {
			gt.mesh_.parse(verbose_, child);
		}
		verbose_.nest_up();
	}


	void dae_io::parse_geometries_(const ptree::value_type& element)
	{
		const std::string& s = element.first.data();
		if(s != "library_geometries") return;
		if(verbose_()) {
			cout << s << ":" << endl;
		}
		verbose_.nest_down();
		BOOST_FOREACH(const ptree::value_type& child, element.second) {
			geometry gt;
			parse_geometry_(child, gt);
			geometries_.push_back(gt);
		}
		verbose_.nest_up();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パース
		@param[in]	filename	ファイル名
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool dae_io::parse(const std::string& filename)
	{
		read_xml(filename, pt_);

		// version の確認
		if(boost::optional<string> ver = pt_.get_optional<string>("COLLADA.<xmlattr>.version")) {
			filename_ = filename;
			version_ = ver.get();

			if(verbose_()) {
				std::cout <<
					boost::format("Collada: file: '%s',  version: '%s'") % filename % version_ << endl;
			}

			// 全エントリーを展開
			int error = 0;
			BOOST_FOREACH(const ptree::value_type& child, pt_.get_child("COLLADA")) {
				error += asset_.parse(verbose_, child);
				error += lights_.parse(verbose_, child);
				error += images_.parse(verbose_, child);
				error += materials_.parse(verbose_, child);
				error += effects_.parse(verbose_, child);
				parse_geometries_(child);
				error += controllers_.parse(verbose_, child);
				error += visual_scenes_.parse(verbose_, child);
				error += scene_.parse(verbose_, child);
				if(error) {
					return false;
				}
			}

			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	三角形メッシュを作成する
		@param[out]	tms	三角メッシュ構造体
		@param[in]	scale	スケール
	*/
	//-----------------------------------------------------------------//
	void dae_io::create_triangle_mesh(triangle_meshes& tms, const vtx::fvtx& scale)
	{
		if(geometries_.empty()) return;

		float vsc[4];
		vsc[0] = scale.x;
		vsc[1] = scale.y;
		vsc[2] = scale.z;
		vsc[3] = 1.0f;

		BOOST_FOREACH(const geometry& g, geometries_) {
			const dae_mesh& mesh = g.mesh_;
			BOOST_FOREACH(const dae_mesh::triangle& tri, mesh.get_triangles()) {
				triangle_mesh tm;
				tm.matrix_ = tri.matrix_;
				utils::optional_str_ref ref = materials_.get_from(tri.material_);
				if(ref) {
					setup_material_(ref.get(), tm.material_);
				}
				size_t p_stride = tri.inputs_.size();
				BOOST_FOREACH(const dae_mesh::input& inp, tri.inputs_) {
					if(inp.semantic_ == dae_mesh::input::semantic::VERTEX) {
						boost::optional<const dae_mesh::source&> srcopt = mesh.get_vertex_source(inp.source_);
						if(!srcopt) continue;
						const dae_mesh::source& src = *srcopt;
						tm.vertex_stride_ = src.stride_;
						for(size_t i = inp.offset_; i < tri.pointer_.size(); i += p_stride) {
							int idx = tri.pointer_[i] * src.stride_;
							for(int j = 0; j < tm.vertex_stride_; ++j) {
								tm.vertex_.push_back(src.array_[idx + j] * vsc[j]);
							}
						}
						// 頂点集合から、min、max をスキャン
						dae_mesh::scan_min_max(src, tm.min_, tm.max_);
					} else if(inp.semantic_ == dae_mesh::input::semantic::NORMAL) {
						boost::optional<const dae_mesh::source&> srcopt = mesh.get_source(inp.source_);
						if(!srcopt) continue;
						const dae_mesh::source& src = *srcopt;
						tm.normal_stride_ = src.stride_;
						for(size_t i = inp.offset_; i < tri.pointer_.size(); i += p_stride) {
							int idx = tri.pointer_[i] * src.stride_;
							for(int j = 0; j < tm.normal_stride_; ++j) {
								tm.normal_.push_back(src.array_[idx + j]);
							}
						}
					} else if(inp.semantic_ == dae_mesh::input::semantic::TEXCOORD) {
						boost::optional<const dae_mesh::source&> srcopt = mesh.get_source(inp.source_);
						if(!srcopt) continue;
						const dae_mesh::source& src = *srcopt;
						tm.texcoord_stride_ = src.stride_;
						for(size_t i = inp.offset_; i < tri.pointer_.size(); i += p_stride) {
							int idx = tri.pointer_[i] * src.stride_;
							for(int j = 0; j < tm.texcoord_stride_; ++j) {
								tm.texcoord_.push_back(src.array_[idx + j]);
							}
						}						
					} else if(inp.semantic_ == dae_mesh::input::semantic::COLOR) {
						boost::optional<const dae_mesh::source&> srcopt = mesh.get_source(inp.source_);
						if(!srcopt) continue;
						const dae_mesh::source& src = *srcopt;
						tm.color_stride_ = src.stride_;
						for(size_t i = inp.offset_; i < tri.pointer_.size(); i += p_stride) {
							int idx = tri.pointer_[i] * src.stride_;
							for(int j = 0; j < tm.color_stride_; ++j) {
								tm.color_.push_back(src.array_[idx + j]);
							}
						}						
					}
				}
				tms.push_back(tm);
			}
		}
	}
}

