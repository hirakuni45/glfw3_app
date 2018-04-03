#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget スクロール・バー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <bitset>
#include "core/glcore.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_arrow.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_scrollbar クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_scrollbar : public widget {

		typedef widget_scrollbar value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_scrollbar スタイル
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class style {
			HOLIZONTAL,		///< 下側
			VERTICAL,		///< 右側
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_scrollbar ステート
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class state {
			none,	///< 何もしない
			initial,///< 初期化
			inc,	///< インクリメント
			update,	///< スライダーを更新
			dec,	///< デクリメント
		};

		typedef std::function< void(state st, float pos) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_scrollbar パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			style				style_;			///< スクロール・バーのスタイル
			float				handle_ratio_;	///< スクロール・ハンドルの比率
			float				scroll_gain_;	///< スクロール・ゲイン
			float				scroll_step_;	///< スクロール・ステップ

			select_func_type	select_func_;

			param(style st) : style_(st), handle_ratio_(0.25f),
				scroll_gain_(0.1f), scroll_step_(0.1f),
				select_func_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		widget_arrow*		arrow_inc_;
		widget_slider*		slider_;
		widget_arrow*		arrow_dec_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_scrollbar(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p),
			arrow_inc_(nullptr), slider_(nullptr), arrow_dec_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_scrollbar() { }


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
		const char* type_name() const override { return "scrollbar"; }


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
			@brief	スライダーの取得
			@return スライダー
		*/
		//-----------------------------------------------------------------//
		widget_slider* get_slider() const { return slider_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			if(get_param().parents_ == nullptr) {  // 親が必ず必要
				return;
			}

			// 標準的設定
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);

			const auto& msz = get_param().parents_->get_rect().size;
			auto bs = wd_.at_mobj().get_size(wd_.get_share_image().up_box_);

			if(param_.style_ == style::VERTICAL) {
				at_rect().org.x = msz.x - bs.x;
				at_rect().org.y = 0;
				at_rect().size.x = bs.x;
				at_rect().size.y = msz.y;
				{
	                widget::param wp(vtx::irect(0, 0, 0, 0), this);
					wp.pre_group_ = get_param().pre_group_;
					widget_arrow::param wp_(widget_arrow::direction::up);
					arrow_inc_ = wd_.add_widget<widget_arrow>(wp, wp_);
					arrow_inc_->at_local_param().select_func_ = [=](uint32_t level) {
						float& pos = slider_->at_position();
						pos -= param_.scroll_step_;
						if(pos < 0.0f) pos = 0.0f;
					};
				}
				{
	                widget::param wp(vtx::irect(0, bs.y, bs.x, msz.y - bs.y * 2), this);
					wp.pre_group_ = get_param().pre_group_;
					widget_slider::param wp_(0.0f, slider_param::direction::VERTICAL);
					wp_.slider_param_.handle_ratio_ = param_.handle_ratio_;
					wp_.scroll_gain_ = param_.scroll_gain_;
					wp_.plate_param_.round_radius_ = 0;
					slider_ = wd_.add_widget<widget_slider>(wp, wp_);
				}
				{
	                widget::param wp(vtx::irect(0, msz.y - bs.y, 0, 0), this);
					wp.pre_group_ = get_param().pre_group_;
					widget_arrow::param wp_(widget_arrow::direction::down);
					arrow_dec_ = wd_.add_widget<widget_arrow>(wp, wp_);
					arrow_dec_->at_local_param().select_func_ = [=](uint32_t level) {
						float& pos = slider_->at_position();
						pos += param_.scroll_step_;
						if(pos > 1.0f) pos = 1.0f;
					};
				}
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
			if(!get_enable()) {
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
		bool save(sys::preference& pre) override {
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override {
			return true;
		}
	};
}
