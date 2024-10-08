#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget エディター（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "utils/terminal.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI editor クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_editor : public widget {

		typedef widget_editor value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_editor パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			std::string		font_;			///< ターミナル描画フォント
			uint32_t		font_width_;	///< フォント幅（初期化で設定される）
			uint32_t		font_height_;	///< フォント高
			uint32_t		height_;		///< 行の高さ

			bool			auto_fit_;	   	///< 等幅フォントに対するフレームの最適化

			param() : font_("Inconsolata"), font_width_(0), font_height_(18), height_(18),
				auto_fit_(true) { }
		};

		widget_director&	wd_;

		param				param_;

		utils::terminal		terminal_;
		uint32_t			interval_;

		bool				focus_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_editor(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), terminal_(), interval_(0),
			focus_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_editor() { }


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
		const char* type_name() const override { return "editor"; }


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
			@brief	１文字出力
			@param[in]	wch	文字
		*/
		//-----------------------------------------------------------------//
		// void output(uint32_t wch);


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストの出力
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		// void output(const std::string& text);


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
			at_param().state_.set(widget::state::RESIZE_ROOT);
			at_param().state_.set(widget::state::CLIP_PARENTS);
			at_param().state_.set(widget::state::AREA_ROOT);

			using namespace gl;
			core& core = core::get_instance();
			fonts& fonts = core.at_fonts();

			fonts.push_font_face();
			fonts.set_font_type(param_.font_);
			fonts.set_font_size(param_.font_height_);
			fonts.enable_proportional(false);
			fonts.set_spaceing(0);
			param_.font_width_ = fonts.get_width(' ');
			fonts.pop_font_face();
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
						if(param_.auto_fit_) {
							vtx::ipos ss(sr.size.x / param_.font_width_,
										 sr.size.y / param_.height_);
							ss.x *= param_.font_width_;
							w->set_draw_area(ss);
							sr = w->get_draw_area();
						}
						if(sr.size != get_rect().size) resize = true;
						at_rect() = sr;
						if(resize) {

						}
					}
				}
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
				focus_ = false;
				return;
			}

			if(focus_) {
///				if(param_.echo_) {
					terminal_.output(wd_.at_keyboard().input());
///				}
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
			const vtx::spos& siz = core.get_rect().size;
			gl::fonts& fonts = core.at_fonts();

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

				fonts.push_font_face();
				fonts.set_font_type(param_.font_);
				fonts.set_font_size(param_.font_height_);

				vtx::irect clip_ = wp.clip_;

				int sx = vsz.x / siz.x;
				int sy = vsz.y / siz.y;
				glViewport(clip_.org.x * sx, vsz.y - clip_.org.y * sy - clip_.size.y * sy,
					clip_.size.x * sx, clip_.size.y * sy);
				fonts.setup_matrix(clip_.size.x, clip_.size.y);

				fonts.enable_center(false);
				fonts.enable_proportional(false);

				const img::rgbaf& cf = wd_.get_color();
				vtx::ipos limit(clip_.size.x / param_.font_width_, clip_.size.y / param_.height_);
				vtx::ipos chs(rect.org);
				auto ln = terminal_.get_line_num();
				vtx::ipos ofs(0);
				if(ln > limit.y) ofs.y = ln - limit.y;
				vtx::ipos pos;
				for(pos.y = 0; pos.y < limit.y; ++pos.y) {
					for(pos.x = 0; pos.x < limit.x; ++pos.x) {
						const auto& t = terminal_.get_char(pos + ofs);
						img::rgba8 fc = t.fc_;
						fc *= cf.r;
						fc.alpha_scale(cf.a);
						fonts.set_fore_color(fc);
						img::rgba8 bc = t.bc_;
						bc *= cf.r;
						bc.alpha_scale(cf.a);
						fonts.set_back_color(bc);
						if(focus_ && (pos + ofs) == terminal_.get_cursor()) {
							if((interval_ % 40) < 20) {
								fonts.swap_color();
							}
						}
						if(t.cha_ > 0x7f) {
							fonts.pop_font_face();
						}
						int fw = fonts.get_width(t.cha_);
						vtx::irect br(chs, vtx::ipos(fw, param_.height_));
						fonts.draw_back(br);
						chs.x += fonts.draw(chs, t.cha_);
						fonts.swap_color(false);
						if(t.cha_ > 0x7f) {
							fonts.push_font_face();
							fonts.set_font_type(param_.font_);
						}
					}
					chs.y += param_.height_;
					chs.x = rect.org.x;
				}
				++interval_;

				fonts.restore_matrix();
				fonts.pop_font_face();
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
