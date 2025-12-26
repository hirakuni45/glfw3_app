#pragma once
//=========================================================================//
/*!	@file
	@brief	キーボード入力を扱うクラス @n
			glfw3 フレームワークで、キースイッチに対応するビットを生成している @n
			対応するビットは、「押した瞬間」、「離した瞬間」、「押している状態」など @n
			を生成している。
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include "utils/string_utils.hpp"
#include "core/glcore.hpp"

namespace sys {

	struct keyboard_def {
		struct key_t {
			gl::device::key	key_type;
			char	normal_code;
			char	shift_code;
			char	ctrl_code;

			constexpr key_t(gl::device::key k = gl::device::key::NONE, char n = 0, char s = 0, char c = 0) noexcept :
				key_type(k),
				normal_code(n),
				shift_code(s),
				ctrl_code(c)
			{ }
		};
	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	キーボード・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct keyboard : public keyboard_def {

		static constexpr uint32_t KEY_CR  = 0x0D;
		static constexpr uint32_t KEY_BS  = 0x08;
		static constexpr uint32_t KEY_TAB = 0x09;
		static constexpr uint32_t KEY_ESC = 0x1B;
		static constexpr uint32_t KEY_RIGHT = 'Q' - 0x40;
		static constexpr uint32_t KEY_LEFT  = 'R' - 0x40;
		static constexpr uint32_t KEY_DOWN  = 'S' - 0x40;
		static constexpr uint32_t KEY_UP    = 'T' - 0x40;
		static constexpr uint32_t KEY_DEL = 0x7F;

	private:
		std::u32string	input_;
		bool			repeat_enable_;
		uint32_t		last_char_;

		static constexpr key_t key_tbls_[] = {
			{ gl::device::key::SPACE,			' ',  ' ',  ' '  },
			{ gl::device::key::APOSTROPHE,		'\'', '"',  0    },	/* ' */
			{ gl::device::key::COMMA,			',',  '<',  0    },	/* , */
			{ gl::device::key::MINUS,			'-',  '_',  0    },	/* - */
			{ gl::device::key::PERIOD,			'.',  '>',  0    },	/* . */
			{ gl::device::key::SLASH,			'/',  '?',  0    },	/* / */
			{ gl::device::key::_0,				'0',  ')',  0    },
			{ gl::device::key::_1,				'1',  '!',  0    },
			{ gl::device::key::_2,				'2',  '@',  0    },
			{ gl::device::key::_3,				'3',  '#',  0    },
			{ gl::device::key::_4,				'4',  '$',  0    },
			{ gl::device::key::_5,				'5',  '%',  0    },
			{ gl::device::key::_6,				'6',  '^',  0    },
			{ gl::device::key::_7,				'7',  '&',  0    },
			{ gl::device::key::_8,				'8',  '*',  0    },
			{ gl::device::key::_9,				'9',  '(',  0    },
			{ gl::device::key::SEMICOLON,		';',  ':',  0    },	/* ; */
			{ gl::device::key::EQUAL,			'=',  '+',  0    },	/* = */
			{ gl::device::key::A,				'A',  'a',  'A' - 0x40  },
			{ gl::device::key::B,				'B',  'b',  'B' - 0x40  },
			{ gl::device::key::C,				'C',  'c',  'C' - 0x40  },
			{ gl::device::key::D,				'D',  'd',  'D' - 0x40  },
			{ gl::device::key::E,				'E',  'e',  'E' - 0x40  },
			{ gl::device::key::F,				'F',  'f',  'F' - 0x40  },
			{ gl::device::key::G,				'G',  'g',  'G' - 0x40  },
			{ gl::device::key::H,				'H',  'h',  'H' - 0x40  },
			{ gl::device::key::I,				'I',  'i',  'I' - 0x40  },
			{ gl::device::key::J,				'J',  'j',  'J' - 0x40  },
			{ gl::device::key::K,				'K',  'k',  'K' - 0x40  },
			{ gl::device::key::L,				'L',  'l',  'L' - 0x40  },
			{ gl::device::key::M,				'M',  'm',  'M' - 0x40  },
			{ gl::device::key::N,				'N',  'n',  'N' - 0x40  },
			{ gl::device::key::O,				'O',  'o',  'O' - 0x40  },
			{ gl::device::key::P,				'P',  'p',  'P' - 0x40  },
			{ gl::device::key::Q,				'Q',  'q',  'Q' - 0x40  },
			{ gl::device::key::R,				'R',  'r',  'R' - 0x40  },
			{ gl::device::key::S,				'S',  's',  'S' - 0x40  },
			{ gl::device::key::T,				'T',  't',  'T' - 0x40  },
			{ gl::device::key::U,				'U',  'u',  'U' - 0x40  },
			{ gl::device::key::V,				'V',  'v',  'V' - 0x40  },
			{ gl::device::key::W,				'W',  'w',  'W' - 0x40  },
			{ gl::device::key::X,				'X',  'x',  'X' - 0x40  },
			{ gl::device::key::Y,				'Y',  'y',  'Y' - 0x40  },
			{ gl::device::key::Z,				'Z',  'z',  'Z' - 0x40  },
			{ gl::device::key::LEFT_BRACKET,	'[',  '{',  '[' - 0x40  },	/* [ (0x5B) */
			{ gl::device::key::BACKSLASH,		'\\', '|',  '\\' - 0x40 },	/* \ (0x5C) */
			{ gl::device::key::RIGHT_BRACKET,	']', '}',  ']' - 0x40  },	/* ] (0x5D) */
			{ gl::device::key::GRAVE_ACCENT,	'`',  '~',  0  },	/* ` */
			{ gl::device::key::RIGHT,			KEY_RIGHT,	KEY_RIGHT,	KEY_RIGHT },
			{ gl::device::key::LEFT,			KEY_LEFT,	KEY_LEFT,	KEY_LEFT  },
			{ gl::device::key::DOWN,			KEY_DOWN,	KEY_DOWN,	KEY_DOWN  },
			{ gl::device::key::UP,				KEY_UP,		KEY_UP,		KEY_UP    },
			{ gl::device::key::ESCAPE,			KEY_ESC,	KEY_ESC,	KEY_ESC   },
			{ gl::device::key::BACKSPACE,		KEY_BS,		KEY_BS,		KEY_BS    },
			{ gl::device::key::TAB,				KEY_TAB,	KEY_TAB,	KEY_TAB   },
			{ gl::device::key::ENTER,			KEY_CR,		KEY_CR,		KEY_CR    },
			{ gl::device::key::DEL,				KEY_DEL,	KEY_DEL,	KEY_DEL   },
			{ gl::device::key::HOME,			-128,  -128,  -128  },  // 0x80
			{ gl::device::key::INSERT,			-127,  -127,  -127  },  // 0x81
			{ gl::device::key::END,				-126,  -126,  -126  },  // 0x82
			{ gl::device::key::PAGE_UP,			-125,  -125,  -125  },  // 0x83
			{ gl::device::key::PAGE_DOWN,		-124,  -124,  -124  },  // 0x84
		};

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		keyboard() noexcept :
			input_(),
			repeat_enable_(true),
			last_char_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~keyboard() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	リピートの許可（初期「有効」）
			@param[in]	f	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable_repeat(bool f = true) noexcept { repeat_enable_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス（毎フレーム呼び出す） @n
					上方（glfw3 フレームワーク）からエコーされるキーボードの状態 @n
					をスキャンして、キー入力として反映する。
		*/
		//-----------------------------------------------------------------//
		void service() noexcept
		{
			using namespace gl;

			auto& core = core::get_instance();

			input_.clear();

			const auto& dev = core.get_device();

			auto l_shift = dev.get_level(device::key::LEFT_SHIFT);
			auto r_shift = dev.get_level(device::key::RIGHT_SHIFT);
			bool shift = l_shift | r_shift;
			bool shift_a2z = shift;
			if(!dev.get_level(device::key::STATE_CAPS_LOCK)) {
				shift_a2z = !shift_a2z;
			}
			auto l_ctrl  = dev.get_level(device::key::LEFT_CONTROL);
			auto r_ctrl  = dev.get_level(device::key::RIGHT_CONTROL);
			bool ctrl = l_ctrl | r_ctrl;
			if(dev.get_level(device::key::STATE_REPEAT)) {
				if(repeat_enable_ && last_char_ < 256) {
					input_ += last_char_;
				}
			} else {
				for(auto& t : key_tbls_) {
					if(dev.get_positive(t.key_type)) {
						if(ctrl) {
							auto c = t.ctrl_code;
							if(c != 0) {
								input_ += c;
							}
						} else {
							auto s = shift;
							if(t.normal_code >= 'A' && t.normal_code <= 'Z') {
								s = shift_a2z;
							}
							if(s) {
								input_ += t.shift_code;
							} else {
								input_ += t.normal_code;
							}
						}
					}
				}
				if(!input_.empty()) {
					last_char_ = input_.back();
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	入力
			@return 入力されたキーの値
		*/
		//-----------------------------------------------------------------//
		const auto& input() const noexcept { return input_; }
	};
}
