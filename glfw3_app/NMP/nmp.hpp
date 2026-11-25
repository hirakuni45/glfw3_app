#pragma once
//=========================================================================//
/*! @file
	@brief  NECRONOMICON PINBALL glfw3_app メイン・クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
#include <format>
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
#include "utils/format.hpp"
#include "utils/input.hpp"

#include "src/saturn.h"
#include "nmp_core.hpp"

static uint32_t mobj_idx_pos_ = 0;
static uint32_t mobj_idx_last_ = 0;
static MOBJ mobj_pad_[64];

extern "C" {

	void start_motion_object(void)
	{
		mobj_idx_pos_ = 0;
		mobj_idx_last_ = 0;
	}

	void setup_motion_object(const void* mobj)
	{
		if(mobj_idx_pos_ < 64) {
			auto p = static_cast<const MOBJ*>(mobj);
			mobj_pad_[mobj_idx_pos_] = *p;
			mobj_idx_pos_++;
		}
	}

	void end_motion_object(void)
	{
		mobj_idx_last_ = mobj_idx_pos_;
	}
}

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	NMP シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class nmp : public utils::i_scene {

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
		gui::widget_button*		nes_reset_;
		gui::widget_slider*		volume_;

		gui::widget_dialog*		dialog_;

		gl::texfb				texfb_;

		static const int fb_width_   = 640;
		static const int fb_height_  = 448;
		static const int sample_rate_ = 44100;
		static const int audio_len_   = sample_rate_ / 60;

		uint8_t			fb_[fb_width_ * fb_height_ * 4];

		void pad_() noexcept
		{
			gl::core& core = gl::core::get_instance();

			const auto& dev = core.get_device();

			// A
			if(dev.get_level(gl::device::key::Z)) {
///				inp_[0].data |= INP_PAD_A;
			}
			if(dev.get_level(gl::device::key::GAME_0)) {
///				inp_[0].data |= INP_PAD_A;
			}

			// B
			if(dev.get_level(gl::device::key::X)) {
///				inp_[0].data |= INP_PAD_B;
			}
			if(dev.get_level(gl::device::key::GAME_1)) {
///				inp_[0].data |= INP_PAD_B;
			}

			// SELECT
			if(dev.get_level(gl::device::key::_1)) {
///				inp_[0].data |= INP_PAD_SELECT;
			}
			if(dev.get_level(gl::device::key::GAME_2)) {
///				inp_[0].data |= INP_PAD_SELECT;
			}
			// START
			if(dev.get_level(gl::device::key::_2)) {
///				inp_[0].data |= INP_PAD_START;
			}
			if(dev.get_level(gl::device::key::GAME_3)) {
///				inp_[0].data |= INP_PAD_START;
			}

			if(dev.get_level(gl::device::key::LEFT)) {
///				inp_[0].data |= INP_PAD_LEFT;
			}
			if(dev.get_level(gl::device::key::GAME_LEFT)) {
///				inp_[0].data |= INP_PAD_LEFT;
			}
			if(dev.get_level(gl::device::key::RIGHT)) {
///				inp_[0].data |= INP_PAD_RIGHT;
			}
			if(dev.get_level(gl::device::key::GAME_RIGHT)) {
///				inp_[0].data |= INP_PAD_RIGHT;
			}
			if(dev.get_level(gl::device::key::UP)) {
///				inp_[0].data |= INP_PAD_UP;
			}
			if(dev.get_level(gl::device::key::GAME_UP)) {
///				inp_[0].data |= INP_PAD_UP;
			}
			if(dev.get_level(gl::device::key::DOWN)) {
///				inp_[0].data |= INP_PAD_DOWN;
			}
			if(dev.get_level(gl::device::key::GAME_DOWN)) {
///				inp_[0].data |= INP_PAD_DOWN;
			}
		}

		void service_SATURN_BG() noexcept
		{
			const uint8_t* src = get_VDP2_VRAM_TOP();
			uint8_t* dst = fb_;
			for(int y = 0; y < 448; ++y) {
				const uint32_t* clut = GetCLUT();
				for(int x = 0; x < 640; ++x) {
					uint8_t idx = src[x];
					*dst++ = (clut[idx]) & 0xff;
					*dst++ = (clut[idx] >> 8) & 0xff;
					*dst++ = (clut[idx] >> 16) & 0xff;
					*dst++ = 255;
				}
				src += 1024;
			}
		}

		void render_mobj(const MOBJ& mobj) noexcept
		{
			int yt = mobj.y;
			int xt = mobj.x;
			int ye = mobj.y + mobj.v;
			int xe = mobj.x + mobj.h;

			const uint32_t* clut_org = GetCLUT();
			clut_org += mobj.color;

			const uint8_t* src = get_VDP1_VRAM_TOP();
			src += static_cast<uint32_t>(mobj.chaptr) << 3;
			uint32_t srcofs = 0;
			for(int y = yt; y < ye; ++y) {
				auto ofs = srcofs;
				srcofs += mobj.h;
				if(y >= 0 && y < fb_height_) ;
				else {
					continue;
				}
				uint8_t* dst = &fb_[y * fb_width_ * 4];
				for(int x = xt; x < xe; ++x) {
					if(x >= 0 && x < fb_width_) {
						uint8_t idx = 0;
						if(mobj.mode == 0) {
							idx = src[ofs >> 1];
							if((ofs & 1) == 0) idx >>= 4;
							idx &= 0xf;
						} else {
							idx = src[ofs];
						}
						uint32_t c = clut_org[idx];
						if(idx != 0) {
							auto l = x * 4;
							dst[l + 0] = c & 0xff;
							dst[l + 1] = (c >> 8) & 0xff;
							dst[l + 2] = (c >> 16) & 0xff;
							dst[l + 3] = 255;
						}
					}
					ofs++;
				}
			}
		}

		void service_SATURN_OBJ() noexcept
		{
			for(uint32_t i = 0; i < mobj_idx_last_; ++i) {
				render_mobj(mobj_pad_[i]);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		nmp(utils::director<core>& d) noexcept : director_(d),
			terminal_frame_(nullptr), terminal_core_(nullptr), terminal_(false),
			menu_frame_(nullptr), menu_(false),
			volume_(nullptr),
			dialog_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~nmp() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept
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
					wp_.enter_func_ = [=] (const std::u32string& inp) {
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

///					if(nes_insertcart(fn.c_str()) == 0) {
///						nes_file_ = fn;
///						nes_play_ = true;
///						tools_.enable();
///					} else {
///						nes_file_ = "";
///						nes_play_ = false;
///						dialog_->enable();
///						dialog_->set_text("NES file false:\n'" + fn + "'");
///						tools_.enable(false);
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
						return std::format("{}", newpos);
					};
					state_slot_ = wd.add_widget<widget_spinbox>(wp, wp_);
				}
				{ // ステート・セーブ
					widget::param wp(vtx::irect(110, 35+40*0, 90, 30), menu_frame_);
					widget_button::param wp_("Save");
					state_save_ = wd.add_widget<widget_button>(wp, wp_);
					state_save_->at_local_param().select_func_ = [=](int id) {
///						state_setslot(get_state_no_());
///						if(state_save() != 0) {
///							dialog_->set_text("Save state error");
///							dialog_->enable();
///						} else {
///							if(filer_ != nullptr) filer_->rescan_center();
///						}
					};

				}
				{ // ステート・ロード
					widget::param wp(vtx::irect(110, 35+40*1, 90, 30), menu_frame_);
					widget_button::param wp_("Load");
					state_load_ = wd.add_widget<widget_button>(wp, wp_);
					state_load_->at_local_param().select_func_ = [=](int id) {
///						state_setslot(get_state_no_());
///						if(!nes_play_) {
///							dialog_->set_text("Load state error: 'NES file not load'");
///							dialog_->enable();
///						} else if(state_load() != 0) {
///							dialog_->set_text("Load state error");
///							dialog_->enable();
///						}
					};
				}
				{ // リセット
					widget::param wp(vtx::irect(10, 35+40*2, 90, 30), menu_frame_);
					widget_button::param wp_("Reset");
					nes_reset_ = wd.add_widget<widget_button>(wp, wp_);
					nes_reset_->at_local_param().select_func_ = [=](int id) {
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

			texfb_.initialize(fb_width_, fb_height_, 32);

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

			main_nmp(0);
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
			if(dev.get_positive(gl::device::key::F2)) {
				menu_ = !menu_;
				menu_frame_->enable(menu_);
			}

			uint32_t keybits = 0;
			if(dev.get_level(gl::device::key::UP)) {
				keybits |= SATURN_DIR_UP;
			}
			if(dev.get_level(gl::device::key::DOWN)) {
				keybits |= SATURN_DIR_DOWN;
			}
			if(dev.get_level(gl::device::key::RIGHT)) {
				keybits |= SATURN_DIR_RIGHT;
			}
			if(dev.get_level(gl::device::key::LEFT)) {
				keybits |= SATURN_DIR_LEFT;
			}
			if(dev.get_level(gl::device::key::ENTER)) {
				keybits |= SATURN_PAD_START;
			}
			if(dev.get_level(gl::device::key::ESCAPE)) {
//				keybits |= KEY_BITS_ESC;
			}
			if(dev.get_level(gl::device::key::TAB)) {
//				keybits |= SATURN_PAD_;
			}

			EntryVsync(keybits);  // NMP API
			ExitVsync();   // NMP API
			ServiceTask();  // NMP API

			service_SATURN_BG();
			service_SATURN_OBJ();

			texfb_.rendering(gl::texfb::IMAGE::RGBA, (const char*)&fb_[0]);
			texfb_.flip();

			// ストリームのゲイン(volume)を設定
			if(volume_ != nullptr) {
				auto vol = volume_->get_slider_param().position_;
				al::sound& sound = director_.at().sound_;
				sound.set_gain_stream(vol);
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
			gl::core& core = gl::core::get_instance();

			const vtx::spos& siz = core.get_rect().size;

			texfb_.setup_matrix(0, 0, siz.x, siz.y);

			float scale = 1.0f;
			float ofsx = 0.0f;
			float ofsy = 0.0f;
			if(siz.x < siz.y) {
				scale = static_cast<float>(siz.x) / static_cast<float>(fb_width_);
				float h = static_cast<float>(fb_height_);
				ofsy = (static_cast<float>(siz.y) - h * scale) * 0.5f;
			} else {
				scale = static_cast<float>(siz.y) / static_cast<float>(fb_height_);
				float w = static_cast<float>(fb_width_);
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