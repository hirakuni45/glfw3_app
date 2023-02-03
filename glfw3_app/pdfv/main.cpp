//=====================================================================//
/*! @file
	@brief  メイン
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "pdf_main.hpp"

typedef app::pdf_main start_app;

static const char* window_key_ = { "application/window" };
static const char* app_title_ = { "PDFv" };
static const vtx::spos start_pos_(10, 40);
static const vtx::spos start_size_(800, 600);
static const vtx::spos limit_size_(800, 600);

int main(int argc, char** argv)
{
	gl::core& core = gl::core::get_instance();

	if(!core.initialize(argc, argv)) {
		std::cerr << "glcore initialize error." << std::endl;
///		fgetc(stdin);
		return -1;
	}
	
	std::string pref = core.get_exec_path();
   	pref += ".pre";

	utils::director<app::core> director;

	director.at().preference_.load(pref);

	vtx::srect rect(start_pos_, start_size_);
	if(!director.at().preference_.load_rect(window_key_, rect)) {
//		std::cout << "Load rect error..." << std::endl; 
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
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
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

	core.destroy();
}
