#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・リレー切り替えクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <array>
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "img_io/img_files.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  リレー・マップ・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class relay_map {

		utils::director<core>&	director_;

		gui::widget_dialog*		dialog_;

		gui::widget_image*		image_;

		gui::widget_check*		l_sw_[18];
		gui::widget_check*		r_sw_[18];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		relay_map(utils::director<core>& d) : director_(d),
			dialog_(nullptr), image_(nullptr), l_sw_{ nullptr }, r_sw_{ nullptr }
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
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int d_w = 800;
			int d_h = 600;
			img::img_files imgf;
			auto state = imgf.load("relay_map.png");
			if(state) {
				auto s = imgf.get_image().get()->get_size();
				d_w = s.x + 10;
				d_h = s.y + 60 + 10;
			}
			{
				widget::param wp(vtx::irect(100, 100, d_w, d_h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
//				dialog_->at_local_param().select_func_ = [this](bool ok) {
//				};
			}

			{  // リレー・マップ・イメージ
				widget::param wp(vtx::irect(5, 5, 0, 0), dialog_);
				widget_image::param wp_(imgf.get_image().get());
				image_ = wd.add_widget<widget_image>(wp, wp_);
			}

			static const vtx::ipos l_tbls[] = {
				{ 253, 52 + 20 * 0 },	// S1
				{ 253, 52 + 20 * 1 },	// S2
				{ 253, 52 + 20 * 2 },	// S3
				{ 253, 52 + 20 * 3 },	// S4
				{ 253, 52 + 20 * 4 },	// S5
				{ 253, 52 + 20 * 5 },	// S6
				{ 253, 52 + 20 * 6 },	// S7
				{ 253, 52 + 20 * 7 },	// S8
				{ 253, 52 + 20 * 8 },	// S9
				{ 253, 52 + 20 * 9 },	// S10
				{ 253, 52 + 20 * 10 },	// S11
				{ 253, 52 + 20 * 11 },	// S12
				{ 253, 52 + 20 * 12 },	// S13
				{ 253, 52 + 20 * 13 },	// S14
				{ 253, 52 + 20 * 14 },	// S15
				{ 253, 52 + 20 * 15 },	// S16
				{ 253, 52 + 20 * 16 },	// S17
				{ 253, 52 + 20 * 17 },	// S18
			};
			for(int i = 0; i < 18; ++i) {  // リレー切り替え、左側
				widget::param wp(vtx::irect(l_tbls[i].x + 4, l_tbls[i].y - 10, 30, 30), dialog_);
				widget_check::param wp_;
				l_sw_[i] = wd.add_widget<widget_check>(wp, wp_);
//				l_sw_[i]_->at_local_param().select_func_
//					= [=](const std::string& str, uint32_t pos) {
//				};
			}
			static const vtx::ipos r_tbls[] = {
				{ 614, 52 + 20 * 0 },	// S34

				{ 614, 52 + 20 * 2 },	// S19
				{ 614, 52 + 20 * 3 },	// S20

				{ 614, 52 + 20 * 5 },	// S21
				{ 614, 52 + 20 * 6 },	// S35
				{ 614, 52 + 20 * 7 },	// S22
				{ 614, 52 + 20 * 8 },	// S23
				{ 614, 52 + 20 * 9 },	// S24
				{ 614, 52 + 20 * 10 },	// S25
				{ 614, 52 + 20 * 11 },	// S26
				{ 614, 52 + 20 * 12 },	// S27
				{ 614, 52 + 20 * 13 },	// S28

				{ 614, 52 + 20 * 15 },	// S29
				{ 614, 52 + 20 * 16 },	// S35
				{ 614, 52 + 20 * 17 },	// S30
				{ 614, 52 + 20 * 18 },	// S31
				{ 614, 52 + 20 * 19 },	// S32
				{ 614, 52 + 20 * 20 },	// S33
			};
			for(int i = 0; i < 18; ++i) {  // リレー切り替え、右側
				widget::param wp(vtx::irect(r_tbls[i].x + 4, r_tbls[i].y - 10, 30, 30), dialog_);
				widget_check::param wp_;
				r_sw_[i] = wd.add_widget<widget_check>(wp, wp_);
//				r_sw_[i]_->at_local_param().select_func_
//					= [=](const std::string& str, uint32_t pos) {
//				};
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(!dialog_->get_state(gui::widget::state::ENABLE)) return;

			if(image_->get_select_in()) {
				auto pos = image_->get_param().in_point_;
				if(52 <= pos.y && pos.y <= 397) {
					if(253 <= pos.x && pos.x <= 275) {  // S1 to S18

					} else if(614 <= pos.x && pos.x <= 636) {  // S19 to S34

					}
				}
//				std::cout << pos.x << ", " << pos.y << std::endl;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
			for(int i = 0; i < 18; ++i) {
				l_sw_[i]->save(pre);
			}
			for(int i = 0; i < 18; ++i) {
				r_sw_[i]->save(pre);
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre)
		{
			for(int i = 0; i < 18; ++i) {
				l_sw_[i]->load(pre);
			}
			for(int i = 0; i < 18; ++i) {
				r_sw_[i]->load(pre);
			}
			return true;
		}
	};
}
