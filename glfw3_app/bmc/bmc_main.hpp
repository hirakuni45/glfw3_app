#pragma once
//=====================================================================//
/*! @file
	@brief  BMC メイン関係
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
#include "widgets/widget_list.hpp"
#include "img_io/bdf_io.hpp"
#include "gl_fw/glmobj.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  BMC メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bmc_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		filer_;

		gui::widget_frame*		src_frame_;
		gui::widget_image*		src_image_;
		gui::widget_frame*		dst_frame_;
		gui::widget_image*		dst_image_;

		gui::widget_frame*		tools_;
///		gui::widget_button*		open_;
		gui::widget_check*		scale_;
		gui::widget_dialog*		dialog_;
		gui::widget_list*		bdf_page_;

		gl::mobj			mobj_;
		gl::mobj::handle	src_handle_;
		gl::mobj::handle	dst_handle_;

///		uint32_t	filer_id_;
		uint32_t	bdf_page_no_;

		vtx::fpos	src_image_offset_;
		vtx::fpos	dst_image_offset_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		bmc_main(utils::director<core>& d) : director_(d),
			filer_(0), src_frame_(0), src_image_(0), dst_frame_(0), dst_image_(0),
											 tools_(0), /* open_(0), */ scale_(0),
			dialog_(0), bdf_page_(0),
											 src_handle_(0), dst_handle_(0), /* filer_id_(0), */ bdf_page_no_(0),
			src_image_offset_(0.0f), dst_image_offset_(0.0f)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~bmc_main() { }


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
