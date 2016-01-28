#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget スライダー・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <bitset>
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_slider クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_slider : public widget {

		typedef widget_slider value_type;

		typedef std::function< void(float) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_slider パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			widget::plate_param		plate_param_;	///< プレートパラメーター
			widget::color_param		color_param_;	///< カラーパラメーター
			widget::slider_param	slider_param_;	///< スライダーパラメーター

			const img::i_img*		base_image_;	///< ベース画像を使う場合
			const img::i_img*		hand_image_;	///< ハンドル画像を使う場合

			bool					hand_ctrl_;		///< ハンドル・コントロール
			bool					scroll_ctrl_;	///< スクロール・コントロール
			float					scroll_gain_;	///< スクロール・ゲイン

			bool					select_fin_;	///< 選択が完了した場合に呼び出す
			select_func_type		select_func_;

			param(float pos = 0.0f, slider_param::direction::type dir = slider_param::direction::HOLIZONTAL) :
				plate_param_(),
				color_param_(widget_director::default_slider_color_),
				slider_param_(),
				base_image_(0), hand_image_(0), hand_ctrl_(true),
				scroll_ctrl_(true), scroll_gain_(0.01f),
				select_fin_(false), select_func_()
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		float				ref_position_;
		vtx::ipos			ref_point_;

		vtx::ipos			handle_offset_;

		gl::mobj::handle	base_h_;	///< ベース
		gl::mobj::handle	hand_h_;	///< ハンドル

		float               position_;

		void update_offset_();
		void update_position_();

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_slider(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p), ref_position_(0.0f),
			handle_offset_(0), base_h_(0), hand_h_(0), position_(-1.0f)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_slider() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const { return "slider"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const { return false; }


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
			@brief	スライダーパラメーター取得
			@return スライダーパラメーター
		*/
		//-----------------------------------------------------------------//
		const slider_param& get_slider_param() const { return param_.slider_param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダーパラメーター参照
			@return スライダーパラメーター
		*/
		//-----------------------------------------------------------------//
		slider_param& at_slider_param() { return param_.slider_param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
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
		void service() {
			if(position_ != param_.slider_param_.position_) {
				bool f = false;
				if(param_.select_fin_) {
					if(get_select_out()) {
						f = true;
					}
				} else {
					f = true;
				}
				if(f) {
					if(param_.select_func_) param_.select_func_(param_.slider_param_.position_);
					position_ = param_.slider_param_.position_;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre);
	};
}
