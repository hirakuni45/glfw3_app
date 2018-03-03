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
#include "widgets/widget_scrollbar.hpp"

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

			vtx::ipos	item_size_;		///< アイテムの大きさ

			bool		scroll_bar_h_;	///< 水平スクロール・バーによる制御
			bool		scroll_bar_v_;	///< 垂直スクロール・バーによる制御
			bool		scroll_ctrl_;	///< スクロール・コントロール（マウスのダイアル）

			param() : cell_(), item_size_(0),
				scroll_bar_h_(false), scroll_bar_v_(false), scroll_ctrl_(true)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		widget_null*		base_;
		widget_scrollbar*	scroll_h_;
		widget_scrollbar*	scroll_v_;

		vtx::ipos			max_;
		vtx::fpos			offset_;
		vtx::ipos			chip_size_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_table(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p),
			base_(nullptr), scroll_h_(nullptr), scroll_v_(nullptr),
			max_(0), offset_(0.0f), chip_size_(0)
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
			@brief	水平スクロールバーの取得
			@return 水平スクロールバー
		*/
		//-----------------------------------------------------------------//
		widget_scrollbar* get_scrollbar_h() const { return scroll_h_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	垂直スクロールバーの取得
			@return 垂直スクロールバー
		*/
		//-----------------------------------------------------------------//
		widget_scrollbar* get_scrollbar_v() const { return scroll_v_; }


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
			at_param().state_.set(widget::state::RESIZE_ROOT);
//			at_param().state_.set(widget::state::CLIP_PARENTS);
//			at_param().state_.set(widget::state::MOVE_STALL);
//			at_param().state_.set(widget::state::MOVE_ROOT, false);
//			at_param().state_.set(widget::state::AREA_ROOT);

			auto h = wd_.get_share_image().down_box_;
			chip_size_ = wd_.at_mobj().get_size(h);

			vtx::ipos msz = get_rect().size;
			if(param_.scroll_bar_h_) {
				msz.y -= chip_size_.y;
			}
			if(param_.scroll_bar_v_) {
				msz.x -= chip_size_.x;
			}

			for(auto w : param_.cell_) {  // 子の最大値をスキャン
				if(max_.x < w->get_rect().end_x()) max_.x = w->get_rect().end_x();
				if(max_.y < w->get_rect().end_y()) max_.y = w->get_rect().end_y();
			}
			{
				widget::param wp(vtx::irect(0, 0, max_.x, max_.y), this);
				wp.state_.set(widget::state::CLIP_PARENTS);
				widget_null::param wp_;
				base_ = wd_.add_widget<widget_null>(wp, wp_);
			}
			for(auto w : param_.cell_) {  // 子の基本設定
				w->at_param().parents_ = base_;
				w->at_param().state_.set(widget::state::CLIP_PARENTS);
			}

			if(param_.scroll_bar_h_) {
				widget::param wp(vtx::irect(0, 0, 0, 0), this);
				widget_scrollbar::param wp_(widget_scrollbar::style::HOLIZONTAL);
				wp_.handle_ratio_ = static_cast<float>(msz.x) / static_cast<float>(max_.x);
				wp_.scroll_step_  =
					static_cast<float>(param_.item_size_.x) / static_cast<float>(max_.x - msz.x);
				wp_.scroll_gain_  = wp_.scroll_step_ * 0.5f;
				scroll_h_ = wd_.add_widget<widget_scrollbar>(wp, wp_);
			}
			if(param_.scroll_bar_v_) {
				widget::param wp(vtx::irect(0, 0, 0, 0), this);
				widget_scrollbar::param wp_(widget_scrollbar::style::VERTICAL);
				wp_.handle_ratio_ = static_cast<float>(msz.y) / static_cast<float>(max_.y);
				wp_.scroll_step_  =
					static_cast<float>(param_.item_size_.y) / static_cast<float>(max_.y - msz.y);
				wp_.scroll_gain_  = wp_.scroll_step_ * 0.5f;
				scroll_v_ = wd_.add_widget<widget_scrollbar>(wp, wp_);
			}

			// 親の状態を子に反映
			if(get_param().parents_ != nullptr) {
				wd_.enable(this, get_param().parents_->get_enable(), true);
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
				if(param_.scroll_ctrl_) {
#if 0
					const vtx::spos& scr = wd_.get_scroll();
					float& pos = scroll_v_->get_slider()->at_position();
					pos += static_cast<float>(scr.y)
						* scroll_v_->get_local_param().scroll_step_ * 0.5f;
					if(pos < 0.0f) pos = 0.0f;
					else if(pos > 1.0f) pos = 1.0f;
#endif
				}
#if 0
				gl::core& core = gl::core::get_instance();
				const gl::device& dev = core.get_device();
				int step = param_.page_step_;
				if(param_.page_div_ != 0) {
					step = (param_.max_pos_ - param_.min_pos_) / param_.page_div_;
				}
				if(dev.get_positive(gl::device::key::PAGE_UP)) {
					d =  step;
					st = state::inc;
				} else if(dev.get_positive(gl::device::key::PAGE_DOWN)) {
					d = -step;
					st = state::dec;
				} else if(dev.get_positive(gl::device::key::UP)) {
					d = step >= 0 ? 1 : -1;
					st = state::inc;
				} else if(dev.get_positive(gl::device::key::DOWN)) {
					d = step < 0 ? -1 : 1;
				}
#endif
			}

			if(param_.scroll_bar_v_ && scroll_v_ != nullptr) {
				auto pr = scroll_v_->get_slider()->get_position();
				offset_.y = pr * static_cast<float>(max_.y - get_rect().size.y);
			}
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
			base_->at_rect().org.x = -offset_.x;
			base_->at_rect().org.y = -offset_.y;
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
