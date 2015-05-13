//=====================================================================//
/*!	@file
	@brief	GUI widget_label クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_utils.hpp"
#include "img_io/paint.hpp"

namespace gui {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_label::initialize()
	{
		// 標準的に固定
		at_param().state_.set(widget::state::POSITION_LOCK);
		at_param().state_.set(widget::state::SIZE_LOCK);
		at_param().state_.set(widget::state::SERVICE);

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

		if(!param_.read_only_) {
			param_.text_in_pos_ = param_.text_param_.text_.size();
		}

		share_t t;
		t.size_ = size;
		t.color_param_ = param_.color_param_;
		t.plate_param_ = param_.plate_param_;
		objh_ = wd_.share_add(t);

		t.color_param_ = param_.color_param_select_;
		select_objh_ = wd_.share_add(t);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_label::update()
	{
		if(param_.text_in_) return;

		// テキスト入力位置を調整
		if(param_.text_in_pos_ > param_.text_param_.text_.size()) {
			param_.text_in_pos_ = param_.text_param_.text_.size();
		}

		param_.shift_param_.size_ = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
		shift_text_update(get_param(), param_.text_param_, param_.shift_param_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_label::service()
	{
		if(get_select_in()) {
			if(!param_.read_only_) {
				param_.text_in_ = true;
			}
		}
		if(wd_.get_focus_widget() == this || wd_.get_focus_widget() == wd_.root_widget(this)) {
			focus_ = true;
		} else {
			param_.text_param_.cursor_ = -1;
			focus_ = false;
		}
		if(focus_) {
			if(!param_.read_only_ && param_.text_in_) {
				bool text_in = param_.text_in_;
				const std::string& ins = wd_.get_keyboard().input();
				BOOST_FOREACH(char ch, ins) {
					if(param_.text_in_limit_ > 0 && param_.text_in_limit_ <= param_.text_in_pos_) {
						param_.text_in_ = false;
						continue;
					}
					if(ch == sys::keyboard::ctrl::DELETE) {
						if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
							param_.text_param_.text_.erase(param_.text_in_pos_, 1);
						}
					} else if(ch < 0x20) {
						if(ch == sys::keyboard::ctrl::BS) {
							if(param_.text_in_pos_) {
								--param_.text_in_pos_;
								param_.text_param_.text_.erase(param_.text_in_pos_, 1);
							}
						} else if(ch == sys::keyboard::ctrl::CR) {
							if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
								param_.text_param_.text_.erase(param_.text_in_pos_);
							}
							param_.text_param_.offset_.x = 0;
							param_.text_in_ = false;
						} else if(ch == sys::keyboard::ctrl::ESC) {
							param_.text_param_.offset_.x = 0;
							param_.text_in_ = false;
						} else if(ch == sys::keyboard::ctrl::RIGHT) {
							if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
								++param_.text_in_pos_;
							}
						} else if(ch == sys::keyboard::ctrl::LEFT) {
							if(param_.text_in_pos_) {
								--param_.text_in_pos_;
							}
						}
					} else {
						if(param_.text_param_.text_.size() <= param_.text_in_pos_) {
							param_.text_param_.text_ += ch;
						} else {
							param_.text_param_.text_.insert(param_.text_in_pos_, 1, ch);
						}
						++param_.text_in_pos_;
					}
				}
				// 入力完了、ファンクション呼び出し
				if(text_in && !param_.text_in_) {
					if(param_.select_func_) param_.select_func_(param_.text_param_.get_text());
				}
			}

			// テキスト幅が、収容範囲を超える場合
			if(param_.text_in_) {
				gl::core& core = gl::core::get_instance();
				gl::fonts& fonts = core.at_fonts();

				if(!param_.text_param_.font_.empty()) {
					fonts.push_font_face();
					fonts.set_font_type(param_.text_param_.font_);
				}
				fonts.enable_proportional(param_.text_param_.proportional_);
				utils::lstring ls = param_.text_param_.text_;
				if(param_.text_in_pos_ < ls.size()) {
					ls.erase(param_.text_in_pos_);
				}
				ls += ' ';
				short fw = fonts.get_width(ls);
				if(!param_.text_param_.font_.empty()) {
					fonts.pop_font_face();
				}
				short w = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
				if(fw >= w) {
					param_.text_param_.offset_.x = -(fw - w);
				} else {
					param_.text_param_.offset_.x = 0;
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_label::render()
	{
		gl::mobj::handle h = objh_;
		if(get_select() || get_state(widget::state::SELECTED)) {
			h = select_objh_;
		}

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(h, get_param().rect_.size);
		}

		text_param tp = param_.text_param_;
		param_.text_param_.cursor_ = -1;
		if(param_.text_in_ && focus_) {
			if((interval_ % 40) < 20) {
				if(param_.text_param_.text_.size() <= param_.text_in_pos_) {
					param_.text_param_.cursor_ = param_.text_param_.text_.size();
					tp.text_ += ' ';
				} else {
					param_.text_param_.cursor_ = param_.text_in_pos_;
				}
			}
		}

		render_text(wd_, h, get_param(), tp, param_.plate_param_);
		++interval_;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_label::save(sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		if(!pre.put_text(path + "/text", get_text())) ++err;
		return err == 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_label::load(const sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		std::string s;
		bool f = pre.get_text(path + "/text", s);
		if(f) {
			set_text(s);
		}
		return f;
	}
}
