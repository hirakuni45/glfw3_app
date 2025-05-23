//=====================================================================//
/*!	@file
	@brief	GUI Widget ファイラー
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "widgets/widget_utils.hpp"
#include "widgets/widget_filer.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <utility>

namespace gui {

	static const std::string new_file_text_ = "New File...";
	static const float def_gain = 0.85f;
	static const float slip_gain = 0.5f;
	static const float speed_gain = 0.95f;
	static const float speed_move = 38.0f;	/// 横スクロールの初期速度

	void widget_filer::create_file_(widget_file& wf, const vtx::irect& rect, int ofs, const std::string& str)
	{
		{
			widget::param wp(rect, files_);
			widget_null::param wp_;
			wf.base = wd_.add_widget<widget_null>(wp, wp_);
			wf.base->set_state(widget::state::POSITION_LOCK);
			wf.base->set_state(widget::state::SIZE_LOCK);
			wf.base->set_state(widget::state::CLIP_PARENTS);
			wf.base->set_state(widget::state::DRAG_UNSELECT);
			wf.base->set_state(widget::state::FOCUS_CHILDS);
			wf.base->set_state(widget::state::SELECT_CHILDS);
		}
		short fns = rect.size.x * 2 / 3;
		short ats = rect.size.x - fns - 2;
		{
			vtx::irect r;
			r.org.set(0);
			r.size.set(fns, param_.label_height_);
			widget::param wp(r, wf.base);
			wp.action_.set(widget::action::SELECT_HIGHLIGHT);
			widget_label::param wp_(str);
			wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
			wp_.plate_param_.frame_width_ = 0;
			wp_.plate_param_.round_radius_ = 0;
			wp_.plate_param_.resizeble_ = true;
			wf.name = wd_.add_widget<widget_label>(wp, wp_);
			wf.name->set_state(widget::state::POSITION_LOCK);
			wf.name->set_state(widget::state::CLIP_PARENTS);
			wf.name->set_state(widget::state::SELECT_PARENTS);
			wf.name->set_state(widget::state::MOVE_STALL, false);
		}
		{
			vtx::irect r;
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
			wf.info->set_state(widget::state::SELECT_PARENTS);
			wf.info->set_state(widget::state::MOVE_STALL, false);
		}
	}


	void widget_filer::create_files_(widget_files& wfs, int ofs)
	{
		wfs.resize(file_infos_.size() + drv_.get_num() + (param_.new_file_ ? 1 : 0));
		wfs.clear();

		// ラベルを新規に作成
		vtx::irect rect;
		rect.org.set(ofs, 0);
		rect.size.x = get_rect().size.x - (param_.plate_param_.frame_width_ * 2);
		rect.size.y = param_.label_height_;

		// ルートパスならドライブレターを加える
		std::string pp = utils::previous_path(fsc_path_);
		if(pp.empty()) {
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

		// 新規ファイルタグ
		if(param_.new_file_) {
			widget_file wf;
			wf.dir = true;
			create_file_(wf, rect, ofs, new_file_text_);
			wf.name->at_local_param().read_only_ = false;
			wf.name->at_local_param().select_func_ = [this](const std::string& filename) {
				if(filename == "..") {

				} else if(filename.back() == '/') {  // for make directory
					auto path = utils::strip_last_of_delimita_path(filename);
					utils::create_directory(utils::append_path(param_.path_, path));
					center_update_ = true;
				} else {
					file_ = utils::append_path(param_.path_, filename);
					++select_file_id_;
					enable(false);
					if(param_.select_file_func_ != nullptr) {
						param_.select_file_func_(file_);
					}
				}
			};

			wf.size = 0;
			wf.time = 0;
			wf.mode = 0;

			wfs.push_back(wf);
			rect.org.y += param_.label_height_;
		}

		for(const auto& fi : file_infos_) {
			std::string fn = fi.get_name();
			if(fn == ".") continue;

			widget_file wf;
			if(fn == "..") {
				if(pp.empty()) continue;
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


	void widget_filer::un_selected_(widget_files& wfs)
	{
		for(auto& wf : wfs) {
			wf.name->set_state(widget::state::SELECTED, false);
			wf.info->set_state(widget::state::SELECTED, false);
		}
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
			if(info_state_ == info_state::NONE || info_state_ == info_state::ALIAS) {
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
				if(wf.dir) {
					wf.info->at_local_param().text_param_.set_text(" -");
				} else {
					wf.info->at_local_param().text_param_.set_text(" " + boost::lexical_cast<std::string>(wf.size));
				}
			} else if(info_state_ == info_state::TIME) {
				std::string s;
				s += ' ';
				struct tm* t = localtime(&wf.time);
				s += (boost::format("%02d:%02d ") % t->tm_hour % t->tm_min).str();
				s += (boost::format("%d/%d ")
					% (t->tm_mon + 1) % t->tm_mday).str();
				s += (boost::format("%4d") % (t->tm_year + 1900)).str();
				wf.info->at_local_param().text_param_.set_text(s);
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
				wf.info->at_local_param().text_param_.set_text(s);
			}
		}
	}


	void widget_filer::update_files_alias_(widget_files& wfs)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(param_.new_file_ && cit == wfs.begin()) continue;
			if(!wf.name) continue;
			if(wf.size == 0 && wf.time == 0 && wf.mode == 0) continue;
			if(wf.name->get_text() == "..") continue;
			bool f = info_state_ == info_state::ALIAS;
			if(wf.name->get_alias().empty()) f = false;
			wf.name->enable_alias(f);
		}
	}


	void widget_filer::destroy_files_(widget_files& wfs)
	{
		// ラベル郡を破棄
		for(const auto& wf : wfs) {
			wd_.del_widget(wf.info);
			wd_.del_widget(wf.name);
			wd_.del_widget(wf.base);
		}
		wfs.clear();
	}


	void widget_filer::get_regist_state_()
	{
		// 選択位置の回復
		file_map::iterator it = file_map_.find(param_.path_);
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
		file_map::iterator it = file_map_.find(param_.path_);
		if(it == file_map_.end()) {
			file_t t;
			it = file_map_.insert(file_map::value_type(param_.path_, t)).first;
		}
		file_t& f = it->second;
		f.position_ = position_;
		f.select_pos_ = select_pos_;
	}


	void widget_filer::set_select_pos_(uint32_t pos)
	{
		un_selected_(center_);
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
	}


	widget_filer::widget_file_copt widget_filer::scan_item_(const std::string& fn) const
	{
		auto path = utils::strip_last_of_delimita_path(fn);

		for(const auto& wf : center_) {
			auto t = utils::strip_last_of_delimita_path(wf.name->get_text());
			if(wf.name && t == path) {
				return widget_file_copt(wf);
			}
		}
		return widget_file_copt();
	}


	bool widget_filer::focus_(const std::string& path)
	{
		auto fn = utils::strip_last_of_delimita_path(path);
		uint32_t n = 0;
		for(const auto& wf : center_) {
			auto t = utils::strip_last_of_delimita_path(wf.name->get_text());
			if(wf.name && !t.empty() && t == fn) {
				// センターリング
				short lh = param_.label_height_;
				short len = main_->get_rect().size.y / lh;
				short ofs = static_cast<short>(n) - (len / 2);
				if(ofs >= 0) {
					if(center_.size() > len) {
						if((center_.size() * lh - main_->get_rect().size.y / 2) > n * lh) {
							position_.y = static_cast<float>(ofs * -lh);
						}
					}
				}
				if(wf.name->get_state(widget::state::SELECTED)) {
					return true;
				}
				set_select_pos_(n);
				wf.name->set_state(widget::state::SELECTED);
				wf.info->set_state(widget::state::SELECTED);
				return true;
			}
			++n;
		}
		return false;
	}


	std::string widget_filer::make_path_(const std::string path)
	{
		std::string fin;
		if(utils::probe_full_path(path)) {
			auto root = utils::get_file_path(path);
			if(param_.path_ != root) {
				return fin;
			}
			auto s = utils::get_file_name(path);
			if(s.empty()) {
				return fin;
			}
			fin = s;
		} else {
			fin = path;
		}
		return fin;
	}


	void widget_filer::select_path_(const std::string& n)
	{
		// 移動中は無視！
		if(move_speed_ != 0.0f) return;

		if(n == "..") {  // 一つ前に戻る
			request_right_ = false;
			move_speed_ =  speed_move;
		} else if(n.back() == '/') {
			request_right_ = true;
			move_speed_ = -speed_move;
			std::string ap;
			if(n.size() > 2 && 'A' <= n[0] && n[0] <= 'Z' && n[1] == ':') {
				param_.path_ = n;
			} else {
				ap = utils::append_path(param_.path_, n);
				param_.path_ = utils::strip_last_of_delimita_path(ap);
			}
			file_infos_.clear();
			fsc_path_.clear();
			fsc_.set_path(param_.path_, param_.filter_);
			fsc_wait_ = true;
			destroy_files_(right_);
		} else {
			file_ = utils::append_path(param_.path_, n);
			++select_file_id_;
			enable(false);
			if(param_.select_file_func_ != nullptr) {
				param_.select_file_func_(file_);
			}
		}
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
		at_param().state_.set(widget::state::RESIZE_EDGE_ENABLE);
		at_param().state_.set(widget::state::SERVICE);
		at_param().state_.set(widget::state::ENABLE, false);

		param_.plate_param_.set_caption(param_.label_height_);
		param_.plate_param_.resizeble_ = true;
		at_param().resize_min_ = param_.plate_param_.grid_ * 3;
		at_param().resize_min_.x += 32;
		at_param().resize_min_.x += 32;

		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

		file_infos_.clear();
		fsc_path_.clear();
		fsc_.set_path(param_.path_, param_.filter_);
		fsc_wait_ = true;

		short frame_width = param_.plate_param_.frame_width_;
		// info ボタン
		{
			gl::mobj::handle hnd = wd_.get_share_image().right_box_;
			vtx::irect r;
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
			int fw = 4;
			vtx::irect r(vtx::ipos(frame_width, frame_width + param_.path_height_),
				vtx::ipos(get_rect().size.x - 8,
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
			vtx::irect r(vtx::ipos(0), get_rect().size);
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
		param_.shift_param_.size_ = get_rect().size.x - param_.plate_param_.frame_width_ * 2;
		shift_text_update(get_param(), param_.text_param_, param_.shift_param_);

		if(param_.every_top_) {
			wd_.top_widget(this);
		}

		if(!center_.empty()) {
			// 「new file」がキャンセルされた場合に文字列を再設定
			widget_files_cit cit = scan_select_in_file_(center_);
			if(param_.new_file_ && cit != center_.end()) {
				if(cit == center_.begin()) {
					center_[0].name->set_text("");
				} else {
					center_[0].name->set_text(new_file_text_);
				}
			}

			// アクセレーターキーの設定
			// new_file にフォーカスがある場合は、キーの設定をしない
			acc_key_ = 0;
			if(param_.new_file_ && wd_.get_focus_widget() == (*cit).base) ;
			else if(wd_.get_focus_widget() == this || wd_.get_focus_widget() == wd_.root_widget(this)) {
				const utils::lstring& ins = wd_.get_keyboard().input();
				if(!ins.empty()) {
					auto ch = ins.back();
					if(ch >= 'a' && ch <= 'z') ch -= 0x20;
					if(ch == 0x1b || ch == '\r' ||
					   ch == sys::keyboard::CTRL::UP || ch == sys::keyboard::CTRL::DOWN) {
						acc_key_ = ch;
						acc_cnt_ = 0;
					} else if((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z')) {
						acc_key_ = ch;
						if(acc_key_ch_ != ch) acc_cnt_ = 0;
						else ++acc_cnt_;
						acc_key_ch_ = ch;
					} else {
						acc_cnt_ = 0;
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
	void widget_filer::service()
	{
		if(!get_state(widget::state::ENABLE)) {
			return;
		}

		// センターコンテキストを作り直す
		if(center_update_) {
			destroy_files_(center_);
			fsc_wait_ = true;
			center_update_ = false;
		}

		// ファイル情報の取得と反映（ファイル情報収集はスレッドで動作）
		if(fsc_wait_ && fsc_.probe()) {
			fsc_path_ = fsc_.get_path();
			file_infos_ = fsc_.get();
///			std::cout << "File infos: " << static_cast<unsigned int>(file_infos_.size()) << std::endl;
///			for(auto i : file_infos_) {
///				std::cout << i.get_name() << std::endl;
///			}
			fsc_wait_ = false;

			if(center_.empty()) {
				create_files_(center_, 0);
				update_files_info_(center_);
				if(left_.empty()) {
					std::string pp = utils::previous_path(param_.path_);
					if(!pp.empty()) {
						file_infos_.clear();
						fsc_path_.clear();
						fsc_.set_path(pp);
						fsc_wait_ = true;
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

		// アクセレーターキー操作
		if(param_.acc_focus_ && acc_key_ && !center_.empty()) {
			if(acc_key_ >= ' ') {
				utils::strings ss;
				for(const auto& wf : center_) {
					char ch = wf.name->get_text()[0];
					if(ch >= 'a' && ch <= 'z') ch -= 0x20;
					if(ch == acc_key_) {
						ss.push_back(wf.name->get_text());
					}
				}
				if(!ss.empty()) {
					focus_path_ = ss[acc_cnt_ % ss.size()];
					focus_(focus_path_);
				}
			} else if(acc_key_ == 0x1b) { // ESC
				if(param_.cancel_file_func_ != nullptr) {
					param_.cancel_file_func_();
				}
				enable(false);
			} else if(acc_key_ == '\r') { // Enter (CR)
				for(const auto& wf : center_) {
					if(wf.name->get_text() == focus_path_) {
						select_path_(focus_path_);
						break;
					}
				}
			} else if(acc_key_ == sys::keyboard::CTRL::UP || acc_key_ == sys::keyboard::CTRL::DOWN) {
				if(focus_path_.empty()) {
					focus_path_ = center_[0].name->get_text();
					focus_(focus_path_);
				} else {
					uint32_t n = 0;
					for(const auto& wf : center_) {
						if(wf.name->get_text() == focus_path_) {
							break;
						}
						++n;
					}
					if(acc_key_ == sys::keyboard::CTRL::UP) --n;
					else ++n;
					if(n < center_.size()) {
						focus_path_ = center_[n].name->get_text();
						focus_(focus_path_);
					}
				}
			}
			acc_key_ = 0;
		}

		// フレームのサイズを、仮想ウィジェットに反映
		{
			short fw = param_.plate_param_.frame_width_;
			const vtx::ipos& size = get_rect().size;
			short bw = size.x - fw * 2;
			main_->at_rect().size.x = bw;
			main_->at_rect().size.y = size.y - param_.path_height_ - fw * 2;
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

		int base_size = main_->get_rect().size.y;
		int d = base_size - center_.size() * param_.label_height_;

		// ドラッグアクションで、一つ前に戻る
		if(back_directory_ && files_->get_select_out()) {
			request_right_ = false;
			move_speed_ =  speed_move;
			back_directory_ = false;
		}

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
				std::string pp = utils::previous_path(param_.path_);
				if(!pp.empty()) {
					std::string tmp = param_.path_;
					param_.path_ = pp;
					destroy_files_(center_);
					center_.swap(left_);
					focus_file(tmp);
					pp = utils::previous_path(param_.path_);
					if(!pp.empty()) {
						file_infos_.clear();
						fsc_path_.clear();
						fsc_.set_path(pp, param_.filter_);
						fsc_wait_ = true;
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
			if(!files_->get_state(state::BEFORE_DRAG)) {
				position_ = files_->get_param().move_pos_;
				if(left_.size() > 0) {
					position_.x += main_->get_rect().size.x;
				}
			}
			if(d < 0) {
				if(position_.y < d) {
/// std::cout << "slip+: " << d << std::endl;
					position_.y -= d;
					position_.y *= slip_gain;
					position_.y += d;
				} else if(position_.y > 0) {
/// std::cout << "slip-" << std::endl;
					position_.y *= slip_gain;
				}
			} else {
/// std::cout << "slip+-" << std::endl;
				position_.y *= slip_gain;
			}
		} else {
			if(files_->get_state(state::BEFORE_DRAG) && !files_->get_state(state::DRAG) && left_.size() > 0) {
				short ref = files_->get_rect().org.x;
				ref += main_->get_rect().size.x;
				if(ref > (get_rect().size.x / 3)) {
					request_right_ = false;
					move_speed_ =  speed_move;
				}
			}

			if(files_->get_select_out()) {
				speed_ = files_->get_param().speed_;
			}

			position_ += speed_;
			speed_ *= speed_gain;
			position_.x *= def_gain;
			if(-0.5f < position_.x && position_.x < 0.5f) {
				position_.x = 0.0f;
				speed_.x = 0;
			}

			// y 軸クリップ
			{
				if(d < 0) {
					if(position_.y < d) {
						position_.y -= d;
						position_.y *= def_gain;
						position_.y += d;
						speed_.y = 0.0f;
						if(position_.y > (d - 0.5f)) {
							position_.y = d;
						}
					} else if(position_.y > 0.0f) {
						position_.y *= def_gain;
						speed_.y = 0.0f;
						if(position_.y < 0.5f) {
							position_.y = 0.0f;
						}
					} else {
						const vtx::ipos& scr = wd_.get_scroll();
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
					position_.y *= def_gain;
					if(-0.5f < position_.y && position_.y < 0.5f) {
						position_.y = 0.0f;
						speed_.y = 0.0f;
					}
				}
			}
		}

		files_->at_rect().org = position_;
		if(left_.size() > 0) {
			files_->at_rect().org.x -= main_->get_rect().size.x;
		}

		// path 文字列を設定
		{
			short ref = files_->get_rect().org.x;
			if(left_.size() > 0) ref += main_->get_rect().size.x;
			if(ref > (get_rect().size.x / 3)) {
				param_.text_param_.set_text(utils::previous_path(param_.path_));
				back_directory_ = true;
			} else {
				param_.text_param_.set_text(param_.path_);
				back_directory_ = false;
			}
		}

		// 情報ボタンが押された場合の処理
		if(info_->get_selected()) {
			info_state_ = static_cast<info_state::type>(info_state_ + 1);
			if(info_state_ == info_state::limit_) {
				info_state_ = info_state::NONE;
			}
			update_files_info_(left_);
			update_files_alias_(left_);
			update_files_info_(center_);
			update_files_alias_(center_);
			update_files_info_(right_);
			update_files_alias_(right_);
		}

		// 選択の確認と動作
		if(!center_.empty()) {
			widget_files_cit cit = scan_selected_file_(center_);
			if(cit == center_.end()) return;

			const widget_file& wf = *cit;
			if(param_.new_file_ && cit == center_.begin()) ;
			else if(wf.name) {
				select_path_(wf.name->get_text());
			}
		}
	}
}
