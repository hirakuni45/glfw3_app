#pragma once
//=========================================================================//
/*!	@file
	@brief	GUI Widget スライダー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <bitset>
#include "core/glcore.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_slider クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_slider : public widget {

		typedef widget_slider value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	選択関数型
			@param[in]	val		ハンドル位置（0.0 to 1.0）
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		typedef std::function<void (float val)> select_func_type;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_slider パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			widget::plate_param		plate_param_;	///< プレートパラメーター
			widget::color_param		color_param_;	///< カラーパラメーター
			widget::slider_param	slider_param_;	///< スライダーパラメーター

			const img::i_img*		base_image_;	///< ベース画像を使う場合
			const img::i_img*		hand_image_;	///< ハンドル画像を使う場合

			bool					hand_ctrl_;		///< ハンドル・コントロール
			bool					area_ctrl_;		///< エリア・コントロール（ハンドル外をクリックした時にハンドルが移動する）
			bool					scroll_ctrl_;	///< スクロール・コントロール（マウスのダイアル）
			bool					select_fin_;	///< 選択が完了した場合に呼び出す
			float					scroll_gain_;	///< スクロール・ゲイン

			select_func_type		select_func_;	///< 選択完了関数

			param(float pos = 0.0f,
				slider_param::direction dir = slider_param::direction::HOLIZONTAL) noexcept :
				plate_param_(),
				color_param_(widget_director::default_slider_color_),
				slider_param_(pos, dir),
				base_image_(0), hand_image_(0),
				hand_ctrl_(true), area_ctrl_(true), scroll_ctrl_(true), select_fin_(false),
				scroll_gain_(0.01f),
				select_func_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		float				ref_position_;
		vtx::ipos			ref_point_;

		vtx::spos			handle_offset_;

		gl::mobj::handle	base_h_;		///< ベース
		gl::mobj::handle	hand_h_;		///< ハンドル

		float               position_;

		void update_offset_() noexcept
		{
			const slider_param& sp = param_.slider_param_;
			if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
				handle_offset_.x = param_.plate_param_.frame_width_;
				auto ofs = wd_.at_mobj().get_size(hand_h_).y - wd_.at_mobj().get_size(base_h_).y;
				handle_offset_.y = -ofs / 2;
			} else if(sp.direction_ == slider_param::direction::VERTICAL) {
				auto ofs = wd_.at_mobj().get_size(hand_h_).x - wd_.at_mobj().get_size(base_h_).x;
				handle_offset_.x = -ofs / 2;
				handle_offset_.y = param_.plate_param_.frame_width_;
			}		
		}

		float get_ratio_(const vtx::spos& ref) noexcept
		{
			const slider_param& sp = param_.slider_param_;
			auto fw = param_.plate_param_.frame_width_;
			float ratio = param_.slider_param_.position_;
			if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
				short sz = get_rect().size.x - fw * 2;
				short ofs;
				if(param_.hand_image_) {
					ofs = wd_.at_mobj().get_size(hand_h_).x;
				} else {
					ofs = sz * sp.handle_ratio_;
				}
				ratio = static_cast<float>(ref.x) / static_cast<float>(sz - ofs);
			} else if(sp.direction_ == slider_param::direction::VERTICAL) {
				short sz = get_rect().size.y - fw * 2;
				short ofs;
				if(param_.hand_image_) {
					ofs = wd_.at_mobj().get_size(hand_h_).y;
				} else {
					ofs = sz * sp.handle_ratio_;
				}
				ratio = static_cast<float>(ref.y) / static_cast<float>(sz - ofs);
			}
			return ratio;
		}

		void update_position_() noexcept
		{
			const slider_param& sp = param_.slider_param_;
			const vtx::spos& size = get_rect().size;
			auto fw = param_.plate_param_.frame_width_;
			if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
				short sz = size.x - fw * 2;
				short ofs;
				if(param_.hand_image_) {
					ofs = wd_.at_mobj().get_size(hand_h_).x;
				} else {
					ofs = sz * sp.handle_ratio_;
				}
				handle_offset_.x = fw + sp.position_ * (sz - ofs);
			} else if(sp.direction_ == slider_param::direction::VERTICAL) {
				short sz = size.y - fw * 2;
				short ofs;
				if(param_.hand_image_) {
					ofs = wd_.at_mobj().get_size(hand_h_).y;
				} else {
					ofs = sz * sp.handle_ratio_;
				}
				handle_offset_.y = fw + sp.position_ * (sz - ofs);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_slider(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p), ref_position_(0.0f),
			handle_offset_(0), base_h_(0), hand_h_(0),
			position_(-1.0f)
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
		type_id type() const noexcept override { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const noexcept override { return "slider"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const noexcept override { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const noexcept { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() noexcept { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダーパラメーター取得
			@return スライダーパラメーター
		*/
		//-----------------------------------------------------------------//
		const slider_param& get_slider_param() const noexcept { return param_.slider_param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダーパラメーター参照
			@return スライダーパラメーター
		*/
		//-----------------------------------------------------------------//
		slider_param& at_slider_param() noexcept { return param_.slider_param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダー位置の取得
			@return スライダー位置
		*/
		//-----------------------------------------------------------------//
		float get_position() const noexcept { return param_.slider_param_.position_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スライダー位置の参照
			@return スライダー位置
		*/
		//-----------------------------------------------------------------//
		float& at_position() noexcept { return param_.slider_param_.position_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept override
		{
			// 標準的設定
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);

			auto size = get_rect().size;

			img::paint pa;
			{
				const widget::plate_param& pp = param_.plate_param_;
				if(pp.resizeble_) {
					auto bsz = size;
					if(bsz.x >= pp.grid_.x * 3) bsz.x = pp.grid_.x * 3;
					if(bsz.y >= pp.grid_.y * 3) bsz.y = pp.grid_.y * 3;
					create_round_frame(pa, param_.plate_param_, param_.color_param_, bsz);
					base_h_ = wd_.at_mobj().install(&pa, bsz, param_.plate_param_.grid_);
				} else {
					create_round_frame(pa, param_.plate_param_, param_.color_param_, size);
					base_h_ = wd_.at_mobj().install(&pa);
				}
			}

			const slider_param& sp = param_.slider_param_;
			if(param_.hand_image_) {
				const vtx::spos& size = param_.hand_image_->get_size();
				create_image_base(param_.hand_image_, size, pa);
			} else {
				auto wf = param_.plate_param_.frame_width_;
				auto hr = param_.slider_param_.handle_ratio_;
				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
					size.x = static_cast<float>(size.x - wf * 2) * hr;
					size.y -= wf * 2;
				} else if(sp.direction_ == slider_param::direction::VERTICAL) {
					size.x -= wf * 2;
					size.y = static_cast<float>(size.y - wf * 2) * hr;
				}
				plate_param pp = param_.plate_param_;
				if(pp.round_radius_ > param_.plate_param_.frame_width_) {
					pp.round_radius_ -= param_.plate_param_.frame_width_;
				}
				pp.frame_width_ = 0;
				color_param cp = param_.color_param_;
				cp.swap_color();
				cp.back_color_ *= 0.75f;
				create_round_frame(pa, pp, cp, size);
				pa.set_fore_color(cp.fore_color_);
				pa.alpha_blend();
				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
					pa.fill_rect(size.x / 2 - 2, 3, 4, size.y - 3 * 2, true);
				} else if(sp.direction_ == slider_param::direction::VERTICAL) {
					pa.fill_rect(3, size.y / 2 - 2, size.x - 3 * 2, 4, true);
				}
			}
			hand_h_ = wd_.at_mobj().install(&pa);

			update_offset_();
			update_position_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() noexcept override
		{
			update_offset_();

			const slider_param& sp = param_.slider_param_;
			if(get_focus() && param_.hand_ctrl_) {
				if(get_select()) {
					if(get_select_in()) {
						ref_position_ = sp.position_;
						ref_point_ = get_param().in_point_;
					}
					auto ref = get_param().in_point_ - ref_point_;
					auto ratio = get_ratio_(ref);
					ratio += ref_position_;
					if(ratio < 0.0f) ratio = 0.0f;
					else if(ratio > 1.0f) ratio = 1.0f;
					if(sp.grid_ > 0.0f) {
						short step = ratio / (sp.grid_ * 0.5f);
						ratio = ((step + 1) / 2) * sp.grid_;
					}
					param_.slider_param_.position_ = ratio;
				} else {
					if(param_.area_ctrl_ && get_select_out()) {  // クリックした位置に移動
						auto d = ref_point_ - get_param().in_point_;
						if((std::abs(d.x) + std::abs(d.y)) <= 1) {
							auto fw = param_.plate_param_.frame_width_;
							auto ratio = param_.slider_param_.position_;
							if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
								auto hw = wd_.at_mobj().get_size(hand_h_).x;
								auto sz = get_rect().size.x;
								ratio = static_cast<float>(get_param().in_point_.x - fw - hw / 2) / static_cast<float>(sz - fw * 2 - hw);
							} else if(sp.direction_ == slider_param::direction::VERTICAL) {
								auto hw = wd_.at_mobj().get_size(hand_h_).y;
								auto sz = get_rect().size.y;
								ratio = static_cast<float>(get_param().in_point_.y - fw - hw / 2) / static_cast<float>(sz - fw * 2 - hw);
							}
							if(ratio < 0.0f) ratio = 0.0f;
							else if(ratio > 1.0f) ratio = 1.0f;
							param_.slider_param_.position_ = ratio;
						}
					}
					if(param_.scroll_ctrl_) {  // スクロールホイール制御
						const vtx::spos& scr = wd_.get_scroll();
						float ratio = param_.slider_param_.position_;
						if(scr.y != 0) {
							if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
								ratio += static_cast<float>(scr.y) * -param_.scroll_gain_;
							} else if(sp.direction_ == slider_param::direction::VERTICAL) {
								ratio += static_cast<float>(scr.y) *  param_.scroll_gain_;
							}
							if(ratio < 0.0f) ratio = 0.0f;
							else if(ratio > 1.0f) ratio = 1.0f;
							if(sp.grid_ > 0.0f) {
								short step = ratio / (sp.grid_ * 0.5f);
								ratio = ((step + 1) / 2) * sp.grid_;
							}
							param_.slider_param_.position_ = ratio;
						}
					}
				}
			}
			if(get_focus()) {  // キーボードによる制御
				gl::core& core = gl::core::get_instance();
				const gl::device& dev = core.get_device();
				float& pos = param_.slider_param_.position_;
				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
					if(dev.get_positive(gl::device::key::LEFT)) {
						pos -= param_.scroll_gain_;
					}
					if(dev.get_positive(gl::device::key::RIGHT)) {
						pos += param_.scroll_gain_;
					}
				} else if(sp.direction_ == slider_param::direction::VERTICAL) {
					if(dev.get_positive(gl::device::key::UP)) {
						pos -= param_.scroll_gain_;
					}
					if(dev.get_positive(gl::device::key::DOWN)) {
						pos += param_.scroll_gain_;
					}
				}
				if(pos < 0.0f) {
					pos = 0.0f;
				}
				if(pos > 1.0f) {
					pos = 1.0f;
				}
			}

			update_position_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept override
		{
			if(!get_state(state::ENABLE)) {
				return;
			}

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
					if(param_.select_func_ != nullptr) {
						param_.select_func_(param_.slider_param_.position_);
					}
					position_ = param_.slider_param_.position_;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() noexcept override
		{
			if(base_h_ == 0) return;

			using namespace gl;
			core& core = core::get_instance();

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(base_h_, get_param().rect_.size);
			}

			const vtx::spos& vsz = core.get_size();

			const widget::param& wp = get_param();

			if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {
				glPushMatrix();
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					draw_mobj(wd_, base_h_, wp.clip_);
				} else {
					wd_.at_mobj().draw(base_h_, gl::mobj::attribute::normal, vtx::spos(0));
				}
				wd_.at_mobj().draw(hand_h_, gl::mobj::attribute::normal, handle_offset_);
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
		bool save(sys::preference& pre) noexcept override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.put_real(path + "/level", param_.slider_param_.position_)) ++err;
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) noexcept override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.get_real(path + "/level", param_.slider_param_.position_)) ++err;
			return err == 0;
		}
	};
}
