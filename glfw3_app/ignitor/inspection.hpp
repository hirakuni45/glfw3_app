#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター検査クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection {

		utils::director<core>&	director_;

		gui::widget_dialog*		dialog_;
		gui::widget_label*		title_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		inspection(utils::director<core>& d) : director_(d),
			dialog_(nullptr),
			title_(nullptr)
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
			@brief  タイトルの設定
			@param[in]	title	タイトル
		*/
		//-----------------------------------------------------------------//
		void set_title(const std::string& title)
		{
			title_->set_text(title);
		}


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

			int w = 800;
			int h = 600;
			{
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [this](bool ok) {
				};
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 0, 150, 30), dialog_);
				widget_text::param wp_("検査項目名：");
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(190, 20 + 40 * 0, 220, 40), dialog_);
				widget_label::param wp_;
				title_ = wd.add_widget<widget_label>(wp, wp_);
			}

			{
				widget::param wp(vtx::irect(20, 20 + 40 * 1, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("検査規格：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 2, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("検査方法：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 3, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("電源設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 4, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("ジェネレーター設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 5, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("オシロスコープ設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 6, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("測定項目設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 7, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("ＬＣＲ設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 8, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("二次負荷切替設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 9, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("リレー切替設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(20, 20 + 40 * 10, w - 20 * 2, 30), dialog_);
				widget_text::param wp_("Wait時間設定：");
				wd.add_widget<widget_text>(wp, wp_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
		}
	};
}
