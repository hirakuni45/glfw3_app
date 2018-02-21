#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_frame クラス @n
			・ベースとなるウィンドウを描画する。 @n
			・描画領域の管理など
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_frame クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_frame : public widget {

		typedef widget_frame value_type;

		typedef std::function< void() > update_func_type;
		typedef std::function< void() > render_func_type;
		typedef std::function< void() > service_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_frame パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;	///< 平面描画パラメーター
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			text_param		text_param_;	///< テキスト描画のパラメータ
			shift_param		shift_param_;	///< テキスト描画のパラメータ

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_frame_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255)),
				shift_param_()
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
		widget_frame(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p), objh_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_frame() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const { return "frame"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const { return false; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(rw)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	有効・無効の設定
			@param[in]	f	無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { wd_.enable(this, f, true); }


		//-----------------------------------------------------------------//
		/*!
			@brief	描画領域の設定
			@param[in]	size	描画領域
		*/
		//-----------------------------------------------------------------//
		void set_draw_area(const vtx::ipos& size) {
			short ofs = param_.plate_param_.frame_width_;
			at_rect().size.x = size.x + ofs * 2;
			at_rect().size.y = size.y + ofs * 2 + param_.plate_param_.caption_width_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	描画領域の取得
			@param[out]	area	描画領域の参照
		*/
		//-----------------------------------------------------------------//
		vtx::irect get_draw_area() const {
			vtx::irect area;
			short ofs = param_.plate_param_.frame_width_;
			area.org.set(ofs, ofs + param_.plate_param_.caption_width_);
			area.size.x = get_rect().size.x - ofs * 2;
			area.size.y = get_rect().size.y - ofs * 2 - param_.plate_param_.caption_width_;
			return area;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			// 標準的設定（自由な大きさの変更）
			at_param().state_.set(widget::state::SIZE_LOCK, false);
			at_param().state_.set(widget::state::RESIZE_H_ENABLE);
			at_param().state_.set(widget::state::RESIZE_V_ENABLE);
			at_param().state_.set(widget::state::RESIZE_EDGE_ENABLE);
			at_param().state_.set(widget::state::MOVE_TOP);
			at_param().state_.set(widget::state::RESIZE_TOP);

			param_.plate_param_.resizeble_ = true;
			param_.text_param_.shadow_offset_.set(0);	// 通常「影」は付けない。
			param_.text_param_.fore_color_.set(250, 250, 250);
			at_param().resize_min_ = param_.plate_param_.grid_ * 3;

			// フレームの生成
			objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			param_.shift_param_.size_ = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
			shift_text_update(get_param(), param_.text_param_, param_.shift_param_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render()
		{
			if(objh_ == 0) return;

			wd_.at_mobj().resize(objh_, get_param().rect_.size);
			glEnable(GL_TEXTURE_2D);
			wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, vtx::spos(0));

			shift_text_render(wd_, get_param(), param_.text_param_, param_.plate_param_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			if(!pre.put_position(path + "/locate",  vtx::ipos(get_rect().org))) ++err;
			if(!pre.put_position(path + "/size", vtx::ipos(get_rect().size))) ++err;

			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@param[in]	w_ena	幅を無視する場合「false」
			@param[in]	h_ena	縦を無視する場合「false」
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre, bool w_ena, bool h_ena)
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			vtx::ipos p;
			if(pre.get_position(path + "/locate", p)) {
				at_rect().org = p;
			} else {
				++err;
			}
			// サイズ、ロックの場合、サイズを上書きしない
			if(!get_state(widget::state::SIZE_LOCK)) {
				if(pre.get_position(path + "/size", p)) {
					if(w_ena) at_rect().size.x = p.x;
					if(h_ena) at_rect().size.y = p.y;
				} else {
					++err;
				}
			}
			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre)
		{
			return load(pre, true, true);
		}
	};

}
