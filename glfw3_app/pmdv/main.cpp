//=====================================================================//
/*! @file
	@brief  共通メイン
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "pmdv_main.hpp"

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

		vtx::ipos locate(10, 10);
		director.at_core().preference_.get_position("/window/locate", locate);
		vtx::ipos size(800, 600);
		vtx::spos lsz = size;
		director.at_core().preference_.get_position("/window/size", size);

		if(!igl->setup(vtx::srect(locate, size), "PMD Viewer", false)) {
			return -1;
		}
		igl->set_limit_size(lsz);

		director.at_core().sound_.initialize(16);

		director.at_core().widget_director_.initialize();

		director.install_scene<app::pmdv_main>();

		while(!igl->get_exit_signal()) {
			igl->service();

			director.render();

			igl->flip_frame();

			director.at_core().sound_.service();
		}
		// プログラム終了の廃棄
		director.erase_scene();
		director.render();

		vtx::ipos p = igl->get_size();
		director.at_core().preference_.put_position("/window/size", p);
		p = igl->get_locate();
		director.at_core().preference_.put_position("/window/locate", p);

		director.at_core().preference_.save(pref);
	}

	igl->destroy();

	gl::destroy_glcore();
}
