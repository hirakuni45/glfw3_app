//=============================================================================//
/*! @file
	@brief  オーディオ・アナライザ・メイン
	@author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=============================================================================//
#include "main.hpp"
#include "aan_main.hpp"

typedef app::aan_main start_app;

static constexpr char app_title_[] = "Audio Analizer";
static constexpr char window_key_[] = "application/window";
static constexpr vtx::spos start_pos_(10, 40);
static constexpr vtx::spos start_size_(1280, 1024);
static constexpr vtx::spos limit_size_(1920, 1600);

int main(int argc, char** argv)
{
	auto& core = gl::core::get_instance();

	if(!core.initialize(argc, argv)) {
		std::cerr << "glcore initialize error." << std::endl;
		return -1;
	}
	
	std::string pref = core.get_exec_path();
	pref += ".pre";

	utils::director<app::core> director;

	director.at().preference_.load(pref);

	vtx::srect rect(start_pos_, start_size_);
	if(!director.at().preference_.load_rect(window_key_, rect)) {
	}

	if(!core.setup(rect, app_title_, false)) {
		std::cerr << "Core setup error" << std::endl;
		return -1;
	}
	core.set_limit_size(limit_size_);

	director.at().sound_.initialize(16);

	director.at().widget_director_.initialize();

	director.install_scene<start_app>();

	while(!core.get_exit_signal()) {
		core.service();

		glClearColor(0, 0, 0, 255);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::glColor(img::rgbaf(1.0f));

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		director.render();

		core.flip_frame();

		director.at().sound_.service();
	}
	// プログラム終了の廃棄
	director.erase_scene();
	director.render();

	{
		const vtx::srect& rect = core.get_rect();
		director.at().preference_.save_rect(window_key_, rect);
	}

	director.at().preference_.save(pref);

	director.at().sound_.destroy();

	core.destroy();
}
