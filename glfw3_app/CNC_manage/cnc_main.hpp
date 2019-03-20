#pragma once
//=====================================================================//
/*! @file
    @brief  CNC メイン・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
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
#include "gl_fw/glcamera.hpp"

#include "utils/serial_win32.hpp"
#include "utils/format.hpp"
#include "utils/input.hpp"
#include "utils/vtx.hpp"

#include <queue>

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  CNC メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class cnc_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		menu_;
		gui::widget_list*		ports_;
		gui::widget_list*		baud_;
		gui::widget_button*		connect_;
		gui::widget_button*		carib_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gl::camera				camera_;

		typedef device::serial_win32 SERIAL;
		SERIAL					serial_;
		SERIAL::name_list		serial_list_;

		typedef std::queue<char> FIFO;
		FIFO					fifo_;
		std::string				line_;

		vtx::ivtx				raw_;
		vtx::ivtx				ref_min_;
		vtx::ivtx				ref_max_;
		uint32_t				ref_count_;
		vtx::fvtx				gv_;

		uint32_t				count_;

		// ターミナル、行入力
		void term_enter_(const utils::lstring& text) {
			auto s = utils::utf32_to_utf8(text);
		}


		std::string get_line_()
		{
			std::string line;
			if(fifo_.empty()) {
				return line;
			}

			while(!fifo_.empty()) {
				auto ch = fifo_.front();
				fifo_.pop();
				if(ch == '\n') {
					line = line_;
					line_.clear();
					break;
				} else if(ch == '\r') {
				} else {
					line_ += ch;
				}
			}
			return line;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		cnc_main(utils::director<core>& d) noexcept : director_(d),
			menu_(nullptr),
			ports_(nullptr), baud_(nullptr), connect_(nullptr), carib_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			serial_(), serial_list_(), fifo_(),
			raw_(), ref_min_(), ref_max_(0), ref_count_(0), gv_(), count_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;
			gl::core& core = gl::core::get_instance();

			int menu_width  = 200;
			int menu_height = 320;
			{	// メニューパレット
				widget::param wp(vtx::irect(10, 10, menu_width, menu_height));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				menu_ = wd.add_widget<widget_frame>(wp, wp_);
				menu_->set_state(gui::widget::state::SIZE_LOCK);
			}

			{ // Serial PORT select
				widget::param wp(vtx::irect(10, 25 + 55 * 0, 150, 40), menu_);
				widget_list::param wp_;
				ports_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{ // baud select
				widget::param wp(vtx::irect(10, 25 + 55 * 1, 150, 40), menu_);
				widget_list::param wp_;
		   		wp_.init_list_.push_back("19200");
		   		wp_.init_list_.push_back("38400");
		   		wp_.init_list_.push_back("57600");
		   		wp_.init_list_.push_back("115200");
				baud_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{ // コネクションボタン
				widget::param wp(vtx::irect(10, 25 + 55 * 2, 150, 40), menu_);
				widget_button::param wp_("connect");
				connect_ = wd.add_widget<widget_button>(wp, wp_);
				connect_->at_local_param().select_func_ = [=](int id) {
					const auto& port = ports_->get_select_text();
					if(serial_.probe()) {
						ports_->set_stall(false);
						baud_->set_stall(false);
						carib_->set_stall();
						serial_.close();
						terminal_core_->output("Close Serialport: '" + port + "'\n");
						connect_->set_text("connect");
					} else {
						if(!port.empty()) {
							int b;
							if((utils::input("%d", baud_->get_select_text().c_str())
								 % b).status()) {
								if(serial_.open(port, b)) {
									connect_->set_text("close");
									terminal_core_->output("Open Serialport: '" + port + "'\n");
									ports_->set_stall();
									baud_->set_stall();
									carib_->set_stall(false);
								} else {
									terminal_core_->output(
										"Can't open Serialport: '" + port + "'\n");
								}
							}
						}
					}
				};
			}
			{ // キャリブレーション・ボタン
				widget::param wp(vtx::irect(10, 25 + 55 * 3, 150, 40), menu_);
				widget_button::param wp_("carib");
				carib_ = wd.add_widget<widget_button>(wp, wp_);
				carib_->set_stall();
				carib_->at_local_param().select_func_ = [=](int id) {
					ref_min_.set(0);
					ref_max_.set(0);
					ref_count_ = 120;
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
			}

			// プリファレンスのロード
			sys::preference& pre = director_.at().preference_;

			if(baud_ != nullptr) {
				baud_->load(pre);
			}
			if(menu_ != nullptr) {
				menu_->load(pre, false, false);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(ports_ != nullptr) ports_->load(pre);
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

			// ＲＡＷデータの取得と変換
			if(serial_.probe()) {
				char tmp[256];
				auto len = serial_.read(tmp, sizeof(tmp));
				for(uint32_t i = 0; i < len; ++i) {
					fifo_.push(tmp[i]);
				}
			}

			while(1)  {
				auto line = get_line_();
				if(line.empty()) {
					break;
				}
				int x, y, z;
				if((utils::input("%d,%d,%d", line.c_str()) % x % y % z).num() == 3) {
					raw_.x = x;
					raw_.y = y;
					raw_.z = z;
//					char tmp[256];
//					utils::sformat("%d, %d, %d\n", tmp, sizeof(tmp)) % x % y % z;
//					terminal_core_->output(tmp);
					if(ref_count_ > 0) {  // キャリブレーション
						if(ref_min_.x > raw_.x) ref_min_.x = raw_.x;
						if(ref_max_.x < raw_.x) ref_max_.x = raw_.x;
						if(ref_min_.y > raw_.y) ref_min_.y = raw_.y;
						if(ref_max_.y < raw_.y) ref_max_.y = raw_.y;
						if(ref_min_.z > raw_.z) ref_min_.z = raw_.z;
						if(ref_max_.z < raw_.z) ref_max_.z = raw_.z;
						--ref_count_;
						if(ref_count_ == 0) {
							gv_.set(0.0f);
						}
					} else {
						vtx::ivtx d(0);
						if(ref_min_.x > raw_.x || ref_max_.x < raw_.x) d.x = raw_.x;
						if(ref_min_.y > raw_.y || ref_max_.y < raw_.y) d.y = raw_.y;
						if(ref_min_.z > raw_.z || ref_max_.z < raw_.z) d.z = raw_.z;
						float g = 0.07f;
						gv_.x += static_cast<float>(d.x) * g;
						gv_.y += static_cast<float>(d.y) * g;
						gv_.z += static_cast<float>(d.z) * g;
					}
				} else {
					std::string s = "NG: ";
					s += line;
					terminal_core_->output(s);
				}
			}

			if(serial_.probe()) {
				++count_;
				if(count_ >= 30) {
					count_ = 0;
					char tmp[256];
					utils::sformat("%3.2f, %3.2f, %3.2f\n", tmp, sizeof(tmp))
						% gv_.x % gv_.y % gv_.z;
					terminal_core_->output(tmp);				
				}
			}

			if(!wd.update()) {
				camera_.update();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render()
		{
#if 0
			camera_.service();

			{
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
				glEnable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

				gl::draw_grid(vtx::fpos(-10.0f), vtx::fpos(10.0f), vtx::fpos(1.0f));
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_BLEND);
			}
#endif
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

			if(ports_ != nullptr) ports_->save(pre);

			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(menu_ != nullptr) {
				menu_->save(pre);
			}
			if(baud_ != nullptr) {
				baud_->save(pre);
			}
		}
	};
}
