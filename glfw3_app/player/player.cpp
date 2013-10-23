//=====================================================================//
/*! @file
	@brief  player クラス
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

namespace app {

	static const char* resume_path_ = { "/player/resume" };
	static const char* volume_path_ = { "/player/volume" };
	static const char* remain_pos_path_ = { "/player/remain/position" };
	static const char* remain_file_path_ = { "/player/remain/file" };


	void player::sound_play_(const std::string& file)
	{
		al::sound& sound = director_.at_core().sound_;

		std::string path;
		utils::get_file_path(file, path);
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
		if(tt) {
			pa.text_param_.text_  = boost::lexical_cast<std::string>(tt / 10);
			pa.text_param_.text_ += boost::lexical_cast<std::string>(tt % 10);
			pa.text_param_.text_ += ':';
		} else {
			pa.text_param_.text_.clear();
		}
		tt = t / 60;
		pa.text_param_.text_ += boost::lexical_cast<std::string>(tt / 10);
		pa.text_param_.text_ += boost::lexical_cast<std::string>(tt % 10);
		pa.text_param_.text_ += ':';
		tt = t % 60;
		pa.text_param_.text_ += boost::lexical_cast<std::string>(tt / 10);
		pa.text_param_.text_ += boost::lexical_cast<std::string>(tt % 10);
	}


	gui::widget_label* player::create_text_pad_(const vtx::spos& size,
		const std::string& text, const std::string& font, bool proportional)
	{
		using namespace gui;
		widget_director& wd = director_.at_core().widget_director_;

		widget::param wp(vtx::srect(vtx::spos(0), size), 0);
		widget_label::param wp_(text);
		wp_.color_param_ = widget_director::default_slider_color_;
		wp_.color_param_select_ = widget_director::default_slider_color_;
		wp_.text_param_.font_ = font;
		wp_.text_param_.proportional_ = proportional;
		wp_.text_param_.placement_.hpt = vtx::placement::holizontal::CENTER;
		wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
		wp_.plate_param_.resizeble_ = true;
		wp_.shift_every_ = true;
		return wd.add_widget<widget_label>(wp, wp_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void player::initialize()
	{
		gl::IGLcore* igl = gl::get_glcore();

		mobj_.initialize();

		gl::fonts& fonts = igl->at_fonts();
		std::string cf = fonts.get_font_type();
		if(!fonts.install_font_type("./res/seg12.ttf", "led")) {
			std::cerr << "Can't install system TTF font..." << std::endl;
		}
		fonts.set_font_type(cf);

		// サウンド・デコーダーの拡張子設定
		al::sound& sound = director_.at_core().sound_;
		tag_serial_ = sound.get_tag_stream().serial_;

		using namespace gui;
		widget_director& wd = director_.at_core().widget_director_;

		{	// ファイラーリソースの生成
			widget::param wp(vtx::srect(10, 10, 500, 350));
			widget_filer::param wp_(igl->get_current_path(), sound.get_file_exts());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}

		file_btn_  = gui::create_image<widget_button>(wd, "res/select.png");
		play_btn_  = gui::create_image<widget_button>(wd, "res/play.png");
		pause_btn_ = gui::create_image<widget_button>(wd, "res/pause.png");

		rew_btn_   = gui::create_image<widget_button>(wd, "res/rew.png");
		ff_btn_    = gui::create_image<widget_button>(wd, "res/ff.png");

		short lw = 40;
		total_time_  = create_text_pad_(vtx::spos(16 * 3, lw), "00:00", "led", false);
		remain_time_ = create_text_pad_(vtx::spos(16 * 3, lw), "00:00", "led", false);
		{
			widget::param wp(vtx::srect(10, 10, 500, 16), 0);
			widget_slider::param wp_;
			if(wd.at_img_files().load("res/seek_handle.png")) {
				wp_.hand_image_ = wd.at_img_files().get_image_if();
			}
			wp_.plate_param_.resizeble_ = true;
			wp_.hand_ctrl_ = false;
			seek_time_ = wd.add_widget<widget_slider>(wp, wp_);
		}

		album_pad_  = create_text_pad_(vtx::spos(16 * 3, lw), "");
		title_pad_  = create_text_pad_(vtx::spos(16 * 3, lw), "");
		artist_pad_ = create_text_pad_(vtx::spos(16 * 3, lw), "");
		other_pad_  = create_text_pad_(vtx::spos(16 * 3, lw), "");

		{
			widget::param wp(vtx::srect(20, 500, 200, 20), 0);
			widget_slider::param wp_;
			if(wd.at_img_files().load("res/slider_handle.png")) {
				wp_.hand_image_ = wd.at_img_files().get_image_if();
			}
   			volume_ = wd.add_widget<widget_slider>(wp, wp_);
		}
		{
			widget::param wp(vtx::srect(0, 0, 0, 0), 0);
			widget_image::param wp_;
			if(wd.at_img_files().load("res/piano.png")) {
				wp_.image_ = wd.at_img_files().get_image_if();
			}
   			vol_min_img_ = wd.add_widget<widget_image>(wp, wp_);
			if(wd.at_img_files().load("res/forte.png")) {
				wp_.image_ = wd.at_img_files().get_image_if();
			}
   			vol_max_img_ = wd.add_widget<widget_image>(wp, wp_);
		}
		{
			widget::param wp(vtx::srect(0, 0, 120, 30), 0);
			widget_check::param wp_("Resume");
			resume_play_ = wd.add_widget<widget_check>(wp, wp_);
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at_core().preference_;
		pre.get_boolean(resume_path_, resume_play_->at_local_param().check_);
		if(resume_play_->at_local_param().check_) {
			int pos = 0;
			pre.get_integer(remain_pos_path_, pos);
			std::string file;
			pre.get_text(remain_file_path_, file);
			std::string path;
			utils::get_file_path(file, path);
			if(!file.empty() && !path.empty()) {
				sound.play_stream(path, utils::get_file_name(file));
				sound.seek_stream(static_cast<size_t>(pos));
			}
		}
		pre.get_real(volume_path_, volume_->at_slider_param().position_);
		if(filer_) {
			filer_->load(pre);
		}

		// エラー用ダイアログリソースの生成
		{
			const vtx::spos& scs = igl->get_size();
			short w = 450;
			short h = 150;
			widget::param wp(vtx::srect((scs.x - w) / 2, (scs.y - h) / 2, w, h));
			widget_dialog::param wp_;
			error_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			error_dialog_->enable(false);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void player::update()
	{
		gl::IGLcore* igl = gl::get_glcore();

		const vtx::spos& size = igl->get_size();

		// ウィジェットのコア、更新
		gui::widget_director& wd = director_.at_core().widget_director_;
		wd.update();

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
		seek_time_->at_rect().org.x   = padsz + padspc * 2;
		short ofs = total_time_->at_rect().size.y - seek_time_->at_rect().size.y;
		seek_time_->at_rect().org.y   = pady + ofs / 2;
		seek_time_->at_rect().size.x  = size.x - padsz * 2 - padspc * 4;

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
		al::sound& sound = director_.at_core().sound_;
		if(sound.get_state_stream() == al::sound::stream_state::STALL) {
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL);
			rew_btn_->set_state(gui::widget::state::STALL);
			ff_btn_->set_state(gui::widget::state::STALL);
		} else if(sound.get_state_stream() == al::sound::stream_state::PLAY) {
			play_btn_->set_state(gui::widget::state::STALL, false);
			rew_btn_->set_state(gui::widget::state::STALL, false);
			ff_btn_->set_state(gui::widget::state::STALL, false);
			wd.enable(pause_btn_);
			if(pause_btn_->get_selected()) {
				sound.pause_stream();
			}

			igl->set_title(sound.get_file_stream());

		} else if(sound.get_state_stream() == al::sound::stream_state::PAUSE) {
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL, false);
			rew_btn_->set_state(gui::widget::state::STALL, false);
			ff_btn_->set_state(gui::widget::state::STALL, false);
			if(play_btn_->get_selected()) {
				sound.pause_stream(false);
			}
		} else {	// to stop.
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL);
		}

		// ファイラーボタンの確認
		if(filer_ && file_btn_->get_selected()) {
			bool f = filer_->get_state(gui::widget::state::ENABLE);
			filer_->enable(!f);
		}

		// 「送り」ボタン
		if(ff_btn_->get_selected()) {
			sound.next_stream();
		}
		// 「戻り」ボタン
		if(rew_btn_->get_selected()) {
			// 開始５秒以降なら、曲の先頭に～
			if(sound.get_time_stream() < seek_change_time_) {
				sound.prior_stream();
			} else {
				sound.replay_stream();
			}
		}

		// 時間表示
		time_t t = remain_t_;
		remain_t_ = sound.get_time_stream();
		if(remain_t_ != t) {
			frame_limit_ = frame_count_;
			frame_count_ = 0;
		} else {
			if(sound.get_state_stream() == al::sound::stream_state::PLAY) {
				++frame_count_;
			}
		}
		t = total_t_;
		total_t_ = sound.get_end_time_stream();
		if(t != total_t_) frame_count_ = 0;
		if(sound.get_state_stream() == al::sound::stream_state::STALL) {
			frame_count_ = 0;
			total_t_ = 0;
			remain_t_ = 0;
		}
		set_time_(total_time_, total_t_);
		set_time_(remain_time_, remain_t_);
		{
			gui::widget::slider_param& sp = seek_time_->at_slider_param();
			if(total_t_) {
				float limit = static_cast<float>(frame_limit_);
				float t = static_cast<float>(remain_t_) * limit;
				t += static_cast<float>(frame_count_);
				sp.position_ = t / (static_cast<float>(total_t_) * limit);
			} else {
				sp.position_ = 0.0f;
				frame_count_ = 0;
			}
		}

		// 曲の情報を取得して表示
		const al::tag& tag = sound.get_tag_stream();
		if(tag_serial_ != tag.serial_) {

			gui::set_widget_text(album_pad_, tag.album_);
			gui::set_widget_text(title_pad_, tag.title_);
			std::string s = tag.track_;
			if(!s.empty()) {
				if(!tag.total_tracks_.empty()) {
					s += " / " + tag.total_tracks_;
				}
			}
			if(!tag.disc_.empty()) {
				s += " : " + tag.disc_;
				if(!tag.total_discs_.empty()) {
					s += " / " + tag.total_discs_;
				}
			}
			if(!tag.date_.empty()) {
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
   			if(tag.image_) {
				jacket_ = mobj_.install(tag.image_);
			} else {
				if(wd.at_img_files().load("res/NoImage.png")) {
					jacket_ = mobj_.install(wd.at_img_files().get_image_if()); 
				}
			}

			// 演奏ファイル名を取得して、ファイラーのフォーカスを設定
			{ 
				filer_->focus_file(sound.get_file_stream());
			}
			tag_serial_ = tag.serial_;
		}

		// ストリームのゲイン(volume)を設定
		sound.set_gain_stream(volume_->get_slider_param().position_);

		// ファイラーがファイル選択をした場合
		if(filer_->get_select_file_id() != select_file_id_) {
			select_file_id_ = filer_->get_select_file_id();
			const std::string& file = filer_->get_file();
			sound_play_(file);
		}

		// ファイラーが有効で、マウス操作が無い状態が５秒続いたら、演奏ファイルへフォーカス
   		if(filer_->get_state(gui::widget::state::ENABLE)) {
			const vtx::spos& msp = igl->get_device().get_locator().cursor_;
			if(msp == mouse_pos_) {
				++filer_count_;
				if(filer_count_ >= (60 * 5)) {
					filer_->focus_file(sound.get_file_stream());
				}
			} else {
				filer_count_ = 0;
				mouse_pos_ = msp;
			}
		}

		// Drag & Drop されたファイルを再生
		int id = igl->get_recv_file_id();
		if(drop_file_id_ != id) {
			drop_file_id_ = id;
			const utils::strings& ss = igl->get_recv_file_path();
			if(!ss.empty()) {
				std::string file = ss[0];
				if(ss.size() > 1) {
					if(file > ss.back()) file = ss.back();
				}
				sound_play_(file);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void player::render()
	{
		gl::IGLcore* igl = gl::get_glcore();
		const vtx::spos& size = igl->get_size();

		if(jacket_) {			
			mobj_.setup_matrix(size.x, size.y);
			float refs = static_cast<float>(size.x) * 0.5f;
			float scale = refs / static_cast<float>(mobj_.get_size(jacket_).x);
			glScalef(scale, scale, scale);
			float sci = 1.0f / scale;
			float ofsx = (size.x * 0.5f) / 2 * sci;
			float ofsy = 10.0f * sci;
			mobj_.draw(jacket_, gl::mobj::normal, ofsx, ofsy);
			mobj_.restore_matrix();
		}

		director_.at_core().widget_director_.render();

		director_.at_core().widget_director_.service();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void player::destroy()
	{
		sys::preference& pre = director_.at_core().preference_;

		// remain time
		al::sound& sound = director_.at_core().sound_;
		int po = 0;
		std::string fn;
		if(sound.get_state_stream() == al::sound::stream_state::PLAY ||
			sound.get_state_stream() == al::sound::stream_state::PAUSE) {
			po = static_cast<int>(sound.get_position_stream());
			fn = sound.get_file_stream();
		}
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
