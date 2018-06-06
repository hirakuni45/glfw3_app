//=====================================================================//
/*! @file
	@brief  PMD main クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_tree.hpp"
#include "widgets/widget_terminal.hpp"
#include "mdf/pmd_io.hpp"
#include "mdf/pmx_io.hpp"
#include "gl_fw/glcamera.hpp"
#include "gl_fw/gllight.hpp"

namespace app {

	class pmdv_main : public utils::i_scene {

		utils::director<core>&	director_;

		gui::widget_filer*		filer_;
		uint32_t	filer_id_;

		gui::widget_frame*		tools_;
		gui::widget_button*		fopen_;
		gui::widget_check*		grid_;
		gui::widget_check*		body_;
		gui::widget_check*		bone_;

		gui::widget_frame*		tree_frame_;
		gui::widget_tree*		tree_;

		gui::widget_frame*		terminal_frame_;
		gui::widget_terminal*	terminal_;

		mdf::pmd_io		pmd_io_;
		mdf::pmx_io		pmx_io_;
		bool			pmx_enable_;

		gl::camera		camera_;
		gl::light		light_;
		gl::light::handle	bone_light_;

		void info_pmd_()
		{
			using namespace gui;
			using namespace mdf;

			tree_->clear();

			widget_tree::tree_unit& tu = tree_->at_tree_unit();
			/// シリアルIDはクリアしない

			tu.make_directory("/bone");
			tu.set_current_path("/bone");

			for(uint32_t i = 0; i < pmd_io_.get_bones().size(); ++i) {
				std::string path;
				pmd_io_.create_bone_path(i, path);
				if(path.empty()) continue;
// std::cout << path << std::endl;
				widget_tree::value v;
				v.data_ = "0";			
				tu.install(&path[1], v);
			}

//			tu.make_directory("/bone_disp");
		}

		void info_pmx_()
		{
			using namespace gui;
			using namespace mdf;

			tree_->clear();

			widget_tree::tree_unit& tu = tree_->at_tree_unit();
			/// シリアルIDはクリアしない

			tu.make_directory("/bone");
			tu.set_current_path("/bone");

			for(uint32_t i = 0; i < pmx_io_.get_bones().size(); ++i) {
				std::string path;
				pmx_io_.create_bone_path(i, path);
				widget_tree::value v;
				v.data_ = "0";			
				tu.install(&path[1], v);
			}
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		pmdv_main(utils::director<core>& d) :
			director_(d),
			filer_(0), filer_id_(0),
			tools_(0), fopen_(0), grid_(0), body_(0), bone_(0),
			tree_frame_(0), tree_(0),
			terminal_frame_(0), terminal_(0),
			pmd_io_(), pmx_io_(), pmx_enable_(false),
			bone_light_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~pmdv_main() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			gl::core& core = gl::core::get_instance();

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{	// ファイラー・リソース
				widget::param wp(vtx::irect(30, 30, 300, 200));
				widget_filer::param wp_(core.get_current_path());
				filer_ = wd.add_widget<widget_filer>(wp, wp_);
				filer_->enable(false);
			}
			{	// ツールパレット
				widget::param wp(vtx::irect(20, 20, 150, 350));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}
			short h = 12 + 10;
			{	// ファイラー起動ボタン
				widget::param wp(vtx::irect(10, h, 100, 36), tools_);
				h += 40;
				widget_button::param wp_("開く");
				fopen_ = wd.add_widget<widget_button>(wp, wp_);
			}
			{	// Grid、On/Off
				widget::param wp(vtx::irect(10, h, 150, 30), tools_);
				h += 30;
				widget_check::param wp_("Grid", true);
				grid_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{	// ボディー、On/Off
				widget::param wp(vtx::irect(10, h, 150, 30), tools_);
				h += 30;
				widget_check::param wp_("Body");
				body_ = wd.add_widget<widget_check>(wp, wp_);
				body_->set_check();
			}
			{	// ボーン、On/Off
				widget::param wp(vtx::irect(10, h, 150, 30), tools_);
				h += 30;
				widget_check::param wp_("Bone");
				bone_ = wd.add_widget<widget_check>(wp, wp_);
			}

			{	// ツリー
				widget::param wp(vtx::irect(20, 400, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tree_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0), tree_frame_);
				widget_tree::param wp_;
				tree_ = wd.add_widget<widget_tree>(wp, wp_);
			}

			{	// ターミナル
				widget::param wp(vtx::irect(100, 400, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0), terminal_frame_);
				widget_terminal::param wp_;
				terminal_ = wd.add_widget<widget_terminal>(wp, wp_);
			}


			// ボーン表示用ライトの設定
			bone_light_ = light_.create();
			light_.set_position(bone_light_, vtx::fvtx(5.0f, 5.0f, 5.0f));

			// プリファレンスの取得
			sys::preference& pre = director_.at().preference_;
			if(filer_) {
				filer_->load(pre);
			}
			if(tree_frame_) {
				tree_frame_->load(pre);
			}
			if(terminal_frame_) {
				terminal_frame_->load(pre);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			gui::widget_director& wd = director_.at().widget_director_;

			if(fopen_->get_selected()) {
				bool f = filer_->get_state(gui::widget::state::ENABLE);
				filer_->enable(!f);
			}

			/// ファイラー、ファイル選択
			if(filer_id_ != filer_->get_select_file_id()) {
				filer_id_ = filer_->get_select_file_id();

				if(pmd_io_.load(filer_->get_file())) {
					info_pmd_();
					pmd_io_.render_setup();
					pmx_enable_ = false;
					std::string info;
					pmd_io_.get_info(info);
					terminal_->output(info);
				} else if(pmx_io_.load(filer_->get_file())) {
					info_pmx_();
					pmx_io_.render_setup();
					pmx_enable_ = true;
					std::string info;
					pmx_io_.get_info(info);
					terminal_->output(info);
				}
			}

			if(!wd.update()) {
				camera_.update();
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render()
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			gl::glColor(img::rgbaf(1.0f));

			camera_.service();

// glActiveTexture(0);

			if(grid_->get_check()) {
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
				glEnable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				gl::draw_grid(vtx::fpos(-10.0f), vtx::fpos(10.0f), vtx::fpos(1.0f));
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_BLEND);
			}

			if(pmx_enable_) {
				if(body_->get_check()) {
					pmx_io_.render_surface();
				}
				if(bone_->get_check()) {
					pmx_io_.render_bone();
				}
			} else {
				if(body_->get_check()) {
					pmd_io_.render_surface();
				}
				if(bone_->get_check()) {
					pmd_io_.render_bone();
				}
			}

			light_.enable(false);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			director_.at().widget_director_.service();
			director_.at().widget_director_.render();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			sys::preference& pre = director_.at().preference_;
			if(terminal_frame_) {
				terminal_frame_->save(pre);
			}
			if(tree_frame_) {
				tree_frame_->save(pre);
			}
			if(filer_) {
				filer_->save(pre);
			}
		}
	};
}
