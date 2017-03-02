//=====================================================================//
/*! @file
	@brief  logger main クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"

namespace app {

	class logger_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		logger_main(utils::director<core>& d) :
			director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~logger_main() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{	// ターミナルのテスト
				{
					widget::param wp(vtx::irect(10, 10, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(30);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
				}
			}


			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(terminal_frame_) {
				terminal_frame_->load(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;

			wd.update();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render()
		{
			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			sys::preference& pre = director_.at().preference_;

			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}
		}
	};
}
