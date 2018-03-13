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
#include "widgets/widget_utils.hpp"
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
#include "widgets/widget_chip.hpp"

#include "gl_fw/render_waves.hpp"
#include "img_io/img_files.hpp"

#include "ign_client_tcp.hpp"
#include "interlock.hpp"
#include "test.hpp"

// #define TEST_SIN

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  wave_cap クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wave_cap {

		static constexpr const char* WAVE_DATA_EXT_ = "wad";

	public:
		//=================================================================//
		/*!
			@brief  情報構造体
		*/
		//=================================================================//
		struct info_t {
			double	sample_org_;	///< サンプリング開始時間
			double	sample_width_;	///< サンプリング領域（時間）
			double	sig_[4];		///< 各チャネル電圧
			double	min_[4];		///< 領域の最低値
			double	max_[4];		///< 領域の最大値
			double	average_[4];	///< 領域のアベレージ

			uint32_t	id_;		///< 領域更新 ID

			info_t() : sample_org_(0.0), sample_width_(0.0),
				sig_{ 0.0 }, min_{ 0.0 }, max_{ 0.0 }, average_{ 0.0 },
				id_(0)
			{ }
		};


		//=================================================================//
		/*!
			@brief  サンプリング・パラメーター構造体
		*/
		//=================================================================//
		struct sample_param {
			double	rate;
			double	gain[4];
			sample_param() : rate(1e-6) {
				gain[0] = 1.0;
				gain[1] = 1.0;
				gain[2] = 1.0;
				gain[3] = 1.0;
			}
		};

	private:
		typedef utils::director<core> DR;
		DR&						director_;

		typedef net::ign_client_tcp CLIENT;
		CLIENT&					client_;

		interlock&				interlock_;

		typedef view::render_waves<uint16_t, CLIENT::WAVE_BUFF_SIZE, 4> WAVES;
		WAVES					waves_;

		gui::widget_frame*		frame_;
		gui::widget_view*		core_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_core_;

		gui::widget_frame*		tools_;
		gui::widget_check*		smooth_;
		gui::widget_button*		load_;
		gui::widget_button*		save_;
		gui::widget_list*		mesa_type_;
		gui::widget_label*		mesa_filt_;
		gui::widget_list*		org_trg_;
		gui::widget_spinbox*	org_slope_;
		gui::widget_check*		org_ena_;
		gui::widget_list*		fin_trg_;
		gui::widget_spinbox*	fin_slope_;
		gui::widget_check*		fin_ena_;
		gui::widget_text*		ch_to_time_;
		gui::widget_button*		wdm_exec_;
		uint32_t				meas_id_before_;
		uint32_t				meas_id_;
		float					org_value_;
		float					fin_value_;

		gui::widget_sheet*		share_frame_;

		sample_param			sample_param_;

		uint32_t				wdm_id_[4];
		uint32_t				treg_id_[2];

		bool					info_in_;
		vtx::ipos				info_org_;

		info_t					info_;

		gui::widget_chip*		chip_;

		// チャネル毎の電圧スケールサイズ
		static const uint32_t volt_scale_0_size_ = 8;
		static const uint32_t volt_scale_1_size_ = 15;
		static const uint32_t volt_scale_2_size_ = 10;
		static const uint32_t volt_scale_3_size_ = 12;
		static uint32_t get_volt_scale_size_(uint32_t ch) {
			if(ch == 0) return volt_scale_0_size_;
			else if(ch == 1) return volt_scale_1_size_;
			else if(ch == 2) return volt_scale_2_size_;
			else return volt_scale_3_size_;
		}


		static float get_volt_scale_value_(uint32_t ch, uint32_t idx) {
			if(ch == 0) {
				static const float tbl[volt_scale_0_size_] = {
					0.25f, 0.5f, 1.0f, 2.0f, 2.5f, 5.0f, 7.5f, 10.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];
			} else if(ch == 1) {
				static const float tbl[volt_scale_1_size_] = {
					0.125f, 0.25f, 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f, 25.0f, 50.0f, 75.0f,
					100.0f, 125.0f, 150.0f, 200.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];
			} else if(ch == 2) {
				static const float tbl[volt_scale_2_size_] = {
					0.05f, 0.1f, 0.2f, 0.25f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 5.0f
				};
				return tbl[idx % get_volt_scale_size_(ch)];
			} else {
				static const float tbl[volt_scale_3_size_] = {
					0.0625f, 0.125f, 0.25f, 0.5f, 1.0f, 1.25f, 2.5f, 5.0f,
					7.5f, 10.0f, 15.0f, 20.0f
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
		static double get_time_unit_(uint32_t idx) {
			static const double tbl[time_unit_size_] = {
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
					widget_spinbox::param wp_(-20, 0, 20); // grid 数の倍
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
						static const char* unit[4] = { "A", "V", "V", "KV" };
						org_->exec();
						len_->exec();
						return (boost::format("%3.2f %s") % a % unit[ch]).str();
					};
				}
				{  // メジャー有効、無効
					widget::param wp(vtx::irect(10, 120, 130, 40), root_);
					widget_check::param wp_("Measure");
					mes_ = wd.add_widget<widget_check>(wp, wp_);
				}
				{  // メジャー開始位置
					widget::param wp(vtx::irect(10, 170, 145, 40), root_);
					widget_spinbox::param wp_(0, 0, 100);
					org_ = wd.add_widget<widget_spinbox>(wp, wp_);
					org_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						float a = static_cast<float>(-newpos) 
							/ static_cast<float>(waves_.get_info().grid_step_)
							* get_volt_scale_value_(ch, scale_->get_select_pos());

						float b = static_cast<float>(-pos_->get_select_pos()) / 2.0f;
						b *= get_volt_scale_value_(ch, scale_->get_select_pos());
						return (boost::format("%3.2f") % (a - b)).str();
					};
				}
				{  // メジャー長さ
					widget::param wp(vtx::irect(10 + 155, 170, 145, 40), root_);
					widget_spinbox::param wp_(0, 0, 100); // grid 数の倍
					len_ = wd.add_widget<widget_spinbox>(wp, wp_);
					len_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						// メジャーに対する電圧、電流表示
						float a = static_cast<float>(-newpos) 
							/ static_cast<float>(waves_.get_info().grid_step_)
							* get_volt_scale_value_(ch, scale_->get_select_pos());
						return (boost::format("%3.2f") % a).str();
					};
				}
			}


			void update(uint32_t ch, const vtx::ipos& size, float gainrate)
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
				waves_.at_param(ch).gain_ = u / value / gainrate;

				// 電圧計測設定
				if(mes_->get_check()) {
					waves_.at_info().volt_enable_[ch] = true;
					waves_.at_info().volt_org_[ch] = org_->get_select_pos() + msofs;
					waves_.at_info().volt_len_[ch] = len_->get_select_pos();
				} else {
					waves_.at_info().volt_enable_[ch] = false;
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


		chn_t& at_ch(uint32_t no) {
			switch(no) {
			case 0: return chn0_;
			case 1: return chn1_;
			case 2: return chn2_;
			case 3: return chn3_;
			default:
				return chn0_;
			}
		}


		const chn_t& get_ch(uint32_t no) const {
			switch(no) {
			case 0: return chn0_;
			case 1: return chn1_;
			case 2: return chn2_;
			case 3: return chn3_;
			default:
				return chn0_;
			}
		}



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
						if(t > 0.0f) {
							float f = 1.0f / t;
							if(f >= 1000.0) {
								f /= 1000.0;
								frq_->set_text((boost::format("%4.3f KHz") % f).str());
							} else if(f >= 1000000.0) {
								f /= 1000000.0;
								frq_->set_text((boost::format("%4.3f MHz") % f).str());
							} else {
								frq_->set_text((boost::format("%4.3f Hz") % f).str());
							}
						} else {
							frq_->set_text("---");
						}
						return (boost::format("%3.2f %s") % a % get_time_unit_str_(tbp_)).str();
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

			uint32_t				id_;

			time_t(WAVES& waves) : waves_(waves),
				root_(nullptr),
				scale_(nullptr), offset_(nullptr), res_ofs_(nullptr), trg_gate_(nullptr),
				id_(0)
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
						++id_;
						float a = get_time_unit_(newpos) / get_time_unit_base_(newpos);
						return (boost::format("%2.1f %s") % a % get_time_unit_str_(newpos)).str();
					};
				}
				{  // タイム・オフセット（グリッド単位）
					widget::param wp(vtx::irect(10, 90, 200, 40), root_);
					widget_spinbox::param wp_(-50, 0, 50);
					offset_ = wd.add_widget<widget_spinbox>(wp, wp_);
					offset_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						++id_;
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
					res_ofs_->at_local_param().select_func_ = [=](uint32_t id) {
						++id_;
						offset_->set_select_pos(0);
					};
				}
				{  // トリガー・ゲート有効、無効
					widget::param wp(vtx::irect(10, 140, 140, 40), root_);
					widget_check::param wp_("Trigger");
					trg_gate_ = wd.add_widget<widget_check>(wp, wp_);
					trg_gate_->at_local_param().select_func_ = [=](bool ena) {
						++id_;
					};
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
		uint32_t				time_id_;

		vtx::ipos				size_;

		double					mese_value_;

		void volt_scale_conv_(uint32_t ch, const WAVES::analize_param& ap, info_t& t)
		{ 
			t.min_[ch] = get_volt_scale_limit_(ch) * ap.min_;
			t.max_[ch] = get_volt_scale_limit_(ch) * ap.max_;
			t.average_[ch] = get_volt_scale_limit_(ch) * ap.average_;
		}


		// 波形描画
		void update_view_()
		{
			if(core_->get_select_in()) {
				info_in_ = true;
				info_org_ = core_->get_param().in_point_;
			}
			if(core_->get_selected()) {
				uint32_t n = 0;
				if(time_.scale_ != nullptr) {
					n = time_.scale_->get_select_pos();
				}
				auto msp = core_->get_param().in_point_;
				info_in_ = false;

				auto grid = waves_.get_info().grid_step_;
				int32_t sta = info_org_.x + time_.offset_->get_select_pos() * grid;
				int32_t fin = msp.x + time_.offset_->get_select_pos() * grid;
				if(sta > fin) std::swap(sta, fin);
				auto tu = get_time_unit_(n);
				info_.sample_org_ = static_cast<double>(sta) * tu;
				double org = static_cast<double>(sta) / static_cast<double>(grid) * tu;
				double end = static_cast<double>(fin) / static_cast<double>(grid) * tu;
 				info_.sample_width_ = end - org;
				for(uint32_t i = 0; i < 4; ++i) {
					if(!get_ch(i).ena_->get_check()) continue;
					auto a = waves_.get(i, sample_param_.rate, org);
					a *= get_volt_scale_limit_(i);
					if(get_ch(i).gnd_->get_check()) {
						a = 0.0f;
					}
					info_.sig_[i] = a;

					static const char* t[4] = { "A", "V", "V", "KV" };
					double len = info_.sample_width_;
					static const char* ut[4] = { "S", "mS", "uS", "nS" };
					uint32_t uti = 0;
					if(len < 1e-6) { len *= 1e9; uti = 3; }
					else if(len < 1e-3) { len *= 1e6; uti = 2; }
					else if(len < 1.0) { len *= 1e3; uti = 1; }
					std::string s = (boost::format("CH%d: %4.3f %s, %4.3f %s\n")
						% (i + 1) % a % t[i] % len % ut[uti]).str();
					terminal_core_->output(s);

					{  // 波形解析
						double step = tu / static_cast<double>(grid);
						auto ap = waves_.analize(i, sample_param_.rate, org, end - org, step);
						volt_scale_conv_(i, ap, info_);
						s = (boost::format("Min: %4.3f %s, ") % info_.min_[i] % t[i]).str();
						terminal_core_->output(s);
						s = (boost::format("Max: %4.3f %s\n") % info_.max_[i] % t[i]).str();
						terminal_core_->output(s);
						s = (boost::format("Ave: %4.3f %s\n") % info_.average_[i] % t[i]).str();
						terminal_core_->output(s);
						++info_.id_;
					}
				}
			}

			// フォーカスが外れたら、情報(IN) を強制終了
			if(!core_->get_focus()) {
				info_in_ = false;
			}
		}


		void render_view_(const vtx::irect& clip)
		{
			glDisable(GL_TEXTURE_2D);

			if(info_in_ && core_->get_select()) {
				vtx::sposs r;
				r.emplace_back(info_org_.x, info_org_.y);
				auto msp = core_->get_param().in_point_;
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
			waves_.render(clip.size, sample_param_.rate, get_time_unit_(n));

			glEnable(GL_TEXTURE_2D);
			size_ = clip.size;
		}


		void service_view_()
		{
		}


		void meas_run_()
		{
			double len = get_time_unit_(time_.scale_->get_select_pos());
			len *= waves_.get_info().grid_step_;
			WAVES::measure_param param;
			double tm = 0.0;
			if(mesa_type_->get_select_pos() == 0) {  // single
				param.org_ch_ = org_trg_->get_select_pos() / 2;
				float base = 100.0f;
				if(org_trg_->get_select_pos() & 1) base = -100.0f;
				param.org_slope_ = static_cast<float>(org_slope_->get_select_pos()) / base;

				tm = waves_.measure_org(sample_param_.rate, 0.0, len, param);
				uint32_t idx = time_.scale_->get_select_pos();
				auto grid = waves_.get_info().grid_step_;
				auto ofs = time_.offset_->get_select_pos() * -grid;
				waves_.at_info().meas_pos_[0] = ofs + (tm / get_time_unit_(idx)) * grid;

			} else if(mesa_type_->get_select_pos() == 1) {  // multi
				param.org_ch_ = org_trg_->get_select_pos() / 2;
				float base = 100.0f;
				if(org_trg_->get_select_pos() & 1) base = -100.0f;
				param.org_slope_ = static_cast<float>(org_slope_->get_select_pos()) / base;

				param.fin_ch_ = fin_trg_->get_select_pos() / 2;
				base = 100.0f;
				if(fin_trg_->get_select_pos() & 1) base = -100.0f;
				param.fin_slope_ = static_cast<float>(fin_slope_->get_select_pos()) / base;

				auto a = waves_.measure_org(sample_param_.rate, 0.0, len, param);
				uint32_t idx = time_.scale_->get_select_pos();
				auto grid = waves_.get_info().grid_step_;
				auto ofs = time_.offset_->get_select_pos() * -grid;
				waves_.at_info().meas_pos_[0] = ofs + (a / get_time_unit_(idx)) * grid;

				auto b = waves_.measure_fin(sample_param_.rate, a, len - a, param);
				waves_.at_info().meas_pos_[1] = ofs + (b / get_time_unit_(idx)) * grid;

				tm = b -a;
			}

			uint32_t idx = time_.scale_->get_select_pos();
			tm /= get_time_unit_base_(idx);
			std::string un = get_time_unit_str_(idx);
			ch_to_time_->set_text((boost::format("%5.4f [%s]") % tm % un).str());

			mese_value_ = tm;
		}

		std::string				proj_root_;
		utils::select_file		data_load_filer_;
		utils::select_file		data_save_filer_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		wave_cap(utils::director<core>& d, CLIENT& client, interlock& ilock) :
			director_(d), client_(client), interlock_(ilock), waves_(),
			frame_(nullptr), core_(nullptr),
			terminal_frame_(nullptr), terminal_core_(nullptr),
			tools_(nullptr), smooth_(nullptr), load_(nullptr), save_(nullptr),
			mesa_type_(nullptr), mesa_filt_(nullptr),
			org_trg_(nullptr), org_slope_(nullptr), org_ena_(nullptr),
			fin_trg_(nullptr), fin_slope_(nullptr), fin_ena_(nullptr),
			ch_to_time_(nullptr), wdm_exec_(nullptr),
			meas_id_before_(0), meas_id_(0), org_value_(0.0f), fin_value_(0.0f),
			share_frame_(nullptr),
			sample_param_(),
			wdm_id_{ 0 }, treg_id_{ 0 },
			info_in_(false), info_org_(0), info_(),
			chn0_(waves_, 1.25f),
			chn1_(waves_, 1.25f),
			chn2_(waves_, 1.25f),
			chn3_(waves_, 1.25f),
			measure_time_(waves_),
			time_(waves_), time_id_(0), size_(0),
			mese_value_(0.0),
			proj_root_(), data_load_filer_(), data_save_filer_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  プロジェクト・ルートの設定
			@param[in]	root	プロジェクト・ルート
		*/
		//-----------------------------------------------------------------//
		void set_project_root(const std::string& root)
		{
			proj_root_ = root;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  計測結果の取得
			@return 計測結果
		*/
		//-----------------------------------------------------------------//
		double get_mesa_value() const {
			return mese_value_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  時間スケール値を取得
			@return 時間スケール値
		*/
		//-----------------------------------------------------------------//
		double get_time_scale() const {
			uint32_t newpos = time_.scale_->get_select_pos();
			return get_time_unit_(newpos) / get_time_unit_base_(newpos);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  電圧スケール値を取得
			@param[in]	ch	チャネル
			@return 電圧スケール値
		*/
		//-----------------------------------------------------------------//
		float get_volt_scale(uint32_t ch) const
		{
			uint32_t newpos = 0;
			switch(ch) {
			case 0:
				newpos = chn0_.scale_->get_select_pos();
				break;
			case 1:
				newpos = chn1_.scale_->get_select_pos();
				break;
			case 2:
				newpos = chn2_.scale_->get_select_pos();
				break;
			case 3:
				newpos = chn3_.scale_->get_select_pos();
				break;
			default:
				break;
			}
			return get_volt_scale_value_(ch, newpos);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形情報の取得
			@return 波形情報
		*/
		//-----------------------------------------------------------------//
		const info_t& get_info() const { return info_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  転送ボタンを取得
			@return 転送ボタン
		*/
		//-----------------------------------------------------------------//
		gui::widget_button* get_exec_button() const { return wdm_exec_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  測定単位文字列の取得
			@return 測定単位文字列
		*/
		//-----------------------------------------------------------------//
		std::string get_unit_str() const {
			std::string u;
			auto n = mesa_type_->get_select_pos();
			if(n == 0 || n == 1) {  // 時間
				return get_time_unit_str_(time_.scale_->get_select_pos());
			} else {  // 電圧

			}
			return u;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  許可、不許可
			@param[in]	ena	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool ena = true)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			if(frame_->get_enable() != ena) {
				wd.enable(frame_, ena, true);
				wd.enable(tools_, ena, true);
				wd.enable(terminal_frame_, ena, true);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  サンプリング・パラメーターの設定
			@param[in]	rate	サンプリング・パラメーター	
		*/
		//-----------------------------------------------------------------//
		void set_sample_param(const sample_param& sp) { sample_param_ = sp; }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			size_.set(930, 820);
			{  // 波形描画フレーム
				widget::param wp(vtx::irect(610, 5, size_.x, size_.y));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				wp_.color_param_.fore_color_ = img::rgba8(65, 100, 150);
				wp_.color_param_.back_color_ = wp_.color_param_.fore_color_ * 0.7f;
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
					widget::param wp(vtx::irect(270, 610, 330, 220));
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
				widget::param wp(vtx::irect(270, 5, mw, mh));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}
			{	// スムース
				widget::param wp(vtx::irect(10, 20, 110, 40), tools_);
				widget_check::param wp_("Smooth");
				smooth_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{	// ロード
				widget::param wp(vtx::irect(140, 20, 80, 40), tools_);
				widget_button::param wp_("Load");
				load_ = wd.add_widget<widget_button>(wp, wp_);
				load_->at_local_param().select_func_ = [=](uint32_t id) {
					std::string filter = "波形データ(*.";
					filter += WAVE_DATA_EXT_;
					filter += ")\t*.";
					filter += WAVE_DATA_EXT_;
					filter += "\t";
					data_load_filer_.open(filter, false, proj_root_);
				};
			}
			{	// セーブ
				widget::param wp(vtx::irect(230, 20, 80, 40), tools_);
				widget_button::param wp_("Save");
				save_ = wd.add_widget<widget_button>(wp, wp_);
				save_->at_local_param().select_func_ = [=](uint32_t id) {
					std::string filter = "波形データ(*.";
					filter += WAVE_DATA_EXT_;
					filter += ")\t*.";
					filter += WAVE_DATA_EXT_;
					filter += "\t";
					data_save_filer_.open(filter, true, proj_root_);
				};
			}


			{	// 計測開始チャネルとスロープ
				widget::param wp(vtx::irect(10, 120, 110, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH1 ↑");
				wp_.init_list_.push_back("CH1 ↓");
				wp_.init_list_.push_back("CH2 ↑");
				wp_.init_list_.push_back("CH2 ↓");
				wp_.init_list_.push_back("CH3 ↑");
				wp_.init_list_.push_back("CH3 ↓");
				wp_.init_list_.push_back("CH4 ↑");
				wp_.init_list_.push_back("CH4 ↓");
				org_trg_ = wd.add_widget<widget_list>(wp, wp_);
				org_trg_->at_local_param().select_func_ = [=](const std::string& t, uint32_t p) {
					++meas_id_;
				};
			}
			{	// 計測開始トリガー・レベル
				widget::param wp(vtx::irect(130, 120, 100, 40), tools_);
				widget_spinbox::param wp_(0, 50, 100);
				org_slope_ = wd.add_widget<widget_spinbox>(wp, wp_);
				org_slope_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					++meas_id_;
					return (boost::format("%d") % newpos).str();
				};
			}

			{	// 計測終了チャネルとスロープ
				widget::param wp(vtx::irect(10, 170, 110, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("CH1 ↑");
				wp_.init_list_.push_back("CH1 ↓");
				wp_.init_list_.push_back("CH2 ↑");
				wp_.init_list_.push_back("CH2 ↓");
				wp_.init_list_.push_back("CH3 ↑");
				wp_.init_list_.push_back("CH3 ↓");
				wp_.init_list_.push_back("CH4 ↑");
				wp_.init_list_.push_back("CH4 ↓");
				fin_trg_ = wd.add_widget<widget_list>(wp, wp_);
				fin_trg_->at_local_param().select_func_ = [=](const std::string& t, uint32_t p) {
					++meas_id_;
				};
			}
			{	// 計測終了トリガー・レベル
				widget::param wp(vtx::irect(130, 170, 100, 40), tools_);
				widget_spinbox::param wp_(0, 50, 100);
				fin_slope_ = wd.add_widget<widget_spinbox>(wp, wp_);
				fin_slope_->at_local_param().select_func_ =
					[=](widget_spinbox::state st, int before, int newpos) {
					++meas_id_;
					return (boost::format("%d") % newpos).str();
				};
			}

			{	// 計測時間表示
				widget::param wp(vtx::irect(10, 220, 260, 40), tools_);
				widget_text::param wp_;
				ch_to_time_ = wd.add_widget<widget_text>(wp, wp_);
			}

			{	// 共有フレーム（プロパティシート）
				widget::param wp(vtx::irect(5, 280, mw - 10, mh - 280 - 5), tools_);
				widget_sheet::param wp_;
				share_frame_ = wd.add_widget<widget_sheet>(wp, wp_);
			}

			measure_time_.init(director_, share_frame_, "Time Measure");

			chn0_.init(director_, share_frame_, 0);
			chn1_.init(director_, share_frame_, 1);
			chn2_.init(director_, share_frame_, 2);
			chn3_.init(director_, share_frame_, 3);

			time_.init(director_, share_frame_);

			{  // 計測タイプ
				widget::param wp(vtx::irect(10, 20 + 50, 120, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("SINGLE");
				wp_.init_list_.push_back("MULTI");
				mesa_type_ = wd.add_widget<widget_list>(wp, wp_);
				mesa_type_->at_local_param().select_func_ = [=](const std::string& t, uint32_t p) {
				};
			}
			{  // 計測フィルタ係数
				widget::param wp(vtx::irect(10 + 130, 20 + 50, 130, 40), tools_);
				widget_label::param wp_("0.7", false);
				mesa_filt_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // 計測開始ボタン
				widget::param wp(vtx::irect(270, 220, 30, 30), tools_);
				widget_button::param wp_(">");
				wdm_exec_ = wd.add_widget<widget_button>(wp, wp_);
			}
			{	// 計測ライン描画 (org)
				widget::param wp(vtx::irect(240, 120, 100, 40), tools_);
				widget_check::param wp_("1st");
				org_ena_ = wd.add_widget<widget_check>(wp, wp_);
				org_ena_->at_local_param().select_func_ = [=](bool ena) {
					waves_.at_info().meas_enable_[0] = ena;
				};
			}
			{	// 計測ライン描画 (fin)
				widget::param wp(vtx::irect(240, 170, 100, 40), tools_);
				widget_check::param wp_("2nd");
				fin_ena_ = wd.add_widget<widget_check>(wp, wp_);
				fin_ena_->at_local_param().select_func_ = [=](bool ena) {
					waves_.at_info().meas_enable_[1] = ena;
				};
			}
			{  // help message (widget_chip)
				widget::param wp(vtx::irect(0, 0, 100, 40), tools_);
				widget_chip::param wp_;
				chip_ = wd.add_widget<widget_chip>(wp, wp_);
				chip_->active(0);
			}

			waves_.create_buffer();

			waves_.at_param(0).color_ = img::rgba8(255,  64, 255, 255);
			waves_.at_info().volt_color_[0] = waves_.get_param(0).color_;
			waves_.at_param(1).color_ = img::rgba8( 64, 255, 255, 255);
			waves_.at_info().volt_color_[1] = waves_.get_param(1).color_;
			waves_.at_param(2).color_ = img::rgba8(255, 255,  32, 255);
			waves_.at_info().volt_color_[2] = waves_.get_param(2).color_;
			waves_.at_param(3).color_ = img::rgba8( 64, 255,  64, 255);
			waves_.at_info().volt_color_[3] = waves_.get_param(3).color_;

#ifdef TEST_SIN
			waves_.build_sin(0, sample_param_.rate, 15000.0, 1.0f);
			waves_.build_sin(1, sample_param_.rate, 10000.0, 0.75f);
#endif
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

			auto sheetpos = share_frame_->get_select_pos();
			switch(sheetpos) {
			case 1:
			case 2:
			case 3:
			case 4:
				share_frame_->at_local_param().text_param_.fore_color_
					= waves_.get_param(sheetpos - 1).color_;
				break;
			default:
				share_frame_->at_local_param().text_param_.fore_color_
					= img::rgba8( 255, 255, 255, 255);
				break;
			}

			if(mesa_type_->get_select_pos() == 0) {  // トリガーからの時間計測
				fin_trg_->set_stall();
				fin_slope_->set_stall();
				fin_ena_->set_stall();
			} else if(mesa_type_->get_select_pos() == 1) {  // チャネル間時間計測
				fin_trg_->set_stall(false);
				fin_slope_->set_stall(false);
				fin_ena_->set_stall(false);
			}

			wdm_exec_->set_stall(!client_.probe());

			waves_.enable_smooth(smooth_->get_check());

			measure_time_.tbp_ = time_.scale_->get_select_pos();
			measure_time_.update(size_);

			chn0_.update(0, size_, sample_param_.gain[0]);
			chn1_.update(1, size_, sample_param_.gain[1]);
			chn2_.update(2, size_, sample_param_.gain[2]);
			chn3_.update(3, size_, sample_param_.gain[3]);

			time_.update(size_, sample_param_.rate);

			// 波形のコピー（中間位置がトリガー）
			for(uint32_t i = 0; i < 4; ++i) {
				auto id = client_.get_mod_status().wdm_id_[i];
				if(wdm_id_[i] != id) {
					auto sz = waves_.size();
					waves_.copy(i, client_.get_wdm(i), sz, sz / 2);
					wdm_id_[i] = id;
					++meas_id_;
				}
			}

			if(time_id_ != time_.id_) {
				time_id_ = time_.id_;
				++meas_id_;
			}

			// 時間計測 ( single, multi)
			if(meas_id_before_ != meas_id_) {
				meas_run_();
				meas_id_before_ = meas_id_;
			}

			// 仮熱抵抗表示
			for(uint32_t i = 0; i < 2; ++i) {
				auto id = client_.get_mod_status().treg_id_[i];
				if(treg_id_[i] != id) {
std::cout << "Th REG recv" << std::endl;
					auto sz = waves_.size();
					waves_.copy(i + 1, client_.get_treg(i), sz, sz / 2);
					treg_id_[i] = id;
				}
			}

			{
				uint32_t act = 60 * 3;
				if(org_slope_->get_focus()) {
					std::string s;
					tools::set_help(chip_, org_slope_, s);
				} else if(fin_slope_->get_focus()) {
					std::string s;
					tools::set_help(chip_, fin_slope_, s);
				} else {
					act = 0;
				}
				chip_->active(act);
			}

			if(data_load_filer_.state()) {
				auto path = data_load_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += WAVE_DATA_EXT_;
					}
					if(waves_.load(path)) {
					}
				}
			}
			if(data_save_filer_.state()) {
				auto path = data_save_filer_.get();
				if(!path.empty()) {
					if(utils::get_file_ext(path).empty()) {
						path += '.';
						path += WAVE_DATA_EXT_;
					}
					if(waves_.save(path)) {
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->load(pre);
			}
			if(tools_ != nullptr) {
				tools_->load(pre);
			}
			if(smooth_ != nullptr) {
				smooth_->load(pre);
			}
			if(share_frame_ != nullptr) {
				share_frame_->load(pre);
			}

			chn0_.load(pre);
			chn1_.load(pre);
			chn2_.load(pre);
			chn3_.load(pre);

			time_.load(pre);

			measure_time_.load(pre);

			mesa_type_->load(pre);
			mesa_filt_->load(pre);
			org_trg_->load(pre);
			org_slope_->load(pre);
			org_ena_->load(pre);
			fin_trg_->load(pre);
			fin_slope_->load(pre);
			fin_ena_->load(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
			if(frame_ != nullptr) {
				frame_->save(pre);
			}
			if(terminal_frame_ != nullptr) {
				terminal_frame_->save(pre);
			}
			if(tools_ != nullptr) {
				tools_->save(pre);
			}
			if(smooth_ != nullptr) {
				smooth_->save(pre);
			}
			if(share_frame_ != nullptr) {
				share_frame_->save(pre);
			}

			chn0_.save(pre);
			chn1_.save(pre);
			chn2_.save(pre);
			chn3_.save(pre);

			time_.save(pre);

			measure_time_.save(pre);

			mesa_type_->save(pre);
			mesa_filt_->save(pre);
			org_trg_->save(pre);
			org_slope_->save(pre);
			org_ena_->save(pre);
			fin_trg_->save(pre);
			fin_slope_->save(pre);
			fin_ena_->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形画像のセーブ
			@param[in]	path	セーブ・パス
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save_image(const std::string& path) const
		{
			bool ret = false;
			if(path.empty()) return ret;
			if(core_ == nullptr) return ret;

			vtx::ipos pos;
			gui::final_position(core_, pos);
			const auto& size = core_->get_param().rect_.size;
			auto simg = gl::get_frame_buffer(pos.x , pos.y, size.x, size.y);
			img::img_files imfs;
			imfs.set_image(simg);
			ret = imfs.save(path);			

			return ret;
		}
	};
}
