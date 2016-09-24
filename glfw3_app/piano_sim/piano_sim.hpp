//=====================================================================//
/*! @file
	@brief  Piano SIM クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"

namespace app {

	class piano_sim : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_button*		test_ring_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;
		
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano_sim(utils::director<core>& d) : director_(d),
											 terminal_frame_(nullptr), terminal_core_(nullptr) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~piano_sim() { }


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
