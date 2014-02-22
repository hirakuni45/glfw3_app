//=====================================================================//
/*!	@file
	@brief	GUI widget_label クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_label.hpp"
#include "widgets/widget_utils.hpp"
#include "gl_fw/IGLcore.hpp"
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
		gl::IGLcore* Igl = gl::get_glcore();
		const vtx::spos& size = Igl->get_size();		
		gl::fonts& fonts = Igl->at_fonts();

		const widget::param& bp = get_param();
		if(param_.shift_every_ ||
		  (param_.shift_enable_ && bp.hold_frame_ >= param_.shift_hold_frame_)) {
			if(!param_.text_param_.font_.empty()) {
				fonts.push_font_info();
				fonts.set_font_type(param_.text_param_.font_);
			}
			fonts.enable_proportional(param_.text_param_.proportional_);
			short fw = fonts.get_width(param_.text_param_.text_);
			if(!param_.text_param_.font_.empty()) {
				fonts.pop_font_info();
			}
			const vtx::spos& size = bp.rect_.size;
			if(size.x < fw) {
				param_.shift_offset_ -= param_.shift_speed_;
				if((static_cast<short>(param_.shift_offset_) + fw) <= 0) {
					param_.shift_offset_ = size.x;
				}
				param_.text_param_.offset_.x = param_.shift_offset_;
			} else {
				param_.text_param_.offset_.x = 0;
				param_.shift_offset_ = 0.0f;
			}
		} else {
			param_.text_param_.offset_.x = 0;
			param_.shift_offset_ = 0.0f;
		}
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
		if(wd_.get_top_widget() != this) {
			param_.text_in_ = false;
		} else {
			if(!param_.read_only_ && param_.text_in_) {
				const std::string& ins = wd_.get_keyboard().input();
				BOOST_FOREACH(char ch, ins) {
					if(ch == 0x3f) {
						if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
							param_.text_param_.text_.erase(param_.text_in_pos_, 1);
						}
					} else if(ch < 0x20) {
						if(ch == 0x08) {
							if(param_.text_in_pos_) {
								--param_.text_in_pos_;
								param_.text_param_.text_.erase(param_.text_in_pos_, 1);
							}
						} else if(ch == ('Q'-0x40)) {
							if(param_.text_in_pos_ < param_.text_param_.text_.size()) {
								++param_.text_in_pos_;
							}
						} else if(ch == ('R'-0x40)) {
							if(param_.text_in_pos_) {
								--param_.text_in_pos_;
							}
						}
					} else {
						if(param_.text_param_.text_.size() <= param_.text_in_pos_) {
							param_.text_param_.text_ += ch;
						} else {
							param_.text_param_.text_[param_.text_in_pos_] = ch;
						}
						++param_.text_in_pos_;
					}
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
		if(get_select() || get_state(widget::state::SYSTEM_SELECT)) {
			h = select_objh_;
		}

		if(param_.plate_param_.resizeble_) {
			wd_.at_mobj().resize(h, get_param().rect_.size);
		}

		text_param tp = param_.text_param_;
		if(wd_.get_top_widget() == this) {
			if((interval_ % 40) < 20) {
				if(tp.text_.size() <= param_.text_in_pos_) {
					tp.text_ += '_';
				} else {
					tp.text_[param_.text_in_pos_] = '_';
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
		return true;
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
		return true;
	}
}
