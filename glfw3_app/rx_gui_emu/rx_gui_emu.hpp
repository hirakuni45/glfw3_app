#pragma once
//=====================================================================//
/*! @file
	@brief  RX GUI Emulation クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2019 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"

#include "snd_io/midi_io.hpp"

#include "utils/format.hpp"

namespace app {

	class rx_gui_emu : public utils::i_scene {

		utils::director<core>&	director_;

#if 0
		gui::widget_button*		files_;
		gui::widget_filer*		filer_;

		gui::widget_list*		midi_list_;
		
		gui::widget_button*		piano_keys_[12 * key_octave];

		gui::widget_slider*		overtone_[8];

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;
#endif

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		rx_gui_emu(utils::director<core>& d) : director_(d) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~rx_gui_emu() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
#if 0
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;


			{  // ボタン
				widget::param wp(vtx::irect(10, 10, 120, 40));
				widget_button::param wp_("Files");
				files_ = wd.add_widget<widget_button>(wp, wp_);
				files_->at_local_param().text_param_.set_alias("ファイラー");
				files_->at_local_param().text_param_.alias_enable_ = true;
				files_->at_local_param().select_func_ = [this](int id) {
					bool f = filer_->get_state(gui::widget::state::ENABLE);
					filer_->enable(!f);
					if(!f) {
//						auto& sound = director_.at().sound_;
//						filer_->focus_file(sound.get_file_stream());
//						files_.clear();
					}
				};
			}
			{	// ファイラーのテスト
				widget::param wp(vtx::irect(30, 30, 300, 200));
				bool save = false;
				widget_filer::param wp_(core.get_current_path(), "", save);
				filer_ = wd.add_widget<widget_filer>(wp, wp_);
				filer_->enable(false);
//				filer_->at_local_param().select_file_func_ = [this](const std::string& file) {
				   	// sound_play_(file);
//				};
			}


			{  // MIDI input 選択
				widget::param wp(vtx::irect(10, 70, 250, 40), 0);
				widget_list::param wp_("MIDI Input");
				midi_list_ = wd.add_widget<widget_list>(wp, wp_);
			}


			{  // 倍音スライダー
				for(int i = 0; i < 8; ++i) {
					widget::param wp(vtx::irect(30, 130 + 30 * i, 180, 20));
					widget_slider::param wp_;
//					wp_.select_func_ = [this] (float lvl) {
//						if(progress_ != nullptr) {
//							progress_->set_ratio(lvl);
//						}
//					};
					overtone_[i] = wd.add_widget<widget_slider>(wp, wp_);
				}
			}


			{ // 鍵盤
				for(int i = 0; i < key_octave; ++i) {
					init_piano_keyb_(10 + 170 * i, 12 * i);
				}
			}


			{	// ターミナル
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

			if(midi_list_ != nullptr) {
				midi_list_->load(pre);
			}

			if(filer_ != nullptr) {
				filer_->load(pre);
			}

			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}

			for(int i = 0; i < 8; ++i) {
				if(overtone_[i] == nullptr) continue;
				overtone_[i]->load(pre);
			}
#endif
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
			update_midi_input_();

///			keys_();

			for(int i = 0; i < 12 * key_octave; ++i) {
				if(piano_keys_[i]->get_select_in()) {
					piano_.ring(i);
///					utils::format("%d\n") % i;
				}
			}

			{
				auto& fifo = midi_in_.at_midi_in();
				while(fifo.length() > 0) {
					const auto& t = fifo.get_at();
					utils::format("N: %d, V: %d\n")
						% static_cast<uint32_t>(t.note) % static_cast<uint32_t>(t.velocity);
					fifo.get_go();
				}
			}

			al::sound& sound = director_.at().sound_;

			// サンプルレートに対応するバッファ長
			uint32_t wlen = (sample_rate / 60);
			uint32_t mod = 16;
			if(sound.get_queue_audio_length() < mod) {
				wlen += mod;
			}

			piano_.service(wlen, 1.0f);
			sound.queue_audio(piano_.get_wav());

			float vol = 1.0f;
			sound.set_gain_stream(vol);
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
#if 0
			sys::preference& pre = director_.at().preference_;

			for(int i = 0; i < 8; ++i) {
				if(overtone_[i] == nullptr) continue;
				overtone_[i]->save(pre);
			}

			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}

			if(filer_ != nullptr) {
				filer_->save(pre);
			}

			if(midi_list_ != nullptr) {
				midi_list_->save(pre);
			}
#endif
		}
	};

}
