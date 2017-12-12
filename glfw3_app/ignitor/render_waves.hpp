#pragma once
//=====================================================================//
/*! @file
    @brief  波形描画テンプレート・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include "gl_fw/glutils.hpp"

namespace view {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  render_waves template class
		@param[in]	UNIT	波形の型
		@param[in]	LIMIT	最大波形数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename UNIT, uint32_t LIMIT>
	class render_waves {

		std::vector<UNIT>	units_;

		double	div_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		render_waves() : units_(), div_(0.0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  波形生成
			@param[in]	time	生成時間 [sec]
			@param[in]	div		分解能 [sec]
			@return 生成した数
		*/
		//-----------------------------------------------------------------//
		uint32_t create_waves(double time, double div)
		{
			if(div <= 0.0 || time <= 0.0) return 0;

			auto n = time / div;

			if(n <= 0.0) return 0;
			else if(n > static_cast<double>(LIMIT)) return 0;

			units_.resize(static_cast<uint32_t>(n));

			div_ = div;  // 分解能

			return n;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト波形生成
			@param[in]	frq		周波数 [Hz]
		*/
		//-----------------------------------------------------------------//
		void create_sin(double frq)
		{
			for(uint32_t i = 0; i < units_.size(); ++i) {
				double t = 1.0 / frq;
				units_[i] = 32768 - static_cast<UNIT>(sin(2.0 * vtx::get_pi<double>() * t * i) * 32767.0);  
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  描画
			@param[in]	width	横幅（ピクセル）
		*/
		//-----------------------------------------------------------------//
		void render(uint32_t width)
		{
			vtx::sposs list;
			list.resize(width);
			for(uint32_t i = 0; i < width; ++i) {
				list[i] = vtx::spos(i, units_[i] / 256);
			}
			gl::draw_line_strip(list);
		}
	};
}
