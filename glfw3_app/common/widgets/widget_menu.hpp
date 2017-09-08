#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_menu クラス @n
			・リソース作成時、「高さ」が、メニュー一つの高さに相当する。@n
			・リソース作成後、「高さ」は、メニュー全体の高さに更新される。@n
			※メニューの文字列リストは、操作性を考慮して二重に持っている。@n
			※あまりに巨大なメニューには向いていない。@n
			※メニューは単なるテキストの集合なので、同じ文字列が許容される。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_label.hpp"
#include "utils/format.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_menu クラス @n
				※widget_label を並べた、ハイブリッド・モデル
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_menu : public widget {

		typedef widget_menu value_type;

		typedef utils::strings strings;

		typedef std::function<void (const std::string& select_text, uint32_t select_pos)> select_func_type;

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

			strings		text_list_;		///< テキスト・リスト

			uint32_t	base_height_;	///< メニュー部品、基本の高さ
			bool		round_;			///< 角をラウンドしない場合「false」

			std::string	select_text_;	///< 選択位置のテキスト（コピー）
			uint32_t	select_pos_;	///< テキスト・リストの選択位置

			select_func_type	select_func_;	///< セレクト関数

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_list_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_list_color_select_),
				text_list_(), base_height_(0), round_(true), select_text_(), select_pos_(0),
				select_func_(nullptr)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		widget_labels		list_;

		int32_t				select_pos_;
		uint32_t			select_id_;


		widget_label* build_menu_(uint32_t pos, const widget::param& wp, widget_label::param& wp_, bool mono)
		{
			if(pos == 0 && param_.round_) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				if(mono) {
					wp_.plate_param_.round_style_ = widget::plate_param::round_style::ALL;
				} else {
					wp_.plate_param_.round_style_ = widget::plate_param::round_style::TOP;
				}
			} else if(pos == (param_.text_list_.size() - 1) && param_.round_) {
				wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
				wp_.plate_param_.round_style_ = widget::plate_param::round_style::BOTTOM;
			} else {
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.round_style_ = widget::plate_param::round_style::ALL;
			}
			widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
			return w;
		}


		void build_list_()
		{
			widget::param wp(vtx::irect(vtx::ipos(0), vtx::ipos(get_rect().size.x, param_.base_height_)), this);
			widget_label::param wp_;
			wp_.plate_param_ = param_.plate_param_;
			wp_.color_param_ = param_.color_param_select_;
			wp_.plate_param_.frame_width_ = 0;
			int n = 0;
			bool mono = param_.text_list_.size() == 1 ? true : false; 
			for(const std::string& s : param_.text_list_) {
				wp_.text_param_.set_text(s);
				widget_label* w = build_menu_(n, wp, wp_, mono);
				list_.push_back(w);
				wp.rect_.org.y += get_rect().size.y;
				++n;
			}
		}

		void destroy_()
		{
			for(widget_label* w : list_) {
				wd_.del_widget(w);
			}
			list_.clear();
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_menu(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p),
			list_(), select_pos_(-1), select_id_(0)
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
			@brief	文字列リストの参照（ＲＯ）
			@return 文字列リスト
		*/
		//-----------------------------------------------------------------//
		const strings& get_list() const { return param_.text_list_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	文字列リストの参照
			@return 文字列リスト
		*/
		//-----------------------------------------------------------------//
		strings& at_list() { return param_.text_list_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	メニューのサイズを取得
			@return メニューのサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t size() const { return param_.text_list_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	メニューの全クリア
		*/
		//-----------------------------------------------------------------//
		void clear() {
			param_.text_list_.clear();
			destroy_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メニューの消去
			@param[in]	pos	消去位置
			@return 成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool erase(uint32_t pos)
		{
			if(pos >= list_.size()) return false;

			int h = param_.base_height_;
			param_.text_list_.erase(param_.text_list_.begin() + pos);
			wd_.del_widget(list_[pos]);
			list_.erase(list_.begin() + pos);

			for(uint32_t n = pos; n < list_.size(); ++n) {
				list_[n]->at_rect().org.y -= h;
			}

			// プレートのスタイルを変更
			if(param_.round_ && !list_.empty()) {
				if(list_.size() == 1) {
					widget_label* w = list_[0];
					w->at_local_param().plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					w->at_local_param().plate_param_.round_style_ = widget::plate_param::round_style::ALL;
					w->build_plate();
				} else if(pos == 0) {
					widget_label* w = list_[0];
					w->at_local_param().plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					w->at_local_param().plate_param_.round_style_ = widget::plate_param::round_style::TOP;
					w->build_plate();
				} else {
					widget_label* w = list_[list_.size() - 1];
					w->at_local_param().plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					w->at_local_param().plate_param_.round_style_ = widget::plate_param::round_style::BOTTOM;
					w->build_plate();
				}
			}

			at_rect().size.y = list_.size() * param_.base_height_;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メニューへテキストを挿入
			@param[in]	text	テキスト
			@param[in]	pos		位置
			@return 挿入できたら「true」
		*/
		//-----------------------------------------------------------------//
		bool insert(const std::string& text, uint32_t pos)
		{
			if(pos > list_.size()) return false;

			param_.text_list_.insert(param_.text_list_.begin() + pos, text);

			int h = param_.base_height_;
			widget::param wp(vtx::irect(vtx::ipos(0, pos * h), vtx::ipos(get_rect().size.x, h)), this);
			widget_label::param wp_;
			wp_.plate_param_ = param_.plate_param_;
			wp_.color_param_ = param_.color_param_select_;
			wp_.plate_param_.frame_width_ = 0;
			wp_.text_param_.set_text(text);

			widget_label* w = build_menu_(pos, wp, wp_, false);

			list_.insert(list_.begin() + pos, w);
			++pos;
			for(uint32_t n = pos; n < list_.size(); ++n) {
				list_[n]->at_rect().org.y += h;
			}

			if(param_.round_) {
				if(pos == 1 && list_.size() > 1) {
					widget_label* w = list_[pos];
					w->at_local_param().plate_param_.round_radius_ = 0;
					w->at_local_param().plate_param_.round_style_ = widget::plate_param::round_style::ALL;
					w->build_plate();
				} else if(pos == list_.size()) {
					widget_label* w = list_[pos - 2];
					w->at_local_param().plate_param_.round_radius_ = 0;
					w->at_local_param().plate_param_.round_style_ = widget::plate_param::round_style::ALL;
					w->build_plate();
				}
			}

			at_rect().size.y = list_.size() * param_.base_height_;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	メニューの最後尾へ追加
			@param[in]	text	テキスト
			@return 挿入できたら「true」
		*/
		//-----------------------------------------------------------------//
		bool push_back(const std::string& text) { return insert(text, list_.size()); }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択 ID の取得 @n
					選択される毎に＋１される
			@return 選択 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_id() const { return select_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択テキストの取得
			@return 選択テキスト
		*/
		//-----------------------------------------------------------------//
		const std::string& get_select_text() const { return param_.select_text_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置の取得
			@return 選択位置
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_pos() const { return param_.select_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	メニューの再構築 @n
					※「text_list_」を作り直してから呼ぶ事で、以前のリスト @n
					が廃棄され、新規リストになる。
		*/
		//-----------------------------------------------------------------//
		void build()
		{
			destroy_();
			build_list_();
			param_.select_text_.clear();
			param_.select_pos_ = 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			// 標準的に固定、リサイズ不可、サービス呼び出し
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::ENABLE, false);

			param_.base_height_ = get_rect().size.y;

			build_list_();

			at_rect().size.y = list_.size() * param_.base_height_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(!get_state(widget::state::ENABLE) || list_.empty()) {
				return;
			}

			wd_.top_widget(this);

			uint32_t n = 0;
			for(widget_label* w : list_) {
				if(w->get_select()) {
					param_.select_pos_ = n;
					at_local_param().text_param_.text_ = w->get_local_param().text_param_.text_;
					w->set_action(widget::action::SELECT_HIGHLIGHT);
				} else if(w->get_selected()) {
					select_pos_ = param_.select_pos_;
					if(select_pos_ < list_.size()) {
						param_.select_text_ = list_[select_pos_]->get_local_param().text_param_.get_text();
					}
					++select_id_;
				} else {
					w->set_action(widget::action::SELECT_HIGHLIGHT, false);
				}
				++n;
			}

			// list_limit_ は実装中
			// ・list_limit_ は、表示するラベルの制限を行う
			// ・スクロール・ダイアルで、リストのスクロールを行う
#if 0
			if(select_pos_ < 0) {
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
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(select_pos_ >= 0 && static_cast<uint32_t>(select_pos_) < list_.size()) {
				wd_.enable(this, false, true);
				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.select_text_, param_.select_pos_);
				}
				select_pos_ = -1;
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
		bool save(sys::preference& pre) override { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override { return true; }
	};
}
