#pragma once
//=========================================================================//
/*!	@file
	@brief	ターミナル・クラス @n
			VT100 を模倣したクラス、エスケープシーケンスなど @n
			※現状では、完全な互換性を実装していない（作業中） @n
			TeraTerm の動作を基準とする
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
#include "utils/file_io.hpp"
#include "utils/format.hpp"

/* ASCII code table:
NUL	0	DLE	16	SPACE	32	0	48
(^@)	$00	(^P)	$10		$20		$30
SOH	1	DC1	17	!	33	1	49
(^A)	$01	(^Q)	$11		$21		$31
STX	2	DC2	18	"	34	2	50
(^B)	$02	(^R)	$12		$22		$32
ETX	3	DC3	19	#	35	3	51
(^C)	$03	(^S)	$13		$23		$33
EOT	4	DC4	20	$	36	4	52
(^D)	$04	(^T)	$14		$24		$34
ENQ	5	NAK	21	%	37	5	53
(^E)	$05	(^U)	$15		$25		$35
ACK	6	SYN	22	&	38	6	54
(^F)	$06	(^V)	$16		$26		$36
BEL	7	ETB	23	'	39	7	55
(^G)	$07	(^W)	$17		$27		$37
BS	8	CAN	24	(	40	8	56
(^H)	$08	(^X)	$18		$28		$38
HT	9	EM	25	)	41	9	57
(^I)	$09	(^Y)	$19		$29		$39
LF	10	SUB	26	*	42	:	58
(^J)	$0A	(^Z)	$1A		$2A		$3A
VT	11	ESC	27	+	43	;	59
(^K)	$0B	(^[)	$1B		$2B		$3B
FF	12	FS	28	,	44	<	60
(^L)	$0C	(^)	$1C		$2C		$3C
CR	13	GS	29	-	45	=	61
(^M)	$0D	(^])	$1D		$2D		$3D
SO	14	RS	30	.	46	>	62
(^N)	$0E	(^^)	$1E		$2E		$3E
SI	15	US	31	/	47	?	63
(^O)	$0F	(^_)	$1F		$2F		$3F

@	64	P	80	`	96	p	112
	$40		$50		$60		$70
A	65	Q	81	a	97	q	113
	$41		$51		$61		$71
B	66	R	82	b	98	r	114
	$42		$52		$62		$72
C	67	S	83	c	99	s	115
	$43		$53		$63		$73
D	68	T	84	d	100	t	116
	$44		$54		$64		$74
E	69	U	85	e	101	u	117
	$45		$55		$65		$75
F	70	V	86	f	102	v	118
	$46		$56		$66		$76
G	71	W	87	g	103	w	119
	$47		$57		$67		$77
H	72	X	88	h	104	x	120
	$48		$58		$68		$78
I	73	Y	89	i	105	y	121
	$49		$59		$69		$79
J	74	Z	90	j	106	z	122
	$4A		$5A		$6A		$7A
K	75	[	91	k	107	{	123
	$4B		$5B		$6B		$7B
L	76	\	92	l	108	|	124
	$4C		$5C		$6C		$7C
M	77	]	93	m	109	}	125
	$4D		$5D		$6D		$7D
N	78	^	94	n	110	~	126
	$4E		$5E		$6E		$7E
O	79	-	95	o	111	DEL	127
	$4F		$5F		$6F		$7F
*/

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

