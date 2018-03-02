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

			std::vector<widget*>	cell_;

			bool		scroll_ctrl_;	///< スクロール・コントロール（マウスのダイアル）

			param() : cell_(), scroll_ctrl_(true)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		vtx::iposs			ref_poss_;
		vtx::ipos			ref_size_;
		vtx::fpos			offset_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_table(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), ref_poss_(), ref_size_(0), offset_(0.0f)
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
//			at_param().state_.set(widget::state::CLIP_PARENTS);
//			at_param().state_.set(widget::state::AREA_ROOT);

			// 基準の位置をセーブしておく
			for(auto w : param_.cell_) {
				ref_poss_.push_back(w->at_param().rect_.org);
				if(ref_size_.x < w->get_param().rect_.end_x()) {
					ref_size_.x = w->get_param().rect_.end_x();
				}
				if(ref_size_.y < w->get_param().rect_.end_y()) {
					ref_size_.y = w->get_param().rect_.end_y();
				}
				w->at_param().parents_ = this;
				w->at_param().state_.set(widget::state::CLIP_PARENTS);
//				w->at_param().state_.set(widget::state::AREA_ROOT);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(get_focus()) {
				auto szy = get_param().rect_.size.y;
				if(ref_size_.y > szy) {
					if(param_.scroll_ctrl_) {
						const vtx::spos& scr = wd_.get_scroll();
						offset_.y += static_cast<float>(scr.y * 8);
						if(offset_.y > 0.0f) offset_.y = 0.0f;
						else if(offset_.y < -static_cast<float>(ref_size_.y - szy)) {
							offset_.y = -static_cast<float>(ref_size_.y - szy);
						}
					}
				}
			}

			uint32_t i = 0;
			for(auto w : param_.cell_) {
				w->at_param().rect_.org.x = ref_poss_[i].x + static_cast<int>(offset_.x);
				w->at_param().rect_.org.y = ref_poss_[i].y + static_cast<int>(offset_.y);
				++i;
			}
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
			if(!pre.put_position(path + "/locate",  vtx::ipos(get_rect().org))) ++err;

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
			vtx::ipos p;
			if(pre.get_position(path + "/locate", p)) {
				at_rect().org = p;
			} else {
				++err;
			}
			return err == 0;
		}
	};
}
