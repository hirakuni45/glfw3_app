#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_tree クラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_check.hpp"
#include "utils/tree_unit.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_tree クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_tree : public widget {

		typedef widget_tree value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_tree パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			color_param	color_param_;	///< カラー・パラメーター

			short		height_;		///< ユニットの高さ

			bool		single_;		///< シングル選択の場合「true」

			param() :
				color_param_(widget_director::default_tree_color_),
				height_(28),
				single_(true)
			{ }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ツリー・データベースのユニット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct value {
			widget_check*	w_;
			std::string		data_;

			value() : w_(0), data_()
			{ }
		};
		typedef utils::tree_unit<value>	tree_unit;

	private:
		widget_director&	wd_;

		param				param_;

		gl::mobj::handle	vr_;
		gl::mobj::handle	r_;
		gl::mobj::handle	v_;
		gl::mobj::handle	h_;

		tree_unit			tree_unit_;
		uint32_t			serial_id_;
		tree_unit::unit_map_its	tree_unit_its_;

		vtx::fpos			speed_;
		vtx::fpos			offset_;
		vtx::fpos			position_;

		uint32_t			select_id_;
		tree_unit::unit_map_it	select_it_;

		struct root_t {
			vtx::ipos			pos;
			gl::mobj::handle	h;
		};
		std::vector<root_t>	roots_;


		void create_()
		{
			vtx::irect r;
			r.org.set(0);
			r.size.x = get_rect().size.x;
			r.size.y = param_.height_;
			tree_unit::unit_map_its its;
			tree_unit_.create_list("", its);

			BOOST_FOREACH(tree_unit::unit_map_it it, its) {
				if(!it->second.value.w_) {
					gl::core& core = gl::core::get_instance();
					gl::fonts& fonts = core.at_fonts();
					r.size.x = fonts.get_width(utils::get_file_name(it->first)) + r.size.y + 8;
					widget::param wp(r, this);
					widget_check::param wp_(utils::get_file_name(it->first));
					wp_.type_ = widget_check::style::MINUS_PLUS;
					widget_check* w = wd_.add_widget<widget_check>(wp, wp_);
					w->set_state(widget::state::POSITION_LOCK);
					w->set_state(widget::state::SIZE_LOCK);
					w->set_state(widget::state::MOVE_ROOT, false);
					w->set_state(widget::state::RESIZE_ROOT);
					w->set_state(widget::state::DRAG_UNSELECT);
					w->set_state(widget::state::CLIP_PARENTS);
					if(tree_unit_.is_directory(it)) {
						r.org.y += param_.height_;
					} else {
						wd_.enable(w, false);
					}
					it->second.value.w_ = w;
				}
			}
		}


		void destroy_()
		{
			tree_unit::unit_map_its its;
			tree_unit_.create_list("", its);
			BOOST_FOREACH(tree_unit::unit_map_it it, its) {
				wd_.del_widget(it->second.value.w_);
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_tree(widget_director& wd, const widget::param& bp, const param& p) :
			widget(bp), wd_(wd), param_(p),
			vr_(0), r_(0), v_(0), h_(0),
			tree_unit_(), serial_id_(0),
			speed_(0.0f), offset_(0.0f), position_(0.0f), select_id_(0), select_it_()
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_tree() { destroy_(); }


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
		const char* type_name() const override { return "tree"; }


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
			@brief	有効・無効の設定
			@param[in]	f	無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { wd_.enable(this, f, true); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ツリーの参照
			@return ツリー
		*/
		//-----------------------------------------------------------------//
		tree_unit& at_tree_unit() { return tree_unit_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択 ID を取得（メニューが選択される毎に ID が進む）
			@return 選択 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_id() const { return select_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択メニューを取得
			@return 選択メニュー
		*/
		//-----------------------------------------------------------------//
		tree_unit::unit_map_it get_select_it() const { return select_it_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			at_param().state_.set(widget::state::SIZE_LOCK);
			at_param().state_.set(widget::state::RESIZE_H_ENABLE, false);
			at_param().state_.set(widget::state::RESIZE_V_ENABLE, false);
			at_param().state_.set(widget::state::SERVICE);
			at_param().state_.set(widget::state::MOVE_ROOT, false);
			at_param().state_.set(widget::state::RESIZE_ROOT);
			at_param().state_.set(widget::state::CLIP_PARENTS);
			at_param().state_.set(widget::state::AREA_ROOT);

			vr_ = wd_.get_share_image().VR_junction_;
			r_  = wd_.get_share_image().R_junction_;
			v_  = wd_.get_share_image().V_line_;
			h_  = wd_.get_share_image().H_line_;
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
					at_rect() = w->get_draw_area();
				}
			}

			if(param_.single_) {
				widget_check* sel = 0;
				BOOST_FOREACH(tree_unit::unit_map_it it, tree_unit_its_) {
					widget_check* w = it->second.value.w_;
					if(w == 0) continue;
					if(tree_unit_.is_directory(it)) continue;
					if(w->get_select_out()) {
						sel = w;
						break;
					}
				}
				if(sel) {
					BOOST_FOREACH(tree_unit::unit_map_it it, tree_unit_its_) {
						widget_check* w = it->second.value.w_;
						if(w == 0) continue;
						if(tree_unit_.is_directory(it)) continue;
						if(w != sel) {
							w->set_check(false);
						} else {
							select_it_ = it;
						}
					}
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
			if(!get_state(widget::state::ENABLE)) {
				return;
			}

			// ツリーが更新されたら、アイテムを作り直す
			if(tree_unit_.get_serial_id() != serial_id_) {
				create_();
				tree_unit_.create_list("", tree_unit_its_);
				serial_id_ = tree_unit_.get_serial_id();
			}

			roots_.clear();
			vtx::spos pos(0);
			BOOST_FOREACH(tree_unit::unit_map_it it, tree_unit_its_) {
				widget_check* w = it->second.value.w_;
				if(w == 0) continue;
				bool draw = true;
				std::vector<bool> st;
				uint32_t n = utils::count_char(it->first, '/');
				if(n > 1) {
					std::string path = it->first;
					for(uint32_t i = 1; i < n; ++i) {
						auto p = utils::get_file_path(path);
						tree_unit::optional_const_ref opt = tree_unit_.get(p);
						if(opt) {
							widget_check* w = (*opt).w_;
							if(w) {
								if(!w->get_check()) {
									draw = false;
									break;
								}
								st.push_back(w->get_check());						
							}
						}
						path = p;
					}
				}
				wd_.enable(w, draw);
				if(draw) {
					w->at_local_param().draw_box_ = tree_unit_.is_directory(it);
					pos.x = (n - 1) * param_.height_;
					w->at_rect().org = pos;
#if 0
					short x = 0;
					BOOST_FOREACH(bool b, st) {
						root_t r;
						r.pos.x = x;
						r.pos.y = pos.y;
						r.h = h_;
////						roots_.push_back(r);
						x += param_.height_;
					}
#endif
					pos.y += param_.height_;
				}
			}

			if(get_select_in()) {
				speed_.set(0.0f);
				offset_ = position_;
			}
			float damping = 0.85f;
			float slip_gain = 0.5f;
			short d = get_rect().size.y - pos.y;
			if(get_select()) {
				position_ = offset_ + get_param().move_pos_ - get_param().move_org_;
				if(d < 0) {
					if(position_.y < d) {
						position_.y -= d;
						position_.y *= slip_gain;
						position_.y += d;
					} else if(position_.y > 0) {
						position_.y *= slip_gain;
					}
				} else {
					position_.y *= slip_gain;
				}
			} else {
				if(d < 0) {
					if(position_.y < d) {
						position_.y -= d;
						position_.y *= damping;
						position_.y += d;
						speed_.y = 0.0f;
						if(position_.y > (d - 0.5f)) {
							position_.y = d;
						}
					} else if(position_.y > 0.0f) {
						position_.y *= damping;
						speed_.y = 0.0f;
						if(position_.y < 0.5f) {
							position_.y = 0.0f;
						}
					} else {
						const vtx::spos& scr = wd_.get_scroll();
						if(get_focus() && scr.y != 0) {
							position_.y += scr.y * param_.height_;
							if(position_.y < d) {
								position_.y = d;
							} else if(position_.y > 0.0f) {
								position_.y = 0.0f;
							}
						}
					}
				} else {
					position_.y *= damping;
					if(-0.5f < position_.y && position_.y < 0.5f) {
						position_.y = 0.0f;
						speed_.y = 0.0f;
					}
				}
			}

			short ofsy = position_.y;
			BOOST_FOREACH(tree_unit::unit_map_it it, tree_unit_its_) {
				widget_check* w = it->second.value.w_;
				if(w == 0) continue;
				w->at_rect().org.y += ofsy;
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
			const widget::param& wp = get_param();

			// 各部品のルートを描画
			if(wp.clip_.size.x > 0 && wp.clip_.size.y > 0) { 
				BOOST_FOREACH(const root_t& r, roots_) {
					glPushMatrix();
					const vtx::spos& mosz = wd_.at_mobj().get_size(r.h);
					vtx::spos ofs(0, (wp.rect_.size.y - mosz.y) / 2);
					ofs += r.pos;
					if(wp.state_[widget::state::CLIP_PARENTS]) {
						draw_mobj(wd_, r.h, wp.clip_, ofs + wp.rpos_);
					} else {
						wd_.at_mobj().draw(r.h, gl::mobj::attribute::normal, ofs);
					}
					glPopMatrix();
				}

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
		bool save(sys::preference& pre) override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			tree_unit::unit_map_its its;
			tree_unit_.create_list("", its);
			BOOST_FOREACH(tree_unit::unit_map_it it, its) {
				widget_check* w = it->second.value.w_;
				if(w == 0) continue;
///				const vtx::spos& pos = w->get_rect().org;
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
		bool load(const sys::preference& pre) override
		{
			std::string path;
			path += '/';
			path += wd_.create_widget_name(this);

			int err = 0;
			tree_unit::unit_map_its its;
			tree_unit_.create_list("", its);
			BOOST_FOREACH(tree_unit::unit_map_it it, its) {
				widget_check* w = it->second.value.w_;
				if(w == 0) continue;			
			}

			return err == 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	構造のクリア
		*/
		//-----------------------------------------------------------------//
		void clear() {
			destroy_();
			tree_unit_.clear();
			tree_unit_its_.clear();
///			select_it_ = tree_unit_.end();
		}

	};

}
