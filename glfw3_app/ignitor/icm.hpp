#pragma once
//=====================================================================//
/*! @file
    @brief  ICM クラス
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
		@brief  検査パラメーター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class icm
	{
		utils::director<core>&	director_;
		net::ign_client_tcp&	client_;
		interlock&				interlock_;

	public:
		// ICM 設定
		gui::widget_check*		sw_[6];		///< ICM 接続スイッチ
		gui::widget_button*		exec_;		///< ICM 設定転送
		gui::widget_check*		all_;		///< ICM 全体

	private:
		struct icm_t {
			uint16_t	sw;		///< 6 bits

			icm_t() : sw(0) { }

			std::string build() const
			{
				std::string s;
				s = (boost::format("icm ICSW%02X\n") % sw).str();
				return s;
			}
		};

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		icm(utils::director<core>& d, net::ign_client_tcp& client, interlock& ilc) :
			director_(d), client_(client), interlock_(ilc),
			sw_{ nullptr }, exec_(nullptr), all_(nullptr)
		{ }			


		//-----------------------------------------------------------------//
		/*!
			@brief  リセット・スイッチ
		*/
		//-----------------------------------------------------------------//
		void reset_sw()
		{
			for(uint32_t i = 0; i < 6; ++i) {
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
			icm_t t;
			client_.send_data(t.build());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
			@param[in]	root	ルート
			@param[in]	d_w		横幅最大
			@param[in]	ofsx	オフセット X
			@param[in]	ofsy	オフセット Y
		*/
		//-----------------------------------------------------------------//
		void init(gui::widget* root, int d_w, int ofsx, int ofsy)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{
				widget::param wp(vtx::irect(20, ofsy, 60, 40), root);
				widget_text::param wp_("ICM:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			tools::init_sw(wd, root, interlock_, ofsx, ofsy, sw_, 6, 34);

			{  // exec
				widget::param wp(vtx::irect(d_w - 85, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					icm_t t;
					uint16_t sw = 0;
					for(uint32_t i = 0; i < 6; ++i) {
						sw <<= 1;
						if(sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;

					client_.send_data(t.build());
				};
			}
			{
				widget::param wp(vtx::irect(d_w - 45, ofsy, 30, 30), root);
				widget_check::param wp_;
				all_ = wd.add_widget<widget_check>(wp, wp_);
				all_->at_local_param().select_func_ = [=](bool ena) {
					if(!ena) {
						tools::set_checks(sw_, false, 6);
						startup();						
					}
					for(uint32_t i = 0; i < 6; ++i) {
						sw_[i]->set_stall(!ena, widget::STALL_GROUP::_1);
					}
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
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
			tools::save_sw(pre, sw_, 6);
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
			tools::load_sw(pre, sw_, 6);
			all_->load(pre);
			all_->exec();
		}
	};
}
