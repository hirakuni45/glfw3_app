#pragma once
//=====================================================================//
/*!	@file
	@brief	MIDI を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <windows.h>
#include "utils/format.hpp"

namespace snd {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	MIDI クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class midi_io {


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		midi_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイスの数を取得
			@return MIDI デバイスの数
		*/
		//-----------------------------------------------------------------//
		static uint32_t get_device_num() { return midiInGetNumDevs(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイスの識別子を探す
			@param[in]	key	識別子のキーワード
			@return 識別子を返す（見つからない場合、負の値）
		*/
		//-----------------------------------------------------------------//
		static int32_t scan_id(const char* key)
		{
			auto num = midiInGetNumDevs();
			if(num == 0) {
				return -1;
			}

			for(uint32_t i = 0; i < num; ++i) {
				MIDIINCAPS caps;
				midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
				std::string s = caps.szPname;
				if(s.find(key) != std::string::npos) {
					return static_cast<int32_t>(i);
				}
			}
			return -1;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイス・サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイスのリスト表示
		*/
		//-----------------------------------------------------------------//
		static void list_device()
		{
			auto num = midiInGetNumDevs();
			if(num == 0) {
				return;
			}

			for(uint32_t i = 0; i < num; ++i) {
				MIDIINCAPS caps;
				midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
				utils::format("(%d): '%s'\n") % i % caps.szPname;
			}
		}
	};
}
