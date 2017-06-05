//=====================================================================//
/*! @file
	@brief  player クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/lexical_cast.hpp>
#include "player.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_image.hpp"
#include "snd_io/snd_files.hpp"

namespace app {

	static const char* resume_path_ = { "/player/resume" };
	static const char* volume_path_ = { "/player/volume" };
	static const char* remain_pos_path_  = { "/player/remain/position" };
	static const char* remain_type_path_ = { "/player/remain/type" };
	static const char* remain_file_path_ = { "/player/remain/file" };

	void player::sound_play_(const std::string& file)
	{
		al::sound& sound = director_.at().sound_;

		auto path = utils::get_file_path(file);
		if(!sound.play_stream(path, utils::get_file_name(file))) {
			std::string s = "Can't open input file:  " + path + '/' + file;
			if(error_dialog_) {
				error_dialog_->set_text(s);
				error_dialog_->enable();
			}
		}
	}


	static void set_time_(gui::widget_label* w, time_t t)
	{
		gui::widget_label::param& pa = w->at_local_param();
		time_t tt = t / 3600;
		std::string s;
		if(tt) {
			s  = boost::lexical_cast<std::string>(tt / 10);
			s += boost::lexical_cast<std::string>(tt % 10);
			s += ':';
		}
		tt = t / 60;
		s += boost::lexical_cast<std::string>(tt / 10);
		s += boost::lexical_cast<std::string>(tt % 10);
		s += ':';
		tt = t % 60;
		s += boost::lexical_cast<std::string>(tt / 10);
		s += boost::lexical_cast<std::string>(tt % 10);
		pa.text_param_.set_text(s);
	}


	gui::widget_label* player::create_text_pad_(const vtx::spos& size,
		const std::string& text, const std::string& font, bool proportional)
	{
		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		widget::param wp(vtx::irect(vtx::ipos(0), size), 0);
		widget_label::param wp_(text);
		wp_.color_param_ = widget_director::default_slider_color_;
		wp_.color_param_select_ = widget_director::default_slider_color_;
		if(!font.empty()) {
			wp_.text_param_.font_ = font;
		}
		wp_.text_param_.proportional_ = proportional;
		wp_.text_param_.placement_.hpt = vtx::placement::holizontal::CENTER;
		if(font.empty()) wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
		else wp_.text_param_.placement_.vpt = vtx::placement::vertical::BOTTOM;
		wp_.plate_param_.resizeble_ = true;
		wp_.shift_param_.every_ = true;
		wp_.shift_param_.org_wait_frame_ = 60 * 4;
		return wd.add_widget<widget_label>(wp, wp_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void player::initialize()
	{
		auto& core = gl::core::get_instance();

		mobj_.initialize();

		auto& fonts = core.at_fonts();
		auto cf = fonts.get_font_type();
		auto fp = core.get_current_path();
		fp += "/res/seg12.ttf";
		if(!fonts.install_font_type(fp, "led")) {
			std::cerr << "Can't install player's TTF font: '" << fp << "'" << std::endl;
		}
		fonts.set_spaceing(6);
		fonts.set_font_type(cf);

		// サウンド・デコーダーの拡張子設定
		auto& sound = director_.at().sound_;
		tag_serial_ = sound.get_tag_stream().serial_;

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{	// ファイラーリソースの生成
			widget::param wp(vtx::irect(10, 10, 500, 350));
			widget_filer::param wp_(core.get_current_path(), sound.get_file_exts());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
			filer_->at_local_param().select_file_func_ = [this](const std::string& file) {
			   	sound_play_(file);
			};
		}

		// ファイラー起動ボタン（ファイル選択）
		const std::string& curp = core.get_current_path();
		file_btn_  = gui::create_image<widget_button>(wd, curp + "/res/select.png");
		file_btn_->at_local_param().select_func_ = [this](int id) {
			bool f = filer_->get_state(gui::widget::state::ENABLE);
			filer_->enable(!f);
			if(!f) {
				auto& sound = director_.at().sound_;
				filer_->focus_file(sound.get_file_stream());
				files_step_ = 0;
				files_.clear();
			}
		};

		play_btn_  = gui::create_image<widget_button>(wd, curp + "/res/play.png");

		pause_btn_ = gui::create_image<widget_button>(wd, curp + "/res/pause.png");

		rew_btn_   = gui::create_image<widget_button>(wd, curp + "/res/rew.png");
		rew_btn_->at_local_param().select_func_ = [this](int id) {
			al::sound& sound = director_.at().sound_;
			// 開始５秒以降なら、曲の先頭に～
			if(sound.get_time_stream() < seek_change_time_) {
				sound.prior_stream();
			} else {
				sound.replay_stream();
			}
		};

		ff_btn_    = gui::create_image<widget_button>(wd, curp + "/res/ff.png");
		ff_btn_->at_local_param().select_func_ = [this](int id) {
			al::sound& sound = director_.at().sound_;
			sound.next_stream();
		};

		short lw = 40;
		total_time_  = create_text_pad_(vtx::spos(16 * 3, lw), "00:00", "led", false);
		remain_time_ = create_text_pad_(vtx::spos(16 * 3, lw), "00:00", "led", false);
		{
			widget::param wp(vtx::irect(10, 10, 500, 16), 0);
			widget_slider::param wp_;
			if(wd.at_img_files().load(curp + "/res/seek_handle.png")) {
				wp_.hand_image_ = wd.at_img_files().get_image().get();
			}
			wp_.plate_param_.resizeble_ = true;
			wp_.hand_ctrl_ = true;
			seek_handle_ = wd.add_widget<widget_slider>(wp, wp_);
		}

		album_pad_  = create_text_pad_(vtx::spos(16 * 3, lw), "");
		title_pad_  = create_text_pad_(vtx::spos(16 * 3, lw), "");
		artist_pad_ = create_text_pad_(vtx::spos(16 * 3, lw), "");
		other_pad_  = create_text_pad_(vtx::spos(16 * 3, lw), "");

		{
			widget::param wp(vtx::irect(20, 500, 200, 20), 0);
			widget_slider::param wp_;
			if(wd.at_img_files().load(curp + "/res/slider_handle.png")) {
				wp_.hand_image_ = wd.at_img_files().get_image().get();
			}
   			volume_ = wd.add_widget<widget_slider>(wp, wp_);
			volume_->at_local_param().select_func_ = [this](float pos) {
				// ストリームのゲイン(volume)を設定
				al::sound& sound = director_.at().sound_;
				sound.set_gain_stream(pos);
			};
		}
		{
			widget::param wp(vtx::irect(0, 0, 0, 0), 0);
			widget_image::param wp_;
			if(wd.at_img_files().load(curp + "/res/piano.png")) {
				wp_.image_ = wd.at_img_files().get_image().get();
			}
   			vol_min_img_ = wd.add_widget<widget_image>(wp, wp_);
			if(wd.at_img_files().load(curp + "/res/forte.png")) {
				wp_.image_ = wd.at_img_files().get_image().get();
			}
   			vol_max_img_ = wd.add_widget<widget_image>(wp, wp_);
		}
		{
			widget::param wp(vtx::irect(0, 0, 120, 30), 0);
			widget_check::param wp_("Resume");
			resume_play_ = wd.add_widget<widget_check>(wp, wp_);
		}

		// エラー用ダイアログリソースの生成
		{
			const auto& scs = core.get_rect().size;
			short w = 450;
			short h = 150;
			widget::param wp(vtx::irect((scs.x - w) / 2, (scs.y - h) / 2, w, h));
			widget_dialog::param wp_;
   			wp_.style_ = widget_dialog::param::style::OK;
			error_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			const utils::strings& ss = wd.get_error_report();
			if(ss.empty()) {
				error_dialog_->enable(false);
			} else {
				std::string s = utils::strings_to_string(ss, true);
				error_dialog_->set_text(s);
				std::cout << s << std::endl;
			}
		}

		// プリファレンスの取得
		auto& pre = director_.at().preference_;
		pre.get_boolean(resume_path_, resume_play_->at_local_param().check_);
		if(resume_play_->at_local_param().check_) {
			int pos = 0;
			pre.get_integer(remain_pos_path_, pos);
			std::string file;
			pre.get_text(remain_file_path_, file);
			auto path = utils::get_file_path(file);
			int type;
			pre.get_integer(remain_type_path_, type);
			al::sound::stream_state::type t = static_cast<al::sound::stream_state::type>(type);
			if(!file.empty() && !path.empty()) {
				if(t == al::sound::stream_state::PAUSE) {
					sound.play_stream(path, utils::get_file_name(file));
					sound.pause_stream();
					sound.seek_stream(static_cast<size_t>(pos));
				} else if(t == al::sound::stream_state::PLAY) {
					sound.play_stream(path, utils::get_file_name(file));
					sound.seek_stream(static_cast<size_t>(pos));
				}
			}
		}
		{
			float pos;
			pre.get_real(volume_path_, pos);
			volume_->at_slider_param().position_ = pos;
			sound.set_gain_stream(pos);
		}
		if(filer_) {
			filer_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void player::update()
	{
		gl::core& core = gl::core::get_instance();

		const vtx::spos& size = core.get_rect().size;

		// ウィジェットのコア、更新
		gui::widget_director& wd = director_.at().widget_director_;

		// 画面のサイズに合わせた動的な位置補正
		short btw = 64;
		short btofs = btw / 2;
		short btspc = 10;
		short cenx = size.x / 2;
		short ofsy = size.y - btw - btspc;
		file_btn_-> at_rect().org.set(btspc, ofsy);
		rew_btn_->  at_rect().org.set(cenx - btofs - btspc - btw, ofsy);
		play_btn_-> at_rect().org.set(cenx - btofs, ofsy);
		pause_btn_->at_rect().org.set(cenx - btofs, ofsy);
		ff_btn_->   at_rect().org.set(cenx - btofs + btspc + btw, ofsy);

		short padh = album_pad_->get_rect().size.y; 
		short pady = size.y - btspc - btw - btspc - padh;
		short padspc = 10;
		short padsz = (size.x - padspc * 3) / 2;
		album_pad_->at_rect().size.x = padsz;
		album_pad_->at_rect().org.x  = padspc;
		album_pad_->at_rect().org.y  = pady;
		title_pad_->at_rect().size.x = padsz;
		title_pad_->at_rect().org.x  = size.x - padsz - padspc;
		title_pad_->at_rect().org.y  = pady;
		pady -= 10 + padh;
		other_pad_->at_rect().size.x = padsz;
		other_pad_->at_rect().org.x  = padspc;
		other_pad_->at_rect().org.y  = pady;
		artist_pad_->at_rect().size.x = padsz;
		artist_pad_->at_rect().org.x  = size.x - padsz - padspc;
		artist_pad_->at_rect().org.y  = pady;
		pady -= 10 + padh;
		padsz = 120;
		remain_time_->at_rect().size.x = padsz;
		remain_time_->at_rect().org.x = padspc;
		remain_time_->at_rect().org.y = pady;
		total_time_->at_rect().size.x = padsz;
		total_time_->at_rect().org.x  = size.x - padsz - padspc;
		total_time_->at_rect().org.y  = pady;
		seek_handle_->at_rect().org.x   = padsz + padspc * 2;
		short ofs = total_time_->at_rect().size.y - seek_handle_->at_rect().size.y;
		seek_handle_->at_rect().org.y   = pady + ofs / 2;
		seek_handle_->at_rect().size.x  = size.x - padsz * 2 - padspc * 4;

		short vcen = size.x - (size.x / 2 + btw / 2 + btspc + btw);
		volume_->at_rect().org.x = size.x - vcen / 2 - volume_->get_rect().size.x / 2;
		short btofsy = size.y - btspc - btw + btw / 2;
		volume_->at_rect().org.y = btofsy - volume_->get_rect().size.y / 2;
		vol_min_img_->at_rect().org.x = volume_->get_rect().org.x - 10 -
			vol_min_img_->at_rect().size.x;
		vol_min_img_->at_rect().org.y = btofsy - vol_min_img_->get_rect().size.y / 2;
		vol_max_img_->at_rect().org.x = volume_->get_rect().end().x + 10;
		vol_max_img_->at_rect().org.y = btofsy - vol_max_img_->get_rect().size.y / 2;

		resume_play_->at_rect().org.x = size.x / 4
			- (resume_play_->get_rect().size.x / 2);
		resume_play_->at_rect().org.y = btofsy - resume_play_->get_rect().size.y / 2;

		// ボタンの状態を設定
		wd.enable(play_btn_, false);
		wd.enable(pause_btn_, false);
		auto& sound = director_.at().sound_;
		std::string state;
		if(sound.get_state_stream() == al::sound::stream_state::STALL) {
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL);
			rew_btn_->set_state(gui::widget::state::STALL);
			ff_btn_->set_state(gui::widget::state::STALL);
			seek_handle_->set_state(gui::widget::state::STALL);
			state = " (stalled)";
		} else if(sound.get_state_stream() == al::sound::stream_state::PLAY) {
			play_btn_->set_state(gui::widget::state::STALL, false);
			rew_btn_->set_state(gui::widget::state::STALL, false);
			ff_btn_->set_state(gui::widget::state::STALL, false);
			seek_handle_->set_state(gui::widget::state::STALL, false);
			wd.enable(pause_btn_);
			if(pause_btn_->get_selected()) {
				sound.pause_stream();
			}
			state = " (playing)";
		} else if(sound.get_state_stream() == al::sound::stream_state::PAUSE) {
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL, false);
			rew_btn_->set_state(gui::widget::state::STALL, false);
			ff_btn_->set_state(gui::widget::state::STALL, false);
			seek_handle_->set_state(gui::widget::state::STALL, false);
			if(play_btn_->get_selected()) {
				sound.pause_stream(false);
			}
			state = " (pause)";
		} else {	// to stop.
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL);
			state = " (stoped)";
		}
		core.set_title(sound.get_file_stream() + state);

		// 時間表示
		remain_t_ = sound.get_time_stream();
		total_t_ = sound.get_end_time_stream();
		if(sound.get_state_stream() == al::sound::stream_state::STALL) {
			total_t_ = 0;
			remain_t_ = 0;
		}
		set_time_(total_time_, total_t_);
		if(seek_handle_->get_select()) {
			const gui::widget::slider_param& sp = seek_handle_->get_slider_param();
			remain_t_ = sp.position_ * sound.get_end_time_stream();
		}
		set_time_(remain_time_, remain_t_);

		// シークハンドルの操作と応答
		float stream_length = static_cast<float>(sound.get_length_stream());
		if(seek_handle_->get_select_in()) {
			const gui::widget::slider_param& sp = seek_handle_->get_slider_param();
			seek_pos_ = sp.position_ * stream_length;
		}
		if(seek_handle_->get_select_out()) {
			const gui::widget::slider_param& sp = seek_handle_->get_slider_param();
			size_t pos = sp.position_ * stream_length;
			sound.seek_stream(pos);
		} else if(seek_handle_->get_select()) {
			const gui::widget::slider_param& sp = seek_handle_->get_slider_param();
			uint32_t new_pos = sp.position_ * stream_length;
			if(seek_pos_ != new_pos) {
				seek_pos_ = new_pos;
				sound.seek_stream(seek_pos_);
			}
		} else {
			gui::widget::slider_param& sp = seek_handle_->at_slider_param();
			if(total_t_) {
				float pos = static_cast<float>(sound.get_position_stream());
				sp.position_ = pos / static_cast<float>(sound.get_length_stream());
			} else {
				sp.position_ = 0.0f;
			}
		}

		// 曲の情報を取得して表示
		const al::tag& tag = sound.get_tag_stream();
		if(tag_serial_ != tag.serial_) {

			gui::set_widget_text(album_pad_, tag.album_);
			gui::set_widget_text(title_pad_, tag.title_);
			std::string s = tag.track_;
			if(s == "0") s.clear();
			if(!s.empty()) {
				if(!tag.total_tracks_.empty()) {
					s += " / " + tag.total_tracks_;
				}
			}
			if(!tag.disc_.empty() && tag.disc_ != "0") {
				s += " : " + tag.disc_;
				if(!tag.total_discs_.empty()) {
					s += " / " + tag.total_discs_;
				}
			}
			if(!tag.date_.empty() && tag.date_ != "0") {
				s += " : " + tag.date_;
			}
			gui::set_widget_text(other_pad_, s);
			s = tag.artist_;
			if(!tag.writer_.empty()) {
				s += " / " + tag.writer_;
			}
			gui::set_widget_text(artist_pad_, s);

			// ジャケット画像の取得とテクスチャーへの登録
			mobj_.destroy();
			mobj_.initialize();
			img::shared_img sig = tag.decode_image();
   			if(sig) {
				const img::i_img* im = sig.get();
				jacket_ = mobj_.install(im);
			} else {
				const std::string& curp = core.get_current_path();
				if(wd.at_img_files().load(curp + "/res/NoImage.png")) {
					jacket_ = mobj_.install(wd.at_img_files().get_image().get()); 
				}
			}
			// 演奏ファイル名を取得して、ファイラーのフォーカスを設定
			{ 
				filer_->focus_file(sound.get_file_stream());
			}
			tag_serial_ = tag.serial_;
		}

		// ファイラーが有効で、マウス操作が無い状態が５秒続いたら、
		// ※スクロール・ダイアルの操作
		// 演奏ファイルパスへフォーカスする
   		if(filer_->get_state(gui::widget::state::ENABLE)) {
			const vtx::spos& msp = core.get_device().get_locator().get_cursor();
			const vtx::spos& scr = core.get_device().get_locator().get_scroll();
			if(msp == mouse_pos_ && scr == mouse_scr_) {
				++filer_count_;
				if(filer_count_ >= (60 * 5)) {
					filer_->focus_file(sound.get_file_stream());
				}
			} else {
				filer_count_ = 0;
				mouse_pos_ = msp;
				mouse_scr_ = scr;
			}

			// ファイルのタグ情報をファイラーのエイリアスに設定
			if(filer_->get_file_state()) {
				files_.clear();
				files_step_ = 0;
			} else {
				if(files_.empty()) {
					if(!sound.state_tag_info()) {
						files_ = filer_->get_file_list();
						tag_info_serial_ = sound.get_tag_info().serial_;
						tag_wait_ = false;
					}
				} else if(files_step_ < files_.size()) {
					const al::tag& t = sound.get_tag_info();
					if(t.serial_ == tag_info_serial_) {
						if(!tag_wait_) {
							const std::string fn = files_[files_step_];
							sound.request_tag_info(fn);
							tag_wait_ = true;
						}
					} else {
						tag_wait_ = false;
						tag_info_serial_ = t.serial_;
						using namespace std;
						const string p = utils::get_file_name(files_[files_step_]);
						// 75% 一致しない場合
						if(utils::compare(p, t.title_) < 0.75f) {
							if(t.title_.empty()) {
								filer_->set_alias(p, p);
							} if(!t.track_.empty()) {
								string n;
								auto pos = t.track_.find('/');
								if(pos != string::npos) {
									n = t.track_.substr(0, pos);
								} else {
									n = t.track_;
								}
								if(n.size() == 1) {
									if(n[0] == '0') n.clear();
									else if(n[0] >= '0' && n[0] <= '9') {
										n = '0' + n;
									}
								}
								if(!n.empty()) n += ' ';
								filer_->set_alias(p, n + t.title_);
							} else {
								filer_->set_alias(p, t.title_);
							}
						}
						++files_step_;
					}
				}
			}
		}

		// Drag & Drop されたファイルを再生
		int id = core.get_recv_files_id();
		if(drop_file_id_ != id) {
			drop_file_id_ = id;
			const utils::strings& ss = core.get_recv_files_path();
			if(!ss.empty()) {
				std::string file = ss[0];
				if(ss.size() > 1) {
					if(file > ss.back()) file = ss.back();
				}
				sound_play_(file);
			}
		}

		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void player::render()
	{
		auto& core = gl::core::get_instance();
		const auto& siz = core.get_rect().size;

		if(jacket_) {			
			mobj_.setup_matrix(siz.x, siz.y);
			float refs = static_cast<float>(siz.x) * 0.5f;
			float scale = refs / static_cast<float>(mobj_.get_size(jacket_).x);
			glScalef(scale, scale, scale);
			float sci = 1.0f / scale;
			float ofsx = (siz.x * 0.5f) / 2 * sci;
			float ofsy = 10.0f * sci;
			mobj_.draw(jacket_, gl::mobj::attribute::normal, vtx::spos(ofsx, ofsy));
			mobj_.restore_matrix();
		}

		director_.at().widget_director_.render();

		director_.at().widget_director_.service();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void player::destroy()
	{
		auto& pre = director_.at().preference_;

		// remain time
		auto& sound = director_.at().sound_;
		int po = 0;
		std::string fn;
		if(sound.get_state_stream() == al::sound::stream_state::PLAY ||
			sound.get_state_stream() == al::sound::stream_state::PAUSE) {
			po = static_cast<int>(sound.get_position_stream());
			fn = sound.get_file_stream();
		}
		pre.put_integer(remain_type_path_, sound.get_state_stream());
		pre.put_integer(remain_pos_path_, po);
		pre.put_text(remain_file_path_, fn);

		// Resume check box
		pre.put_boolean(resume_path_, resume_play_->get_local_param().check_);
		// volume slider
		pre.put_real(volume_path_, volume_->get_local_param().slider_param_.position_);

		if(filer_) {
			filer_->save(pre);
		}
	}
}
