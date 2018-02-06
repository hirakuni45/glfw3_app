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


