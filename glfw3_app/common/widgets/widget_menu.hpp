#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_menu クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_label.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_menu クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_menu : public widget {

		typedef widget_menu value_type;

		typedef std::function<void (const std::string&)> select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_menu パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param	plate_param_;	///< プレート・パラメーター
			color_param	color_param_;	///< カラー・パラメーター
			text_param	text_param_;	///< テキスト描画のパラメーター
			color_param	color_param_select_;	///< 選択時カラー・パラメーター

			utils::strings	text_list_;	///< テキスト・リスト

			bool		round_;			///< 角をラウンドしない場合「false」

			std::string	select_text_;	///< 選択位置のテキスト
			uint32_t	select_pos_;	///< テキスト・リストの選択位置

			select_func_type	select_func_;	///< セレクト関数

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_list_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_list_color_select_),
				text_list_(), round_(true), select_text_(), select_pos_(0),
				select_func_(nullptr)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		widget_labels		list_;

		uint32_t			select_id_;

		void destroy_() {
			for(widget_label* w : list_) {
				wd_.del_widget(w);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_menu(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p),
			list_(), select_id_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_menu() { destroy_(); }


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
		const char* type_name() const override { return "menu"; }


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
			@brief	選択 ID の取得
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_id() const { return select_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択テキストの取得
		*/
		//-----------------------------------------------------------------//
		const std::string& get_select_text() const { return param_.select_text_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置の取得
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_pos() const { return param_.select_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override {
			// 標準的に固定、リサイズ不可
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::ENABLE, false);

			widget::param wp(vtx::irect(vtx::spos(0), get_rect().size), this);
			widget_label::param wp_;
			wp_.plate_param_ = param_.plate_param_;
			wp_.color_param_ = param_.color_param_select_;
			wp_.plate_param_.frame_width_ = 0;
			int n = 0;
			for(const std::string& s : param_.text_list_) {
				wp_.text_param_.set_text(s);
				if(n == 0 && param_.round_) {
					wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					wp_.plate_param_.round_style_
						= widget::plate_param::round_style::TOP;
				} else if(n == (param_.text_list_.size() - 1) && param_.round_) {
					wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					wp_.plate_param_.round_style_
						= widget::plate_param::round_style::BOTTOM;
				} else {
					wp_.plate_param_.round_radius_ = 0;
					wp_.plate_param_.round_style_
						= widget::plate_param::round_style::ALL;
				}
				widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
				w->set_state(widget::state::ENABLE, false);
				list_.push_back(w);
				wp.rect_.org.y += get_rect().size.y;
				++n;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override {
			if(!get_state(widget::state::ENABLE) || list_.empty()) {
				return;
			}

			wd_.top_widget(this);

			uint32_t n = 0;
			bool selected = false;
			for(widget_label* w : list_) {
				if(w->get_select()) {
					param_.select_pos_ = n;
					at_local_param().text_param_.text_
						= w->get_local_param().text_param_.text_;
				} else if(w->get_selected()) {
					selected = true;
				}
				++n;
			}
			if(selected) {
				if(param_.select_pos_ < list_.size()) {
					param_.select_text_ = list_[param_.select_pos_]->get_local_param().text_param_.get_text();
				}
				++select_id_;
				wd_.enable(this, false, true);
				if(param_.select_func_ != nullptr) param_.select_func_(param_.select_text_);
			} else {
				const vtx::spos& scr = wd_.get_scroll();
				if(get_focus() && scr.y != 0) {
					int pos = param_.select_pos_;
					pos += scr.y;
					if(pos < 0) {
						pos = 0;
					} else if(pos >= static_cast<int>(list_.size())) {
						pos = list_.size() - 1;
					}
					param_.select_pos_ = pos;
				}
				uint32_t n = 0;
				for(widget_label* w : list_) {
					if(n == param_.select_pos_) {
						w->set_action(widget::action::SELECT_HIGHLIGHT);
					} else {
						w->set_action(widget::action::SELECT_HIGHLIGHT, false);
					}
					++n;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override { }


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
