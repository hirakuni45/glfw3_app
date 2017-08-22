//=====================================================================//
/*! @file
	@brief  Piano SIM クラス
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
#include "widgets/widget_slider.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "piano.hpp"

#include "utils/format.hpp"

namespace app {

	class piano_sim : public utils::i_scene {

		static const uint32_t sample_rate = 44100;

		utils::director<core>&	director_;

		gui::widget_button*		test_ring_;

		gui::widget_button*		piano_keys_[48];

		gui::widget_slider*		overtone_[8];

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;


		class piano_t {

			const float	*abc_;

			float	frq_;
			float	angle_[8];

			al::sound::waves16 reb_;
			al::sound::waves16 wav_;

			uint32_t	reb_smp_;

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

		public:
			piano_t(const float* abc) : abc_(abc), frq_(0.0f), angle_{ 0.0f },
				reb_(), wav_(),
				reb_smp_(0) { }

			void reset()
			{
				frq_ = 0.0f;
				for(int i = 0; i < 8; ++i) angle_[i] = 0.0f;
			}


			void set_key(uint32_t idx)
			{
				float oct = static_cast<float>(1 << (idx / 12));
				frq_ = abc_[idx % 12] * oct;
			}


			void render(uint32_t len, float gain)
			{
				static const float sqlvt[] = {
///					1.0f, 0.6f, 0.7f, 0.6f, 0.68f, 0.68f, 0.55f, 0.55f 
					1.0f, 0.72f, 0.55f, 0.49f, 0.73f, 0.52f, 0.2f, 0.15f 
				};

				reb_ = wav_;
				wav_.resize(len);
				for(int i = 0; i < len; ++i) {
					float l = 0.0f;
					for(int j = 0; j < 8; ++j) {
						l += sinf(angle_[j]) * sqlvt[j];
						angle_[j] += frq_ * static_cast<float>(j + 1);
					}

					l *= gain;
					wav_[i] = static_cast<int16_t>(l * 10000.0f);
				}

				// リバーブ処理				
				reberb_(50e-3, 0.6f);
			}

			const al::sound::waves16& get_wav() const { return wav_; }
		};



		float		abc_[12];
		float		gain_;

		piano_t		piano_;

		float		env_;
		uint32_t	idx_;

		bool		key_[13];

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

		void init_piano_key_(int x, int o)
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			const std::string& curp = core.get_current_path();
			vtx::irect r(x, 400, 0, 0);
			widget_button* wb;

			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_c.png", r);  // C
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[0 + o] = wb;

			r.org.x = x + 24;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_d.png", r);  // D
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[2 + o] = wb;

			r.org.x = x + 14;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_cs.png", r); // C#
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[1 + o] = wb;

			r.org.x = x + 24 + 24 + 1;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_e.png", r);  // E
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[4 + o] = wb;

			r.org.x = x + 43;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_cs.png", r);  // D#
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[3 + o] = wb;

			r.org.x = x + 48 + 24 + 1;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_f.png", r);  // F
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[5 + o] = wb;

			r.org.x = x + 48 + 24 + 1 + 24 + 1;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_g.png", r);  // G
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[7 + o] = wb;

			r.org.x = x + 85;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_cs.png", r);  // F#
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[6 + o] = wb;

			r.org.x = x + 122;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_a.png", r);  // A
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[9 + o] = wb;

			r.org.x = x + 114;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_cs.png", r);  // G#
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[8 + o] = wb;

			r.org.x = x + 146;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_a.png", r);  // B
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[11 + o] = wb;

			r.org.x = x + 140;
			wb = gui::create_image<widget_button>(wd, curp + "/res/piano_key_cs.png", r);  // A#
			wb->at_param().action_.set(widget::action::SELECT_SCALE, false);
			wb->at_param().action_.set(widget::action::SELECT_HIGHLIGHT);
			piano_keys_[10 + o] = wb;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano_sim(utils::director<core>& d) : director_(d),
			test_ring_(nullptr), piano_keys_{ nullptr },
			overtone_{ nullptr },
			terminal_frame_(nullptr), terminal_core_(nullptr),
			abc_{ 0.0f }, gain_(0.0f),
			piano_(abc_), env_(0.0f),
			idx_(0), key_{ false } { }


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
//			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;


			{  // ボタン
				widget::param wp(vtx::irect(30, 30, 100, 40));
				widget_button::param wp_("Ring");
				test_ring_ = wd.add_widget<widget_button>(wp, wp_);
			}

			{  // 倍音スライダー
				for(int i = 0; i < 8; ++i) {
					widget::param wp(vtx::irect(30, 100 + 30 * i, 180, 20));
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
				init_piano_key_(10+170*0, 12 * 0);
				init_piano_key_(10+170*1, 12 * 1);
				init_piano_key_(10+170*2, 12 * 2);
				init_piano_key_(10+170*3, 12 * 3);
#if 0
				file_btn_->at_local_param().select_func_ = [this](int id) {
					bool f = filer_->get_state(gui::widget::state::ENABLE);
					filer_->enable(!f);
					if(!f) {
						auto& sound = director_.at().sound_;
						filer_->focus_file(sound.get_file_stream());
						files_step_ = 0;
						files_.clear();
					}
				};
#endif
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
			if(terminal_frame_) {
				terminal_frame_->load(pre);
			}


			// １２平均音階テーブルの作成
			float a = 27.5f;
			for(int i = 0; i < 12; ++i) {
				abc_[i] = a * 2.0f * 3.14159265f / static_cast<float>(sample_rate);
				a *= 1.059463094f;
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

			keys_();

			for(int i = 0; i < 48; ++i) {
				if(piano_keys_[i]->get_select_in()) {
					utils::format("%d\n") % i;
				}
			}


			al::sound& sound = director_.at().sound_;

			// Envelope
			if(test_ring_ != nullptr) {
				if(test_ring_->get_select_in()) {
					gain_ = 1.0f;
					env_ = 0.965f;
					piano_.reset();
				}
				if(test_ring_->get_select_out()) {
					++idx_;
				}
			}
			gain_ *= env_;

			// 44100 16 stereo
			uint32_t len = (44100 / 60);
			uint32_t mod = 16;
			if(sound.get_queue_audio_length() < mod) {
				len += mod;
			}

			static const uint32_t mus[] = {
				39, 41, 43, 44, 46, 48, 50, 51,
				51, 50, 48, 46, 44, 43, 41, 39,
			};
			piano_.set_key(mus[idx_]);
			piano_.render(len, gain_);

			sound.queue_audio(piano_.get_wav());

			float vol = 0.75f;
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

			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}
		}
	};

}
