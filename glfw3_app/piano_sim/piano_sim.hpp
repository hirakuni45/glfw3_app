#pragma once
//=====================================================================//
/*! @file
	@brief  Piano SIM クラス（アプリケーションＧＵＩ）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
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
#include "piano.hpp"

#include "snd_io/midi_io.hpp"

#include "utils/format.hpp"

namespace app {

	class piano_sim : public utils::i_scene {

		static constexpr uint32_t sample_rate = 44100;
		static constexpr uint32_t key_octave = 6;

		utils::director<core>&	director_;

		gui::widget_button*		files_;
		gui::widget_filer*		filer_;

		gui::widget_list*		midi_list_;
		
		gui::widget_button*		piano_keys_[12 * key_octave];

		gui::widget_slider*		overtone_[8];

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

#if 0
			void reberb_(float delay, float gain)
			{
//				uint32_t d = delay * static_cast<float>(sample_rate);
				uint32_t d = 600;
				if(reb_.size() < d) {
					return;
				}

				int32_t g = gain * 8192;
				for(int i = 0; i < wav_.size(); ++i) {
					int idx = i - d;
					int16_t w;
					if(idx < 0) {
						w =reb_[reb_.size() + idx];
					} else {
						w = wav_[idx];
					}
					int32_t tmp = wav_[i];
					tmp += (static_cast<int32_t>(w) * g) / 8192;
					tmp >>= 1;
					wav_[i] = tmp;
				}
			}
#endif
		bool		key_[13];

		typedef utils::piano<sample_rate, 8> PIANO;
		PIANO		piano_;


		uint32_t	midi_num_;
		snd::midi_io	midi_in_;


		void keys_()
		{
	   		gl::core& core = gl::core::get_instance();
			const gl::device& dev = core.get_device();

			key_[0] = dev.get_level(gl::device::key::Z);
			key_[1] = dev.get_level(gl::device::key::S);
			key_[2] = dev.get_level(gl::device::key::X);
			key_[3] = dev.get_level(gl::device::key::D);
			key_[4] = dev.get_level(gl::device::key::C);
			key_[5] = dev.get_level(gl::device::key::V);
			key_[6] = dev.get_level(gl::device::key::G);
			key_[7] = dev.get_level(gl::device::key::B);
			key_[8] = dev.get_level(gl::device::key::H);
			key_[9] = dev.get_level(gl::device::key::N);
			key_[10] = dev.get_level(gl::device::key::J);
			key_[11] = dev.get_level(gl::device::key::M);
			key_[12] = dev.get_level(gl::device::key::COMMA);
		}


		void init_piano_keyb_(int x, int o)
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			const std::string& curp = core.get_current_path();
			vtx::irect r(x, 400, 0, 0);
			widget_button* wb;
			static const int idx[] = {
				0, 2, 1, 4, 3, 5, 7, 6, 9, 8, 11, 10
			};
			static const int tbl[] = {
				0, 24, 14, 49, 43, 48 + 24 + 1, 48 + 24 + 1 + 24 + 1, 85, 121, 114, 146, 140
			};
			static const char* path[] = {
				"c", "d", "cs", "e", "cs", "f", "g", "cs", "a", "cs", "b", "cs", 
			};
			for(int i = 0; i < 12; ++i) {
				r.org.x = x + tbl[i];
				std::string s;
				s = "/res/piano_key_";
				s += path[i];
				s += ".png";
				wb = gui::create_image<widget_button>(wd, curp + s, r);
				wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
				wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
				piano_keys_[idx[i] + o] = wb;
			}
		}


		void update_midi_input_()
		{
			uint32_t num = snd::midi_io::get_device_num();
			if(midi_num_ != num) {
				midi_num_ = num;
				std::string tmp = midi_list_->get_select_text();
				midi_list_->at_local_param().init_list_.clear();
				for(int32_t i = 0; i < num; ++i) {
					auto s = snd::midi_io::get_device_name(i);
// utils::format("%s\n") % s.c_str();
					midi_list_->at_local_param().init_list_.push_back(s);
				}
				midi_list_->build();

				// MIDI in の開始
				if(tmp != midi_list_->get_select_text() || !midi_in_.probe()) {
					midi_in_.start(midi_list_->get_select_text());
				}
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano_sim(utils::director<core>& d) : director_(d),
			files_(nullptr), filer_(nullptr), midi_list_(nullptr),
			piano_keys_{ nullptr },
			overtone_{ nullptr },
			terminal_frame_(nullptr), terminal_core_(nullptr),
			key_{ false }, midi_num_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~piano_sim() { }


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
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;

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
		}
	};

}
