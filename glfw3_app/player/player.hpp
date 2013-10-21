//=====================================================================//
/*! @file
	@brief  player クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <time.h>
#include "main.hpp"
#include "utils/director.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_slider.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_dialog.hpp"
#include "gl_fw/glmobj.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  プレイヤー・メイン・シーン
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class player : public utils::i_scene {

		/// 「REW」ボタンが押された場合の挙動切り替え時間
		static const int seek_change_time_ = 5;

		utils::director<core>&	director_;

		gui::widget_filer*	filer_;

		gui::widget*	file_btn_;
		gui::widget*	play_btn_;
		gui::widget*	pause_btn_;

		gui::widget*	rew_btn_;
		gui::widget*	ff_btn_;

		gui::widget_slider*	volume_;
		gui::widget*		vol_min_img_;
		gui::widget*		vol_max_img_;

		gui::widget_label*	total_time_;
		gui::widget_label*	remain_time_;
		gui::widget_slider*	seek_time_;
		gui::widget_check*	resume_play_;

		gui::widget_label*	title_pad_;
		gui::widget_label*	album_pad_;
		gui::widget_label*	artist_pad_;
		gui::widget_label*	other_pad_;

		gui::widget_dialog*	error_dialog_;

		time_t			total_t_;
		time_t			remain_t_;
		uint32_t		frame_count_;
		uint32_t		frame_limit_;

		uint32_t			tag_serial_;
		gl::mobj			mobj_;
		gl::mobj::handle	jacket_;

		int				drop_file_id_;
		uint32_t		select_file_id_;

		void sound_play_(const std::string& file);
		gui::widget* create_image_button_(const std::string& file, const vtx::spos& pos);
		gui::widget_label* create_text_pad_(const vtx::spos& size, const std::string& text,
			const std::string& font = "", bool proportional = true);
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		player(utils::director<core>& d) : director_(d),
		  filer_(0),
		  file_btn_(0), play_btn_(0), pause_btn_(0), rew_btn_(0), ff_btn_(0),
		  volume_(0), vol_min_img_(0), vol_max_img_(0),
		  total_time_(0), remain_time_(0), seek_time_(0), resume_play_(0),
		  title_pad_(0), album_pad_(0), artist_pad_(0), other_pad_(0),
		  error_dialog_(0),
		  total_t_(0), remain_t_(0), frame_count_(0), frame_limit_(60),
		  tag_serial_(0), jacket_(0), drop_file_id_(0), select_file_id_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~player() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
