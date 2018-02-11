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

// #include "ign_client.hpp"
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

		net::ign_client_tcp&	client_;

		typedef view::render_waves<uint16_t, 2048, 4> WAVES;
		WAVES					waves_;

		gui::widget_frame*		frame_;
		gui::widget_view*		core_;

		gui::widget_frame*		tools_;

		gui::widget_list*		time_div_;
		gui::widget_list*		trg_ch_;
		gui::widget_list*		trg_slope_;
		gui::widget_spinbox*	trg_window_;
		gui::widget_label*		trg_level_;
		gui::widget_list*		ch_gain_[4];
		gui::widget_button*		exec_;

		gui::widget_spinbox*	time_scale_;
		gui::widget_spinbox*	time_offset_;


		struct chn_t {
			gui::widget_null*		root_;
			float					unit_v_;  ///< １単位に対する電圧
			gui::widget_check*		ena_;
			gui::widget_spinbox*	pos_;
			gui::widget_spinbox*	scale_;

			chn_t() : root_(nullptr), unit_v_(1.25f / 65535.0f),
				ena_(nullptr), pos_(nullptr), scale_(nullptr)
			{ }


			void init(DR& dr, WAVES& wvs, gui::widget* root, int ch, int x, int y)
			{
				using namespace gui;
				auto& wd = dr.at().widget_director_;

				{  // 有効、無効
					widget::param wp(vtx::irect(x, y, 60, 40), root);
					widget_check::param wp_((boost::format("CH%d") % ch).str());
					ena_ = wd.add_widget<widget_check>(wp, wp_);
					ena_->at_local_param().select_func_ = [=](bool f) {
					};
				}
				y += 50;
				{  // 位置
					widget::param wp(vtx::irect(x, y, 110, 40), root);
					widget_spinbox::param wp_(0, 0, 20); // grid 数の倍
					pos_ = wd.add_widget<widget_spinbox>(wp, wp_);
					pos_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						int wy = root->get_param().rect_.size.y;
						float a = static_cast<float>(newpos)
							/ static_cast<float>(wy / (wvs.get_info().grid_step_ / 2));
						return (boost::format("%2.1f") % a).str();
					};
				}
				y += 50;
				{  // 電圧スケール
					widget::param wp(vtx::irect(x, y, 110, 40), root);
					widget_spinbox::param wp_(0, 0, (volt_scale_size_ - 1));
					scale_ = wd.add_widget<widget_spinbox>(wp, wp_);
					scale_->at_local_param().select_func_
						= [=](widget_spinbox::state st, int before, int newpos) {
						// グリッドに対する電圧表示
//						auto sc = get_volt_scale_(newpos);
//						auto a = static_cast<float>(sc) / 65536.0f;
//						float a = waves_.get_info().grid_step_ * t.unit_v_;
						return (boost::format("%d") % newpos).str();
					};
				}
			}


			void update(const vtx::ipos& size)
			{
				if(pos_ != nullptr) {
					pos_->at_local_param().max_pos_ = size.y;
				}
			}
		};

		chn_t			chn_[4];

		struct mesure_t {
			gui::widget_check*		ena_;
			gui::widget_spinbox*   	org_;
			gui::widget_spinbox*   	len_;
			gui::widget_label*		in_;
			bool					type_;
			mesure_t(bool ty) : ena_(nullptr), org_(nullptr), len_(nullptr), in_(nullptr),
				type_(ty)
			{ }

			void load(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->load(pre);
				}
			}

			void save(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->save(pre);
				}
			}
		};
		mesure_t				time_;
		mesure_t				volt_;



		vtx::ipos				size_;

		static constexpr double div_tbls_[] = {
			1000e-3,
			500e-3,
			250e-3,
			100e-3,
			50e-3,
			10e-3,
			5e-3,
			1e-3,
			500e-6,
			100e-6,
			50e-6,
			25e-6,
			10e-6,
			5e-6,
			1e-6,
		};

		void init_mesure_(int loc, const std::string& text, mesure_t& t)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int y = loc * 50;
			{  // メジャー有効、無効
				widget::param wp(vtx::irect(10, 22 + 5 + y, 130, 40), tools_);
				widget_check::param wp_(text);
				t.ena_ = wd.add_widget<widget_check>(wp, wp_);
				t.ena_->at_local_param().select_func_ = [=](bool f) {
					if(t.type_) {
						waves_.at_info().time_enable_ = f;
					} else {
						waves_.at_info().volt_enable_ = f;
					}
				};
			}
			y += 50;
			{
				widget::param wp(vtx::irect(10, 22 + 5 + y, 130, 40), tools_);
				widget_spinbox::param wp_(0, 0, 100);
				t.org_ = wd.add_widget<widget_spinbox>(wp, wp_);
				t.org_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			y += 50;
			{
				widget::param wp(vtx::irect(10, 22 + 5 + y, 130, 40), tools_);
				widget_spinbox::param wp_(0, 0, 100);
				t.len_ = wd.add_widget<widget_spinbox>(wp, wp_);
				t.len_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
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


		std::string build_wdm_()
		{
			std::string s;
			uint32_t cmd;

			// SW
			cmd = 0b00001000;
			cmd <<= 16;
			cmd |= (0x80 << 8);  // S29
			s += (boost::format("wdm %06X\n") % cmd).str();
			
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
			{  // trigger channel
				cmd = (0b00100000 << 16);
				auto n = trg_ch_->get_select_pos();
				uint8_t sub = 0;
				if(n > 0) {
					sub |= 0x80;
					cmd |= static_cast<uint32_t>(n % 4) << 14;
				}
				if(trg_slope_->get_select_pos()) sub |= 0x40;
				sub |= trg_window_->get_select_pos() & 15;
				cmd |= sub;
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
			return s;
		}

		// 波形描画
		void update_view_()
		{
		}


		static const uint32_t time_scale_size_ = 11;
		uint32_t get_time_scale_(uint32_t idx)
		{
			static const uint32_t table[time_scale_size_] = {
				65536, 32768+16384, 32768, 16384+8192,
				16384, 8192+4096, 8192, 4096+2048,
				4096, 4096+2048, 2048
			};
			return table[idx % time_scale_size_];
		}


		static const uint32_t volt_scale_size_ = 16;
		uint32_t get_volt_scale_(uint32_t idx)
		{
			static const uint32_t tbl[volt_scale_size_] = {
				32, 32+16, 64, 64+32, 128, 128+64, 256, 256+128,
				512, 512+256, 1024, 1024+512, 2048, 2048+1024, 4096, 4096+2048,
			};
			return tbl[idx % volt_scale_size_];
		}


		void render_view_(const vtx::irect& clip)
		{
//			gui::widget_director& wd = director_.at().widget_director_;
			glDisable(GL_TEXTURE_2D);
			uint32_t idx = 0;
			if(time_scale_ != nullptr) {
				idx = time_scale_->get_select_pos();
			}
			waves_.render(clip.size, get_time_scale_(idx));
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
		wave_cap(utils::director<core>& d, net::ign_client_tcp& client) : director_(d),
			client_(client),
			waves_(), frame_(nullptr), core_(nullptr),
			tools_(nullptr),
			time_div_(nullptr), trg_ch_(nullptr), trg_slope_(nullptr),
			trg_window_(nullptr), trg_level_(nullptr), ch_gain_{ nullptr },
			exec_(nullptr),
			time_scale_(nullptr), time_offset_(nullptr), chn_{ },
			time_(true), volt_(false)
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
			{	// 波形フレーム
				widget::param wp(vtx::irect(40, 150, size_.x, size_.y));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
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

			int mw = 330;
			int mh = 600;
			{	// 波形ツールフレーム
				widget::param wp(vtx::irect(100, 100, mw, mh));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}
			{  // 時間軸リスト 10K、20K、50K、100K、200K、500K、1M、2M、5M、10M、20M、50M、100M
				widget::param wp(vtx::irect(20, 22 + 10, 110, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100us");
				wp_.init_list_.push_back(" 50us");
				wp_.init_list_.push_back(" 20us");
				wp_.init_list_.push_back(" 10us");
				wp_.init_list_.push_back("  5us");
				wp_.init_list_.push_back("  2us");
				wp_.init_list_.push_back("  1us");
				wp_.init_list_.push_back("500ns");
				wp_.init_list_.push_back("200ns");
				wp_.init_list_.push_back("100ns");
				wp_.init_list_.push_back(" 50ns");
				wp_.init_list_.push_back(" 20ns");
				wp_.init_list_.push_back(" 10ns");
				time_div_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・チャネル選択
				widget::param wp(vtx::irect(20 + 120, 22 + 10, 80, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("---");
				wp_.init_list_.push_back("CH0");
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				trg_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・スロープ選択
				widget::param wp(vtx::irect(20, 22 + 60, 80, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Fall");
				wp_.init_list_.push_back("Rise");
				trg_slope_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・ウィンドウ（１～１５）
				widget::param wp(vtx::irect(20 + 90, 22 + 60, 90, 40), tools_);
				widget_spinbox::param wp_(1, 1, 15);
				trg_window_ = wd.add_widget<widget_spinbox>(wp, wp_);
				trg_window_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(20, 22 + 110, 80, 40), tools_);
				widget_label::param wp_("32768", false);
				trg_level_ = wd.add_widget<widget_label>(wp, wp_);
				trg_level_->at_local_param().select_func_ = [=](const std::string& str) {
					trg_level_->set_text(limiti_(str, 1, 65534, "%d"));
				};
			}
			{  // exec
				widget::param wp(vtx::irect(mw - 40, 22 + 110, 30, 30), tools_);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					auto s = build_wdm_();
					client_.send_data(s);
				};
			}

			for(int i = 0; i < 4; ++i) {
					static const vtx::ipos ofs[4] = {
						{ 0, 0 }, { 110, 0 }, { 0, 50 }, { 110, 50 }
					};
					widget::param wp(vtx::irect(20 + ofs[i].x, 22 + 160 + ofs[i].y,
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

			init_mesure_(5, "Time", time_);
			init_mesure_(8, "Volt", volt_);

			{  // タイム・スケール
				widget::param wp(vtx::irect(180, 280, 110, 40), tools_);
				widget_spinbox::param wp_(0, 0, time_scale_size_ - 1);
				time_scale_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_scale_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					float r = 65536.0f / static_cast<float>(get_time_scale_(newpos));
					return (boost::format("%2.1f") % r).str();
				};
			}
			{  // タイム・オフセット（グリッド単位）
				widget::param wp(vtx::irect(180, 330, 110, 40), tools_);
				widget_spinbox::param wp_(0, 0, 50);
				time_offset_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_offset_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}


			chn_[0].init(director_, waves_, tools_, 0, 180, 380);


			sys::preference& pre = director_.at().preference_;
			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(tools_ != nullptr) {
				tools_->load(pre);
			}

			waves_.create_buffer();

//			waves_.at_param(0).color_ = img::rgba8(255, 128, 255, 255);
//			waves_.at_param(1).color_ = img::rgba8(128, 255, 255, 255);

///			waves_.build_sin(10e3);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(frame_ == nullptr) return;

			exec_->set_stall(!client_.probe());

			if(time_.org_ != nullptr) {
				time_.org_->at_local_param().max_pos_ = size_.x;
				waves_.at_info().time_org_ = time_.org_->get_select_pos();
			}
			if(time_.len_ != nullptr) {
				time_.len_->at_local_param().max_pos_ = size_.x;
				waves_.at_info().time_len_ = time_.len_->get_select_pos();
			}
			if(volt_.org_ != nullptr) {
				volt_.org_->at_local_param().max_pos_ = size_.x;
				waves_.at_info().volt_org_ = volt_.org_->get_select_pos();
			}
			if(volt_.len_ != nullptr) {
				volt_.len_->at_local_param().max_pos_ = size_.x;
				waves_.at_info().volt_len_ = volt_.len_->get_select_pos();
			}

			{
				chn_[0].update(size_);

				auto grid_step = waves_.get_info().grid_step_;

				auto ts = get_time_scale_(time_scale_->get_select_pos());
				time_offset_->at_local_param().max_pos_ = 
					(waves_.size() * ts / 65536 - size_.x) / grid_step;
	
				auto ofs = time_offset_->get_select_pos();
				waves_.at_param(0).offset_.x = ofs * grid_step;

				// 波形位置
				chn_[0].pos_->at_local_param().max_pos_ = size_.y / (grid_step / 2);
				waves_.at_param(0).offset_.y = chn_[0].pos_->get_select_pos() * (grid_step / 2);
				// 波形拡大、縮小
				auto n = chn_[0].scale_->get_select_pos();
				waves_.at_param(0).gain_ = static_cast<float>(get_volt_scale_(n)) / 65536.0f;
			}

			waves_.copy(0, client_.get_wdm(0), waves_.size());
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
			if(tools_ != nullptr) {
				tools_->load(pre);
			}
			time_.load(pre);
			volt_.load(pre);
#if 0
					waves_.at_info().time_enable_ = time_->get_check();
				}
				if(volt_ != nullptr) {
					volt_->load(pre);
					waves_.at_info().volt_enable_ = volt_->get_check();
#endif
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
			if(tools_ != nullptr) {
				tools_->save(pre);
			}
			time_.save(pre);
			volt_.save(pre);
		}
	};
}
