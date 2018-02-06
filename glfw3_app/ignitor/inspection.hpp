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
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "relay_map.hpp"
#include "ign_client.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  単体検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection {
	public:

		struct dc2_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits

			dc2_t() : sw(0), ena(0), mode(0), volt(0), curr(0) { }

			utils::strings build() const
			{
				utils::strings ss;
				ss.push_back((boost::format("dc2 W2SW%04X\n") % sw).str());
				ss.push_back((boost::format("dc2 W2MD%d\n") % mode).str());
				ss.push_back((boost::format("dc2 D2VS%05X\n") % (volt & 0xfffff)).str());
				ss.push_back((boost::format("dc2 D2IS%05X\n") % (curr & 0xfffff)).str());
				ss.push_back((boost::format("dc2 D2OE%d\n") % ena).str());
				return ss;
			}
		};


		struct wgm_t {
			uint16_t	sw;		///< 5 bits
			bool		ena;	///< 0, 1
			bool		type;	///< 0, 1
			uint16_t   	frq;	///< 7 bits
			uint16_t	duty;	///< 10 bits
			uint16_t	volt;	///< 10 bits

			wgm_t() : ena(0), type(0), frq(0), duty(0), volt(0) { }

			utils::strings build() const
			{
				utils::strings ss;
				ss.push_back((boost::format("wgm WGSW%02X\n") % sw).str());
				ss.push_back((boost::format("wgm WGSP%d\n") % type).str());
				ss.push_back((boost::format("wgm WGFQ%02X\n") % (frq & 0x7f)).str());
				ss.push_back((boost::format("wgm WGPW%03X\n") % (duty & 0x3ff)).str());
				ss.push_back((boost::format("wgm WGPV%03X\n") % (volt & 0x3ff)).str());
				ss.push_back((boost::format("wgm WGOE%d\n") % ena).str());
				return ss;
			}
		};


		struct crm_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			uint16_t	freq;	///< 0, 1, 2
			uint16_t	mode;	///< 0, 1

			crm_t() : sw(0), ena(0), freq(0), mode(0) { }

			utils::strings build() const
			{
				utils::strings ss;
				ss.push_back((boost::format("crm CRSW%04X\n") % sw).str());
				static const char* frqtbl[3] = { "001", "010", "100" };
				ss.push_back((boost::format("crm CRFQ%s\n") % frqtbl[freq]).str());
				ss.push_back((boost::format("crm CROE%d\n") % ena).str());
				if(mode) {
					ss.push_back((boost::format("crm CRC?1\n")).str());
				} else {
					ss.push_back((boost::format("crm CRR?1\n")).str());
				}
				return ss;
			}
		};


	private:
		static constexpr const char* UNIT_EXT_ = "unt";  ///< 単体検査ファイル、拡張子

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

		net::ign_client&		client_;

		gui::widget_dialog*		dialog_;
		gui::widget_label*		unit_name_;				///< 単体試験名
		gui::widget_button*		load_file_;				///< load file
		gui::widget_button*		save_file_;				///< save file
		utils::select_file		unit_load_filer_;
		utils::select_file		unit_save_filer_;

		gui::widget_list*		inspection_standards_;	///< 検査規格 (Inspection standards)

		// DC2 設定
		gui::widget_check*		dc2_sw_[14];	///< DC2 接続スイッチ
		gui::widget_check*		dc2_ena_;		///< DC2 有効、無効
		gui::widget_list*		dc2_mode_;		///< DC2 電流、電圧モード
		gui::widget_label*		dc2_voltage_;	///< DC2（電圧）
		gui::widget_label*		dc2_current_;	///< DC2（電流）
		gui::widget_button*		dc2_exec_;		///< DC2 設定転送

		// WGM 設定
		gui::widget_check*		gen_sw_[5];	///< ジェネレータ接続スイッチ
		gui::widget_check*		gen_ena_;	///< ジェネレータ有効、無効
		gui::widget_list*		gen_mode_;	///< ジェネレータモード（矩形波、三角波、直流）
		gui::widget_label*		gen_freq_;	///< ジェネレータ設定・周波数（1Hz to 100Hz / 1Hz)
		gui::widget_label*		gen_volt_;	///< ジェネレータ設定・電圧（0 to 14V / 0.1V）
		gui::widget_label*		gen_duty_;	///< ジェネレーター設定・Duty（0.1% to 100% / 0.1%）
		gui::widget_button*		gen_exec_;	///< ジェネレーター設定転送

		// CRM 設定
		gui::widget_check*		crm_sw_[14];	///< CRM 接続スイッチ
		gui::widget_check*		crm_ena_;		///< CRM 有効、無効
		gui::widget_list*		crm_freq_;		///< CRM 周波数（100Hz, 1KHz, 10KHz）
		gui::widget_list*		crm_mode_;		///< CRM 抵抗測定、容量測定
		gui::widget_label*		crm_ans_;		///< CRM 測定結果 
		gui::widget_button*		crm_exec_;		///< CRM 設定転送

