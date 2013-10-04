#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_label クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_label クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_label : public widget {

		typedef widget_label value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	Widget label パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param	plate_param_;	///< プレート・パラメーター
			color_param	color_param_;	///< カラーパラメーター
			text_param	text_param_;	///< テキスト描画のパラメーター
			color_param	color_param_select_;	///< 選択時のカラーパラメーター

			bool		shift_enable_;	///< シフト表示を有効にする場合
			bool		shift_every_;	///< 常にシフト動作の場合
			float		shift_offset_;	///< シフト表示のオフセット
			float		shift_speed_;	///< シフト表示の速度
			uint32_t	shift_hold_frame_;	///< シフト表示までのホールドフレーム

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_label_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
					vtx::placement(vtx::placement::holizontal::LEFT,
						vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_label_color_select_),
				shift_enable_(true), shift_every_(false),
				shift_offset_(0.0f), shift_speed_(0.5f),
				shift_hold_frame_(2 * 60)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	select_objh_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_label(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p), objh_(0), select_objh_(0) {
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_label() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


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
		void service() { }
	};

	typedef std::vector<widget_label*>	widget_labels;
	typedef widget_labels::iterator		widget_labels_it;
	typedef widget_labels::const_iterator	widget_labels_cit;
}
