#pragma once
//=====================================================================//
/*! @file
	@brief  img メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_sheet.hpp"
#include "widgets/widget_utils.hpp"

#include "img_io/pdf_in.hpp"
#include "gl_fw/glmobj.hpp"

#include <tuple>
#include "core/glcore.hpp"
#include <boost/lexical_cast.hpp>

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  img メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pdf_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		load_ctx_;
		gui::widget_filer*		save_ctx_;

		gui::widget_frame*		frame_;
		gui::widget_image*		image_;

		gui::widget_frame*		tools_;
		gui::widget_button*		new_;
		gui::widget_button*		load_;
		gui::widget_button*		save_;

		gui::widget_sheet*		func_;
		gui::widget_spinbox*	view_;
		gui::widget_spinbox*	crop_ox_;
		gui::widget_spinbox*	crop_oy_;
		gui::widget_spinbox*	crop_sx_;
		gui::widget_spinbox*	crop_sy_;
		gui::widget_text*		crop_ex_;
		gui::widget_text*		crop_ey_;
		gui::widget_button*		crop_;

		gui::widget_frame*		info_;
		gui::widget_terminal*	term_;

		gui::widget_dialog*		dialog_;
		gui::widget_dialog*		dialog_yes_no_;
		gui::widget_dialog*		dialog_new_;

		gl::mobj			mobj_;
		gl::mobj::handle	img_handle_;

		int				dd_id_;
		uint32_t		load_id_;
		uint32_t		save_id_;

		vtx::fpos		image_offset_;

		img::shared_img	src_image_;

		std::string		save_file_name_;

//		std::future<bool>	image_saver_;

		bool			save_dialog_;

		std::string		start_path_;

		float			image_scale_;

		img::pdf_in		pdf_in_;

		typedef std::tuple<const std::string, const img::shared_img> save_t;


		bool save_task_(save_t t) noexcept
		{
			img::img_files imfs;
			imfs.set_image(std::get<1>(t));
			return imfs.save(std::get<0>(t));
		}


		void create_new_image_(const vtx::spos& size) noexcept
		{

		}


		void image_info_(const std::string& file, const img::i_img* img) noexcept
		{
			std::string s;
			if(!file.empty()) {
				size_t fsz = utils::get_file_size(file);
				if(fsz > 0) s = ": " + boost::lexical_cast<std::string>(fsz) + '\n';
				term_->output(s);
			}
			s = "W: " + boost::lexical_cast<std::string>(img->get_size().x) + '\n';
			term_->output(s);
			s = "H: " + boost::lexical_cast<std::string>(img->get_size().y) + '\n';
			term_->output(s);

			crop_ox_->at_local_param().max_pos_ = img->get_size().x;
			crop_ox_->set_select_pos(0);
			crop_oy_->at_local_param().max_pos_ = img->get_size().y;
			crop_oy_->set_select_pos(0);
			crop_sx_->at_local_param().max_pos_ = img->get_size().x;
			crop_sx_->set_select_pos(img->get_size().x);
			crop_sy_->at_local_param().max_pos_ = img->get_size().y;
			crop_sy_->set_select_pos(img->get_size().y);

			img::IMG::type t = img->get_type();
			if(t == img::IMG::INDEXED8) {
				term_->output("INDEXED8\n");
			} else if(t == img::IMG::FULL8) {
				term_->output("FULL8\n");
			}
			if(img->test_alpha()) {
				term_->output("Alpha\n");
			}
			s = "C: " + boost::lexical_cast<std::string>(img->count_color()) + '\n';
			term_->output(s);
		}


		gui::widget* init_view_(gui::widget_director& wd) noexcept
		{
			using namespace gui;

			widget::param wpr(vtx::irect(10, 20, 0, 0));
			widget_null::param wpr_;
			widget* view = wd.add_widget<widget_null>(wpr, wpr_);
			{
				widget::param wp(vtx::irect(0, 20, 70, 40), view);
				widget_text::param wp_;
				wp_.text_param_.set_text("Scale:");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				wd.add_widget<widget_text>(wp, wp_);
			}
			widget::param wp(vtx::irect(80, 20+50*0, 110, 40), view);
			widget_spinbox::param wp_(0, 0, 10);
			view_ = wd.add_widget<widget_spinbox>(wp, wp_);

			return view;
		}


		gui::widget* init_resize_(gui::widget_director& wd) noexcept
		{
			using namespace gui;

			return nullptr;
		}


		gui::widget* init_crop_(gui::widget_director& wd) noexcept
		{
			using namespace gui;

			widget::param wpr(vtx::irect(10, 20, 0, 0));
			widget_null::param wpr_;
			widget* crop = wd.add_widget<widget_null>(wpr, wpr_);
			{
				widget::param wp(vtx::irect(0, 20+50*0, 30, 40), crop);
				widget_text::param wp_;
				wp_.text_param_.set_text("X:");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(30, 20+50*0, 120, 40), crop);
				widget_spinbox::param wp_(0, 0, 512);
				wp_.page_step_ = 5;
				crop_ox_ = wd.add_widget<widget_spinbox>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 20+50*1, 30, 40), crop);
				widget_text::param wp_;
				wp_.text_param_.set_text("Y:");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
   	           	wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(30, 20+50*1, 120, 40), crop);
				widget_spinbox::param wp_(0, 0, 512);
				wp_.page_step_ = 5;
				crop_oy_ = wd.add_widget<widget_spinbox>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 20+50*2, 30, 40), crop);
				widget_text::param wp_;
				wp_.text_param_.set_text("W:");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
  	            	wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(30, 20+50*2, 120, 40), crop);
				widget_spinbox::param wp_(0, 512, 512);
				wp_.page_step_ = 5;
				crop_sx_ = wd.add_widget<widget_spinbox>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(160, 20+50*2, 60, 40), crop);
             	widget_text::param wp_;
				wp_.text_param_.set_text("512");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				crop_ex_ = wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0, 20+50*3, 30, 40), crop);
             	widget_text::param wp_;
				wp_.text_param_.set_text("H:");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(30, 20+50*3, 120, 40), crop);
				widget_spinbox::param wp_(0, 512, 512);
				wp_.page_step_ = 5;
				crop_sy_ = wd.add_widget<widget_spinbox>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(160, 20+50*3, 60, 40), crop);
             	widget_text::param wp_;
				wp_.text_param_.set_text("512");
				wp_.text_param_.placement_.vpt = vtx::placement::vertical::CENTER;
				crop_ey_ = wd.add_widget<widget_text>(wp, wp_);
			}
			{ // クロップボタン
				widget::param wp(vtx::irect(110, 20+50*4, 100, 40), crop);
				widget_button::param wp_("Crop");
				crop_ = wd.add_widget<widget_button>(wp, wp_);
				crop_->at_local_param().select_func_ = [=](uint32_t id) {
					auto src = src_image_.get();
					if(src == nullptr) return;

					vtx::spos org(crop_ox_->get_select_pos(), crop_oy_->get_select_pos());
					vtx::spos siz(crop_sx_->get_select_pos(), crop_sy_->get_select_pos());
					vtx::srect r(org, siz);					
					auto* dst = new img::img_rgba8;
					dst->create(r.size, src->test_alpha());
					img::copy_to_rgba8(src, r, *dst, vtx::spos(0));
					src_image_ = img::shared_img(dst);

					term_->output("Crop:\n");
					std::string s;
					image_info_(s, dst);

					setup_src_image_();
				};
			}
			return crop;
		}


		void cursor_render_(const vtx::irect& clip) noexcept
		{
			static uint16_t stp_idx = 0;
			static uint16_t stp_spd = 0;
			static const uint16_t stp_pat[4] = {
				0b1100110011001100,
				0b0110011001100110,
				0b0011001100110011,
				0b1001100110011001
			};

			gui::widget_director& wd = director_.at().widget_director_;

			if(func_->get_select_pos() == 2) {
				glDisable(GL_TEXTURE_2D);
				gl::glColor(wd.get_color());

				glPushMatrix();
				glScalef(image_scale_, image_scale_, image_scale_);
				gl::glTranslatei(
					image_->get_local_param().offset_.x,
					image_->get_local_param().offset_.y, 0);
				glLineStipple(4, stp_pat[stp_idx & 3]);
				stp_spd++;
				if(stp_spd >= 4) {
					stp_idx++;
					stp_spd = 0;
				}
				glEnable(GL_LINE_STIPPLE);
				glLineWidth(2.0f);
				vtx::spos org(crop_ox_->get_select_pos(), crop_oy_->get_select_pos());
				vtx::spos siz(crop_sx_->get_select_pos(), crop_sy_->get_select_pos());
				gl::draw_line_rectangle(org, siz);
				glDisable(GL_LINE_STIPPLE);
				glPopMatrix();

				glEnable(GL_TEXTURE_2D);
			}
        }


		void setup_src_image_() noexcept
		{
			image_offset_.set(0.0f);
			mobj_.destroy();
			mobj_.initialize();
			img_handle_ = mobj_.install(src_image_.get());
			image_->at_local_param().mobj_ = mobj_;
			image_->at_local_param().mobj_handle_ = img_handle_;
		}


		void update_func_() noexcept
		{
			gui::widget_director& wd = director_.at().widget_director_;

			if(func_->get_select_pos() == 2) {

				// クロップサイズの更新
				if(src_image_.get() != nullptr) {
					auto img = src_image_.get();
					int w = img->get_size().x - crop_ox_->get_select_pos();
					if(crop_sx_->get_select_pos() > w) {
						crop_sx_->set_select_pos(w);
					}
					int h = img->get_size().y - crop_oy_->get_select_pos();
					if(crop_sy_->get_select_pos() > h) {
						crop_sy_->set_select_pos(h);
					}

					int ex = crop_ox_->get_select_pos() + crop_sx_->get_select_pos();
					ex--;
					crop_ex_->set_text((boost::format("%d") % ex).str());
					int ey = crop_oy_->get_select_pos() + crop_sy_->get_select_pos();
					ey--;
					crop_ey_->set_text((boost::format("%d") % ey).str());
				}

				// クロップボタンの有効無効
				{
					bool ena = true;
					if(src_image_.get() == nullptr) ena = false;
					else {
						auto img = src_image_.get();
						if(crop_ox_->get_select_pos() == 0
							&& crop_oy_->get_select_pos() == 0
							&& crop_sx_->get_select_pos() == img->get_size().x
							&& crop_sy_->get_select_pos() == img->get_size().y) ena = false;
					}
					wd.stall(crop_, !ena);
				}
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
			@param[in]	d	シーン・ディレクターの参照
		*/
		//-----------------------------------------------------------------//
		pdf_main(utils::director<core>& d) noexcept :
			director_(d),
			load_ctx_(nullptr), save_ctx_(nullptr), 
			frame_(nullptr), image_(nullptr),
			tools_(nullptr), new_(nullptr), load_(nullptr), save_(nullptr),
			func_(nullptr),
			view_(nullptr),
			crop_ox_(nullptr), crop_oy_(nullptr), crop_sx_(nullptr), crop_sy_(nullptr),
			crop_ex_(nullptr), crop_ey_(nullptr), crop_(nullptr),
			info_(nullptr), term_(nullptr),
			dialog_(nullptr), dialog_yes_no_(nullptr), dialog_new_(nullptr),
			img_handle_(0), dd_id_(0), load_id_(0), save_id_(0),
			image_offset_(0.0f), src_image_(), save_dialog_(false), start_path_(),
			image_scale_(1.0f),
			pdf_in_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~pdf_main() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{ // 画像ファイル表示用フレーム
				widget::param wp(vtx::irect(30, 30, 256, 256));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(30);
				frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{ // 画像ファイル表示イメージ
				widget::param wp(vtx::irect(0, 0, 256, 256), frame_);
				widget_image::param wp_;
				wp_.render_func_ = [=](const vtx::irect& clip) {
					cursor_render_(clip);
				};
				image_ = wd.add_widget<widget_image>(wp, wp_);
				image_->set_state(widget::state::CLIP_PARENTS);
				image_->set_state(widget::state::RESIZE_ROOT);
				image_->set_state(widget::state::MOVE_ROOT, false);
				image_->set_state(widget::state::POSITION_LOCK, false);
			}

			{ // 機能ツールパレット
				widget::param wp(vtx::irect(10, 10, 250, 500));
				widget_frame::param wp_;
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(widget::state::SIZE_LOCK);
			}
			{ // 新規作成ボタン
				widget::param wp(vtx::irect(10, 10+50*0, 100, 40), tools_);
				widget_button::param wp_("new");
				new_ = wd.add_widget<widget_button>(wp, wp_);
			}
			{ // ロード起動ボタン
				widget::param wp(vtx::irect(10, 10+50*1, 100, 40), tools_);
				widget_button::param wp_("load");
				load_ = wd.add_widget<widget_button>(wp, wp_);
				load_->at_local_param().select_func_ = [=](uint32_t id) {
					if(load_ctx_ != nullptr) {
						bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
						load_ctx_->enable(!f);
					}
				};
			}

			{ // セーブ起動ボタン
				widget::param wp(vtx::irect(10, 10+50*2, 100, 40), tools_);
				widget_button::param wp_("save");
				save_ = wd.add_widget<widget_button>(wp, wp_);
				save_->at_local_param().select_func_ = [=](uint32_t id) {
					if(save_ctx_ != nullptr) {
						bool f = save_ctx_->get_state(gui::widget::state::ENABLE);
						save_ctx_->enable(!f);
					}
				};
			}

			short ofs = 160;
			{  // 機能選択
				widget* view = init_view_(wd);
				widget* resize;
				{  // resize sheet
                    widget::param wpr(vtx::irect(10, 20, 0, 0));
                    widget_null::param wpr_;
                    resize = wd.add_widget<widget_null>(wpr, wpr_);

				}
				widget* crop = init_crop_(wd);

                widget::param wp(vtx::irect(10, ofs, 230, 300), tools_);
                widget_sheet::param wp_;
                wp_.sheets_.emplace_back("View",   view);
                wp_.sheets_.emplace_back("Resize", resize);
                wp_.sheets_.emplace_back("Crop",   crop);
                func_ = wd.add_widget<widget_sheet>(wp, wp_);
			}


			{ // ターミナル
				{
					widget::param wp(vtx::irect(10, 320, 9*14-8, 18*16+28));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(20);
					info_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), info_);
					widget_terminal::param wp_;
					wp_.echo_ = false;
					term_ = wd.add_widget<widget_terminal>(wp, wp_);
				}
			}

			{ // load ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				load_ctx_->enable(false);
			}
			{ // save ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				wp_.new_file_ = true;
				save_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				save_ctx_->enable(false);
			}
			{ // ダイアログ
				widget::param wp(vtx::irect(10, 30, 450, 200));
				widget_dialog::param wp_;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
			}
			{ // ダイアログ(cancel/ok)
				widget::param wp(vtx::irect(10, 30, 450, 200));
				widget_dialog::param wp_(widget_dialog::style::CANCEL_OK);
				dialog_yes_no_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_yes_no_->enable(false);
			}
			{ // ダイアログ(new)
				widget::param wp(vtx::irect(10, 30, 450, 200));
				widget_dialog::param wp_(widget_dialog::style::CANCEL_OK);
				dialog_new_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_new_->enable(false);
			}

			mobj_.initialize();

			pdf_in_.initialize();

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(load_ctx_ != nullptr) load_ctx_->load(pre);
			if(save_ctx_ != nullptr) save_ctx_->load(pre);
			if(frame_ != nullptr) frame_->load(pre);
			if(tools_ != nullptr) tools_->load(pre, false, false);
			if(view_ != nullptr) view_->load(pre);
