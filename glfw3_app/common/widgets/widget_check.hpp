#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_check クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_check クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_check : public widget {

		typedef widget_check value_type;

		typedef std::function< void (bool) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_check 表示タイプ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct style {
			enum type {
				CHECKED,		///< チェックド・ボックス
				MINUS_PLUS,		///< minus, plus
			};
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_check パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			style::type	type_;
			widget::text_param	text_param_;
			float	gray_text_gain_;	///< 不許可時のグレースケールゲイン
			bool	disable_gray_text_;	///< 不許可時、文字をグレースケールする場合
			bool	draw_box_;			///< ボックスの表示を行わない場合 false
			bool	check_;				///< 許可、不許可の状態
			select_func_type	select_func_;

			param(const std::string& text = "", bool check = false) :
				type_(style::CHECKED),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				gray_text_gain_(0.65f), disable_gray_text_(true), draw_box_(true),
				check_(check), select_func_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		bool				obj_state_;

		gl::mobj::handle	ena_h_;
		gl::mobj::handle	dis_h_;

		bool				check_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_check(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p),
			obj_state_(false), ena_h_(0), dis_h_(0), check_(p.check_) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_check() { }


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
		const char* type_name() const override { return "check"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const override { return false; }


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
			@brief	状態を取得
			@return チェックなら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_check() const { return param_.check_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	状態を設定
			@param[in]	f	非選択状態の場合「false」
		*/
		//-----------------------------------------------------------------//
		void set_check(bool f = true) { param_.check_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override;


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override;


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override {
			if(check_ != param_.check_) {
				if(param_.select_func_ != nullptr) param_.select_func_(param_.check_);
				check_ = param_.check_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override;


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override;
	};
}
