#pragma once
//=====================================================================//
/*! @file
    @brief  波形キャプチャー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <array>
#include "core/glcore.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_sheet.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_utils.hpp"

#include "gl_fw/render_waves.hpp"

#include "ign_client_tcp.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  wave_cap クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wave_cap {

		typedef utils::director<core> DR;
		DR&						director_;

		typedef net::ign_client_tcp CLIENT;
		CLIENT&					client_;

		typedef view::render_waves<uint16_t, CLIENT::WAVE_BUFF_SIZE, 4> WAVES;
		WAVES					waves_;

		gui::widget_frame*		frame_;
		gui::widget_view*		core_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_frame*		tools_;

		gui::widget_check*		sw_[4];
		gui::widget_list*		time_div_;
		gui::widget_list*		trg_ch_;
		gui::widget_list*		trg_slope_;
		gui::widget_spinbox*	trg_window_;
		gui::widget_label*		trg_level_;
		gui::widget_list*		ch_gain_[4];
		gui::widget_button*		exec_;

		gui::widget_sheet*		share_frame_;

		double					sample_rate_;

		uint32_t				wdm_st_[4];

		static const uint32_t time_div_size_ = 16;
		double get_time_div_() const {
			if(time_div_ == nullptr) return 0.0;

			static constexpr double tbls[time_div_size_] = {
				1.0 / 1e3,
				1.0 / 2e3,
				1.0 / 5e3,
				1.0 / 10e3,
				1.0 / 20e3,
				1.0 / 50e3,
				1.0 / 100e3,
				1.0 / 200e3,
				1.0 / 500e3,
				1.0 / 1e6,
				1.0 / 2e6,
				1.0 / 5e6,
				1.0 / 10e6,
				1.0 / 20e6,
				1.0 / 50e6,
				1.0 / 100e6
			};
			return tbls[time_div_->get_select_pos() % time_div_size_];
		}

		// チャネル毎の電圧スケールサイズ
		static uint32_t get_volt_scale_size_(uint32_t ch) {
			if(ch == 0) return 8;
			else if(ch == 1) return 12;
			else if(ch == 2) return 10;
			else return 9;
		}


		static float get_volt_scale_value_(uint32_t ch, uint32_t idx) {
			if(ch == 0) {
				static const float tbl[8] = {
					0.25f, 0.5f, 1.0f, 2.0f, 2.5f, 5.0f, 7.5f, 10.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];
			} else if(ch == 1) {
				static const float tbl[12] = {
					1.0f, 2.5f, 5.0f, 10.0f, 20.0f, 25.0f, 50.0f, 75.0f,
					100.0f, 125.0f, 150.0f, 200.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];
			} else if(ch == 2) {
				static const float tbl[10] = {
					0.05f, 0.1f, 0.2f, 0.25f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 5.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];
			} else {
				static const float tbl[9] = {
					0.5f, 1.0f, 1.25f, 2.5f, 5.0f, 7.5f, 10.0f, 15.0f, 20.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];				
			}
		}

		static float get_volt_scale_limit_(uint32_t ch) {
			if(ch == 0) return 32.768f;
			else if(ch == 1) return 655.36f;
			else if(ch == 2) return 16.384f;
			else return 65.536f;
		}


		static const uint32_t time_unit_size_ = 20;
		static float get_time_unit_(uint32_t idx) {
			static const float tbl[time_unit_size_] = {
				100e-9, 250e-9, 500e-9,   1e-6,
				2.5e-6,   5e-6,  10e-6,  25e-6,
				 50e-6, 100e-6, 250e-6, 500e-6,
				  1e-3, 2.5e-3,   5e-3,  10e-3,
				 25e-3,  50e-3,  75e-3, 100e-3
			};
			return tbl[idx % time_unit_size_];
		}
		static float get_time_unit_base_(uint32_t idx) {
			static const float tbl[time_unit_size_] = {
				1e-9, 1e-9, 1e-9, 1e-6,
				1e-6, 1e-6, 1e-6, 1e-6,
				1e-6, 1e-6, 1e-6, 1e-6,
				1e-3, 1e-3, 1e-3, 1e-3,
				1e-3, 1e-3, 1e-3, 1e-3
			};
			return tbl[idx % time_unit_size_];
		}
		static const char* get_time_unit_str_(uint32_t idx) {
			static const char* tbl[time_unit_size_] = {
				"nS", "nS", "nS", "uS",
				"uS", "uS", "uS", "uS",
				"uS", "uS", "uS", "uS",
				"mS", "mS", "mS", "mS",
				"mS", "mS", "mS", "mS"
			};
			return tbl[idx % time_unit_size_];
		}

		static uint32_t get_time_scale_(uint32_t idx, uint32_t grid, float rate) {
			float g = static_cast<float>(grid);
			float step = get_time_unit_(idx) / g;
			return static_cast<uint32_t>(65536.0f * step / rate);
		}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  チャネル・クラス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct chn_t {
			WAVES&					waves_;	///< 波形レンダリング
			gui::widget_null*		root_;	///< ルート
			gui::widget_check*		ena_;	///< チャネル有効、無効
			gui::widget_check*		gnd_;	///< GND 電位
			gui::widget_spinbox*	pos_;  	///< 水平位置
			gui::widget_spinbox*	scale_;	///< 電圧スケール
			gui::widget_check*		mes_;	///< メジャー有効
			gui::widget_spinbox*	org_;	///< 計測開始位置
			gui::widget_spinbox*	len_;	///< 計測長さ

			// fs: フルスケール電圧
			chn_t(WAVES& waves, float fs) : waves_(waves), root_(nullptr),
				ena_(nullptr), gnd_(nullptr), pos_(nullptr), scale_(nullptr),
				mes_(nullptr), org_(nullptr), len_(nullptr)
			{ }


			void init(DR& dr, gui::widget_sheet* share, int ch)
			{
				using namespace gui;
				auto& wd = dr.at().widget_director_;

				{  // 仮想フレーム
					widget::param wp(vtx::irect(0, 0, 0, 0), share);
					widget_null::param wp_;
					root_ = wd.add_widget<widget_null>(wp, wp_);
					share->add((boost::format("CH%d") % (ch + 1)).str(), root_);
				}
				{  // 有効、無効
					widget::param wp(vtx::irect(10, 30, 80, 40), root_);
					widget_check::param wp_((boost::format("CH%d") % (ch + 1)).str());
					ena_ = wd.add_widget<widget_check>(wp, wp_);
					ena_->at_local_param().select_func_ = [=](bool f) {
						waves_.at_param(ch).render_ = f;
					};
				}
				{  // GND 電位
					widget::param wp(vtx::irect(10 + 90, 30, 80, 40), root_);
					widget_check::param wp_("GND");
					gnd_ = wd.add_widget<widget_check>(wp, wp_);
				}
				{  // 電圧位置 (+-1.0)
					widget::param wp(vtx::irect(10, 70, 130, 40), root_);
					widget_spinbox::param wp_(0, -20, 20); // grid 数の倍
					pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
					pos_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						int wy = share->get_param().rect_.size.y;
						float a = static_cast<float>(-newpos)
							/ static_cast<float>(wy * 2 / waves_.get_info().grid_step_);
						return (boost::format("%3.2f") % a).str();
					};
				}
				{  // 電圧スケール
					widget::param wp(vtx::irect(10 + 140, 70, 160, 40), root_);
					widget_spinbox::param wp_(0, 0, (get_volt_scale_size_(ch) - 1));
					scale_ = wd.add_widget<widget_spinbox>(wp, wp_);
					scale_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						// グリッドに対する電圧表示
						auto a = get_volt_scale_value_(ch, newpos);
						static const char unit[4] = { 'A', 'V', 'V', 'V' };
						return (boost::format("%3.2f %c") % a % unit[ch]).str();
					};
				}
				{  // メジャー有効、無効
					widget::param wp(vtx::irect(10, 120, 130, 40), root_);
					widget_check::param wp_("Measure");
					mes_ = wd.add_widget<widget_check>(wp, wp_);
				}
				{  // メジャー開始位置
					widget::param wp(vtx::irect(10, 170, 130, 40), root_);
					widget_spinbox::param wp_(0, 0, 100);
					org_ = wd.add_widget<widget_spinbox>(wp, wp_);
					org_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						float a = static_cast<float>(newpos) 
							/ static_cast<float>(waves_.get_info().grid_step_);
						return (boost::format("%3.2f") % a).str();
					};
				}
				{  // メジャー長さ
					widget::param wp(vtx::irect(10 + 140, 170, 160, 40), root_);
					widget_spinbox::param wp_(0, 0, 100); // grid 数の倍
					len_ = wd.add_widget<widget_spinbox>(wp, wp_);
					len_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						// メジャーに対する電圧、電流表示
						float a = static_cast<float>(-newpos) 
							/ static_cast<float>(waves_.get_info().grid_step_)
							* get_volt_scale_value_(ch, scale_->get_select_pos());
						static const char unit[4] = { 'A', 'V', 'V', 'V' };
						return (boost::format("%3.2f %c") % a % unit[ch]).str();
					};
				}
			}


			void update(uint32_t ch, const vtx::ipos& size, bool mena)
			{
				if(pos_ == nullptr || scale_ == nullptr) return;
				if(org_ == nullptr || len_ == nullptr) return;

				int grid = waves_.get_info().grid_step_;

				auto pos = (size.y / grid) * 3 / 2;
				pos_->at_local_param().min_pos_ = -pos;
				pos_->at_local_param().max_pos_ =  pos;

				int msofs = size.y / 2;
				org_->at_local_param().min_pos_ = -msofs;
				org_->at_local_param().max_pos_ =  msofs;
				org_->at_local_param().page_div_ = 0;
				org_->at_local_param().page_step_ = -grid;
				len_->at_local_param().min_pos_ = -size.y;
				len_->at_local_param().max_pos_ =  size.y;
				len_->at_local_param().page_div_ = 0;
				len_->at_local_param().page_step_ = -grid;

				// 波形位置
				waves_.at_param(ch).offset_.y = (size.y / 2)
					+ pos_->get_select_pos() * (grid / 2);
				// 波形拡大、縮小
				float value = get_volt_scale_value_(ch, scale_->get_select_pos())
					/ static_cast<float>(grid);
				float u = get_volt_scale_limit_(ch) / static_cast<float>(32768);
				if(gnd_->get_check()) u = 0.0f;
				waves_.at_param(ch).gain_ = u / value;

				// 電圧計測設定
				if(mena) {
					if(mes_->get_check()) {
						waves_.at_info().volt_enable_ = true;
						waves_.at_info().volt_org_ = org_->get_select_pos() + msofs;
						waves_.at_info().volt_len_ = len_->get_select_pos();
					} else {
						waves_.at_info().volt_enable_ = false;
					}
				}
			}


			void load(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->load(pre);
				}
				if(gnd_ != nullptr) {
					gnd_->load(pre);
				}
				if(pos_ != nullptr) {
					pos_->load(pre);
				}
				if(scale_ != nullptr) {
					scale_->load(pre);
				}
				if(mes_ != nullptr) {
					mes_->load(pre);
				}
				if(org_ != nullptr) {
					org_->load(pre);
				}
				if(len_ != nullptr) {
					len_->load(pre);
				}
			}


			void save(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->save(pre);
				}
				if(gnd_ != nullptr) {
					gnd_->save(pre);
				}
				if(pos_ != nullptr) {
					pos_->save(pre);
				}
				if(scale_ != nullptr) {
					scale_->save(pre);
				}
				if(mes_ != nullptr) {
					mes_->save(pre);
				}
				if(org_ != nullptr) {
					org_->save(pre);
				}
				if(len_ != nullptr) {
					len_->save(pre);
				}
			}
		};
		chn_t			chn0_;
		chn_t			chn1_;
		chn_t			chn2_;
		chn_t			chn3_;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  時間軸計測クラス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct measure_t {
			WAVES&					waves_;

			gui::widget_null*		root_;
			gui::widget_check*		ena_;
			gui::widget_spinbox*   	org_;
			gui::widget_spinbox*   	len_;
			gui::widget_text*		frq_;	///< 周波数表示
			uint32_t				tbp_;	///< タイム・ベース位置

			measure_t(WAVES& waves) : waves_(waves), root_(nullptr),
				ena_(nullptr), org_(nullptr), len_(nullptr),
				frq_(nullptr),
				tbp_(0)
			{ }


			void init(DR& dr, gui::widget_sheet* share, const std::string& text)
			{
				using namespace gui;
				widget_director& wd = dr.at().widget_director_;

				{  // 仮想フレーム
					widget::param wp(vtx::irect(0, 0, 0, 0), share);
					widget_null::param wp_;
					root_ = wd.add_widget<widget_null>(wp, wp_);
					share->add(text, root_);
				}
				{  // メジャー有効、無効
					widget::param wp(vtx::irect(10, 30, 140, 40), root_);
					widget_check::param wp_("Enable");
					ena_ = wd.add_widget<widget_check>(wp, wp_);
					ena_->at_local_param().select_func_ = [=](bool f) {
						waves_.at_info().time_enable_ = f;
					};
				}
				{
					widget::param wp(vtx::irect(10, 70, 200, 40), root_);
					widget_spinbox::param wp_(0, 0, 100);
					org_ = wd.add_widget<widget_spinbox>(wp, wp_);
					org_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						float t = get_time_unit_(tbp_) * static_cast<float>(newpos)
							/ waves_.get_info().grid_step_;
						float a = t / get_time_unit_base_(tbp_);
						return (boost::format("%2.1f %s") % a % get_time_unit_str_(tbp_)).str();
					};
				}
				{
					widget::param wp(vtx::irect(10, 120, 200, 40), root_);
					widget_spinbox::param wp_(0, 0, 100);
					len_ = wd.add_widget<widget_spinbox>(wp, wp_);
					len_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						float t = get_time_unit_(tbp_) * static_cast<float>(newpos)
							/ waves_.get_info().grid_step_;
						float a = t / get_time_unit_base_(tbp_);
						float f = 1.0f / t;
						frq_->set_text((boost::format("%2.1f Hz") % f).str());
						return (boost::format("%2.1f %s") % a % get_time_unit_str_(tbp_)).str();
					};
				}
				{
					widget::param wp(vtx::irect(10, 170, 150, 40), root_);
					widget_text::param wp_;
					frq_ = wd.add_widget<widget_text>(wp, wp_);
				}
			}


			void update(const vtx::ipos& size)
			{
				if(org_ == nullptr || len_ == nullptr) return;

				auto grid = waves_.get_info().grid_step_;

				org_->at_local_param().min_pos_ = 0;
				org_->at_local_param().max_pos_ = size.x;
				org_->at_local_param().page_div_ = 0;
				org_->at_local_param().page_step_ = grid;
				waves_.at_info().time_org_ = org_->get_select_pos();

				len_->at_local_param().min_pos_ = 0;
				len_->at_local_param().max_pos_ = size.x;
				len_->at_local_param().page_div_ = 0;
				len_->at_local_param().page_step_ = grid;
				waves_.at_info().time_len_ = len_->get_select_pos();
			}


			void load(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->load(pre);
				}
				if(org_ != nullptr) {
					org_->load(pre);
				}
				if(len_ != nullptr) {
					len_->load(pre);
				}
			}


			void save(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->save(pre);
				}
				if(org_ != nullptr) {
					org_->save(pre);
				}
				if(len_ != nullptr) {
					len_->save(pre);
				}
			}
		};
		measure_t				measure_time_;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  時間軸クラス
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct time_t {
			WAVES&					waves_;

			gui::widget_null*		root_;

			gui::widget_spinbox*	scale_;		///< 時間スケール
			gui::widget_spinbox*	offset_;	///< 時間オフセット
			gui::widget_button*		res_ofs_;	///< 時間オフセット、リセット
			gui::widget_check*		trg_gate_;	///< トリガー表示

			time_t(WAVES& waves) : waves_(waves),
				root_(nullptr),
				scale_(nullptr), offset_(nullptr), res_ofs_(nullptr), trg_gate_(nullptr)
			{ }


			void init(DR& dr, gui::widget_sheet* share)
			{
				using namespace gui;
				auto& wd = dr.at().widget_director_;

				{  // 仮想フレーム
					widget::param wp(vtx::irect(0, 0, 0, 0), share);
					widget_null::param wp_;
					root_ = wd.add_widget<widget_null>(wp, wp_);
					share->add("Time Scale", root_);
				}
				{  // タイム・スケール
					widget::param wp(vtx::irect(10, 40, 200, 40), root_);
					widget_spinbox::param wp_(0, 0, time_unit_size_ - 1);
					scale_ = wd.add_widget<widget_spinbox>(wp, wp_);
					scale_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						float a = get_time_unit_(newpos) / get_time_unit_base_(newpos);
						return (boost::format("%2.1f %s") % a % get_time_unit_str_(newpos)).str();
					};
				}
				{  // タイム・オフセット（グリッド単位）
					widget::param wp(vtx::irect(10, 90, 200, 40), root_);
					widget_spinbox::param wp_(0, 0, 50);
					offset_ = wd.add_widget<widget_spinbox>(wp, wp_);
					offset_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						float t = get_time_unit_(scale_->get_select_pos()) * newpos;
						auto un = scale_->get_select_pos();
						float a = t / get_time_unit_base_(un);
						return (boost::format("%2.1f %s") % a % get_time_unit_str_(un)).str();
					};
				}
				{  // オフセット・リセット
					widget::param wp(vtx::irect(10 + 215, 95, 30, 30), root_);
					widget_button::param wp_("R");
					res_ofs_ = wd.add_widget<widget_button>(wp, wp_);
					res_ofs_->at_local_param().select_func_
						= [=](uint32_t id) {
						offset_->set_select_pos(0);
					};
				}
				{  // トリガー・ゲート有効、無効
					widget::param wp(vtx::irect(10, 140, 140, 40), root_);
					widget_check::param wp_("Trigger");
					trg_gate_ = wd.add_widget<widget_check>(wp, wp_);
				}
			}


			void update(const vtx::ipos& size, float rate)
			{
				int grid = waves_.get_info().grid_step_;

				auto ts = get_time_scale_(scale_->get_select_pos(), grid, rate);
				int w = (waves_.size() / 2 * ts / 65536 - size.x) / grid;
				offset_->at_local_param().min_pos_ = -w;
				offset_->at_local_param().max_pos_ =  w;

				auto ofs = offset_->get_select_pos();
				// 波形のトリガー先頭
				ofs += CLIENT::WAVE_BUFF_SIZE / 2;
				waves_.at_param(0).offset_.x = ofs * grid;
				waves_.at_param(1).offset_.x = ofs * grid;
				waves_.at_param(2).offset_.x = ofs * grid;
				waves_.at_param(3).offset_.x = ofs * grid;

				if(trg_gate_->get_check()) {
					waves_.at_info().trig_enable_ = true;
					waves_.at_info().trig_pos_ = -offset_->get_select_pos() * grid;
				} else {
					waves_.at_info().trig_enable_ = false;
				}
			}


			void load(sys::preference& pre)
			{
				if(scale_ != nullptr) {
					scale_->load(pre);
				}
				if(offset_ != nullptr) {
					offset_->load(pre);
				}
				if(trg_gate_ != nullptr) {
					trg_gate_->load(pre);
				}
			}


			void save(sys::preference& pre)
			{
				if(scale_ != nullptr) {
					scale_->save(pre);
				}
				if(offset_ != nullptr) {
					offset_->save(pre);
				}
				if(trg_gate_ != nullptr) {
					trg_gate_->save(pre);
				}
			}
		};
		time_t					time_;

		vtx::ipos				size_;


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
			cmd = (0b00010000 << 16) | (smtbl[time_div_->get_select_pos() % 16] << 8);
			s += (boost::format("wdm %06X\n") % cmd).str();
			// channel gain
			for(int i = 0; i < 4; ++i) {
				cmd = ((0b00011000 | (i + 1)) << 16) | ((ch_gain_[i]->get_select_pos() % 8) << 13);
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			{ // trigger level
				cmd = (0b00101000 << 16);
				int v;
				if((utils::input("%d", trg_level_->get_text().c_str()) % v).status()) {
					if(v < 1) v = 1;
					else if(v > 65534) v = 65534;
					cmd |= v;
					s += (boost::format("wdm %06X\n") % cmd).str();
				}
			}
			{  // trigger channel
				cmd = (0b00100000 << 16);
				auto n = trg_ch_->get_select_pos();
				uint8_t sub = 0;
				sub |= 0x80;  // trigger ON
				cmd |= static_cast<uint32_t>(n & 3) << 14;
				if(trg_slope_->get_select_pos()) sub |= 0x40;
				sub |= trg_window_->get_select_pos() & 15;
				cmd |= sub;
				s += (boost::format("wdm %06X\n") % cmd).str();
			}
			return s;
		}

		// 波形描画
		void update_view_()
		{
		}


		void render_view_(const vtx::irect& clip)
		{
			glDisable(GL_TEXTURE_2D);
			uint32_t n = 0;
			if(time_.scale_ != nullptr) {
				n = time_.scale_->get_select_pos();
			}
			uint32_t step = get_time_scale_(n, waves_.get_info().grid_step_, sample_rate_);
static int nnn = 0;
if(nnn >= 60) {
	// std::cout << "Step: " << step << std::endl;
	nnn = 0;
} else {
	++nnn;
}

			waves_.render(clip.size, step);
			glEnable(GL_TEXTURE_2D);
			size_ = clip.size;
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
		wave_cap(utils::director<core>& d, CLIENT& client) : director_(d),
			client_(client),
			waves_(), frame_(nullptr), core_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			tools_(nullptr),
			sw_{ nullptr }, time_div_(nullptr), trg_ch_(nullptr), trg_slope_(nullptr),
			trg_window_(nullptr), trg_level_(nullptr), ch_gain_{ nullptr },
			exec_(nullptr), share_frame_(nullptr), sample_rate_(1e-6),
			wdm_st_{ 0 },
			chn0_(waves_, 1.25f),
			chn1_(waves_, 1.25f),
			chn2_(waves_, 1.25f),
			chn3_(waves_, 1.25f),
			measure_time_(waves_),
			time_(waves_), size_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			size_.set(400, 400);
			{  // 波形描画フレーム
				widget::param wp(vtx::irect(40, 150, size_.x, size_.y));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{  // 波形描画ビュー 
				widget::param wp(vtx::irect(0), frame_);
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
				core_ = wd.add_widget<widget_view>(wp, wp_);
			}

			{	// ターミナル
				{
					widget::param wp(vtx::irect(20, 100, 100, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(12);
					terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), terminal_frame_);
					widget_terminal::param wp_;
					wp_.enter_func_ = [=](const utils::lstring& text) {
						// term_enter_(text);
					};
					terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
					term_chaout::set_output(terminal_core_);
				}
			}

			int mw = 330;
			int mh = 600;
			{	// 波形ツールフレーム
				widget::param wp(vtx::irect(100, 100, mw, mh));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}
			{	// 共有フレーム（プロパティシート）
				widget::param wp(vtx::irect(5, 280, mw - 10, mh - 280 - 5), tools_);
				widget_sheet::param wp_;
				share_frame_ = wd.add_widget<widget_sheet>(wp, wp_);
			}

			for(uint32_t i = 0; i < 4; ++i) {  //  WMD スイッチ設定
				widget::param wp(vtx::irect(20 + 65 * i, 22, 65, 40), tools_);
				widget_check::param wp_((boost::format("%d") % (29 + i)).str());
				sw_[i] = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 時間軸リスト 10K、20K、50K、100K、200K、500K、1M、2M、5M、10M、20M、50M、100M
				widget::param wp(vtx::irect(20, 22 + 50, 110, 40), tools_);
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
				time_div_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・チャネル選択
				widget::param wp(vtx::irect(20 + 120, 22 + 50, 80, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				wp_.init_list_.push_back("CH4");
				trg_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・スロープ選択
				widget::param wp(vtx::irect(20 + 210, 22 + 50, 80, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Fall");
				wp_.init_list_.push_back("Rise");
				trg_slope_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・ウィンドウ（１～１５）
				widget::param wp(vtx::irect(20, 22 + 100, 90, 40), tools_);
				widget_spinbox::param wp_(1, 1, 15);
				trg_window_ = wd.add_widget<widget_spinbox>(wp, wp_);
				trg_window_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(20 + 100, 22 + 100, 100, 40), tools_);
				widget_label::param wp_("32768", false);
				trg_level_ = wd.add_widget<widget_label>(wp, wp_);
				trg_level_->at_local_param().select_func_ = [=](const std::string& str) {
					trg_level_->set_text(limiti_(str, 1, 65534, "%d"));
				};
			}
			{  // exec
				widget::param wp(vtx::irect(mw - 40, 22 + 200, 30, 30), tools_);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					sample_rate_ = get_time_div_();
					auto s = build_wdm_();
					client_.send_data(s);
				};
			}

			for(int i = 0; i < 4; ++i) {  // 各チャネル減衰設定
				static const vtx::ipos ofs[4] = {
					{ 0, 0 }, { 110, 0 }, { 0, 50 }, { 110, 50 }
				};
				widget::param wp(vtx::irect(20 + ofs[i].x, 22 + 150 + ofs[i].y,
					100, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("-22dB");
				wp_.init_list_.push_back("-16dB");
				wp_.init_list_.push_back("-10dB");
				wp_.init_list_.push_back(" -4dB");
				wp_.init_list_.push_back("  2dB");
				wp_.init_list_.push_back("  8dB");
				wp_.init_list_.push_back(" 14dB");
				wp_.init_list_.push_back(" 20dB");
				ch_gain_[i] = wd.add_widget<widget_list>(wp, wp_);
			}

			measure_time_.init(director_, share_frame_, "Time Measure");

			chn0_.init(director_, share_frame_, 0);
			chn1_.init(director_, share_frame_, 1);
			chn2_.init(director_, share_frame_, 2);
			chn3_.init(director_, share_frame_, 3);

			time_.init(director_, share_frame_);

			load();

			waves_.create_buffer();

			waves_.at_param(0).color_ = img::rgba8(255,  64, 255, 255);
			waves_.at_param(1).color_ = img::rgba8( 64, 255, 255, 255);
			waves_.at_param(2).color_ = img::rgba8(255, 255,  64, 255);
			waves_.at_param(3).color_ = img::rgba8( 64, 255,  64, 255);

//			waves_.build_sin(0, 10.0, 1.0f);
//			waves_.build_sin(1, 15.0, 0.75f);
//			waves_.build_sin(2, 20.0, 0.5f);
//			waves_.build_sin(3, 25.0, 0.25f);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(frame_ == nullptr) return;
			if(share_frame_ == nullptr) return;

			exec_->set_stall(!client_.probe());

			measure_time_.tbp_ = time_.scale_->get_select_pos();
			measure_time_.update(size_);

			chn0_.update(0, size_, share_frame_->get_select_pos() == 1);
			chn1_.update(1, size_, share_frame_->get_select_pos() == 2);
			chn2_.update(2, size_, share_frame_->get_select_pos() == 3);
			chn3_.update(3, size_, share_frame_->get_select_pos() == 4);
			if(share_frame_->get_select_pos() < 1 || share_frame_->get_select_pos() > 4) {
				waves_.at_info().volt_enable_ = false;				
			}

			time_.update(size_, sample_rate_);

			// 波形のコピー
			for(uint32_t i = 0; i < 4; ++i) {
				auto st = client_.get_wdm_st(i);
				if(wdm_st_[i] != st) {
					waves_.copy(i, client_.get_wdm(i), waves_.size());
					wdm_st_[i] = st;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
		*/
		//-----------------------------------------------------------------//
		void load()
		{
			sys::preference& pre = director_.at().preference_;

			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(tools_ != nullptr) {
				tools_->load(pre);
			}
			if(share_frame_ != nullptr) {
				share_frame_->load(pre);
			}

			for(int i = 0; i < 4; ++i) {
				if(sw_[i] != nullptr) {
					sw_[i]->load(pre);
				}
			}
			if(time_div_ != nullptr) {
				time_div_->load(pre);
			}
			if(trg_ch_ != nullptr) {
				trg_ch_->load(pre);
			}
			if(trg_slope_ != nullptr) {
				trg_slope_->load(pre);
			}
			if(trg_window_ != nullptr) {
				trg_window_->load(pre);
			}
			if(trg_level_ != nullptr) {
				trg_level_->load(pre);
			}
			for(int i = 0; i < 4; ++i) {
				if(ch_gain_[i] != nullptr) {
					ch_gain_[i]->load(pre);
				}
			}

			chn0_.load(pre);
			chn1_.load(pre);
			chn2_.load(pre);
			chn3_.load(pre);

			time_.load(pre);

			measure_time_.load(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
		*/
		//-----------------------------------------------------------------//
		void save()
		{
			sys::preference& pre = director_.at().preference_;

			if(frame_ != nullptr) {
				frame_->save(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(tools_ != nullptr) {
				tools_->save(pre);
			}
			if(share_frame_ != nullptr) {
				share_frame_->save(pre);
			}

			for(int i = 0; i < 4; ++i) {
				if(sw_[i] != nullptr) {
					sw_[i]->save(pre);
				}
			}
			if(time_div_ != nullptr) {
				time_div_->save(pre);
			}
			if(trg_ch_ != nullptr) {
				trg_ch_->save(pre);
			}
			if(trg_slope_ != nullptr) {
				trg_slope_->save(pre);
			}
			if(trg_window_ != nullptr) {
				trg_window_->save(pre);
			}
			if(trg_level_ != nullptr) {
				trg_level_->save(pre);
			}
			for(int i = 0; i < 4; ++i) {
				if(ch_gain_[i] != nullptr) {
					ch_gain_[i]->save(pre);
				}
			}

			chn0_.save(pre);
			chn1_.save(pre);
			chn2_.save(pre);
			chn3_.save(pre);

			time_.save(pre);

			measure_time_.save(pre);
		}
	};
}
