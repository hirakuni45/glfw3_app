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
#include "widgets/widget_sheet.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_dialog.hpp"

#include "oscilloscope.hpp"
#include "tone.hpp"

namespace app {

	class aan_main : public utils::i_scene {

		static constexpr uint32_t VERSION = 10;
		static constexpr uint32_t SAMPLE_RATE = 48'000;
		static constexpr uint32_t SLOT_N = 16;
		static constexpr uint32_t CAP_BUFFER_N = SAMPLE_RATE * 1;  // 1 sec.

		utils::director<core>&	director_;

		gui::widget_dialog*		about_dialog_;
		gui::widget_button*		about_;

		gui::widget_label*		output_device_;
		gui::widget_check*		tone_ena_;
		gui::widget_list*		tone_type_;
		gui::widget_list*		tone_ch_;
		static constexpr uint32_t FREQ_TABLE[] = { 50, 100, 250, 500, 1000, 2000, 4000, 5000, 7500, 10000, 12000 };
		gui::widget_list*		tone_freq_;
		gui::widget_slider*		tone_vol_;
		gui::widget_spinbox*	tone_vol_step_;

		gui::widget_label*		input_device_;
		gui::widget_check*		input_ena_;

		gui::widget_sheet*		oscillo_ctrl_;

		static constexpr float VOLT_SCALE_TABLE[] = { 0.025f, 0.05f, 0.1f, 0.25f, 0.5f, 1.0f, 1.5f, 2.0f };
		gui::widget_check*		ch0_ena_;
		gui::widget_spinbox*	ch0_pos_;
		gui::widget_list*		ch0_volt_;
		gui::widget_check*		ch1_ena_;
		gui::widget_spinbox*	ch1_pos_;
		gui::widget_list*		ch1_volt_;

		static constexpr float TIME_SCALE_TABLE[] = { 10e-6f, 50e-6f, 100e-6f, 200e-6f, 500e-6f, 1e-3f, 2e-3f, 5e-3f, 10e-3f, 50e-3f, 100e-3f };
		gui::widget_list*		time_scale_;

		gui::widget_frame*		wave_frame_;
		gui::widget_view*		wave_view_;
		vtx::spos				wave_size_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		typedef utils::tone<SAMPLE_RATE, SLOT_N> TONE;
		TONE					tone_;

		typedef view::oscilloscope<int16_t, CAP_BUFFER_N, 2> WAVES;
		WAVES					waves_;

		uint32_t				cap_position_;
		uint32_t				cap_total_;

		bool					info_;

		void create_freq_list_(utils::strings& ss)
		{
			for(auto t : FREQ_TABLE) {
				if(t >= 1000) {
					ss.push_back((boost::format("%2.1fKHz") % (static_cast<float>(t) / 1000.0f)).str());
				} else {
					ss.push_back((boost::format("%dHz") % t).str());
				}
			}
		}

		void create_time_scale_list_(utils::strings& ss)
		{
			for(auto t : TIME_SCALE_TABLE) {
				if(t < 1e-3f) {
					ss.push_back((boost::format("%duS") % static_cast<int>(std::round(t * 1e6f))).str());
				} else if(t >= 1e-3f && t < 1.0f) {
					ss.push_back((boost::format("%dmS") % static_cast<int>(std::round(t * 1e3f))).str());
				}
			}
		}

		void create_volt_scale_list_(utils::strings& ss)
		{
			for(auto t : VOLT_SCALE_TABLE) {
				if(t < 0.1f) {
					ss.push_back((boost::format("%dmV") % static_cast<int>(std::round(t * 1e3f))).str());
				} else {
					ss.push_back((boost::format("%3.2fV") % t).str());
				}
			}
		}

