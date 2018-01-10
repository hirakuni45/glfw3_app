#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_list クラス @n
			このクラスは、DROP_DOWN 等に相当するクラスで、「widget_menu」@n
			を内包する。@n
			メニューの直接操作は、widget_menu のポインター経由で行う。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_menu.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI WidgetList クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_list : public widget {

		typedef widget_list value_type;

		typedef utils::strings strings;

		typedef std::function<void (const std::string& select_text, uint32_t select_pos)> select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_list パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param	plate_param_;	///< プレート・パラメーター
			color_param	color_param_;	///< カラー・パラメーター
			text_param	text_param_;	///< テキスト描画のパラメーター
			color_param	color_param_select_;	///< 選択時カラー・パラメーター

			strings		init_list_;		///< リスト

			select_func_type	select_func_;	///< セレクト関数

			bool		load_func_;		///< プリファレンス・ロード時に「セレクト関数」を実行しない場合 false
			bool		drop_box_;		///< ドロップ・ボックスの表示
			bool		scroll_ctrl_;	///< スクロール・コントロール（マウスのダイアル）

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_list_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_list_color_select_),
				init_list_(),
				load_func_(true), drop_box_(true), scroll_ctrl_(true)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	select_objh_;

		widget_menu*		menu_;
		uint32_t			id_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_list(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p),
			objh_(0), select_objh_(0), menu_(nullptr), id_(0)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_list() { }


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
		const char* type_name() const override { return "list"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const override { return true; }


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
			@brief	選択テキストの取得（ボタンの表示テキスト）
			@return 選択テキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_select_text() const { return param_.text_param_.get_text(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	メニュー・リソースの取得
			@return メニュー・リソース
		*/
		//-----------------------------------------------------------------//
		widget_menu* get_menu() { return menu_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override {
			// 標準的に固定、リサイズ不可
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::POSITION_LOCK);
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::MOVE_STALL);

			vtx::ipos size;
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

			t.color_param_ = param_.color_param_select_;
			select_objh_ = wd_.share_add(t);

			{  // メニューの生成
				widget::param wp(vtx::irect(vtx::ipos(0), get_rect().size), this);
				widget_menu::param wp_;
				wp_.init_list_ = param_.init_list_;
				menu_ = wd_.add_widget<widget_menu>(wp, wp_);
				menu_->enable(false);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			if(!get_state(widget::state::ENABLE)) {
				return;
			}

			if(get_selected()) {  // 「ボタン選択」で内包メニューを有効にする。
				menu_->enable();
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

			if(id_ != menu_->get_local_param().id_) {
				param_.text_param_.set_text(menu_->get_select_text());
				if(param_.select_func_ != nullptr) {
					param_.select_func_(param_.text_param_.get_text(), menu_->get_select_pos());
				}
				id_ = menu_->get_local_param().id_;
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
			const vtx::spos& siz = core.get_rect().size;

			gl::mobj::handle h = objh_;
			if(get_select()) {
				h = select_objh_;
			}

			if(param_.plate_param_.resizeble_) {
				wd_.at_mobj().resize(objh_, get_param().rect_.size);
			}

			render_text(wd_, h, get_param(), param_.text_param_, param_.plate_param_);

			if(param_.drop_box_) {
				wd_.at_mobj().setup_matrix(siz.x, siz.y);
				wd_.set_TSC();

				// チップの描画
				gl::mobj::handle h;
				if((get_rect().org.y + menu_->get_rect().size.y) > siz.y) {
					h = wd_.get_share_image().up_box_;
				} else {
					h = wd_.get_share_image().down_box_;
				}

				const vtx::spos& bs = wd_.at_mobj().get_size(h);
				const vtx::spos& size = get_rect().size;
				short wf = param_.plate_param_.frame_width_;
				short space = 4;
				vtx::spos pos(size.x - bs.x - wf - space, (size.y - bs.y) / 2);
				wd_.at_mobj().draw(h, gl::mobj::attribute::normal, pos);
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
			if(!pre.put_text(path + "/selector", param_.text_param_.get_text())) {
				++err;
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
		bool load(const sys::preference& pre) override {
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			std::string text;
			if(!pre.get_text(path + "/selector", text)) {
				++err;
			} else {
				param_.text_param_.set_text(text);
				if(menu_ != nullptr) {
					menu_->select(text);
					if(param_.load_func_ && param_.select_func_ != nullptr) {
						param_.select_func_(menu_->get_select_text(), menu_->get_select_pos());
					}
				}
			}
			return err == 0;
		}
	};
}
