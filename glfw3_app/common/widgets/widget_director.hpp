#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ディレクター（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include "widgets/widget.hpp"
#include "widgets/common_parts.hpp"
#include "gl_fw/glmobj.hpp"
#include "img_io/paint.hpp"
#include "img_io/img_files.hpp"

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
			gl::glmobj::handle	un_check_;	///< チェックボタン解除
			gl::glmobj::handle	to_check_;	///< チェックボタン選択

			gl::glmobj::handle	un_radio_;	///< ラジオボタン解除
			gl::glmobj::handle	to_radio_;	///< ラジオボタン選択

			gl::glmobj::handle	minus_box_;	///< マイナス
			gl::glmobj::handle	plus_box_;	///< プラス

			gl::glmobj::handle	up_box_;	///< up arrow
			gl::glmobj::handle	down_box_;	///< down arrow
			gl::glmobj::handle	left_box_;	///< left arrow
			gl::glmobj::handle	right_box_;	///< right arrow

			share_img() : un_check_(0), to_check_(0),
				un_radio_(0), to_radio_(0),
				minus_box_(0), plus_box_(0),
				up_box_(0), down_box_(0), left_box_(0), right_box_(0)
			{ }
		};

	private:
		img::img_files			img_files_;

		gl::glmobj				mobj_;
		common_parts			common_parts_;
		uint32_t				serial_;
		widgets					widgets_;

		float					unselect_length_;
		vtx::fvtx	   			position_;
		float					scale_;
		img::rgbaf				color_;

		widget*					top_move_;
		widget*					top_resize_;

		float					msp_length_;
		vtx::spos				msp_speed_;
		vtx::spos				position_positive_;
		vtx::spos				position_level_;
		vtx::spos				position_negative_;
		vtx::spos				scroll_;

		share_img				share_img_;

		void message_widget_(widget* w, const std::string& s);
		void parents_widget_mark_(widget* root);
	public:
		static widget::color_param		default_frame_color_;
		static widget::color_param		default_button_color_;
		static widget::color_param		default_label_color_;
		static widget::color_param		default_label_color_select_;
		static widget::color_param		default_slider_color_;
		static widget::color_param		default_check_color_;
		static widget::color_param		default_list_color_;
		static widget::color_param		default_list_color_select_;
		static widget::color_param		default_dialog_color_;
		static widget::color_param		default_filer_color_;
		static widget::color_param		default_tree_color_;
		static widget::color_param		default_terminal_color_;

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_director() : img_files_(),
			mobj_(), common_parts_(mobj_), serial_(0), widgets_(),
			unselect_length_(7.0f), position_(0.0f), scale_(1.0f),
			top_move_(0), top_resize_(0),
			msp_length_(0.0f), msp_speed_(0),
			position_positive_(0), position_level_(0), position_negative_(0),
			scroll_(0)
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
			@param[in]	p	T 型パラメーター
		*/
		//-----------------------------------------------------------------//
		template <class T>
		T* add_widget(const widget::param& bp, const typename T::param& p) {
			T* w = new T(*this, bp, p);
			w->initialize();
			w->set_serial(serial_);
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
		void enable(widget* root, bool flag = true, bool child = false);


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
		widget* get_top_move() const { return top_move_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクロールを取得（マウスのスクロール・ホイールの差分）
			@return スクロール
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_scroll() const { return scroll_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	挙動制御
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


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
		gl::glmobj& at_mobj() { return mobj_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品を追加
			@return ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::glmobj::handle share_add(const share_t& key) {
			return common_parts_.add(key);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	共通部品を取得
			@return ハンドル
		*/
		//-----------------------------------------------------------------//
		gl::glmobj::handle share_get(const share_t& key) {
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
	};
}
