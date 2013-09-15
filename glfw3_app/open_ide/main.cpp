//=====================================================================//
/*! @file
	@brief  player サンプル
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "gl_fw/glutils.hpp"
#include "open_ide.hpp"

#if 0
void dropfiles(main_core& core)
{
	gl::IGLcore* igl = gl::get_glcore();

	int id = igl->get_recv_file_id();
	if(core.id_ != id) {
		core.id_ = id;
		const utils::strings& ss = igl->get_recv_file_path();
		if(!ss.empty()) {
		}
	}
}
#endif

int main(int argc, char** argv);

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

	{
		utils::director<app::core> director;

		director.at_core().preference_.load(pref);

		if(!igl->setup(vtx::srect(100, 100, 1024, 768), "Open IDE", false)) {
			return -1;
		}
		igl->set_limit_size(vtx::spos(1024, 768));

		director.at_core().sound_.initialize(16);

		director.at_core().widget_director_.initialize();

		director.install_scene<app::open_ide>();

		while(!igl->get_exit_signal()) {
			igl->service();

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			gl::glColor(img::rgbaf(1.0f));

//	gl::IGLcore* igl = gl::get_glcore();
//	const vtx::spos& size = igl->get_size();
//	core.mobj_bg_.setup_matrix(-size.x / 2, -size.y / 2, size.x / 2, size.y / 2, -1.0f, 1.0f);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			director.render();

			igl->flip_frame();

			director.at_core().sound_.service();
		}

		director.at_core().preference_.save(pref);
	}

	igl->destroy();

	gl::destroy_glcore();

	return 0;
}
