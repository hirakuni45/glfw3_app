#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget プログレス・バー・クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <bitset>
#include "core/glcore.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_progress クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_progress : public widget {

		typedef widget_progress value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_progress パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			widget::plate_param		plate_param_;	///< プレートパラメーター
			widget::color_param		color_param_;	///< カラーパラメーター

			float					ratio_;

			const img::i_img*		base_image_;	///< ベース画像を使う場合
			const img::i_img*		hand_image_;	///< ハンドル画像を使う場合

			param(float ratio = 0.0f) :
				plate_param_(),
				color_param_(widget_director::default_progress_color_),
				ratio_(ratio),
				base_image_(nullptr), hand_image_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	base_h_;	///< ベース
		gl::mobj::handle	hand_h_;	///< ハンドル

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_progress(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p),
			base_h_(0), hand_h_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_progress() { }


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
		const char* type_name() const override { return "progress"; }


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
			@brief	レシオ（進捗率）の設定
			@param[in]	ratio	レシオ（進捗率）
		*/
		//-----------------------------------------------------------------//
		void set_ratio(float ratio) { param_.ratio_ = ratio; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override {
			// 標準的設定
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);

			vtx::spos size = get_rect().size;
			img::paint pa;
			{
				const widget::plate_param& pp = param_.plate_param_;
				if(pp.resizeble_) {
					vtx::spos bsz = size;
					if(bsz.x >= pp.grid_.x * 3) bsz.x = pp.grid_.x * 3;
					if(bsz.y >= pp.grid_.y * 3) bsz.y = pp.grid_.y * 3;
					create_round_frame(pa, param_.plate_param_, param_.color_param_, bsz);
					base_h_ = wd_.at_mobj().install(&pa, bsz, param_.plate_param_.grid_);
				} else {
					create_round_frame(pa, param_.plate_param_, param_.color_param_, size);
					base_h_ = wd_.at_mobj().install(&pa);
				}
			}

///			const slider_param& sp = param_.slider_param_;
			if(param_.hand_image_) {
				const vtx::spos& size = param_.hand_image_->get_size();
				create_image_base(param_.hand_image_, size, pa);
			} else {
				short wf = param_.plate_param_.frame_width_;
///				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
					size.x = (size.x - wf * 2);  // param_.slider_param_.handle_ratio_;
					size.y -= wf * 2;
///				} else if(sp.direction_ == slider_param::direction::VERTICAL) {
///					size.x -= wf * 2;
///					size.y = (size.y - wf * 2) * param_.slider_param_.handle_ratio_;
///				}
				plate_param pp = param_.plate_param_;
				pp.round_radius_ -= param_.plate_param_.frame_width_;
				pp.frame_width_ = 0;
				color_param cp = param_.color_param_;
				cp.swap_color();
				cp.back_color_ *= 0.75f;
				create_round_frame(pa, pp, cp, size);
				pa.set_fore_color(cp.fore_color_);
				pa.alpha_blend();
///				if(sp.direction_ == slider_param::direction::HOLIZONTAL) {
					pa.line(size.x / 2, 3, size.x / 2, size.y - 3 * 2);
///				} else if(sp.direction_ == slider_param::direction::VERTICAL) {
///				}
			}
			hand_h_ = wd_.at_mobj().install(&pa);
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
		void render() override {
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
				vtx::srect rect;
				if(wp.state_[widget::state::CLIP_PARENTS]) {
					draw_mobj(wd_, base_h_, wp.clip_);
				} else {
					wd_.at_mobj().draw(base_h_, gl::mobj::attribute::normal, vtx::spos(0));
				}

				auto fw = param_.plate_param_.frame_width_;
				glTranslatei(fw, fw, 0);
				glScalef(param_.ratio_, 1.0f, 1.0f);
				wd_.at_mobj().draw(hand_h_, gl::mobj::attribute::normal, vtx::spos(0));
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
			if(!pre.put_real(path + "/ratio", param_.ratio_)) ++err;
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
			if(!pre.get_real(path + "/ratio", param_.ratio_)) ++err;
			return err == 0;
		}
	};
}
