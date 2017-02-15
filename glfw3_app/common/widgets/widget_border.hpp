#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget Border クラス @n
			水平、又は垂直の「境界線」を描画する @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/glutils.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_frame.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_border クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_border : public widget {

		typedef widget_border value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_border パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {

			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			/*!
				@brief	ボーダー・タイプ
			*/
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			enum class type {
				holizontal,   ///< 水平線
				vertical      ///< 垂直線
			}; 

			type	type_;	///< 描画タイプ

			int		pos_;	///< 位置
			int		size_;	///< サイズ
			int		width_;	///< ボーダーの幅
			int		ofs_;	///< オフセット

			widget::color_param	color_;

			param(type t) : type_(t), pos_(0), size_(0), width_(4), ofs_(0),
				color_(widget_director::default_border_color_) { }
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
		widget_border(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_border() { }


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
		const char* type_name() const override { return "border"; }


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
			// 標準的に固定
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_ROOT);
			at_param().state_.set(widget::state::AREA_ROOT);

			const widget::param& wp = get_param();
			if(param_.type_ == param::type::holizontal) {
				param_.ofs_ = wp.rect_.org.x;
				param_.pos_ = wp.rect_.org.y;
				param_.size_  = wp.rect_.size.x;
				param_.width_ = wp.rect_.size.y;
			} else if(param_.type_ == param::type::vertical) {
				param_.pos_ = wp.rect_.org.x;
				param_.ofs_ = wp.rect_.org.y;
				param_.width_ = wp.rect_.size.x;
				param_.size_  = wp.rect_.size.y;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(get_param().parents_ && get_state(widget::state::AREA_ROOT)) {
				if(get_param().parents_->type() == get_type_id<widget_frame>()) {
					widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
					w->get_draw_area(at_rect());
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
			const vtx::spos& siz = core.get_rect().size;
			const widget::param& wp = get_param();

			glPushMatrix();
			glDisable(GL_TEXTURE_2D);

			int sx = vsz.x / siz.x;
			int sy = vsz.y / siz.y;
			glViewport(wp.clip_.org.x * sx, vsz.y - wp.clip_.org.y * sy - wp.clip_.size.y * sy,
				wp.clip_.size.x * sx, wp.clip_.size.y * sy);
			wd_.at_mobj().setup_matrix(wp.clip_.size.x, wp.clip_.size.y);

			vtx::srect rect;
			vtx::spos ofs(0);
			if(param_.type_ == param::type::holizontal) {
				rect.org.x = param_.ofs_;
				rect.org.y = param_.pos_;
				rect.size.x = param_.size_ <= 0 ? (wp.rect_.size.x - param_.ofs_) : (param_.size_ - param_.ofs_); 
				rect.size.y = param_.width_ / 2;
				ofs.y = param_.width_ / 2;
			} else if(param_.type_ == param::type::vertical) {
				rect.org.x = param_.pos_;
				rect.org.y = param_.ofs_;
				rect.size.x = param_.width_ / 2;
				rect.size.y = param_.size_ <= 0 ? (wp.rect_.size.y - param_.ofs_) : (param_.size_ - param_.ofs_);
				ofs.x = param_.width_ / 2;
			}
			gl::glColor(param_.color_.fore_color_);
			gl::draw_filled_rectangle(rect);
			rect.org += ofs;
			gl::glColor(param_.color_.back_color_);
			gl::draw_filled_rectangle(rect);

			glPopMatrix();

			glViewport(0, 0, vsz.x, vsz.y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre) override
		{
			return true;
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
			return true;
		}
	};
}
