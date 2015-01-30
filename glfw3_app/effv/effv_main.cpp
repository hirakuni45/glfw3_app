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

	void effv_main::init_effekseer_()
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
#if 0
	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );

	// 音再生用インスタンスの生成
	g_sound = EffekseerSound::Sound::Create( 32 );

	// 音再生用インスタンスから再生機能を指定
	g_manager->SetSoundPlayer( g_sound->CreateSoundPlayer() );
	
	// 音再生用インスタンスからサウンドデータの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetSoundLoader( g_sound->CreateSoundLoader() );

	// 視点位置を確定
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f );

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH_OpenGL( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );

	// カメラ行列を設定
	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH( g_position, ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ), ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	
	// エフェクトの読込
	g_effect = Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"test.efk" );

	// エフェクトの再生
	g_handle = g_manager->Play( g_effect, 0, 0, 0 );
#endif
	}


#if 0
	// エフェクトの停止
	g_manager->StopEffect( g_handle );

	// エフェクトの破棄
	ES_SAFE_RELEASE( g_effect );

	// 先にエフェクト管理用インスタンスを破棄
	g_manager->Destroy();

	// 次に音再生用インスタンスを破棄
	g_sound->Destory();

	// 次に描画用インスタンスを破棄
	g_renderer->Destory();

	// OpenALの解放
	alcDestroyContext(g_alcctx);
	alcCloseDevice(g_alcdev);
	
	g_alcctx = NULL;
	g_alcdev = NULL;

	// OpenGLの解放
	wglMakeCurrent( 0, 0 );
	wglDeleteContext( g_hGLRC );
	timeEndPeriod(1);

	// COMの終了処理
	CoUninitialize();
#endif


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
			widget::param wp(vtx::srect(30, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path());
			filer_ = wd.add_widget<widget_filer>(wp, wp_);
			filer_->enable(false);
		}
		{	// ツールパレット
			widget::param wp(vtx::srect(20, 20, 150, 350));
			widget_frame::param wp_;
			wp_.plate_param_.set_caption(12);
			tools_ = wd.add_widget<widget_frame>(wp, wp_);
			tools_->set_state(gui::widget::state::SIZE_LOCK);
		}
		short h = 12 + 10;
		{	// ファイラー起動ボタン
			widget::param wp(vtx::srect(10, h, 100, 36), tools_);
			h += 40;
			widget_button::param wp_("開く");
			fopen_ = wd.add_widget<widget_button>(wp, wp_);
		}
		{	// Grid、On/Off
			widget::param wp(vtx::srect(10, h, 150, 30), tools_);
			h += 30;
			widget_check::param wp_("Grid", true);
			grid_ = wd.add_widget<widget_check>(wp, wp_);
		}

		init_effekseer_();

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


		}

		if(!wd.update()) {
			camera_.update();
		}
	}

#if 0
			// エフェクトの移動処理を行う
			g_manager->AddLocation( g_handle, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );

			// エフェクトの更新処理を行う
			g_manager->Update();
			
			
			wglMakeCurrent( g_hDC, g_hGLRC );
			wglSwapIntervalEXT( 1 );

			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// エフェクトの描画開始処理を行う。
			g_renderer->BeginRendering();

			// エフェクトの描画を行う。
			g_manager->Draw();

			// エフェクトの描画終了処理を行う。
			g_renderer->EndRendering();

			glFlush();
			wglMakeCurrent( 0, 0 );
			WaitFrame();
			SwapBuffers( g_hDC );
#endif

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
