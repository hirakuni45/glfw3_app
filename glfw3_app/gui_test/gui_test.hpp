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
#include "widgets/widget_check.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_progress.hpp"
#include "widgets/widget_menu.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_progress.hpp"
#include "widgets/widget_spinbox.hpp"

namespace app {

	class gui_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_button*		dialog_open_;
		gui::widget_button*		filer_open_;
		gui::widget_button*		menu_open_;
		gui::widget_button*		menu_ins_;
		gui::widget_button*		menu_era_;
		gui::widget_check*		check_;
		gui::widget_slider*		slider_;
		gui::widget_progress*	progress_;
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
		gui::widget_spinbox*	spinbox_;

		uint32_t	filer_id_;
		uint32_t	menu_id_;
		int			menu_ins_cnt_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		gui_test(utils::director<core>& d) : director_(d),
			dialog_open_(nullptr), filer_open_(nullptr),
			menu_open_(nullptr), menu_ins_(nullptr), menu_era_(nullptr),
			check_(nullptr),
			slider_(nullptr),
			dialog_(nullptr), image_(nullptr), label_(nullptr), frame_(nullptr), menu_(nullptr),
			tree_frame_(nullptr), tree_core_(nullptr),
			filer_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			spinbox_(nullptr),
			filer_id_(0), menu_id_(0), menu_ins_cnt_(0)
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
