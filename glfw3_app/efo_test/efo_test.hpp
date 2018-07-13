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
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_utils.hpp"

#include "gl_fw/render_waves.hpp"

#include "utils/fixed_fifo.hpp"
#include "utils/serial_win32.hpp"
#include "utils/file_io.hpp"
#include "utils/input.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  Ignitor アプリケーション・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class efo_test : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		menu_;
		gui::widget_list*		ports_;
		gui::widget_button*		connect_;

		gui::widget_list*		div_;
		gui::widget_spinbox*	ch1_pos_;
		gui::widget_spinbox*	ch2_pos_;
		gui::widget_spinbox*	ch1_gain_;
		gui::widget_spinbox*	ch2_gain_;

		gui::widget_button*		single_;
		gui::widget_button*		autotest_;
		gui::widget_label*		autotest_min_;
		gui::widget_label*		autotest_max_;
		gui::widget_label*		autotest_result_;

		gui::widget_button*		capture_;
		gui::widget_list*		slope_;
		gui::widget_label*		level_;
		gui::widget_button*		trigger_;

		gui::widget_frame*		wave_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_frame*		view_frame_;
		gui::widget_view*		view_core_;

		typedef view::render_waves<uint16_t, 1024, 2> WAVES;
		WAVES					waves_;

		typedef device::serial_win32 SERIAL;
		SERIAL					serial_;
		SERIAL::name_list		serial_list_;
		typedef utils::fixed_fifo<uint8_t, 8192> FIFO;
		FIFO					fifo_;

		bool					autotest_enable_;
		uint32_t				autotest_count_;
		utils::file_io			autotest_fp_;
		uint32_t				autotest_fail_;

		double get_div_rate_()
		{
			if(div_ == nullptr) return 0.0;

			static const double tbl[] = {
				500e-3,
				250e-3,
				100e-3,
				50e-3,
				25e-3,
				10e-3,
				5e-3,
				1e-3,
				500e-6,
				250e-6,
				100e-6,
				50e-6,
				25e-6,
				10e-6,
				5e-6,
				1e-6,
				500e-9,
				250e-9,
				100e-9,
				50e-9,
				25e-9,
				10e-9 };
			return tbl[div_->get_select_pos()];
		}


		enum class wave_task {
			idle,
			multi_body,
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
			auto pos = div_->get_menu()->get_select_pos();
//			auto div = div_tbls_[pos];
//			waves_.render(clip.size, 65536 * (pos + 1));
			waves_.render(clip.size, 1e-6, get_div_rate_());

			glEnable(GL_TEXTURE_2D);
		}


		bool get_float_(const std::string& str, float& ans) {
			float a;
			auto f = (utils::input("%f", str.c_str()) % a).status();
			if(f) {
				ans = a;
			}
			return f;
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
					if(cmd == 0x21) {  // Version
						auto d = fifo_.get();
						auto c = fifo_.get();
						auto b = fifo_.get();
						auto a = fifo_.get();
						auto s = (boost::format("Voltage: %d, Version %c%c.%c%c\n")
							% static_cast<int>(ch) % a % b % c % d).str();
						terminal_core_->output(s);
					} else if(cmd == 0x01) {  // SINGLE data
						uint16_t w = fifo_.get();
						w <<= 8;
						w |= fifo_.get();
						float a = static_cast<int16_t>(w);
						a /= 32767.0f;
						a *= 20.48f;
						auto s = (boost::format("%d Value (ch%d): %4.2f [V] (%d)")
							% autotest_count_ % static_cast<int>(ch)
							% a % static_cast<int16_t>(w)).str();
						s += '\n';
						if(!autotest_fp_.is_open()) {
							autotest_fp_.open("autotest.txt", "wb");
						}
						autotest_fp_.write(s);
						terminal_core_->output(s);
						float min = 0.0f;
						float max = 0.0f;
						if(get_float_(autotest_min_->get_text(), min)) {
							if(get_float_(autotest_max_->get_text(), max)) {
								auto d = max - min;
								if(d > 0.0f) {
									if(min <= a && a <= max) {
									} else {
										++autotest_fail_;
									}
								}
							}
						}
						{
							std::string s;
							s = (boost::format("%d/%d") % autotest_fail_ % autotest_count_).str();
							autotest_result_->set_text(s);
						}
						wave_task_ = wave_task::idle;
					} else if(cmd == 0x02) { // MULTI data
						auto len_h = fifo_.get();
						auto len_l = fifo_.get();
						wave_ch_ = ch;
						wave_max_ = (static_cast<uint32_t>(len_h) << 8) | len_l;
						wave_pos_ = 0;
						wave_task_ = wave_task::multi_body;
					} else {
						wave_task_ = wave_task::flush;
					}
				}
				break;
			case wave_task::multi_body:
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


		void send_single_()
		{
			uint8_t tmp[6];
			tmp[0] = 0x01;  // START
			tmp[1] = 0x00;  // Length(H)
			tmp[2] = 0x00;  // LENGTH(L)
			tmp[3] = 0x00;  // Volt(H)
			tmp[4] = 0x00;  // Volt(L)
			tmp[5] = 0x01;  // Edge
			serial_.write(tmp, sizeof(tmp));
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		efo_test(utils::director<core>& d) : director_(d),
			menu_(nullptr),
			ports_(nullptr), connect_(nullptr),

			div_(nullptr),
			ch1_pos_(nullptr), ch2_pos_(nullptr),
			ch1_gain_(nullptr), ch2_gain_(nullptr),

			single_(nullptr), autotest_(nullptr), autotest_min_(nullptr), autotest_max_(nullptr),
			autotest_result_(nullptr),

			capture_(nullptr),
			slope_(nullptr), level_(nullptr), trigger_(nullptr),
			wave_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			view_frame_(nullptr), view_core_(nullptr),
			waves_(), serial_(), serial_list_(), fifo_(),

			autotest_enable_(false), autotest_count_(0), autotest_fp_(), autotest_fail_(0),

			wave_task_(wave_task::idle),
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

			static const int menu_width  = 200;
			static const int menu_height = 800;
			static const int btn_width   = 120;
			{	// メニューパレット
				widget::param wp(vtx::irect(10, 10, menu_width, menu_height));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				menu_ = wd.add_widget<widget_frame>(wp, wp_);
				menu_->set_state(gui::widget::state::SIZE_LOCK);
			}

			{ // Serial PORT select
				widget::param wp(vtx::irect(10, 20+40*0, btn_width, 30), menu_);
				widget_list::param wp_;
				ports_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{ // コネクションボタン
				widget::param wp(vtx::irect(10, 20+40*1, btn_width, 30), menu_);
				widget_button::param wp_("connect");
				connect_ = wd.add_widget<widget_button>(wp, wp_);
				connect_->at_local_param().select_func_ = [=](int id) {
					const auto& port = ports_->get_select_text();
					if(!port.empty()) {
						if(serial_.open(port, 115200)) {
							terminal_core_->output("Open Serialport: '" + port + "'\n");
						} else {
							terminal_core_->output("Can't open Serialport: '" + port + "'\n"); 
						}
					}
				};
			}

			{ // DIV select
				widget::param wp(vtx::irect(10, 20+40*3, btn_width, 30), menu_);
				widget_list::param wp_;
				wp_.init_list_.push_back("500 ms");
				wp_.init_list_.push_back("250 ms");
				wp_.init_list_.push_back("100 ms");
				wp_.init_list_.push_back("50 ms");
				wp_.init_list_.push_back("25 ms");
				wp_.init_list_.push_back("10 ms");
				wp_.init_list_.push_back("5 ms");
				wp_.init_list_.push_back("1 ms");
				wp_.init_list_.push_back("500 us");
				wp_.init_list_.push_back("250 us");
				wp_.init_list_.push_back("100 us");
				wp_.init_list_.push_back("50 us");
				wp_.init_list_.push_back("25 us");
				wp_.init_list_.push_back("10 us");
				wp_.init_list_.push_back("5 us");
				wp_.init_list_.push_back("1 us");
				wp_.init_list_.push_back("500 ns");
				wp_.init_list_.push_back("250 ns");
				wp_.init_list_.push_back("100 ns");
				wp_.init_list_.push_back("50 ns");
				wp_.init_list_.push_back("25 ns");
				wp_.init_list_.push_back("10 ns");
				div_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{	// CH1 Positon
				widget::param wp(vtx::irect(10, 20+40*4, btn_width, 30), menu_);
				widget_spinbox::param wp_(-200, 0, 200);
				ch1_pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch1_pos_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					waves_.at_param(0).offset_.y = newpos * 10;
					return (boost::format("%d") % newpos).str();
				};
			}
			{	// CH2 Positon
				widget::param wp(vtx::irect(10, 20+40*5, btn_width, 30), menu_);
				widget_spinbox::param wp_(-200, 0, 200);
				ch2_pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch2_pos_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					waves_.at_param(1).offset_.y = newpos * 10;
					return (boost::format("%d") % newpos).str();
				};
			}
			{	// CH1 Gain
				widget::param wp(vtx::irect(10, 20+40*6, btn_width, 30), menu_);
				widget_spinbox::param wp_(0, 50, 100);
				ch1_gain_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch1_gain_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
		  			waves_.at_param(0).gain_ = static_cast<float>(newpos) * 0.01f / 10.0f;
					return (boost::format("%d") % newpos).str();
				};
			}
			{	// CH2 Gain
				widget::param wp(vtx::irect(10, 20+40*7, btn_width, 30), menu_);
				widget_spinbox::param wp_(0, 50, 100);
				ch2_gain_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch2_gain_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
		  			waves_.at_param(1).gain_ = static_cast<float>(newpos) * 0.01f / 10.0f;
					return (boost::format("%d") % newpos).str();
				};
			}

			{ // シングル・ボタン
				widget::param wp(vtx::irect(10, 20+40*8, btn_width, 30), menu_);
				widget_button::param wp_("single");
				single_ = wd.add_widget<widget_button>(wp, wp_);
				single_->at_local_param().select_func_ = [=](int id) {
					send_single_();
				};
			}
			{ // オートテスト・ボタン
				widget::param wp(vtx::irect(10, 20+40*9, btn_width, 30), menu_);
				widget_button::param wp_("autotest");
				autotest_ = wd.add_widget<widget_button>(wp, wp_);
				autotest_->at_local_param().select_func_ = [=](int id) {
					bool f = autotest_enable_;
					autotest_enable_ = !autotest_enable_;
					if(!f && autotest_enable_) {
						autotest_count_ = 0;
						autotest_fail_ = 0;
					} 
				};
			}
			{  // オートテスト最小
				widget::param wp(vtx::irect(10, 20+40*10, btn_width, 30), menu_);
				widget_label::param wp_("0", false);
				autotest_min_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // オートテスト最大
				widget::param wp(vtx::irect(10, 20+40*11, btn_width, 30), menu_);
				widget_label::param wp_("0", false);
				autotest_max_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // オートテスト結果
				widget::param wp(vtx::irect(10, 20+40*12, btn_width, 30), menu_);
				widget_label::param wp_("");
				autotest_result_ = wd.add_widget<widget_label>(wp, wp_);
			}

			{  // キャプチャー・ボタン
				widget::param wp(vtx::irect(10, 20+40*14, btn_width, 30), menu_);
				widget_button::param wp_("capture");
				capture_ = wd.add_widget<widget_button>(wp, wp_);
				capture_->at_local_param().select_func_ = [=](int fd) {
					char tmp[1];
					tmp[0] = 'B';
					serial_.write(tmp, sizeof(tmp));
				};
			}

			{  // スロープ選択
				widget::param wp(vtx::irect(10, 20+40*15, btn_width, 30), menu_);
				widget_list::param wp_;
				wp_.init_list_.push_back("pos");
				wp_.init_list_.push_back("neg");
				slope_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{  // トリガー・レベル
				widget::param wp(vtx::irect(10, 20+40*16, btn_width, 30), menu_);
				widget_label::param wp_("0");
				level_ = wd.add_widget<widget_label>(wp, wp_);
			}

			{ // トリガー・ボタン
				widget::param wp(vtx::irect(10, 20+40*17, btn_width, 30), menu_);
				widget_button::param wp_("trigger");
				capture_ = wd.add_widget<widget_button>(wp, wp_);
				capture_->at_local_param().select_func_ = [=](int fd) {
					char tmp[1];
					if(slope_->get_select_pos() == 0) tmp[0] = 'q';
					else tmp[0] = 'Q';
					serial_.write(tmp, sizeof(tmp));
				};
			}
			{ // バージョン・ボタン
				widget::param wp(vtx::irect(10, 20+40*18, btn_width, 30), menu_);
				widget_button::param wp_("version");
				single_ = wd.add_widget<widget_button>(wp, wp_);
				single_->at_local_param().select_func_ = [=](int id) {
					char tmp[1];
					tmp[0] = 0x21;
					serial_.write(tmp, sizeof(tmp));
				};
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

			if(ports_ != nullptr) ports_->load(pre);
			if(div_ != nullptr) div_->load(pre);
			if(slope_ != nullptr) slope_->load(pre); 
			if(level_ != nullptr) level_->load(pre); 

			if(ch1_pos_ != nullptr) ch1_pos_->load(pre);
			if(ch2_pos_ != nullptr) ch2_pos_->load(pre);
			if(ch1_gain_ != nullptr) ch1_gain_->load(pre);
			if(ch2_gain_ != nullptr) ch2_gain_->load(pre);
			if(autotest_min_ != nullptr) autotest_min_->load(pre);
			if(autotest_max_ != nullptr) autotest_max_->load(pre);

			// 波形バッファ生成
			waves_.create_buffer();

///			waves_.build_sin(0, 1e-6, 15e3, 1.0);
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
			{
				bool update = false;
				static uint32_t count = 1;
				++count;
				if(count >= 60) {
					count = 0;
					serial_.create_list();
					if(serial_list_.empty() || !serial_.compare(serial_list_)) {
						update = true;
						serial_list_ = serial_.get_list();
					}
				}
				if(update) {			
					if(serial_list_.empty()) {
						connect_->set_stall();
					} else {
						connect_->set_stall(false);
					}
					utils::strings list;
					for(const auto& t : serial_list_) {
						list.push_back(t.port);
						terminal_core_->output(t.port + " (" + t.info + ")\n");
					}
					ports_->get_menu()->build(list);
					ports_->select(0);
				}
			}

//			waves_.at_param(0).gain_ = 0.05f;
//			waves_.at_param(0).gain_ = 0.0055f;
//			waves_.at_param(1).gain_ = 0.05f;
//			waves_.at_param(1).gain_ = 0.0055f;
			waves_.at_param(0).color_ = img::rgba8(255, 128, 255, 255);
			waves_.at_param(1).color_ = img::rgba8(128, 255, 255, 255);
//			waves_.at_param(0).offset_.y = -300;
//			waves_.at_param(0).offset_.y = 0;
//			waves_.at_param(1).offset_.y = 100;

			waves_.at_info().time_org_ = 50;
			waves_.at_info().time_len_ = 150;
			waves_.at_info().volt_org_[0] = 90;
			waves_.at_info().volt_len_[0] = 130;
			waves_.at_info().volt_org_[1] = 90;
			waves_.at_info().volt_len_[1] = 130;

			service_waves_();

			if(autotest_enable_) {
				static uint32_t count = 0;
				++count;
				if(count >= 15) {
					send_single_();
					++autotest_count_;
					count = 0;
				}
			}

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
			if(autotest_fp_.is_open()) {
				autotest_fp_.close();
			}

			sys::preference& pre = director_.at().preference_;

			if(ch1_pos_ != nullptr) ch1_pos_->save(pre);
			if(ch2_pos_ != nullptr) ch2_pos_->save(pre);
			if(ch1_gain_ != nullptr) ch1_gain_->save(pre);
			if(ch2_gain_ != nullptr) ch2_gain_->save(pre);
			if(autotest_min_ != nullptr) autotest_min_->save(pre);
			if(autotest_max_ != nullptr) autotest_max_->save(pre);

			if(slope_ != nullptr) slope_->save(pre); 
			if(level_ != nullptr) level_->save(pre); 
			if(div_ != nullptr) div_->save(pre);
			if(ports_ != nullptr) ports_->save(pre);

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
