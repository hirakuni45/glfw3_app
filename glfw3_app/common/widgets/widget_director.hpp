#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ディレクター（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include <functional>
#include <boost/unordered_set.hpp>
#include "widgets/widget.hpp"
#include "widgets/common_parts.hpp"
#include "gl_fw/glmobj.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_files.hpp"
#include "utils/keyboard.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	gui widgetDirector クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_director {

		typedef std::vector<widget*>		widgets;
		typedef widgets::iterator			widgets_it;
		typedef widgets::const_iterator		widgets_cit;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	共通画像構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct share_img {
			gl::mobj::handle	un_check_;	///< チェックボタン解除
			gl::mobj::handle	to_check_;	///< チェックボタン選択

			gl::mobj::handle	un_radio_;	///< ラジオボタン解除
			gl::mobj::handle	to_radio_;	///< ラジオボタン選択

			gl::mobj::handle	minus_box_;	///< マイナス箱
			gl::mobj::handle	plus_box_;	///< プラス箱

			gl::mobj::handle	up_box_;	///< up arrow
			gl::mobj::handle	down_box_;	///< down arrow
			gl::mobj::handle	left_box_;	///< left arrow
			gl::mobj::handle	right_box_;	///< right arrow

			gl::mobj::handle	VR_junction_;	///< 垂直、右、交差点（ト）
			gl::mobj::handle	R_junction_;	///< 右、交差点（L）
			gl::mobj::handle	V_line_;	  	///< 縦線（｜）
			gl::mobj::handle	H_line_;		///< 横線（―）

			share_img() : un_check_(0), to_check_(0),
				un_radio_(0), to_radio_(0),
				minus_box_(0), plus_box_(0),
				up_box_(0), down_box_(0), left_box_(0), right_box_(0),
				VR_junction_(0), R_junction_(0), V_line_(0), H_line_(0)
			{ }
		};

	private:
		img::img_files			img_files_;

		gl::mobj				mobj_;
		common_parts			common_parts_;
		uint32_t				serial_;
		widgets					widgets_;
		widgets					ci_widgets_;

		vtx::fvtx	   			position_;
		float					scale_;
		img::rgbaf				color_;

		widget*					select_widget_;
		widget*					move_widget_;
		widget*					resize_l_widget_;
		widget*					resize_r_widget_;

		widget*					top_widget_;

		widget*					focus_widget_;

		float					msp_length_;
		vtx::ipos				msp_speed_;
		vtx::ipos				position_positive_;
		vtx::ipos				position_level_;
		vtx::ipos				position_negative_;
		vtx::ipos				scroll_;

		share_img				share_img_;

		sys::keyboard			keyboard_;

		boost::unordered_set<widget*>	del_mark_;

		utils::strings			error_list_;

		float					unselect_length_;

		uint32_t				render_num_ = 0;

		void message_widget_(widget* w, const std::string& s);
		void parents_widget_mark_(widget* root);
		void unselect_parents_(widget* root);

	public:
		static widget::color_param		default_frame_color_;
		static widget::color_param		default_border_color_;
		static widget::color_param		default_button_color_;
		static widget::color_param		default_label_color_;
		static widget::color_param		default_label_color_select_;
		static widget::color_param		default_slider_color_;
		static widget::color_param		default_progress_color_;
		static widget::color_param		default_check_color_;
		static widget::color_param		default_list_color_;
		static widget::color_param		default_list_color_select_;
		static widget::color_param		default_menu_color_;
		static widget::color_param		default_menu_color_select_;
		static widget::color_param		default_spinbox_color_;
		static widget::color_param		default_dialog_color_;
		static widget::color_param		default_filer_color_;
		static widget::color_param		default_tree_color_;
		static widget::color_param		default_terminal_color_;
		static widget::color_param		default_sheet_color_;

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_director() :
			img_files_(),
			mobj_(), common_parts_(mobj_), serial_(0), widgets_(),
			position_(0.0f), scale_(1.0f),
			select_widget_(nullptr),
			move_widget_(nullptr),
			resize_l_widget_(nullptr), resize_r_widget_(nullptr),
			top_widget_(nullptr), focus_widget_(nullptr),
			msp_length_(0.0f), msp_speed_(0),
			position_positive_(0), position_level_(0), position_negative_(0),
			scroll_(0), share_img_(), keyboard_(), del_mark_(),
			unselect_length_(6.0f)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~widget_director() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	共通画像を取得
			@return 共通画像構造体の参照
		*/
		//-----------------------------------------------------------------//
		const share_img& get_share_image() const { return share_img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	部品の追加
			@param[in]	bp	ベース型パラメーター
			@param[in]	tp	T 型パラメーター
		*/
		//-----------------------------------------------------------------//
		template <class T>
		T* add_widget(const widget::param& bp, const typename T::param& tp) {
			T* w = new T(*this, bp, tp);
			w->initialize();
			w->set_serial(serial_);
			if(bp.parents_ != nullptr) {  // 親の状態を反映
				if(!root_widget(w)->get_state(widget::state::ENABLE)) {
					w->set_state(widget::state::ENABLE, false);
				}
			}
			++serial_;
			widgets_.push_back(w);
			return w;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ウィジェットの許可、不許可
			@param[in]	root	ウィジェット
			@param[in]	flag	不許可なら「false」
			@param[in]	child	子も不許可にする場合「true」
		*/
		//-----------------------------------------------------------------//
		void enable(widget* root, bool flag = true, bool child = false)
		{
			if(root == nullptr) return;

			if(!root->get_param().state_[widget::state::ENABLE] && flag) {
				set_focus_widget(root);
			}
			root->at_param().state_[widget::state::ENABLE] = flag;
			if(!child) {
				return;
			}

			widgets ws;
			parents_widget(root, ws);
			for(auto w : ws) {
				w->at_param().state_[widget::state::ENABLE] = flag;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ウィジェットのストール設定
			@param[in]	root	ウィジェット
			@param[in]	flag	不許可なら「false」
			@param[in]	child	子も不許可にする場合「true」
		*/
		//-----------------------------------------------------------------//
		void stall(widget* root, bool flag = true, bool child = false)
		{
			if(root == nullptr) return;

			root->set_stall(flag);
			if(!child) {
				return;
			}

			widgets ws;
			parents_widget(root, ws);
			for(auto w : ws) {
				w->set_stall(flag);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ウィジェットの削除
			@param[in]	w	ウィジェット
		*/
		//-----------------------------------------------------------------//
		bool del_widget(widget* w);


		//-----------------------------------------------------------------//
		/*!
			@brief	ペアレンツ・ウィジェットの収集
			@param[in]	pw	ペアレンツ・ウィジェット
			@param[out]	ws	ウィジェット列
		*/
		//-----------------------------------------------------------------//
		void parents_widget(widget* pw, widgets& ws);


		//-----------------------------------------------------------------//
		/*!
			@brief	マーキングをリセットする
		*/
		//-----------------------------------------------------------------//
		void reset_mark();


		//-----------------------------------------------------------------//
		/*!
			@brief	ルート・ウィジェットを返す
			@param[in]	w	基準ウィジェット
			@return ルート・ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* root_widget(widget* w) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	最前面にする
			@param[in]	w	ウィジェット
		*/
		//-----------------------------------------------------------------//
		void top_widget(widget* w);


		//-----------------------------------------------------------------//
		/*!
			@brief	前面移動を取得
			@return 前面移動ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* get_move_widget() const { return move_widget_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	トップを取得
			@return トップ・ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* get_top_widget() const { return top_widget_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカスを取得
			@return フォーカス・ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* get_focus_widget() const { return focus_widget_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカスを設定
			@param[in]	w	フォーカス・ウィジェット
		*/
		//-----------------------------------------------------------------//
		void set_focus_widget(widget* w) { focus_widget_ = w; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクロールを取得（マウスのスクロール・ホイールの差分）
			@return スクロール
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_scroll() const { return scroll_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ポイント時位置の取得
			@return 位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_positive_pos() const { return position_positive_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ポイント中位置の取得
			@return 位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_level_pos() const { return position_level_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リリース位置の取得
			@return 位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_negative_pos() const { return position_negative_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	挙動制御
			@return 操作があった場合「true」
		*/
		//-----------------------------------------------------------------//
		bool update();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	T.S.C の設定
		*/
		//-----------------------------------------------------------------//
		void set_TSC() const {
			glTranslatef(position_.x, position_.y, position_.z);
			glScalef(scale_, scale_, 1.0f);
			glColor4f(color_.r, color_.g, color_.b, color_.a);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カレント・カラーの取得
			@return カレント・カラー
		*/
		//-----------------------------------------------------------------//
		const img::rgbaf& get_color() const { return color_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージ・ファイルスへの参照
			@return イメージ・ファイルス
		*/
		//-----------------------------------------------------------------//
		img::img_files& at_img_files() { return img_files_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクトへの参照
			@return モーション・オブジェクト
		*/
		//-----------------------------------------------------------------//
		gl::mobj& at_mobj() { return mobj_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品を追加
			@return ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle share_add(const share_t& key) {
			return common_parts_.add(key);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品を取得
			@return ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle share_get(const share_t& key) {
			return common_parts_.get(key);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アクション・モニター
			@param[in]	wd	widget_director
		*/
		//-----------------------------------------------------------------//
		void action_monitor();


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品の取得
			@return 共通部品構造体の参照
		*/
		//-----------------------------------------------------------------//
		const common_parts& get_common_parts() const { return common_parts_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 固有の文字列を生成
			@param[in]	w	生成する widget
			@return widget 固有の文字列
		*/
		//-----------------------------------------------------------------//
		const std::string create_widget_name(const widget* w) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	キーボードの取得
			@return キーボード
		*/
		//-----------------------------------------------------------------//
		const sys::keyboard& get_keyboard() const { return keyboard_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キーボードの参照
			@return キーボード
		*/
		//-----------------------------------------------------------------//
		sys::keyboard& at_keyboard() { return keyboard_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーレポートを報告
			@param[in]	msg	エラーレポート文
		*/
		//-----------------------------------------------------------------//
		void add_error_report(const std::string& msg) {
			error_list_.push_back(msg);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーレポートを取得
			@return	エラーレポート
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get_error_report() const { return error_list_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーレポートを消去
		*/
		//-----------------------------------------------------------------//
		void clear_error_report() { error_list_.clear(); }
	};
}