		void ring_(uint32_t slot)
		{
			auto type = static_cast<TONE::WAVE_TYPE>(tone_type_->get_select_pos() + 1);
			auto ch = static_cast<TONE::CHANNEL>(tone_ch_->get_select_pos());
			auto freq = FREQ_TABLE[tone_freq_->get_select_pos()];
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
			if(input_ena_->get_check()) {
				waves_.at_param(0).update_ = true;
				waves_.at_param(1).update_ = true;
			}

			waves_.render(clip.size);

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
			about_dialog_(nullptr), about_(nullptr),
			output_device_(nullptr), tone_ena_(nullptr),
			tone_type_(nullptr), tone_ch_(nullptr), tone_freq_(nullptr), tone_vol_(nullptr), tone_vol_step_(nullptr),
			input_device_(nullptr), input_ena_(nullptr),
			oscillo_ctrl_(nullptr),
			ch0_ena_(nullptr), ch0_pos_(nullptr), ch0_volt_(nullptr),
			ch1_ena_(nullptr), ch1_pos_(nullptr), ch1_volt_(nullptr),
			time_scale_(nullptr),
			wave_frame_(nullptr), wave_view_(nullptr), wave_size_(0),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			tone_(),
			waves_(),
			cap_position_(0), cap_total_(0),
			info_(false)
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
				widget::param wp(vtx::irect(300, 300, 400, 200));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				about_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				auto s = (boost::format("Audio Analizer\nVersion %d.%02d") % (VERSION / 100) % (VERSION % 100)).str();
				about_dialog_->set_text(s);
				about_dialog_->enable(false);
			}
			{
				widget::param wp(vtx::irect(10, 250, 150, 40));
				widget_button::param wp_("About");
				about_ = wd.add_widget<widget_button>(wp, wp_);
				about_->at_local_param().select_func_ = [=](uint32_t id) {
					about_dialog_->enable();
				};
			}


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
//					ring_(0);
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
//					ring_(0);
				};
				tone_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{
				widget::param wp(vtx::irect(290, 60, 100, 40), 0);
				widget_list::param wp_("");
				create_freq_list_(wp_.init_list_);
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
//					ring_(0);
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
				input_ena_ = wd.add_widget<widget_check>(wp, wp_);
				input_ena_->at_local_param().select_func_ = [=](bool f) {
					auto& sound = director_.at().sound_;
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


			{  // オシロスコープ制御板
				widget* ch0;
				{  // CH0 コントロール
					widget::param wpr(vtx::irect(20, 20, 0, 0));
					widget_null::param wpr_;
					ch0 = wd.add_widget<widget_null>(wpr, wpr_);
					{
						widget::param wp(vtx::irect(10, 20, 200, 40), ch0);
						widget_check::param wp_("CH0 (Left)");
						ch0_ena_ = wd.add_widget<widget_check>(wp, wp_);
						ch0_ena_->at_local_param().select_func_ = [=](bool f) {
							waves_.at_param(0).render_ = f;
						};
					}
					{
						widget::param wp(vtx::irect(10, 20+60, 120, 40), ch0);
						widget_list::param wp_("");
						create_volt_scale_list_(wp_.init_list_);
						wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
							waves_.at_param(0).volt_grid_ = VOLT_SCALE_TABLE[pos];
						};
						ch0_volt_ = wd.add_widget<widget_list>(wp, wp_);
					}
					{
						widget::param wp(vtx::irect(140, 20+60, 100, 40), ch0);
						widget_spinbox::param wp_(0, 0, 300);
						ch0_pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
						ch0_pos_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
							waves_.at_param(0).offset_.y = newpos * 5;
							return (boost::format("%d") % newpos).str();
						};
					}
				}

				widget* ch1;
				{  // CH1 コントロール
					widget::param wpr(vtx::irect(20, 20, 0, 0));
					widget_null::param wpr_;
					ch1 = wd.add_widget<widget_null>(wpr, wpr_);
					{
						widget::param wp(vtx::irect(10, 20, 200, 40), ch1);
						widget_check::param wp_("CH1 (Right)");
						ch1_ena_ = wd.add_widget<widget_check>(wp, wp_);
						ch1_ena_->at_local_param().select_func_ = [=](bool f) {
							waves_.at_param(1).render_ = f;
						};
					}
					{
						widget::param wp(vtx::irect(10, 20+60, 120, 40), ch1);
						widget_list::param wp_("");
						create_volt_scale_list_(wp_.init_list_);
						wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
							waves_.at_param(1).volt_grid_ = VOLT_SCALE_TABLE[pos];
						};
						ch1_volt_ = wd.add_widget<widget_list>(wp, wp_);
					}
					{
						widget::param wp(vtx::irect(140, 20+60, 100, 40), ch1);
						widget_spinbox::param wp_(0, 0, 300);
						ch1_pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
						ch1_pos_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
							waves_.at_param(1).offset_.y = newpos * 5;
							return (boost::format("%d") % newpos).str();
						};
					}
				}

				widget* time;
				{  // 時間軸コントロール
					widget::param wpr(vtx::irect(20, 20, 0, 0));
					widget_null::param wpr_;
					time = wd.add_widget<widget_null>(wpr, wpr_);
					{
						widget::param wp(vtx::irect(10, 20, 120, 40), time);
						widget_list::param wp_("");
						create_time_scale_list_(wp_.init_list_);
						wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
							auto t = TIME_SCALE_TABLE[pos];
							waves_.set_time_grid(t);
						};
						time_scale_ = wd.add_widget<widget_list>(wp, wp_);
					}
				}

				widget* measure;
				{  // 測定コントロール
					widget::param wpr(vtx::irect(20, 20, 0, 0));
					widget_null::param wpr_;
					measure = wd.add_widget<widget_null>(wpr, wpr_);

					widget::param wp(vtx::irect(20, 100, 100, 40), measure);
					widget_button::param wp_("ddd");
					wd.add_widget<widget_button>(wp, wp_);
				}
				{  // オシロスコープ・シート（メイン）
					widget::param wp(vtx::irect(10, 350, 500, 600));
					widget_sheet::param wp_;
					wp_.sheets_.emplace_back("CH0(L)",  ch0);
					wp_.sheets_.emplace_back("CH1(R)",  ch1);
					wp_.sheets_.emplace_back("Time",    time);
					wp_.sheets_.emplace_back("Measure", measure);
					oscillo_ctrl_ = wd.add_widget<widget_sheet>(wp, wp_);
				}
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

			waves_.at_units(0).resize(CAP_BUFFER_N);
			waves_.at_param(0).color_.set(255, 255, 255);
			waves_.at_units(1).resize(CAP_BUFFER_N);
			waves_.at_param(1).color_.set(255, 32, 32);

			info_ = true;

			waves_.build_sin(0, SAMPLE_RATE, 1000.0, 1.0);
			waves_.build_sin(1, SAMPLE_RATE,  500.0, 0.75);
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

			if(info_) {
				{
					terminal_core_->at_terminal().output("Audio out:\n");
					auto ss = sound.at_audio_io().get_output_name();
					for(auto s : ss) {
						auto str = (boost::format("  %s\n") % s).str();
						terminal_core_->at_terminal().output(str);
					}
				}
				{
					terminal_core_->at_terminal().output("Audio inp:\n");
					auto ss = sound.at_audio_io().get_input_name();
					for(auto s : ss) {
						auto str = (boost::format("  %s\n") % s).str();
						terminal_core_->at_terminal().output(str);
					}
				}
				info_ = false;
			}

			// サンプルレートに対応するバッファ長
			uint32_t wlen = (SAMPLE_RATE / 60);
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
				al::pcm16_s_waves ws;
				if(sound.at_audio_io().get_capture(csn, ws)) {
					for(const auto w : ws) {
						waves_.at_units(0)[cap_position_] = w.l;
						waves_.at_units(1)[cap_position_] = w.r;
						++cap_position_;
						cap_position_ %= CAP_BUFFER_N;
					}
				}
			}

			static uint32_t fms = 0;
			++fms;
			if(fms >= 30) {
				fms = 0;
				if(input_ena_->get_check()) {
//					utils::format("Capture: %d\n") % cap_total_;
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
