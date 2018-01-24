#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター検査クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "relay_map.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection {

		// 電圧/電流レンジ選択、時間レンジ選択、トリガー選択、フィルター選択、平均化選択
		struct oscillo_t {
			gui::widget_check*	ch_;		///< チャネル有効、無効
			gui::widget_list*	wave_div_;	///< 振幅軸設定（電圧、電流）

			oscillo_t() : ch_(nullptr), wave_div_(nullptr) { }

			bool load(sys::preference& pre)
			{
				int n = 0;
				if(ch_->load(pre)) ++n;
				if(wave_div_->load(pre)) ++n;
				return n == 2;
			}

			bool save(sys::preference& pre)
			{
				int n = 0;
				if(ch_->save(pre)) ++n;
				if(wave_div_->save(pre)) ++n;
				return n == 2;
			}
		};


		utils::director<core>&	director_;

		gui::widget_dialog*		dialog_;
		gui::widget_label*		title_;					///< 検査項目名
		gui::widget_list*		inspection_standards_;	///< 検査規格 (Inspection standards)
		gui::widget_list*		inspection_method_;		///< 検査方法選択
		gui::widget_label*		voltage_[2];			///< 電源設定（電圧）
		gui::widget_label*		current_[2];			///< 電源設定（電流）
		gui::widget_text*		current_text_;			///< 電源設定、電流単位
		gui::widget_list*		generator_mode_;		///< ジェネレータ設定・モード（矩形波、三角波、直流）
		gui::widget_list*		generator_type_;		///< ジェネレータ設定・タイプ（連続、単発）
		gui::widget_label*		generator_freq_;		///< ジェネレータ設定・周波数（1Hz to 100Hz / 1Hz)
		gui::widget_label*		generator_volt_;		///< ジェネレータ設定・電圧（0 to 14V / 0.1V）
		gui::widget_label*		generator_duty_;		///< ジェネレーター設定・Duty（0.1% to 100% / 0.1%）

		gui::widget_list*		oscillo_secdiv_;		///< オシロスコープ設定、時間（周期）
		gui::widget_list*		oscillo_trg_ch_;		///< オシロスコープ設定、トリガー・チャネル選択
		gui::widget_list*		oscillo_trg_slope_;		///< オシロスコープ設定、トリガー・スロープ選択
		gui::widget_spinbox*	oscillo_trg_window_;	///< オシロスコープ設定、トリガー・ウィンドウ
		gui::widget_label*		oscillo_trg_level_;		///< オシロスコープ設定、トリガー・レベル
		oscillo_t				oscillo_[4];			///< オシロスコープ各チャネル設定

		gui::widget_list*		fukasel_;				///< ２次負荷切替設定

		gui::widget_list*		cr_measure_freq_;		///< CR メジャー設定、周波数設定 (100, 1K, 10K)
		gui::widget_list*		cr_measure_volt_;		///< CR メジャー設定、電圧設定 (50mV, 0.5V, 1V)
		gui::widget_button*		setup_relay_;			///< リレー切り替え設定ボタン
		relay_map				relay_map_;
		gui::widget_label*		wait_time_;				///< Wait時間設定

		gui::widget_text*		help_;					///< HELP

		struct vc_t {
			float		volt_max_;	/// 0.1V step
			float		volt_;		/// 0.1V step
			float		curt_max_;	/// 0.1A/0.01mA step
			float		curt_;		/// 0.1A/0.01mA step
		};

		std::string limitf_(const std::string& str, float min, float max, const char* form)
		{
			std::string newtext;
			float v;
			if((utils::input("%f", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}

		std::string limiti_(const std::string& str, int min, int max, const char* form)
		{
			std::string newtext;
			int v;
			if((utils::input("%d", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}


		void init_oscillo_(gui::widget_director& wd, int x, int y, const char* ch, oscillo_t& out)
		{
			using namespace gui;
			{
				widget::param wp(vtx::irect(x, y, 80, 40), dialog_);
				widget_check::param wp_(ch);
				out.ch_ = wd.add_widget<widget_check>(wp, wp_);
//				out.ch_->at_local_param().select_func_
//					= [this](const std::string& str, uint32_t pos) {
//				};
			}
			{
				widget::param wp(vtx::irect(80 + 10 + x, y, 120, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("-22 dB");
				wp_.init_list_.push_back("-16 dB");
				wp_.init_list_.push_back("-10 dB");
				wp_.init_list_.push_back("- 4 dB");
				wp_.init_list_.push_back("+ 2 dB");
				wp_.init_list_.push_back("+ 8 dB");
				wp_.init_list_.push_back("+14 dB");
				wp_.init_list_.push_back("+20 dB");
				out.wave_div_ = wd.add_widget<widget_list>(wp, wp_);
			}

		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		inspection(utils::director<core>& d) : director_(d),
			dialog_(nullptr),
			title_(nullptr),
			inspection_standards_(nullptr),
			inspection_method_(nullptr),
			voltage_{ nullptr }, current_{ nullptr }, current_text_(nullptr),
			generator_mode_(nullptr), generator_type_(nullptr), generator_freq_(nullptr),
			generator_volt_(nullptr), generator_duty_(nullptr),
			oscillo_secdiv_(nullptr) ,oscillo_trg_ch_(nullptr), oscillo_trg_slope_(nullptr),
			oscillo_trg_window_(nullptr), oscillo_trg_level_(nullptr),

			fukasel_(nullptr),
			cr_measure_freq_(nullptr), cr_measure_volt_(nullptr),
			setup_relay_(nullptr), relay_map_(d),
			wait_time_(nullptr),
			help_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  タイトルの取得
			@return タイトル
		*/
		//-----------------------------------------------------------------//
		std::string get_title() const
		{
			return title_->get_text();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  タイトルの設定
			@param[in]	title	タイトル
		*/
		//-----------------------------------------------------------------//
		void set_title(const std::string& title)
		{
			title_->set_text(title);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
//			auto& core = gl::core::get_instance();
//			const auto& scs = core.get_rect().size;

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int d_w = 950;
			int d_h = 700;
			{
				widget::param wp(vtx::irect(100, 100, d_w, d_h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [this](bool ok) {
				};
			}
			int w = 220;
			int h = 45;
			static const char* tbls[] = {
				"検査項目名：",
				"検査規格：",
				"検査方法：",
				"ジェネレーター設定：",
				"オシロスコープ設定：", nullptr, nullptr, nullptr,
				"測定項目設定：",
				"ＣＲ設定：",
				"２次負荷切替設定：",
				"リレー切替設定：",
				"Wait時間設定："
			};
			for(int i = 0; i < sizeof(tbls) / sizeof(const char*); ++i) {
				widget::param wp(vtx::irect(20, 20 + h * i, w, h), dialog_);
				if(tbls[i] == nullptr) continue;
				widget_text::param wp_(tbls[i]);
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			int ofsx = w + 20;
			{  // 検査項目名
				widget::param wp(vtx::irect(ofsx, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_;
				title_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 検査規格
				widget::param wp(vtx::irect(ofsx, 20 + h * 1, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("MAX 値");
				wp_.init_list_.push_back("MIN 値");
				inspection_standards_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 検査方法
				widget::param wp(vtx::irect(ofsx, 20 + h * 2, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("静特性検査");
				wp_.init_list_.push_back("動特性検査");
				wp_.init_list_.push_back("LCR検査");
				inspection_method_ = wd.add_widget<widget_list>(wp, wp_);
				inspection_method_->at_local_param().select_func_
					= [this](const std::string& str, uint32_t pos) {
					widget_director& wd = director_.at().widget_director_;
					if(pos == 0) {
						voltage_[0]->set_stall(false);
						current_[0]->set_stall(false);

						wd.enable(voltage_[0]);
						wd.enable(current_[0]);
						wd.enable(voltage_[1], false);
						wd.enable(current_[1], false);
						current_text_->set_text("mA");
					} else if(pos == 1) {
						voltage_[1]->set_stall(false);
						current_[1]->set_stall(false);

						wd.enable(voltage_[0], false);
						wd.enable(current_[0], false);
						wd.enable(voltage_[1]);
						wd.enable(current_[1]);
						current_text_->set_text("A");
					} else if(pos == 2) {  // LCR 検査
						voltage_[0]->set_stall();
						current_[0]->set_stall();
						voltage_[1]->set_stall();
						current_[1]->set_stall();
					}
				};
				{  // [0]静特性検査： 300V/0.1V, 100mA/0.01mA
					widget::param wp(vtx::irect(ofsx + 170, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					voltage_[0] = wd.add_widget<widget_label>(wp, wp_);
					voltage_[0]->at_local_param().select_func_ = [this](const std::string& str) {
						voltage_[0]->set_text(limitf_(str, 0.0f, 300.0f, "%2.1f"));
					};
				}
				{  // [1]動特性検査：  60V/0.1V, 30A/0.1A
					widget::param wp(vtx::irect(ofsx + 170, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					voltage_[1] = wd.add_widget<widget_label>(wp, wp_);
					voltage_[1]->at_local_param().select_func_ = [this](const std::string& str) {
						voltage_[1]->set_text(limitf_(str, 0.0f, 60.0f, "%2.1f"));
					};
				}
				{
					widget::param wp(vtx::irect(ofsx + 270, 20 + h * 2, 40, 40), dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
				{  // Max: 100mA / step: 0.01mA
					widget::param wp(vtx::irect(ofsx + 320, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					current_[0] = wd.add_widget<widget_label>(wp, wp_);
					current_[0]->at_local_param().select_func_ = [this](const std::string& str) {
						current_[0]->set_text(limitf_(str, 0.0f, 100.0f, "%3.2f"));
					};
				}
				{  // Max: 30A / step: 0.1A
					widget::param wp(vtx::irect(ofsx + 320, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					current_[1] = wd.add_widget<widget_label>(wp, wp_);
					current_[1]->at_local_param().select_func_ = [this](const std::string& str) {
						current_[1]->set_text(limitf_(str, 0.0f, 30.0f, "%2.1f"));
					};
				}
				{
					widget::param wp(vtx::irect(ofsx + 420, 20 + h * 2, 50, 40), dialog_);
					widget_text::param wp_("mA");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					current_text_ = wd.add_widget<widget_text>(wp, wp_);
				}
			}

			{  // (6) ジェネレータ設定： 出力モード選択（矩形波/三角波/直流）
			   // タイプ（連続、単発）、
			   // 出力電圧、周波数、ON時間（レンジ：0.01ms）
				widget::param wp(vtx::irect(ofsx, 20 + h * 3, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("矩形波");
				wp_.init_list_.push_back("三角波");
///				wp_.init_list_.push_back("直流");
				generator_mode_ = wd.add_widget<widget_list>(wp, wp_);
				generator_mode_->at_local_param().select_func_ = [this](const std::string& str, uint32_t pos) {
					if(pos == 0) {
						generator_duty_->set_stall(false);
					} else {
						generator_duty_->set_stall();
					}
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 120, 20 + h * 3, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("連続");
				wp_.init_list_.push_back("単発");
				generator_type_ = wd.add_widget<widget_list>(wp, wp_);
				generator_type_->at_local_param().select_func_
					= [this](const std::string& str, uint32_t pos) {
				};
			}
			{  // ジェネレータ設定、周波数（1Hz to 100Hz, 1Hz/step)
				widget::param wp(vtx::irect(ofsx + 240 + 130 * 0, 20 + h * 3, 70, 40), dialog_);
				widget_label::param wp_("1", false);
				generator_freq_ = wd.add_widget<widget_label>(wp, wp_);
				generator_freq_->at_local_param().select_func_ = [this](const std::string& str) {
					generator_freq_->set_text(limitf_(str, 1.0f, 100.0f, "%1.0f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 240 + 120 * 0 + 80, 20 + h * 3, 30, 40), dialog_);
					widget_text::param wp_("Hz");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、電圧（0V to 14V, 0.1V/step)
				widget::param wp(vtx::irect(ofsx + 240 + 120 * 1, 20 + h * 3, 70, 40), dialog_);
				widget_label::param wp_("0", false);
				generator_volt_ = wd.add_widget<widget_label>(wp, wp_);
				generator_volt_->at_local_param().select_func_ = [this](const std::string& str) {
					generator_volt_->set_text(limitf_(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 240 + 120 * 1 + 80, 20 + h * 3, 30, 40), dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、DUTY（0.1% to 100%, 0.1%/step）
				widget::param wp(vtx::irect(ofsx + 240 + 120 * 2, 20 + h * 3, 70, 40), dialog_);
				widget_label::param wp_("0.1", false);
				generator_duty_ = wd.add_widget<widget_label>(wp, wp_);
				generator_duty_->at_local_param().select_func_ = [this](const std::string& str) {
					generator_duty_->set_text(limitf_(str, 0.1f, 100.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 240 + 120 * 2 + 80, 20 + h * 3, 30, 40), dialog_);
					widget_text::param wp_("%");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}

			// オシロスコープ設定
			// CH選択（1～4、math1～4･･･）
			// math1=CH1-CH3、math2=CH3-CH2、math3=CH4-CH2、math4=(CH1-CH3)×CH3
			// 電圧/電流レンジ選択、時間レンジ選択、トリガー選択、フィルター選択、平均化選択
			{  // 時間軸リスト 10K、20K、50K、100K、200K、500K、1M、2M、5M、10M、20M、50M、100M
				widget::param wp(vtx::irect(ofsx, 20 + h * 4, 220, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100us ( 10KHz)");
				wp_.init_list_.push_back(" 50us ( 20KHz)");
				wp_.init_list_.push_back(" 20us ( 50KHz)");
				wp_.init_list_.push_back(" 10us (100KHz)");
				wp_.init_list_.push_back("  5us (200KHz)");
				wp_.init_list_.push_back("  2us (500KHz)");
				wp_.init_list_.push_back("  1us (  1MHz)");
				wp_.init_list_.push_back("500ns (  2MHz)");
				wp_.init_list_.push_back("200ns (  5MHz)");
				wp_.init_list_.push_back("100ns ( 10MHz)");
				wp_.init_list_.push_back(" 50ns ( 20MHz)");
				wp_.init_list_.push_back(" 20ns ( 50MHz)");
				wp_.init_list_.push_back(" 10ns (100MHz)");
				oscillo_secdiv_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・チャネル選択
				widget::param wp(vtx::irect(ofsx + 240, 20 + h * 4, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH0");
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				oscillo_trg_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・スロープ選択
				widget::param wp(vtx::irect(ofsx + 360, 20 + h * 4, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Pos");
				wp_.init_list_.push_back("Neg");
				oscillo_trg_slope_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・ウィンドウ（１～１５）
				widget::param wp(vtx::irect(ofsx + 480, 20 + h * 4, 100, 40), dialog_);
				widget_spinbox::param wp_(1, 1, 15);
				oscillo_trg_window_ = wd.add_widget<widget_spinbox>(wp, wp_);
				oscillo_trg_window_->at_local_param().select_func_
					= [this](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(ofsx + 600, 20 + h * 4, 80, 40), dialog_);
				widget_label::param wp_("1", false);
				oscillo_trg_level_ = wd.add_widget<widget_label>(wp, wp_);
				oscillo_trg_level_->at_local_param().select_func_ = [this](const std::string& str) {
					oscillo_trg_level_->set_text(limiti_(str, 1, 65534, "%d"));
				};
			}
			init_oscillo_(wd, ofsx,       20 + h * 5, "CH0", oscillo_[0]);
			init_oscillo_(wd, ofsx + 290, 20 + h * 5, "CH1", oscillo_[1]);
			init_oscillo_(wd, ofsx,       20 + h * 6, "CH2", oscillo_[2]);
			init_oscillo_(wd, ofsx + 290, 20 + h * 6, "CH3", oscillo_[3]);




			{  // (9) Ｃ／Ｒ選択、周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(ofsx, 20 + h * 9, 120, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back("1  KHz");
				wp_.init_list_.push_back("10 KHz");
				cr_measure_freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // (9) Ｃ／Ｒ選択、電圧設定 (50mV, 0.5V, 1V)
				widget::param wp(vtx::irect(ofsx + 130, 20 + h * 9, 120, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("50 mV");
				wp_.init_list_.push_back("0.5 V");
				wp_.init_list_.push_back("1.0 V");
				cr_measure_volt_ = wd.add_widget<widget_list>(wp, wp_); 
			}

			//  測定項目設定 ： （MAX、MIN、AVERAGE、FALL時間、等）一般的なオシロスコープにある項目
			// （RTH、CH1×CH2(math1)etc、Σ（CH1×CH2）etc） … 専用項目



			{  // (8) 2次負荷切替設定： イグニションコイル、抵抗（１ｋΩ）
				widget::param wp(vtx::irect(ofsx, 20 + h * 10, 250, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("イグニッションコイル");
				wp_.init_list_.push_back("抵抗（１ｋΩ）");
				fukasel_ = wd.add_widget<widget_list>(wp, wp_); 
			}

			// (7) リレー切替設定： 各リレーＯＮ／ＯＦＦ
			{  // Max: 100mA / step: 0.01mA
				widget::param wp(vtx::irect(ofsx, 20 + h * 11, 150, 40), dialog_);
				widget_button::param wp_("リレー設定");
				setup_relay_ = wd.add_widget<widget_button>(wp, wp_);
				setup_relay_->at_local_param().select_func_ = [this](int n) {
					relay_map_.get_dialog()->enable();
				};
			}

			{  // (10) Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
				widget::param wp(vtx::irect(ofsx, 20 + h * 12, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				wait_time_ = wd.add_widget<widget_label>(wp, wp_);
				wait_time_->at_local_param().select_func_ = [this](const std::string& str) {
					wait_time_->set_text(limitf_(str, 0.0f, 1.0f, "%3.2f"));
				};
			}

			{  // help message
				widget::param wp(vtx::irect(20, d_h - 100, d_w, 40), dialog_);
				widget_text::param wp_;
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				help_ = wd.add_widget<widget_text>(wp, wp_);
			}

			// リレー・マップ・ダイアログ初期化
			relay_map_.initialize();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(voltage_[0]->get_focus()) {
				help_->set_text("0.0 to 300.0 [V], 0.1 [V] / step");
			} else if(voltage_[1]->get_focus()) {
				help_->set_text("0.0 to 60.0 [V], 0.1 [V] / step");
			} else if(current_[0]->get_focus()) {
				help_->set_text("0.0 to 100.0 [mA], 0.01 [mA] / step");
			} else if(current_[1]->get_focus()) {
				help_->set_text("0.0 to 30.0 [A], 0.1 [A] / step");
			} else if(generator_freq_->get_focus()) {
				help_->set_text("1 to 100 [Hz], 1 [Hz] / step");
			} else if(generator_volt_->get_focus()) {
				help_->set_text("0.0 to 14.0 [V], 0.1 [V] / step");
			} else if(generator_duty_->get_focus()) {
				help_->set_text("0.1 to 100.0 [%], 0.1 [%] / step");
			} else if(wait_time_->get_focus()) {
				help_->set_text("0.0 to 1.0 [秒], 0.01 [秒] / step");
			} else if(oscillo_secdiv_->get_focus()) {
				help_->set_text("サンプリング周期");
			} else if(oscillo_trg_ch_->get_focus()) {
				help_->set_text("トリガー・チャネル");
			} else if(oscillo_trg_slope_->get_focus()) {
				help_->set_text("トリガー・スロープ（Pos: 立ち上がり、Neg: 立下り）");
			} else if(oscillo_trg_window_->get_focus()) {
				help_->set_text("トリガー検知窓(幅): 1 to 15");
			} else if(oscillo_trg_level_->get_focus()) {
				help_->set_text("トリガーレベル: 1 to 65534");
			} else {
				help_->set_text("");
			}

			relay_map_.update();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path)
		{
			sys::preference pre;
			title_->save(pre);
			inspection_standards_->save(pre);
			inspection_method_->save(pre);
			voltage_[0]->save(pre);
			voltage_[1]->save(pre);
			current_[0]->save(pre);
			current_[1]->save(pre);
			generator_mode_->save(pre);
			generator_type_->save(pre);
			generator_freq_->save(pre);
			generator_volt_->save(pre);
			generator_duty_->save(pre);
			oscillo_secdiv_->save(pre);
			oscillo_trg_ch_->save(pre);
			oscillo_trg_slope_->save(pre);
			oscillo_trg_window_->save(pre);
			oscillo_trg_level_->save(pre);
			oscillo_[0].save(pre);
			oscillo_[1].save(pre);
			oscillo_[2].save(pre);
			oscillo_[3].save(pre);

			fukasel_->save(pre);

			cr_measure_freq_->load(pre);
			cr_measure_volt_->load(pre);

			wait_time_->save(pre);

			relay_map_.save(pre);

			return pre.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& path)
		{
			sys::preference pre;
			auto ret = pre.load(path);
			if(ret) {
				title_->load(pre);
				inspection_standards_->load(pre);
				inspection_method_->load(pre);
				voltage_[0]->load(pre);
				voltage_[1]->load(pre);
				current_[0]->load(pre);
				current_[1]->load(pre);
				generator_mode_->load(pre);
				generator_type_->load(pre);
				generator_freq_->load(pre);
				generator_volt_->load(pre);
				generator_duty_->load(pre);
				oscillo_secdiv_->load(pre);
				oscillo_trg_ch_->load(pre);
				oscillo_trg_slope_->load(pre);
				oscillo_trg_window_->load(pre);
				oscillo_trg_level_->load(pre);
				oscillo_[0].load(pre);
				oscillo_[1].load(pre);
				oscillo_[2].load(pre);
				oscillo_[3].load(pre);

				fukasel_->load(pre);

				cr_measure_freq_->load(pre);
				cr_measure_volt_->load(pre);

				wait_time_->load(pre);

				relay_map_.load(pre);
			}
			return ret; 
		}
	};
}
