//=====================================================================//
/*!	@file
	@brief	GUI Widget ファイラー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_utils.hpp"
#include "widgets/widget_filer.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

namespace gui {

	void widget_filer::create_file_(widget_file& wf, const vtx::srect& rect, short ofs, const std::string& fn)
	{
		{
			widget::param wp(rect, files_);
			widget_null::param wp_;
			wf.base = wd_.add_widget<widget_null>(wp, wp_);
			wf.base->set_state(widget::state::POSITION_LOCK);
			wf.base->set_state(widget::state::SIZE_LOCK);
			wf.base->set_state(widget::state::CLIP_PARENTS);
			wf.base->set_state(widget::state::DRAG_UNSELECT);
			wf.base->set_state(widget::state::SELECT_PARENTS);
		}
		short fns = rect.size.x * 2 / 3;
		short ats = rect.size.x - fns - 2;
		{
			vtx::srect r;
			r.org.set(0);
			r.size.set(fns, param_.label_height_);
			widget::param wp(r, wf.base);
			wp.action_.set(widget::action::SELECT_HIGHLIGHT);
			widget_label::param wp_(fn);
			wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
			wp_.plate_param_.frame_width_ = 0;
			wp_.plate_param_.round_radius_ = 0;
			wp_.plate_param_.resizeble_ = true;
			wf.name = wd_.add_widget<widget_label>(wp, wp_);
			wf.name->set_state(widget::state::POSITION_LOCK);
			wf.name->set_state(widget::state::CLIP_PARENTS);
			wf.name->set_state(widget::state::DRAG_UNSELECT);
			wf.name->set_state(widget::state::RESIZE_ROOT);
		}
		{
			vtx::srect r;
			r.org.set(fns + 2, 0);
			r.size.set(ats, param_.label_height_);
			widget::param wp(r, wf.base);
			wp.action_.set(widget::action::SELECT_HIGHLIGHT);
			widget_label::param wp_;
			wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
			wp_.plate_param_.frame_width_ = 0;
			wp_.plate_param_.round_radius_ = 0;
			wp_.plate_param_.resizeble_ = true;
			wf.info = wd_.add_widget<widget_label>(wp, wp_);
			wf.info->set_state(widget::state::POSITION_LOCK);
			wf.info->set_state(widget::state::CLIP_PARENTS);
			wf.info->set_state(widget::state::DRAG_UNSELECT);
			wf.info->set_state(widget::state::RESIZE_ROOT);
		}
	}


	void widget_filer::create_files_(widget_files& wfs, short ofs)
	{
		// ラベルを新規に作成
		vtx::srect rect;
		rect.org.set(ofs, 0);
		rect.size.x = path_->get_rect().size.x;
		rect.size.y = param_.label_height_;

		// ルートパスならドライブレターを加える
		std::string pp;
		if(!utils::previous_path(fsc_.get_path(), pp)) {
			for(uint32_t i = 0; i < drv_.get_num(); ++i) {
				widget_file wf;
				std::string fn;
				fn += 'A' + drv_.get_info(i).drive_;
				fn += ":/";
				wf.dir = true;
				create_file_(wf, rect, ofs, fn);
				wfs.push_back(wf);
				rect.org.y += param_.label_height_;
			}
		}

		const utils::file_infos& fos = fsc_.get();
		BOOST_FOREACH(const utils::file_info& fi, fos) {
			std::string fn = fi.get_name();
			if(fn == ".") continue;

			widget_file wf;
			if(fn == "..") {
				wf.dir = true;
			} else if(fi.is_directory()) {
				fn += '/';
				wf.dir = true;
			}

			create_file_(wf, rect, ofs, fn);

			wf.size = fi.get_size();
			wf.time = fi.get_time();
			wf.mode = fi.get_mode();

			wfs.push_back(wf);
			rect.org.y += param_.label_height_;
		}
	}


	widget_filer::widget_files_cit widget_filer::scan_select_in_file_(widget_files& wfs) const
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(wf.name->get_select_in() || wf.info->get_select_in()) {
				return cit;
			}
		}
		return wfs.end();
	}


	widget_filer::widget_files_cit widget_filer::scan_select_file_(widget_files& wfs) const
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(wf.name->get_select() || wf.info->get_select()) {
				return cit;
			}
		}
		return wfs.end();
	}


	widget_filer::widget_files_cit widget_filer::scan_selected_file_(widget_files& wfs) const
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(wf.name->get_selected() || wf.info->get_selected()) {
				return cit;
			}
		}
		return wfs.end();
	}


	void widget_filer::resize_files_(widget_files& wfs, short ofs, short width)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			wf.base->at_rect().org.x = ofs;
			wf.base->at_rect().size.x = width;
			short name_size = width * 2 / 3;
			short space = 2;
			short info_size = width - name_size - space;
			short info_limit = 130;
			if(info_state_ == info_state::TIME) {
				info_limit = 200;
			}
			if(info_size >= info_limit) {
				info_size = info_limit;
				name_size = width - space - info_limit;
			}
			if(info_state_ == info_state::NONE) {
				name_size = width;
				info_size = 0;
			}
			wf.name->at_rect().size.x = name_size;
			wf.info->at_rect().org.x  = name_size + space;
			wf.info->at_rect().size.x = info_size;
		}
	}


	void widget_filer::update_files_info_(widget_files& wfs)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(info_state_ == info_state::SIZE) {
				std::string& s = wf.info->at_local_param().text_param_.text_;
				if(wf.dir) {
					s = " -";
				} else {
					s = ' ' + boost::lexical_cast<std::string>(wf.size);
				}
			} else if(info_state_ == info_state::TIME) {
				std::string s;
				s += ' ';
				struct tm* t = localtime(&wf.time);
				s += (boost::format("%02d:%02d ") % t->tm_hour % t->tm_min).str();
				s += (boost::format("%d/%d ")
					% (t->tm_mon + 1) % t->tm_mday).str();
				s += (boost::format("%4d") % (t->tm_year + 1900)).str();
				wf.info->at_local_param().text_param_.text_ = s;
			} else if(info_state_ == info_state::MODE) {
				std::string s;
				s += ' ';
				uint32_t bit = 1 << 8;
				static const char chmod[9]
					= { 'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x' };
				for(int i = 0; i < 9; ++i) {
					if(wf.mode & bit) {
						s += chmod[i];
					} else {
						s += '-';
					}
					bit >>= 1;
				}
				wf.info->at_local_param().text_param_.text_ = s;
			}
		}
	}


	void widget_filer::destroy_files_(widget_files& wfs)
	{
		// ラベル郡を破棄
		BOOST_FOREACH(const widget_file& wf, wfs) {
			wd_.del_widget(wf.info);
			wd_.del_widget(wf.name);
			wd_.del_widget(wf.base);
		}
		wfs.clear();
	}


	void widget_filer::get_regist_state_()
	{
		// 選択位置の回復
		file_map_it it = file_map_.find(param_.path_);
		if(it != file_map_.end()) {
			position_ = it->second.position_;
			select_pos_ = it->second.select_pos_;
		} else {
			select_pos_ = 0;
			position_.set(0.0f);
		}
	}


	void widget_filer::set_regist_state_()
	{
		// パスに紐づいた位置の記録
		file_map_it it = file_map_.find(param_.path_);
		if(it == file_map_.end()) {
			file_t t;
			it = file_map_.insert(file_map_pair(param_.path_, t)).first;
		}
		file_t& f = it->second;
		f.position_ = position_;
		f.select_pos_ = select_pos_;
	}


	void widget_filer::set_select_pos_(uint32_t pos)
	{
		BOOST_FOREACH(const widget_file& wf, center_) {
			wf.name->set_state(widget::state::SYSTEM_SELECT, false);
			wf.info->set_state(widget::state::SYSTEM_SELECT, false);
		}
		select_pos_ = pos;
		set_regist_state_();
	}


	void widget_filer::destroy_()
	{
		destroy_files_(left_);
		destroy_files_(center_);
		destroy_files_(right_);
		wd_.del_widget(files_);
		wd_.del_widget(main_);
		wd_.del_widget(path_);
	}


	bool widget_filer::focus_(const std::string& fn)
	{
		uint32_t n = 0;
		BOOST_FOREACH(const widget_file& wf, center_) {
			std::string t;
			utils::strip_last_of_delimita_path(wf.name->at_local_param().text_param_.text_, t);
			if(t == fn) {
				if(wf.name->get_state(widget::state::SYSTEM_SELECT)) {
					return true;
				}
				short ofs = static_cast<short>(n) -
				(main_->get_rect().size.y / param_.label_height_) / 2;
				if(ofs >= 0) {
					position_.y = static_cast<float>(ofs * -param_.label_height_);
				}
				set_select_pos_(n);
				return true;
			}
			++n;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・リストを取得
		@param[out]	ss	ファイル・リスト
		@param[in]	dir	ディレクトリーを含める場合「true」
		@return ファイル・パス・インデックス（負の値ならマッチ無し）
	*/
	//-----------------------------------------------------------------//
	int widget_filer::get_file_list(utils::strings& ss, bool dir)
	{
		uint32_t n = 0;
		int ret = -1;
		BOOST_FOREACH(const utils::file_info& fi, fsc_.get()) {
			std::string fn;
			if(fi.get_name() == ".") continue;
			else if(fi.get_name() == "..") continue;
			utils::append_path(param_.path_, fi.get_name(), fn);
			if(fn == file_) ret = n;
			if(dir && fi.is_directory()) {
				fn += "/";
				ss.push_back(fn);
				++n;
			} else {
				ss.push_back(fn);
				++n;
			}
		}
		return ret;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_filer::initialize()
	{
		// 自由な大きさの変更
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);
		at_param().state_.set(widget::state::SERVICE);
		at_param().state_.set(widget::state::ENABLE, false);

		param_.plate_param_.resizeble_ = true;
		at_param().resize_min_ = param_.plate_param_.grid_ * 3;
		at_param().resize_min_.x += 32;
		at_param().resize_min_.x += 32;

		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

		fsc_.set_path(param_.path_, param_.filter_);

		short frame_width = param_.plate_param_.frame_width_;
		// path（ハンドル）
		{
			vtx::srect r;
			r.org.set(frame_width);
			r.size.x = get_rect().size.x - frame_width * 2;
			r.size.y = param_.path_height_;
			widget::param wp(r, this);
			wp.action_.set(widget::action::SELECT_HIGHLIGHT);
			widget_label::param wp_(param_.path_);
			wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
			wp_.color_param_.fore_color_.set(236, 181, 63);
			wp_.color_param_.back_color_.set(131, 104, 45);
			wp_.color_param_select_.fore_color_ = wp_.color_param_.fore_color_;
			wp_.color_param_select_.back_color_ = wp_.color_param_.back_color_;
			wp_.color_param_select_.fore_color_.gainY(1.3f);
			wp_.color_param_select_.back_color_.gainY(1.3f);
			wp_.plate_param_.resizeble_ = true;
			wp_.plate_param_.frame_width_  = 2;
			wp_.plate_param_.round_radius_ = 4;
			path_ = wd_.add_widget<widget_label>(wp, wp_);
			path_->set_state(widget::state::POSITION_LOCK);
			path_->set_state(widget::state::MOVE_ROOT);
			path_->set_state(widget::state::RESIZE_ROOT);
		}

		// info ボタン
		{
			gl::mobj::handle hnd = wd_.get_share_image().right_box_;
			vtx::srect r;
			r.size = wd_.at_mobj().get_size(hnd);
			short space = 4;
			r.org.x = get_rect().size.x - r.size.x - frame_width - space;
			r.org.y = frame_width + (param_.path_height_ - r.size.y) / 2;
			widget::param wp(r, this);
			widget_button::param wp_;
			wp_.handle_ = hnd;
			info_ = wd_.add_widget<widget_button>(wp, wp_);
		}

		// main null frame
		{
			short fw = 4;
			vtx::srect r(vtx::spos(frame_width, frame_width + param_.path_height_),
				vtx::spos(get_rect().size.x - 8,
					get_rect().size.y - param_.path_height_ - fw * 2 - fw));
			widget::param wp(r, this);
			wp.state_.reset(widget::state::RENDER_ENABLE);
			widget_null::param wp_;
			main_ = wd_.add_widget<widget_null>(wp, wp_);
			main_->set_state(widget::state::POSITION_LOCK);
			main_->set_state(widget::state::SIZE_LOCK);
			main_->set_state(widget::state::RESIZE_ROOT);
		}

		// files null frame
		{
			vtx::srect r(vtx::spos(0), get_rect().size);
			widget::param wp(r, main_);
			wp.state_.reset(widget::state::RENDER_ENABLE);
			widget_null::param wp_;
			files_ = wd_.add_widget<widget_null>(wp, wp_);
			files_->set_state(widget::state::CLIP_PARENTS);
			files_->set_state(widget::state::SIZE_LOCK);
			files_->set_state(widget::state::RESIZE_ROOT);
		}

		position_.set(0.0f);
		speed_.set(0.0f, 0.0f);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_filer::update()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_filer::service()
	{
		if(!get_state(widget::state::ENABLE)) {
			return;
		}

		wd_.top_widget(this);

		// ファイル情報の取得と反映（ファイル情報収集はスレッドで動作）
		if(fsc_.probe()) {

			if(center_.empty()) {
				create_files_(center_, 0);
				update_files_info_(center_);
				if(left_.empty()) {
					std::string pp;
					if(utils::previous_path(param_.path_, pp)) {
						fsc_.set_path(pp);
						request_right_ = false;
					}
				}
			} else {
				if(request_right_) {
					if(right_.empty()) {
						create_files_(right_, 0);
						update_files_info_(right_);
					}
				} else {
					if(left_.empty()) {
						create_files_(left_, 0);
						update_files_info_(left_);
					}
				}
			}
			focus_(focus_path_);
		}

		// フレームのサイズを、仮想ウィジェットに反映
		{
			short fw = param_.plate_param_.frame_width_;
			const vtx::spos& size = get_rect().size;
			short bw = size.x - fw * 2;
			path_->at_rect().size.x = bw - param_.path_height_;
			main_->at_rect().size.x = bw;
			main_->at_rect().size.y = size.y - path_->get_rect().size.y - fw * 2;
			short space = 4;
			info_->at_rect().org.x = size.x - info_->get_rect().size.x - fw - space;
			short bh = center_.size();
			short sc = 1;
			if(left_.size() > 0) {
				resize_files_(left_,    0, bw);
				resize_files_(center_, bw, bw);
				++sc;
				if(left_.size() > bh) bh = left_.size();
			} else {
				resize_files_(center_, 0, bw);
			}
			if(right_.size() > 0) {
				if(right_.size() > bh) bh = right_.size();
				resize_files_(right_, bw * 2, bw);
				++sc;
			}
			files_->at_rect().size.x = bw * sc;
			files_->at_rect().size.y = bh * param_.label_height_;
		}

		short base_size = main_->get_rect().size.y;
		short d = base_size - center_.size() * param_.label_height_;
		float gain = 0.85f;
		float slip_gain = 0.5f;
		float speed_gain = 0.95f;
		float speed_move = 38.0f;	/// 横スクロールの初期速度

		// 左右スクロールと、スプリング効果など
		if(move_speed_ != 0.0f) {
			position_.x += move_speed_;
			float spd = move_speed_;
			move_speed_ *= 0.985f;
			if(-6.0f < move_speed_ && move_speed_ < 6.0f) {
				move_speed_ = spd;
			}
			if(move_speed_ > 0.0f && position_.x >= main_->get_rect().size.x) {
				move_speed_ = 0.0f;
				position_.x = 0.0f;
				std::string pp;
				if(utils::previous_path(param_.path_, pp)) {
					std::string tmp = param_.path_;
					param_.path_ = pp;
					destroy_files_(center_);
					center_.swap(left_);
					focus_file(tmp);
					if(utils::previous_path(param_.path_, pp)) {
						fsc_.set_path(pp, param_.filter_);
					}
				} else {
					destroy_files_(left_);
				}
				get_regist_state_();
			} else if(move_speed_ < 0.0f && position_.x <= -main_->get_rect().size.x) {
				move_speed_ = 0.0f;
				position_.x = 0.0f;
				destroy_files_(left_);
				center_.swap(left_);
				center_.swap(right_);
				get_regist_state_();
			}
		} else if(files_->get_state(widget::state::DRAG)) {
			position_ = files_->get_param().move_pos_;
			if(left_.size() > 0) {
				position_.x += main_->get_rect().size.x;
			}
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
			if(files_->get_select_out()) {
				speed_ = files_->get_param().speed_;
			}
			position_ += speed_;
			speed_ *= speed_gain;
			position_.x *= gain;
//			if(-0.5f < position_.x && position_.x < 0.5f) {
//				position_.x = 0.0f;
//				speed_.x = 0;
//			}

			// y 軸クリップ
			{
				if(d < 0) {
					if(position_.y < d) {
						position_.y -= d;
						position_.y *= gain;
						position_.y += d;
						speed_.y = 0.0f;
						if(position_.y > (d - 0.5f)) {
							position_.y = d;
						}
					} else if(position_.y > 0.0f) {
						position_.y *= gain;
						speed_.y = 0.0f;
						if(position_.y < 0.5f) {
							position_.y = 0.0f;
						}
					} else {
						const vtx::spos& scr = wd_.get_scroll();
						if(files_->get_focus() && scr.y != 0) {
							position_.y += scr.y * param_.label_height_;
							if(position_.y < d) {
								position_.y = d;
							} else if(position_.y > 0.0f) {
								position_.y = 0.0f;
							}
						}
					}
				} else {
					position_.y *= gain;
					if(-0.5f < position_.y && position_.y < 0.5f) {
						position_.y = 0.0f;
						speed_.y = 0.0f;
					}
				}
			}
		}
		{
			files_->at_rect().org = position_;
			if(left_.size() > 0) {
				files_->at_rect().org.x -= main_->get_rect().size.x;
			}
		}

		// path 文字列を設定
		{
			short ref = files_->get_rect().org.x;
			if(left_.size() > 0) ref += main_->get_rect().size.x;
			if(ref > (get_rect().size.x / 2)) {
				std::string np;
				if(utils::previous_path(param_.path_, np)) {
					path_->at_local_param().text_param_.text_ = np;
				}
			} else {
				path_->at_local_param().text_param_.text_ = param_.path_;
			}
		}

		// 選択されたファイルをハイライトする位置の検出
		BOOST_FOREACH(const widget_file& wf, left_) {
			wf.name->set_state(widget::state::SYSTEM_SELECT, false);
			wf.info->set_state(widget::state::SYSTEM_SELECT, false);
		}
		BOOST_FOREACH(const widget_file& wf, right_) {
			wf.name->set_state(widget::state::SYSTEM_SELECT, false);
			wf.info->set_state(widget::state::SYSTEM_SELECT, false);
		}
		widget_files_cit cit = scan_select_in_file_(center_);
		if(cit != center_.end()) {
			set_select_pos_(cit - center_.begin());
		} else if(scan_select_file_(center_) != center_.end()) {
			// 何もしない
		} else if(!files_->get_state(widget::state::DRAG)){
			if(select_pos_ < center_.size()) {
				widget_file& wf = center_[select_pos_];
				wf.name->set_state(widget::state::SYSTEM_SELECT);
				wf.info->set_state(widget::state::SYSTEM_SELECT);
			}
		}

		// 情報ボタンが押された場合の処理
		if(info_->get_selected()) {
			info_state_ = static_cast<info_state::type>(info_state_ + 1);
			if(info_state_ == info_state::limit_) {
				info_state_ = info_state::NONE;
			}
			update_files_info_(left_);
			update_files_info_(center_);
			update_files_info_(right_);
		}

		// 選択の確認と動作
		{
			widget_files_cit cit = scan_selected_file_(center_);
			if(cit != center_.end()) {
				const widget_file& wf = *cit;
				if(wf.name) {
					const std::string& n = wf.name->get_local_param().text_param_.text_;
					if(n == "..") {  // 一つ前に戻る
						request_right_ = false;
						move_speed_ =  speed_move;
					} else if(wf.dir) {
						request_right_ = true;
						move_speed_ = -speed_move;
						std::string ap;
						if(n.size() > 2 && 'A' <= n[0] && n[0] <= 'Z' && n[1] == ':') {
							param_.path_ = n;
						} else {
							utils::append_path(param_.path_, n, ap);
							utils::strip_last_of_delimita_path(ap, param_.path_);
						}
						fsc_.set_path(param_.path_, param_.filter_);
						destroy_files_(right_);
					} else {
						utils::append_path(param_.path_, n, file_);
						++select_file_id_;
						enable(false);
					}
				}
#if 0
			} else if(!files_->get_state(widget::state::DRAG)){
				short ref = files_->get_rect().org.x;
				if(left_.size() > 0) ref += main_->get_rect().size.x;
				if(ref > (get_rect().size.x / 2)) {
					move_speed_ = 30.0f;
				}
#endif
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_filer::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::mobj::attribute::normal, 0, 0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルをフォーカスする
		@param[in]	path	選択するファイルパス
		@return 該当するファイルが無い場合「false」
	*/
	//-----------------------------------------------------------------//
	bool widget_filer::focus_file(const std::string& path)
	{
		if(utils::probe_full_path(path)) {
			std::string root;
			if(utils::get_file_path(path, root)) {
				if(param_.path_ != root) return false;
			}
			const char* p = utils::get_file_name(path);
			if(p == 0) {
				return false;
			}
			focus_path_ = p;
		} else {
			focus_path_ = path;
		}

		return focus_(focus_path_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_filer::save(sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);
		int err = 0;
		if(!pre.put_text(path + "/current_path", param_.path_)) ++err;
		if(!pre.put_position(path + "/locate", vtx::ipos(get_rect().org))) ++err;
		if(!pre.put_position(path + "/size", vtx::ipos(get_rect().size))) ++err;
///		if(!pre.put_integer(key_info_, info_state_) ++err;
		return err == 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_filer::load(const sys::preference& pre)
	{
		std::string path;
		path += '/';
		path += wd_.create_widget_name(this);

		int err = 0;
		std::string s;
		if(pre.get_text(path + "/current_path", s)) {
			param_.path_ = s;
			fsc_.set_path(param_.path_, param_.filter_);
			destroy_files_(left_);
			destroy_files_(center_);
			destroy_files_(right_);
		} else {
			++err;
		}

		vtx::ipos p;
		if(pre.get_position(path + "/locate", p)) {
			at_rect().org = p;
		} else {
			++err;
		}
		if(pre.get_position(path + "/size", p)) {
			at_rect().size = p;
		} else {
			++err;
		}
#if 0
		int i;
		if(pre.get_integer(key_info_, i)) {
			info_state_ = static_cast<info_state::type>(i);
		} else {
			++err;
		}
#endif
		return err == 0;
	}
}
