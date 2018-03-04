#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター単体検査クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "utils/select_file.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_chip.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "relay_map.hpp"
#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "wave_cap.hpp"
#include "test.hpp"
#include "kikusui.hpp"

// DC2 を菊水電源で置き換える場合有効にする
#define DC2_KIKUSUI

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  単体検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection {

		static std::string limitf_(const std::string& str, float min, float max, const char* form)
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


		static std::string limiti_(const std::string& str, int min, int max, const char* form)
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

	public:
		//=================================================================//
		/*!
			@brief  テストパラメータ、構造体
		*/
		//=================================================================//
		struct test_param {
			utils::director<core>&	director_;

			gui::widget_label*		symbol_;	///< 検査記号
			gui::widget_spinbox*	retry_;		///< リトライ回数
			gui::widget_label*		wait_;		///< 検査遅延時間設定
			gui::widget_list*		term_;		///< 検査端子設定
			gui::widget_label*		delay_;		///< 検査信号遅延時間
			gui::widget_list*		filter_;	///< 検査信号フィルター
			gui::widget_label*		width_;		///< 検査信号取得幅
			gui::widget_label*		min_;		///< 検査最小値
			gui::widget_label*		max_;		///< 検査最大値

			test::value_t			value_;

			test_param(utils::director<core>& d) : director_(d),
				symbol_(nullptr), retry_(nullptr), wait_(nullptr),
				term_(nullptr), delay_(nullptr), filter_(nullptr),
				width_(nullptr), min_(nullptr), max_(nullptr), value_()
			{ }


			void build_value()
			{
				value_.symbol_ = symbol_->get_text();
				value_.retry_ = retry_->get_select_pos();
				{
					int n = 0;
					utils::string_to_int(wait_->get_text(), n);
					value_.wait_ = n;
				}
				value_.term_ = term_->get_select_pos();
				{
					double a = 0.0f;
					utils::string_to_double(delay_->get_text(), a);
					value_.delay_ = a;
				}
				value_.filter_ = filter_->get_select_pos();
				{
					double a = 0.0f;
					utils::string_to_double(width_->get_text(), a);
					value_.width_ = a;
				}
				{
					double a = 0.0f;
					utils::string_to_double(min_->get_text(), a);
					value_.min_ = a;
				}
				{
					double a = 0.0f;
					utils::string_to_double(max_->get_text(), a);
					value_.max_ = a;
				}
			}


			void init(gui::widget* root, int d_w, int ofsx, int h, int loc)
			{
				using namespace gui;
				widget_director& wd = director_.at().widget_director_;
				{  // 検査記号
					widget::param wp(vtx::irect(ofsx, 20 + h * 12, 90, 40), root);
					widget_label::param wp_("", false);
					symbol_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // リトライ回数
					widget::param wp(vtx::irect(ofsx, 20 + h * 13, 90, 40), root);
					widget_spinbox::param wp_(1, 1, 5);
					retry_ = wd.add_widget<widget_spinbox>(wp, wp_);
					retry_->at_local_param().select_func_ =
						[=](widget_spinbox::state st, int before, int newpos) {
						return (boost::format("%d") % newpos).str();
					};
				}
				{  // Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
					widget::param wp(vtx::irect(ofsx + 100, 20 + h * 13, 90, 40), root);
					widget_label::param wp_("0", false);
					wait_ = wd.add_widget<widget_label>(wp, wp_);
					wait_->at_local_param().select_func_ = [=](const std::string& str) {
						wait_->set_text(limitf_(str, 0.0f, 1.0f, "%3.2f"));
					};
				}
				{  // 計測対象選択
					widget::param wp(vtx::irect(ofsx + 200, 20 + h * 13, 90, 40), root);
					widget_list::param wp_;
					wp_.init_list_.push_back("CH1");
					wp_.init_list_.push_back("CH2");
					wp_.init_list_.push_back("CH3");
					wp_.init_list_.push_back("CH4");
					wp_.init_list_.push_back("DC2");
					wp_.init_list_.push_back("CRM");
					term_ = wd.add_widget<widget_list>(wp, wp_);
				}
				{  // テスト 信号計測ポイント（時間）
					widget::param wp(vtx::irect(ofsx + 300, 20 + h * 13, 90, 40), root);
					widget_label::param wp_("0", false);
					delay_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // 計測信号フィルター
					widget::param wp(vtx::irect(ofsx + 400, 20 + h * 13, 90, 40), root);
					widget_list::param wp_;
					wp_.init_list_.push_back("SIG");
					wp_.init_list_.push_back("MIN");
					wp_.init_list_.push_back("MAX");
					wp_.init_list_.push_back("AVE");
					filter_ = wd.add_widget<widget_list>(wp, wp_);
					filter_->at_local_param().select_func_
						= [=](const std::string& text, uint32_t pos) {
						if(pos == 0) {
							width_->set_stall();
						} else {
							width_->set_stall(false);
						}
					};
				}
				{  // テスト 信号計測ポイント（時間）
					widget::param wp(vtx::irect(ofsx + 500, 20 + h * 13, 90, 40), root);
					widget_label::param wp_("0", false);
					width_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // テスト MIN 値設定
					widget::param wp(vtx::irect(ofsx + 600, 20 + h * 13, 90, 40), root);
					widget_label::param wp_("0", false);
					min_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // テスト MAX 値設定
					widget::param wp(vtx::irect(ofsx + 700, 20 + h * 13, 90, 40), root);
					widget_label::param wp_("0", false);
					max_ = wd.add_widget<widget_label>(wp, wp_);
				}
			} 


			void save(sys::preference& pre)
			{
				symbol_->save(pre);
				retry_->save(pre);
				wait_->save(pre);
				term_->save(pre);
				delay_->save(pre);
				filter_->save(pre);
				width_->save(pre);
				min_->save(pre);
				max_->save(pre);
			}


			void load(sys::preference& pre)
			{
				symbol_->load(pre);
				retry_->load(pre);
				wait_->load(pre);
				term_->load(pre);
				delay_->load(pre);
				filter_->load(pre);
				width_->load(pre);
				min_->load(pre);
				max_->load(pre);
			}
		};

	private:
		static constexpr const char* UNIT_EXT_ = "unt";  ///< 単体検査ファイル、拡張子

		utils::director<core>&	director_;

		net::ign_client_tcp&	client_;

		interlock&				interlock_;
		wave_cap&				wave_cap_;
		kikusui&				kikusui_;

		gui::widget_dialog*		dialog_;
		gui::widget_label*		unit_name_;			///< 単体試験名
		gui::widget_button*		load_file_;			///< load file
		gui::widget_button*		save_file_;			///< save file
		gui::widget_check*		ilock_enable_;		///< Interlock 許可／不許可
		utils::select_file		unit_load_filer_;
		utils::select_file		unit_save_filer_;

		// DC1 設定
		gui::widget_check*		dc1_sw_[5];		///< DC1 接続スイッチ
		gui::widget_check*		dc1_ena_;		///< DC1 有効、無効
		gui::widget_list*		dc1_mode_;		///< DC1 電流、電圧モード
		gui::widget_label*		dc1_voltage_;	///< DC1（電圧）
		gui::widget_label*		dc1_current_;	///< DC1（電流）
		gui::widget_spinbox*	dc1_count_;		///< DC1 熱抵抗測定回数
		gui::widget_button*		dc1_exec_;		///< DC1 設定転送

		// DC2 設定
		gui::widget_check*		dc2_sw_[14];	///< DC2 接続スイッチ
		gui::widget_check*		dc2_ena_;		///< DC2 有効、無効
		gui::widget_list*		dc2_mode_;		///< DC2 電流、電圧モード
		gui::widget_label*		dc2_voltage_;	///< DC2（電圧）
		gui::widget_label*		dc2_current_;	///< DC2（電流）
		gui::widget_label*		dc2_probe_;		///< DC2（電流、電圧測定値）
		gui::widget_button*		dc2_exec_;		///< DC2 設定転送
		bool					dc2_probe_mode_;
		uint32_t				dc2_curr_id_;
		uint32_t				dc2_volt_id_;

		// WGM 設定
		gui::widget_check*		gen_sw_[5];	///< ジェネレータ接続スイッチ
		gui::widget_check*		gen_ena_;	///< ジェネレータ有効、無効
		gui::widget_list*		gen_mode_;	///< ジェネレータモード（矩形波、三角波、直流）
		gui::widget_label*		gen_freq_;	///< ジェネレータ設定・周波数（1Hz to 100Hz / 1Hz)
		gui::widget_label*		gen_volt_;	///< ジェネレータ設定・電圧（0 to 14V / 0.1V）
		gui::widget_label*		gen_duty_;	///< ジェネレーター設定・Duty（0.1% to 100% / 0.1%）
		gui::widget_check*		gen_iena_;	///< ジェネレータ内臓電源、有効、無効
		gui::widget_label*		gen_ivolt_;	///< ジェネレータ設定・内臓電源
		gui::widget_button*		gen_exec_;	///< ジェネレーター設定転送

		// CRM 設定
		gui::widget_check*		crm_sw_[14];	///< CRM 接続スイッチ
		gui::widget_check*		crm_ena_;		///< CRM 有効、無効
		gui::widget_list*		crm_amps_;		///< CRM 電流レンジ切り替え
		gui::widget_list*		crm_freq_;		///< CRM 周波数（100Hz, 1KHz, 10KHz）
		gui::widget_list*		crm_mode_;		///< CRM 抵抗測定、容量測定
		gui::widget_label*		crm_ans_;		///< CRM 測定結果 
		gui::widget_button*		crm_exec_;		///< CRM 設定転送

		// ICM 設定
		gui::widget_check*		icm_sw_[6];		///< ICM 接続スイッチ
		gui::widget_button*		icm_exec_;		///< ICM 設定転送

		// WDM 設定
		gui::widget_check*		wdm_sw_[4];		///< WDM 接続スイッチ
		gui::widget_list*		wdm_smpl_;		///< WDM サンプリング周期
		gui::widget_list*		wdm_ch_;		///< WDM トリガーチャネル
		gui::widget_list*		wdm_slope_;		///< WDM スロープ
		gui::widget_spinbox*	wdm_window_;	///< WDM トリガー領域
		gui::widget_label*		wdm_level_;		///< WDM トリガー・レベル
		gui::widget_check*		wdm_cnv_;		///< WDM トリガー・レベル変換
		gui::widget_label*		wdm_level_va_;	///< WDM トリガー・レベル（電圧、電流）
		gui::widget_list*		wdm_gain_[4];	///< WDM チャネル・ゲイン
		gui::widget_list*		wdm_ie_trg_;	///< WDM 内部、外部トリガ
		gui::widget_list*		wdm_sm_trg_;	///< WDM Single, Manual トリガ
		gui::widget_button*		wdm_exec_;		///< WDM 設定転送
		wave_cap::sample_param	sample_param_;

		// 測定用件
		test_param				test_;

		gui::widget_chip*		chip_;			///< help chip

		bool					startup_init_;
		uint32_t				crrd_id_;
		uint32_t				crcd_id_;
		uint32_t				d2md_id_;

		double					crrd_value_;
		double					crcd_value_;

		struct dc1_t {
			uint32_t	sw;		///< 5 bits
			uint32_t	delay;	///< SW オンからコマンド発行までの遅延（10ms単位）
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits
			bool		thermal;	///< 熱抵抗モード
			uint32_t	count;	///< 熱抵抗測定回数（最大５００）

			dc1_t() : sw(0), delay(1), ena(0), mode(0), volt(0), curr(0),
				thermal(false), count(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("dc1 D1SW%02X\n") % sw).str();
				if(thermal) {
					s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
					s += (boost::format("dc1 D1TT%03X\n") % (count & 0x1ff)).str();
				} else {
					if(ena) {
						s += (boost::format("delay %d\n") % delay).str();
						s += (boost::format("dc1 D1MD%d\n") % mode).str();
						if(mode != 0) {
							s += (boost::format("dc1 D1VS%05X\n") % (volt & 0xfffff)).str();
						} else {
							s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
						}
					}
					s += (boost::format("dc1 D1OE%d\n") % ena).str();
				}			
				return s;
			}
		};


		struct dc2_t {
			uint32_t	sw;		///< 14 bits
			uint32_t	delay;	///< SW オンからコマンド発行までの遅延（10ms単位）
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits

			dc2_t() : sw(0), ena(0), mode(0), volt(0), curr(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("dc2 D2SW%04X\n") % sw).str();
#ifndef DC2_KIKUSUI
				if(ena) {
					s += (boost::format("delay %d\n") % delay).str();
					s += (boost::format("dc2 D2MD%d\n") % mode).str();
					if(mode != 0) {
						s += (boost::format("dc2 D2VS%05X\n") % (volt & 0xfffff)).str();
					} else {
						s += (boost::format("dc2 D2IS%05X\n") % (curr & 0xfffff)).str();
					}
				}
				s += (boost::format("dc2 D2OE%d\n") % ena).str();
				if(ena) {
					s += "delay 1\n";
					s += "dc2 D2M?\n";
				}
#endif
				return s;
			}
		};


		struct wgm_t {
			uint16_t	sw;		///< 5 bits
			bool		ena;	///< 0, 1
			bool		type;	///< 0, 1
			uint16_t   	frq;	///< 7 bits
			uint16_t	duty;	///< 10 bits
			uint16_t	volt;	///< 10 bits
			bool		iena;	///< 内臓電源
			uint32_t	ivolt;	///< 内臓電源電圧

			wgm_t() : ena(0), type(0), frq(0), duty(0), volt(0), iena(false), ivolt(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("wgm WGSW%02X\n") % sw).str();

				s += (boost::format("wgm WGSP%d\n") % type).str();
				s += (boost::format("wgm WGFQ%02X\n") % (frq & 0x7f)).str();
				s += (boost::format("wgm WGPW%03X\n") % (duty & 0x3ff)).str();
				s += (boost::format("wgm WGPV%03X\n") % (volt & 0x3ff)).str();

				s += (boost::format("wgm WGOE%d\n") % ena).str();
				if(iena) {
					s += (boost::format("wgm WGDV%05X\n") % (ivolt & 0xfffff)).str();
				}
				s += (boost::format("wgm WGDE%d\n") % iena).str();
				return s;
			}
		};


		struct crm_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			uint16_t	amps;	///< 0, 1, 2
			uint16_t	freq;	///< 0, 1, 2
			uint16_t	mode;	///< 0, 1

			crm_t() : sw(0), ena(0), amps(0), freq(0), mode(0) { }

			std::string build() const
			{
				std::string s;
				if(ena) {
					s += (boost::format("crm CRSW%04X\n") % sw).str();
					s += (boost::format("crm CRIS%d\n") % (amps + 1)).str();
					static const char* frqtbl[3] = { "001", "010", "100" };
					s += (boost::format("crm CRFQ%s\n") % frqtbl[freq % 3]).str();
				}
				s += (boost::format("crm CROE%d\n") % ena).str();
				if(ena) {
					s += "delay 1\n";
					if(mode) {
						s += (boost::format("crm CRC?1\n")).str();
					} else {
						s += (boost::format("crm CRR?1\n")).str();
					}
				}
				return s;
			}
		};


		struct icm_t {
			uint16_t	sw;		///< 6 bits

			icm_t() : sw(0) { }

			std::string build() const
			{
				std::string s;
				s = (boost::format("icm ICSW%02X\n") % sw).str();
				return s;
			}
		};


		struct vc_t {
			float		volt_max_;	/// 0.1V step
			float		volt_;		/// 0.1V step
			float		curt_max_;	/// 0.1A/0.01mA step
			float		curt_;		/// 0.1A/0.01mA step
		};


		static const uint32_t time_div_size_ = 16;
		double get_time_div_() const {
			if(wdm_smpl_ == nullptr) return 0.0;

			static constexpr double tbls[time_div_size_] = {
				1.0 / 1e3,    //  0
				1.0 / 2e3,    //  1
				1.0 / 5e3,    //  2
				1.0 / 10e3,   //  3
				1.0 / 20e3,   //  4
				1.0 / 50e3,   //  5
				1.0 / 100e3,  //  6
				1.0 / 200e3,  //  7
				1.0 / 500e3,  //  8
				1.0 / 1e6,    //  9
				1.0 / 2e6,    // 10
				1.0 / 5e6,    // 11
				1.0 / 10e6,   // 12
				1.0 / 20e6,   // 13
				1.0 / 50e6,   // 14
				1.0 / 100e6   // 15
			};
			return tbls[wdm_smpl_->get_select_pos() % time_div_size_];
		}


		static const uint32_t gain_rate_size_ = 8;
		double get_gain_rate_(uint32_t ch) const {
			if(wdm_gain_[ch] == nullptr) return 0.0f;
			auto n = wdm_gain_[ch]->get_select_pos() % gain_rate_size_;
			static constexpr float tbls[gain_rate_size_] = {
				0.0625f,
				0.125f,
				0.25f,
				0.5f,
				1.0f,
				2.0f,
				4.0f,
				8.0f
			};
			return tbls[n];
		}


		interlock::module get_module_(int swn)
		{
			interlock::module md = interlock::module::N;
			switch(swn) {
			case 1:
				md = interlock::module::CRM;
				break;
			case 15:
				md = interlock::module::DC2;
				break;
			case 29:
				md = interlock::module::WDM;
				break;
			case 34:
				md = interlock::module::ICM;
				break;
			case 40:
				md = interlock::module::DC1;
				break;
			case 44:
				md = interlock::module::WGM;
				break;
			default:
				break;
			}
			return md;
		}


		void init_sw_(int ofsx, int h, int loc, gui::widget_check* out[], int num, int swn)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;			
			auto md = get_module_(swn);
			for(int i = 0; i < num; ++i) {
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 60, 40), dialog_);
				widget_check::param wp_((boost::format("%d") % swn).str());
				out[i] = wd.add_widget<widget_check>(wp, wp_);
				ofsx += 60;
				interlock_.install(md, static_cast<interlock::swtype>(swn), out[i]);
				++swn;
			}
		}


		// DC1 専用
		void init_sw_dc1_(int ofsx, int h, int loc, gui::widget_check* out[], int num,
			const char* swt[])
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;			
			auto md = interlock::module::DC1;
			for(int i = 0; i < num; ++i) {
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 60, 40), dialog_);
				widget_check::param wp_(swt[i]);
				out[i] = wd.add_widget<widget_check>(wp, wp_);
				ofsx += 60;
				int swn;
				if((utils::input("%d", swt[i]) % swn).status()) {
					interlock_.install(md, static_cast<interlock::swtype>(swn), out[i]);
				}
			}
		}


		void init_dc1_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			static const char* swt[] = {
				"40", "41", "42", "43", "49"
			};
			init_sw_dc1_(ofsx, h, loc, dc1_sw_, 5, swt);
			++loc;
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				dc1_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("定電流");
				wp_.init_list_.push_back("定電圧");
				wp_.init_list_.push_back("熱抵抗");
				dc1_mode_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 60V/0.1V, 30A/10mA
				widget::param wp(vtx::irect(ofsx + 230, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc1_voltage_ = wd.add_widget<widget_label>(wp, wp_);
				dc1_voltage_->at_local_param().select_func_ = [=](const std::string& str) {
					dc1_voltage_->set_text(limitf_(str, 0.0f, 60.0f, "%3.1f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 330, 20 + h * loc, 40, 40), dialog_);
				widget_text::param wp_("V");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // Max: 30A / step: 10mA
				widget::param wp(vtx::irect(ofsx + 370, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc1_current_ = wd.add_widget<widget_label>(wp, wp_);
				dc1_current_->at_local_param().select_func_ = [=](const std::string& str) {
					dc1_current_->set_text(limitf_(str, 0.0f, 30.0f, "%4.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 470, 20 + h * loc, 50, 40), dialog_);
				widget_text::param wp_("A");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 熱抵抗回数
				widget::param wp(vtx::irect(ofsx + 510, 20 + h * loc, 110, 40), dialog_);
				widget_spinbox::param wp_(10, 10, 500);
				dc1_count_ = wd.add_widget<widget_spinbox>(wp, wp_);
				dc1_count_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			} 
			{
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				dc1_exec_ = wd.add_widget<widget_button>(wp, wp_);
				dc1_exec_->at_local_param().select_func_ = [=](int n) {
					dc1_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(dc1_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = dc1_ena_->get_check();
					if(dc1_mode_->get_select_pos() < 2) {
						t.mode = dc1_mode_->get_select_pos() & 1;
						t.count = 10;
						t.thermal = false;
					} else {
						t.count = dc1_count_->get_select_pos();
						t.thermal = true;
					}

					float v;
					if((utils::input("%f", dc1_voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 62.5e-6;
					}
					if((utils::input("%f", dc1_current_->get_text().c_str()) % v).status()) {
						t.curr = v / 31.25e-6;
					}
					client_.send_data(t.build());
				};
			}
		}


		void init_dc2_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, dc2_sw_, 14, 15);
			++loc;
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				dc2_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("定電流");
				wp_.init_list_.push_back("定電圧");
				dc2_mode_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 300V/0.1V, 100mA/0.01mA
				widget::param wp(vtx::irect(ofsx + 230, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc2_voltage_ = wd.add_widget<widget_label>(wp, wp_);
				dc2_voltage_->at_local_param().select_func_ = [=](const std::string& str) {
					dc2_voltage_->set_text(limitf_(str, 0.0f, 300.0f, "%2.1f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 330, 20 + h * loc, 40, 40), dialog_);
				widget_text::param wp_("V");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // Max: 100mA / step: 0.01mA
				widget::param wp(vtx::irect(ofsx + 370, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc2_current_ = wd.add_widget<widget_label>(wp, wp_);
				dc2_current_->at_local_param().select_func_ = [=](const std::string& str) {
					dc2_current_->set_text(limitf_(str, 0.0f, 100.0f, "%3.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 470, 20 + h * loc, 50, 40), dialog_);
				widget_text::param wp_("mA");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 電流、電圧測定値
				widget::param wp(vtx::irect(ofsx + 520, 20 + h * loc, 150, 40), dialog_);
				widget_label::param wp_("");
				dc2_probe_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				dc2_exec_ = wd.add_widget<widget_button>(wp, wp_);
				dc2_exec_->at_local_param().select_func_ = [=](int n) {
					dc2_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 14; ++i) {
						sw <<= 1;
						if(dc2_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
#ifdef DC2_KIKUSUI
					kikusui_.set_output(dc2_ena_->get_check());
					bool err = false;
					float c = 0.0f;
					if((utils::input("%f", dc2_current_->get_text().c_str()) % c).status()) {
						c /= 1000.0f;
					} else {
						err = true;
					}
					float v = 0.0f;
					if((utils::input("%f", dc2_voltage_->get_text().c_str()) % v).status()) {
					} else {
						err = true;
					}
					if(!err) {
						if(dc2_mode_->get_select_pos() == 0) {  // 電流 [mA]
							kikusui_.set_curr(c, v);
							dc2_curr_id_ = kikusui_.get_curr_id();
							kikusui_.req_curr();
						} else {  // 電圧 [V]
							kikusui_.set_volt(v, c);
							dc2_volt_id_ = kikusui_.get_volt_id();
							kikusui_.req_volt();
						}
					}
#else
					t.ena = dc2_ena_->get_check();
					t.mode = dc2_mode_->get_select_pos() & 1;
					dc2_probe_mode_ = t.mode;
					float v;
					if((utils::input("%f", dc2_voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 312.5e-6;
					}
					if((utils::input("%f", dc2_current_->get_text().c_str()) % v).status()) {
						t.curr = v / 100e-6;
					}
#endif
					client_.send_data(t.build());
				};
			}
		}


		void init_gen_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, gen_sw_, 5, 44);
			++loc;
			// ジェネレーター・モジュール
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 100, 40), dialog_);
				widget_check::param wp_("有効");
				gen_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // (6) ジェネレータ設定： 出力モード選択（矩形波/三角波/直流）
			   // タイプ（連続、単発）、
			   // 出力電圧、周波数、ON時間（レンジ：0.01ms）
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("矩形波");
				wp_.init_list_.push_back("三角波");
				gen_mode_ = wd.add_widget<widget_list>(wp, wp_);
				gen_mode_->at_local_param().select_func_ = [=](const std::string& str,
															   uint32_t pos) {
					if(pos == 0) {
						gen_duty_->set_stall(false);
					} else {
						gen_duty_->set_stall();
					}
				};
			}
			{  // ジェネレータ設定、周波数（1Hz to 100Hz, 1Hz/step)
				widget::param wp(vtx::irect(ofsx + 230 + 130 * 0, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("1", false);
				gen_freq_ = wd.add_widget<widget_label>(wp, wp_);
				gen_freq_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_freq_->set_text(limitf_(str, 1.0f, 100.0f, "%1.0f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 0 + 80, 20 + h * loc, 30, 40),
						dialog_);
					widget_text::param wp_("Hz");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、電圧（0V to 14V, 0.1V/step)
				widget::param wp(vtx::irect(ofsx + 230 + 120 * 1, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("0", false);
				gen_volt_ = wd.add_widget<widget_label>(wp, wp_);
				gen_volt_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_volt_->set_text(limitf_(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 1 + 80, 20 + h * loc, 30, 40),
									 dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、DUTY（0.1% to 100%, 0.1%/step）
				widget::param wp(vtx::irect(ofsx + 230 + 120 * 2, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("0.1", false);
				gen_duty_ = wd.add_widget<widget_label>(wp, wp_);
				gen_duty_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_duty_->set_text(limitf_(str, 0.1f, 100.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 2 + 80, 20 + h * loc, 30, 40),
									 dialog_);
					widget_text::param wp_("%");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			// ジェネレーター内臓電源
			{
				int ofs = 230 + 120 * 2 + 80 + 40;
				widget::param wp(vtx::irect(ofsx + ofs, 20 + h * loc, 80, 40), dialog_);
				widget_check::param wp_("電源");
				gen_iena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 内臓電源設定
				int ofs = 230 + 120 * 2 + 80 + 40 + 90;
				widget::param wp(vtx::irect(ofsx + ofs, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("0", false);
				gen_ivolt_ = wd.add_widget<widget_label>(wp, wp_);
				gen_ivolt_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_ivolt_->set_text(limitf_(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					int ofs = 230 + 120 * 2 + 80 + 40 + 90 + 80;
					widget::param wp(vtx::irect(ofsx + ofs, 20 + h * loc, 30, 40), dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}

			{
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				gen_exec_ = wd.add_widget<widget_button>(wp, wp_);
				gen_exec_->at_local_param().select_func_ = [=](int n) {
					wgm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(gen_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = gen_ena_->get_check();
					t.type = gen_mode_->get_select_pos() & 1;
					float v;
					if((utils::input("%f", gen_freq_->get_text().c_str()) % v).status()) {
						t.frq = v;
					}
					if((utils::input("%f", gen_duty_->get_text().c_str()) % v).status()) {
						t.duty = v * 10.0f;
					}
					if((utils::input("%f", gen_volt_->get_text().c_str()) % v).status()) {
						t.volt = v / 0.02f;
					}
					t.iena = gen_iena_->get_check();
					if((utils::input("%f", gen_ivolt_->get_text().c_str()) % v).status()) {
						t.ivolt = v / 15.626e-6;
					}
					auto s = t.build();
// std::cout << s << std::endl;
					client_.send_data(s);
				};
			}
		}


		void init_crm_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, crm_sw_, 14, 1);
			++loc;
			// ＣＲメジャー・モジュール
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				crm_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 電流レンジ切り替え（2mA, 20mA, 200mA）
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("  2 mA");
				wp_.init_list_.push_back(" 20 mA");
				wp_.init_list_.push_back("200 mA");
				crm_amps_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(ofsx + 220, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back(" 1 KHz");
				wp_.init_list_.push_back("10 KHz");
				crm_freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 抵抗値、容量値選択
				widget::param wp(vtx::irect(ofsx + 350, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("抵抗値");
				wp_.init_list_.push_back("容量値");
				crm_mode_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 490, 20 + h * loc, 200, 40), dialog_);
				widget_label::param wp_("");
				crm_ans_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				crm_exec_ = wd.add_widget<widget_button>(wp, wp_);
				crm_exec_->at_local_param().select_func_ = [=](int n) {
					crm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 14; ++i) {
						sw <<= 1;
						if(crm_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = crm_ena_->get_check();
					t.amps = crm_amps_->get_select_pos();
					t.freq = crm_freq_->get_select_pos();
					t.mode = crm_mode_->get_select_pos();

					client_.send_data(t.build());
				};
			}
		}


		void init_icm_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, icm_sw_, 6, 34);
			{  // exec
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				icm_exec_ = wd.add_widget<widget_button>(wp, wp_);
				icm_exec_->at_local_param().select_func_ = [=](int n) {
					icm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 6; ++i) {
						sw <<= 1;
						if(icm_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;

					client_.send_data(t.build());
				};
			}
		}


		std::string build_wdm_()
		{
			std::string s;
			uint32_t cmd;

			// SW
			cmd = 0b00001000;
			cmd <<= 16;
			uint32_t sw = 0;
			for(uint32_t i = 0; i < 4; ++i) {
				sw <<= 1;
				if(wdm_sw_[i]->get_check()) ++sw;
			}
			cmd |= sw << 12;
			s += (boost::format("wdm %06X\n") % cmd).str();
			s += "delay 1\n";

			static const uint8_t smtbl[] = {
				0b01000001,  // 1K
				0b10000001,  // 2K
				0b11000001,  // 5K
				0b01000010,  // 10K
				0b10000010,  // 20K
				0b11000010,  // 50K
				0b01000011,  // 100K
				0b10000011,  // 200K
				0b11000011,  // 500K
				0b01000100,  // 1M
				0b10000100,  // 2M
				0b11000100,  // 5M
				0b01000101,  // 10M
				0b10000101,  // 20M
				0b11000101,  // 50M
				0b01000110,  // 100M
			};
			// sampling freq
			cmd = (0b00010000 << 16) | (smtbl[wdm_smpl_->get_select_pos() % 16] << 8);
			s += (boost::format("wdm %06X\n") % cmd).str();
			// channel gain
			for(int i = 0; i < 4; ++i) {
				cmd = ((0b00011000 | (i + 1)) << 16) | ((wdm_gain_[i]->get_select_pos() % 8) << 13);
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			{ // trigger level
				cmd = (0b00101000 << 16);
				int v;
				if((utils::input("%d", wdm_level_->get_text().c_str()) % v).status()) {
					if(v < 1) v = 1;
					else if(v > 65534) v = 65534;
					cmd |= v;
					s += (boost::format("wdm %06X\n") % cmd).str();
				}
			}
			{  // trigger channel
				cmd = (0b00100000 << 16);
				if(wdm_ie_trg_->get_select_pos()) cmd |= 0x0200;
				if(wdm_sm_trg_->get_select_pos()) cmd |= 0x0100;
				auto n = wdm_ch_->get_select_pos();
				uint8_t sub = 0;
				sub |= 0x80;  // trigger ON
				cmd |= static_cast<uint32_t>(n & 3) << 14;
				if(wdm_slope_->get_select_pos()) sub |= 0x40;
				sub |= wdm_window_->get_select_pos() & 15;
				cmd |= sub;
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			return s;
		}


		void init_wdm_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, wdm_sw_, 4, 29);
			for(int i = 0; i < 4; ++i) {  // 各チャネル減衰設定
				static const vtx::ipos ofs[4] = {
					{ 0, 0 }, { 110, 0 }, { 220, 0 }, { 330, 0 }
				};
				widget::param wp(vtx::irect(ofsx + ofs[i].x + 4 * 60 + 10, 20 + h * loc + ofs[i].y,
					100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("-22dB");
				wp_.init_list_.push_back("-16dB");
				wp_.init_list_.push_back("-10dB");
				wp_.init_list_.push_back(" -4dB");
				wp_.init_list_.push_back("  2dB");
				wp_.init_list_.push_back("  8dB");
				wp_.init_list_.push_back(" 14dB");
				wp_.init_list_.push_back(" 20dB");
				wdm_gain_[i] = wd.add_widget<widget_list>(wp, wp_);
				wdm_gain_[i]->select(4);
			}
			{  // 内部、外部トリガー選択
				widget::param wp(vtx::irect(ofsx + 690, 20 + h * loc, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Int");
				wp_.init_list_.push_back("Ext");
				wdm_ie_trg_ = wd.add_widget<widget_list>(wp, wp_);
			}
			++loc;
			{  // 時間軸リスト 10K、20K、50K、100K、200K、500K、1M、2M、5M、10M、20M、50M、100M
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("1mS");
				wp_.init_list_.push_back("500uS");
				wp_.init_list_.push_back("200uS");
				wp_.init_list_.push_back("100uS");
				wp_.init_list_.push_back(" 50uS");
				wp_.init_list_.push_back(" 20uS");
				wp_.init_list_.push_back(" 10uS");
				wp_.init_list_.push_back("  5uS");
				wp_.init_list_.push_back("  2uS");
				wp_.init_list_.push_back("  1uS");
				wp_.init_list_.push_back("500nS");
				wp_.init_list_.push_back("200nS");
				wp_.init_list_.push_back("100nS");
				wp_.init_list_.push_back(" 50nS");
				wp_.init_list_.push_back(" 20nS");
				wp_.init_list_.push_back(" 10nS");
				wdm_smpl_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・チャネル選択
				widget::param wp(vtx::irect(ofsx + 110, 20 + h * loc, 90, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				wp_.init_list_.push_back("CH4");
				wdm_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・スロープ選択
				widget::param wp(vtx::irect(ofsx + 210, 20 + h * loc, 90, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Fall");
				wp_.init_list_.push_back("Rise");
				wdm_slope_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・ウィンドウ（１～１５）
				widget::param wp(vtx::irect(ofsx + 310, 20 + h * loc, 90, 40), dialog_);
				widget_spinbox::param wp_(1, 1, 15);
				wdm_window_ = wd.add_widget<widget_spinbox>(wp, wp_);
				wdm_window_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(ofsx + 410, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("32768", false);
				wdm_level_ = wd.add_widget<widget_label>(wp, wp_);
				wdm_level_->at_local_param().select_func_ = [=](const std::string& str) {
					wdm_level_->set_text(limiti_(str, 1, 65534, "%d"));
				};
			}
			{  // トリガーレベル変換
				widget::param wp(vtx::irect(ofsx + 510, 20 + h * loc, 70, 40), dialog_);
				widget_check::param wp_("ＶＡ");
				wdm_cnv_ = wd.add_widget<widget_check>(wp, wp_);
				wdm_cnv_->at_local_param().select_func_ = [=](bool f) {
					if(f) {
						wdm_level_->set_stall();
						wdm_level_va_->set_stall(false);
					} else {
						wdm_level_->set_stall(false);
						wdm_level_va_->set_stall();
					}
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(ofsx + 590, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				wdm_level_va_ = wd.add_widget<widget_label>(wp, wp_);
				wdm_level_va_->at_local_param().select_func_ = [=](const std::string& str) {
					static const float vat[4] = { 32.768f, 655.36f, 16.384f, 65.536f };
					auto n = wdm_ch_->get_select_pos();
					auto g = get_gain_rate_(n);
					auto s = limitf_(str, -vat[n] * g, vat[n] * g, "%4.3f");
					if(s.empty()) {
						s = "0.0";
					}
					wdm_level_va_->set_text(s);
					float a;
					if((utils::input("%f", s.c_str()) % a).status()) {
						int32_t b = a * 32767.0f / vat[n] * g;
						b += 32767;
						wdm_level_->set_text((boost::format("%d") % b).str());
					}
				};
			}
			{  // Single, Manual トリガー選択
				widget::param wp(vtx::irect(ofsx + 690, 20 + h * loc, 100, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Single");
				wp_.init_list_.push_back("Manual");
				wdm_sm_trg_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				wdm_exec_ = wd.add_widget<widget_button>(wp, wp_);
				wdm_exec_->at_local_param().select_func_ = [=](int n) {
					sample_param_.rate = get_time_div_();
					for(uint32_t i = 0; i < 4; ++i) {
						sample_param_.gain[i] = get_gain_rate_(i);
					}
					auto s = build_wdm_();
					client_.send_data(s);
				};
			}
		}


		void load_sw_(sys::preference& pre, gui::widget_check* sw[], uint32_t n)
		{
			for(uint32_t i = 0; i < n; ++i) {
				sw[i]->load(pre);
			}
		}


		void save_sw_(sys::preference& pre, gui::widget_check* sw[], uint32_t n)
		{
			for(uint32_t i = 0; i < n; ++i) {
				sw[i]->save(pre);
			}
		}


		void set_help_(gui::widget* src, const std::string& text)
		{
			auto l = src->get_param().rect_.size.x;
			chip_->set_offset(src, vtx::ipos(l - 10, -35));
			chip_->set_text(text);
		}


		// 各モジュールへ初期化状態の転送
		void startup_()
		{
			{  // ICM
				icm_t t;
				client_.send_data(t.build());
			}
			{  // DC1
				dc1_t t;
				client_.send_data(t.build());
			}
			{  // DC2
				dc2_t t;
				client_.send_data(t.build());
			}
			{  // CRM
				crm_t t;
				client_.send_data(t.build());
			}
			{  // WGM
				wgm_t t;
				client_.send_data(t.build());
			}
			{  // WDM / SW
				uint32_t cmd = 0b00001000;
				cmd <<= 16;
				auto s = (boost::format("wdm %06X\n") % cmd).str();
				client_.send_data(s);
			}
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		inspection(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilock, wave_cap& wc, kikusui& kik) :
			director_(d), client_(client), interlock_(ilock), wave_cap_(wc), kikusui_(kik),
			dialog_(nullptr),
			unit_name_(nullptr), load_file_(nullptr), save_file_(nullptr), ilock_enable_(nullptr),

			dc1_sw_{ nullptr },
			dc1_ena_(nullptr), dc1_mode_(nullptr), dc1_voltage_(nullptr), dc1_current_(nullptr),
			dc1_count_(nullptr),
			dc1_exec_(nullptr),

			dc2_sw_{ nullptr },
			dc2_ena_(nullptr), dc2_mode_(nullptr), dc2_voltage_(nullptr), dc2_current_(nullptr),
			dc2_probe_(nullptr),
			dc2_exec_(nullptr),
			dc2_probe_mode_(false), dc2_curr_id_(0), dc2_volt_id_(0),

			gen_ena_(nullptr), gen_mode_(nullptr), gen_freq_(nullptr),
			gen_volt_(nullptr), gen_duty_(nullptr), gen_iena_(nullptr), gen_ivolt_(nullptr),
			gen_exec_(nullptr),

			crm_sw_{ nullptr },
			crm_ena_(nullptr), crm_amps_(nullptr), crm_freq_(nullptr), crm_mode_(nullptr),
			crm_ans_(nullptr), crm_exec_(nullptr),

			icm_sw_{ nullptr }, icm_exec_(nullptr),

			wdm_sw_{ nullptr },
			wdm_smpl_(nullptr), wdm_ch_(nullptr), wdm_slope_(nullptr), wdm_window_(nullptr),
			wdm_level_(nullptr), wdm_cnv_(nullptr), wdm_level_va_(nullptr),
			wdm_gain_{ nullptr }, wdm_ie_trg_(nullptr), wdm_sm_trg_(nullptr),
			wdm_exec_(nullptr),
			sample_param_(),

			test_(d),

			chip_(nullptr),

			startup_init_(false),
			crrd_id_(0), crcd_id_(0), d2md_id_(0),
			crrd_value_(0.0), crcd_value_(0.0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト・パラメーターの取得
			@return テスト・パラメーター
		*/
		//-----------------------------------------------------------------//
		const test_param& get_test_param() const { return test_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト・パラメーターの取得
			@return テスト・パラメーター
		*/
		//-----------------------------------------------------------------//
		test_param& at_test_param() { return test_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  サンプリング・パラメーターの取得
			@return サンプリング・パラメーター
		*/
		//-----------------------------------------------------------------//
		const wave_cap::sample_param& get_sample_param() const { return sample_param_; }

		bool get_crm_mode() const { return crm_mode_->get_select_pos(); }
		double get_crrd_value() const { return crrd_value_; }
		double get_crcd_value() const { return crcd_value_; }

		//-----------------------------------------------------------------//
		/*!
			@brief  DC1 コマンドの発行
		*/
		//-----------------------------------------------------------------//
		void exec_dc1() { dc1_exec_->exec(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  DC2 コマンドの発行
		*/
		//-----------------------------------------------------------------//
		void exec_dc2() { dc2_exec_->exec(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  GEN コマンドの発行
		*/
		//-----------------------------------------------------------------//
		void exec_gen() { gen_exec_->exec(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  CRM コマンドの発行
		*/
		//-----------------------------------------------------------------//
		void exec_crm() { crm_exec_->exec(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  ICM コマンドの発行
		*/
		//-----------------------------------------------------------------//
		void exec_icm() { icm_exec_->exec(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  WDM コマンドの発行
		*/
		//-----------------------------------------------------------------//
		void exec_wdm() { wdm_exec_->exec(); }


		//-----------------------------------------------------------------//
		/*!
			@brief  各モジュールからの受信(DC2, CRM)
		*/
		//-----------------------------------------------------------------//
		void update_client()
		{
			// モジュールから受け取ったパラメーターをＧＵＩに反映
			static const uint32_t sample_num = 50;
			if(crm_mode_->get_select_pos() == 0) {  // CRRD
				if(crrd_id_ != client_.get_mod_status().crrd_id_) {
					crrd_id_ = client_.get_mod_status().crrd_id_;
					uint32_t v = client_.get_mod_status().crrd_;
					v -= sample_num * 0x7FFFF;
					double a = static_cast<double>(v) / 50.0 / static_cast<double>(0x7FFFF)
						* 1.570798233;
					a *= 778.2;  // 778.2 mV P-P
					static const double itbl[3] = {  // 電流テーブル
						2.0, 20.0, 200.0
					};
					a /= itbl[crm_amps_->get_select_pos() % 3];
					crrd_value_ = a;
					crm_ans_->set_text((boost::format("%5.4f Ω") % a).str());
				}
			} else { // CRCD
				if(crcd_id_ != client_.get_mod_status().crcd_id_) {
					crcd_id_ = client_.get_mod_status().crcd_id_;
					uint32_t v = client_.get_mod_status().crcd_;
					v -= sample_num * 0x7FFFF;
					double a = static_cast<double>(v) / 50.0 / static_cast<double>(0x7FFFF)
						* 1.570798233;
					a *= 778.2 * 2.0;  // 778.2 mV P-P
					static const double itbl[3] = {  // 電流テーブル
						2.0, 20.0, 200.0
					};
					a /= itbl[crm_amps_->get_select_pos() % 3];

					a = 1.0 / (2.0 * 3.141592654 * 1000.0 * a);
					a *= 1e6;
					crcd_value_ = a;
					crm_ans_->set_text((boost::format("%5.4f uF") % a).str());
				}
			}

			// D2MD
			if(d2md_id_ != client_.get_mod_status().d2md_id_) {
				d2md_id_ = client_.get_mod_status().d2md_id_;
				uint32_t v = client_.get_mod_status().d2md_;
				float a = static_cast<float>(v);
				if(dc2_probe_mode_) {
					a /= 999960.2f;
					a *= 100.0f;
					dc2_probe_->set_text((boost::format("%5.2f mA") % a).str());
				} else {
					a /= static_cast<float>(0xFFFFF);
	   				a *= 330.0f;
					dc2_probe_->set_text((boost::format("%5.2f V") % a).str());
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int d_w = 970;
			int d_h = 720;
			{
				widget::param wp(vtx::irect(100, 100, d_w, d_h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [=](bool ok) {
				};
			}
			int w = 130;
			int h = 45;
			static const char* tbls[] = {
				"ファイル名：",
				"ＷＤＭ：", nullptr,
				"ＤＣ１：", nullptr,
				"ＤＣ２：", nullptr,
				"ＷＧＭ：", nullptr,
				"ＣＲＭ：", nullptr,
				"ＩＣＭ：",
				"検査：",
			};
			for(int i = 0; i < sizeof(tbls) / sizeof(const char*); ++i) {
				widget::param wp(vtx::irect(20, 20 + h * i, w, h), dialog_);
				if(tbls[i] == nullptr) continue;
				widget_text::param wp_(tbls[i]);
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			int ofsx = w + 10;
			{  // 単体試験名
				widget::param wp(vtx::irect(ofsx, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_("", false);
				unit_name_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // ロード・ファイル
				widget::param wp(vtx::irect(ofsx + 320, 20 + h * 0, 100, 40), dialog_);
				widget_button::param wp_("ロード");
				load_file_ = wd.add_widget<widget_button>(wp, wp_);
				load_file_->at_local_param().select_func_ = [=](bool f) {
					std::string filter = "単体テスト(*.";
					filter += UNIT_EXT_;
					filter += ")\t*.";
					filter += UNIT_EXT_;
					filter += "\t";
					unit_load_filer_.open(filter);
				};
			}
			{  // ロード・ファイル
				widget::param wp(vtx::irect(ofsx + 440, 20 + h * 0, 100, 40), dialog_);
				widget_button::param wp_("セーブ");
				save_file_ = wd.add_widget<widget_button>(wp, wp_);
				save_file_->at_local_param().select_func_ = [=](bool f) {
					std::string filter = "単体テスト(*.";
					filter += UNIT_EXT_;
					filter += ")\t*.";
					filter += UNIT_EXT_;
					filter += "\t";
					unit_save_filer_.open(filter, true);
				};
			}

			ofsx = 110;
			init_wdm_(d_w, ofsx, h, 1);
			init_dc1_(d_w, ofsx, h, 3);
			init_dc2_(d_w, ofsx, h, 5);
			init_gen_(d_w, ofsx, h, 7);
			init_crm_(d_w, ofsx, h, 9);
			init_icm_(d_w, ofsx, h, 11);

			test_.init(dialog_, d_w, ofsx, h, 12);

			{  // help message (widget_chip)
				widget::param wp(vtx::irect(0, 0, 100, 40), dialog_);
				widget_chip::param wp_;
				chip_ = wd.add_widget<widget_chip>(wp, wp_);
			}

			{  // インターロック機構、On/Off
				widget::param wp(vtx::irect(ofsx + 590, 20 + h * 0, 180, 40), dialog_);
				widget_check::param wp_("Interlock", true);
				ilock_enable_ = wd.add_widget<widget_check>(wp, wp_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
#if 0
			if(!startup_init_) {
				if(!client_.probe()) return;
				startup_();
				startup_init_ = true;
			}
#endif
			interlock_.update(ilock_enable_->get_check());

#ifdef DC2_KIKUSUI
			if(dc2_curr_id_ != kikusui_.get_curr_id()) {
				dc2_curr_id_ = kikusui_.get_curr_id();
				auto c = kikusui_.get_curr();
				c /= 1000.0f;
				dc2_probe_->set_text((boost::format("%6.5f") % c).str());
			}
			if(dc2_volt_id_ != kikusui_.get_volt_id()) {
				dc2_volt_id_ = kikusui_.get_volt_id();
				auto v = kikusui_.get_volt();
				dc2_probe_->set_text((boost::format("%6.5f") % v).str());
			}
#endif

			if(!dialog_->get_state(gui::widget::state::ENABLE)) return;

			// 転送スイッチの状態をネットワークの接続状態で設定
			dc1_exec_->set_stall(!client_.probe());
///			dc2_exec_->set_stall(!client_.probe());
			gen_exec_->set_stall(!client_.probe());
			crm_exec_->set_stall(!client_.probe());
			icm_exec_->set_stall(!client_.probe());
			wdm_exec_->set_stall(!client_.probe());

			if(unit_load_filer_.state()) {
				auto path = unit_load_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += '.';
						ph += UNIT_EXT_;
					}
					sys::preference pre;
					auto ret = pre.load(path);
					if(ret) {
						load(pre);
						wave_cap_.load(pre);
					}
				}
			}
			if(unit_save_filer_.state()) {
				auto path = unit_save_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += '.';
						ph += UNIT_EXT_;
					}
					sys::preference pre;
					save(pre);
					wave_cap_.save(pre);
					pre.save(ph);
				}
			}

			// ヘルプ機能
			uint32_t act = 60 * 3;
			if(dc1_current_->get_focus()) {
				set_help_(dc1_current_, "0.0 to 30.0 [A], 0.1 [A] / step");
			} else if(dc1_voltage_->get_focus()) {
				set_help_(dc1_voltage_, "0.0 to 60.0 [V], 0.1 [V] / step");
			} else if(dc2_voltage_->get_focus()) {
				set_help_(dc2_voltage_, "0.0 to 300.0 [V], 0.1 [V] / step");
			} else if(dc2_current_->get_focus()) {
				set_help_(dc2_current_, "0.0 to 100.0 [mA], 0.01 [mA] / step");
			} else if(gen_freq_->get_focus()) {
				set_help_(gen_freq_, "1 to 100 [Hz], 1 [Hz] / step");
			} else if(gen_volt_->get_focus()) {
				set_help_(gen_volt_, "0.0 to 14.0 [V], 0.1 [V] / step");
			} else if(gen_duty_->get_focus()) {
				set_help_(gen_duty_, "0.1 to 100.0 [%], 0.1 [%] / step");
			} else if(gen_ivolt_->get_focus()) {
				set_help_(gen_ivolt_, "0.0 to 16.0 [V], 0.01 [V] / step");
			} else if(test_.symbol_->get_focus()) {
				set_help_(test_.symbol_, "検査記号");
			} else if(test_.wait_->get_focus()) {
				set_help_(test_.wait_, "検査遅延: 0.0 to 1.0 [秒], 0.01 [秒] / step");
			} else if(test_.retry_->get_focus()) {
				set_help_(test_.retry_, "検査リトライ回数");
			} else if(test_.delay_->get_focus()) {
				auto ch = test_.term_->get_select_pos() + 1;
				auto str = (boost::format("CH%d 検査ポイント（時間）") % ch).str();
				set_help_(test_.delay_, str);
			} else if(test_.width_->get_focus()) {
				set_help_(test_.width_, "検査幅（時間）");
			} else if(test_.min_->get_focus()) {
				set_help_(test_.min_, "検査：最低値");
			} else if(test_.max_->get_focus()) {
				set_help_(test_.max_, "検査：最大値");
			} else if(wdm_level_->get_focus()) {
				set_help_(wdm_level_, "トリガーレベル（数値入力）");
			} else if(wdm_level_va_->get_focus()) {
				static const float vat[4] = { 32.768f, 655.36f, 16.384f, 65.536f };
				static const char* vau[4] = { " A", " V", " V", " KV" };
				auto n = wdm_ch_->get_select_pos();
				std::string s = "トリガーレベル（";
				if(wdm_ch_->get_select_pos() == 0) {
					s += "電流：±";
				} else {
					s += "電圧：±";
				}
				auto a = get_gain_rate_(n);
				s += (boost::format("%4.3f") % (vat[n] * a)).str();
				s += vau[n];
				s += "）";
				set_help_(wdm_level_va_, s);
			} else {
				act = 0;
			}
			chip_->active(act);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
			unit_name_->save(pre);

			save_sw_(pre, dc1_sw_, 5);
			dc1_ena_->save(pre);
			dc1_mode_->save(pre);
			dc1_voltage_->save(pre);
			dc1_current_->save(pre);
			dc1_count_->save(pre);

			save_sw_(pre, dc2_sw_, 14);
			dc2_ena_->save(pre);
			dc2_mode_->save(pre);
			dc2_voltage_->save(pre);
			dc2_current_->save(pre);

			save_sw_(pre, gen_sw_, 5);
			gen_ena_->save(pre);
			gen_mode_->save(pre);
			gen_freq_->save(pre);
			gen_volt_->save(pre);
			gen_duty_->save(pre);
			gen_iena_->save(pre);
			gen_ivolt_->save(pre);

			save_sw_(pre, crm_sw_, 14);
			crm_ena_->save(pre);
			crm_amps_->save(pre);
			crm_freq_->save(pre);
			crm_mode_->save(pre);

			save_sw_(pre, icm_sw_, 6);

			save_sw_(pre, wdm_sw_, 4);
			wdm_smpl_->save(pre);
			wdm_ch_->save(pre);
			wdm_slope_->save(pre);
			wdm_window_->save(pre);
			wdm_level_->save(pre);
			wdm_gain_[0]->save(pre);
			wdm_gain_[1]->save(pre);
			wdm_gain_[2]->save(pre);
			wdm_gain_[3]->save(pre);
			wdm_ie_trg_->save(pre);
			wdm_sm_trg_->save(pre);

			test_.save(pre);

			ilock_enable_->save(pre);

			return true;
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
			save(pre);
			return pre.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre)
		{
			unit_name_->load(pre);

			load_sw_(pre, dc1_sw_, 5);
			dc1_ena_->load(pre);
			dc1_mode_->load(pre);
			dc1_voltage_->load(pre);
			dc1_current_->load(pre);
			dc1_count_->load(pre);

			load_sw_(pre, dc2_sw_, 14);
			dc2_ena_->load(pre);
			dc2_mode_->load(pre);
			dc2_voltage_->load(pre);
			dc2_current_->load(pre);

			load_sw_(pre, gen_sw_, 5);
			gen_ena_->load(pre);
			gen_mode_->load(pre);
			gen_freq_->load(pre);
			gen_volt_->load(pre);
			gen_duty_->load(pre);
			gen_iena_->load(pre);
			gen_ivolt_->load(pre);

			load_sw_(pre, crm_sw_, 14);
			crm_ena_->load(pre);
			crm_amps_->load(pre);
			crm_freq_->load(pre);
			crm_mode_->load(pre);

			load_sw_(pre, icm_sw_, 6);

			load_sw_(pre, wdm_sw_, 4);
			wdm_smpl_->load(pre);
			wdm_ch_->load(pre);
			wdm_slope_->load(pre);
			wdm_window_->load(pre);
			wdm_level_->load(pre);
			wdm_gain_[0]->load(pre);
			wdm_gain_[1]->load(pre);
			wdm_gain_[2]->load(pre);
			wdm_gain_[3]->load(pre);
			wdm_ie_trg_->load(pre);
			wdm_sm_trg_->load(pre);

			test_.load(pre);

			ilock_enable_->load(pre);

			return true;
		}


		bool load(const std::string& path)
		{
			sys::preference pre;
			auto ret = pre.load(path);
			if(ret) {
				ret = load(pre);
			}
			return ret;
		}
	};
}
