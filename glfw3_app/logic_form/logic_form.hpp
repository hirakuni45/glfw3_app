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
#include "widgets/widget_text.hpp"
#include "widgets/widget_border.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_view.hpp"

namespace app {

	class logic_form : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		tools_;

		gui::widget_frame*		project_;
		gui::widget_view*		view_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;


		static void render_view_(const vtx::irect& clip)
		{
			glDisable(GL_TEXTURE_2D);
			vtx::srect rect(30, 0, 2, clip.size.y);
			gui::draw_border(rect);

			gui::widget::text_param tp("A", img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::TOP));

			rect.org.x = 0;
			rect.size.x = clip.size.x;
			rect.size.y = 2;
			for(int i = 0; i < 24; ++i) {
				glDisable(GL_TEXTURE_2D);
				if(i > 0) {
					rect.org.y = i * 30;
					gui::draw_border(rect);
				}
			}

			gl::core& core = gl::core::get_instance();
			gl::fonts& fonts = core.at_fonts();
			for(int i = 0; i < 24; ++i) {
				vtx::irect tr(0, i * 30 + 3, 30, 30);
				tp.set_text(std::to_string(i));
				gui::draw_text(tp, tr, clip);
			}
			fonts.restore_matrix();
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		logic_form(utils::director<core>& d) : director_(d),
			tools_(nullptr), project_(nullptr), view_(nullptr),
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
			{   // 描画ビュー
				widget::param wp(vtx::irect(0, 0, 500, 100), project_);
				wp.state_.set(widget::state::CLIP_PARENTS);
				widget_view::param wp_;
				wp_.render_func_ = render_view_;
				view_ = wd.add_widget<widget_view>(wp, wp_);
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
