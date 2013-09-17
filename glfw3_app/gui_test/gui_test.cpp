//=====================================================================//
/*! @file
	@brief  player クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/lexical_cast.hpp>
#include "gui_test.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_utils.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_image.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void gui_test::initialize()
	{
		gl::IGLcore* igl = gl::get_glcore();

		using namespace gui;
		widget_director& wd = director_.at_core().widget_director_;

		if(1) {	// ラジオボタンのテスト
			widget::param wpr(vtx::srect(20, 20, 100, 200), 0);
			widget_null::param wpr_;
			widget* root = wd.add_widget<widget_null>(wpr, wpr_);
			root->set_state(widget::state::POSITION_LOCK);

			widget::param wp(vtx::srect(0, 0, 100, 30), root);
			widget_radio::param wp_("Enable");
			wd.add_widget<widget_radio>(wp, wp_);
			wp.rect_.org.y += 40;
			wd.add_widget<widget_radio>(wp, wp_);
			wp.rect_.org.y += 40;
			wp_.check_ = true;
			wd.add_widget<widget_radio>(wp, wp_);
		}

		if(1) { // リストのテスト
			widget::param wp(vtx::srect(30, 300, 150, 40), 0);
			widget_list::param wp_("List Box");
			wp_.text_list_.push_back("abc");
			wp_.text_list_.push_back("1234");
			wp_.text_list_.push_back("qwert");
			wd.add_widget<widget_list>(wp, wp_);
		}

		if(0) {	// イメージのテスト
			widget::param wp(vtx::srect(200, 20, 500, 500), 0);
			img::paint pa;
			pa.set_fore_color(img::rgba8(0, 255, 0));
			pa.create(vtx::spos(500), true);
			pa.fill(img::rgba8(255, 100, 100));
			pa.alpha_blend();
			pa.fill_circle(vtx::spos(250), 220);
			vtx::sposs ss;
			ss.push_back(vtx::spos(10, 10));
			ss.push_back(vtx::spos(100, 100));
			ss.push_back(vtx::spos(50, 200));
			pa.set_fore_color(img::rgba8(240, 40, 50));
//			pa.fill_polygon(ss);
			widget_image::param wp_(&pa);
			wd.add_widget<widget_image>(wp, wp_);
		}

		if(1) { // フレームのテスト
			widget::param wp(vtx::srect(200, 20, 100, 80));
			widget_frame::param wp_;
			wd.add_widget<widget_frame>(wp, wp_);
		}

		if(1) { // ボタンのテスト
			widget::param wp(vtx::srect(30, 200, 100, 40));
			widget_button::param wp_("Button");
			wd.add_widget<widget_button>(wp, wp_);
		}

		if(1) { // ラベルのテスト
			widget::param wp(vtx::srect(30, 250, 150, 40));
			widget_label::param wp_("Label");
			wd.add_widget<widget_label>(wp, wp_);
		}

	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void gui_test::update()
	{
		gl::IGLcore* igl = gl::get_glcore();
		const vtx::spos& size = igl->get_size();

		gui::widget_director& wd = director_.at_core().widget_director_;
		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void gui_test::render()
	{
		director_.at_core().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void gui_test::destroy()
	{
	}

}
