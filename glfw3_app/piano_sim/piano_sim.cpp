//=====================================================================//
/*! @file
	@brief  Piano SIM クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <boost/lexical_cast.hpp>
#include "piano_sim.hpp"
#include "core/glcore.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void piano_sim::initialize()
	{
//		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;


		{ // ボタン
			widget::param wp(vtx::irect(30, 30, 100, 40));
			widget_button::param wp_("Ring");
			test_ring_ = wd.add_widget<widget_button>(wp, wp_);
		}


		{	// ターミナル
			{
				widget::param wp(vtx::irect(10, 10, 200, 200));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(30);
				terminal_frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0), terminal_frame_);
				widget_terminal::param wp_;
				terminal_core_ = wd.add_widget<widget_terminal>(wp, wp_);
			}
		}


		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;

		if(terminal_frame_) {
			terminal_frame_->load(pre);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void piano_sim::update()
	{
		gui::widget_director& wd = director_.at().widget_director_;


//		auto& sound = director_.at().sound_;
//		audio_io::wave_handle request_pw(int slot, audio_format aform, const wave_gen_func& func);



		wd.update();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void piano_sim::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void piano_sim::destroy()
	{
		sys::preference& pre = director_.at().preference_;

		if(terminal_frame_) {
			terminal_frame_->save(pre);
		}
	}

}
