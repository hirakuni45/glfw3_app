#pragma once
//=========================================================================//
/*!	@file
	@brief	pdfv メイン（GUI）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2022, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=========================================================================//
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
#include "widgets/widget_tree.hpp"
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

	struct pdf_base {
		struct pair_list_t {
			const char* str;
			float		factor;
			constexpr pair_list_t(const char* s, float f) : str(s), factor(f) { }
		};
	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  img メイン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class pdf_main : public utils::i_scene, pdf_base {

		static constexpr int CAPTION_HEIGHT = 28;

		static constexpr pair_list_t scale_list_[] = {
			{ "Fit", 0.0f },  // scale factor 0 is fit
			{ "100 %", 1.0f },
			{ "125 %", 1.25f },
			{ "150 %", 1.5f },
			{ "175 %", 1.75f },
			{ "200 %", 2.0f },
			{ "300 %", 3.0f },
		};

		static constexpr pair_list_t rotate_list_[] = {
			{ "0",     0.0f },
			{ "90",   90.0f },
			{ "180", 180.0f },
			{ "270", 270.0f },
			{ "45",   45.0f },
		};

		utils::director<core>&	director_;

		gui::widget_filer*		load_ctx_;

		gui::widget_frame*		tool_frame_;
		gui::widget_button*		load_;
		gui::widget_list*		scale_;
		gui::widget_spinbox*	page_;
		gui::widget_list*		rotate_;
		gui::widget_check*		term_;

		gui::widget_frame*		img_frame_;
		gui::widget_image*		img_core_;

		gui::widget_frame*		tree_frame_;
		gui::widget_tree*		tree_core_;

		gui::widget_frame*		term_frame_;
		gui::widget_terminal*	term_core_;

		gui::widget_dialog*		dialog_;
		gui::widget_dialog*		dialog_yes_no_;

		gl::mobj			mobj_;
		gl::mobj::handle	img_handle_;

		int				dd_id_;
		uint32_t		load_id_;

		vtx::fpos		image_offset_;

		img::shared_img	src_image_;

		std::string		start_path_;

		img::pdf_in		pdf_in_;
		bool			pdf_redraw_;

		typedef gui::widget_tree::tree_unit UNIT;


		void image_info_(const std::string& file, const img::i_img* img) noexcept
		{
			std::string s;
			if(!file.empty()) {
				size_t fsz = utils::get_file_size(file);
				if(fsz > 0) s = ": " + boost::lexical_cast<std::string>(fsz) + '\n';
				term_core_->output(s);
			}
			s = "W: " + boost::lexical_cast<std::string>(img->get_size().x) + '\n';
			term_core_->output(s);
			s = "H: " + boost::lexical_cast<std::string>(img->get_size().y) + '\n';
			term_core_->output(s);

			img::IMG::type t = img->get_type();
			if(t == img::IMG::INDEXED8) {
				term_core_->output("INDEXED8\n");
			} else if(t == img::IMG::FULL8) {
				term_core_->output("FULL8\n");
			}
			if(img->test_alpha()) {
				term_core_->output("Alpha\n");
			}
			s = "C: " + boost::lexical_cast<std::string>(img->count_color()) + '\n';
			term_core_->output(s);
		}


		void setup_src_image_() noexcept
		{
			image_offset_.set(0.0f);
			mobj_.destroy();
			mobj_.initialize();
			img_handle_ = mobj_.install(src_image_.get());
			img_core_->at_local_param().mobj_ = mobj_;
			img_core_->at_local_param().mobj_handle_ = img_handle_;

			const auto& ws = img_core_->get_rect().size;
			img_core_->at_local_param().offset_ = (ws - src_image_->get_size()) / 2;
		}


		std::string make_path_(const std::string base, int nest) noexcept
		{
			std::string t;
			t = base;
			int n = 1;
			while(n <= nest) {
				t += (boost::format("/%04d") % n).str();
				++n;
			}
			return t;
		}


		void create_outline_() noexcept
		{
			tree_core_->clear();
			auto& tu = tree_core_->at_tree_unit();
			int page = 1;
			for(auto t : pdf_in_.get_outlines()) {
				if(t->title != nullptr) {
					std::string path;
					path = (boost::format("/%05d") % page).str();
					++page;
					gui::widget_tree::value v;
					v.title_ = t->title;
					v.data_ = t->uri;
//					std::cout << t->uri << std::endl;
					tu.install(path, v);
					auto down = t->down;
					int nest = 1;
					while(down != nullptr) {
						gui::widget_tree::value v;
						v.title_ = down->title;
						v.data_ = down->uri;
						auto s = make_path_(path, nest);
						tu.install(s, v);
						down = down->down;
						++nest;
					}
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
			load_ctx_(nullptr),
			tool_frame_(nullptr),
			load_(nullptr), scale_(nullptr), page_(nullptr), rotate_(nullptr),
			term_(nullptr),
			img_frame_(nullptr), img_core_(nullptr),
			tree_frame_(nullptr), tree_core_(nullptr),
			term_frame_(nullptr), term_core_(nullptr),
			dialog_(nullptr), dialog_yes_no_(nullptr),
			img_handle_(0), dd_id_(0), load_id_(0),
			image_offset_(0.0f), src_image_(), start_path_(),
			pdf_in_(), pdf_redraw_(false)
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
		void initialize() noexcept override
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{ // 画像ファイル表示用フレーム
				widget::param wp(vtx::irect(315, 10, 256, 256));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(CAPTION_HEIGHT);
				img_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{ // 画像ファイル表示イメージ
				widget::param wp(vtx::irect(0, 0, 256, 256), img_frame_);
				widget_image::param wp_;
				img_core_ = wd.add_widget<widget_image>(wp, wp_);
				img_core_->set_state(widget::state::CLIP_PARENTS);
				img_core_->set_state(widget::state::RESIZE_ROOT);
				img_core_->set_state(widget::state::MOVE_ROOT, false);
				img_core_->set_state(widget::state::POSITION_LOCK, false);
			}

			{ // 機能ツールパレット
				widget::param wp(vtx::irect(10, 10, 300, 260));
				widget_frame::param wp_;
				tool_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				tool_frame_->set_state(widget::state::SIZE_LOCK);
			}
			{ // ロード起動ボタン
				widget::param wp(vtx::irect(10, 10+50*0, 100, 40), tool_frame_);
				widget_button::param wp_("load");
				load_ = wd.add_widget<widget_button>(wp, wp_);
				load_->at_local_param().select_func_ = [=](uint32_t id) {
					if(load_ctx_ != nullptr) {
						bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
						load_ctx_->enable(!f);
					}
				};
			}
			{  // スケール・リスト
				widget::param wp(vtx::irect(10, 10+50*1, 200, 40), tool_frame_);
				widget_list::param wp_(scale_list_[0].str);
				for(auto t : scale_list_) {
					wp_.init_list_.push_back(t.str);
				};
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					pdf_redraw_ = true;
				};
				scale_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // ページ操作
				widget::param wp(vtx::irect(10, 10+50*2, 200, 40), tool_frame_);
				widget_spinbox::param wp_(1, 1, 1);
				page_ = wd.add_widget<widget_spinbox>(wp, wp_);
				page_->at_local_param().select_func_ = [=](widget_spinbox::state st, int before, int newpos) {
					if(st == widget_spinbox::state::inc || st == widget_spinbox::state::dec) {
						pdf_redraw_ = true;
					}
					return (boost::format("%d / %d") % newpos % page_->get_select_max()).str();
				};
			}
			{  // 回転・リスト
				widget::param wp(vtx::irect(10, 10+50*3, 200, 40), tool_frame_);
				widget_list::param wp_(rotate_list_[0].str);
				for(auto t : rotate_list_) {
					wp_.init_list_.push_back(t.str);
				}
				wp_.select_func_ = [=](const std::string& text, uint32_t pos) {
					pdf_redraw_ = true;
				};
				rotate_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // ターミナル、有効、無効
				widget::param wp(vtx::irect(10, 10+50*4, 130, 40), tool_frame_);
				widget_check::param wp_("Terminal");
				term_ = wd.add_widget<widget_check>(wp, wp_);
			}

			{  // ツリー
				{
					widget::param wp(vtx::irect(10, 275, 300, 200));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(CAPTION_HEIGHT);
					tree_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), tree_frame_);
					widget_tree::param wp_;
					tree_core_ = wd.add_widget<widget_tree>(wp, wp_);
					tree_core_->at_local_param().select_func_ = [=] (UNIT::unit_map_it it) {

					};
				}
			}

			{ // ターミナル
				{
					widget::param wp(vtx::irect(10, 400, 9*14-8, 18*16+28));
					widget_frame::param wp_;
					wp_.plate_param_.set_caption(CAPTION_HEIGHT);
					term_frame_ = wd.add_widget<widget_frame>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(0), term_frame_);
					widget_terminal::param wp_;
					wp_.echo_ = false;
					term_core_ = wd.add_widget<widget_terminal>(wp, wp_);
				}
			}

			{ // load ファイラー本体
				widget::param wp(vtx::irect(10, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
				load_ctx_->enable(false);
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

			mobj_.initialize();

			pdf_in_.initialize();

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(load_ctx_ != nullptr) load_ctx_->load(pre);
			if(img_frame_ != nullptr) img_frame_->load(pre);
			if(tool_frame_ != nullptr) tool_frame_->load(pre, false, false);
			if(tree_frame_ != nullptr) tree_frame_->load(pre);
			if(term_frame_ != nullptr) term_frame_->load(pre);

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
		void update() noexcept override
		{
			gl::core& core = gl::core::get_instance();

			gui::widget_director& wd = director_.at().widget_director_;

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

			if(load_ctx_ != nullptr) {
				if(load_id_ != load_ctx_->get_select_file_id()) {
					load_id_ = load_ctx_->get_select_file_id();
					imfn = load_ctx_->get_file();
				}
			}

			// open PDF file:
			int redraw = 0;
			if(!imfn.empty()) {
				img::img_files& imf = wd.at_img_files();
				if(!pdf_in_.open(imfn)) {
					dialog_->set_text("Can't open PDF file:\n '"
								  + load_ctx_->get_file() + "'");
					dialog_->enable();
				} else {
					page_->set_select_max(pdf_in_.get_page_limit());
					create_outline_();
					++redraw;
					image_offset_ = vtx::fpos(0.0f);
				}
			}
			
			if(pdf_redraw_) {
				pdf_redraw_ = false;
				++redraw;
			}
			if(pdf_in_.is_document()) {  // フレーム上での操作
				if(img_frame_->get_focus()) {
					const auto& scr = wd.get_scroll();  // スクロールホイールでページを移動
					auto p = page_->get_select_pos();
					int d = 0;
					d = -scr.y;
					const auto& dev = core.get_device();
					if(dev.get_positive(gl::device::key::UP)) {
						d = 1;
					} else if(dev.get_positive(gl::device::key::DOWN)) {
						d = -1;
					}
					p += d;
					if(p < 1) p = 1;
					else if(p > pdf_in_.get_page_limit()) p = pdf_in_.get_page_limit();
					if(p != page_->get_select_pos()) {
						page_->set_select_pos(p);
						++redraw;
					}
				}
			}
			if(pdf_in_.is_document() && redraw > 0) {
				pdf_in_.set_page(page_->get_select_pos() - 1);
				img::pdf_in::area_t a(1.0f);
				const auto& t = scale_list_[scale_->get_select_pos()];
				if(t.factor == 0.0f) {
					a.atype = img::pdf_in::area_type::FIT;
					a.size = img_core_->at_rect().size;
				} else {
					a.atype = img::pdf_in::area_type::ZOOM;
					a.zoom = t.factor;
				}
				a.rotation = rotate_list_[rotate_->get_select_pos()].factor;
				if(pdf_in_.render(a)) {
					src_image_ = pdf_in_.get_image();
					term_core_->output("Ld: " + load_ctx_->get_file() + "\n");
					image_info_(load_ctx_->get_file(), src_image_.get());
					img_frame_->at_local_param().text_param_.set_text(imfn);
					setup_src_image_();
				}
			}

			{  // ターミナルの On/Off
				term_frame_->enable(term_->get_check());
			}

			// frame 内 image のサイズを設定
			if(img_frame_ != nullptr && img_core_ != nullptr) {
				if(img_core_->get_select_in()) {
					image_offset_ = img_core_->get_local_param().offset_;
				}
				if(img_core_->get_select()) {
					vtx::spos d = img_core_->get_param().move_pos_ - img_core_->get_param().move_org_;
					img_core_->at_local_param().offset_ = image_offset_ + d;
				}
			}

			load_->set_state(gui::widget::state::STALL, load_stall);

			wd.update();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render() noexcept override
		{
			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() noexcept override
		{
			sys::preference& pre = director_.at().preference_;
			if(load_ctx_ != nullptr) load_ctx_->save(pre);
			if(img_frame_ != nullptr) img_frame_->save(pre);
			if(tool_frame_ != nullptr) tool_frame_->save(pre);
			if(tree_frame_ != nullptr) tree_frame_->save(pre);
			if(term_frame_ != nullptr) term_frame_->save(pre);
		}
	};
}
