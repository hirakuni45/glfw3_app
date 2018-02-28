#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_table クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_null.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_table クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_table : public widget {

		typedef widget_table value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_table パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {

			std::vector<widget_null*>	cell_;

			param() : cell_()
			{ }
		};

	private:
		widget_director&	wd_;

		param		param_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_table(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_table() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const override { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const override { return "table"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const override { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	有効・無効の設定
			@param[in]	f	無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { wd_.enable(this, f, true); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::RESIZE_H_ENABLE, false);
			at_param().state_.set(widget::state::RESIZE_V_ENABLE, false);
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::MOVE_ROOT, false);
			at_param().state_.set(widget::state::RESIZE_ROOT);
			at_param().state_.set(widget::state::CLIP_PARENTS);
			at_param().state_.set(widget::state::AREA_ROOT);

			for(auto w : param_.cell_) {

			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
		}



		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(!get_state(widget::state::ENABLE)) {
				return;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override
		{

		}



		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;

			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;

			return err == 0;
		}
	};
}
