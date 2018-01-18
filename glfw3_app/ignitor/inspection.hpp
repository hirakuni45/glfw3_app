#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター検査クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection {

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

		gui::widget_list*		fukasel_;				///< ２次負荷切替設定

		gui::widget_list*		cr_measure_freq_;		///< CR メジャー設定、周波数設定 (100, 1K, 10K)
		gui::widget_list*		cr_measure_volt_;		///< CR メジャー設定、電圧設定 (50mV, 0.5V, 1V)

		gui::widget_label*		wait_time_;				///< Wait時間設定

		gui::widget_text*		help_;					///< HELP

		struct vc_t {
			float		volt_max_;	/// 0.1V step
			float		volt_;		/// 0.1V step
			float		curt_max_;	/// 0.1A/0.01mA step
			float		curt_;		/// 0.1A/0.01mA step
		};

		std::string limit_(const std::string& str, float min, float max, const char* form)
		{
			std::string newtext = "0.0";
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

			fukasel_(nullptr),

			cr_measure_freq_(nullptr), cr_measure_volt_(nullptr),

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

			int d_w = 850;
			int d_h = 600;
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
				"オシロスコープ設定：",
				"測定項目設定：",
				"ＣＲ設定：",
				"２次負荷切替設定：",
				"リレー切替設定：",
				"Wait時間設定："
			};
			for(int i = 0; i < sizeof(tbls) / sizeof(const char*); ++i) {
				widget::param wp(vtx::irect(20, 20 + h * i, w, h), dialog_);
				widget_text::param wp_(tbls[i]);
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 検査項目名
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_;
				title_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 検査規格
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 1, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("MAX 値");
				wp_.init_list_.push_back("MIN 値");
				inspection_standards_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 検査方法
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 2, 150, 40), dialog_);
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
					widget::param wp(vtx::irect(20 + w + 180, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					voltage_[0] = wd.add_widget<widget_label>(wp, wp_);
					voltage_[0]->at_local_param().select_func_ = [this](const std::string& str) {
						voltage_[0]->set_text(limit_(str, 0.0f, 300.0f, "%2.1f"));
					};
				}
				{  // [1]動特性検査：  60V/0.1V, 30A/0.1A
					widget::param wp(vtx::irect(20 + w + 180, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					voltage_[1] = wd.add_widget<widget_label>(wp, wp_);
					voltage_[1]->at_local_param().select_func_ = [this](const std::string& str) {
						voltage_[1]->set_text(limit_(str, 0.0f, 60.0f, "%2.1f"));
					};
				}
				{
					widget::param wp(vtx::irect(20 + w + 280, 20 + h * 2, 40, 40), dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
				{  // Max: 100mA / step: 0.01mA
					widget::param wp(vtx::irect(20 + w + 330, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					current_[0] = wd.add_widget<widget_label>(wp, wp_);
					current_[0]->at_local_param().select_func_ = [this](const std::string& str) {
						current_[0]->set_text(limit_(str, 0.0f, 100.0f, "%3.2f"));
					};
				}
				{  // Max: 30A / step: 0.1A
					widget::param wp(vtx::irect(20 + w + 330, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					current_[1] = wd.add_widget<widget_label>(wp, wp_);
					current_[1]->at_local_param().select_func_ = [this](const std::string& str) {
						current_[1]->set_text(limit_(str, 0.0f, 30.0f, "%2.1f"));
					};
				}
				{
					widget::param wp(vtx::irect(20 + w + 430, 20 + h * 2, 50, 40), dialog_);
					widget_text::param wp_("mA");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					current_text_ = wd.add_widget<widget_text>(wp, wp_);
				}
			}

			{  // (6) ジェネレータ設定： 出力モード選択（矩形波/三角波/直流）
			   // タイプ（連続、単発）、
			   // 出力電圧、周波数、ON時間（レンジ：0.01ms）
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 3, 110, 40), dialog_);
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
				widget::param wp(vtx::irect(20 + w + 130, 20 + h * 3, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("連続");
				wp_.init_list_.push_back("単発");
				generator_type_ = wd.add_widget<widget_list>(wp, wp_);
				generator_type_->at_local_param().select_func_
					= [this](const std::string& str, uint32_t pos) {
				};
			}
			{  // ジェネレータ設定、周波数（1Hz to 100Hz, 1Hz/step)
				widget::param wp(vtx::irect(20 + w + 240 + 120 * 0, 20 + h * 3, 70, 40), dialog_);
				widget_label::param wp_("1", false);
				generator_freq_ = wd.add_widget<widget_label>(wp, wp_);
				generator_freq_->at_local_param().select_func_ = [this](const std::string& str) {
					generator_freq_->set_text(limit_(str, 1.0f, 100.0f, "%1.0f"));
				};
				{
					widget::param wp(vtx::irect(20 + w + 240 + 120 * 0 + 80, 20 + h * 3, 30, 40), dialog_);
					widget_text::param wp_("Hz");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、電圧（0V to 14V, 0.1V/step)
				widget::param wp(vtx::irect(20 + w + 240 + 120 * 1, 20 + h * 3, 70, 40), dialog_);
				widget_label::param wp_("0", false);
				generator_volt_ = wd.add_widget<widget_label>(wp, wp_);
				generator_volt_->at_local_param().select_func_ = [this](const std::string& str) {
					generator_volt_->set_text(limit_(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(20 + w + 240 + 120 * 1 + 80, 20 + h * 3, 30, 40), dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、DUTY（0.1% to 100%, 0.1%/step）
				widget::param wp(vtx::irect(20 + w + 240 + 120 * 2, 20 + h * 3, 70, 40), dialog_);
				widget_label::param wp_("0.1", false);
				generator_duty_ = wd.add_widget<widget_label>(wp, wp_);
				generator_duty_->at_local_param().select_func_ = [this](const std::string& str) {
					generator_duty_->set_text(limit_(str, 0.1f, 100.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(20 + w + 240 + 120 * 2 + 80, 20 + h * 3, 30, 40), dialog_);
					widget_text::param wp_("%");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}





			{  // (9) Ｃ／Ｒ選択、周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 6, 120, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back("1  KHz");
				wp_.init_list_.push_back("10 KHz");
				cr_measure_freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // (9) Ｃ／Ｒ選択、電圧設定 (50mV, 0.5V, 1V)
				widget::param wp(vtx::irect(20 + w + 10 + 130, 20 + h * 6, 120, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("50 mV");
				wp_.init_list_.push_back("0.5 V");
				wp_.init_list_.push_back("1.0 V");
				cr_measure_volt_ = wd.add_widget<widget_list>(wp, wp_); 
			}




			// (7) リレー切替設定： 各リレーＯＮ／ＯＦＦ

			{  // (8) 2次負荷切替設定： イグニションコイル、抵抗（１ｋΩ）
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 7, 250, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("イグニッションコイル");
				wp_.init_list_.push_back("抵抗（１ｋΩ）");
				fukasel_ = wd.add_widget<widget_list>(wp, wp_); 
			}

			// -(7) オシロスコープ（Ａ／Ｄ変換）設定： ＣＨ選択、電圧/電流レンジ選択、時間レンジ選択、
			// トリガー選択、フィルター選択、平均化数選択
			// -(8) 測定項目選択： （ＭＡＸ、ＭＩＮ、ＡＶＥＲＡＧＥ、ＦＡＬＬ時間、等）
			// （Ｒｔｈ、ＣＨ１×ＣＨ２、Σ（ＣＨ１×ＣＨ２）、等） … 専用項目



			{  // (10) Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 9, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				wait_time_ = wd.add_widget<widget_label>(wp, wp_);
				wait_time_->at_local_param().select_func_ = [this](const std::string& str) {
					wait_time_->set_text(limit_(str, 0.0f, 1.0f, "%3.2f"));
				};
			}

			{  // help message
				widget::param wp(vtx::irect(20, d_h - 100, d_w, 40), dialog_);
				widget_text::param wp_;
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				help_ = wd.add_widget<widget_text>(wp, wp_);
			}
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
			} else {
				help_->set_text("");
			}
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

			fukasel_->save(pre);

			cr_measure_freq_->load(pre);
			cr_measure_volt_->load(pre);

			wait_time_->save(pre);

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

				fukasel_->load(pre);

				cr_measure_freq_->load(pre);
				cr_measure_volt_->load(pre);

				wait_time_->load(pre);
			}
			return ret; 
		}
	};
}
