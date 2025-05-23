#pragma once
//=========================================================================//
/*!	@file
	@brief	GUI Widget ディレクター（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <vector>
#include <functional>
#include <iostream>
#include <boost/unordered_set.hpp>
#include "gl_fw/glmobj.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_files.hpp"
#include "utils/keyboard.hpp"
#include "utils/format.hpp"
#include "widgets/widget.hpp"
#include "widgets/common_parts.hpp"

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
			gl::mobj::handle	un_check_;		///< チェックボタン解除
			gl::mobj::handle	to_check_;		///< チェックボタン選択

			gl::mobj::handle	un_radio_;		///< ラジオボタン解除
			gl::mobj::handle	to_radio_;		///< ラジオボタン選択

			gl::mobj::handle	minus_box_;		///< マイナス箱
			gl::mobj::handle	plus_box_;		///< プラス箱

			gl::mobj::handle	up_box_;		///< up arrow
			gl::mobj::handle	up_boxp_;		///< up arrow PUSH
			gl::mobj::handle	down_box_;		///< down arrow
			gl::mobj::handle	down_boxp_;		///< down arrow PUSH
			gl::mobj::handle	left_box_;		///< left arrow
			gl::mobj::handle	left_boxp_;		///< left arrow PUSH
			gl::mobj::handle	right_box_;		///< right arrow
			gl::mobj::handle	right_boxp_;	///< right arrow PUSH

			gl::mobj::handle	VR_junction_;	///< 垂直、右、交差点（ト）
			gl::mobj::handle	R_junction_;	///< 右、交差点（L）
			gl::mobj::handle	V_line_;	  	///< 縦線（｜）
			gl::mobj::handle	H_line_;		///< 横線（―）

			share_img() : un_check_(0), to_check_(0),
				un_radio_(0), to_radio_(0),
				minus_box_(0), plus_box_(0),
				up_box_(0), up_boxp_(0), down_box_(0), down_boxp_(0),
				left_box_(0), left_boxp_(0), right_box_(0), right_boxp_(0),
				VR_junction_(0), R_junction_(0), V_line_(0), H_line_(0)
			{ }
		};

	private:
		img::img_files			img_files_;

		gl::mobj				mobj_;
		common_parts			common_parts_;
		uint32_t				serial_[8];
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
		static widget::color_param		default_toggle_color_;
		static widget::color_param		default_label_color_;
		static widget::color_param		default_label_color_select_;
		static widget::color_param		default_slider_color_;
		static widget::color_param		default_progress_color_;
		static widget::color_param		default_check_color_;
		static widget::color_param		default_checkp_color_;
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
		static widget::color_param		default_chip_color_;


		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_director() noexcept :
			img_files_(),
			mobj_(), common_parts_(mobj_),
			serial_{ 0, 5000, 10000, 15000, 20000, 25000, 30000, 35000 },
			widgets_(),
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
		const share_img& get_share_image() const noexcept { return share_img_; }


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
			uint32_t preidx = static_cast<uint32_t>(bp.pre_group_) & 7;
			w->set_serial(serial_[preidx]);
			if(bp.parents_ != nullptr) {  // 親の状態を反映
				if(!root_widget(w)->get_state(widget::state::ENABLE)) {
					w->set_state(widget::state::ENABLE, false);
				}
			}
			++serial_[preidx];
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
		void enable(widget* root, bool flag = true, bool child = false) noexcept
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
		void stall(widget* root, bool flag = true, bool child = false) noexcept
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
		bool del_widget(widget* w)
		{
			if(w == nullptr) return false;

			widgets ws;
			for(auto ww : widgets_) {
				if(ww != w) {
					ws.push_back(ww);
				}
			}
			widgets_ = ws;

			if(select_widget_ == w) select_widget_ = nullptr;
			if(move_widget_ == w) move_widget_ = nullptr;
			if(resize_l_widget_ == w) resize_l_widget_ = nullptr;
			if(resize_r_widget_ == w) resize_r_widget_ = nullptr;
			if(top_widget_ == w) top_widget_ = nullptr;
			if(focus_widget_ == w) focus_widget_ = nullptr;

			del_mark_.insert(w);

			delete w;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ペアレンツ・ウィジェットの収集
			@param[in]	pw	ペアレンツ・ウィジェット
			@param[out]	ws	ウィジェット列
		*/
		//-----------------------------------------------------------------//
		void parents_widget(widget* pw, widgets& ws) noexcept
		{
			for(auto w : widgets_) {
				if(w->get_param().parents_ == pw) {
					ws.push_back(w);
					parents_widget(w, ws);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ペアレンツ・ウィジェットの収集
			@param[in]	pw	ペアレンツ・ウィジェット
			@return		ウィジェット列
		*/
		//-----------------------------------------------------------------//
		widgets parents_widget(widget* pw) noexcept
		{
			widgets ws;
			parents_widget(pw, ws);
			return ws;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	マーキングをリセットする
		*/
		//-----------------------------------------------------------------//
		void reset_mark() noexcept
		{
			for(auto w : widgets_) {
				w->set_mark(false);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ルート・ウィジェットを返す
			@param[in]	w	基準ウィジェット
			@return ルート・ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* root_widget(widget* w) const noexcept
		{
			if(w == nullptr) return nullptr;

			do {
				if(w->get_param().parents_ == nullptr) {
					break;
				}
				w = w->get_param().parents_;
			} while(w) ;
			return w;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	最前面にする
			@param[in]	w	ウィジェット
		*/
		//-----------------------------------------------------------------//
		void top_widget(widget* w) noexcept
		{
			if(w == nullptr) return;

			reset_mark();
			widgets ws;
			ws.push_back(w);
			parents_widget(w, ws);
			for(auto cw : ws) {
				cw->set_mark();
			}
			widgets wss;
			for(auto cw : widgets_) {
				if(!cw->get_mark()) {
					wss.push_back(cw);
				}
			}
			for(auto cw : ws) {
				wss.push_back(cw);
			}
			widgets_ = wss;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	前面移動を取得
			@return 前面移動ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* get_move_widget() const noexcept { return move_widget_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	トップを取得
			@return トップ・ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* get_top_widget() const noexcept { return top_widget_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカスを取得
			@return フォーカス・ウィジェット
		*/
		//-----------------------------------------------------------------//
		widget* get_focus_widget() const noexcept { return focus_widget_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォーカスを設定
			@param[in]	w	フォーカス・ウィジェット
		*/
		//-----------------------------------------------------------------//
		void set_focus_widget(widget* w) noexcept { focus_widget_ = w; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクロールを取得（マウスのスクロール・ホイールの差分）
			@return スクロール
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_scroll() const noexcept { return scroll_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ポイント時位置の取得
			@return 位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_positive_pos() const noexcept { return position_positive_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ポイント中位置の取得
			@return 位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_level_pos() const noexcept { return position_level_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リリース位置の取得
			@return 位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_negative_pos() const noexcept { return position_negative_; }


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
		void set_TSC() const noexcept
		{
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
		const img::rgbaf& get_color() const noexcept { return color_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージ・ファイルスへの参照
			@return イメージ・ファイルス
		*/
		//-----------------------------------------------------------------//
		img::img_files& at_img_files() noexcept { return img_files_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	モーション・オブジェクトへの参照
			@return モーション・オブジェクト
		*/
		//-----------------------------------------------------------------//
		gl::mobj& at_mobj() noexcept { return mobj_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品を追加
			@return ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle share_add(const share_t& key) noexcept
		{
			return common_parts_.add(key);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品を取得
			@return ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::mobj::handle share_get(const share_t& key) noexcept
		{
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
		const common_parts& get_common_parts() const noexcept { return common_parts_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 固有の文字列を生成
			@param[in]	w	生成する widget
			@return widget 固有の文字列
		*/
		//-----------------------------------------------------------------//
		std::string create_widget_name(const widget* w) const noexcept
		{
			if(w == nullptr) return "";

			std::map<uint32_t, widget*> tbl;
			typedef std::pair<uint32_t, widget*> tbl_p;
			for(auto ww : widgets_) {
				if(w->type() == ww->type()) {
					tbl.insert(tbl_p(ww->get_serial(), ww));
				}
			}

			int n = 0;
			for(const auto& t : tbl) {
				if(t.second == w) break;
				++n;
			}

			std::string s = w->type_name();
			s += '/';
			s += (boost::format("%05d") % n).str();
			return s;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	キーボードの取得
			@return キーボード
		*/
		//-----------------------------------------------------------------//
		const sys::keyboard& get_keyboard() const noexcept { return keyboard_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キーボードの参照
			@return キーボード
		*/
		//-----------------------------------------------------------------//
		sys::keyboard& at_keyboard() noexcept { return keyboard_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーレポートを報告
			@param[in]	msg	エラーレポート文
		*/
		//-----------------------------------------------------------------//
		void add_error_report(const std::string& msg) noexcept
		{
			error_list_.push_back(msg);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーレポートを取得
			@return	エラーレポート
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get_error_report() const noexcept { return error_list_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	エラーレポートを消去
		*/
		//-----------------------------------------------------------------//
		void clear_error_report() noexcept { error_list_.clear(); }
	};


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	画像を作成
		@param[in]	wd		Widget ディレクター
		@param[in]	file	画像ファイル名
		@param[in]	rect	位置と大きさ（サイズ０指定で画像サイズで作成）
		@param[in]	parents	ペアレント
		@return 画像ボタン
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class WIDGET>
	WIDGET* create_image(widget_director& wd, const std::string& file,
		const vtx::irect& rect = vtx::irect(0), widget* parents = 0) {
		using namespace gui;

		WIDGET* w;
		widget::param wp(rect, parents);
		if(wd.at_img_files().load(file)) {
			typename WIDGET::param wp_;
			wp_.image_ = wd.at_img_files().get_image().get();
			w = wd.add_widget<WIDGET>(wp, wp_);
		} else {
			wd.add_error_report("Can't load: " + file);
			typename WIDGET::param wp_("X");
			// ロード出来ない場合の仮の大きさ
			wp.rect_.size.set(32, 32);
			w = wd.add_widget<WIDGET>(wp, wp_);
		}
		return w;
	}
}