#if 0
		gui::widget_list*		oscillo_secdiv_;		///< オシロスコープ設定、時間（周期）
		gui::widget_list*		oscillo_trg_ch_;		///< オシロスコープ設定、トリガー・チャネル選択
		gui::widget_list*		oscillo_trg_slope_;		///< オシロスコープ設定、トリガー・スロープ選択
		gui::widget_spinbox*	oscillo_trg_window_;	///< オシロスコープ設定、トリガー・ウィンドウ
		gui::widget_label*		oscillo_trg_level_;		///< オシロスコープ設定、トリガー・レベル
		oscillo_t				oscillo_[4];			///< オシロスコープ各チャネル設定
#endif

		gui::widget_list*		fukasel_;				///< ２次負荷切替設定

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
//					= [=](const std::string& str, uint32_t pos) {
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


		void init_sw_(int ofsx, int h, int loc, gui::widget_check* out[], int num)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;			
			for(int i = 0; i < num; ++i) {
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 40, 40), dialog_);
				widget_check::param wp_;
				out[i] = wd.add_widget<widget_check>(wp, wp_);
				ofsx += 50;
			}
		}


		void init_dc2_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, dc2_sw_, 14);
			++loc;
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				dc2_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("定電圧");
				wp_.init_list_.push_back("定電流");
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
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
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
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
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
					t.ena = dc2_ena_->get_check();
					t.mode = dc2_mode_->get_select_pos() & 1;
					float v;
					if((utils::input("%f", dc2_voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 312.5e-6;
					}
					if((utils::input("%f", dc2_current_->get_text().c_str()) % v).status()) {
						t.curr = v / 100e-6;
					}

					auto ss = t.build();
					for(const std::string& s : ss) {
						client_.send(s);
					}
				};
			}
		}

#if 0
// DC1
				{  // [1]動特性検査：  60V/0.1V, 30A/0.1A
					widget::param wp(vtx::irect(ofsx + 170, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					voltage_[1] = wd.add_widget<widget_label>(wp, wp_);
					voltage_[1]->at_local_param().select_func_ = [=](const std::string& str) {
						voltage_[1]->set_text(limitf_(str, 0.0f, 60.0f, "%2.1f"));
					};
				}
				{  // Max: 30A / step: 0.1A
					widget::param wp(vtx::irect(ofsx + 320, 20 + h * 2, 90, 40), dialog_);
					widget_label::param wp_("0", false);
					current_[1] = wd.add_widget<widget_label>(wp, wp_);
					current_[1]->at_local_param().select_func_ = [=](const std::string& str) {
						current_[1]->set_text(limitf_(str, 0.0f, 30.0f, "%2.1f"));
					};
				}
#endif


		void init_gen_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, gen_sw_, 5);
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
				gen_mode_->at_local_param().select_func_ = [=](const std::string& str, uint32_t pos) {
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
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 0 + 80, 20 + h * loc, 30, 40), dialog_);
					widget_text::param wp_("Hz");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
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
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 1 + 80, 20 + h * loc, 30, 40), dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
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
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 2 + 80, 20 + h * loc, 30, 40), dialog_);
					widget_text::param wp_("%");
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
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
					auto ss = t.build();
					for(const std::string& s : ss) {
						client_.send(s);
					}					
				};
			}
		}


		void init_crm_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, crm_sw_, 14);
			++loc;
			// ＣＲメジャー・モジュール
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				crm_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back("1  KHz");
				wp_.init_list_.push_back("10 KHz");
				crm_freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 抵抗値、容量値選択
				widget::param wp(vtx::irect(ofsx + 220, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("抵抗値");
				wp_.init_list_.push_back("容量値");
				crm_mode_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 350, 20 + h * loc, 110, 40), dialog_);
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
					t.freq = crm_freq_->get_select_pos();
					t.mode = crm_mode_->get_select_pos();
					auto ss = t.build();
					for(const std::string& s : ss) {
std::cout << s << std::flush;
						client_.send(s);
					}
				};
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		inspection(utils::director<core>& d, net::ign_client& client) : director_(d), client_(client),
			dialog_(nullptr),
			unit_name_(nullptr), load_file_(nullptr), save_file_(nullptr),
			inspection_standards_(nullptr),

			dc2_sw_{ nullptr },
			dc2_ena_(nullptr), dc2_mode_(nullptr), dc2_voltage_(nullptr), dc2_current_(nullptr),
			dc2_exec_(nullptr),

			gen_ena_(nullptr), gen_mode_(nullptr), gen_freq_(nullptr),
			gen_volt_(nullptr), gen_duty_(nullptr), gen_exec_(nullptr),

			crm_sw_{ nullptr },
			crm_ena_(nullptr), crm_freq_(nullptr), crm_mode_(nullptr),
			crm_ans_(nullptr), crm_exec_(nullptr),

