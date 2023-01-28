#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_frame.hpp"
#include "utils/terminal.hpp"
#include "core/glcore.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI terminal クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_terminal : public widget {

		typedef widget_terminal value_type;

		typedef std::function< void(uint32_t ch) > input_func_type;
		typedef std::function< void(const utils::lstring& line) > enter_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_terminal パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			std::string		font_;			///< ターミナル描画フォント
			uint32_t		font_width_;	///< フォント幅（初期化で設定される）
			uint32_t		font_height_;	///< フォント高
			uint32_t		height_;		///< 行の高さ

			bool			echo_;			///< キー入力とエコー
			bool			auto_fit_;	   	///< 等幅フォントに対するフレームの最適化

			input_func_type input_func_;	///< １文字入力毎に呼ぶ関数
			enter_func_type enter_func_;	///< 「Enter」時に呼ぶ関数

			param() : font_("Inconsolata"), font_width_(0), font_height_(18), height_(20),
				echo_(true), auto_fit_(true) { }
		};

	private:

		widget_director&	wd_;

		param				param_;

		utils::terminal		terminal_;
		uint32_t			interval_;

		bool				focus_;

		vtx::ipos			scroll_ofs_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_terminal(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), terminal_(), interval_(0),
			focus_(false), scroll_ofs_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_terminal() { }


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
		const char* type_name() const override { return "terminal"; }


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
			@brief	ターミナル・インスタンスへの参照
			@return ターミナル・インスタンス
		*/
		//-----------------------------------------------------------------//
		utils::terminal& at_terminal() { return terminal_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	１文字出力
			@param[in]	wch	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t wch) {
			terminal_.output(wch);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストの出力
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void output(const std::string& text) {
			if(text.empty()) return;

			auto ls = utils::utf8_to_utf32(text);
			for(auto ch : ls) {
				output(ch);
			}
		}


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

			// スクロール処理、マウスホイール
			if(get_focus()) {
				const vtx::spos& scr = wd_.get_scroll();
				scroll_ofs_ += scr;
				if(scroll_ofs_.y < 0) scroll_ofs_.y = 0;
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
//							if(ss.x < w->get_param().resize_min_.x) {
//								ss.x = w->get_param().resize_min_.x / param_.font_width_;
//								ss.x *= param_.font_width_;
//							}
							ss.y *= param_.height_;
//							if(ss.y < w->get_param().resize_min_.y) {
//								ss.y = w->get_param().resize_min_.y / param_.height_;
//								ss.y *= param_.height_;
//							}
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
				if(param_.echo_) {
					auto s = wd_.at_keyboard().input();
					terminal_.output(s);
					if(!s.empty()) {
						if(param_.input_func_ != nullptr) {
							for(auto ch : s) {
								param_.input_func_(ch);
							}
						}
						if(param_.enter_func_ != nullptr && terminal_.get_last_char() == 0x0D) {
							param_.enter_func_(terminal_.get_last_text32());
						}
					}
				}
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
//			const vtx::spos& siz = core.get_rect().size;
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

//				float sx = vsz.x / siz.x;
				float sx = core.get_dpi_scale();
//				float sy = vsz.y / siz.y;
				float sy = core.get_dpi_scale();
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
				auto npy = ofs.y - scroll_ofs_.y;
				if(npy < 0) {
					npy = 0;
					scroll_ofs_.y = ofs.y;
				} else if(npy > ofs.y) {
					npy = ofs.y;
				}
				ofs.y = npy;
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
						auto cha = t.cha_;
						if(cha < 0x20) cha = 0x3F;  // 制御コードは DEL-char として扱う
						if(cha > 0x7f) {
							fonts.pop_font_face();
						}
						int fw = fonts.get_width(cha);
						vtx::irect br(chs, vtx::ipos(fw, param_.height_));
						fonts.draw_back(br);
						chs.x += fonts.draw(chs, cha);
						fonts.swap_color(false);
						if(cha > 0x7f) {
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


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ターミナル出力ファンクタ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class _>
	class term_chaout_t {
		static widget*	output_;

		std::string		buff_;

		char*		out_;
		uint16_t	len_;
		uint16_t	pos_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		term_chaout_t(char* out = nullptr, uint16_t len = 0) : out_(out), len_(len), pos_(0) { } 


		//-----------------------------------------------------------------//
		/*!
			@brief	ターミナル出力の設定
		*/
		//-----------------------------------------------------------------//
		static void set_output(widget* w) { output_ = w; }


		//-----------------------------------------------------------------//
		/*!
			@brief	format 出力、標準オペレーターの実装
			@param[in]	ch	出力キャラクター
		*/
		//-----------------------------------------------------------------//
		void operator() (char ch)
		{
			if(output_ != nullptr && output_->type() == get_type_id<widget_terminal>()) {

				buff_ += ch;

				widget_terminal* term = static_cast<widget_terminal*>(output_);

				if(ch == '\n' || ch == 0) {
					term->output(buff_);
					buff_.clear();
				} else if(buff_.size() >= 64) {
					term->output(buff_);
					buff_.clear();
				}
			}
		}
	};
	template <class _> widget* term_chaout_t<_>::output_ = nullptr;

	typedef term_chaout_t<void> term_chaout;

	typedef utils::basic_format<term_chaout> format;
}
