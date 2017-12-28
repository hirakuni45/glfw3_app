#pragma once
//=====================================================================//
/*! @file
    @brief  EFO Test・アプリケーション・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "main.hpp"
#include "core/glcore.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_utils.hpp"

#include "gl_fw/render_waves.hpp"

#include "utils/serial_win32.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor アプリケーション・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class efo_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		menu_;
		gui::widget_button*		load_;
		gui::widget_list*		div_;

		gui::widget_filer*		load_ctx_;

		gui::widget_frame*		wave_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_frame*		view_frame_;
		gui::widget_view*		view_core_;

		typedef view::render_waves<uint16_t, 65536 * 4> WAVES;
		WAVES					waves_;

		// ターミナル、行入力
		void term_enter_(const utils::lstring& text) {
			auto s = utils::utf32_to_utf8(text);
//			project_.logic_edit_.command(s);
///			std::cout << s << std::endl;
		}

		// 波形描画
		void update_view_()
		{
		}


		void render_view_(const vtx::irect& clip)
		{
			gui::widget_director& wd = director_.at().widget_director_;

			glDisable(GL_TEXTURE_2D);

			gl::glColor(wd.get_color());

			auto pos = div_->get_menu()->get_select_pos();
//			auto div = div_tbls_[pos];
			waves_.render(clip.size.x, 65536 * (pos + 1));

			glEnable(GL_TEXTURE_2D);
		}


		void service_view_()
		{
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		efo_test(utils::director<core>& d) : director_(d),
			menu_(nullptr), load_(nullptr), load_ctx_(nullptr),
			wave_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			view_frame_(nullptr), view_core_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			gl::core& core = gl::core::get_instance();

			int menu_width  = 130;
			int menu_height = 300;
			{	// メニューパレット
				widget::param wp(vtx::irect(10, 10, menu_width, menu_height));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				menu_ = wd.add_widget<widget_frame>(wp, wp_);
				menu_->set_state(gui::widget::state::SIZE_LOCK);
			}

			{ // ロード起動ボタン
				widget::param wp(vtx::irect(10, 20+40*0, menu_width - 20, 30), menu_);
				widget_button::param wp_("load");
				wp_.select_func_ = [this](int id) {
//					gui::get_open_file_name();
					if(load_ctx_) {
//						script_on_ = false;
						bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
						load_ctx_->enable(!f);
//						save_->set_stall(!f);
//						export_->set_stall(!f);
//						script_->set_stall(!f);
					}
				};
				load_ = wd.add_widget<widget_button>(wp, wp_);
			}

			{ // DIV select
				widget::param wp(vtx::irect(10, 20+40*1, menu_width - 20, 30), menu_);
				widget_list::param wp_("1000 ms");
				wp_.init_list_.push_back("1000 ms");
				wp_.init_list_.push_back("500 ms");
				wp_.init_list_.push_back("250 ms");
				wp_.init_list_.push_back("100 ms");
				wp_.init_list_.push_back("50 ms");
				wp_.init_list_.push_back("10 ms");
				wp_.init_list_.push_back("5 ms");
				wp_.init_list_.push_back("1 ms");
				wp_.init_list_.push_back("500 us");
				wp_.init_list_.push_back("100 us");
				wp_.init_list_.push_back("50 us");
				wp_.init_list_.push_back("25 us");
				wp_.init_list_.push_back("10 us");
				wp_.init_list_.push_back("5 us");
				wp_.init_list_.push_back("1 us");
//				wp_.select_func_ = [this](const std::string& text, uint32_t pos) {
//					utils::format("List Selected: '%s', (%d)\n") % text.c_str() % pos;
//				};
				div_ = wd.add_widget<widget_list>(wp, wp_);
			}


			{ // load ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				wp_.select_file_func_ = [this](const std::string& path) {
#if 0
					bool f = false;
					if(script_on_) {
						f = project_.logic_edit_.injection(path);
					} else {
						f = project_.logic_.load(path);
					}
					load_->set_stall(false);
					save_->set_stall(false);
					export_->set_stall(false);
					script_->set_stall(false);
					if(!f) {  // load error
						if(script_on_) dialog_->set_text("Script error:\n" + path);
						else dialog_->set_text("Load error:\n" + path);
						dialog_->enable();
					}
#endif
				};
				load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				load_ctx_->enable(false);
			}


			{	// ターミナル
				{
					widget::param wp(vtx::irect(20, 100, 100, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [this](const utils::lstring& text) {
						term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					term_chaout::set_output(terminal_core_);
				}

				// ロジック編集クラスの出力先の設定
//				project_.logic_edit_.set_output([this](const std::string& s) {
//					terminal_core_->output(s);
//				}
//				);
			}

			{	// 波形ビュー
				{
					widget::param wp(vtx::irect(40, 150, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
					view_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0, 50, 0, 0), view_frame_);
					widget_view::param wp_;
					wp_.update_func_ = [this]() {
						update_view_();
					};
					wp_.render_func_ = [this](const vtx::irect& clip) {
						render_view_(clip);
					};
					wp_.service_func_ = [this]() {
						service_view_();
					};
					view_core_ = wd.add_widget<widget_view>(wp, wp_);
				}
			}


			// プリファレンスのロード
			sys::preference& pre = director_.at().preference_;

			if(menu_ != nullptr) {
				menu_->load(pre, false, false);
			}

///			project_.load(pre);

			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}

			if(view_frame_ != nullptr) {
				view_frame_->load(pre);
			}

//			if(argv_frame_ != nullptr) {
//				argv_frame_->load(pre);
//			}

			if(load_ctx_ != nullptr) load_ctx_->load(pre);
///			if(save_ctx_ != nullptr) save_ctx_->load(pre);

			if(div_ != nullptr) div_->load(pre);

			// テスト波形生成
			waves_.create_buffer(0.5, 10e-6);
			waves_.build_sin(10e3);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;
#if 0
			// Drag & Drop されたファイル
			gl::core& core = gl::core::get_instance();
			int id = core.get_recv_files_id();
			if(drop_file_id_ != id) {
				drop_file_id_ = id;
				const utils::strings& ss = core.get_recv_files_path();
				if(!ss.empty()) {
					std::string path = ss[0];
					if(load_ctx_ != nullptr && load_ctx_->get_local_param().select_file_func_ != nullptr) {
						load_ctx_->get_local_param().select_file_func_(path);
					}
				}
			}
#endif
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

			if(div_ != nullptr) div_->save(pre);

			if(load_ctx_ != nullptr) load_ctx_->save(pre);
///			if(save_ctx_ != nullptr) save_ctx_->save(pre);

//			if(argv_frame_ != nullptr) {
//				argv_frame_->save(pre);
//			}

			if(view_frame_ != nullptr) {
				view_frame_->save(pre);
			}

			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}

//			project_.save(pre);

			if(menu_ != nullptr) {
				menu_->save(pre);
			}
		}
	};
}
