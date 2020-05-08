#pragma once
//=====================================================================//
/*! @file
	@brief  DX7 エミュレータクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
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
#include "snd_io/midi_file_io.hpp"

#include "utils/format.hpp"

#include "synth_unit.h"
#include "MidiFile.h"

namespace app {

	class dx7emu : public utils::i_scene {

		static const uint32_t sample_rate = 44100;
		static const uint32_t key_octave = 6;

		utils::director<core>&	director_;

		gui::widget_button*		files_;
		gui::widget_filer*		filer_;

		gui::widget_list*		midi_list_;
		
		gui::widget_button*		piano_keys_[12 * key_octave];

		gui::widget_slider*		overtone_[8];

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_list*		prog_list_;

		bool			key_back_[13];
		bool			key_[13];

		uint32_t		midi_num_;
		snd::midi_io	midi_in_;

		typedef snd::midi_file_io MIDI_FILE;
		MIDI_FILE		midi_file_;

		RingBuffer		ring_buffer_;
		SynthUnit		synth_unit_;

		void keys_()
		{
	   		gl::core& core = gl::core::get_instance();
			const gl::device& dev = core.get_device();

			for(int i = 0; i < 13; ++i) key_back_[i] = key_[i];

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

			if(dev.get_positive(gl::device::key::_0)) {
				uint8_t tmp[2];
				tmp[0] = 0xc0;
				tmp[1] = 0;
				ring_buffer_.Write(tmp, sizeof(tmp));								
			}
			if(dev.get_positive(gl::device::key::_1)) {
				uint8_t tmp[2];
				tmp[0] = 0xc0;
				tmp[1] = 1;
				ring_buffer_.Write(tmp, sizeof(tmp));								
			}
			if(dev.get_positive(gl::device::key::_2)) {
				uint8_t tmp[2];
				tmp[0] = 0xc0;
				tmp[1] = 2;
				ring_buffer_.Write(tmp, sizeof(tmp));								
			}
			if(dev.get_positive(gl::device::key::_3)) {
				uint8_t tmp[2];
				tmp[0] = 0xc0;
				tmp[1] = 3;
				ring_buffer_.Write(tmp, sizeof(tmp));								
			}
			if(dev.get_positive(gl::device::key::_9)) {
				uint8_t tmp[2];
				tmp[0] = 0xc0;
				tmp[1] = 9;
				ring_buffer_.Write(tmp, sizeof(tmp));								
			}
		}


		void init_piano_keyb_(int x, int y, int o)
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			const std::string& curp = core.get_current_path();
			vtx::irect r(x, y, 0, 0);
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


		void parse_midi_(const std::string& filename)
		{
			// MIDIファイルをパース
			smf::MidiFile midi_file;
			midi_file.read(filename);

			// マスタートラックのテンポを元に、全MIDIイベントの時間(秒)を計算
			midi_file.doTimeAnalysis();

			// 全トラックのMIDIイベントからノートオンだけ取り出して時間を出力
			for (int track = 0; track < midi_file.getTrackCount(); ++track) {
				utils::format("Track%d: Event: %d\n")
					% track % midi_file[track].size();
				for (int event = 0; event < midi_file[track].size(); ++event) {
					if (midi_file[track][event].isNote()) {
						if(midi_file[track][event].isNoteOn()) {
//							uint32_t t = midi_file[track][event].seconds * 60.0;
//							auto key = midi_file[track][event].getKeyNumber();
//							auto vel = midi_file[track][event].getVelocity();
///							utils::format(" K: %d, V: %d") % key % vel;
						}
						if(midi_file[track][event].isNoteOff()) {
///							utils::format("\n");
						}
					}
				}
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		dx7emu(utils::director<core>& d) : director_(d),
			files_(nullptr), filer_(nullptr), midi_list_(nullptr),
			piano_keys_{ nullptr },
			overtone_{ nullptr },
			terminal_frame_(nullptr), terminal_core_(nullptr),
			prog_list_(nullptr),
			key_back_{ false }, key_{ false },
			midi_num_(0), midi_in_(),
			midi_file_(),
			ring_buffer_(), synth_unit_(ring_buffer_)
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
				filer_->at_local_param().select_file_func_ = [this](const std::string& file) {
					if(utils::no_capital_strcmp(utils::get_file_ext(file), "SYX") == 0) {
						utils::file_io fin;
						if(fin.open(file, "rb")) {
							uint8_t tmp[4096 + 8];
							if(fin.read(tmp, sizeof(tmp)) == sizeof(tmp)) {
								ring_buffer_.Write(tmp, sizeof(tmp));
								{  // データを処理させる。
									const uint32_t len = 44100 / 60;
									int16_t tmp[len];
									synth_unit_.GetSamples(len, tmp);
								}
								prog_list_->at_local_param().init_list_.clear();
								for(int i = 0; i < 32; ++i) {
									char tmp[11];
									synth_unit_.get_patch_name(i, tmp, sizeof(tmp));
									char str[32];
									utils::sformat("%2d: '%s'\n", str, sizeof(str)) % i % tmp;
									terminal_core_->output(str);
									prog_list_->at_local_param().init_list_.push_back(tmp);
								}
								prog_list_->build();
							}
							fin.close();
						}
					} else if(utils::no_capital_strcmp(utils::get_file_ext(file), "MID") == 0) {
						parse_midi_(file);
						// setup midi file
//						if(!midi_file_.open(file.c_str())) {
//							utils::format("MIDI-file fail: '%s'\n") % file.c_str();
//						}
					}
				};
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
					init_piano_keyb_(10 + 170 * i, 650, 12 * i);
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

			{  // DX7 Program 選択
				widget::param wp(vtx::irect(10 + 120 + 20, 10, 200, 40), 0);
				widget_list::param wp_("DX7 Program");
				prog_list_ = wd.add_widget<widget_list>(wp, wp_);
				prog_list_->at_local_param().select_func_ = [this](const std::string& file, uint32_t pos) {
					uint8_t tmp[2];
					tmp[0] = 0xc0;
					tmp[1] = pos;
					ring_buffer_.Write(tmp, sizeof(tmp));
				};
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

			SynthUnit::Init(44'100);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;

///	   		gl::core& core = gl::core::get_instance();
///			const gl::device& dev = core.get_device();

			midi_file_.service();

			update_midi_input_();

			keys_();

			for(int i = 0; i < 13; ++i) {
				if(key_[i] && !key_back_[i]) {
					uint8_t key[3] = { 0x90, 0x3C, 0x7F };
					key[1] = 0x3C + i;
					ring_buffer_.Write(key, 3);
				}
				if(!key_[i] && key_back_[i]) {
					uint8_t key[3] = { 0x80, 0x3C, 0x7F };
					key[1] = 0x3C + i;
					ring_buffer_.Write(key, 3);
				}
			}

			for(int i = 0; i < 12 * key_octave; ++i) {
				if(piano_keys_[i]->get_select_in()) {
					uint8_t key[3] = { 0x90, 0x3C, 0x7F };
					key[1] = 24+i;
					ring_buffer_.Write(key, 3);
				}
				if(piano_keys_[i]->get_select_out()) {
					uint8_t key[3] = { 0x80, 0x3C, 0x7F };
					key[1] = 24+i;
					ring_buffer_.Write(key, 3);
				}
			}

			{
				auto& fifo = midi_in_.at_midi_in();
				while(fifo.length() > 0) {
					const auto& t = fifo.get_at();
//					utils::format("S: %02X, N: %d, V: %d\n") % static_cast<uint16_t>(t.status)
//						% static_cast<uint16_t>(t.note) % static_cast<uint16_t>(t.velocity);
					if(t.velocity != 0) {
						uint8_t key[3] = { 0x90, 0x3C, 0x7F };
						key[1] = t.note;
						key[2] = t.velocity;
						ring_buffer_.Write(key, 3);
					} else {
						uint8_t key[3] = { 0x80, 0x3C, 0x7F };
						key[1] = t.note;
						key[2] = t.velocity;
						ring_buffer_.Write(key, 3);
					}
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

///			piano_.service(wlen, 1.0f);
///			sound.queue_audio(piano_.get_wav());
			{
				const uint32_t len = 44100 / 60;
				int16_t tmp[len];
				synth_unit_.GetSamples(len, tmp);

				al::audio aif(new al::audio_sto16);
				aif->create(44100, len);
				for(uint32_t i = 0; i < len; ++i) {
					al::pcm16_s w;
					w.l = tmp[i];
					w.r = tmp[i];
					aif->put(i, w); 
				}
				al::sound& sound = director_.at().sound_;
				sound.queue_audio(aif);
			}

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
