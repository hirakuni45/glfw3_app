#pragma once
//=========================================================================//
/*!	@file
	@brief	キーボード入力を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2025 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=========================================================================//
#include "utils/string_utils.hpp"
#include "core/glcore.hpp"

namespace sys {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	キーボード・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct keyboard {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	制御コード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct CTRL {
			enum {
				BS    = 0x08,
				CR    = 0x0D,
				RIGHT = 'Q' - 0x40,
				LEFT  = 'R' - 0x40,
				DOWN  = 'S' - 0x40,
				UP    = 'T' - 0x40,
				ESC   = 0x1B,
				DEL   = 0x7F,
			};
		};

	private:
		std::u32string	input_;

		uint32_t	repeat_delay_;
		uint32_t	repeat_delay_cnt_;
		uint32_t	repeat_cycle_;
		uint32_t	repeat_cycle_cnt_;
		int			last_key_idx_;
		bool		repeat_enable_;
		char		last_char_;

		struct key_t {
			gl::device::key	key_type;
			char	normal_code;
			char	shift_code;
		};

		static constexpr key_t key_type_tbls_[] = {
			{ gl::device::key::ESCAPE,        0x1b, 0x1b },
			{ gl::device::key::BACKSPACE,     0x08, 0x08 },
			{ gl::device::key::DEL,           0x7f, 0x7f },
			{ gl::device::key::TAB,           0x09, 0x09 },
			{ gl::device::key::ENTER,         0x0d, 0x0d },
			{ gl::device::key::RIGHT,         'Q'-0x40, 'Q'-0x40 },
			{ gl::device::key::LEFT,          'R'-0x40, 'R'-0x40 },
			{ gl::device::key::DOWN,          'S'-0x40, 'S'-0x40 },
			{ gl::device::key::UP,            'T'-0x40, 'T'-0x40 },
//			{ gl::device::key::INSERT,        0x09, 0x09 },
		};

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		keyboard() : input_(),
			repeat_delay_(60), repeat_delay_cnt_(0),
			repeat_cycle_(3), repeat_cycle_cnt_(0), last_key_idx_(-1), repeat_enable_(true),
			last_char_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~keyboard() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	リピートの許可
			@param[in]	f	不許可の場合「false」
		*/
		//-----------------------------------------------------------------//
		void repeat_enable(bool f = true) { repeat_enable_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リピート・遅延の設定
			@param[in]	frame	フレーム数
		*/
		//-----------------------------------------------------------------//
		void repeat_delay(uint32_t frame) { repeat_delay_ = frame; }


		//-----------------------------------------------------------------//
		/*!
			@brief	リピート・サイクルの設定
			@param[in]	frame	フレーム数
		*/
		//-----------------------------------------------------------------//
		void repeat_cycle(uint32_t frame) { repeat_cycle_ = frame; }


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス @n
					上方（glfw3 フレームワーク）からエコーされるキーボードの状態 @n
					をスキャンして、キー入力として反映する。
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			using namespace gl;

			core& core = core::get_instance();

			input_.clear();

			if(!core.get_recv_text().empty()) {
				input_ += core.get_recv_text();
				core.at_recv_text().clear();
			}

			const device& dev = core.get_device();
			{  // CTRL コードは特別なケアが必要、ALT コードは未設定
				auto left_ctrl = dev.get_level(gl::device::key::LEFT_CONTROL);
				auto right_ctrl = dev.get_level(gl::device::key::RIGHT_CONTROL);
				if(left_ctrl || right_ctrl) {
					for(auto i = static_cast<int>(gl::device::key::A); i <= static_cast<int>(gl::device::key::Z); ++i) {
						if(dev.get_positive(static_cast<gl::device::key>(i))) input_ += 'A' - 0x40 + i - static_cast<int>(gl::device::key::A);
					}
				}
			}
			const key_t* tbl = key_type_tbls_;
			for(int i = 0; i < (sizeof(key_type_tbls_) / sizeof(key_t)); ++i) {
				const key_t& t = tbl[i];
				if(dev.get_positive(t.key_type)) {
					input_ += t.normal_code;
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	入力
			@return 入力されたキーの値
		*/
		//-----------------------------------------------------------------//
		const auto& input() const { return input_; }
	};
}
