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

			bool		read_only_;		///< 読み出し専用の場合
			bool		text_in_;		///< テキスト入力状態
			uint32_t	text_in_pos_;	///< テキスト入力位置
			uint32_t	text_in_limit_;	///< テキスト入力最大数

			shift_param	shift_param_;

			bool		menu_enable_;	///< メニュー許可

			//-------------------------------------------------------------//
			/*!
				@brief	param コンストラクター
				@param[in]	text	ラベル・テキスト
				@param[in]	ro		テキスト変更の場合「false」
			*/
			//-------------------------------------------------------------//
			param(const std::string& text = "", bool ro = true) :
				plate_param_(),
				color_param_(widget_director::default_label_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
					vtx::placement(vtx::placement::holizontal::LEFT,
						vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_label_color_select_),
				read_only_(ro), text_in_(false), text_in_pos_(0), text_in_limit_(0),
				shift_param_(),
				menu_enable_(false)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		utils::lstring		text_;

		uint32_t			interval_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	select_objh_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_label(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), interval_(0), objh_(0), select_objh_(0) {
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
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const { return "label"; }


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


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストを設定
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text) {
			at_local_param().text_param_.text_ = text;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストを取得
			@return	テキスト
		*/
		//-----------------------------------------------------------------//
		const std::string& get_text() const {
			return get_local_param().text_param_.text_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エイリアス・テキストを設定
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_alias(const std::string& text) {
			at_local_param().text_param_.alias_ = text;
			at_local_param().text_param_.alias_enable_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エイリアス・テキストを取得
			@return エイリアス・テキスト
		*/
		//-----------------------------------------------------------------//
		const std::string& get_alias() const {
			return get_local_param().text_param_.alias_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	エイリアスの許可、不許可
			@param[in]	ena		不許可にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_alias(bool ena = true) {
			at_local_param().text_param_.alias_enable_ = ena;
		}
	};

	typedef std::vector<widget_label*>	widget_labels;
	typedef widget_labels::iterator		widget_labels_it;
	typedef widget_labels::const_iterator	widget_labels_cit;
}
