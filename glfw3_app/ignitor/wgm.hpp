#pragma once
//=====================================================================//
/*! @file
    @brief  WGM クラス
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

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  wgm クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wgm
	{
		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;

	public:
		// WGM 設定
		gui::widget_check*		sw_[5];	///< ジェネレータ接続スイッチ
		gui::widget_check*		ena_;	///< ジェネレータ有効、無効
		gui::widget_list*		mode_;	///< ジェネレータモード（矩形波、三角波、直流）
		gui::widget_label*		freq_;	///< ジェネレータ設定・周波数（1Hz to 100Hz / 1Hz)
		gui::widget_label*		volt_;	///< ジェネレータ設定・電圧（0 to 14V / 0.1V）
		gui::widget_label*		duty_;	///< ジェネレーター設定・Duty（0.1% to 100% / 0.1%）
		gui::widget_check*		iena_;	///< ジェネレータ内臓電源、有効、無効
		gui::widget_label*		ivolt_;	///< ジェネレータ設定・内臓電源
		gui::widget_button*		exec_;	///< ジェネレーター設定転送
		gui::widget_check*		all_;	///< ジェネレータ全体

	private:
		struct wgm_t {
			uint16_t	sw;		///< 5 bits
			bool		ena;	///< 0, 1
			bool		type;	///< 0, 1
			uint16_t   	frq;	///< 7 bits
			uint16_t	duty;	///< 10 bits
			uint16_t	volt;	///< 10 bits
			bool		iena;	///< 内臓電源
			uint32_t	ivolt;	///< 内臓電源電圧

			wgm_t() : sw(0), ena(0), type(0), frq(0), duty(0), volt(0), iena(false), ivolt(0) { }

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

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		wgm(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc) :
			director_(d), client_(client), interlock_(ilc),
			ena_(nullptr), mode_(nullptr), freq_(nullptr),
			volt_(nullptr), duty_(nullptr), iena_(nullptr), ivolt_(nullptr),
			exec_(nullptr), all_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  リセット・スイッチ
		*/
		//-----------------------------------------------------------------//
		void reset_sw()
		{
			for(uint32_t i = 0; i < 5; ++i) {
				sw_[i]->set_check(false);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  スタートアップ
		*/
		//-----------------------------------------------------------------//
		void startup()
		{
			wgm_t t;
			client_.send_data(t.build());
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
				widget_text::param wp_("WGM:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			tools::init_sw(wd, root, interlock_, ofsx, ofsy, sw_, 5, 44, pg);
			ofsy += 50;
			// ジェネレーター・モジュール
			{
				widget::param wp(vtx::irect(ofsx, ofsy, 100, 40), root);
				wp.pre_group_ = pg;
				widget_check::param wp_("有効");
				ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // (6) ジェネレータ設定： 出力モード選択（矩形波/三角波/直流）
			   // タイプ（連続、単発）、
			   // 出力電圧、周波数、ON時間（レンジ：0.01ms）
				widget::param wp(vtx::irect(ofsx + 90, ofsy, 110, 40), root);
				wp.pre_group_ = pg;
				widget_list::param wp_;
				wp_.init_list_.push_back("矩形波");
				wp_.init_list_.push_back("三角波");
				mode_ = wd.add_widget<widget_list>(wp, wp_);
				mode_->at_local_param().select_func_ = [=](const std::string& str, uint32_t pos) {
					if(pos == 0) {
						duty_->set_stall(false);
					} else {
						duty_->set_stall();
					}
				};
			}
			{  // ジェネレータ設定、周波数（1Hz to 100Hz, 1Hz/step)
				widget::param wp(vtx::irect(ofsx + 230 + 130 * 0, ofsy, 70, 40), root);
				wp.pre_group_ = pg;
				widget_label::param wp_("1", false);
				freq_ = wd.add_widget<widget_label>(wp, wp_);
				freq_->at_local_param().select_func_ = [=](const std::string& str) {
					freq_->set_text(tools::limitf(str, 1.0f, 100.0f, "%1.0f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 0 + 80, ofsy, 30, 40),
						root);
					wp.pre_group_ = pg;
					widget_text::param wp_("Hz");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、電圧（0V to 14V, 0.1V/step)
				widget::param wp(vtx::irect(ofsx + 230 + 120 * 1, ofsy, 70, 40), root);
				wp.pre_group_ = pg;
				widget_label::param wp_("0", false);
				volt_ = wd.add_widget<widget_label>(wp, wp_);
				volt_->at_local_param().select_func_ = [=](const std::string& str) {
					volt_->set_text(tools::limitf(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 1 + 80, ofsy, 30, 40),
									 root);
					wp.pre_group_ = pg;
					widget_text::param wp_("V");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、DUTY（0.1% to 100%, 0.1%/step）
				widget::param wp(vtx::irect(ofsx + 230 + 120 * 2, ofsy, 70, 40), root);
				wp.pre_group_ = pg;
				widget_label::param wp_("0.1", false);
				duty_ = wd.add_widget<widget_label>(wp, wp_);
				duty_->at_local_param().select_func_ = [=](const std::string& str) {
					duty_->set_text(tools::limitf(str, 0.1f, 50.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 2 + 80, ofsy, 30, 40),
									 root);
					wp.pre_group_ = pg;
					widget_text::param wp_("%");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			// ジェネレーター内臓電源
			{
				int ofs = 230 + 120 * 2 + 80 + 40;
				widget::param wp(vtx::irect(ofsx + ofs, ofsy, 80, 40), root);
				wp.pre_group_ = pg;
				widget_check::param wp_("電源");
				iena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 内臓電源設定
				int ofs = 230 + 120 * 2 + 80 + 40 + 90;
				widget::param wp(vtx::irect(ofsx + ofs, ofsy, 70, 40), root);
				wp.pre_group_ = pg;
				widget_label::param wp_("0", false);
				ivolt_ = wd.add_widget<widget_label>(wp, wp_);
				ivolt_->at_local_param().select_func_ = [=](const std::string& str) {
					ivolt_->set_text(tools::limitf(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					int ofs = 230 + 120 * 2 + 80 + 40 + 90 + 80;
					widget::param wp(vtx::irect(ofsx + ofs, ofsy, 30, 40), root);
					wp.pre_group_ = pg;
					widget_text::param wp_("V");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}

			{
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				wp.pre_group_ = pg;
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					wgm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = ena_->get_check();
					t.type = mode_->get_select_pos() & 1;
					float v;
					if((utils::input("%f", freq_->get_text().c_str()) % v).status()) {
						t.frq = v;
					}
					if((utils::input("%f", duty_->get_text().c_str()) % v).status()) {
						t.duty = v * 10.0f;
					}
					if((utils::input("%f", volt_->get_text().c_str()) % v).status()) {
						t.volt = v / 0.02f;
					}
					t.iena = iena_->get_check();
					if((utils::input("%f", ivolt_->get_text().c_str()) % v).status()) {
						t.ivolt = v / 15.626e-6;
					}
					auto s = t.build();
// std::cout << s << std::endl;
					client_.send_data(s);
				};
			}
			{
				widget::param wp(vtx::irect(d_w - 45, ofsy, 30, 30), root);
				wp.pre_group_ = pg;
				widget_check::param wp_;
				all_ = wd.add_widget<widget_check>(wp, wp_);
				all_->at_local_param().select_func_ = [=](bool ena) {
					if(!ena) {
						tools::set_checks(sw_, false, 5);
						startup();
					}
					for(uint32_t i = 0; i < 5; ++i) {
						sw_[i]->set_stall(!ena, widget::STALL_GROUP::_1);
					}
					ena_->set_stall(!ena, widget::STALL_GROUP::_1);
					mode_->set_stall(!ena, widget::STALL_GROUP::_1);
					freq_->set_stall(!ena, widget::STALL_GROUP::_1);
					volt_->set_stall(!ena, widget::STALL_GROUP::_1);
					duty_->set_stall(!ena, widget::STALL_GROUP::_1);
					iena_->set_stall(!ena, widget::STALL_GROUP::_1);
					ivolt_->set_stall(!ena, widget::STALL_GROUP::_1);
					exec_->set_stall(!ena, widget::STALL_GROUP::_1);
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
			exec_->set_stall(!client_.probe());
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
			if(freq_->get_focus()) {
				tools::set_help(chip, freq_, "1 to 100 [Hz], 1 [Hz] / step");
			} else if(volt_->get_focus()) {
				tools::set_help(chip, volt_, "0.0 to 14.0 [V]");
			} else if(duty_->get_focus()) {
				tools::set_help(chip, duty_, "0.1 to 50.0 [%]");
			} else if(ivolt_->get_focus()) {
				tools::set_help(chip, ivolt_, "0.0 to 16.0 [V]");
			} else if(all_->get_focus()) {
				tools::set_help(chip, all_, "WGM ON/OFF");
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
			tools::save_sw(pre, sw_, 5);
			ena_->save(pre);
			mode_->save(pre);
			freq_->save(pre);
			volt_->save(pre);
			duty_->save(pre);
			iena_->save(pre);
			ivolt_->save(pre);
			all_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			tools::load_sw(pre, sw_, 5);
			ena_->load(pre);
			mode_->load(pre);
			freq_->load(pre);
			volt_->load(pre);
			duty_->load(pre);
			iena_->load(pre);
			ivolt_->load(pre);
			all_->load(pre);
			all_->exec();
		}
	};
}

