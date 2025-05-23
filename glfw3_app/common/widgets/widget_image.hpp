#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget Image クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "gl_fw/glutils.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include <boost/optional.hpp>

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_image クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_image : public widget {

		typedef widget_image value_type;
		typedef std::function< void(const vtx::irect& clip) > render_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_image パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			const img::i_img*		image_;			///< 画像を使う場合
			gl::mobj::handle		mobj_handle_;	///< モーション・オブジェクトのハンドル
			boost::optional<gl::mobj&>	mobj_;		///< モーションオブジェクトの参照
			vtx::fpos	offset_;					///< 描画オフセット
			vtx::fpos	scale_;						///< 描画スケール
			bool		linear_;					///< リニアフィルター

			render_func_type	render_func_;

			param(const img::i_img* image = nullptr) noexcept : image_(image),
				mobj_handle_(0), offset_(0.0f), scale_(1.0f), linear_(true),
				render_func_()
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_image(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), objh_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_image() { }


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
		const char* type_name() const override { return "image"; }


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

			using namespace img;

			if(param_.image_) {
				paint pa;
				const vtx::spos& size = get_rect().size;

				vtx::spos s;
				vtx::spos o;
				if(size.x <= 0) {
					o.x = 0;
					s.x = param_.image_->get_size().x;
				} else {
					o.x = (size.x - param_.image_->get_size().x) / 2;
					s.x = size.x;
				}
				if(size.y <= 0) {
					o.y = 0;
					s.y = param_.image_->get_size().y;
				} else {
					o.y = (size.y - param_.image_->get_size().y) / 2;
					s.y = size.y;
				}
				at_rect().size = s;
				pa.create(s, true);
				pa.fill(img::rgba8(0, 0));

				img::copy_to_rgba8(param_.image_, vtx::srect(vtx::spos(0), s), pa, o);

				objh_ = wd_.at_mobj().install(&pa);
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
				if(get_param().parents_->type() == get_type_id<widget_frame>() ||
				   get_param().parents_->type() == get_type_id<widget_null>()) {
					widget_frame* w = static_cast<widget_frame*>(at_param().parents_);
					at_rect() = w->get_draw_area();
				}
			}
		}


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
			const vtx::spos& siz = core.get_rect().size;
			const widget::param& wp = get_param();

			glPushMatrix();
			if(objh_) {
				if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {
					vtx::srect rect;
					if(wp.state_[widget::state::CLIP_PARENTS]) {
						draw_mobj(wd_, objh_, wp.clip_);
					} else {
						wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));
					}
				}
			} else if(param_.mobj_ && param_.mobj_handle_) {
				mobj& mo = *param_.mobj_;
				if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) {
//					vtx::srect rect;
					if(wp.state_[widget::state::CLIP_PARENTS]) {
						int sx = vsz.x / siz.x;
						int sy = vsz.y / siz.y;
						glViewport(wp.clip_.org.x * sx,
							vsz.y - wp.clip_.org.y * sy - wp.clip_.size.y * sy,
							wp.clip_.size.x * sx, wp.clip_.size.y * sy);
							mo.setup_matrix(wp.clip_.size.x, wp.clip_.size.y);
					}
					glScale(param_.scale_);
					mo.draw(param_.mobj_handle_, gl::mobj::attribute::normal, param_.offset_, param_.linear_);
				}
			}
			glPopMatrix();

			if(param_.render_func_ != nullptr) {
				glPushMatrix();
				float sc = core.get_dpi_scale();
				const widget::param& wp = get_param();
				float sx = sc;
				float sy = sc;
				glViewport(
					wp.clip_.org.x * sx, vsz.y - wp.clip_.org.y * sy - wp.clip_.size.y * sy - 1,
					wp.clip_.size.x * sx, wp.clip_.size.y * sy);
				wd_.at_mobj().setup_matrix(wp.clip_.size.x - 1, wp.clip_.size.y - 1);
				param_.render_func_(wp.clip_);
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
