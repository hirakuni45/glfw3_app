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

	/// ファイルのタグを読み出す
	std::string player::tag_server_(const std::string path)
	{
		std::string alias;

		alias = path;

		return alias;
	}


	void player::sound_play_(const std::string& file)
	{
		al::sound& sound = director_.at().sound_;

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
		widget_director& wd = director_.at().widget_director_;

		widget::param wp(vtx::srect(vtx::spos(0), size), 0);
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
		gl::core& core = gl::core::get_instance();

		mobj_.initialize();

		gl::fonts& fonts = core.at_fonts();
		std::string cf = fonts.get_font_type();
		std::string fp = core.get_current_path();
		fp += "/res/seg12.ttf";
		if(!fonts.install_font_type(fp, "led")) {
			std::cerr << "Can't install system TTF font: '" << fp << "'" << std::endl;
		}
		fonts.set_spaceing(6);
		fonts.set_font_type(cf);

		// サウンド・デコーダーの拡張子設定
		al::sound& sound = director_.at().sound_;
		tag_serial_ = sound.get_tag_stream().serial_;

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{	// ファイラーリソースの生成
			widget::param wp(vtx::srect(10, 10, 500, 350));
			widget_filer::param wp_(core.get_current_path(), sound.get_file_exts());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}

		const std::string& curp = core.get_current_path();
		file_btn_  = gui::create_image<widget_button>(wd, curp + "/res/select.png");
		play_btn_  = gui::create_image<widget_button>(wd, curp + "/res/play.png");
		pause_btn_ = gui::create_image<widget_button>(wd, curp + "/res/pause.png");

		rew_btn_   = gui::create_image<widget_button>(wd, curp + "/res/rew.png");
		ff_btn_    = gui::create_image<widget_button>(wd, curp + "/res/ff.png");

		short lw = 40;
		total_time_  = create_text_pad_(vtx::spos(16 * 3, lw), "00:00", "led", false);
		remain_time_ = create_text_pad_(vtx::spos(16 * 3, lw), "00:00", "led", false);
		{
			widget::param wp(vtx::srect(10, 10, 500, 16), 0);
			widget_slider::param wp_;
			if(wd.at_img_files().load(curp + "/res/seek_handle.png")) {
				wp_.hand_image_ = wd.at_img_files().get_image();
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
			widget::param wp(vtx::srect(20, 500, 200, 20), 0);
			widget_slider::param wp_;
			if(wd.at_img_files().load(curp + "/res/slider_handle.png")) {
				wp_.hand_image_ = wd.at_img_files().get_image();
			}
   			volume_ = wd.add_widget<widget_slider>(wp, wp_);
		}
		{
			widget::param wp(vtx::srect(0, 0, 0, 0), 0);
			widget_image::param wp_;
			if(wd.at_img_files().load(curp + "/res/piano.png")) {
				wp_.image_ = wd.at_img_files().get_image();
			}
   			vol_min_img_ = wd.add_widget<widget_image>(wp, wp_);
			if(wd.at_img_files().load(curp + "/res/forte.png")) {
				wp_.image_ = wd.at_img_files().get_image();
			}
   			vol_max_img_ = wd.add_widget<widget_image>(wp, wp_);
		}
		{
			widget::param wp(vtx::srect(0, 0, 120, 30), 0);
			widget_check::param wp_("Resume");
			resume_play_ = wd.add_widget<widget_check>(wp, wp_);
		}

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
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
			const vtx::spos& scs = core.get_rect().size;
			short w = 450;
			short h = 150;
			widget::param wp(vtx::srect((scs.x - w) / 2, (scs.y - h) / 2, w, h));
			widget_dialog::param wp_;
   			wp_.style_ = widget_dialog::param::style::OK;
			error_dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			const utils::strings& ss = wd.get_error_report();
			if(ss.empty()) {
				error_dialog_->enable(false);
			} else {
				std::string s;
				utils::strings_to_string(ss, true, s);
				error_dialog_->set_text(s);
				std::cout << s << std::endl;
			}
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
		al::sound& sound = director_.at().sound_;
		std::string state;
		if(sound.get_state_stream() == al::sound::stream_state::STALL) {
			wd.enable(play_btn_);
			play_btn_->set_state(gui::widget::state::STALL);
			rew_btn_->set_state(gui::widget::state::STALL);
			ff_btn_->set_state(gui::widget::state::STALL);
			state = " (stalled)";
		} else if(sound.get_state_stream() == al::sound::stream_state::PLAY) {
			play_btn_->set_state(gui::widget::state::STALL, false);
			rew_btn_->set_state(gui::widget::state::STALL, false);
			ff_btn_->set_state(gui::widget::state::STALL, false);
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

		// ファイラーボタンの確認
		if(filer_ && file_btn_->get_selected()) {
			bool f = filer_->get_state(gui::widget::state::ENABLE);
			filer_->enable(!f);
			if(!f) {
				filer_->focus_file(sound.get_file_stream());
				files_step_ = 0;
				files_.clear();
			}
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
			const img::i_img* im = tag.image_.get();
   			if(im) {
				jacket_ = mobj_.install(im);
			} else {
				const std::string& curp = core.get_current_path();
				if(wd.at_img_files().load(curp + "/res/NoImage.png")) {
					jacket_ = mobj_.install(wd.at_img_files().get_image()); 
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

		// ファイラーが有効で、マウス操作が無い状態が５秒続いたら、
		// 演奏ファイルパスへフォーカス
   		if(filer_->get_state(gui::widget::state::ENABLE)) {
			const vtx::spos& msp = core.get_device().get_locator().get_cursor();
			if(msp == mouse_pos_) {
				++filer_count_;
				if(filer_count_ >= (60 * 5)) {
					filer_->focus_file(sound.get_file_stream());
				}
			} else {
				filer_count_ = 0;
				mouse_pos_ = msp;
			}

			// ファイルのタグをファイラーのエイリアスに設定
#if 0
			if(files_.empty() && files_step_ == 0 && !filer_->get_file_infos().empty()) {
				filer_->get_file_list(files_);
			}
			if(!files_.empty() && files_step_ < files_.size()) {
				if(tag_future_.valid()) {
					std::cout << tag_future_.get() << std::endl;
					++files_step_;
				} else {
					tag_future_ = std::async(std::launch::async, tag_server_, files_[files_step_]);
				}
			}
#endif
		}

		// Drag & Drop されたファイルを再生
		int id = core.get_recv_file_id();
		if(drop_file_id_ != id) {
			drop_file_id_ = id;
			const utils::strings& ss = core.get_recv_file_path();
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
		gl::core& core = gl::core::get_instance();
///		const vtx::spos& vsz = core.get_size();
		const vtx::spos& siz = core.get_rect().size;

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
		sys::preference& pre = director_.at().preference_;

		// remain time
		al::sound& sound = director_.at().sound_;
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
