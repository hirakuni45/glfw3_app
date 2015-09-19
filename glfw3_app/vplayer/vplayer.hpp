#pragma once
//=====================================================================//
/*! @file
	@brief  vplayer メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "av/av_decoder.hpp"
#include "gl_fw/gltexfb.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  vplayer メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class vplayer : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		tools_frame_;
		gui::widget_button*		open_file_;
		
		gui::widget_filer*		load_ctx_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		double					video_time_;
		av::decoder				decoder_;
		bool					decode_open_;

		gl::texfb				texfb_;

		void output_term_(const std::string& text);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		vplayer(utils::director<core>& d) :
			director_(d),
			tools_frame_(nullptr), open_file_(nullptr),
			load_ctx_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			video_time_(0.0), decoder_(), decode_open_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~vplayer() { }


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
