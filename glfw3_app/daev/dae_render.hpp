#pragma once
//=====================================================================//
/*!	@file
	@brief	DAE レンダー・メニュー・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <vector>
#include "main.hpp"
#include "utils/director.hpp"
#include "gl_fw/gl_info.hpp"
#include "gl_fw/glcamera.hpp"
#include "gl_fw/gllight.hpp"
#include "gl_fw/glutils.hpp"
#include "collada/dae_io.hpp"
#include "img_io/img_files.hpp"
#include "physics.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	DAE レンダリング・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dae_render : public utils::i_scene {
		utils::director<core>&	director_;

		collada::dae_io		dae_;

		img::img_files		img_files_;

		gl::camera			camera_;
		gl::light			light_;

	   	vtx::fvtx			model_scale_;
		collada::dae_visual_scenes::skeletons	skeletons_;

		bt::physics			physics_;
		float				physics_time_;

		struct capsule_body {
			vtx::fvtx	org_;
			vtx::fvtx	end_;
			btRigidBody*	body_;
			capsule_body() : body_(0) { }
		};
		typedef std::vector<capsule_body>	capsules;
		capsules	capsules_;

		gui::widget_filer*	filer_;
		uint32_t			file_id_;
		gui::widget_frame*	tools_palette_;
		gui::widget_button*	filer_button_;
		gui::widget*		physics_button_;

		bool	wire_frame_;
		bool	light_enable_;
		bool	grid_enable_;
		bool	model_enable_;
		bool	joint_enable_;
		bool	physics_enable_;

		void service_light_();
		void render_skeleton_(const vtx::fvtx& src, const collada::dae_visual_scenes::skeleton& sk);
		void physics_skeleton_(const vtx::fvtx& s, const collada::dae_visual_scenes::skeleton& sk);
		void create_material_();
		void destroy_material_();
		void open_dae_(const std::string& fn);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		dae_render(utils::director<core>& d) : director_(d),
			model_scale_(10.0f),
			physics_(), physics_time_(0.0f),
			filer_(0), file_id_(0),
			tools_palette_(0), filer_button_(0), physics_button_(0),
			wire_frame_(false), light_enable_(true),
			grid_enable_(true), model_enable_(true), joint_enable_(false),
			physics_enable_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~dae_render() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		 */
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		 */
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		 */
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	カメラを取得
			@return カメラ
		 */
		//-----------------------------------------------------------------//
		const gl::camera& get_camera() const { return camera_; }
	};

}
