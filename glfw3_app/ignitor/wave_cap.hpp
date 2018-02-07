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
#include "core/glcore.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_utils.hpp"

#include "gl_fw/render_waves.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  wave_cap クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wave_cap {

		utils::director<core>&	director_;

		typedef view::render_waves<uint16_t, 4096, 4> WAVES;
		WAVES					waves_;

		gui::widget_frame*		frame_;
		gui::widget_view*		core_;

		gui::widget_frame*		tools_;

		gui::widget_check*		time_;
		gui::widget_spinbox*   	time_org_;
		gui::widget_spinbox*   	time_len_;
		gui::widget_label*		time_in_;

		gui::widget_check*		volt_;
		gui::widget_spinbox*   	volt_org_;
		gui::widget_spinbox*	volt_len_;
		gui::widget_label*		volt_in_;


#if 0
		gui::widget_list*		oscillo_secdiv_;		///< オシロスコープ設定、時間（周期）
		gui::widget_list*		oscillo_trg_ch_;		///< オシロスコープ設定、トリガー・チャネル選択
		gui::widget_list*		oscillo_trg_slope_;		///< オシロスコープ設定、トリガー・スロープ選択
		gui::widget_spinbox*	oscillo_trg_window_;	///< オシロスコープ設定、トリガー・ウィンドウ
		gui::widget_label*		oscillo_trg_level_;		///< オシロスコープ設定、トリガー・レベル
		oscillo_t				oscillo_[4];			///< オシロスコープ各チャネル設定
#endif



		vtx::ipos				size_;

#if 0
				wp_.init_list_.push_back("1000 ms");
				wp_.init_list_.push_back("500 ms");
				wp_.init_list_.push_back("250 ms");
				wp_.init_list_.push_back("100 ms");
				wp_.init_list_.push_back("50 ms");
				wp_.init_list_.push_back("10 ms");
				wp_.init_list_.push_back("5 ms");
				wp_.init_list_.push_back("1 ms");
				wp_.init_list_.push_back("500 us");
				wp_.init_list_.push_back("100 us");
				wp_.init_list_.push_back("50 us");
				wp_.init_list_.push_back("25 us");
				wp_.init_list_.push_back("10 us");
				wp_.init_list_.push_back("5 us");
				wp_.init_list_.push_back("1 us");

			{ // DIV select
				widget::param wp(vtx::irect(10, 20 + 40 * 1, menu_width - 20, 30), menu_);
				widget_list::param wp_("1000 ms");
				wp_.init_list_.push_back("1000 ms");
				wp_.init_list_.push_back("500 ms");
				wp_.init_list_.push_back("250 ms");
				wp_.init_list_.push_back("100 ms");
				wp_.init_list_.push_back("50 ms");
				wp_.init_list_.push_back("10 ms");
				wp_.init_list_.push_back("5 ms");
				wp_.init_list_.push_back("1 ms");
				wp_.init_list_.push_back("500 us");
				wp_.init_list_.push_back("100 us");
				wp_.init_list_.push_back("50 us");
				wp_.init_list_.push_back("25 us");
				wp_.init_list_.push_back("10 us");
				wp_.init_list_.push_back("5 us");
				wp_.init_list_.push_back("1 us");
//				wp_.select_func_ = [this](const std::string& text, uint32_t pos) {
//					utils::format("List Selected: '%s', (%d)\n") % text.c_str() % pos;
//				};
				div_ = wd.add_widget<widget_list>(wp, wp_);
			}
#endif
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

		// 波形描画
		void update_view_()
		{
		}


		void render_view_(const vtx::irect& clip)
		{
//			gui::widget_director& wd = director_.at().widget_director_;

			glDisable(GL_TEXTURE_2D);

//			auto pos = div_->get_menu()->get_select_pos();
			waves_.render(clip.size, 65536);

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
		wave_cap(utils::director<core>& d) : director_(d),
			waves_(), frame_(nullptr), core_(nullptr),
			tools_(nullptr),
			time_(nullptr), time_org_(nullptr), time_len_(nullptr), time_in_(nullptr),
			volt_(nullptr), volt_org_(nullptr), volt_len_(nullptr), volt_in_(nullptr)

//			oscillo_secdiv_(nullptr) ,oscillo_trg_ch_(nullptr), oscillo_trg_slope_(nullptr),
//			oscillo_trg_window_(nullptr), oscillo_trg_level_(nullptr),

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

			{	// 波形フレーム
				widget::param wp(vtx::irect(40, 150, 400, 400));
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

			int menu_width = 200;
			{	// 波形ツールフレーム
				widget::param wp(vtx::irect(100, 100, menu_width, 500));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}
			{  // 時間メジャー有効、無効
				widget::param wp(vtx::irect(10, 22 + 50 * 0, 130, 40), tools_);
				widget_check::param wp_("Time:");
				time_ = wd.add_widget<widget_check>(wp, wp_);
				time_->at_local_param().select_func_ = [=](bool f) {
					waves_.at_info().time_enable_ = f;
				};
			}
			{
				widget::param wp(vtx::irect(10, 22 + 50 * 1, 130, 40), tools_);
				widget_spinbox::param wp_(0, 0, 100);
				time_org_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_org_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{
				widget::param wp(vtx::irect(10, 22 + 50 * 2, 130, 40), tools_);
				widget_spinbox::param wp_(0, 0, 100);
				time_len_ = wd.add_widget<widget_spinbox>(wp, wp_);
				time_len_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}


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





			sys::preference& pre = director_.at().preference_;
			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(tools_ != nullptr) {
				tools_->load(pre);
			}

			// テスト波形生成
///			waves_.create_buffer(0.5, 10e-6);

//			waves_.at_param(0).gain_ = 0.025f;
//			waves_.at_param(1).gain_ = 0.025f;
//			waves_.at_param(0).color_ = img::rgba8(255, 128, 255, 255);
//			waves_.at_param(1).color_ = img::rgba8(128, 255, 255, 255);
//			waves_.at_param(0).offset_ = 0;
//			waves_.at_param(1).offset_ = 200;

			waves_.at_info().volt_org_ = 80;
			waves_.at_info().volt_len_ = 130;
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
			if(time_org_ == nullptr) return;
			if(time_len_ == nullptr) return;
			time_org_->at_local_param().max_pos_ = size_.x;
			time_len_->at_local_param().max_pos_ = size_.x;
			waves_.at_info().time_org_ = time_org_->get_select_pos();
			waves_.at_info().time_len_ = time_len_->get_select_pos();
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
			if(time_ != nullptr) {
				time_->load(pre);
				waves_.at_info().time_enable_ = time_->get_check();
			}
			if(volt_ != nullptr) {
				volt_->load(pre);
				waves_.at_info().volt_enable_ = volt_->get_check();
			}
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
			if(time_ != nullptr) {
				time_->save(pre);
			}
			if(volt_ != nullptr) {
				volt_->save(pre);
			}
		}
	};
}