//			oscillo_secdiv_(nullptr) ,oscillo_trg_ch_(nullptr), oscillo_trg_slope_(nullptr),
//			oscillo_trg_window_(nullptr), oscillo_trg_level_(nullptr),

			fukasel_(nullptr),
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
				dialog_->at_local_param().select_func_ = [=](bool ok) {
				};
			}
			int w = 220;
			int h = 45;
			static const char* tbls[] = {
				"ファイル名：",
				"検査規格：",
				"ＤＣ２設定：", nullptr,
				"ジェネレーター設定：", nullptr,
				"ＣＲ測定設定：", nullptr,
				"測定項目設定：", nullptr,
				"：",
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

			{  // 検査規格
				widget::param wp(vtx::irect(ofsx, 20 + h * 1, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("MAX 値");
				wp_.init_list_.push_back("MIN 値");
				inspection_standards_ = wd.add_widget<widget_list>(wp, wp_);
			}
#if 0
			{  // 検査方法
				widget::param wp(vtx::irect(ofsx, 20 + h * 2, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("静特性検査");
				wp_.init_list_.push_back("動特性検査");
				wp_.init_list_.push_back("LCR検査");
				inspection_method_ = wd.add_widget<widget_list>(wp, wp_);
				inspection_method_->at_local_param().select_func_
					= [=](const std::string& str, uint32_t pos) {
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
#endif

				init_dc2_(d_w, ofsx, h, 2);
				init_gen_(d_w, ofsx, h, 4);
				init_crm_(d_w, ofsx, h, 6);
#if 0
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
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(ofsx + 600, 20 + h * 4, 80, 40), dialog_);
				widget_label::param wp_("1", false);
				oscillo_trg_level_ = wd.add_widget<widget_label>(wp, wp_);
				oscillo_trg_level_->at_local_param().select_func_ = [=](const std::string& str) {
					oscillo_trg_level_->set_text(limiti_(str, 1, 65534, "%d"));
				};
			}
			init_oscillo_(wd, ofsx,       20 + h * 5, "CH0", oscillo_[0]);
			init_oscillo_(wd, ofsx + 290, 20 + h * 5, "CH1", oscillo_[1]);
			init_oscillo_(wd, ofsx,       20 + h * 6, "CH2", oscillo_[2]);
			init_oscillo_(wd, ofsx + 290, 20 + h * 6, "CH3", oscillo_[3]);
#endif




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
				setup_relay_->at_local_param().select_func_ = [=](int n) {
					relay_map_.get_dialog()->enable();
				};
			}

			{  // (10) Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
				widget::param wp(vtx::irect(ofsx, 20 + h * 12, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				wait_time_ = wd.add_widget<widget_label>(wp, wp_);
				wait_time_->at_local_param().select_func_ = [=](const std::string& str) {
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
			if(!dialog_->get_state(gui::widget::state::ENABLE)) return;

			if(client_.recv_probe()) {
				auto s = client_.recv();
				if(s.find("CRCD") == 0) {
					auto t = s.substr(4, 4);
					int v = 0;
					utils::input("%x", t.c_str()) % v;
		std::cout << "C: " << v << std::endl;
				} else if(s.find("CRRD") == 0) {
					auto t = s.substr(4, 4);
					int v = 0;
					utils::input("%x", t.c_str()) % v;
		std::cout << "R: " << v << std::endl;
				}
			}


			if(unit_load_filer_.state()) {
				auto path = unit_load_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += UNIT_EXT_;
					}
					load(ph);
				}
			}
			if(unit_save_filer_.state()) {
				auto path = unit_save_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += UNIT_EXT_;
					}
					save(ph);
				}
			}

			if(dc2_voltage_->get_focus()) {
				help_->set_text("0.0 to 300.0 [V], 0.1 [V] / step");
			} else if(dc2_current_->get_focus()) {
				help_->set_text("0.0 to 100.0 [mA], 0.01 [mA] / step");
			} else

//			} else if(current_[1]->get_focus()) {
//				help_->set_text("0.0 to 30.0 [A], 0.1 [A] / step");
//			}
//			} else if(voltage_[1]->get_focus()) {
//				help_->set_text("0.0 to 60.0 [V], 0.1 [V] / step");

			if(gen_freq_->get_focus()) {
				help_->set_text("1 to 100 [Hz], 1 [Hz] / step");
			} else if(gen_volt_->get_focus()) {
				help_->set_text("0.0 to 14.0 [V], 0.1 [V] / step");
			} else if(gen_duty_->get_focus()) {
				help_->set_text("0.1 to 100.0 [%], 0.1 [%] / step");
			} else if(wait_time_->get_focus()) {
				help_->set_text("0.0 to 1.0 [秒], 0.01 [秒] / step");
//			} else if(oscillo_secdiv_->get_focus()) {
//				help_->set_text("サンプリング周期");
//			} else if(oscillo_trg_ch_->get_focus()) {
//				help_->set_text("トリガー・チャネル");
//			} else if(oscillo_trg_slope_->get_focus()) {
//				help_->set_text("トリガー・スロープ（Pos: 立ち上がり、Neg: 立下り）");
//			} else if(oscillo_trg_window_->get_focus()) {
//				help_->set_text("トリガー検知窓(幅): 1 to 15");
//			} else if(oscillo_trg_level_->get_focus()) {
//				help_->set_text("トリガーレベル: 1 to 65534");
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
			unit_name_->save(pre);
			inspection_standards_->save(pre);

			dc2_ena_->save(pre);
			dc2_mode_->save(pre);
			dc2_voltage_->save(pre);
			dc2_current_->save(pre);

			gen_ena_->save(pre);
			gen_mode_->save(pre);
			gen_freq_->save(pre);
			gen_volt_->save(pre);
			gen_duty_->save(pre);
#if 0
			oscillo_secdiv_->save(pre);
			oscillo_trg_ch_->save(pre);
			oscillo_trg_slope_->save(pre);
			oscillo_trg_window_->save(pre);
			oscillo_trg_level_->save(pre);
			oscillo_[0].save(pre);
			oscillo_[1].save(pre);
			oscillo_[2].save(pre);
			oscillo_[3].save(pre);
#endif
			fukasel_->save(pre);

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
				unit_name_->load(pre);
				inspection_standards_->load(pre);

				dc2_ena_->load(pre);
				dc2_mode_->load(pre);
				dc2_voltage_->load(pre);
				dc2_current_->load(pre);

				gen_ena_->load(pre);
				gen_mode_->load(pre);
				gen_freq_->load(pre);
				gen_volt_->load(pre);
				gen_duty_->load(pre);

#if 0
				oscillo_secdiv_->load(pre);
				oscillo_trg_ch_->load(pre);
				oscillo_trg_slope_->load(pre);
				oscillo_trg_window_->load(pre);
				oscillo_trg_level_->load(pre);
				oscillo_[0].load(pre);
				oscillo_[1].load(pre);
				oscillo_[2].load(pre);
				oscillo_[3].load(pre);
#endif

				fukasel_->load(pre);

				wait_time_->load(pre);

				relay_map_.load(pre);
			}
			return ret; 
		}
	};
}
