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
#include "widgets/widget_frame.hpp"
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
		gui::widget_label*		title_;		///< 検査項目名
		gui::widget_list*		kikaku_;	///< 検査規格
		gui::widget_list*		kensahou_;	///< 検査方法選択

		gui::widget_label*		pow_title_;	///< 電源設定

		gui::widget_list*		fukasel_;	///< ２次負荷切替設定


		void write_(utils::file_io& fio, const std::string& key, const std::string& body) {
			fio.put(key);
			fio.put_char(' ');
			fio.put(body);
			fio.put_char('\n');
		}


		bool decode_(utils::file_io& fio)
		{
			if(title_ == nullptr) return false;

			uint32_t no = 0;
			while(!fio.eof()) {
				++no;
				auto line = fio.get_line();
				if(line.empty()) continue;
				if(line[0] == '#') continue;  // comment line

				auto ss = utils::split_text(line, " ");
				if(ss.size() < 2) {
					return false;
				}
				if(ss[0] == "title") {
					title_->set_text(ss[1]);
				} else {
					return false;
				}
			}

			return true;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		inspection(utils::director<core>& d) : director_(d),
			dialog_(nullptr),
			title_(nullptr), kikaku_(nullptr), kensahou_(nullptr),
			fukasel_(nullptr)
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
			@brief  タイトルの取得
			@return タイトル
		*/
		//-----------------------------------------------------------------//
		std::string get_title() const
		{
			return title_->get_text();
		}


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

			int d_w = 800;
			int d_h = 600;
			{
				widget::param wp(vtx::irect(100, 100, d_w, d_h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [this](bool ok) {
				};
			}
			int w = 220;
			int h = 45;
			for(int i = 0; i < 11; ++i) {
				static const char* tbl[] = {
					"検査項目名：",
					"検査規格：",
					"検査方法：",
					"電源設定：",
					"ジェネレーター設定：",
					"オシロスコープ設定：",
					"測定項目設定：",
					"ＬＣＲ設定：",
					"２次負荷切替設定：",
					"リレー切替設定：",
					"Wait時間設定："
				};
				widget::param wp(vtx::irect(20, 20 + h * i, w, h), dialog_);
				widget_text::param wp_(tbl[i]);
				wp_.text_param_.placement_
					= vtx::placement(vtx::placement::holizontal::LEFT, vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // 検査項目名
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_;
				title_ = wd.add_widget<widget_label>(wp, wp_);
			}

			{  // 検査規格
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 1, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("MAX 値");
				wp_.init_list_.push_back("MIN 値");
				kikaku_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{  // 検査方法
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 2, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("静特性検査");
				wp_.init_list_.push_back("動特性検査");
				wp_.init_list_.push_back("LCR検査");
				kensahou_ = wd.add_widget<widget_list>(wp, wp_);
			}

			{  // 電源設定


			}
			{  // ２次負荷切替設定
				widget::param wp(vtx::irect(20 + w + 10, 20 + h * 10, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("コイル");
				wp_.init_list_.push_back("抵抗");
				fukasel_ = wd.add_widget<widget_list>(wp, wp_); 
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
			@brief  セーブ
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path)
		{
			utils::file_io fio;
			auto ret = fio.open(path, "wb");
			if(ret) {
				write_(fio, "title", title_->get_text());

				fio.close();
			}			
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& path)
		{
			utils::file_io fio;
			auto ret = fio.open(path, "rb");
			if(ret) {
				ret = decode_(fio);
				fio.close();
			}			
			return ret;
		}
	};
}
