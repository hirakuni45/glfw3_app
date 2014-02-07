#pragma once
//=====================================================================//
/*! @file
	@brief  img メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "img_io/bdf_io.hpp"
#include "gl_fw/glmobj.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  img メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class img_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		filer_;
		gui::widget_frame*		frame_;
		gui::widget_image*		image_;
		gui::widget_frame*		tools_;
		gui::widget_button*		open_;
		gui::widget_check*		scale_;
		gui::widget_dialog*		dialog_;

		gl::mobj			mobj_;
		gl::mobj::handle	img_handle_;

		uint32_t	filer_id_;

		vtx::fpos	image_offset_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		img_main(utils::director<core>& d) : director_(d),
			filer_(0), frame_(0), image_(0),
			tools_(0), open_(0), scale_(0),
			dialog_(0),
			img_handle_(0), filer_id_(0),
			image_offset_(0.0f)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~img_main() { }


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
