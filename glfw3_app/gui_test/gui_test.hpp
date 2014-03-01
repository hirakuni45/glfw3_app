//=====================================================================//
/*! @file
	@brief  GUI Test クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_menu.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_terminal.hpp"

namespace app {

	class gui_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_button*		dialog_open_;
		gui::widget_button*		filer_open_;
		gui::widget_button*		menu_open_;
		gui::widget_slider*		slider_;
		gui::widget_dialog*		dialog_;
		gui::widget_image*		image_;
		gui::widget_label*		label_;
		gui::widget_frame*		frame_;
		gui::widget_menu*		menu_;
		gui::widget_frame*		tree_frame_;
		gui::widget_tree*		tree_core_;
		gui::widget_filer*		filer_;
		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		uint32_t	filer_id_;
		uint32_t	menu_id_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		gui_test(utils::director<core>& d) : director_(d),
			dialog_open_(0), filer_open_(0), menu_open_(0),
			slider_(0),
			dialog_(0), image_(0), label_(0), frame_(0), menu_(0),
			tree_frame_(0), tree_core_(0),
			filer_(0),
			terminal_frame_(0), terminal_core_(0),
			filer_id_(0), menu_id_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~gui_test() { }


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
