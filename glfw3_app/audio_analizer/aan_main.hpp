//=============================================================================//
/*! @file
	@brief  audio analizer main クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=============================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_dialog.hpp"

#include "gl_fw/render_waves.hpp"

#include "tone.hpp"

namespace app {

	class aan_main : public utils::i_scene {

		static constexpr uint32_t sample_rate = 48'000;
		static constexpr uint32_t SLOT_N = 16;

		utils::director<core>&	director_;

		gui::widget_label*		output_device_;
		gui::widget_check*		tone_ena_;
		gui::widget_list*		tone_type_;
		gui::widget_list*		tone_ch_;
		gui::widget_list*		tone_freq_;
		gui::widget_slider*		tone_vol_;
		gui::widget_spinbox*	tone_vol_step_;

		gui::widget_label*		input_device_;
		gui::widget_check*		capture_ena_;

		gui::widget_frame*		wave_frame_;
		gui::widget_view*		wave_view_;
		vtx::spos				wave_size_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		typedef utils::tone<sample_rate, SLOT_N> TONE;
		TONE		tone_;

		typedef view::render_waves<uint16_t, 1024, 2> WAVES;
		WAVES		waves_;

		uint32_t	cap_total_;



		void ring_(uint32_t slot)
		{
			auto type = static_cast<TONE::WAVE_TYPE>(tone_type_->get_select_pos() + 1);
			auto ch = static_cast<TONE::CHANNEL>(tone_ch_->get_select_pos());
			static uint32_t freq_tbl[] = { 50, 100, 250, 500, 1000, 2000, 4000, 5000, 7500, 10000, 12000 };
			auto freq = freq_tbl[tone_freq_->get_select_pos()];
			if(tone_ena_->get_check()) {
				tone_.ring(slot, type, ch, freq);
			} else {
				tone_.stop(slot);
			}
		}


		void update_view_()
		{

		}


		void render_view_(const vtx::irect& clip)
		{
			glDisable(GL_TEXTURE_2D);
#if 0
			if(info_in_ && wave_view_->get_select()) {
				vtx::sposs r;
				r.emplace_back(info_org_.x, info_org_.y);
				auto msp = wave_view_->get_param().in_point_;
				r.emplace_back(msp.x, info_org_.y);
				r.emplace_back(msp.x, msp.y);
				r.emplace_back(info_org_.x, msp.y);
				gl::glColor(img::rgba8(255, 255));
				gl::draw_line_loop(r);
			}

			uint32_t n = 0;
			if(time_.scale_ != nullptr) {
				n = time_.scale_->get_select_pos();
			}
#endif
//			waves_.render(clip.size, sample_param_.rate, get_time_unit_(n));
			waves_.render(clip.size, 65536);

			glEnable(GL_TEXTURE_2D);
			wave_size_ = clip.size;
		}


		void service_view_()
		{
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		aan_main(utils::director<core>& d) :
			director_(d),
			output_device_(nullptr), tone_ena_(nullptr),
			tone_type_(nullptr), tone_ch_(nullptr), tone_freq_(nullptr), tone_vol_(nullptr), tone_vol_step_(nullptr),
			input_device_(nullptr), capture_ena_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			tone_(),
			waves_(),
			cap_total_(0)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~aan_main() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			auto& core = gl::core::get_instance();

			using namespace gui;
			auto& wd = director_.at().widget_director_;

			auto& sound = director_.at().sound_;

			{
				widget::param wp(vtx::irect(10, 10, 650, 40), nullptr);
				widget_label::param wp_(sound.at_audio_io().get_def_output_name());
				output_device_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(10, 60, 50, 40), nullptr);
				widget_check::param wp_("0");
				tone_ena_ = wd.add_widget<widget_check>(wp, wp_);
				tone_ena_->at_local_param().select_func_ = [=](bool f) {
					ring_(0);
				};
			}

			{
				widget::param wp(vtx::irect(70, 60, 100, 40), 0);
				widget_list::param wp_("+SIN");
				wp_.init_list_.push_back("+SIN");
				wp_.init_list_.push_back("-SIN");
				wp_.init_list_.push_back("+COS");
				wp_.init_list_.push_back("-COS");
				wp_.init_list_.push_back("+TRI");
				wp_.init_list_.push_back("-TRI");
				wp_.init_list_.push_back("+SAW");
				wp_.init_list_.push_back("-SAW");
				wp_.init_list_.push_back("+SQR");
				wp_.init_list_.push_back("-SQR");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					ring_(0);
				};
				tone_type_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{
				widget::param wp(vtx::irect(180, 60, 100, 40), 0);
				widget_list::param wp_("L+R");
				wp_.init_list_.push_back("L+R");
				wp_.init_list_.push_back("LEFT");
				wp_.init_list_.push_back("RIGHT");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					ring_(0);
				};
				tone_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{
				widget::param wp(vtx::irect(290, 60, 100, 40), 0);
				widget_list::param wp_("50");
				wp_.init_list_.push_back("50");
				wp_.init_list_.push_back("100");
				wp_.init_list_.push_back("250");
				wp_.init_list_.push_back("500");
				wp_.init_list_.push_back("1K");
				wp_.init_list_.push_back("2K");
				wp_.init_list_.push_back("4K");
				wp_.init_list_.push_back("5K");
				wp_.init_list_.push_back("7.5K");
				wp_.init_list_.push_back("10K");
				wp_.init_list_.push_back("12K");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					ring_(0);
				};
				tone_freq_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(400, 60+10, 180, 20));
				widget_slider::param wp_;
				wp_.slider_param_.grid_ = 0.1f;
				wp_.select_func_ = [=] (float lvl) {
					tone_.set_volume(0, lvl);
					tone_vol_step_->set_select_pos(static_cast<int>(lvl * 10.0f));
					tone_.set_volume(0, lvl);
				};
				tone_vol_ = wd.add_widget<widget_slider>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(590, 60, 150, 40));
				widget_spinbox::param wp_(0, 0, 10);
				tone_vol_step_ = wd.add_widget<widget_spinbox>(wp, wp_);
				tone_vol_step_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
//					if(st == widget_spinbox::state::initial) std::cout << "initial";
//					else std::cout << "select";
					if(st == widget_spinbox::state::inc || st == widget_spinbox::state::dec) {
						tone_vol_->at_position() = static_cast<float>(newpos) / 10.0f;
						tone_.set_volume(0, tone_vol_->get_position());
					}
					auto pos = newpos * 10;
					return (boost::format("%d %%") % pos).str();
				};
			}


			{
				widget::param wp(vtx::irect(10, 120, 650, 40), nullptr);
				widget_label::param wp_(sound.at_audio_io().get_def_input_name());
				input_device_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(10, 180, 150, 40));
				widget_check::param wp_("Capture");
				capture_ena_ = wd.add_widget<widget_check>(wp, wp_);
				capture_ena_->at_local_param().select_func_ = [=](bool f) {
					auto& sound = director_.at().sound_;
					input_device_->set_stall(f);
					if(f) {
						auto ret = sound.at_audio_io().start_capture();
						if(ret) {
							utils::format("Start capture OK...\n");
						} else {
							utils::format("Start capture NG...\n");
						}
					} else {
						sound.at_audio_io().stop_capture();
					}
				};
			}


			wave_size_.set(400, 400);
			{  // 波形描画フレーム
				widget::param wp(vtx::irect(610, 5, wave_size_.x, wave_size_.y));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				wp_.color_param_.fore_color_ = img::rgba8(65, 100, 150);
				wp_.color_param_.back_color_ = wp_.color_param_.fore_color_ * 0.7f;
				wave_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{  // 波形描画ビュー 
				widget::param wp(vtx::irect(0), wave_frame_);
				widget_view::param wp_;
				wp_.update_func_ = [=]() {
					update_view_();
				};
				wp_.render_func_ = [=](const vtx::irect& clip) {
					render_view_(clip);
				};
				wp_.service_func_ = [=]() {
					service_view_();
				};
				wave_view_ = wd.add_widget<widget_view>(wp, wp_);
			}



			{	// ターミナルの作成
				{
					widget::param wp(vtx::irect(10, 10, 400, 300));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(20);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
				}
			}

			// プリファレンスの取得
			auto& pre = director_.at().preference_;
			if(wave_frame_ != nullptr) {
				wave_frame_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			auto& wd = director_.at().widget_director_;

			auto& sound = director_.at().sound_;

			// サンプルレートに対応するバッファ長
			uint32_t wlen = (sample_rate / 60);
			uint32_t mod = 16;
			if(sound.get_queue_audio_length() < mod) {
				wlen += mod;
			}

			tone_.service(wlen, 1.0f);
			sound.queue_audio(tone_.get_audio());

			float vol = 1.0f;
			sound.set_gain_stream(vol);

			auto csn = sound.at_audio_io().num_capture();
			if(csn > 0) {
				cap_total_ += csn;
				al::pcm16_s_waves w;
				if(sound.at_audio_io().get_capture(csn, w)) {

				}
			}

			static uint32_t fms = 0;
			++fms;
			if(fms >= 30) {
				fms = 0;
				if(capture_ena_->get_check()) {
					utils::format("Capture: %d\n") % cap_total_;
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
			auto& pre = director_.at().preference_;

			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}
			if(wave_frame_ != nullptr) {
				wave_frame_->save(pre);
			}
		}
	};
}
