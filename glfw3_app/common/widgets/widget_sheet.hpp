#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_sheet クラス @n
			・プロパティーシートのような機能を提供する。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_sheet クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_sheet : public widget {

		typedef widget_sheet value_type;

		typedef std::function< void (uint32_t sheet_index, uint32_t select_id) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_sheet パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;	///< 平面描画パラメーター
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			text_param		text_param_;	///< テキスト描画のパラメータ
			shift_param		shift_param_;	///< テキスト描画のパラメータ

			uint32_t		index_;			///< シート・インデックス
			uint32_t		id_;			///< セレクト ID （押された回数）

			select_func_type	select_func_;	///< セレクト関数

			struct sheet_t {
				std::string	title_;
				widget*		widget_;
				sheet_t(const std::string& t, widget* w) : title_(t), widget_(w) { }
			};
			typedef std::vector<sheet_t> SHEETS;
			SHEETS			sheets_;

			param() :
				plate_param_(), color_param_(widget_director::default_sheet_color_),
				text_param_("", img::rgba8(255, 255), img::rgba8(0, 255)),
				shift_param_(),
				index_(0), id_(0),
				select_func_(nullptr),
				sheets_()
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

		uint32_t			id_;

		bool				init_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	wd	ディレクター（参照）
			@param[in]	wp	共通パラメーター
			@param[in]	p	個別パラメーター
		*/
		//-----------------------------------------------------------------//
		widget_sheet(widget_director& wd, const widget::param& wp, const param& p) :
			widget(wp), wd_(wd), param_(p), objh_(0), id_(0), init_(false)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_sheet() { }


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
		const char* type_name() const override { return "sheet"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const override { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			// シート・ベース、標準設定
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);
			at_param().action_.set(widget::action::SELECT_SCALE, false);

			using namespace img;
			param_.plate_param_.frame_width_ = 2;
			param_.plate_param_.caption_width_ = 26;
			param_.plate_param_.resizeble_ = false;
			param_.text_param_.shadow_offset_.set(0);	// 通常「影」は付けない。
			param_.text_param_.fore_color_.set(250, 250, 250);
			at_param().resize_min_ = param_.plate_param_.grid_ * 3;

			// フレームの生成
			objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

			// ペアレンツの設定
			if(param_.index_ < param_.sheets_.size()) {
				param_.text_param_.set_text(param_.sheets_[param_.index_].title_);
			}
			for(auto& s : param_.sheets_) {
				if(s.widget_ != nullptr) {
					s.widget_->at_param().parents_ = this;
				}				
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			auto x = get_param().in_point_.x;
			auto y = get_param().in_point_.y;
			if(get_selected() && y >= 0 && y <= param_.plate_param_.caption_width_) {
				auto xs = get_rect().size.x;
				auto back = param_.index_;
				if(x < (xs / 3)) {  // back
					if(param_.index_ > 0) {
						--param_.index_;
					} else {
						param_.index_ = param_.sheets_.size() - 1;
					} 
				} else if(x >= (xs * 2 / 3)) {  // next
					if(param_.index_ < (param_.sheets_.size() - 1)) {
						++param_.index_;
					} else {
						param_.index_ = 0;
					}
				} else {  // select
				}
				if(back != param_.index_) {
					param_.text_param_.set_text(param_.sheets_[param_.index_].title_);
					++id_;
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
				return;
			}

			if(!init_ || param_.id_ != id_) {
				init_ = true;

				for(uint32_t i = 0; i < param_.sheets_.size(); ++i) {
					widget* w = param_.sheets_[i].widget_;
					if(w != nullptr) {
						wd_.enable(w, i == param_.index_, true);
						wd_.top_widget(w);
					}
				}

				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.index_, id_);
				}
				param_.id_ = id_;
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

			wd_.at_mobj().resize(objh_, get_param().rect_.size);
			glEnable(GL_TEXTURE_2D);
			wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));

			shift_text_render(get_param(), param_.text_param_, param_.plate_param_);

			// チップの描画
			using namespace gl;
			core& core = core::get_instance();
			const vtx::spos& siz = core.get_rect().size;
			wd_.at_mobj().setup_matrix(siz.x, siz.y);
			wd_.set_TSC();

			gl::mobj::handle uph = wd_.get_share_image().left_box_;
			gl::mobj::handle dnh = wd_.get_share_image().right_box_;
			const vtx::spos& bs = wd_.at_mobj().get_size(uph);
			const vtx::spos& size = get_rect().size;
			int wf = param_.plate_param_.frame_width_;
			int spc_x = 2;
			int spc_y = param_.plate_param_.caption_width_ - wf - bs.y;
			if(spc_y < 0) spc_y = 0;
			else spc_y /= 2;
			vtx::spos pos(size.x - bs.x - wf - spc_x, spc_y);
			wd_.at_mobj().draw(dnh, gl::mobj::attribute::normal, pos);
			pos.x = wf + spc_x;
			wd_.at_mobj().draw(uph, gl::mobj::attribute::normal, pos);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			int v = param_.index_;
			if(!pre.put_integer(path + "/index", v)) ++err;
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre) override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			int v;
			if(!pre.get_integer(path + "/index", v)) ++err;
			else {
				param_.index_ = v;
			}
			return err == 0;
		}
	};
}
