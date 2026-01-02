#pragma once
//=========================================================================//
/*!	@file
	@brief	キーボード入力を扱うクラス @n
			glfw3 フレームワークで、キースイッチに対応するビットを生成している @n
			対応するビットは、「押した瞬間」、「離した瞬間」、「押している状態」など @n
			を生成している。 @n
			フレーム毎にサービスを呼び出す事 @n
			キーリピート、CAPSロック、インサート、スクロールロック、など、glfw3 から継承
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

		static constexpr char KEY_CR  = 0x0D;
		static constexpr char KEY_BS  = 0x08;
		static constexpr char KEY_TAB = 0x09;
		static constexpr char KEY_ESC = 0x1B;
		static constexpr char KEY_RIGHT = 'Q' - 0x40;
		static constexpr char KEY_LEFT  = 'R' - 0x40;
		static constexpr char KEY_DOWN  = 'S' - 0x40;
		static constexpr char KEY_UP    = 'T' - 0x40;
		static constexpr char KEY_DEL   = 0x7F;
		static constexpr char KEY_HOME      = -128;
		static constexpr char KEY_INSERT    = -127;
		static constexpr char KEY_END       = -126;
		static constexpr char KEY_PAGE_UP   = -125;
		static constexpr char KEY_PAGE_DOWN = -124;
		static constexpr char KEY_F1        = -123;
		static constexpr char KEY_F2        = -122;
		static constexpr char KEY_F3        = -121;
		static constexpr char KEY_F4        = -120;
		static constexpr char KEY_F5        = -119;
		static constexpr char KEY_F6        = -118;
		static constexpr char KEY_F7        = -117;
		static constexpr char KEY_F8        = -116;
		static constexpr char KEY_F9        = -115;
		static constexpr char KEY_F10       = -114;
		static constexpr char KEY_F11       = -113;
		static constexpr char KEY_F12       = -112;

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
			{ gl::device::key::RIGHT,			KEY_RIGHT,		KEY_RIGHT,		KEY_RIGHT     },
			{ gl::device::key::LEFT,			KEY_LEFT,		KEY_LEFT,		KEY_LEFT      },
			{ gl::device::key::DOWN,			KEY_DOWN,		KEY_DOWN,		KEY_DOWN      },
			{ gl::device::key::UP,				KEY_UP,			KEY_UP,			KEY_UP        },
			{ gl::device::key::ESCAPE,			KEY_ESC,		KEY_ESC,		KEY_ESC       },
			{ gl::device::key::BACKSPACE,		KEY_BS,			KEY_BS,			KEY_BS        },
			{ gl::device::key::TAB,				KEY_TAB,		KEY_TAB,		KEY_TAB       },
			{ gl::device::key::ENTER,			KEY_CR,			KEY_CR,			KEY_CR        },
			{ gl::device::key::DEL,				KEY_DEL,		KEY_DEL,		KEY_DEL       },
			{ gl::device::key::HOME,			KEY_HOME,		KEY_HOME,		KEY_HOME      },
			{ gl::device::key::INSERT,			KEY_INSERT,		KEY_INSERT,		KEY_INSERT    },
			{ gl::device::key::END,				KEY_END,		KEY_END,		KEY_END       },
			{ gl::device::key::PAGE_UP,			KEY_PAGE_UP,	KEY_PAGE_UP,	KEY_PAGE_UP   },
			{ gl::device::key::PAGE_DOWN,		KEY_PAGE_DOWN,	KEY_PAGE_DOWN,	KEY_PAGE_DOWN },
			{ gl::device::key::F1,				KEY_F1,			KEY_F1,			KEY_F1        },
			{ gl::device::key::F2,				KEY_F2,			KEY_F2,			KEY_F2        },
			{ gl::device::key::F3,				KEY_F3,			KEY_F3,			KEY_F3        },
			{ gl::device::key::F4,				KEY_F4,			KEY_F4,			KEY_F4        },
			{ gl::device::key::F5,				KEY_F5,			KEY_F5,			KEY_F5        },
			{ gl::device::key::F6,				KEY_F6,			KEY_F6,			KEY_F6        },
			{ gl::device::key::F7,				KEY_F7,			KEY_F7,			KEY_F7        },
			{ gl::device::key::F8,				KEY_F8,			KEY_F8,			KEY_F8        },
			{ gl::device::key::F9,				KEY_F9,			KEY_F9,			KEY_F9        },
			{ gl::device::key::F10,				KEY_F10,		KEY_F10,		KEY_F10       },
			{ gl::device::key::F11,				KEY_F11,		KEY_F11,		KEY_F11       },
			{ gl::device::key::F12,				KEY_F12,		KEY_F12,		KEY_F12       },
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

			auto l_alt  = dev.get_level(device::key::LEFT_ALT);
			auto r_alt  = dev.get_level(device::key::RIGHT_ALT);
			bool alt = l_alt | r_alt;

			if(dev.get_level(device::key::STATE_REPEAT)) {
				if(alt) ;
				else if(repeat_enable_ && last_char_ < 256) {
					input_ += last_char_;
				}
			} else {
				for(auto& t : key_tbls_) {
					if(dev.get_positive(t.key_type)) {
						if(alt) {

						} else if(ctrl) {
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
			@brief	CAPS LOCK の状態を取得
			@return CAPS LOCK の状態
		*/
		//-----------------------------------------------------------------//
		bool get_caps_lock_state() const noexcept
		{
			using namespace gl;
			auto& core = core::get_instance();
			const auto& dev = core.get_device();
			return dev.get_level(device::key::STATE_CAPS_LOCK);
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
