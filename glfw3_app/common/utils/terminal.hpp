#pragma once
//=========================================================================//
/*!	@file
	@brief	ターミナル・クラス @n
			VT100 を模倣したクラス、エスケープシーケンスなど @n
			※現状では、完全な互換性を実装していない（作業中）
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

/* VT100 (ANSI) Escape code list:
Name                  Description                            Esc Code
setnl LMN             Set new line mode                      ^[[20h
setappl DECCKM        Set cursor key to application          ^[[?1h
setansi DECANM        Set ANSI (versus VT52)                 none
setcol DECCOLM        Set number of columns to 132           ^[[?3h
setsmooth DECSCLM     Set smooth scrolling                   ^[[?4h
setrevscrn DECSCNM    Set reverse video on screen            ^[[?5h
setorgrel DECOM       Set origin to relative                 ^[[?6h
setwrap DECAWM        Set auto-wrap mode                     ^[[?7h
setrep DECARM         Set auto-repeat mode                   ^[[?8h
setinter DECINLM      Set interlacing mode                   ^[[?9h

setlf LMN             Set line feed mode                     ^[[20l
setcursor DECCKM      Set cursor key to cursor               ^[[?1l
setvt52 DECANM        Set VT52 (versus ANSI)                 ^[[?2l
resetcol DECCOLM      Set number of columns to 80            ^[[?3l
setjump DECSCLM       Set jump scrolling                     ^[[?4l
setnormscrn DECSCNM   Set normal video on screen             ^[[?5l
setorgabs DECOM       Set origin to absolute                 ^[[?6l
resetwrap DECAWM      Reset auto-wrap mode                   ^[[?7l
resetrep DECARM       Reset auto-repeat mode                 ^[[?8l
resetinter DECINLM    Reset interlacing mode                 ^[[?9l

altkeypad DECKPAM     Set alternate keypad mode              ^[=
numkeypad DECKPNM     Set numeric keypad mode                ^[>

setukg0               Set United Kingdom G0 character set    ^[(A
setukg1               Set United Kingdom G1 character set    ^[)A
setusg0               Set United States G0 character set     ^[(B
setusg1               Set United States G1 character set     ^[)B
setspecg0             Set G0 special chars. & line set       ^[(0
setspecg1             Set G1 special chars. & line set       ^[)0
setaltg0              Set G0 alternate character ROM         ^[(1
setaltg1              Set G1 alternate character ROM         ^[)1
setaltspecg0          Set G0 alt char ROM and spec. graphics ^[(2
setaltspecg1          Set G1 alt char ROM and spec. graphics ^[)2

setss2 SS2            Set single shift 2                     ^[N
setss3 SS3            Set single shift 3                     ^[O

modesoff SGR0         Turn off character attributes          ^[[m
modesoff SGR0         Turn off character attributes          ^[[0m
bold SGR1             Turn bold mode on                      ^[[1m
lowint SGR2           Turn low intensity mode on             ^[[2m
underline SGR4        Turn underline mode on                 ^[[4m
blink SGR5            Turn blinking mode on                  ^[[5m
reverse SGR7          Turn reverse video on                  ^[[7m
invisible SGR8        Turn invisible text mode on            ^[[8m

setwin DECSTBM        Set top and bottom line#s of a window  ^[[<v>;<v>r

cursorup(n) CUU       Move cursor up n lines                 ^[[<n>A
cursordn(n) CUD       Move cursor down n lines               ^[[<n>B
cursorrt(n) CUF       Move cursor right n lines              ^[[<n>C
cursorlf(n) CUB       Move cursor left n lines               ^[[<n>D
cursorhome            Move cursor to upper left corner       ^[[H
cursorhome            Move cursor to upper left corner       ^[[;H
cursorpos(v,h) CUP    Move cursor to screen location v,h     ^[[<v>;<h>H
hvhome                Move cursor to upper left corner       ^[[f
hvhome                Move cursor to upper left corner       ^[[;f
hvpos(v,h) CUP        Move cursor to screen location v,h     ^[[<v>;<h>f
index IND             Move/scroll window up one line         ^[D
revindex RI           Move/scroll window down one line       ^[M
nextline NEL          Move to next line                      ^[E
savecursor DECSC      Save cursor position and attributes    ^[7
restorecursor DECSC   Restore cursor position and attributes ^[8

tabset HTS            Set a tab at the current column        ^[H
tabclr TBC            Clear a tab at the current column      ^[[g
tabclr TBC            Clear a tab at the current column      ^[[0g
tabclrall TBC         Clear all tabs                         ^[[3g

dhtop DECDHL          Double-height letters, top half        ^[#3
dhbot DECDHL          Double-height letters, bottom half     ^[#4
swsh DECSWL           Single width, single height letters    ^[#5
dwsh DECDWL           Double width, single height letters    ^[#6

cleareol EL0          Clear line from cursor right           ^[[K
cleareol EL0          Clear line from cursor right           ^[[0K
clearbol EL1          Clear line from cursor left            ^[[1K
clearline EL2         Clear entire line                      ^[[2K

cleareos ED0          Clear screen from cursor down          ^[[J
cleareos ED0          Clear screen from cursor down          ^[[0J
clearbos ED1          Clear screen from cursor up            ^[[1J
clearscreen ED2       Clear entire screen                    ^[[2J

devstat DSR           Device status report                   ^[5n
termok DSR               Response: terminal is OK            ^[0n
termnok DSR              Response: terminal is not OK        ^[3n

getcursor DSR         Get cursor position                    ^[6n
cursorpos CPR            Response: cursor is at v,h          ^[<v>;<h>R

ident DA              Identify what terminal type            ^[[c
ident DA              Identify what terminal type (another)  ^[[0c
gettype DA               Response: terminal type code n      ^[[?1;<n>0c

reset RIS             Reset terminal to initial state        ^[c

align DECALN          Screen alignment display               ^[#8
testpu DECTST         Confidence power up test               ^[[2;1y
testlb DECTST         Confidence loopback test               ^[[2;2y
testpurep DECTST      Repeat power up test                   ^[[2;9y
testlbrep DECTST      Repeat loopback test                   ^[[2;10y

ledsoff DECLL0        Turn off all four leds                 ^[[0q
led1 DECLL1           Turn on LED #1                         ^[[1q
led2 DECLL2           Turn on LED #2                         ^[[2q
led3 DECLL3           Turn on LED #3                         ^[[3q
led4 DECLL4           Turn on LED #4                         ^[[4q

#
#  All codes below are for use in VT52 compatibility mode.
#

setansi               Enter/exit ANSI mode (VT52)            ^[<

altkeypad             Enter alternate keypad mode            ^[=
numkeypad             Exit alternate keypad mode             ^[>

setgr                 Use special graphics character set     ^[F
resetgr               Use normal US/UK character set         ^[G

cursorup              Move cursor up one line                ^[A
cursordn              Move cursor down one line              ^[B
cursorrt              Move cursor right one char             ^[C
cursorlf              Move cursor left one char              ^[D
cursorhome            Move cursor to upper left corner       ^[H
cursorpos(v,h)        Move cursor to v,h location            ^[<v><h>
revindex              Generate a reverse line-feed           ^[I

cleareol              Erase to end of current line           ^[K
cleareos              Erase to end of screen                 ^[J

ident                 Identify what the terminal is          ^[Z
identresp             Correct response to ident              ^[/Z

*/

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ターミナル・クラス (VT100 エスケープシーケンスの一部)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct terminal {

		static constexpr int default_lines_  = 150;		///< 標準、最大行数
		static constexpr int default_width_  = 80;		///< 標準、最大横幅（132 文字バージョンもある）
		static constexpr int default_height_ = 24;		///< 標準、最大高さ

		static constexpr img::rgba8 color8_nom_[8] = {  ///< 標準カラー
			{   4,   4,   4 },  // Black
			{ 240,   4,   4 },  // Red
			{   4, 240,   4 },  // Green
			{ 240, 240,   4 },	// Yellow
			{   4,   4, 240 },	// Blue
			{ 240,   4, 240 },	// Magenta
			{   4, 240, 240 },	// Cyan
			{ 240, 240, 240 }	// White
		};
		static constexpr img::rgba8 color8_str_[8] = {  ///< 強調カラー
			{   0,   0,   0 },  // Black
			{ 255,   0,   0 },  // Red
			{   0, 255,   0 },  // Green
			{ 255, 255,   0 },	// Yellow
			{   0,   0, 255 },	// Blue
			{ 255,   0, 255 },	// Magenta
			{   0, 255, 255 },	// Cyan
			{ 255, 255, 255 }	// White
		};
		static constexpr uint32_t esc_number_max_ = 16;  ///< エスケープシーケンスで数字トークンを受け取る最大数 

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

			constexpr cha_t(uint32_t cha = 0,
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

		cha_t		spc_;

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
			if(l.empty()) {
				if(pos_.x > 0) {
					l.assign(pos_.x, ' ');
				}
			}
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

		const auto& getchar_(const vtx::ipos& pos) const noexcept
		{
			if(pos.y < 0 || pos.y >= lines_.size()) return spc_;

			const auto& l = lines_[pos.y];
			if(l.empty()) {
				return spc_;
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
					return spc_;
				}
			}
			return l[n];
		}		

		void right_() noexcept
		{
			if(pos_.x < count_cha_(lines_[pos_.y])) {
				if(test_wide(getchar_(pos_).cha_)) {
					pos_.x += 2;
				} else {
					++pos_.x;
				}
				if(pos_.x >= limit_.x) {
					pos_.x = 0;
					down_();
				}
			} else {
				cha_ = spc_;
				putchar_();
			}
		}

		void left_() noexcept
		{
			if(pos_.x > 0) {
				auto& l = lines_[pos_.y];
				while(l.size() < pos_.x) {
					l.push_back(spc_);
				}
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
				while(l.size() < pos_.x) {
					l.push_back(spc_);
				}
			} else {
				new_line_();
			}
		}

		void up_() noexcept
		{
			if(pos_.y > 0) {
				pos_.y--;
				auto& l = lines_[pos_.y];
				while(l.size() < pos_.x) {
					l.push_back(spc_);
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
			if(cha == 'H') {  // move to cursor
				if(esc_number_idx_ >= 2) {
					auto y = esc_number_[0];
					if(y >= limit_.y) {
						y = limit_.y - 1;
					}
					if(y <= pos_.y) {
						pos_.y = y;
					} else {
						while(pos_.y < y) {
							down_();
						}
					}
					auto x = esc_number_[1];
					if(x >= limit_.x) {
						x = limit_.x - 1;
					}
					if(x <= pos_.x) {
						pos_.x = x;
					} else {
						while(pos_.x < x) {
							right_();
						}
					}
				} else {
					pos_.x = 0;
					pos_.y = 0;
				}
				return;
			}

			uint32_t n = 1;
			if(esc_number_idx_ >= 1) {
				n = esc_number_[0];
			}
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
#if 0
				case 'E':
					down_();
					pos_.x = 0;
					break;
				case 'F':
					up_();
					pos_.x = 0;
					break;
#endif
				default:
					break;
				}
			}
		}

		void esc_getcursorpos_() noexcept
		{
			if(esc_number_idx_ == 1 && esc_number_[0] == 6) {

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
			cha_(), lines_(), limit_(wl, hl), pos_(0), spc_(' '),
			auto_crlf_(false), insert_(false), swap_color_(false),
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
				} else if(cha == 'n') {
					// ESC [ 6 n というエスケープシーケンスを利用してターミナル上のカーソル位置を取得
					esc_getcursorpos_();
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
			return getchar_(pos);
		}		
	};
}
