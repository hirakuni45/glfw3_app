#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget Text クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_text クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_text : public widget {

		typedef widget_text value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_text パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			text_param	text_param_;
			param(const std::string& text = "") : text_param_(text,
				img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
					vtx::placement::vertical::TOP))
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_text(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_text() { }


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
		const char* type_name() const override { return "text"; }


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
			@brief	テキストを設定
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text) {
			param_.text_param_.set_text(text);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストを取得
			@return	テキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_text() const {
			std::string s;
			utils::utf32_to_utf8(param_.text_param_.text_, s);
			return s;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			// 標準的に固定
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_ROOT);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override { }


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
		void render() override
		{
			using namespace gl;
			core& core = core::get_instance();

			const vtx::spos& vsz = core.get_size();
			const widget::param& wp = get_param();

			if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {

				auto clip = wp.clip_;
				glPushMatrix();

				vtx::irect rect;
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					vtx::ipos o(0);
					vtx::ipos w(0);
					widget_frame* par = static_cast<widget_frame*>(wp.parents_);
					if(par != nullptr && par->type() == get_type_id<widget_frame>()) {
						const auto& plate = par->get_local_param().plate_param_; 
						o.x = plate.frame_width_;
						o.y = plate.frame_width_ + plate.caption_width_;
						w.x = plate.frame_width_ * 2;
						w.y = o.y + plate.frame_width_ + 4;
						clip.size.y -= plate.frame_width_;
					}
					rect.org  = wp.rpos_ + o;
					rect.size = wp.rect_.size - w;
				} else {
					rect.org.set(0);
					rect.size = wp.rect_.size;
				}

				widget::text_param tpr = param_.text_param_;
				const img::rgbaf& cf = wd_.get_color();
				tpr.fore_color_ *= cf.r;
				tpr.fore_color_.alpha_scale(cf.a);
				tpr.shadow_color_ *= cf.r;
				tpr.shadow_color_.alpha_scale(cf.a);
				draw_text(tpr, rect, clip);

				core.at_fonts().restore_matrix();

				glPopMatrix();
				glViewport(0, 0, vsz.x, vsz.y);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override {
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);
			int err = 0;
			if(!pre.put_text(path + "/text", param_.text_param_.get_text())) ++err;
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override {
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			std::string s;
			if(!pre.get_text(path + "/text", s)) {
				++err;
			} else {
				param_.text_param_.set_text(s);
			}
			return err == 0;
		}
	};

}
