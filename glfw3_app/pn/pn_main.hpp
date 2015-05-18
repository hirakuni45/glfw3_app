#pragma once
//=====================================================================//
/*! @file
	@brief  Perlin Noise メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <thread>
#include <future>
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "img_io/bdf_io.hpp"
#include "gl_fw/glmobj.hpp"

#include "PerlinNoise.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Perrin Noise メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pn_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		load_ctx_;
		gui::widget_filer*		save_ctx_;

		gui::widget_frame*		frame_;
		gui::widget_image*		image_;

		gui::widget_frame*		tools_;
		gui::widget_slider*		octave_;
		gui::widget_slider*		frequency_;
		gui::widget_slider*		gain_;
		gui::widget_list*		pn_menu_;
		gui::widget_button*		load_;
		gui::widget_button*		save_;

		gui::widget_radio*		scale_fit_;
		gui::widget_radio*		scale_1x_;
		gui::widget_radio*		scale_2x_;
		gui::widget_radio*		scale_3x_;
		gui::widget_button*		scale_;

		gui::widget_frame*		info_;
		gui::widget_terminal*	term_;

		gui::widget_dialog*		dialog_;
		gui::widget_dialog*		dialog_yes_no_;
		gui::widget_dialog*		dialog_scale_;

		gl::mobj			mobj_;
		gl::mobj::handle	img_handle_;

		int				dd_id_;
		uint32_t		load_id_;
		uint32_t		save_id_;

		vtx::fpos		image_offset_;

		img::shared_img	src_image_;
		img::img_rgba8	prn_image_;
		img::shared_img	bld_image_;

		std::string		save_file_name_;

		std::future<bool>	image_saver_;

		bool			save_dialog_;

		int				octave_value_;
		float			frequency_value_;
		float			gain_value_;

		void create_texture_();
		void blend_();
		void image_info_(const std::string& file, const img::i_img* img);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		pn_main(utils::director<core>& d) :
			director_(d),
			load_ctx_(0), save_ctx_(0),
			frame_(nullptr), image_(nullptr),
			tools_(nullptr),
			octave_(nullptr), frequency_(nullptr), gain_(nullptr), pn_menu_(nullptr),
			load_(0), save_(0),
			scale_fit_(0), scale_1x_(0), scale_2x_(0), scale_3x_(0),
			scale_(0),
			info_(0), term_(0),
			dialog_(0), dialog_yes_no_(0), dialog_scale_(0),
			img_handle_(0), dd_id_(0), load_id_(0), save_id_(0),
			image_offset_(0.0f), save_dialog_(false),
			octave_value_(8), frequency_value_(8.0f), gain_value_(0.0f)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~pn_main() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override;


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override;


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override;


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override;

	};

}
