#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_check クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

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
		enum class style {
			CHECKED,		///< チェックド・ボックス（中心点）
			MINUS_PLUS,		///<「－」、「＋」
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_check パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			style		type_;				///< 描画表現
			text_param	text_param_;		///< テキスト描画のパラメータ
			float		gray_text_gain_;	///< 不許可時のグレースケールゲイン
			bool		disable_gray_text_;	///< 不許可時、文字をグレースケールする場合
			bool		draw_box_;			///< ボックスの表示を行わない場合 false
			bool		check_;				///< 許可、不許可の状態
			bool		init_func_;			///< 初期化時にもセレクト関数を呼ぶ場合

			select_func_type	select_func_;	///< セレクト関数

			param(const std::string& text = "", bool check = false) :
				type_(style::CHECKED),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				gray_text_gain_(0.65f), disable_gray_text_(true), draw_box_(true),
				check_(check), init_func_(true), select_func_(nullptr)
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
			obj_state_(false), ena_h_(0), dis_h_(0), check_(p.check_)
		{ }


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
		void initialize() override {
			// ボタンは標準的に固定
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);

			if(param_.type_ == style::CHECKED) {
				dis_h_ = wd_.get_share_image().un_check_;
				ena_h_ = wd_.get_share_image().to_check_;
			} else if(param_.type_ == style::MINUS_PLUS) {
				dis_h_ = wd_.get_share_image().minus_box_;
				ena_h_ = wd_.get_share_image().plus_box_;
			}


		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(get_select()) {
				obj_state_ = !param_.check_;
			} else if(get_selected()) {
				param_.check_ = !param_.check_;
				obj_state_ = param_.check_;
			} else {
				obj_state_ = param_.check_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			bool dofunc = param_.init_func_;
			if(get_state(state::ENABLE)) {
				if(check_ != param_.check_) {
					dofunc = true;
				}
			}

			if(dofunc) {
				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.check_);
				}
				check_ = param_.check_;
				param_.init_func_ = false;
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
				gl::mobj::handle h;
				if(obj_state_) h = ena_h_;
				else h = dis_h_;

				glPushMatrix();

				vtx::irect rect;
				const vtx::spos& mosz = wd_.at_mobj().get_size(h);
				vtx::ipos ofs(0, (wp.rect_.size.y - mosz.y) / 2);
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					if(param_.draw_box_) draw_mobj(wd_, h, wp.clip_, ofs + wp.rpos_);
					rect.org  = wp.rpos_;
					rect.size = wp.rect_.size;
				} else {
					if(param_.draw_box_) wd_.at_mobj().draw(h, gl::mobj::attribute::normal, ofs);
					rect.org.set(0);
					rect.size = wp.rect_.size;
				}

				rect.org.x += mosz.x + 4;
				img::rgba8 fc = param_.text_param_.fore_color_;
				if(param_.disable_gray_text_ && !obj_state_) {
					param_.text_param_.fore_color_ *= param_.gray_text_gain_;
				}
				widget::text_param tmp = param_.text_param_;
				const img::rgbaf& cf = wd_.get_color();
				tmp.fore_color_ *= cf.r;
				tmp.fore_color_.alpha_scale(cf.a);
				tmp.shadow_color_ *= cf.r;
				tmp.shadow_color_.alpha_scale(cf.a);

				draw_text(tmp, rect, wp.clip_);

				core.at_fonts().restore_matrix();

				param_.text_param_.fore_color_ = fc;

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
			if(!pre.put_boolean(path + "/state", param_.check_)) ++err;
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
			if(!pre.get_boolean(path + "/state", param_.check_)) ++err;
			check_ = param_.check_;
			return err == 0;
		}
	};
}
