//=====================================================================//
/*! @file
	@brief  main
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "open_ide.hpp"

typedef app::open_ide start_app;

static const char* window_key_ = { "application/window" };
static const char* app_title_ = { "Open IDE" };
static const vtx::spos start_size_(800, 600);
static const vtx::spos limit_size_(800, 600);

int main(int argc, char** argv)
{
	gl::create_glcore();

	gl::IGLcore* igl = gl::get_glcore();

	// カレントパスを生成
	std::string tmp;
	utils::convert_delimiter(argv[0], '\\', '/', tmp);
	std::string pref;
	utils::get_file_base(tmp, pref);
	pref += ".pre";
	std::string path;
	utils::get_file_path(tmp, path);

	if(!igl->initialize(path)) {
		return -1;
	}

	utils::director<app::core> director;

	director.at().preference_.load(pref);

	vtx::srect rect(vtx::spos(10, 40), start_size_);
	if(!director.at().preference_.load_rect(window_key_, rect)) {
//		std::cout << "Load rect error..." << std::endl; 
	}

	if(!igl->setup(rect, app_title_, false)) {
		return -1;
	}
	igl->set_limit_size(limit_size_);

	director.at().sound_.initialize(16);

	director.at().widget_director_.initialize();

	director.install_scene<start_app>();

	while(!igl->get_exit_signal()) {
		igl->service();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::glColor(img::rgbaf(1.0f));

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		director.render();

		igl->flip_frame();

		director.at().sound_.service();
	}
	// プログラム終了の廃棄
	director.erase_scene();
	director.render();

	{
		vtx::srect rect(igl->get_locate(), igl->get_size());
		director.at().preference_.save_rect(window_key_, rect);
	}

	director.at().preference_.save(pref);

	igl->destroy();

	gl::destroy_glcore();
}
