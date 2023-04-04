#pragma once
//=====================================================================//
/*! @file
	@brief  GB Emulator クラス @n
			操作説明： @n
			「１」： SELECT @n
			「２」： START @n
			「Ｚ」： A-BUTTON @n
			「Ｘ」： B-BUTTON @n
			「↑」： UP-DIR @n
			「↓」： DOWN-DIR @n
			「→」： RIGHT-DIR @n
			「←」： LEFT-DIR @n
			「F1」： Filer @n
			「F4」： Log Terminal @n
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

#include "gearboy.h"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	nesemu シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class gbemu : public utils::i_scene {

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

		gl::texfb				texfb_;

		uint8_t			fb_[GAMEBOY_WIDTH * GAMEBOY_HEIGHT * 4];

		std::string		file_;
		bool			play_;

		int				pause_;

		struct gb_t {
			uint16_t		rgb565_[GAMEBOY_WIDTH * GAMEBOY_HEIGHT];
			int16_t			audio_[AUDIO_BUFFER_SIZE];
//			GearboyCore		core_;

			uint32_t		audio_len_;

//			gb_t() : core_(), audio_len_(0) { }
			gb_t() : audio_len_(0) { }

//			void init() { core_.Init(); }
			void init() {  }

			void clear_fb() {
				for(uint32_t i = 0; i < (GAMEBOY_WIDTH * GAMEBOY_HEIGHT); ++i) {
					rgb565_[i] = 0x0000;
				}
			}

			void clear_sb() {
				for(uint32_t i = 0; i < AUDIO_BUFFER_SIZE; ++i) {
					audio_[i] = 0;
				}
			}

			void update(uint8_t keypad)
			{
				static uint8_t kp_ = 0;
				if(kp_ != keypad) {
//					utils::format("%08b\n") % static_cast<uint16_t>(keypad);
//					kp_ = keypad;
				}
///				core_.KeySet(keypad);
				int sampleCount = 0;
///				core_.RunToVBlank(rgb565_, audio_, &sampleCount);
				if(audio_len_ != sampleCount) {
					std::cout << sampleCount << std::endl;
				}
				audio_len_ = sampleCount;
			}

			al::audio create_audio()
			{
				al::audio aif(new al::audio_sto16);
				auto len = audio_len_;
				aif->create(44100, len / 2);
				for(uint32_t i = 0; i < len / 2; ++i) {
					al::pcm16_s w;
					w.l = audio_[i * 2 + 0];
					w.r = audio_[i * 2 + 1];
					aif->put(i, w); 
				}
				return aif;	
			}
		};
		gb_t			gb_;

		uint8_t pad_()
		{
	   		gl::core& core = gl::core::get_instance();

			const gl::device& dev = core.get_device();

			uint8_t keypad = 0;
			// A
			if(dev.get_level(gl::device::key::Z)) {
				keypad |= 1 << Gameboy_Keys::A_Key;
			}
			if(dev.get_level(gl::device::key::GAME_0)) {
				keypad |= 1 << Gameboy_Keys::A_Key;
			}

			// B
			if(dev.get_level(gl::device::key::X)) {
				keypad |= 1 << Gameboy_Keys::B_Key;
			}
			if(dev.get_level(gl::device::key::GAME_1)) {
				keypad |= 1 << Gameboy_Keys::B_Key;
			}

			// SELECT
			if(dev.get_level(gl::device::key::_1)) {
				keypad |= 1 << Gameboy_Keys::Select_Key;
			}
			if(dev.get_level(gl::device::key::GAME_2)) {
				keypad |= 1 << Gameboy_Keys::Select_Key;
			}
			// START
			if(dev.get_level(gl::device::key::_2)) {
				keypad |= 1 << Gameboy_Keys::Start_Key;
			}
			if(dev.get_level(gl::device::key::GAME_3)) {
				keypad |= 1 << Gameboy_Keys::Start_Key;
			}

			if(dev.get_level(gl::device::key::LEFT)) {
				keypad |= 1 << Gameboy_Keys::Left_Key;
			}
			if(dev.get_level(gl::device::key::GAME_LEFT)) {
				keypad |= 1 << Gameboy_Keys::Left_Key;
			}
			if(dev.get_level(gl::device::key::RIGHT)) {
				keypad |= 1 << Gameboy_Keys::Right_Key;
			}
			if(dev.get_level(gl::device::key::GAME_RIGHT)) {
				keypad |= 1 << Gameboy_Keys::Right_Key;
			}
			if(dev.get_level(gl::device::key::UP)) {
				keypad |= 1 << Gameboy_Keys::Up_Key;
			}
			if(dev.get_level(gl::device::key::GAME_UP)) {
				keypad |= 1 << Gameboy_Keys::Up_Key;
			}
			if(dev.get_level(gl::device::key::DOWN)) {
				keypad |= 1 << Gameboy_Keys::Down_Key;
			}
			if(dev.get_level(gl::device::key::GAME_DOWN)) {
				keypad |= 1 << Gameboy_Keys::Down_Key;
			}
			return keypad;
		}

		int get_state_no_() const {
			if(state_slot_ == nullptr) {
				return -1;
			}
			int n;
			if((utils::input("%d", state_slot_->get_select_text().c_str()) % n).status()) {
				return n;
			} else {
				return -1;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		gbemu(utils::director<core>& d) : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr), terminal_(false),
			menu_frame_(nullptr), menu_(false),
			state_slot_(nullptr), state_save_(nullptr), state_load_(nullptr), reset_(nullptr),
			volume_(nullptr),
			dialog_(nullptr),
			play_(false), pause_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~gbemu() { }


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
					auto ct = Cartridge::CartridgeTypes::CartridgeNoMBC;
///					if(gb_.core_.LoadROM(fn.c_str(), 0, ct)) {
///						file_ = fn;
///						play_ = true;
///					}
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

			texfb_.initialize(GAMEBOY_WIDTH, GAMEBOY_HEIGHT, 32);

			gb_.clear_fb();
			gb_.init();
			gb_.clear_sb();

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

			if(play_) {
				auto keypad = pad_();
				gb_.update(keypad ^ 0xff);
				{  // copy audio
					al::sound& sound = director_.at().sound_;
					auto aif = gb_.create_audio();
					sound.queue_audio(aif);
				}

				// copy video
				for(int h = 0; h < GAMEBOY_HEIGHT; ++h) {
					uint8_t* dst = &fb_[h * GAMEBOY_WIDTH * 4];
					const auto* src = &gb_.rgb565_[h * GAMEBOY_WIDTH];
					for(int w = 0; w < GAMEBOY_WIDTH; ++w) {
						*dst++ = (((src[w] >> 11) & 0x1F ) * 255 + 15) / 31;
						*dst++ = (((src[w] >> 5) & 0x3F ) * 255 + 31) / 63;
						*dst++ = ((src[w] & 0x1F ) * 255 + 15) / 31;
						*dst++ = 255;  // alpha
					}
				}
				texfb_.rendering(gl::texfb::IMAGE::RGBA, (const char*)&fb_[0]);
				texfb_.flip();

				// ストリームのゲイン(volume)を設定
				if(volume_ != nullptr) {
					auto vol = volume_->get_slider_param().position_;
					al::sound& sound = director_.at().sound_;
					sound.set_gain_stream(vol);
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
	   		gl::core& core = gl::core::get_instance();

			const vtx::spos& siz = core.get_rect().size;

			texfb_.setup_matrix(0, 0, siz.x, siz.y);

			float scale = 1.0f;
			float ofsx = 0.0f;
			float ofsy = 0.0f;
			if(siz.x < siz.y) {
				scale = static_cast<float>(siz.x) / static_cast<float>(GAMEBOY_WIDTH);
				float h = static_cast<float>(GAMEBOY_HEIGHT);
				ofsy = (static_cast<float>(siz.y) - h * scale) * 0.5f;
			} else {
				scale = static_cast<float>(siz.y) / static_cast<float>(GAMEBOY_HEIGHT);
				float w = static_cast<float>(GAMEBOY_WIDTH);
				ofsx = (static_cast<float>(siz.x) - w * scale) * 0.5f;
			}
			gl::glTranslate(ofsx, ofsy);
			gl::glScale(scale);
			texfb_.draw();

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
