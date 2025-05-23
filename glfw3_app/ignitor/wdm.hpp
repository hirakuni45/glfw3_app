#pragma once
//=====================================================================//
/*! @file
    @brief  WDM クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
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

#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "tools.hpp"
#include "wave_cap.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  WDM クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wdm
	{
		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;

	public:
		gui::widget_check*		sw_[4];		///< WDM 接続スイッチ
		gui::widget_list*		smpl_;		///< WDM サンプリング周期
		gui::widget_list*		ch_;		///< WDM トリガーチャネル
		gui::widget_list*		slope_;		///< WDM スロープ
		gui::widget_spinbox*	window_;	///< WDM トリガー領域
		gui::widget_label*		level_;		///< WDM トリガー・レベル
		gui::widget_check*		cnv_;		///< WDM トリガー・レベル変換
		gui::widget_label*		level_va_;	///< WDM トリガー・レベル（電圧、電流）
		gui::widget_list*		gain_[4];	///< WDM チャネル・ゲイン
		gui::widget_button*		exec_;		///< WDM 設定転送
		wave_cap::sample_param	sample_param_;

	private:
		bool					init_;

		std::string				emu_path_;

		static const uint32_t time_div_size_ = 16;
		double get_time_div_() const {
			if(smpl_ == nullptr) return 0.0;

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
			return tbls[smpl_->get_select_pos() % time_div_size_];
		}


		static const uint32_t gain_rate_size_ = 8;
		double get_gain_rate_(uint32_t ch) const {
			if(gain_[ch] == nullptr) return 0.0f;
			auto n = gain_[ch]->get_select_pos() % gain_rate_size_;
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
			if(ch == 1) return tbls[n] * 8.0f; 
			else return tbls[n];
		}


		std::string build_wdm_(const std::string& opt)
		{
			std::string s;
			uint32_t cmd;

			// SW
			cmd = 0b00001000;
			cmd <<= 16;
			uint32_t sw = 0;
			for(uint32_t i = 0; i < 4; ++i) {
				sw <<= 1;
				if(sw_[i]->get_check()) ++sw;
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
			cmd = (0b00010000 << 16) | (smtbl[smpl_->get_select_pos() % 16] << 8);
			s += (boost::format("wdm %06X\n") % cmd).str();
			// channel gain
			for(int i = 0; i < 4; ++i) {
				cmd = ((0b00011000 | (i + 1)) << 16) | ((gain_[i]->get_select_pos() % 8) << 13);
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			{ // trigger level
				cmd = (0b00101000 << 16);
				int v;
				if((utils::input("%d", level_->get_text().c_str()) % v).status()) {
					if(v < 1) v = 1;
					else if(v > 65534) v = 65534;
					cmd |= v;
					s += (boost::format("wdm %06X\n") % cmd).str();
				}
			}
			{  // trigger channel
				cmd = (0b00100000 << 16);
				auto n = ch_->get_select_pos();
				uint8_t sub = 0;
				sub |= 0x80;  // trigger ON
				cmd |= static_cast<uint32_t>(n & 3) << 14;
				if(slope_->get_select_pos()) sub |= 0x40;
				sub |= window_->get_select_pos() & 15;
				cmd |= sub;
				if(opt.empty()) {
					s += (boost::format("wdm %06X\n") % cmd).str();
				} else {
					s += (boost::format("wdm %06X ") % cmd).str();
					s += opt;
					s += "\n";
				}
			}
			return s;
		}


		std::string build_init_wdm_()
		{
			std::string s;
			uint32_t cmd;

			// SW
			cmd = 0b00001000;
			cmd <<= 16;
			uint32_t sw = 0;
			for(uint32_t i = 0; i < 4; ++i) {
				sw <<= 1;
				++sw;
			}
			cmd |= sw << 12;
			s += (boost::format("wdm %06X\n") % cmd).str();
			s += "delay 1\n";

			static const uint8_t smtbl[] = {
				0b01000110,  // 100M
			};
			// sampling freq
			cmd = (0b00010000 << 16) | (smtbl[0] << 8);
			s += (boost::format("wdm %06X\n") % cmd).str();
			// channel gain
			for(int i = 0; i < 4; ++i) {
				cmd = ((0b00011000 | (i + 1)) << 16) | (0 << 13);
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			{ // trigger level
				cmd = (0b00101000 << 16);
				int v = 32768;
				cmd |= v;
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			{  // trigger channel
				cmd = (0b00100000 << 16);
				uint8_t sub = 0;
				sub |= 0x80;  // trigger ON
				uint32_t ch = 0 << 6;  // ch0
				ch |= 1;  // manual trigger
				cmd |= static_cast<uint32_t>(ch) << 8;
//				if(slope_->get_select_pos()) sub |= 0x40;
				sub |= 1;  // window
				cmd |= sub;
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			return s;
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		wdm(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc, bool init) :
			director_(d), client_(client), interlock_(ilc),
			sw_{ nullptr },
			smpl_(nullptr), ch_(nullptr), slope_(nullptr), window_(nullptr),
			level_(nullptr), cnv_(nullptr), level_va_(nullptr),
			gain_{ nullptr },
			exec_(nullptr),
			sample_param_(),
			init_(init), emu_path_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  波形エミュレーション・パスの設定
			@param[in]	path	波形パス
		*/
		//-----------------------------------------------------------------//
		void set_emu_path(const std::string& path) { emu_path_ = path; }


		//-----------------------------------------------------------------//
		/*!
			@brief  パラメータ、セットアップ
		*/
		//-----------------------------------------------------------------//
		void setup()
		{
			sample_param_.rate = get_time_div_();
			for(uint32_t i = 0; i < 4; ++i) {
				sample_param_.gain[i] = get_gain_rate_(i);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
			uint32_t cmd = 0b00001000;
			cmd <<= 16;
			auto s = (boost::format("wdm %06X\n") % cmd).str();
			client_.send_data(s);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
			@param[in]	root	ルート
			@param[in]	d_w		横幅最大
			@param[in]	ofsx	オフセット X
			@param[in]	ofsy	オフセット Y
			@param[in]	pg		プリファレンス・グループ
		*/
		//-----------------------------------------------------------------//
		void init(gui::widget* root, int d_w, int ofsx, int ofsy,
			gui::widget::PRE_GROUP pg = gui::widget::PRE_GROUP::_0)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{
				widget::param wp(vtx::irect(15, ofsy, 70, 40), root);
				wp.pre_group_ = pg;
				widget_text::param wp_("WDM:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			tools::init_sw(wd, root, interlock_, ofsx, ofsy, sw_, 4, 29, pg);
			for(int i = 0; i < 4; ++i) {  // 各チャネル減衰設定
				static const vtx::ipos ofs[4] = {
					{ 0, 0 }, { 110, 0 }, { 220, 0 }, { 330, 0 }
				};
				widget::param wp(vtx::irect(ofsx + ofs[i].x + 4 * 60 + 10, ofsy + ofs[i].y,
					100, 40), root);
				wp.pre_group_ = pg;
				widget_list::param wp_;
				wp_.init_list_.push_back("-22dB");
				wp_.init_list_.push_back("-16dB");
				wp_.init_list_.push_back("-10dB");
				wp_.init_list_.push_back(" -4dB");
				wp_.init_list_.push_back("  2dB");
				wp_.init_list_.push_back("  8dB");
				wp_.init_list_.push_back(" 14dB");
				wp_.init_list_.push_back(" 20dB");
				gain_[i] = wd.add_widget<widget_list>(wp, wp_);
				if(i == 1) {
					gain_[i]->select(1);
				} else {
					gain_[i]->select(4);
				}
			}

			ofsy += 50;
			{  // 時間軸リスト 10K、20K、50K、100K、200K、500K、1M、2M、5M、10M、20M、50M、100M
				widget::param wp(vtx::irect(ofsx, ofsy, 110, 40), root);
				wp.pre_group_ = pg;
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
				smpl_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・チャネル選択
				widget::param wp(vtx::irect(ofsx + 120, ofsy, 90, 40), root);
				wp.pre_group_ = pg;
				widget_list::param wp_;
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				wp_.init_list_.push_back("CH4");
				ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・スロープ選択
				widget::param wp(vtx::irect(ofsx + 220, ofsy, 90, 40), root);
				wp.pre_group_ = pg;
				widget_list::param wp_;
				wp_.init_list_.push_back("Fall");
				wp_.init_list_.push_back("Rise");
				slope_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・ウィンドウ（１～１５）
				widget::param wp(vtx::irect(ofsx + 320, ofsy, 90, 40), root);
				wp.pre_group_ = pg;
				widget_spinbox::param wp_(1, 1, 15);
				window_ = wd.add_widget<widget_spinbox>(wp, wp_);
				window_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(ofsx + 420, ofsy, 90, 40), root);
				wp.pre_group_ = pg;
				widget_label::param wp_("32768", false);
				level_ = wd.add_widget<widget_label>(wp, wp_);
				level_->at_local_param().select_func_ = [=](const std::string& str) {
					level_->set_text(tools::limiti(str, 1, 65534, "%d"));
				};
			}
			{  // トリガーレベル変換
				widget::param wp(vtx::irect(ofsx + 520, ofsy, 70, 40), root);
				wp.pre_group_ = pg;
				widget_check::param wp_("ＶＡ");
				cnv_ = wd.add_widget<widget_check>(wp, wp_);
				cnv_->at_local_param().select_func_ = [=](bool f) {
					if(f) {  // 数値から、電圧、電流に変換
						level_->set_stall();
						level_va_->set_stall(false);
						
					} else {  // 電圧、電流から数値に変換
						level_->set_stall(false);
						level_va_->set_stall();
					}
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(ofsx + 600, ofsy, 90, 40), root);
				wp.pre_group_ = pg;
				widget_label::param wp_("0", false);
				level_va_ = wd.add_widget<widget_label>(wp, wp_);
				level_va_->at_local_param().select_func_ = [=](const std::string& str) {
					static const float vat[4] = { 32.768f, 655.36f, 16.384f, 65.536f };
					auto n = ch_->get_select_pos();
					auto g = get_gain_rate_(n);
					auto s = tools::limitf(str, -vat[n] / g, vat[n] / g, "%4.3f");
					if(s.empty()) {
						s = "0.0";
					}
					level_va_->set_text(s);
					float a;
					if((utils::input("%f", s.c_str()) % a).status()) {
						int32_t b = a * 32767.0f / vat[n] * g;
						b += 32767;
						level_->set_text((boost::format("%d") % b).str());
					}
				};
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 50, ofsy, 30, 30), root);
				wp.pre_group_ = pg;
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					setup();
//					auto s = build_wdm_(emu_path_);
//					auto s = build_wdm_("TFALL.wad");
					auto s = build_wdm_("");
					client_.send_data(s);
				};
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			bool ena = false;
//			if(client_.probe() && all_->get_check()) ena = true; 
			if(client_.probe()) ena = true; 
			exec_->set_stall(!ena);

			if(ena && !init_) {
				client_.send_data(build_init_wdm_());
				init_ = true;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ヘルプ機能
			@param[in]	chip	ヘルプ・チップ
		*/
		//-----------------------------------------------------------------//
		bool help(gui::widget_chip* chip)
		{
			bool ret = true;
			if(level_->get_focus()) {
				tools::set_help(chip, level_, "トリガーレベル（数値入力）");
			} else if(level_va_->get_focus()) {
				static const float vat[4] = { 32.768f, 655.36f, 16.384f, 65.536f };
				static const char* vau[4] = { " A", " V", " V", " KV" };
				auto n = ch_->get_select_pos();
				std::string s = "トリガーレベル（";
				if(ch_->get_select_pos() == 0) {
					s += "電流：±";
				} else {
					s += "電圧：±";
				}
				auto a = get_gain_rate_(n);
				s += (boost::format("%4.3f") % (vat[n] / a)).str();
				s += vau[n];
				s += "）";
				tools::set_help(chip, level_va_, s);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
			tools::save_sw(pre, sw_, 4);
			smpl_->save(pre);
			ch_->save(pre);
			slope_->save(pre);
			window_->save(pre);
			level_->save(pre);
			gain_[0]->save(pre);
			gain_[1]->save(pre);
			gain_[2]->save(pre);
			gain_[3]->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			tools::load_sw(pre, sw_, 4);
			smpl_->load(pre);
			ch_->load(pre);
			slope_->load(pre);
			window_->load(pre);
			level_->load(pre);
			gain_[0]->load(pre);
			gain_[1]->load(pre);
			gain_[2]->load(pre);
			gain_[3]->load(pre);
		}
	};
}
