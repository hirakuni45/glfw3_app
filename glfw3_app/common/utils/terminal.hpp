#pragma once
//=========================================================================//
/*!	@file
	@brief	ターミナル・クラス @n
			VT100 を模倣したクラス、エスケープシーケンスなど @n
			※現状では、完全な互換性を実装していないので注意
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>
#include "img_io/img.hpp"
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"
#include "utils/format.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ターミナル・クラス (VT100 エスケープシーケンスの一部)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct terminal {

		static constexpr int default_lines_ = 150;		///< 標準、最大行数
		static constexpr int default_width_ = 80;		///< 標準、最大横幅

		static constexpr img::rgba8 color8_nom_[8] = {
			{   4,   4,   4 },  // Black
			{ 240,   4,   4 },  // Red
			{   4, 240,   4 },  // Green
			{ 240, 240,   4 },	// Yellow
			{   4,   4, 240 },	// Blue
			{ 240,   4, 240 },	// Magenta
			{   4, 240, 240 },	// Cyan
			{ 240, 240, 240 }	// White
		};
		static constexpr img::rgba8 color8_str_[8] = {
			{   0,   0,   0 },  // Black
			{ 255,   0,   0 },  // Red
			{   0, 255,   0 },  // Green
			{ 255, 255,   0 },	// Yellow
			{   0,   0, 255 },	// Blue
			{ 255,   0, 255 },	// Magenta
			{   0, 255, 255 },	// Cyan
			{ 255, 255, 255 }	// White
		};
		static constexpr uint32_t esc_number_max_ = 16;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	文字コンテナ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct cha_t {
			uint32_t	cha_;		///< キャラクターコード (UTF-32)
			img::rgba8	fc_;		///< 文字カラー
			img::rgba8	bc_;		///< 背景カラー
			bool		select_;	///< 文字選択

			cha_t(uint32_t cha = 0,
				  const img::rgba8& fc = img::rgba8(255, 255, 255, 255),
				  const img::rgba8& bc = img::rgba8(  0,   0,   0, 255)) noexcept :
				cha_(cha), fc_(fc), bc_(bc), select_(false)
				{ }
		};

		typedef std::vector<cha_t>	line;
		typedef std::deque<line>	lines;

		typedef std::function< void (uint32_t ch) > output_func;

	private:
		img::rgba8	fore_color_;
		img::rgba8	back_color_;

		cha_t		cha_;

		lines		lines_;
		vtx::ipos	limit_;

		vtx::ipos	pos_;

		cha_t		tmp_;

		bool		auto_crlf_;
		bool		insert_;
		bool		swap_color_;

		line		last_;

		output_func	output_func_;

		enum class ESC_MODE {
			NONE,
			FIRST,
			DECIMAL,
		};
		ESC_MODE	esc_mode_;
		uint32_t	esc_number_cnt_;
		uint32_t	esc_number_idx_;
		uint32_t	esc_number_[esc_number_max_];

		void new_line_() noexcept
		{
			if(lines_.size() >= limit_.y) {
				lines_.pop_front();
			} else {
				++pos_.y;
			}
			last_ = lines_.back();
			line l;
			lines_.push_back(l);
		}

		int count_cha_(const line& l) const noexcept
		{
			if(l.empty()) return 0;

			int n = 0;
			for(const auto& ch : l) {
				if(test_wide(ch.cha_)) n += 2;
				else n++;
			}
			return n;
		}

		int count_index_(const line& l, int x) const noexcept
		{
			if(l.empty()) return 0;

			int idx = 0;
			int n = 0;
			while(n < x) {
				if(test_wide(l[idx].cha_)) { n += 2; }
				else { ++n; }
				++idx;
			}
			return idx;
		}

		void putchar_() noexcept
		{
			int step = 1;
			if(test_wide(cha_.cha_)) {
				step = 2;
				if((pos_.x + step) >= limit_.x) {
					new_line_();
					pos_.x = 0;
				}
			}

			line& l = lines_[pos_.y];
			auto idx = count_index_(l, pos_.x);
			if(idx < l.size()) {
				if(insert_) {
					l.insert(l.begin() + idx, cha_);
				} else {
					l[idx] = cha_;
					pos_.x += step;
				}
			} else {
				l.push_back(cha_);
				pos_.x += step;
			}

			if(count_cha_(l) >= limit_.x) {
				new_line_();
				pos_.x = 0;
			}
		}

		const auto& get_char_(const vtx::ipos& pos) const noexcept
		{
			if(pos.y < 0 || pos.y >= lines_.size()) return tmp_;

			const auto& l = lines_[pos.y];
			if(l.empty()) {
				return tmp_;
			}

			int n = 0;
			int x = 0;
			while(x < pos.x) {
				if(test_wide(l[n].cha_)) {
					x += 2;
				} else {
					++x;
				}
				++n;
				if(n >= l.size()) {
					return tmp_;
				}
			}
			return l[n];
		}		

		void right_() noexcept
		{
			if(pos_.x < count_cha_(lines_[pos_.y])) {
				if(test_wide(get_char_(pos_).cha_)) {
					pos_.x += 2;
				} else {
					++pos_.x;
				}
				if(pos_.x >= limit_.x) {
					pos_.x = 0;
					down_();
				}
			}
		}

		void left_() noexcept
		{
			if(pos_.x > 0) {
				auto& l = lines_[pos_.y];
				auto idx = count_index_(l, pos_.x);
				if(idx > 0) --idx;
				if(test_wide(l[idx].cha_)) {
					pos_.x -= 2;
				} else {
					pos_.x -= 1;
				}
			}
		}

		void down_() noexcept
		{
			if(pos_.y < (lines_.size() - 1)) {
				++pos_.y;
				auto& l = lines_[pos_.y];
				if(l.size() <= pos_.x) {
					pos_.x = l.size();
				}
			}
		}

		void up_() noexcept
		{
			if(pos_.y > 0) {
				pos_.y--;
				auto& l = lines_[pos_.y];
				if(l.size() <= pos_.x) {
					pos_.x = l.size();
				}
			}
		}

		void esc_m_() noexcept
		{
			enum class set_color {
				none,
				fore_color_sel,
				fore_indexed,
				fore_r,
				fore_g,
				fore_b,
				back_color_sel,
				back_indexed,
				back_r,
				back_g,
				back_b,
			};
			auto color_mode = set_color::none;

			for(uint32_t i = 0; i < esc_number_idx_; ++i) {

				auto n = esc_number_[i];

				bool conti = false;
				switch(color_mode) {
				case set_color::fore_color_sel:
					if(n == 2) color_mode = set_color::fore_r;
					else if(n == 5) color_mode = set_color::fore_indexed;
					conti = true;
					break;
				case set_color::fore_r:
					fore_color_.r = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::fore_g;
					conti = true;
					break;
				case set_color::fore_g:
					fore_color_.g = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::fore_b;
					conti = true;
					break;
				case set_color::fore_b:
					fore_color_.b = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::none;
					conti = true;
					break;
				case set_color::fore_indexed:
					// auto idx = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::none;
					conti = true;
					break;
				case set_color::back_color_sel:
					if(n == 2) color_mode = set_color::back_r;
					else if(n == 5) color_mode = set_color::back_indexed;
					conti = true;
					break;
				case set_color::back_r:
					back_color_.r = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::back_g;
					conti = true;
					break;
				case set_color::back_g:
					back_color_.g = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::back_b;
					conti = true;
					break;
				case set_color::back_b:
					back_color_.b = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::none;
					conti = true;
					break;
				case set_color::back_indexed:
					// auto idx = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::none;
					conti = true;
					break;
				default:
					break;
				}
				if(conti) {
					continue;
				}

				if(n == 0) {
					fore_color_.set(255, 255, 255);
					back_color_.set(0, 0, 0);
					swap_color_ = false;
				} else if(n == 7) {
					swap_color_ = !swap_color_;
				} else if(n == 38) {
					color_mode = set_color::fore_color_sel;
				} else if(n == 48) {
					color_mode = set_color::back_color_sel;
				} else if(n == 39) {
					fore_color_.set(255, 255, 255);
				} else if(n == 49) {
					back_color_.set(0, 0, 0);
				} else if(n >= 30 && n <= 37) {
					n -= 30;
					fore_color_ = color8_nom_[n];
				} else if(n >= 40 && n <= 47) {
					n -= 40;
					back_color_ = color8_nom_[n];
				} else if(n >= 90 && n <= 97) {
					n -= 90;
					fore_color_ = color8_str_[n];
				} else if(n >= 100 && n <= 107) {
					n -= 100;
					back_color_ = color8_str_[n];
				}
			}
		}

		void esc_A2T_(uint32_t cha) noexcept
		{
			if(cha == 'G') {

				return;
			}

			uint32_t n = esc_number_[0];
			if(n == 0) n = 1;
			for(uint32_t i = 0; i < n; ++i) {
				switch(cha) {
				case 'A':
					up_();
					break;
				case 'B':
					down_();
					break;
				case 'C':
					right_();
					break;
				case 'D':
					left_();
					break;
				case 'E':
					down_();
					pos_.x = 0;
					break;
				case 'F':
					up_();
					pos_.x = 0;
					break;
				}
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	hl	最大ライン数
			@param[in]	wl	最大横幅
		*/
		//-----------------------------------------------------------------//
		terminal(int hl = default_lines_, int wl = default_width_) noexcept :
			fore_color_(img::rgba8(255, 255, 255, 255)),
			back_color_(img::rgba8(  0,   0,   0, 255)),
			cha_(), lines_(), limit_(wl, hl), pos_(0), tmp_(' '),
			auto_crlf_(false), insert_(true), swap_color_(false),
			last_(), output_func_(nullptr),
			esc_mode_(ESC_MODE::NONE), esc_number_cnt_(0), esc_number_idx_(0), esc_number_{ 0 }
		{
			line l;
			lines_.push_back(l);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~terminal() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ワイド・キャラクターコードを判定
			@return ワイド・キャラクターコードなら「true」
		*/
		//-----------------------------------------------------------------//
		bool test_wide(uint32_t cha) const noexcept
		{
			return cha >= 0x100;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	自動 CR/LF の許可／不許可
			@param[in]	f	自動機能を無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_crlf(bool f = true) noexcept { auto_crlf_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	insert モードの許可／不許可
			@param[in]	f	不許可にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_insert(bool f = true) noexcept { insert_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	全面消去
		*/
		//-----------------------------------------------------------------//
		void clear() noexcept
		{
			lines_.clear();
			line l;
			lines_.push_back(l);
			pos_.set(0);
			last_.clear();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン・クリア
			@param[in]	all		ライン全体を消去の場合「true」
		*/
		//-----------------------------------------------------------------//
		void clear_line(bool all = true) noexcept
		{
			line& l = lines_[pos_.y];
			if(all) l.clear();
			else {
				if(pos_.x < l.size()) {
					l.resize(pos_.x);
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全セレクト
			@param[in] ena		選択状態
		*/
		//-----------------------------------------------------------------//
		void select_all(bool ena) noexcept
		{
			for(auto& l : lines_) {
				for(auto& ch : l) {
					ch.select_ = ena;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力関数設定
			@param[in]	func	関数
		*/
		//-----------------------------------------------------------------//
		void set_output_func(output_func func) noexcept
		{
			output_func_ = func;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t cha) noexcept
		{
			if(output_func_ != nullptr) output_func_(cha);

			cha_.cha_ = cha;
			if(swap_color_) {
				cha_.bc_ = fore_color_;
				cha_.fc_ = back_color_;
			} else {
				cha_.fc_ = fore_color_;
				cha_.bc_ = back_color_;
			}

			switch(esc_mode_) {
			case ESC_MODE::FIRST:
				if(cha == '[') {
					esc_mode_ = ESC_MODE::DECIMAL;
					esc_number_cnt_ = 0;
					esc_number_idx_ = 0;
					esc_number_[esc_number_idx_] = 0;
				} else {
					esc_mode_ = ESC_MODE::NONE;
				}
				return;
			case ESC_MODE::DECIMAL:
				if(cha >= '0' && cha <= '9') {
					esc_number_[esc_number_idx_] *= 10;
					esc_number_[esc_number_idx_] += cha - '0';
					++esc_number_cnt_;
				} else if(cha == 'm') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					} 
					esc_m_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == ';') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					}
					esc_number_cnt_ = 0;
					if(esc_number_idx_ >= esc_number_max_) esc_number_idx_ = esc_number_max_ - 1; 
					esc_number_[esc_number_idx_] = 0;
				} else if(cha >= 'A' && cha <= 'T') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					}
					esc_A2T_(cha);
					esc_mode_ = ESC_MODE::NONE;
				} else {
					esc_mode_ = ESC_MODE::NONE;
				}
				return;
			case ESC_MODE::NONE:
			default:
				break;
			}

			switch(cha) {
			case '\r':  // CR
				pos_.x = 0;
				break;
			case '\n':  // LF
				new_line_();
				if(auto_crlf_) {
					pos_.x = 0;
				}
				break;
			case 0x08:  // Back Space
				if(pos_.x > 0) {
					auto& l = lines_[pos_.y];
					--pos_.x;
					if(pos_.x < l.size()) {
						l.erase(l.begin() + pos_.x);
					}
				}
				break;
			case 0x1b:  // ESC
				esc_mode_ = ESC_MODE::FIRST;
				break;
			case 0x7f:  // DEL
				{
					auto& l = lines_[pos_.y];
					auto idx = count_index_(l, pos_.x);
					if(idx < l.size()) {
						l.erase(l.begin() + idx);
					}
				}
				break;
			case 0x11:
				right_();
				break;
			case 0x12:
				left_();
				break;
			case 0x13:
				down_();
				break;
			case 0x14:
				up_();
				break;
			default:
				if(cha < 0x20) {
//					std::cout << boost::format("%02X") % cha << std::endl << std::flush;
				} else {
					putchar_();
				}
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力（UTF-8)
			@param[in]	str	文字列
		*/
		//-----------------------------------------------------------------//
		void output(const std::string& str) noexcept
		{
			auto ls = utils::utf8_to_utf32(str);
			for(auto lch : ls) {
				output(lch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力（UTF-32）
			@param[in]	str	文字列
		*/
		//-----------------------------------------------------------------//
		void output(const std::u32string& str) noexcept
		{
			for(auto lch : str) {
				output(lch);
			}
		}

#if 0
		//-----------------------------------------------------------------//
		/*!
			@brief	フォア・カラーの設定
			@param[in]	pos		位置
			@param[in]	col		カラー
		*/
		//-----------------------------------------------------------------//
		void set_fore_color(const vtx::ipos& pos, const img::rgba8& col) noexcept
		{
			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = lines_[pos.y];
				if(pos.x >= 0 && pos.x < l.size()) {
					l[pos.x].fc_ = col;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バック・カラーの設定
			@param[in]	pos		位置
			@param[in]	col		カラー
		*/
		//-----------------------------------------------------------------//
		void set_back_color(const vtx::ipos& pos, const img::rgba8& col) noexcept
		{
			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = lines_[pos.y];
				if(pos.x >= 0 && pos.x < l.size()) {
					l[pos.x].bc_ = col;
				}
			}
		}
#endif

		//-----------------------------------------------------------------//
		/*!
			@brief	選択状態の設定
			@param[in]	pos		位置
			@param[in]	ena		非選択の場合は「false」
		*/
		//-----------------------------------------------------------------//
		void set_select(const vtx::ipos& pos, bool ena = true) noexcept
		{
			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = lines_[pos.y];
				if(pos.x >= 0 && pos.x < l.size()) {
					l[pos.x].select_ = ena;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_cursor() const noexcept { return pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン数を取得
			@return ライン数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_line_num() const noexcept { return lines_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラインを取得
			@param[in]	pos	ライン位置
			@return ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_line(uint32_t pos) const noexcept { return lines_[pos]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・ラインを取得
			@return ラスト・ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_last_line() const noexcept { return last_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・テキストを取得 (UTF-32）
			@return ラスト・テキスト
		*/
		//-----------------------------------------------------------------//
		auto get_last_text32() const noexcept
		{
			std::u32string ls;
			for(auto ch : last_) {
				ls += ch.cha_;
			}
			return ls;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・テキストを取得
			@return ラスト・テキスト
		*/
		//-----------------------------------------------------------------//
		std::string get_last_text() const noexcept
		{
			return utf32_to_utf8(get_last_text32());
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・キャラクターを取得
			@return ラスト・キャラクター
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_last_cha() const noexcept { return cha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラスト・キャラクターを取得
			@return ラスト・キャラクター
		*/
		//-----------------------------------------------------------------//
		uint32_t get_last_char() const noexcept { return cha_.cha_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キャラクター・コンテナを取得
			@param[in]	pos	キャラクター位置
			@return キャラクター
		*/
		//-----------------------------------------------------------------//
		const auto& get_char(const vtx::ipos& pos) const noexcept
		{
			return get_char_(pos);
		}		
	};
}
