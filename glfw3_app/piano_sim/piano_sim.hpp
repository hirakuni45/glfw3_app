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
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"
#include "piano.hpp"

namespace app {

	class piano_sim : public utils::i_scene {

		static const uint32_t sample_rate = 44100;

		utils::director<core>&	director_;

		gui::widget_button*		test_ring_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;



		class piano_t {

			const float	*abc_;

			float	frq_;
			float	angle_[8];

			al::sound::waves16 reb_;
			al::sound::waves16 wav_;

			uint32_t	reb_smp_;

		public:
			piano_t(const float* abc) : abc_(abc), frq_(0.0f), angle_{ 0.0f },
				reb_(), wav_(),
				reb_smp_(0) { }


			void set_reverb(float rt)
			{
				reb_smp_ = static_cast<float>(sample_rate) * rt;
				reb_.resize((sample_rate / 60) + reb_smp_);
			}


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

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		piano_sim(utils::director<core>& d) : director_(d),
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


			{ // ボタン
				widget::param wp(vtx::irect(30, 30, 100, 40));
				widget_button::param wp_("Ring");
				test_ring_ = wd.add_widget<widget_button>(wp, wp_);
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
