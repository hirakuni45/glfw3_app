#pragma once
//=====================================================================//
/*! @file
    @brief  波形描画テンプレート・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
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
			img::rgba8	grid_color_;	///< グリッド・カラー
			int32_t		grid_step_;		///< グリッド・ステップ
			uint16_t	grid_stipple_;	///< グリッド・破線パターン
			bool		grid_enable_;	///< グリッド有効

			img::rgba8	time_color_;	///< 時間軸カラー
			int32_t		time_org_;		///< 時間軸開始
			int32_t		time_len_;		///< 時間軸長さ
			uint16_t	time_stipple_;	///< 時間軸破線パターン
			bool		time_enable_;	///< 時間軸有効
//			bool		time_anime_;	///< 時間軸アニメーション

			img::rgba8	volt_color_;	///< 電圧軸カラー
			int32_t		volt_org_;		///< 電圧軸開始
			int32_t		volt_len_;		///< 電圧軸長さ
			uint16_t	volt_stipple_;	///< 電圧軸破線パターン
			bool		volt_enable_;	///< 電圧軸有効
//			bool		volt_anime_;	///< 電圧軸アニメーション

		private:
			vtx::sposs	grid_;
			vtx::sposs	time_;
			vtx::sposs	volt_;
			vtx::ipos	size_;

			uint32_t	count_;

			static void rotate_(uint16_t& bits) {
				auto mod = bits & 0x8000;
				bits <<= 1;
				if(mod) bits |= 1;
			}

		public:
			info_param() : grid_color_(img::rgba8(255, 255, 255, 96)), grid_step_(30),
				grid_stipple_(0b1111000011110000), grid_enable_(true),

				time_color_(img::rgba8(255, 255, 128, 192)),
				time_org_(0), time_len_(0),
				time_stipple_(0b1110110011101100), time_enable_(true),

				volt_color_(img::rgba8(128, 255, 128, 192)),
				volt_org_(0), volt_len_(0),
				volt_stipple_(0b1110110011101100), volt_enable_(true),
				grid_(), time_(), volt_(), size_(), count_(0)
			{ }

			void build(const vtx::ipos& size)
			{
				if(size == size_) return;
				size_ = size;

				grid_.clear();
				for(int h = 0; h < size.x; h += grid_step_) {  // |||
					grid_.push_back(vtx::spos(h, 0));
					grid_.push_back(vtx::spos(h, size.y));
				}
				for(int v = 0; v < size.y; v += grid_step_) {  // ---
					grid_.push_back(vtx::spos(0, v));
					grid_.push_back(vtx::spos(size.x, v));
				}

				time_.clear();
				time_.push_back(vtx::spos(0, 0));
				time_.push_back(vtx::spos(0, size.y));

				volt_.clear();
				volt_.push_back(vtx::spos(0, 0));
				volt_.push_back(vtx::spos(size.x, 0));
			}

			void render()
			{
				glEnable(GL_LINE_STIPPLE);
				if(!grid_.empty() && grid_enable_) {
					glLineStipple(1, grid_stipple_);
					gl::glColor(grid_color_);
					gl::draw_lines(grid_);
				}
				if(time_enable_) {
					glLineStipple(1, time_stipple_);
					glPushMatrix();
					gl::glTranslate(time_org_, 0);
					gl::glColor(time_color_);
					gl::draw_lines(time_);
					gl::glTranslate(time_len_, 0);
					gl::draw_lines(time_);
					glPopMatrix();
				}
				if(volt_enable_) {
					glLineStipple(1, volt_stipple_);
					glPushMatrix();
					gl::glTranslate(0, volt_org_);
					gl::glColor(volt_color_);
					gl::draw_lines(volt_);
					gl::glTranslate(0, volt_len_);
					gl::draw_lines(volt_);
					glPopMatrix();
				}
				if(count_ > 0) {
					--count_;
				} else {
					rotate_(time_stipple_);
					rotate_(volt_stipple_);
					count_ = 3;
				}
				glDisable(GL_LINE_STIPPLE);
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

		vtx::ipos	win_size_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		render_waves() : ch_{ }, div_(0.0), win_size_(0) { }


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
			@brief  情報パラメーターを取得
			@return 情報パラメーター
		*/
		//-----------------------------------------------------------------//
		const info_param& get_info() const {
			return info_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  情報パラメーターを参照
			@return 情報パラメーター
		*/
		//-----------------------------------------------------------------//
		info_param& at_info() {
			return info_;
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
			bool update_win = win_size_ != size;
			win_size_ = size;
			for(uint32_t n = 0; n < CHN; ++n) {
				ch_t& t = ch_[n];
				bool update = update_win;
				if(t.lines_.size() != size.x) {
					t.lines_.resize(size.x);
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

			info_.build(size);

			info_.render();
		}
	};
}
