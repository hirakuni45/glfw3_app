#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_list クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_null.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI WidgetList クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_list : public widget {

		typedef widget_list value_type;

		typedef std::function<void (const std::string&, int)> select_func_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_button パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param	plate_param_;	///< プレート・パラメーター
			color_param	color_param_;	///< カラー・パラメーター
			text_param	text_param_;	///< テキスト描画のパラメーター
			color_param	color_param_select_;	///< 選択時カラー・パラメーター

			utils::strings	text_list_;	///< テキスト・リスト

			int			select_pos_;	///< テキスト・リストの選択位置

			bool		drop_box_;		///< ドロップ・ボックスの表示
			bool		scroll_ctrl_;	///< スクロール・コントロール（マウスのダイアル）

			bool		open_before_;
			bool		open_;

			select_func_type	select_func_;	///< セレクト関数

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_list_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
				vtx::placement(vtx::placement::holizontal::LEFT,
				vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_list_color_select_),
				text_list_(),
				select_pos_(0), drop_box_(true), scroll_ctrl_(true),
				open_before_(false), open_(false),
				select_func_(nullptr)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	objh_;
		gl::mobj::handle	select_objh_;

		widget_null*		frame_;
		widget_labels		list_;

		void destroy_list_() {
			for(widget_label* w : list_) {
				wd_.del_widget(w);
			}
		}

		void destroy_() {
			destroy_list_();
			wd_.del_widget(frame_);
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_list(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p),
			objh_(0), select_objh_(0), frame_(0), list_()
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_list() { destroy_(); }


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
			@brief	選択位置を取得
			@return 選択位置（負の値なら、非選択）
		*/
		//-----------------------------------------------------------------//
		int get_select_pos() const { return param_.select_pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択位置のテキストを取得
			@return 選択位置のテキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_select_text() const { return param_.text_param_.get_text(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	リスト・リソースの更新
		*/
		//-----------------------------------------------------------------//
		void update_list()
		{
			destroy_list_();
			list_.clear();

			widget::param wp(vtx::irect(vtx::ipos(0), get_rect().size), frame_);
			widget_label::param wp_;
			wp_.plate_param_ = param_.plate_param_;
			wp_.color_param_ = param_.color_param_select_;
			wp_.plate_param_.frame_width_ = 0;
			int n = 0;
			for(const std::string& s : param_.text_list_) {
				wp_.text_param_.set_text(s);
				if(n == 0) {
					wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					wp_.plate_param_.round_style_
						= widget::plate_param::round_style::TOP;
				} else if(n == (param_.text_list_.size() - 1)) {
					wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
					wp_.plate_param_.round_style_
						= widget::plate_param::round_style::BOTTOM;
				} else {
					wp_.plate_param_.round_radius_ = 0;
					wp_.plate_param_.round_style_
						= widget::plate_param::round_style::ALL;
				}
				widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
				w->set_state(widget::state::ENABLE, false);
				list_.push_back(w);
				wp.rect_.org.y += get_rect().size.y;
				++n;
			}
		}


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

			t.color_param_ = param_.color_param_select_;
			select_objh_ = wd_.share_add(t);

			{
				widget::param wp(vtx::irect(0, 0,
					get_rect().size.x, get_rect().size.y * param_.text_list_.size()), this);
				widget_null::param wp_;
				frame_ = wd_.add_widget<widget_null>(wp, wp_);
				frame_->set_state(widget::state::POSITION_LOCK);
				frame_->set_state(widget::state::ENABLE, false);
			}

			{
				widget::param wp(vtx::irect(vtx::ipos(0), get_rect().size), frame_);
				widget_label::param wp_;
				wp_.plate_param_ = param_.plate_param_;
				wp_.color_param_ = param_.color_param_select_;
				wp_.plate_param_.frame_width_ = 0;
				int n = 0;
				for(const std::string& s : param_.text_list_) {
					wp_.text_param_.set_text(s);
					if(n == 0) {
						wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
						wp_.plate_param_.round_style_
							= widget::plate_param::round_style::TOP;
					} else if(n == (param_.text_list_.size() - 1)) {
						wp_.plate_param_.round_radius_ = param_.plate_param_.round_radius_;
						wp_.plate_param_.round_style_
							= widget::plate_param::round_style::BOTTOM;
					} else {
						wp_.plate_param_.round_radius_ = 0;
						wp_.plate_param_.round_style_
							= widget::plate_param::round_style::ALL;
					}
					widget_label* w = wd_.add_widget<widget_label>(wp, wp_);
					w->set_state(widget::state::ENABLE, false);
					list_.push_back(w);
					wp.rect_.org.y += get_rect().size.y;
					++n;
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
			if(!get_state(widget::state::ENABLE)) {
				return;
			}



			if(param_.select_pos_ < list_.size()) {
				widget_label* w = list_[param_.select_pos_];
				param_.text_param_.text_ = w->get_local_param().text_param_.text_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() override
		{
			if(!get_state(widget::state::ENABLE)) {
				return;
			}

			param_.open_before_ = param_.open_;
			if(get_selected() && list_.size() > 0) {
				param_.open_ = true;
				wd_.enable(frame_, param_.open_, true);
				wd_.top_widget(frame_);
			}

			if(param_.open_ && list_.size() > 0) {
				uint32_t n = 0;
				bool selected = false;
				for(widget_label* w : list_) {
					if(w->get_select()) {
						param_.select_pos_ = n;
						param_.text_param_.text_ = w->get_local_param().text_param_.text_;
					} else if(w->get_selected()) {
						selected = true;
					}
					++n;
				}
				if(selected) {
					param_.open_ = false;
					wd_.enable(frame_, param_.open_, true);
					if(param_.select_func_ != nullptr) {
						param_.select_func_(param_.text_param_.get_text(), param_.select_pos_);
					}
				} else {
					const vtx::spos& scr = wd_.get_scroll();
					if(frame_->get_focus() && scr.y != 0) {
						int pos = param_.select_pos_;
						pos += scr.y;
						if(pos < 0) {
							pos = 0;
						} else if(pos >= static_cast<int>(list_.size())) {
							pos = list_.size() - 1;
						}
						param_.select_pos_ = pos;
					}
					int n = 0;
					for(widget_label* w : list_) {
						if(n == param_.select_pos_) {
							w->set_action(widget::action::SELECT_HIGHLIGHT);
						} else {
							w->set_action(widget::action::SELECT_HIGHLIGHT, false);
						}
						++n;
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override {
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

			if(!param_.open_ && param_.drop_box_) {
				wd_.at_mobj().setup_matrix(siz.x, siz.y);
				wd_.set_TSC();
				// チップの描画
				gl::mobj::handle h;
				if((get_rect().org.y + frame_->get_rect().size.y) > siz.y) {
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
			if(!pre.put_integer(path + "/selector", param_.select_pos_)) ++err;
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
			if(!pre.get_integer(path + "/selector", param_.select_pos_)) ++err;
			return err == 0;
		}
	};
}
