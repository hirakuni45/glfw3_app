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
		@param[in]	CHN		チャネル数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename UNIT, uint32_t LIMIT, uint32_t CHN>
	class render_waves {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  チャネル描画パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct chr_param {
			img::rgba8	color_;		///< 描画色
			int32_t		offset_;	///< 垂直オフセット（電圧）
			float		gain_;		///< 垂直ゲイン（電圧）

			bool		update_;	///< 描画の更新「true」

			chr_param() : color_(img::rgba8(255, 255)),
				offset_(0), gain_(1.0f), update_(false)
			{ }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  インフォメーション描画パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		class info_param {
		public:
			img::rgba8	grid_color_;
			int32_t		grid_step_;
			bool		grid_enable_;

		private:
			vtx::sposs	grid_;

		public:
			info_param() : grid_color_(img::rgba8(255, 128)), grid_step_(30),
				grid_enable_(true),
				grid_()
			{ }

			void build_grid(const vtx::ipos& size)
			{
				grid_.clear();
				for(int h = 0; h < size.x; h += grid_step_) {  // |||
					grid_.push_back(vtx::spos(h, 0));
					grid_.push_back(vtx::spos(h, size.y));
				}
				for(int v = 0; v < size.y; v += grid_step_) {  // ---
					grid_.push_back(vtx::spos(0, v));
					grid_.push_back(vtx::spos(size.x, v));
				}
			}

			void render_grid()
			{
				if(!grid_.empty()) {
					glEnable(GL_LINE_STIPPLE);
					glLineStipple(1, 0b1111000011110000);
					gl::glColor(grid_color_);
					gl::draw_lines(grid_);
					glDisable(GL_LINE_STIPPLE);
				}
			}
		};

	private:
		typedef std::vector<UNIT> UNITS;

		struct ch_t {
			chr_param	param_;
			uint32_t	tstep_;
			UNITS		units_;
			vtx::sposs	lines_;

			ch_t() : param_(), tstep_(0), units_(), lines_()
			{ }
		};

		info_param	info_;

		ch_t		ch_[CHN];
		double		div_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		render_waves() : ch_{ }, div_(0.0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  パラメーターを取得
			@param[in]	ch	チャネル
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		const chr_param& get_param(uint32_t ch) const
		{
			if(ch >= CHN) {
				static chr_param p;
				return p;
			}
			return ch_[ch].param_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  パラメーターを参照
			@param[in]	ch	チャネル
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		chr_param& at_param(uint32_t ch)
		{
			if(ch >= CHN) {
				static chr_param p;
				return p;
			}
			return ch_[ch].param_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形生成
			@param[in]	time	生成時間 [sec]
			@param[in]	div		分解能 [sec]
			@return 生成した数
		*/
		//-----------------------------------------------------------------//
		uint32_t create_buffer(double time, double div)
		{
			if(div <= 0.0 || time <= 0.0) return 0;

			auto n = time / div;

			if(n <= 0.0) return 0;
			else if(n > static_cast<double>(LIMIT)) return 0;

			for(int i = 0; i < CHN; ++i) {
				ch_[i].units_.resize(static_cast<uint32_t>(n));
			}
			div_ = div;  // 分解能

			return n;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト波形生成
			@param[in]	frq		周波数 [Hz]
		*/
		//-----------------------------------------------------------------//
		void build_sin(double frq)
		{
#if 0
			for(uint32_t i = 0; i < units_.size(); ++i) {
//				double t = 1.0 / frq;
				double t = 1.0 / 1024.0;
				units_[i] = 32768 - static_cast<UNIT>(sin(2.0 * vtx::get_pi<double>() * t * i) * 32767.0);
			}
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形コピー
			@param[in]	ch		チャネル
			@param[in]	src		波形ソース
			@param[in]	len		波形数
		*/
		//-----------------------------------------------------------------//
		void copy(uint32_t ch, const UNIT* src, uint32_t len)
		{
			if(len > ch_[ch].units_.size()) len = ch_[ch].units_.size();
			for(uint32_t i = 0; i < len; ++i) {
				ch_[ch].units_[i] = *src++;
			}
			ch_[ch].param_.update_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
   			@param[in]	size	描画サイズ（ピクセル）
			@param[in]	step	時間軸ステップ（65536を1.0）
		*/
		//-----------------------------------------------------------------//
		void render(const vtx::ipos& size, uint32_t tstep)
		{
			bool update_grid = false;
			for(uint32_t n = 0; n < CHN; ++n) {
				ch_t& t = ch_[n];
				bool update = false;
				if(t.lines_.size() != size.x) {
					t.lines_.resize(size.x);
					update = true;
					update_grid = true;
				}
				if(t.tstep_ != tstep) {
					t.tstep_ = tstep;
					update = true;
				}
				if(update || t.param_.update_) {
					float gain = t.param_.gain_;
					uint32_t ts = 0;
					for(uint32_t i = 0; i < size.x; ++i) {
						float v = static_cast<float>(t.units_[ts >> 16]);
						t.lines_[i] = vtx::spos(i, v * gain);
						ts += tstep;
					}
					t.param_.update_ = false;
				}
				glPushMatrix();
				gl::glTranslate(0, t.param_.offset_);
				gl::glColor(t.param_.color_);
				gl::draw_line_strip(t.lines_);
				glPopMatrix();
			}
			if(update_grid) {
				info_.build_grid(size);
			}
			info_.render_grid();
		}
	};
}
