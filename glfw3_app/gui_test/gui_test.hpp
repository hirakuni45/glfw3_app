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
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_tree.hpp"

namespace app {

	class gui_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_button*		dialog_open_;
		gui::widget_button*		filer_open_;
		gui::widget_dialog*		dialog_;
		gui::widget_tree*		tree_;
		gui::widget_filer*		filer_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		gui_test(utils::director<core>& d) : director_(d),
			dialog_open_(0), filer_open_(0), dialog_(0), tree_(0), filer_(0)
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
