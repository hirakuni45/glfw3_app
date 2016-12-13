#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget View クラス @n
			カスタム描画・テンプレート
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/glutils.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_view クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_view : public widget {

		typedef widget_view value_type;

		typedef std::function< void() > update_func_type;
		typedef std::function< void(const vtx::spos& size) > render_func_type;
		typedef std::function< void() > service_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_view パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {

			update_func_type	update_func_;
			render_func_type	render_func_;
			service_func_type	service_func_;

			param() { }
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
		widget_view(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_view() { }


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
		const char* type_name() const override { return "view"; }


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
			if(param_.update_func_ != nullptr) param_.update_func_();
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

			int sx = vsz.x / siz.x;
			int sy = vsz.y / siz.y;
			glViewport(wp.clip_.org.x * sx, vsz.y - wp.clip_.org.y * sy - wp.clip_.size.y * sy,
				wp.clip_.size.x * sx, wp.clip_.size.y * sy);
			wd_.at_mobj().setup_matrix(wp.clip_.size.x, wp.clip_.size.y);

			if(param_.render_func_ != nullptr) param_.render_func_(wp.clip_.size);
			
			glPopMatrix();
			glViewport(0, 0, vsz.x, vsz.y);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(param_.service_func_ != nullptr) param_.service_func_();
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
