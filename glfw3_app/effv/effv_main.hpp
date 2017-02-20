//=====================================================================//
/*! @file
	@brief  Effectseer main クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_terminal.hpp"
#include "mdf/pmd_io.hpp"
#include "mdf/pmx_io.hpp"
#include "gl_fw/glcamera.hpp"
#include "gl_fw/gllight.hpp"

#include "effekseer/gl/Renderer.h"

namespace app {

	class effv_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		filer_;
		uint32_t	filer_id_;

		gui::widget_frame*		tools_;
		gui::widget_button*		fopen_;
		gui::widget_check*		grid_;

		gui::widget_button*		play_;
		gui::widget_check*		loop_;

		gl::camera		camera_;

		::Effekseer::Manager*			manager_;
		::EffekseerRenderer::Renderer*	renderer_;
		::Effekseer::Effect*			effect_;

		::Effekseer::Vector3D		position_;
		::Effekseer::Handle			handle_;

		void init_effekseer_(const vtx::spos& size);
		void destroy_effekseer_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		effv_main(utils::director<core>& d) :
			director_(d),
			filer_(0), filer_id_(0),
			tools_(nullptr), fopen_(nullptr), grid_(nullptr),
			play_(nullptr), loop_(nullptr),
			camera_(),
			manager_(nullptr), renderer_(nullptr), effect_(nullptr),
			handle_(-1)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~effv_main() { destroy_effekseer_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