///			if(mode_ != nullptr) mode_->load(pre);
			if(info_ != nullptr) info_->load(pre);

			// コマンドラインの取得
			{
				auto cmds = core.get_command_path();
				for(auto s : cmds) {
					if(utils::probe_file(s)) {
						start_path_ = s;
						break;
					}
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update() override
		{
			gl::core& core = gl::core::get_instance();

			gui::widget_director& wd = director_.at().widget_director_;

			if(new_) {
				if(new_->get_selected()) {
					img::img_rgba8* img = new img::img_rgba8;
					img->create(vtx::spos(960, 720), true);
					img->fill(img::rgba8(0, 0, 0, 0));
					src_image_ = img::shared_img(img);
					term_->output("New\n");
					image_info_("new image", src_image_.get());
					image_offset_.set(0.0f);
					frame_->at_local_param().text_param_.set_text("new image");
					mobj_.destroy();
					mobj_.initialize();
					img_handle_ = mobj_.install(src_image_.get());
					image_->at_local_param().mobj_ = mobj_;
					image_->at_local_param().mobj_handle_ = img_handle_;
///					bool f = dialog_new_->get_state(gui::widget::state::ENABLE);
///					dialog_new_->enable(!f);
				}
			}

			std::string imfn;
			// アプリ起動コマンドラインより取得のパス
			if(!start_path_.empty()) {
				imfn = start_path_;
				start_path_.clear();
			}

			// D & D 取得のパス
			int id = core.get_recv_files_id();
			if(dd_id_ != id) {
				dd_id_ = id;
				const utils::strings& ss = core.get_recv_files_path();
				if(!ss.empty()) {
					imfn = ss.back();
				}
			}

			bool load_stall = false;
			bool save_stall = false;

			if(load_ctx_ != nullptr) {
				if(load_ctx_->get_state(gui::widget::state::ENABLE)) {
					save_stall = true;
				}
				if(load_id_ != load_ctx_->get_select_file_id()) {
					load_id_ = load_ctx_->get_select_file_id();
					imfn = load_ctx_->get_file();
				}
			}

			// open file:
			if(!imfn.empty()) {

				img::img_files& imf = wd.at_img_files();
				if(!pdf_in_.open(imfn)) {
					dialog_->set_text("Can't decode PDF file:\n '"
								  + load_ctx_->get_file() + "'");
					dialog_->enable();
				} else {
					pdf_in_.render(vtx::spos(512, 512));
std::cout << boost::format("PDF: %d pages") % pdf_in_.get_page_limit() << std::endl;
#if 0
					src_image_ = pdf_in_.get_image();
					term_->output("Ld: " + load_ctx_->get_file() + "\n");
					image_info_(load_ctx_->get_file(), src_image_.get());
					frame_->at_local_param().text_param_.set_text(imfn);
					setup_src_image_();
#endif
				}
#if 0
				if(!imf.load(imfn)) {
					dialog_->set_text("Can't decode image file:\n '"
								  + load_ctx_->get_file() + "'");
					dialog_->enable();
				} else {
					src_image_ = imf.get_image();
					term_->output("Ld: " + load_ctx_->get_file() + "\n");
					image_info_(load_ctx_->get_file(), src_image_.get());
					frame_->at_local_param().text_param_.set_text(imfn);
					setup_src_image_();
				}
#endif
			}

			// frame 内 image のサイズを設定
			if(frame_ != nullptr && image_ != nullptr) {
				if(!image_->get_local_param().mobj_handle_) {
					save_stall = true;
				}

				float s = 1.0f;
				auto n = view_->get_select_pos();
				if(n == 0) {
					vtx::fpos is = mobj_.get_size(img_handle_);
					vtx::fpos ss = image_->at_rect().size;
					vtx::fpos sc = ss / is;
					if(sc.x < sc.y) s = sc.x; else s = sc.y;
					image_->at_local_param().offset_ = 0.0f;
				} else {
					s = static_cast<float>(n);
					if(image_->get_select_in()) {
						image_offset_ = image_->get_local_param().offset_;
					}
					if(image_->get_select()) {
						vtx::spos d = image_->get_param().move_pos_ - image_->get_param().move_org_;
						image_->at_local_param().offset_ = image_offset_ + d / s;
					}
				}
				image_->at_local_param().scale_ = s;
				image_scale_ = s;
			}

			if(save_ctx_ != nullptr) {
				if(save_ctx_->get_state(gui::widget::state::ENABLE)) {
					load_stall = true;
				}
				if(save_id_ != save_ctx_->get_select_file_id()) {
					save_id_ = save_ctx_->get_select_file_id();
					const std::string& fn = save_ctx_->get_file();
					if(utils::probe_file(fn)) {
						dialog_yes_no_->set_text("Over write ?:\n'"
												 + fn + "'");
						dialog_yes_no_->enable();
						save_dialog_ = true;
					} else {
						save_dialog_ = false;
					}
					save_file_name_ = fn;
				}
			}

			load_->set_state(gui::widget::state::STALL, load_stall);
			save_->set_state(gui::widget::state::STALL, save_stall);

			update_func_();

			wd.update();

			// 画像ファイルのセーブタスク起動
			if(!save_file_name_.empty()) {
				save_t t = std::make_tuple(save_file_name_, src_image_);
				if(!save_task_(t)) {
					dialog_->set_text("Can't encode image file:\n'"
									  + save_file_name_ + "'");
					dialog_->enable();
				} else {
					term_->output("Sv");
					image_info_(save_file_name_, src_image_.get());
				}
				save_file_name_.clear();
#if 0
				if(image_saver_.valid()) {
					if(!image_saver_.get()) {
						dialog_->set_text("Can't encode image file:\n'"
									  + save_file_name_ + "'");
						dialog_->enable();
					} else {
						term_->output("Sv");
						image_info_(save_file_name_, src_image_.get());
					}
					save_file_name_.clear();
				} else {
					bool launch = false;
					if(save_dialog_) {
						if(!dialog_yes_no_->get_state(gui::widget::state::ENABLE)) {
							if(dialog_yes_no_->get_local_param().return_ok_) {
								launch = true;
							}
						}
					} else {
						launch = true;
					}
					if(launch) {
						save_t t = std::make_tuple(save_file_name_, src_image_);
						image_saver_ = std::async(std::launch::async, save_task_, t);
					}
				}
#endif
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() override
		{
			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override
		{
			sys::preference& pre = director_.at().preference_;
			if(load_ctx_ != nullptr) load_ctx_->save(pre);
			if(save_ctx_ != nullptr) save_ctx_->save(pre);
			if(frame_ != nullptr) frame_->save(pre);
			if(tools_ != nullptr) tools_->save(pre);
			if(view_ != nullptr) view_->save(pre);
///			if(mode_ != nullptr) mode_->save(pre);
			if(info_ != nullptr) info_->save(pre);
		}
	};

}
