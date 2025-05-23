#pragma once
//=========================================================================//
/*!	@file
	@brief	GUI widget_button クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_utils.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_button クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_button : public widget {

		typedef widget_button value_type;

		typedef std::function< void(uint32_t id) > select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_button パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;	///< 平面描画パラメーター
			color_param		color_param_;	///< 頂点カラーで変調する場合のパラメーター
			text_param		text_param_;	///< テキスト描画のパラメータ
			const img::i_img*	image_;		///< ボタンに画像を使う場合
			gl::mobj::handle	handle_;	///< ボタンにモーションオブジェクトを使う場合
			uint32_t			id_;		///< セレクト ID （押された回数）

			select_func_type	select_func_;	///< セレクト関数

			param(const std::string& text = "") noexcept :
				plate_param_(), color_param_(widget_director::default_button_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255)),
				image_(0), handle_(0), id_(0),
				select_func_(nullptr)
				{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;

		uint32_t			id_;
		bool				exec_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_button(widget_director& wd, const widget::param& bp, const param& p) noexcept :
			widget(bp), wd_(wd), param_(p), objh_(0), id_(0), exec_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_button() { }


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
		const char* type_name() const noexcept override { return "button"; }


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
			@brief	テキストを設定
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text) noexcept
		{
			param_.text_param_.set_text(text);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	実行（ボタンを押して、登録関数が実行）
		*/
		//-----------------------------------------------------------------//
		void exec() noexcept
		{
			exec_ = true;
			++param_.id_;			
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() noexcept override
		{
			// ボタンは標準的に固定、サイズ固定、選択時拡大
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);
			at_param().action_.set(widget::action::SELECT_SCALE);

			using namespace img;

			if(param_.handle_) {
				at_rect().size = wd_.at_mobj().get_size(param_.handle_);
				objh_ = param_.handle_;
			} else if(param_.image_) {
				paint pa;
				const vtx::spos& size = get_rect().size;
				create_image_base(param_.image_, size, pa);
				at_rect().size = pa.get_size();
				objh_ = wd_.at_mobj().install(&pa);
			} else {
				vtx::spos size;
				if(param_.plate_param_.resizeble_) {
					vtx::spos rsz = param_.plate_param_.grid_ * 3;
					if(get_param().rect_.size.x >= rsz.x) size.x = rsz.x;
					else size.x = get_param().rect_.size.x;
					if(get_param().rect_.size.y >= rsz.y) size.y = rsz.y;
					else size.y = get_param().rect_.size.y;
				} else {
					size = get_param().rect_.size;
				}
				share_t t;
				t.size_ = size;
				t.color_param_ = param_.color_param_;
				t.plate_param_ = param_.plate_param_;
				objh_ = wd_.share_add(t);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() noexcept override
		{
			if(get_selected()) {
				++param_.id_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept override
		{
			if(exec_) {
				exec_ = false;
			} else if(!get_state(state::ENABLE)) {
				return;
			}

			if(id_ != param_.id_) {
				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.id_);
				}
				id_ = param_.id_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() noexcept override
		{
			if(objh_ == 0) return;

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(objh_, get_param().rect_.size);
			}

			render_text(wd_, objh_, get_param(), param_.text_param_, param_.plate_param_);
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
			return true;
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
			return true;
		}
	};
}
