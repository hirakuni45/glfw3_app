#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_dialog クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_dialog クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_dialog : public widget {

		typedef widget_dialog value_type;

		typedef std::function< void (bool ok) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ダイアログ・スタイル
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class style {
			OK,			///< OK ボタンのみ
			CANCEL_OK,	///< キャンセル、OK ボタン
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_dialog パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;
			color_param		color_param_;

			style			style_;		///< ダイアログ・スタイル

			vtx::irect		text_area_;	///< テキスト表示エリア

			bool			return_ok_;
			bool			return_cancel_;

			select_func_type	select_func_;

			param(style s = style::OK) : plate_param_(),
				color_param_(widget_director::default_dialog_color_),
				style_(s),
				text_area_(vtx::ipos(8), vtx::ipos(0)),
				return_ok_(false), return_cancel_(false)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

		widget_text*		text_;
		widget_button*		ok_;
		widget_button*		cancel_;

		void destroy_() {
			wd_.del_widget(cancel_);
			wd_.del_widget(ok_);
			wd_.del_widget(text_);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_dialog(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p), objh_(0),
			text_(0), ok_(0), cancel_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_dialog() { destroy_(); }


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
		const char* type_name() const override { return "dialog"; }


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
			@brief	文書の設定
			@param[in]	text	文書
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text) {
			if(text_) {
				text_->at_local_param().text_param_.set_text(text);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文書の取得
			@return	文書
		*/
		//-----------------------------------------------------------------//
		std::string get_text() const {
			std::string s;
			if(text_) {
				s = text_->get_local_param().text_param_.get_text();
			}
			return s;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	Ok ボタンを返す
			@return	Ok ボタン
		*/
		//-----------------------------------------------------------------//
		widget_button* ok_button() { return ok_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	Cancel ボタンを返す
			@return	Cancel ボタン
		*/
		//-----------------------------------------------------------------//
		widget_button* cancel_button() { return cancel_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override {
			// 自由な大きさの変更を禁止
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::SERVICE);

			param_.plate_param_.resizeble_ = true;
			// フレームの生成
			objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

			// 構成部品の作成
			const vtx::spos& size = get_rect().size;

			short fw = param_.plate_param_.frame_width_;
			short btn_width = 100;
			short btn_height = 40;
			short space_height = 10;
			vtx::spos btn_size(btn_width, btn_height);
			short y = size.y - space_height - btn_height;
			if(param_.style_ == style::OK) {
				widget::param wp(vtx::irect(vtx::ipos(
					(size.x - fw - btn_width) / 2, y),
					btn_size), this);
				widget_button::param wp_("OK");
				ok_ = wd_.add_widget<widget_button>(wp, wp_);
			} else if(param_.style_ == style::CANCEL_OK) {
				short ofs = (size.x - fw * 2 - btn_width * 2) / 3;
				{
					widget::param wp(vtx::irect(vtx::ipos(fw + ofs, y),
					btn_size), this);
					widget_button::param wp_("Cancel");
					cancel_ = wd_.add_widget<widget_button>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(vtx::ipos(fw + ofs + btn_width + ofs, y),
					btn_size), this);
					widget_button::param wp_("OK");
					ok_ = wd_.add_widget<widget_button>(wp, wp_);
				}
			}

			{
				param_.text_area_.size.x = size.x - param_.text_area_.org.x * 2;
				param_.text_area_.size.y = size.y - param_.text_area_.org.y * 2
					- space_height - btn_height;
				widget::param wp(param_.text_area_, this);
				widget_text::param wp_;
				wp_.text_param_.placement_.hpt = vtx::placement::holizontal::CENTER;
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				text_ = wd_.add_widget<widget_text>(wp, wp_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override {
			bool close = false;
			if(ok_ && ok_->get_selected()) {
				param_.return_ok_     = true;
				param_.return_cancel_ = false;
				close = true;
			}
			if(cancel_ && cancel_->get_selected()) {
				param_.return_ok_     = false;
				param_.return_cancel_ = true;
				close = true;
			}

			if(close) {
				enable(false);
				if(param_.select_func_ != nullptr) param_.select_func_(param_.return_ok_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override {
			if(objh_ == 0) return;

			wd_.at_mobj().resize(objh_, get_param().rect_.size);
			glEnable(GL_TEXTURE_2D);
			wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));
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
