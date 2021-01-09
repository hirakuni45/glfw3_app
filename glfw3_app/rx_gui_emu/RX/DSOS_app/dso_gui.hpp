#pragma once
//=====================================================================//
/*! @file
    @brief  DSO GUI クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
    @copyright  Copyright (C) 2020 Kunihito Hiramatsu @n
                Released under the MIT license @n
                https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "graphics/widget_director.hpp"

#include "capture.hpp"
#include "render_wave.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  波形描画クラス
		@param[in]	RENDER	描画クラス
		@param[in]	TOUCH	タッチ・クラス
		@param[in]	CAPTURE	キャプチャー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class RENDER, class TOUCH, class CAPTURE>
	class dso_gui {

		static const int16_t BTN_GRID = 40;
		static const int16_t BTN_SIZE = 38;

		RENDER&		render_;
		TOUCH&		touch_;
		CAPTURE&	capture_;

		typedef gui::widget_director<RENDER, TOUCH, 32> WIDD;
		WIDD		widd_;

		typedef utils::render_wave<RENDER, TOUCH, CAPTURE> RENDER_WAVE;
		RENDER_WAVE	render_wave_;

		utils::capture_trigger	trigger_ = utils::capture_trigger::NONE;

		typedef gui::button BUTTON;
		BUTTON		ch0_;
		BUTTON		ch1_;
		BUTTON		trg_;
		BUTTON		smp_;
		BUTTON		mes_;
		BUTTON		opt_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクタ
			@param[in]	render	描画クラス
			@param[in]	touch	タッチ・クラス
			@param[in]	capture	キャプチャー・クラス
		*/
		//-----------------------------------------------------------------//
		dso_gui(RENDER& render, TOUCH& touch, CAPTURE& capture) noexcept :
			render_(render), touch_(touch), capture_(capture),
			widd_(render, touch),
			render_wave_(render_, touch_, capture_),
			trigger_(utils::capture_trigger::NONE),
			ch0_(vtx::srect(442, 16+BTN_GRID*0, BTN_SIZE, BTN_SIZE), "CH0"),
			ch1_(vtx::srect(442, 16+BTN_GRID*1, BTN_SIZE, BTN_SIZE), "CH1"),
			trg_(vtx::srect(442, 16+BTN_GRID*2, BTN_SIZE, BTN_SIZE), "Trg"),
			smp_(vtx::srect(442, 16+BTN_GRID*3, BTN_SIZE, BTN_SIZE), "Smp"),
			mes_(vtx::srect(442, 16+BTN_GRID*4, BTN_SIZE, BTN_SIZE), "Mes"),
			opt_(vtx::srect(442, 16+BTN_GRID*5, BTN_SIZE, BTN_SIZE), "Opt")
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  開始
		*/
		//-----------------------------------------------------------------//
		void start() noexcept
		{
			ch0_.enable();
			ch0_.at_select_func() = [=](uint32_t id) {
			};

			ch1_.enable();
			ch1_.at_select_func() = [=](uint32_t id) {
			};

			trg_.enable();
			trg_.at_select_func() = [=](uint32_t id) {
			};

			smp_.enable();
			smp_.at_select_func() = [=](uint32_t id) {
			};

			mes_.enable();
			mes_.at_select_func() = [=](uint32_t id) {
			};

			opt_.enable();
			opt_.at_select_func() = [=](uint32_t id) {
			};
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新 @n
					※毎フレーム呼ぶ
		*/
		//-----------------------------------------------------------------//
		void update() noexcept
		{
			widd_.update();

			// タッチ操作による画面更新が必要か？
			bool f = render_wave_.ui_service();

			// 波形をキャプチャーしたら描画
			if(f || (trigger_ != utils::capture_trigger::NONE
				&& capture_.get_trigger() == utils::capture_trigger::NONE)) {
				trigger_ = utils::capture_trigger::NONE;
				render_wave_.update();
			}

		}


		//-----------------------------------------------------------------//
		/*!
			@brief  widget_director の参照
			@return widget_director
		*/
		//-----------------------------------------------------------------//
		auto& at_widd() noexcept { return widd_; }
	};
}
