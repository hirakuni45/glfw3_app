//=====================================================================//
/*! @file
	@brief  main
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "bmc_main.hpp"

#include "bmc_core.hpp"
#include "img_io/img_files.hpp"

#include <memory>

typedef app::bmc_main	start_app;

static const char* window_key_ = { "application/window" };
static const char* app_title_ = { "Bit Map Converter" };
static const vtx::spos start_pos_(10, 50);
static const vtx::spos start_size_(1024, 768);
static const vtx::spos limit_size_(800, 600);

int main(int argc, char** argv)
{
	app::bmc_core bmc;
//	std::cout << static_cast<int>(sizeof(app::bmc_core)) << std::endl;
//	auto bmcp = std::shared_ptr<app::bmc_core>(new app::bmc_core(argc, argv));
//	app::bmc_core& bmc = *bmcp.get();

	if(!bmc.analize(argc, argv) || bmc.get_inp_file().empty()) {
		bmc.help(argv[0]);
	} else {
		if(!bmc.execute()) {
			return 0;
		}
	}

	// プレビューする場合
	if(!bmc.get_option(app::bmc_core::option::preview)) {
		return 0;
	}

	gl::core& core = gl::core::get_instance();

	if(!core.initialize(argc, argv)) {
		std::cerr << "glcore initialize error." << std::endl;
///		fgetc(stdin);
		return -1;
	}
	
	std::string pref = core.get_exec_path();
   	pref += ".pre";

	utils::director<app::core> director;

	// bmc_core を設定
	director.at().bmc_ = bmc;

	director.at().preference_.load(pref);

	vtx::srect rect(start_pos_, start_size_);
	if(!director.at().preference_.load_rect(window_key_, rect)) {
//		std::cout << "Load rect error..." << std::endl; 
	}

	if(!core.setup(rect, app_title_, false)) {
		return -1;
	}
	core.set_limit_size(limit_size_);

	director.at().sound_.initialize(16);

	director.at().widget_director_.initialize();

	director.install_scene<start_app>();

	while(!core.get_exit_signal()) {
		core.service();

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
		director.at().preference_.save_rect(window_key_, core.get_rect());
	}

	director.at().preference_.save(pref);

	core.destroy();
}
