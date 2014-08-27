//=====================================================================//
/*! @file
	@brief  PMD main クラス
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

namespace app {

	class pmdv_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		filer_;
		uint32_t	filer_id_;

		gui::widget_frame*		tools_;
		gui::widget_button*		fopen_;
		gui::widget_check*		grid_;
		gui::widget_check*		body_;
		gui::widget_check*		bone_;

		gui::widget_frame*		tree_frame_;
		gui::widget_tree*		tree_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_;

		mdf::pmd_io		pmd_io_;
		mdf::pmx_io		pmx_io_;
		bool			pmx_enable_;

		gl::camera		camera_;
		gl::light		light_;
		gl::light::handle	bone_light_;

		void info_pmd_();
		void info_pmx_();

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		pmdv_main(utils::director<core>& d) :
			director_(d),
			filer_(0), filer_id_(0),
			tools_(0), fopen_(0), grid_(0), body_(0), bone_(0),
			tree_frame_(0), tree_(0),
			terminal_frame_(0), terminal_(0),
			pmd_io_(), pmx_io_(), pmx_enable_(false),
			bone_light_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~pmdv_main() { }


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
