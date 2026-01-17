//=============================================================================//
/*! @file
	@brief  audio analizer main クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2025, 2026 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=============================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
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
		static constexpr int32_t  VOLT_POS_STEP = 15;

		utils::director<core>&	director_;

		gui::widget_dialog*		about_dialog_;
		gui::widget_button*		about_;
		gui::widget_check*		term_ena_;

		gui::widget_label*		output_device_;
		gui::widget_check*		tone_ena_;
		gui::widget_list*		tone_type_;
		gui::widget_list*		tone_ch_;
		static constexpr uint32_t FREQ_TABLE[] = { 50, 100, 250, 500, 1000, 2000, 4000, 5000, 7500, 10000, 12000 };
		gui::widget_list*		tone_freq_;
		gui::widget_slider*		tone_vol_;
		gui::widget_text*		tone_val_;

		gui::widget_label*		input_device_;
		gui::widget_check*		input_ena_;

		gui::widget_sheet*		oscillo_ctrl_;

		static constexpr float VOLT_SCALE_TABLE[] = { 0.025f, 0.05f, 0.1f, 0.25f, 0.5f, 1.0f, 1.5f, 2.0f };
		struct channel_t {
			gui::widget_check*		ena_;
			gui::widget_list*		volt_;
			gui::widget_check*		grd_ena_;
			gui::widget_spinbox*	pos_;
			gui::widget_check*		mes_ena_;
			gui::widget_spinbox*	mes_pos_;
			gui::widget_spinbox*	mes_len_;
			gui::widget_text*		mes_ans_;
			channel_t() noexcept :
				ena_(nullptr),
				volt_(nullptr), grd_ena_(nullptr), pos_(nullptr),
				mes_ena_(nullptr), mes_pos_(nullptr), mes_len_(nullptr),
				mes_ans_(nullptr)
				{ }
		};
		channel_t				channel_[2];

		static constexpr float TIME_SCALE_TABLE[] = { 1e-6, 5e-6, 10e-6f, 25e-6, 50e-6f, 100e-6f, 200e-6f, 500e-6f, 1e-3f, 2e-3f, 5e-3f, 10e-3f, 50e-3f, 100e-3f };
		gui::widget_list*		time_scale_;
		gui::widget_button*		trig_exec_;
		gui::widget_list*		trig_ch_;
		gui::widget_list*		trig_type_;
		gui::widget_spinbox*	trig_volt_;
		gui::widget_check*		time_mes_ena_;
		gui::widget_spinbox*	time_mes_pos_;
		gui::widget_spinbox*	time_mes_len_;
		gui::widget_text*		time_mes_ans_;
		gui::widget_spinbox*	time_offset_;

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
		uint32_t				cap_length_;
		uint32_t				trig_org_;

		bool					info_;

		void create_freq_list_(utils::strings& ss) noexcept
		{
			for(auto t : FREQ_TABLE) {
				if(t >= 1000) {
					ss.push_back((boost::format(" %2.1fKHz") % (static_cast<float>(t) / 1000.0f)).str());
				} else {
					ss.push_back((boost::format(" %dHz") % t).str());
				}
			}
		}

		std::string create_time_scale_str_(float t, bool round = true) noexcept
		{
			std::string s;
			if(t < 1e-3f) {
				if(round) {
					s = (boost::format(" %dμS") % static_cast<int>(std::round(t * 1e6f))).str();
				} else {
					s = (boost::format(" %3.1fμS") % (t * 1e6f)).str();
				}
			} else if(t >= 1e-3f && t < 1.0f) {
				if(round) {
					s = (boost::format(" %dmS") % static_cast<int>(std::round(t * 1e3f))).str();
				} else {
					s = (boost::format(" %3.2fmS") % (t * 1e3f)).str();
				}
			}
			return s;
		}

		void create_time_scale_list_(utils::strings& ss) noexcept
		{
			for(auto t : TIME_SCALE_TABLE) {
				ss.push_back(create_time_scale_str_(t));
			}
		}

		void create_volt_scale_list_(utils::strings& ss) noexcept
		{
			for(auto t : VOLT_SCALE_TABLE) {
				if(t < 0.1f) {
					ss.push_back((boost::format(" %dmV") % static_cast<int>(std::round(t * 1e3f))).str());
				} else {
					ss.push_back((boost::format(" %3.2fV") % t).str());
				}
			}
		}

		void ring_(uint32_t slot) noexcept
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

		void update_view_() noexcept
		{
		}

		void render_view_(const vtx::irect& clip) noexcept
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
//				waves_.at_param(0).update_ = true;
//				waves_.at_param(1).update_ = true;
			}

			waves_.render(clip.size);

			glEnable(GL_TEXTURE_2D);
			wave_size_ = clip.size;
		}

		void service_view_() noexcept
		{
		}

		gui::widget* setup_channel_(uint32_t chi, const std::string& chs) noexcept
		{
			using namespace gui;

			auto& wd = director_.at().widget_director_;

			auto& ch = channel_[chi];
			widget::param wpr(vtx::irect(10, 20, 0, 0));
			widget_null::param wpr_;
			widget* base = wd.add_widget<widget_null>(wpr, wpr_);
			{
				widget::param wp(vtx::irect(10, 10, 200, 40), base);
				widget_check::param wp_(chs);
				ch.ena_ = wd.add_widget<widget_check>(wp, wp_);
				ch.ena_->at_local_param().select_func_ = [=](bool f) {
					waves_.at_param(chi).render_ = f;
				};
			}
			{
				widget::param wp(vtx::irect(180, 10, 150, 40), base);
				widget_check::param wp_("Ground");
				ch.grd_ena_ = wd.add_widget<widget_check>(wp, wp_);
				ch.grd_ena_->at_local_param().select_func_ = [=](bool f) {
					waves_.at_param(chi).ground_ = f;
				};
			}
			{
				widget::param wp(vtx::irect(10, 10+50, 160, 40), base);
				widget_list::param wp_;
				create_volt_scale_list_(wp_.init_list_);
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					waves_.at_param(chi).volt_grid_ = VOLT_SCALE_TABLE[pos];
				};
				ch.volt_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(180, 10+50, 160, 40), base);
				widget_spinbox::param wp_(0, 0, wave_size_.y / VOLT_POS_STEP);
				ch.pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch.pos_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					waves_.at_param(chi).offset_.y = newpos * VOLT_POS_STEP;
					return (boost::format("%d") % newpos).str();
				};
			}
			{
				widget::param wp(vtx::irect(10, 10+50+50, 200, 40), base);
				widget_check::param wp_("Measure:");
				ch.mes_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(10, 10+50+50*2, 160, 40), base);
				widget_spinbox::param wp_(0, 0, 400);
				ch.mes_pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch.mes_pos_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					float v = VOLT_SCALE_TABLE[ch.volt_->get_select_pos()] / static_cast<float>(waves_.get_info().grid_step_);
					v *= (ch.pos_->get_select_pos() * VOLT_POS_STEP) - newpos;
					return (boost::format("%3.2fV") % v).str();
				};
			}
			{
				widget::param wp(vtx::irect(180, 10+50+50*2, 160, 40), base);
				widget_spinbox::param wp_(0, 0, 400);
				ch.mes_len_ = wd.add_widget<widget_spinbox>(wp, wp_);
				ch.mes_len_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					float v = VOLT_SCALE_TABLE[ch.volt_->get_select_pos()] / static_cast<float>(waves_.get_info().grid_step_);
					v *= (ch.pos_->get_select_pos() * VOLT_POS_STEP) - (ch.mes_pos_->get_select_pos() + newpos);
					return (boost::format("%3.2fV") % v).str();
				};
			}
			{
				widget::param wp(vtx::irect(180+10+160+10, 10+50+50*2, 100, 40), base);
				widget_text::param wp_("");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				ch.mes_ans_ = wd.add_widget<widget_text>(wp, wp_);
			}

			return base;
		}

		void trigger_func_() noexcept
		{
			auto ch = trig_ch_->get_select_pos();
			int32_t org = static_cast<int32_t>(trig_org_) - 800;
			uint32_t len = 800 * 2;
			if(trig_type_->get_select_pos() == 1) {  // auto
				auto th = waves_.scan_trigger_voltage(ch, org, len);
				uint32_t fpos = 0;
				if(waves_.find_trigger_position(ch, org, len, th, true, fpos)) {
					waves_.at_param(ch).unit_offset_ = fpos;
					waves_.at_param(ch).update_ = true;
				}
			} else if(trig_type_->get_select_pos() == 2 || trig_type_->get_select_pos() == 3) {  // positive, negative
				int16_t th = 0;
				bool edge = trig_type_->get_select_pos() == 2;
				uint32_t fpos = 0;
				if(waves_.find_trigger_position(ch, org, len, th, edge, fpos)) {
					waves_.at_param(ch).unit_offset_ = fpos;
					waves_.at_param(ch).update_ = true;
				}
			}
		}

		gui::widget* setup_time_() noexcept
		{
			using namespace gui;

			auto& wd = director_.at().widget_director_;

			widget::param wpr(vtx::irect(10, 20, 0, 0));
			widget_null::param wpr_;
			widget* base = wd.add_widget<widget_null>(wpr, wpr_);
			{
				widget::param wp(vtx::irect(0, 20, 160, 40), base);
				widget_list::param wp_;
				create_time_scale_list_(wp_.init_list_);
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					auto t = TIME_SCALE_TABLE[pos];
					waves_.set_time_grid(t);
				};
				time_scale_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 20+50, 300, 40), base);
				widget_text::param wp_("Trigger settings:");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(170*2, 20+50, 150, 40), base);
				widget_button::param wp_("Trigger");
				wp_.select_func_ = [=](uint32_t id) {
					trigger_func_();
				};
				trig_exec_ = wd.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 20+100, 160, 40), base);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH0 (Left)");
				wp_.init_list_.push_back("CH1 (Right)");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
				};
				trig_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(170, 20+100, 160, 40), base);
				widget_list::param wp_;
				wp_.init_list_.push_back("---");
				wp_.init_list_.push_back("Auto");
				wp_.init_list_.push_back("Positive ↑");
				wp_.init_list_.push_back("Negative ↓");
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
				};
				trig_type_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(170+170, 20+100, 150, 40), base);
				widget_spinbox::param wp_(0, 0, wave_size_.y - 1);
				trig_volt_ = wd.add_widget<widget_spinbox>(wp, wp_);
				trig_volt_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					waves_.at_info().trig_pos_ = newpos;
					auto chi = trig_ch_->get_select_pos();
					auto v = waves_.get_param(chi).offset_.y - newpos;
					auto pos = channel_[chi].volt_->get_select_pos();
					auto t = VOLT_SCALE_TABLE[pos] / waves_.get_info().grid_step_ * v;
					return (boost::format("%3.2fV") % t).str();
				};
			}

			{
				widget::param wp(vtx::irect(0, 20+100+50, 200, 40), base);
				widget_check::param wp_("Measure:");
				time_mes_ena_ = wd.add_widget<widget_check>(wp, wp_);
				time_mes_ena_->at_local_param().select_func_ = [=](bool f) {
				};
			}
			{
				widget::param wp(vtx::irect(0, 20+100+50*2, 160, 40), base);
				widget_spinbox::param wp_(0, 0, 800);
				time_mes_pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_mes_pos_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					float v = TIME_SCALE_TABLE[time_scale_->get_select_pos()] / static_cast<float>(waves_.get_info().grid_step_);
					v *= newpos;
					auto s = create_time_scale_str_(v, false);
					return (boost::format("%s") % s).str();
				};
			}
			{
				widget::param wp(vtx::irect(170, 20+100+50*2, 160, 40), base);
				widget_spinbox::param wp_(0, 0, 800);
				time_mes_len_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_mes_len_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					float v = TIME_SCALE_TABLE[time_scale_->get_select_pos()] / static_cast<float>(waves_.get_info().grid_step_);
					v *= newpos;
					auto s = create_time_scale_str_(v, false);
					return (boost::format("%s") % s).str();
				};
			}
			{
				widget::param wp(vtx::irect(0, 20+100+50*3, 300, 40), base);
				widget_text::param wp_("");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				time_mes_ans_ = wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 20+100+50*4, 160, 40), base);
				widget_spinbox::param wp_(-400, 0, 400);
				time_offset_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_offset_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					waves_.at_param(0).offset_.x = newpos;
					waves_.at_param(1).offset_.x = newpos;
					return (boost::format("%d") % newpos).str();
				};
			}

			return base;
		}

		void load_channel_(sys::preference& pre, int chi) noexcept
		{
			auto& ch = channel_[chi];

			if(ch.ena_ != nullptr) {
				ch.ena_->load(pre);
			}
			if(ch.volt_ != nullptr) {
				ch.volt_->load(pre);
			}
			if(ch.pos_ != nullptr) {
				ch.pos_->load(pre);
			}
			if(ch.grd_ena_ != nullptr) {
				ch.grd_ena_->load(pre);
			}
			if(ch.mes_ena_ != nullptr) {
				ch.mes_ena_->load(pre);
			}
			if(ch.mes_pos_ != nullptr) {
				ch.mes_pos_->load(pre);
			}
			if(ch.mes_len_ != nullptr) {
				ch.mes_len_->load(pre);
			}
		}

		void save_channel_(sys::preference& pre, int chi) noexcept
		{
			auto& ch = channel_[chi];

			if(ch.ena_ != nullptr) {
				ch.ena_->save(pre);
			}
			if(ch.volt_ != nullptr) {
				ch.volt_->save(pre);
			}
			if(ch.pos_ != nullptr) {
				ch.pos_->save(pre);
			}
			if(ch.grd_ena_ != nullptr) {
				ch.grd_ena_->save(pre);
			}
			if(ch.mes_ena_ != nullptr) {
				ch.mes_ena_->save(pre);
			}
			if(ch.mes_pos_ != nullptr) {
				ch.mes_pos_->save(pre);
			}
			if(ch.mes_len_ != nullptr) {
				ch.mes_len_->save(pre);
			}
		}

		void load_time_(sys::preference& pre) noexcept
		{
			if(time_scale_ != nullptr) {
				time_scale_->load(pre);
			}
			if(trig_volt_ != nullptr) {
				trig_volt_->load(pre);
			}
			if(time_mes_ena_ != nullptr) {
				time_mes_ena_->load(pre);
			}
			if(time_mes_pos_ != nullptr) {
				time_mes_pos_->load(pre);
			}
			if(time_mes_len_ != nullptr) {
				time_mes_len_->load(pre);
			}
		}

		void save_time_(sys::preference& pre) noexcept
		{
			if(time_scale_ != nullptr) {
				time_scale_->save(pre);
			}
			if(trig_volt_ != nullptr) {
				trig_volt_->save(pre);
			}
			if(time_mes_ena_ != nullptr) {
				time_mes_ena_->save(pre);
			}
			if(time_mes_pos_ != nullptr) {
				time_mes_pos_->save(pre);
			}
			if(time_mes_len_ != nullptr) {
				time_mes_len_->save(pre);
			}
		}

		void update_waves_() noexcept
		{
			for(int i = 0; i < 2; ++i) {
				auto& ch = channel_[i];
				waves_.at_info().volt_enable_[i] = ch.mes_ena_->get_check();
				waves_.at_info().volt_org_[i] = ch.mes_pos_->get_select_pos();
				waves_.at_info().volt_len_[i] = ch.mes_len_->get_select_pos();
				auto f = ch.mes_ena_->get_check();
				if(f) {
					float v = VOLT_SCALE_TABLE[ch.volt_->get_select_pos()] / waves_.get_info().grid_step_;
					v *= ch.mes_len_->get_select_pos();
					auto s = (boost::format("%3.2fV") % v).str();
					ch.mes_ans_->set_text(s);
					ch.mes_pos_->exec();
					ch.mes_len_->exec();
				}
				ch.pos_->set_select_max(wave_size_.y / VOLT_POS_STEP);
				ch.mes_pos_->set_select_max(wave_size_.y);
				ch.mes_len_->set_select_max(wave_size_.y);
				ch.mes_pos_->set_stall(!f);
				ch.mes_len_->set_stall(!f);
			}

			{
				bool f = trig_type_->get_select_pos() == 0;
				trig_exec_->set_stall(f);
				trig_volt_->set_stall(f);
				waves_.at_info().trig_enable_ = !f;
			}

			{
				auto f = time_mes_ena_->get_check();
				if(f) {
					float v = TIME_SCALE_TABLE[time_scale_->get_select_pos()] / static_cast<float>(waves_.get_info().grid_step_);
					v *= time_mes_len_->get_select_pos();
					auto s = create_time_scale_str_(v, false);
					s += (boost::format(" (%3.2fHz)") % (1.0f / v)).str();
					time_mes_ans_->set_text(s);
				}
				time_mes_pos_->set_select_max(wave_size_.x);
				time_mes_len_->set_select_max(wave_size_.x);
				time_mes_pos_->set_stall(!f);
				time_mes_len_->set_stall(!f);
				time_offset_->set_select_min(-wave_size_.x / 2);
				time_offset_->set_select_max( wave_size_.x / 2);
			}

			waves_.at_info().time_enable_ = time_mes_ena_->get_check();
			waves_.at_info().time_org_ = time_mes_pos_->get_select_pos();
			waves_.at_info().time_len_ = time_mes_len_->get_select_pos();

			trig_volt_->set_select_max(wave_size_.y - 1);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		aan_main(utils::director<core>& d) noexcept :
			director_(d),
			about_dialog_(nullptr), about_(nullptr), term_ena_(nullptr),
			output_device_(nullptr), tone_ena_(nullptr),
			tone_type_(nullptr), tone_ch_(nullptr), tone_freq_(nullptr), tone_vol_(nullptr), tone_val_(nullptr),
			input_device_(nullptr), input_ena_(nullptr),
			oscillo_ctrl_(nullptr),
			channel_{ },
			time_scale_(nullptr), trig_exec_(nullptr), trig_ch_(nullptr), trig_type_(nullptr), trig_volt_(nullptr),
			time_mes_ena_(nullptr), time_mes_pos_(nullptr), time_mes_len_(nullptr), time_mes_ans_(nullptr), time_offset_(nullptr),
			wave_frame_(nullptr), wave_view_(nullptr), wave_size_(0),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			tone_(),
			waves_(),
			cap_position_(0), cap_total_(0), cap_length_(0), trig_org_(0),
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
		void initialize() noexcept
		{
			auto& core = gl::core::get_instance();

			using namespace gui;
			auto& wd = director_.at().widget_director_;

			auto& sound = director_.at().sound_;

			{  // Info Dialog
				widget::param wp(vtx::irect(300, 300, 450, 200));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				about_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				auto s = (boost::format("Audio Analizer\nVersion %d.%02d\n") % (VERSION / 100) % (VERSION % 100)).str();
				int bid = BUILD_ID;  // 環境変数として Makefile で指定
				s += (boost::format("Build: %d\n") % bid).str();
				s += "Copyright 2025 Kunihito Hiramatu\n";
				s += "All Rights Reserved.";
				about_dialog_->set_text(s);
				about_dialog_->enable(false);
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
				widget::param wp(vtx::irect(70, 60, 110, 40), 0);
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
				widget::param wp(vtx::irect(190, 60, 110, 40), 0);
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
				widget::param wp(vtx::irect(310, 60, 130, 40), 0);
				widget_list::param wp_("");
				create_freq_list_(wp_.init_list_);
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					auto freq = FREQ_TABLE[pos];
					tone_.set_freq(0, freq);
				};
				tone_freq_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(450, 60+10, 180, 20));
				widget_slider::param wp_;
				wp_.slider_param_.grid_ = 0.1f;
				wp_.select_func_ = [=] (float lvl) {
					tone_.set_volume(0, lvl);
					auto s = (boost::format("%d%%") % static_cast<int>(lvl * 100.0f)).str();
					tone_val_->set_text(s);
				};
				tone_vol_ = wd.add_widget<widget_slider>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(640, 60, 150, 40));
				widget_text::param wp_("");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				tone_val_ = wd.add_widget<widget_text>(wp, wp_);
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
							terminal_core_->at_terminal().output("Start capture OK...\n");
						} else {
							terminal_core_->at_terminal().output("Start capture NG...\n");
						}
					} else {
						sound.at_audio_io().stop_capture();
					}
				};
			}
			{
				widget::param wp(vtx::irect(10, 240, 150, 40));
				widget_button::param wp_("About");
				about_ = wd.add_widget<widget_button>(wp, wp_);
				about_->at_local_param().select_func_ = [=](uint32_t id) {
					about_dialog_->enable();
				};
			}
			{
				widget::param wp(vtx::irect(10+160, 240, 150, 40), nullptr);
				widget_check::param wp_("Terminal");
				term_ena_ = wd.add_widget<widget_check>(wp, wp_);
				term_ena_->at_local_param().select_func_ = [=](bool f) {
					terminal_frame_->enable(f);
				};
			}


			wave_size_.set(600, 600);
			{  // オシロスコープ制御板
				widget* ch0 = setup_channel_(0, "CH0 (Left)");
				widget* ch1 = setup_channel_(1, "CH1 (Right)");
				widget* time = setup_time_();
				{  // オシロスコープ・シート（メイン）
					widget::param wp(vtx::irect(10, 300, 510, 500));
					widget_sheet::param wp_;
					wp_.sheets_.emplace_back("CH0(L)", ch0);
					wp_.sheets_.emplace_back("CH1(R)", ch1);
					wp_.sheets_.emplace_back("Time",   time);
					oscillo_ctrl_ = wd.add_widget<widget_sheet>(wp, wp_);
				}
			}

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
					terminal_frame_->enable(false);
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
			if(term_ena_ != nullptr) {
				term_ena_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			load_channel_(pre, 0);
			load_channel_(pre, 1);
			load_time_(pre);

			waves_.set_sample_rate(SAMPLE_RATE);
			waves_.set_time_grid(TIME_SCALE_TABLE[time_scale_->get_select_pos()]);
			waves_.at_param(0).volt_grid_ = VOLT_SCALE_TABLE[channel_[0].volt_->get_select_pos()];
			waves_.at_units(0).resize(CAP_BUFFER_N);
			waves_.at_param(0).color_.set(255, 255, 255);
			waves_.at_param(1).volt_grid_ = VOLT_SCALE_TABLE[channel_[1].volt_->get_select_pos()];
			waves_.at_units(1).resize(CAP_BUFFER_N);
			waves_.at_param(1).color_.set(255, 32, 32);

			info_ = true;

//			waves_.build_sin(0, SAMPLE_RATE, 1000.0, 1.0);
//			waves_.build_sin(1, SAMPLE_RATE,  500.0, 0.75);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update() noexcept
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
//			uint32_t mod = 16;
//			if(sound.get_queue_audio_length() < mod) {
//				wlen += mod;
//			}
			tone_.service(wlen, 1.0f);
			sound.queue_audio(tone_.get_audio());

			float vol = 1.0f;
			sound.set_gain_stream(vol);

			auto csn = sound.at_audio_io().num_capture();
			if(csn > 0) {
				cap_total_ += csn;
				trig_org_ = cap_position_;
				cap_length_ = csn;
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

			update_waves_();

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
		void render() noexcept
		{
			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() noexcept
		{
			auto& pre = director_.at().preference_;

			save_channel_(pre, 0);
			save_channel_(pre, 1);
			save_time_(pre);
			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}
			if(term_ena_ != nullptr) {
				term_ena_->save(pre);
			}
			if(wave_frame_ != nullptr) {
				wave_frame_->save(pre);
			}
		}
	};
}