+ modesoff SGR0         Turn off character attributes          ^[[m
+ modesoff SGR0         Turn off character attributes          ^[[0m
+ bold SGR1             Turn bold mode on                      ^[[1m
+ lowint SGR2           Turn low intensity mode on             ^[[2m
+ underline SGR4        Turn underline mode on                 ^[[4m
+ blink SGR5            Turn blinking mode on                  ^[[5m
+ reverse SGR7          Turn reverse video on                  ^[[7m
+ invisible SGR8        Turn invisible text mode on            ^[[8m

setwin DECSTBM        Set top and bottom line#s of a window  ^[[<v>;<v>r

+ cursorup(n) CUU       Move cursor up n lines                 ^[[<n>A
+ cursordn(n) CUD       Move cursor down n lines               ^[[<n>B
+ cursorrt(n) CUF       Move cursor right n lines              ^[[<n>C
+ cursorlf(n) CUB       Move cursor left n lines               ^[[<n>D
+ cursorhome            Move cursor to upper left corner       ^[[H
+ cursorhome            Move cursor to upper left corner       ^[[;H
+ cursorpos(v,h) CUP    Move cursor to screen location v,h     ^[[<v>;<h>H
+ hvhome                Move cursor to upper left corner       ^[[f
+ hvhome                Move cursor to upper left corner       ^[[;f
+ hvpos(v,h) CUP        Move cursor to screen location v,h     ^[[<v>;<h>f
+ index IND             Move/scroll window up one line         ^[D
+ revindex RI           Move/scroll window down one line       ^[M
+ nextline NEL          Move to next line                      ^[E
+ savecursor DECSC      Save cursor position and attributes    ^[7
+ restorecursor DECSC   Restore cursor position and attributes ^[8

tabset HTS            Set a tab at the current column        ^[H
tabclr TBC            Clear a tab at the current column      ^[[g
tabclr TBC            Clear a tab at the current column      ^[[0g
tabclrall TBC         Clear all tabs                         ^[[3g

dhtop DECDHL          Double-height letters, top half        ^[#3
dhbot DECDHL          Double-height letters, bottom half     ^[#4
swsh DECSWL           Single width, single height letters    ^[#5
dwsh DECDWL           Double width, single height letters    ^[#6

+ cleareol EL0          Clear line from cursor right           ^[[K
+ cleareol EL0          Clear line from cursor right           ^[[0K
+ clearbol EL1          Clear line from cursor left            ^[[1K
+ clearline EL2         Clear entire line                      ^[[2K

+ cleareos ED0          Clear screen from cursor down          ^[[J
+ cleareos ED0          Clear screen from cursor down          ^[[0J
+ clearbos ED1          Clear screen from cursor up            ^[[1J
+ clearscreen ED2       Clear entire screen                    ^[[2J

devstat DSR           Device status report                   ^[5n
termok DSR               Response: terminal is OK            ^[0n
termnok DSR              Response: terminal is not OK        ^[3n

+ getcursor DSR         Get cursor position                    ^[6n
+ cursorpos CPR            Response: cursor is at v,h          ^[<v>;<h>R

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
		@brief	ターミナル・クラス (VT100 エスケープシーケンスの一部を実装)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct terminal {

		static constexpr int DEFAULT_WIDTH  = 80;		///< 標準、最大横幅（132 文字バージョンもある）
		static constexpr int DEFAULT_HEIGHT = 24;		///< 標準、最大高さ
		static constexpr img::rgba8 DEFAULT_FORE_COLOR = { 255, 255, 255 };
		static constexpr img::rgba8 DEFAULT_BACK_COLOR = {   0,   0,   0 };
		static constexpr int DEFAULT_LINES = DEFAULT_HEIGHT * 3;	///< 標準、最大行数
		static constexpr uint32_t DEFAULT_ATTR_LEVEL = 16;	///< アトリビュートのスタック長

		// 256 色とは、基本 16 色、RGB 216色(6 x 6 x 6 の立方体に配置)、および 24 レベルのグレースケール
		static constexpr img::rgba8 color256_[256] = {
			{  0,  0,  0}, {255,  0,  0}, {  0,255,  0}, {255,255,  0}, {  0,  0,255}, {255,  0,255}, {  0,255,255}, {255,255,255},  //   0 -   7
			{128,128,128}, {128,  0,  0}, {  0,128,  0}, {128,128,  0}, {  0,  0,128}, {128,  0,128}, {  0,128,128}, {192,192,192},  //   8 -  15
			{  0,  0,  0}, {  0,  0, 95}, {  0,  0,135}, {  0,  0,175}, {  0,  0,215}, {  0,  0,255}, {  0, 95,  0}, {  0, 95, 95},  //  16 -  23
			{  0, 95,135}, {  0, 95,175}, {  0, 95,215}, {  0, 95,255}, {  0,135,  0}, {  0,135, 95}, {  0,135,135}, {  0,135,175},  //  24 -  31
			{  0,135,215}, {  0,135,255}, {  0,175,  0}, {  0,175, 95}, {  0,175,135}, {  0,175,175}, {  0,175,215}, {  0,175,255},  //  32 -  39
			{  0,215,  0}, {  0,215, 95}, {  0,215,135}, {  0,215,175}, {  0,215,215}, {  0,215,255}, {  0,255,  0}, {  0,255, 95},  //  40 -  47
			{  0,255,135}, {  0,255,175}, {  0,255,215}, {  0,255,255}, { 95,  0,  0}, { 95,  0, 95}, { 95,  0,135}, { 95,  0,175},  //  48 -  55
			{ 95,  0,215}, { 95,  0,255}, { 95, 95,  0}, { 95, 95, 95}, { 95, 95,135}, { 95, 95,175}, { 95, 95,215}, { 95, 95,255},  //  56 -  63
			{ 95,135,  0}, { 95,135, 95}, { 95,135,135}, { 95,135,175}, { 95,135,215}, { 95,135,255}, { 95,175,  0}, { 95,175, 95},  //  64 -  71
			{ 95,175,135}, { 95,175,175}, { 95,175,215}, { 95,175,255}, { 95,215,  0}, { 95,215, 95}, { 95,215,135}, { 95,215,175},  //  72 -  79
			{ 95,215,215}, { 95,215,255}, { 95,255,  0}, { 95,255, 95}, { 95,255,135}, { 95,255,175}, { 95,255,215}, { 95,255,255},  //  80 -  87
			{135,  0,  0}, {135,  0, 95}, {135,  0,135}, {135,  0,175}, {135,  0,215}, {135,  0,255}, {135, 95,  0}, {135, 95, 95},  //  88 -  95
			{135, 95,135}, {135, 95,175}, {135, 95,215}, {135, 95,255}, {135,135,  0}, {135,135, 95}, {135,135,135}, {135,135,175},  //  96 - 103
			{135,135,215}, {135,135,255}, {135,175,  0}, {135,175, 95}, {135,175,135}, {135,175,175}, {135,175,215}, {135,175,255},  // 104 - 111
			{135,215,  0}, {135,215, 95}, {135,215,135}, {135,215,175}, {135,215,215}, {135,215,255}, {135,255,  0}, {135,255, 95},  // 112 - 119
			{135,255,135}, {135,255,175}, {135,255,215}, {135,255,255}, {175,  0,  0}, {175,  0, 95}, {175,  0,135}, {175,  0,175},  // 120 - 127
			{175,  0,215}, {175,  0,255}, {175, 95,  0}, {175, 95, 95}, {175, 95,135}, {175, 95,175}, {175, 95,215}, {175, 95,255},  // 128 - 135
			{175,135,  0}, {175,135, 95}, {175,135,135}, {175,135,175}, {175,135,215}, {175,135,255}, {175,175,  0}, {175,175, 95},  // 136 - 143
			{175,175,135}, {175,175,175}, {175,175,215}, {175,175,255}, {175,215,  0}, {175,215, 95}, {175,215,135}, {175,215,175},  // 144 - 151
			{175,215,215}, {175,215,255}, {175,255,  0}, {175,255, 95}, {175,255,135}, {175,255,175}, {175,255,215}, {175,255,255},  // 152 - 159
			{215,  0,  0}, {215,  0, 95}, {215,  0,135}, {215,  0,175}, {215,  0,215}, {215,  0,255}, {215, 95,  0}, {215, 95, 95},  // 160 - 167
			{215, 95,135}, {215, 95,175}, {215, 95,215}, {215, 95,255}, {215,135,  0}, {215,135, 95}, {215,135,135}, {215,135,175},  // 168 - 175
			{215,135,215}, {215,135,255}, {215,175,  0}, {215,175, 95}, {215,175,135}, {215,175,175}, {215,175,215}, {215,175,255},  // 176 - 183
			{215,215,  0}, {215,215, 95}, {215,215,135}, {215,215,175}, {215,215,215}, {215,215,255}, {215,255,  0}, {215,255, 95},  // 184 - 191
			{215,255,135}, {215,255,175}, {215,255,215}, {215,255,255}, {255,  0,  0}, {255,  0, 95}, {255,  0,135}, {255,  0,175},  // 192 - 199
			{255,  0,215}, {255,  0,255}, {255, 95,  0}, {255, 95, 95}, {255, 95,135}, {255, 95,175}, {255, 95,215}, {255, 95,255},  // 200 - 207
			{255,135,  0}, {255,135, 95}, {255,135,135}, {255,135,175}, {255,135,215}, {255,135,255}, {255,175,  0}, {255,175, 95},  // 208 - 215
			{255,175,135}, {255,175,175}, {255,175,215}, {255,175,255}, {255,215,  0}, {255,215, 95}, {255,215,135}, {255,215,175},  // 216 - 223
			{255,215,215}, {255,215,255}, {255,255,  0}, {255,255, 95}, {255,255,135}, {255,255,175}, {255,255,215}, {255,255,255},  // 224 - 231
			{  8,  8,  8}, { 18, 18, 18}, { 28, 28, 28}, { 38, 38, 38}, { 48, 48, 48}, { 58, 58, 58}, { 68, 68, 68}, { 78, 78, 78},  // 232 - 239
			{ 88, 88, 88}, { 98, 98, 98}, {108,108,108}, {118,118,118}, {128,128,128}, {138,138,138}, {148,148,148}, {158,158,158},  // 240 - 247
			{168,168,168}, {178,178,178}, {188,188,188}, {198,198,198}, {208,208,208}, {218,218,218}, {228,228,228}, {238,238,238}   // 248 - 255
		};
		static constexpr uint32_t esc_number_max_ = 16;  ///< エスケープシーケンスで数字トークンを受け取る最大数 

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	文字コンテナ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct cha_t {
			uint32_t	cha_;			///< キャラクターコード (UTF-32)
			img::rgba8	fc_;			///< 文字カラー
			img::rgba8	bc_;			///< 背景カラー
			bool		bold_;			///< Bold 体
			bool		underline_;		///< アンダーライン
			bool		blinking_;		///< 点滅
			bool		reverse_;		///< 反転
			bool		invisible_;		///< 非表示

			bool		select_;		///< 文字選択

			constexpr cha_t(uint32_t cha = 0,
				const img::rgba8& fc = img::rgba8(255, 255, 255, 255),
				const img::rgba8& bc = img::rgba8(  0,   0,   0, 255)) noexcept :
				cha_(cha), fc_(fc), bc_(bc),
				bold_(false), underline_(false), blinking_(false), reverse_(false), invisible_(false),
				select_(false)
				{ }
		};

		typedef std::vector<cha_t>	line;
		typedef std::deque<line>	lines;

		typedef std::function< void (uint32_t ch) > output_func;

		struct attr_t {
			vtx::ipos	cursor_pos_;
			img::rgba8	fore_color_;
			img::rgba8	back_color_;
			bool		bold_;
			bool		underline_;
			bool		blinking_;
			bool		reverse_;
			bool		invisible_;

			attr_t() noexcept :
				cursor_pos_(0),
				fore_color_(DEFAULT_FORE_COLOR), back_color_(DEFAULT_BACK_COLOR),
				bold_(false), underline_(false), blinking_(false), reverse_(false), invisible_(false)
			{ }
		};

		typedef std::deque<attr_t> attrs;

	private:
		cha_t		cha_;

		lines		lines_;
		vtx::ipos	limit_;
		vtx::ipos	pos_;
		vtx::ipos	cursor_;

		cha_t		spc_;

		bool		auto_crlf_;
		bool		insert_;

		attr_t		attr_;
		attrs		attrs_;

		line		last_;
		std::string	response_;

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

		auto conv_text_(const line& l) const noexcept
		{
			std::u32string ls;
			for(auto ch : l) {
				ls += ch.cha_;
			}
			return utils::utf32_to_utf8(ls);
		}

		bool test_wide_(uint32_t cha) const noexcept
		{
			return cha >= 0x100;
		}

		uint32_t linepos_(int pos) const noexcept
		{
			uint32_t ofs = 0;
			if(lines_.size() >= limit_.y) {
				ofs = lines_.size() - limit_.y;
			}
			uint32_t i = ofs + pos;
			if(i >= lines_.size()) {
				i = lines_.size() - 1;
			}
			return i;
		}

		auto& at_line_(int pos) noexcept
		{
			return lines_[linepos_(pos)];
		}

		const auto& get_line_(int pos, bool real = true) const noexcept
		{
			uint32_t p = pos;
			if(real) {
				p = linepos_(pos);
			} else {
				if(lines_.size() <= pos) {
					p = lines_.size() - 1;
				}
			}
			return lines_[p];
		}

		void new_line_() noexcept
		{
			if(pos_.y < (limit_.y - 1)) {
				++pos_.y;
			}

			last_ = lines_.back();
			line l;
			lines_.push_back(l);

			if(lines_.size() > DEFAULT_LINES) {
				lines_.pop_front();
			}
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
			while(n < x && idx < l.size()) {
				if(test_wide(l[idx].cha_)) {
					n += 2;
					if(n > x) break;
				} else {
					++n;
				}
				++idx;
			}
			return idx;
		}

		void put_char_() noexcept
		{
			int step = 1;
			if(test_wide(cha_.cha_)) {
				step = 2;
			}

			if((pos_.x + step) > limit_.x) {
				new_line_();
				pos_.x = 0;
			}

			auto& l = at_line_(pos_.y);
			if(l.empty()) {
				if(pos_.x > 0) {
					l.assign(pos_.x, ' ');
				}
			}
			auto idx = count_index_(l, pos_.x);
			if(idx < l.size()) {
				if(insert_) {
					l.insert(l.begin() + idx, cha_);
					if(count_cha_(l) > limit_.x) {
						l.pop_back();
					}
				} else {
					l[idx] = cha_;
					pos_.x += step;
				}
			} else {
				l.push_back(cha_);
				pos_.x += step;
			}
		}

		auto& at_char_(const vtx::ipos& pos) noexcept
		{
			if(pos.x < 0 || pos.y < 0) {
				return spc_;
			}
			if(pos.y >= lines_.size()) {
				return spc_;
			}

			auto& l = at_line_(pos.y);
			auto idx = count_index_(l, pos.x);
			if(idx >= l.size()) {
				return spc_;
			}

			return l[idx];
		}

		const auto& get_char_(const vtx::ipos& pos, bool real) const noexcept
		{
			if(pos.y < 0 || pos.y >= lines_.size() || pos.x < 0) {
				return spc_;
			}

			const auto& l = get_line_(pos.y, real);
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
			}
			if(n >= l.size()) {
				return spc_;
			}
			return l[n];
		}		

		void up_(bool scroll = false) noexcept
		{
			if(pos_.y > 0) {
				pos_.y--;
				auto& l = at_line_(pos_.y);
				auto n = count_cha_(l);
				while(n < pos_.x) {
					l.push_back(spc_);
					++n;
				}
			} else if(scroll) {
				auto n = 0;
				bool out = false;
				if(limit_.y <= lines_.size()) {
					n = lines_.size() - limit_.y;
					out = true;
				}
				line l;
				lines_.insert(lines_.begin() + n, l);
				if(out) { lines_.pop_back(); }
			}
		}

		void down_(bool scroll = false) noexcept
		{
			if(pos_.y < (limit_.y - 1)) {
				++pos_.y;
			} else if(scroll) {
				auto n = lines_.size() - limit_.y;
				lines_.erase(lines_.begin() + n);
			}
			if(pos_.y >= lines_.size()) {
				line l;
				lines_.push_back(l);
			}
			auto& l = at_line_(pos_.y);
			auto n = count_cha_(l);
			while(n < pos_.x) {
				l.push_back(spc_);
				++n;
			}
		}

		void right_() noexcept
		{
			auto& l = at_line_(pos_.y);
			auto len = count_cha_(l);
			if(pos_.x < len) {
				auto x = pos_.x;
				if(test_wide(get_char_(pos_, true).cha_)) {
					pos_.x += 2;
				} else {
					++pos_.x;
				}
				if(pos_.x >= limit_.x) {
					pos_.x = x;
				}
			} else {
				if(pos_.x < (limit_.x - 1)) {
					++pos_.x;
					l.push_back(spc_);
				}
			}
		}

		void left_() noexcept
		{
			if(pos_.x > 0) {
				auto& l = at_line_(pos_.y);
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

		void set_cursor_(const vtx::ipos& pos) noexcept
		{
			vtx::ipos np = pos;
			if(np.x < 0) { np.x = 0; }
			if(np.y < 0) { np.y = 0; }
			if(np.x >= limit_.x) { np.x = limit_.x - 1; }
			if(np.y >= limit_.y) { np.y = limit_.y - 1; }

			if(np.y > pos_.y) {
				while(np.y > pos_.y) {
					down_();
				}
			} else if(np.y < pos_.y) {
				while(np.y < pos_.y) {
					up_();
				}
			}

			if(np.x > pos_.x) {
				while(np.x > pos_.x) {
					right_();
				}
			} else if(np.x < pos_.x) {
				while(np.x < pos_.x) {
					left_();
				}
			}
		}

		void clear_() noexcept
		{
			auto n = limit_.y;
			while(n > 0) {
				if(lines_.size() == 0) break;
				lines_.pop_back();
				--n;
			}

			line l;
			lines_.push_back(l);
			pos_.set(0);
			last_.clear();
		}

		void cmd_m_() noexcept
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
					attr_.fore_color_.r = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::fore_g;
					conti = true;
					break;
				case set_color::fore_g:
					attr_.fore_color_.g = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::fore_b;
					conti = true;
					break;
				case set_color::fore_b:
					attr_.fore_color_.b = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::none;
					conti = false;
					break;
				case set_color::fore_indexed:
					attr_.fore_color_ = color256_[std::clamp<uint32_t>(n, 0, 255)];
					color_mode = set_color::none;
					conti = false;
					break;
				case set_color::back_color_sel:
					if(n == 2) color_mode = set_color::back_r;
					else if(n == 5) color_mode = set_color::back_indexed;
					conti = true;
					break;
				case set_color::back_r:
					attr_.back_color_.r = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::back_g;
					conti = true;
					break;
				case set_color::back_g:
					attr_.back_color_.g = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::back_b;
					conti = true;
					break;
				case set_color::back_b:
					attr_.back_color_.b = std::clamp<uint32_t>(n, 0, 255);
					color_mode = set_color::none;
					conti = false;
					break;
				case set_color::back_indexed:
					attr_.back_color_ = color256_[std::clamp<uint32_t>(n, 0, 255)];
					color_mode = set_color::none;
					conti = false;
					break;
				default:
					break;
				}
				if(conti) {
					continue;
				}

				if(n == 0) {
					attr_.fore_color_ = DEFAULT_FORE_COLOR;
					attr_.back_color_ = DEFAULT_BACK_COLOR;
					attr_.bold_ = false;
					attr_.underline_ = false;
					attr_.reverse_ = false;
				} else if(n == 1) {
					attr_.bold_ = true;
				} else if(n == 4) {
					attr_.underline_ = true;
				} else if(n == 5) {
					attr_.blinking_ = true;
				} else if(n == 7) {
					attr_.reverse_ = true;
				} else if(n == 8) {
					attr_.invisible_ = true;
				} else if(n == 38) {
					color_mode = set_color::fore_color_sel;
				} else if(n == 48) {
					color_mode = set_color::back_color_sel;
				} else if(n == 39) {
					attr_.fore_color_ = DEFAULT_FORE_COLOR;
				} else if(n == 49) {
					attr_.back_color_ = DEFAULT_BACK_COLOR;
				} else if(n >= 30 && n <= 37) {
					n -= 30;
					attr_.fore_color_ = color256_[8 + n];
				} else if(n >= 40 && n <= 47) {
					n -= 40;
					attr_.back_color_ = color256_[8 + n];
				} else if(n >= 90 && n <= 97) {
					n -= 90;
					attr_.fore_color_ = color256_[n];
				} else if(n >= 100 && n <= 107) {
					n -= 100;
					attr_.back_color_ = color256_[n];
				}
			}
		}

// hvhome                Move cursor to upper left corner       ^[[f
// hvhome                Move cursor to upper left corner       ^[[;f
// hvpos(v,h) CUP        Move cursor to screen location v,h     ^[[<v>;<h>f
		void cmd_Hf_() noexcept
		{
			if(esc_number_idx_ < 2) {
				set_cursor_(vtx::ipos(0));
			} else {
				set_cursor_(vtx::ipos(esc_number_[1], esc_number_[0]));
			}
		}

		void cmd_ABCD_(uint32_t cha) noexcept
		{
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
				default:
					break;
				}
			}
		}

		void clear_right_(line& l) noexcept
		{
			auto i = count_index_(l, pos_.x);
			auto x = pos_.x;
			while(x < limit_.x) {
				if(i < l.size()) {
					l[i] = spc_;
					++i;
				} else {
					l.push_back(spc_);
				}
				++x;
			}
		}

		void clear_left_(line& l) noexcept
		{
			uint32_t i = 0;
			int x = 0;
			while(x <= pos_.x) {
				if(i < l.size()) {
					if(!test_wide_(l[i].cha_)) {
						++x;
					}
					l[i] = spc_;
					++i;
				} else {
					l.push_back(spc_);
					++x;
				}
			}
		}

		void clear_line_(line& l) noexcept
		{
			line tmp;
			l.swap(tmp);
			for(int x = 0; x < limit_.x; ++x) {
				l.push_back(spc_);
			}
		}

// cleareol EL0          Clear line from cursor right           ^[[K
// cleareol EL0          Clear line from cursor right           ^[[0K
// clearbol EL1          Clear line from cursor left            ^[[1K
// clearline EL2         Clear entire line                      ^[[2K
		void cmd_K_() noexcept
		{
			auto& l = at_line_(pos_.y);
			if(esc_number_[0] == 0) {  // right
				clear_right_(l);
			} else if(esc_number_[0] == 1) {  // left
				clear_left_(l);
			} else if(esc_number_[0] == 2) {  // all
				clear_line_(l);
			}
		}

// cleareos ED0          Clear screen from cursor down          ^[[J
// cleareos ED0          Clear screen from cursor down          ^[[0J
// clearbos ED1          Clear screen from cursor up            ^[[1J
// clearscreen ED2       Clear entire screen                    ^[[2J
		void cmd_J_() noexcept
		{
			if(esc_number_[0] == 0) {
				for(auto y = pos_.y; y < limit_.y; ++y) {
					if(y == pos_.y) {
						auto& l = at_line_(y);
						clear_right_(l);
					} else {
						if(y >= lines_.size()) {
							line l;
							lines_.push_back(l);
						}
						auto& l = at_line_(y);
						clear_line_(l);
					}
				}
			} else if(esc_number_[0] == 1) {
				for(int y = 0; y < pos_.y; ++y) {
					auto& l = at_line_(y);
					clear_line_(l);
				}
				clear_left_(at_line_(pos_.y));
			} else if(esc_number_[0] == 2) {
				for(int y = 0; y < limit_.y; ++y) {
					auto& l = at_line_(y);
					clear_line_(l);
				}
			}
		}

// getcursor DSR         Get cursor position                    ^[6n
// cursorpos CPR            Response: cursor is at v,h          ^[<v>;<h>R
		void cmd_6n_() noexcept
		{
			if(esc_number_idx_ == 1 && esc_number_[0] == 6) {
				response_ = 0x1b;
				char str[32];
				utils::sformat("%d;%dR", str, sizeof(str)) % pos_.y % pos_.x;
				response_ += str;
			}
		}

// savecursor DECSC      Save cursor position and attributes    ^[7
		void save_attr_() noexcept
		{
			if(attrs_.size() < DEFAULT_ATTR_LEVEL) {
				attrs_.push_back(attr_);
			}
		}

// restorecursor DECSC   Restore cursor position and attributes ^[8
		void restore_attr_() noexcept
		{
			if(!attrs_.empty()) {
				attr_ = attrs_.back();
				attrs_.pop_back();
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
		terminal(int hl = DEFAULT_LINES, int wl = DEFAULT_WIDTH) noexcept :
			cha_(), lines_(), limit_(wl, hl), pos_(0), cursor_(0), spc_(' '),
			auto_crlf_(true), insert_(false),
			attr_(), attrs_(),
			last_(), response_(),
			output_func_(nullptr),
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
			return test_wide_(cha);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	自動 CR/LF 状態の取得
			@return	自動 CR/LF 状態
		*/
		//-----------------------------------------------------------------//
		bool get_crlf() const noexcept { return auto_crlf_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	自動 CR/LF の許可／不許可 @n
					標準的に「有効」
			@param[in]	f	自動機能を無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_crlf(bool f = true) noexcept { auto_crlf_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	自動 insert の取得
			@return	自動 insert
		*/
		//-----------------------------------------------------------------//
		bool get_insert() const noexcept { return insert_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	insert モードの許可／不許可 @n
					標準的に「無効」
			@param[in]	f	不許可にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_insert(bool f = true) noexcept { insert_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リミットの取得
			@return	リミット
		*/
		//-----------------------------------------------------------------//
		const auto& get_limit() const noexcept { return limit_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リミットの指定
			@param[in]	newl	新規のリミット値
		*/
		//-----------------------------------------------------------------//
		void set_limit(const vtx::ipos& newl) noexcept
		{
			if(newl.y <= 0 || newl.x <= 0) return;

			while((newl.y - 1) < pos_.y) {
				if(lines_.empty()) break;
				pos_.y--;
				lines_.pop_back();
			}
			if(newl.x <= pos_.x) {
				pos_.x = newl.x - 1; 
			}

			limit_ = newl;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	全面消去
		*/
		//-----------------------------------------------------------------//
		void clear() noexcept
		{
			clear_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン・クリア
			@param[in]	all		ライン全体を消去の場合「true」
		*/
		//-----------------------------------------------------------------//
		void clear_line(bool all = true) noexcept
		{
			line& l = at_line_(pos_.y);
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
			@brief	出力関数設定
			@param[in]	func	関数
		*/
		//-----------------------------------------------------------------//
		void set_output_func(output_func func) noexcept
		{
			output_func_ = func;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	応答文字列を取得
			@return 応答文字列
		*/
		//-----------------------------------------------------------------//
		const auto& get_response() const noexcept
		{
			return response_;
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

			cha_.cha_       = cha;
			cha_.fc_        = attr_.fore_color_;
			cha_.bc_        = attr_.back_color_;
			cha_.bold_      = attr_.bold_;
			cha_.underline_ = attr_.underline_;
			cha_.blinking_  = attr_.blinking_;
			cha_.reverse_   = attr_.reverse_;
			cha_.invisible_ = attr_.invisible_;

			switch(esc_mode_) {
			case ESC_MODE::FIRST:
				if(cha == '[') {
					esc_number_cnt_ = 0;
					esc_number_idx_ = 0;
					esc_number_[esc_number_idx_] = 0;
					esc_mode_ = ESC_MODE::DECIMAL;
				} else if(cha == 'D') {
					up_(true);
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == 'M') {
					down_(true);
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == 'E') {
					pos_.x = 0;
					new_line_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == '7') {
					attr_.cursor_pos_ = pos_;
					save_attr_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == '8') {
					restore_attr_();
					pos_ = attr_.cursor_pos_;
					esc_mode_ = ESC_MODE::NONE;
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
					cmd_m_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == 'H' || cha == 'f') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					} 
					cmd_Hf_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == 'K') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					} 
					cmd_K_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == 'J') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					} 
					cmd_J_();
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == ';') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					}
					if(esc_number_idx_ >= esc_number_max_) esc_number_idx_ = esc_number_max_ - 1; 
					esc_number_[esc_number_idx_] = 0;
					esc_number_cnt_ = 0;
				} else if(cha >= 'A' && cha <= 'D') {
					if(esc_number_cnt_ > 0) {
						++esc_number_idx_;
					}
					cmd_ABCD_(cha);
					esc_mode_ = ESC_MODE::NONE;
				} else if(cha == 'n') {
					cmd_6n_();
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
					auto& l = at_line_(pos_.y);
					if(l.size() > 0) {
						--pos_.x;
						auto idx = count_index_(l, pos_.x);
						if(test_wide_(l[idx].cha_)) {
							--pos_.x;
						}
						if(idx < l.size()) {
							l.erase(l.begin() + idx);
						}
					}
				}
				break;
			case 0x1b:  // ESC
				esc_mode_ = ESC_MODE::FIRST;
				break;
			case 0x7f:  // DEL
				{
					auto& l = at_line_(pos_.y);
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
//					utils::format("%02X\n") % cha;
				} else {
					put_char_();
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


		//-----------------------------------------------------------------//
		/*!
			@brief	選択状態の設定
			@param[in]	pos		位置
			@param[in]	ena		非選択の場合は「false」
		*/
		//-----------------------------------------------------------------//
		void set_select(const vtx::ipos& pos, bool ena = true) noexcept
		{
			if(pos.x < 0 || pos.y < 0) {
				return;
			}

			if(pos.y >= 0 && pos.y < lines_.size()) {
				auto& l = at_line_(pos.y);
				auto idx = count_index_(l, pos.x);
				if(idx < l.size()) {
					l[idx].select_ = ena;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_cursor() noexcept {
			cursor_ = pos_;
			if(cursor_.x >= limit_.x) {
				cursor_.x = limit_.x - 1;
			}
			return cursor_;
		}


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
		const line& get_line(uint32_t pos) const noexcept {
			if(pos < lines_.size()) {
				return lines_[pos];
			} else {
				static line l;
				return l;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン文字列を取得
			@param[in]	pos	ライン位置
			@return ライン
		*/
		//-----------------------------------------------------------------//
		std::u32string get_line_text32(uint32_t pos) const noexcept
		{
			const auto& l = get_line(pos);
			std::u32string t;
			for(auto c : l) {
				t += c.cha_;
			}
			return t;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン文字列を取得
			@param[in]	pos	ライン位置
			@return ライン
		*/
		//-----------------------------------------------------------------//
		std::string get_line_text(uint32_t pos) const noexcept
		{
			auto t32 = get_line_text32(pos);
			std::string t;
			utils::utf32_to_utf8(t32, t);
			return t;
		}


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
			@param[in]	pos		キャラクター位置
			@param[in]	real	仮想エリアを取得する場合「false」
			@return キャラクター
		*/
		//-----------------------------------------------------------------//
		const auto& get_char(const vtx::ipos& pos, bool real = true) const noexcept
		{
			return get_char_(pos, real);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字のセーブ
			@param[in]	fn		ファイル名
			@param[in]	all		画面内だけの場合「false」
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& fn, bool all = true) const noexcept
		{
			utils::file_io fo;
			if(!fo.open(fn, "wt")) {
				return false;
			}

			uint32_t lim = 0;
			if(!all) {
				if(lines_.size() > DEFAULT_HEIGHT) {
					lim = lines_.size() - DEFAULT_HEIGHT;
				}
			}
			for(uint32_t pos = lim; pos < lines_.size(); ++pos) {
				auto t = get_line_text32(pos);
				if(!t.empty()) {
					while(t.back() == ' ') {
						t.pop_back();
					}
				}
				t.push_back('\n');

				std::string s;
				utils::utf32_to_utf8(t, s);
				if(!fo.write(s)) {
					fo.close();
					return false;
				}
			}

			fo.close();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字のロード
			@param[in]	fn		ファイル名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& fn) noexcept
		{
			utils::file_io fi;
			if(!fi.open(fn, "rb")) {
				return false;
			}

			while(!fi.eof()) {
				auto t = fi.get_line();
				output(t);
			}

			fi.close();

			return true;
		}
	};
}
