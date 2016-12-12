//=====================================================================//
/*! @file
	@brief  Logic FORM クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_border.hpp"
#include "widgets/widget_terminal.hpp"

namespace app {

	class logic_form : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		tools_;

		gui::widget_frame*		project_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		logic_form(utils::director<core>& d) : director_(d),
			tools_(nullptr), project_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~logic_form() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
	//		gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{	// ツールパレット
				widget::param wp(vtx::irect(20, 20, 200, 300));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}

			{	// プロジェクト・フレーム
				widget::param wp(vtx::irect(20, 50, 500, 100));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				project_ = wd.add_widget<widget_frame>(wp, wp_);
			}

			{   // ボーダーＨ
				widget::param wp(vtx::irect(0, 50, 0, 2), project_);
				widget_border::param wp_(widget_border::param::type::holizontal);
				wd.add_widget<widget_border>(wp, wp_);
			}

			{   // ボーダーＶ
				widget::param wp(vtx::irect(50, 0, 2, 0), project_);
				widget_border::param wp_(widget_border::param::type::vertical);
				wd.add_widget<widget_border>(wp, wp_);
			}


			{	// ターミナルのテスト
				{
					widget::param wp(vtx::irect(20, 100, 100, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
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

			if(tools_ != nullptr) {
				tools_->load(pre, false, false);
			}

			if(project_ != nullptr) {
				project_->load(pre);
			}

			if(terminal_frame_ != nullptr) {
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

			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}

			if(project_ != nullptr) {
				project_->save(pre);
			}

			if(tools_ != nullptr) {
				tools_->save(pre);
			}
		}

	};

}
