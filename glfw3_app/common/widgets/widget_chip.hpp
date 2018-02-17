#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_chip クラス @n
			※「バルーンヘルプ」のようなＧＵＩ
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_chip クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_chip : public widget {

		typedef widget_chip value_type;

		typedef std::function< void(uint32_t) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_chip パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;	///< 平面描画パラメーター
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			text_param		text_param_;	///< テキスト描画のパラメータ
			uint32_t		id_;			///< セレクト ID （押された回数）

			select_func_type	select_func_;	///< セレクト関数

			param(const std::string& text = "") :
				plate_param_(15, 0), color_param_(widget_director::default_chip_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255)),
				id_(0),
				select_func_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

		uint32_t			id_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_chip(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), objh_(0), id_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_chip() { }


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
		const char* type_name() const override { return "chip"; }


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
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			// サイズ固定
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);
			at_param().action_.set(widget::action::SELECT_SCALE, false);

			using namespace img;

			vtx::spos size;
			if(param_.plate_param_.resizeble_) {
				vtx::spos rsz = param_.plate_param_.grid_ * 3;
				if(get_param().rect_.size.x >= rsz.x) size.x = rsz.x;
				else size.x = get_param().rect_.size.x;
				if(get_param().rect_.size.y >= rsz.y) size.y = rsz.y;
				else size.y = get_param().rect_.size.y;
			} else {
				size = get_param().rect_.size;
			}
			share_t t;
			t.size_ = size;
			t.color_param_ = param_.color_param_;
			t.plate_param_ = param_.plate_param_;
			objh_ = wd_.share_add(t);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(get_selected()) {
				++param_.id_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(!get_state(state::ENABLE)) {
				return;
			}

			if(id_ != param_.id_) {
				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.id_);
				}
				id_ = param_.id_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override
		{
			if(objh_ == 0) return;

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(objh_, get_param().rect_.size);
			}

			render_text(wd_, objh_, get_param(), param_.text_param_, param_.plate_param_);
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


		//-----------------------------------------------------------------//
		/*!
			@brief	位置を参照
			@param[in]	pos	位置
		*/
		//-----------------------------------------------------------------//
		vtx::ipos& at_org() { return at_param().rect_.org; }


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストを設定
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text) {
			param_.text_param_.set_text(text);
		}
	};
}
