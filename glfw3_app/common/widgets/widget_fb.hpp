#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget フレーム・バッファ @n
			gl::texfb クラスをアタッチしている。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_frame.hpp"
#include "utils/terminal.hpp"
#include "core/glcore.hpp"
#include "gl_fw/gltexfb.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI gl::texfb クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_fb : public widget {

		typedef widget_fb value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_fb パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {

			vtx::spos	ms_pos_;		///< エリア内マウスポインター（相対）
			bool		ms_level_;		///< ボタンレベル
			bool		ms_positive_;	///< ボタントリガー（ON）
			bool		ms_negative_;	///< ボタントリガー（OFF）

			param() : ms_pos_(0), ms_level_(false), ms_positive_(false), ms_negative_(false)
			{ }
		};

	private:

		widget_director&	wd_;

		param				param_;

		gl::texfb			fb_;

		bool				focus_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_fb(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), fb_(), focus_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_fb() { }


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
		const char* type_name() const override { return "fb"; }


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
			@brief	フレームバッファへの参照
			@return フレームバッファ
		*/
		//-----------------------------------------------------------------//
		auto& at_fb() { return fb_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::RESIZE_H_ENABLE, false);
			at_param().state_.set(widget::state::RESIZE_V_ENABLE, false);
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::MOVE_ROOT, false);
			at_param().state_.set(widget::state::MOVE_STALL);
			at_param().state_.set(widget::state::RESIZE_ROOT);
			at_param().state_.set(widget::state::CLIP_PARENTS);
			at_param().state_.set(widget::state::AREA_ROOT);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(wd_.get_focus_widget() == this || wd_.get_focus_widget() == wd_.root_widget(this)) {
				focus_ = true;
			} else {
				focus_ = false;
			}

			if(get_param().parents_ && get_state(widget::state::AREA_ROOT)) {
				if(get_param().parents_->type() == get_type_id<widget_frame>()) {
					// 親になってるフレームを取得
					widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
					if(w) {
						bool resize = false;
						vtx::irect sr = w->get_draw_area();
						if(sr.size != get_rect().size) resize = true;
						at_rect() = sr;
					}
				}
			}

			// マウス位置のサービス
			auto back = param_.ms_level_;
			if(get_select()) {
				auto pos = get_param().in_point_;
				pos -= get_param().clip_.org;
				param_.ms_pos_ = pos;				
//				utils::format("%d, %d\n") % pos.x % pos.y;
				param_.ms_level_ = true;
			} else {
				param_.ms_level_ = false;
			}
			param_.ms_positive_ = !back &  param_.ms_level_;
			param_.ms_negative_ =  back & !param_.ms_level_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(!get_state(state::ENABLE)) {
				focus_ = false;
				return;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override
		{
			using namespace gl;
			core& core = core::get_instance();
			const vtx::spos& vsz = core.get_size();

			const widget::param& wp = get_param();

			if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {

				glPushMatrix();

				vtx::irect rect;
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					rect.org  = wp.rpos_;
					rect.size = wp.rect_.size;
				} else {
					rect.org.set(0);
					rect.size = wp.rect_.size;
				}

				float sx = core.get_dpi_scale();
				float sy = core.get_dpi_scale();
				vtx::irect clip_ = wp.clip_;
				glViewport(clip_.org.x * sx, vsz.y - clip_.org.y * sy - clip_.size.y * sy,
					clip_.size.x * sx, clip_.size.y * sy);
				fb_.setup_matrix(0, 0, clip_.size.x, clip_.size.y);
				fb_.draw();

				glPopMatrix();
			}
			glViewport(0, 0, vsz.x, vsz.y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override {
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override {
			return false;
		}
	};
}
