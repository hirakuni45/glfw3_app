#pragma once
//=====================================================================//
/*!	@file
	@brief	デバイス用ラッパークラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <bitset>
#include "utils/vtx.hpp"

namespace gl {

	// windows で define されてるので無効にする
	#undef DELETE

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	device クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class device {
	public:
		struct locator {
			vtx::spos	cursor_;
			vtx::spos	scroll_;
			locator() : cursor_(0), scroll_(0) { }
		};

		struct key {
			enum type {
				MOUSE_LEFT,			///< mouse button left
				MOUSE_MIDDLE,		///< mouse button middle
				MOUSE_RIGHT,		///< mouse button right
				MOUSE_FOCUS = 8,	///< mouse focus

				GAME_UP    = 12,	///< game up
				GAME_DOWN  = 13,	///< game down
				GAME_RIGHT = 14,	///< game right
				GAME_LEFT  = 15,	///< game left
				GAME_0     = 16,	///< game 0
				GAME_1     = 17,	///< game 1
				GAME_2     = 18,	///< game 2
				GAME_3     = 19,	///< game 3
				GAME_4     = 20,	///< game 4
				GAME_5     = 21,	///< game 5
				GAME_6     = 22,	///< game 6
				GAME_7     = 23,	///< game 7
				GAME_8     = 24,	///< game 8
				GAME_9     = 25,	///< game 9
				GAME_10    = 26,	///< game 10
				GAME_11    = 27,	///< game 11
				GAME_12    = 28,	///< game 12
				GAME_13    = 29,	///< game 13
				GAME_14    = 30,	///< game 14
				GAME_15    = 31,	///< game 15

				SPACE       = 32,
				APOSTROPHE  = 39,	/* ' */
				COMMA       = 44,	/* , */
				MINUS       = 45,	/* - */
				PERIOD      = 46,	/* . */
				SLASH       = 47,	/* / */
				_0          = 48,
				_1          = 49,
				_2          = 50,
				_3          = 51,
				_4          = 52,
				_5          = 53,
				_6          = 54,
				_7          = 55,
				_8          = 56,
				_9          = 57,
				SEMICOLON   = 59,	/* ; */
				EQUAL       = 61,	/* = */

				A             = 65,
				B             = 66,
				C             = 67,
				D             = 68,
				E             = 69,
				F             = 70,
				G             = 71,
				H             = 72,
				I             = 73,
				J             = 74,
				K             = 75,
				L             = 76,
				M             = 77,
				N             = 78,
				O             = 79,
				P             = 80,
				Q             = 81,
				R             = 82,
				S             = 83,
				T             = 84,
				U             = 85,
				V             = 86,
				W             = 87,
				X             = 88,
				Y             = 89,
				Z             = 90,
				LEFT_BRACKET  = 91,		/* [ */
				BACKSLASH     = 92,		/* \ */
				RIGHT_BRACKET = 93,		/* ] */
				GRAVE_ACCENT  = 96,		/* ` */

				WORLD_1 = 161,			/* non-US #1 */
				WORLD_2 = 162,			/* non-US #2 */

				ESCAPE    = 256,
				ENTER     = 257,
				TAB       = 258,
				BACKSPACE = 259,
				INSERT    = 260,
				DELETE    = 261,
				RIGHT     = 262,
				LEFT      = 263,
				DOWN      = 264,
				UP        = 265,
				PAGE_UP   = 266,
				PAGE_DOWN = 267,
				HOME      = 268,
				END       = 269,

				CAPS_LOCK    = 280,
				SCROLL_LOCK  = 281,
				NUM_LOCK     = 282,
				PRINT_SCREEN = 283,
				PAUSE        = 284,

				F1  = 290,
				F2  = 291,
				F3  = 292,
				F4  = 293,
				F5  = 294,
				F6  = 295,
				F7  = 296,
				F8  = 297,
				F9  = 298,
				F10 = 299,
				F11 = 300,
				F12 = 301,
				F13 = 302,
				F14 = 303,
				F15 = 304,
				F16 = 305,
				F17 = 306,
				F18 = 307,
				F19 = 308,
				F20 = 309,
				F21 = 310,
				F22 = 311,
				F23 = 312,
				F24 = 313,
				F25 = 314,

				KP_0 = 320,
				KP_1 = 321,
				KP_2 = 322,
				KP_3 = 323,
				KP_4 = 324,
				KP_5 = 325,
				KP_6 = 326,
				KP_7 = 327,
				KP_8 = 328,
				KP_9 = 329,
				KP_DECIMAL  = 330,
				KP_DIVIDE   = 331,
				KP_MULTIPLY = 332,
				KP_SUBTRACT = 333,
				KP_ADD      = 334,
				KP_ENTER    = 335,
				KP_EQUAL    = 336,

				LEFT_SHIFT    = 340,
				LEFT_CONTROL,
				LEFT_ALT,
				LEFT_SUPER,
				RIGHT_SHIFT,
				RIGHT_CONTROL,
				RIGHT_ALT,
				RIGHT_SUPER,
				SHIFT,
				CONTROL,
				ALT,
				SUPER,
				MENU,
			};
		};

		static const int device_bit_num_ = 350; 
		typedef std::bitset<device_bit_num_>	bitsets;

	private:
		bitsets	level_;
		bitsets	positive_;
		bitsets	negative_;

		locator	locator_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	device クラス・コンストラクター
		*/
		//-----------------------------------------------------------------//
		device() : level_(), positive_(), negative_(), locator_() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	レベルの取得
			@return bitsets クラスの参照
		*/
		//-----------------------------------------------------------------//
		const bitsets& get_level() const { return level_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief	レベルの取得
			@param[in]	t	key::type	
			@return 値
		*/
		//-----------------------------------------------------------------//
		bool get_level(key::type t) const { return level_[t]; } 


		//-----------------------------------------------------------------//
		/*!
			@brief	押した瞬間の取得
			@return bitsets クラスの参照
		*/
		//-----------------------------------------------------------------//
		const bitsets& get_positive() const { return positive_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief	押した瞬間の取得
			@param[in]	t	key::type	
			@return 値
		*/
		//-----------------------------------------------------------------//
		bool get_positive(key::type t) const { return positive_[t]; } 


		//-----------------------------------------------------------------//
		/*!
			@brief	離した瞬間の取得
			@return bitsets クラスの参照
		*/
		//-----------------------------------------------------------------//
		const bitsets& get_negative() const { return negative_; } 


		//-----------------------------------------------------------------//
		/*!
			@brief	離した瞬間の取得
			@param[in]	t	key::type	
			@return 値
		*/
		//-----------------------------------------------------------------//
		bool get_negative(key::type t) const { return negative_[t]; } 


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_cursor() const { return locator_.cursor_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクロール位置を取得
			@return スクロール位置
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_scroll() const { return locator_.scroll_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス@n
					サンプリングと状態の作成
			@param[in]	bits	スイッチの状態
			@param[in]	poss	位置情報
		*/
		//-----------------------------------------------------------------//
		void service(const bitsets& bits, const locator& poss);



	};
}
