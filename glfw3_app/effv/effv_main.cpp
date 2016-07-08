//=====================================================================//
/*! @file
	@brief  player クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "core/glcore.hpp"
#include "gl_fw/glutils.hpp"
#include "effv_main.hpp"

namespace app {

	void effv_main::init_effekseer_(const vtx::spos& size)
	{
		// 描画用インスタンスの生成
		renderer_ = ::EffekseerRendererGL::Renderer::Create( 2000 );

		// エフェクト管理用インスタンスの生成
		manager_ = ::Effekseer::Manager::Create( 2000 );

		// 描画用インスタンスから描画機能を設定
		manager_->SetSpriteRenderer( renderer_->CreateSpriteRenderer() );
		manager_->SetRibbonRenderer( renderer_->CreateRibbonRenderer() );
		manager_->SetRingRenderer( renderer_->CreateRingRenderer() );
		manager_->SetModelRenderer( renderer_->CreateModelRenderer() );

		// 描画用インスタンスからテクスチャの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		manager_->SetTextureLoader( renderer_->CreateTextureLoader() );
		manager_->SetModelLoader( renderer_->CreateModelLoader() );

		// 音再生用インスタンスの生成
		/// g_sound = EffekseerSound::Sound::Create( 32 );

		// 音再生用インスタンスから再生機能を指定
		/// g_manager->SetSoundPlayer( g_sound->CreateSoundPlayer() );
	
		// 音再生用インスタンスからサウンドデータの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		/// g_manager->SetSoundLoader( g_sound->CreateSoundLoader() );

		// 視点位置を確定
		position_ = ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f );

		// 投影行列を設定
		renderer_->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, size.x / size.y, 1.0f, 50.0f));

		// カメラ行列を設定
		renderer_->SetCameraMatrix(
			::Effekseer::Matrix44().LookAtRH( position_, ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ), ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	}


	void effv_main::destroy_effekseer_()
	{
		// エフェクトの停止
		manager_->StopEffect( handle_ );

		// エフェクトの破棄
		ES_SAFE_RELEASE( effect_ );

		// 先にエフェクト管理用インスタンスを破棄
		manager_->Destroy();

		// 次に音再生用インスタンスを破棄
		/// sound_->Destory();

		// 次に描画用インスタンスを破棄
		renderer_->Destory();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void effv_main::initialize()
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
			h += 40;
			widget_check::param wp_("Grid", true);
			grid_ = wd.add_widget<widget_check>(wp, wp_);
		}
		{	// Play ボタン
			widget::param wp(vtx::irect(10, h, 100, 36), tools_);
			h += 40;
			widget_button::param wp_("Play");
			play_ = wd.add_widget<widget_button>(wp, wp_);
		}
		{	// Loop、On/Off
			widget::param wp(vtx::irect(10, h, 150, 30), tools_);
			h += 40;
			widget_check::param wp_("Loop", true);
			loop_ = wd.add_widget<widget_check>(wp, wp_);
		}

		init_effekseer_(core.get_rect().size);

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void effv_main::update()
	{
		gui::widget_director& wd = director_.at().widget_director_;

		if(fopen_->get_selected()) {
			bool f = filer_->get_state(gui::widget::state::ENABLE);
			filer_->enable(!f);
		}

		/// ファイラー、ファイル選択
		if(filer_id_ != filer_->get_select_file_id()) {
			filer_id_ = filer_->get_select_file_id();
///			std::cout << filer_->get_file() << std::endl;

			// エフェクトの読込
			utils::wstring path;
//			utils::utf8_to_utf16("/Users/hira/glfw3_app/glfw3_app/effv/test.efk", path);
//			utils::utf8_to_utf16("/Users/hira/glfw3_app/glfw3_app/effv/dmg_inferno_ek.efk", path);
			utils::utf8_to_utf16(filer_->get_file(), path);
			effect_ = Effekseer::Effect::Create(manager_, path.c_str());
			if(effect_ == nullptr) {
				std::cout << "Can't open effekseer file..." << std::endl;
			} else {
				// エフェクトの再生
				if(manager_) {
					handle_ = manager_->Play(effect_, 0, 0, 0 );
				}
			}
		}

		// アニメーションの再生
		if(loop_->get_check()) {

		}
		if(play_->get_selected()) {
			if(manager_) {
				handle_ = manager_->Play(effect_, 0, 0, 0 );
			}			
		}

		if(!wd.update()) {
			camera_.update();
		}


		// エフェクトの移動処理を行う
		if(manager_ && handle_ >= 0) {
			manager_->SetScale(handle_, 5.0f, 5.0f, 5.0f);
///			manager_->AddLocation( handle_, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );
			manager_->Update();
		}

	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void effv_main::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::glColor(img::rgbaf(1.0f));


		camera_.service();

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

///		light_.enable(false);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// エフェクトの描画開始処理を行う。
		if(manager_) {
			renderer_->BeginRendering();

			// エフェクトの描画を行う。
			manager_->Draw();
		}
			// エフェクトの描画終了処理を行う。
		if(renderer_) {
			renderer_->EndRendering();
		}

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
	void effv_main::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(filer_) {
			filer_->save(pre);
		}
	}
}
