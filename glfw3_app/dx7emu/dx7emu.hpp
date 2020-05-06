#pragma once
//=====================================================================//
/*! @file
	@brief  @n
			Copyright 2020 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_slider.hpp"
#include "gl_fw/gltexfb.hpp"
#include "snd_io/pcm.hpp"
#include "utils/fifo.hpp"
#include "utils/input.hpp"

#include "synth_unit.h"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	DX7/emu シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dx7emu : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;
		bool					terminal_;

		gui::widget_filer*		filer_;

		gui::widget_frame*		menu_frame_;
		bool					menu_;
		gui::widget_spinbox*	state_slot_;
		gui::widget_button*		state_save_;
		gui::widget_button*		state_load_;
		gui::widget_button*		reset_;
		gui::widget_slider*		volume_;

		gui::widget_dialog*		dialog_;

		std::string		file_;
		bool			play_;

		int				pause_;

		RingBuffer		ring_buffer_;
		SynthUnit		synth_unit_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		dx7emu(utils::director<core>& d) : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr), terminal_(false),
			menu_frame_(nullptr), menu_(false),
			state_slot_(nullptr), state_save_(nullptr), state_load_(nullptr), reset_(nullptr),
			volume_(nullptr),
			dialog_(nullptr),
			play_(false), pause_(0),
			ring_buffer_(), synth_unit_(&ring_buffer_)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~dx7emu() { }


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
			{  	// ターミナル
				{
					widget::param wp(vtx::irect(10, 10, 200, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
					terminal_frame_->enable(terminal_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [=] (const utils::lstring& inp) {
						auto s = utils::utf32_to_utf8(inp);
///						tools_.command(s);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);

///					emu::tools::set_terminal(terminal_core_);
				}
			}

			{  	// ファイラー
				widget::param wp(vtx::irect(10, 10, 200, 200));
				widget_filer::param wp_(core.get_current_path(), "");
				wp_.select_file_func_ = [=](const std::string& fn) {

				};
				filer_ = wd.add_widget<widget_filer>(wp, wp_);
				filer_->enable(false);
			}

			{
				{   // メニュー
					widget::param wp(vtx::irect(20, 20, 210, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(15);
					menu_frame_ = wd.add_widget<widget_frame>(wp, wp_);
					menu_frame_->enable(false);
					menu_frame_->at_param().state_.set(widget::state::SIZE_LOCK);
				}
				{   // スピン・ボックス
					widget::param wp(vtx::irect(10, 35, 90, 30), menu_frame_);
					widget_spinbox::param wp_(0, 0, 9);
					wp_.select_func_ = [=] (widget_spinbox::state st, int before, int newpos) {
						return (boost::format("%d") % newpos).str();
					};
					state_slot_ = wd.add_widget<widget_spinbox>(wp, wp_);
				}
				{ // ステート・セーブ
					widget::param wp(vtx::irect(110, 35+40*0, 90, 30), menu_frame_);
					widget_button::param wp_("Save");
					state_save_ = wd.add_widget<widget_button>(wp, wp_);
					state_save_->at_local_param().select_func_ = [=](int id) {
#if 0
						state_setslot(get_state_no_());
						if(state_save() != 0) {
							dialog_->set_text("Save state error");
							dialog_->enable();
						} else {
							if(filer_ != nullptr) filer_->rescan_center();
						}
#endif
					};

				}
				{ // ステート・ロード
					widget::param wp(vtx::irect(110, 35+40*1, 90, 30), menu_frame_);
					widget_button::param wp_("Load");
					state_load_ = wd.add_widget<widget_button>(wp, wp_);
					state_load_->at_local_param().select_func_ = [=](int id) {
#if 0
						state_setslot(get_state_no_());
						if(!nes_play_) {
							dialog_->set_text("Load state error: 'NES file not load'");
							dialog_->enable();
						} else if(state_load() != 0) {
							dialog_->set_text("Load state error");
							dialog_->enable();
						}
#endif
					};
				}
				{ // NES リセット
					widget::param wp(vtx::irect(10, 35+40*2, 90, 30), menu_frame_);
					widget_button::param wp_("Reset");
					reset_ = wd.add_widget<widget_button>(wp, wp_);
					reset_->at_local_param().select_func_ = [=](int id) {
///						nes_reset(HARD_RESET);
					};
				}
				{   // ボリューム
					widget::param wp(vtx::irect(10, 35+40*3, 190, 20), menu_frame_);
					widget_slider::param wp_(0.5f);
					volume_ = wd.add_widget<widget_slider>(wp, wp_);
				}
			}

			{   // Daialog
				widget::param wp(vtx::irect(70, 70, 300, 150));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
			}



			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(filer_ != nullptr) {
				filer_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(menu_frame_ != nullptr) {
				menu_frame_->load(pre, false, false);
			}
			if(volume_ != nullptr) {
				volume_->load(pre);
			}

			SynthUnit::Init(44'100);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
	   		gl::core& core = gl::core::get_instance();
			const gl::device& dev = core.get_device();

			if(dev.get_negative(gl::device::key::F1)) {
				filer_->enable(!filer_->get_enable());
			}
			if(dev.get_negative(gl::device::key::F5)) {
				terminal_ = !terminal_;
				terminal_frame_->enable(terminal_);
			}
			if(dev.get_positive(gl::device::key::ESCAPE)) {
				pause_ ^= 1;
				// nes_pause(nes_pause_);
			}
			if(dev.get_positive(gl::device::key::F2)) {
				menu_ = !menu_;
				menu_frame_->enable(menu_);
			}

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
//	   		gl::core& core = gl::core::get_instance();

//			const vtx::spos& siz = core.get_rect().size;

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
///			emu::tools::set_terminal(nullptr);
///			nes_destroy();

			sys::preference& pre = director_.at().preference_;
			if(filer_ != nullptr) {
				filer_->save(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(menu_frame_ != nullptr) {
				menu_frame_->save(pre);
			}
			if(volume_ != nullptr) {
				volume_->save(pre);
			}
		}
	};

}
