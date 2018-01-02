#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・ルート・メニュー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_button.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ルート・メニュー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class root_menu {

		gui::widget_director& wd_;

		gui::widget_button*		new_project_;
		gui::widget_button*		sel_project_;
		gui::widget_button*		edit_project_;
		gui::widget_button*		settings_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		root_menu(gui::widget_director& wd) : wd_(wd),
			new_project_(nullptr),
			sel_project_(nullptr),
			edit_project_(nullptr),
			settings_(nullptr)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			int ofsx = 50;
			int ofsy = 50;
			int btw  = 200;
			int bth  = 80;
			int sph = bth + 50;
			{
				widget::param wp(vtx::irect(ofsx, ofsy + sph * 0, btw, bth));
				widget_button::param wp_("新規プロジェクト");
				new_project_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx, ofsy + sph * 1, btw, bth));
				widget_button::param wp_("プロジェクト選択");
				sel_project_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx, ofsy + sph * 2, btw, bth));
				widget_button::param wp_("プロジェクト編集");
				edit_project_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx, ofsy + sph * 3, btw, bth));
				widget_button::param wp_("設定");
				settings_ = wd_.add_widget<widget_button>(wp, wp_);
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


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
		}
	};
}

