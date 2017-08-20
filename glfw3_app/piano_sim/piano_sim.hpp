//=====================================================================//
/*! @file
	@brief  Piano SIM クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_terminal.hpp"

namespace app {

	class piano_sim : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_button*		test_ring_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		float		angle_[8];
		float		gain_;
		float		frq_;
		float		env_;

		float		abc_[12];

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
			angle_{ 0.0f }, gain_(0.0f), frq_(0.0f), env_(0.0f),
			abc_{ 0.0f }, idx_(0), key_{ false } { }


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


			// １２平均音階の作成
			float a = 27.5f;
			for(int i = 0; i < 12; ++i) {
				abc_[i] = a;
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
#if 0
			if(sound.get_queue_audio_length() >= 1024) {
				len -= sound.get_queue_audio_length() - 1024;
			} else if(sound.get_queue_audio_length() < 512) {
				len += 512 - sound.get_queue_audio_length();
			}
#endif

			// Envelope
			if(test_ring_ != nullptr) {
				if(test_ring_->get_select_in()) {
					gain_ = 1.0f;
					env_ = 0.965f;
					for(int i = 0; i < 8; ++i) angle_[i] = 0.0f;
					
				}

				if(test_ring_->get_select_out()) {
					++idx_;
				}
			}
			gain_ *= env_;

			// 44100 16 stereo
			al::sound::waves16 tmp;
			tmp.resize(735);


			static const uint32_t mus[] = {
				39, 41, 43, 44, 46, 48, 50, 51,
				50, 48, 46, 44, 43, 41, 39,
			};

			uint32_t idx = mus[idx_];
			float oct = static_cast<float>(1 << (idx / 12));
			frq_ = abc_[idx % 12] * oct * 2.0f * 3.14159265f / 44100.0f;

			static const float sqlvt[8] = {
				1.0f, 0.6f, 0.7f, 0.6f, 0.68f, 0.68f, 0.55f, 0.55f 
			};
			static const float octave[8] = {
				1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
			};

			for(int i = 0; i < 735; ++i) {
				float l = 0.0f;
				for(int j = 0; j < 8; ++j) {
					l += sinf(angle_[j]) * sqlvt[j];
					angle_[j] += frq_ * octave[j];
				}

				l *= gain_;
				tmp[i] = static_cast<int16_t>(l * 8000.0f);
			}

			sound.queue_audio(tmp);

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
