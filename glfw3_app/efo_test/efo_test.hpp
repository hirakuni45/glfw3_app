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

#include "utils/fixed_fifo.hpp"
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
//		gui::widget_label*		pos_[2];
		gui::widget_list*		ports_;
		gui::widget_button*		connect_;
		gui::widget_button*		capture_;

		gui::widget_filer*		load_ctx_;

		gui::widget_frame*		wave_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_frame*		view_frame_;
		gui::widget_view*		view_core_;

		typedef view::render_waves<uint16_t, 65536, 2> WAVES;
		WAVES					waves_;

		typedef device::serial_win32 SERIAL;
		SERIAL					serial_;
		SERIAL::name_list		serial_list_;
		typedef utils::fixed_fifo<uint8_t, 8192> FIFO;
		FIFO					fifo_;

		enum class wave_task {
			idle,
			body,
			flush,
		};
		wave_task				wave_task_;
		uint32_t				wave_ch_;
		uint32_t				wave_max_;
		uint32_t				wave_pos_;
		uint16_t				wave_ch0_[1024];
		uint16_t				wave_ch1_[1024];

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
///			gl::glColor(wd.get_color());
			auto pos = div_->get_menu()->get_select_pos();
//			auto div = div_tbls_[pos];
			waves_.render(clip.size, 65536 * (pos + 1));
			glEnable(GL_TEXTURE_2D);
		}


		void service_waves_()
		{
			uint8_t tmp[256];
			auto len = serial_.read(tmp, sizeof(tmp));
			if(len > 0) {
				for(uint32_t i = 0; i < len; ++i) {
					fifo_.put(tmp[i]);
				}
			}

			switch(wave_task_) {
			case wave_task::idle:
				wave_max_ = 0;
				if(fifo_.length() >= 4) {
					auto cmd = fifo_.get();
					auto ch = fifo_.get();
					auto len_h = fifo_.get();
					auto len_l = fifo_.get();
					if(cmd == 0x02) {
						wave_ch_ = ch;
						wave_max_ = (static_cast<uint32_t>(len_h) << 8) | len_l;
						wave_pos_ = 0;
						wave_task_ = wave_task::body;
					} else {
						wave_task_ = wave_task::flush;
					}
				}
				break;
			case wave_task::body:
				if(wave_pos_ < wave_max_) {
					auto n = fifo_.length();
					if(n & 1) n &= 0xfffffffe;  // 偶数バイトにする
					uint16_t* org;
					if(wave_ch_ == 1) {
						org = wave_ch0_;
					} else if(wave_ch_ == 2) {
						org = wave_ch1_;
					} else {
						wave_task_ = wave_task::flush;
						break;
					}
					n >>= 1;
					for(uint32_t i = 0; i < n; ++i) {
						uint16_t w = fifo_.get();
						w <<= 8;
						w |= fifo_.get();
						org[wave_pos_] = w;
						++wave_pos_;
						if(wave_pos_ >= wave_max_) {
							auto s = (boost::format("Wave (ch%d): %d\n")
								% wave_ch_ % wave_max_).str();
							terminal_core_->output(s);
							waves_.copy(0, wave_ch0_, 1024);
							waves_.copy(1, wave_ch1_, 1024);
							wave_task_ = wave_task::idle;
							break;
						}						
					}
				} else {
					wave_task_ = wave_task::idle;
				}
				break;
			case wave_task::flush:
				fifo_.clear();
				wave_task_ = wave_task::idle;
				break;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		efo_test(utils::director<core>& d) : director_(d),
			menu_(nullptr), load_(nullptr), div_(nullptr),
			ports_(nullptr), connect_(nullptr), capture_(nullptr),
			load_ctx_(nullptr),
			wave_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			view_frame_(nullptr), view_core_(nullptr),
			waves_(), serial_(), serial_list_(), fifo_(), wave_task_(wave_task::idle),
			wave_ch_(0), wave_max_(0), wave_pos_(0), wave_ch0_{ 0 }, wave_ch1_{ 0 }
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
			int menu_height = 400;
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
				wp_.select_func_ = [=](int id) {
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
//				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
//					utils::format("List Selected: '%s', (%d)\n") % text.c_str() % pos;
//				};
				div_ = wd.add_widget<widget_list>(wp, wp_);
			}


			{ // Serial PORT select
				widget::param wp(vtx::irect(10, 20+40*6, menu_width - 20, 30), menu_);
				widget_list::param wp_;
				ports_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{ // コネクションボタン
				widget::param wp(vtx::irect(10, 20+40*7, menu_width - 20, 30), menu_);
				widget_button::param wp_("connect");
				connect_ = wd.add_widget<widget_button>(wp, wp_);
				connect_->at_local_param().select_func_ = [=](int id) {
					const auto& port = ports_->get_select_text();
					if(!port.empty()) {
						if(serial_.open(port)) {
							terminal_core_->output("Open Serialport: '" + port + "'\n");
						} else {
							terminal_core_->output("Can't open Serialport: '" + port + "'\n"); 
						}
					}
				};
			}
			{ // キャプチャー・ボタン
				widget::param wp(vtx::irect(10, 20+40*8, menu_width - 20, 30), menu_);
				widget_button::param wp_("capture");
				capture_ = wd.add_widget<widget_button>(wp, wp_);
				capture_->at_local_param().select_func_ = [=](int fd) {
					char tmp[1];
					tmp[0] = 'B';
					serial_.write(tmp, sizeof(tmp));
				};
			}




			{ // load ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				wp_.select_file_func_ = [=](const std::string& path) {
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
					wp_.enter_func_ = [=](const utils::lstring& text) {
						term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					term_chaout::set_output(terminal_core_);
				}

				// ロジック編集クラスの出力先の設定
//				project_.logic_edit_.set_output([=](const std::string& s) {
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
					wp_.update_func_ = [=]() {
						update_view_();
					};
					wp_.render_func_ = [=](const vtx::irect& clip) {
						render_view_(clip);
					};
					wp_.service_func_ = [=]() {
///						service_view_();
					};
					view_core_ = wd.add_widget<widget_view>(wp, wp_);
				}
			}


			// プリファレンスのロード
			sys::preference& pre = director_.at().preference_;

			if(menu_ != nullptr) {
				menu_->load(pre, false, false);
			}
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
			if(ports_ != nullptr) ports_->load(pre);
			if(div_ != nullptr) div_->load(pre);

			// テスト波形生成
			waves_.create_buffer(0.5, 10e-6);
///			waves_.build_sin(10e3);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;

			// シリアルポートの更新
			if(serial_list_.empty() || !serial_.compare(serial_list_)) {
				serial_.create_list();
				serial_list_ = serial_.get_list();
				utils::strings list;
				for(const auto& t : serial_list_) {
					list.push_back(t.port);
					terminal_core_->output(t.port + " (" + t.info + ")\n");
				}
				ports_->get_menu()->build(list);
				ports_->select(0);
			}

			waves_.at_param(0).gain_ = 0.025f;
			waves_.at_param(1).gain_ = 0.025f;
			waves_.at_param(0).color_ = img::rgba8(255, 128, 255, 255);
			waves_.at_param(1).color_ = img::rgba8(128, 255, 255, 255);
			waves_.at_param(0).offset_ = 0;
			waves_.at_param(1).offset_ = 200;

			service_waves_();
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
			if(ports_ != nullptr) ports_->save(pre);

			if(load_ctx_ != nullptr) load_ctx_->save(pre);
///			if(save_ctx_ != nullptr) save_ctx_->save(pre);
			if(view_frame_ != nullptr) {
				view_frame_->save(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}

			if(menu_ != nullptr) {
				menu_->save(pre);
			}
		}
	};
}
