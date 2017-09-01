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
#include <mmsystem.h>
#include <functional>
#include "utils/fixed_fifo.hpp"
#include "utils/format.hpp"

namespace snd {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	MIDI 入出力クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class midi_io {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	MIDI メッセージ構造
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct midi_t {
			uint8_t		note;
			uint8_t		velocity;
			uint8_t		status;
			uint16_t	time;
		};

		typedef utils::fixed_fifo<midi_t, 4096> MIDI_FIFO;

	private:
		int32_t		id_;
		HMIDIIN		device_;

		/* MIDI Note 番号対応
			0: -1C  (8.2)
			1: -1C# (8.7)
			2: -1D  (9.2)
			3: -1D# (9.7)
			4: -1E  (10.3)
			5: -1F  (10.9)
			6: -1F# (11.6)
			7: -1G  (12.2)
			8: -1G# (13.0)
			9: -1A  (13.8)
		   10: -1A# (14.6)
		   11: -1B  (15.4)
		*/

		MIDI_FIFO	midi_fifo_;

		static void in_task_(HMIDIIN h, UINT msg, DWORD_PTR instance, DWORD param1, DWORD param2)
		{
			MIDI_FIFO* fifo = (MIDI_FIFO*)instance;
			if(fifo == nullptr) return;

			switch(msg) {
			case MIM_OPEN:
				break;
			case MIM_CLOSE:
				break;
			case MIM_DATA:
				{
				// param1: B0-B7:   MIDI status
				// param1: B8-B15:  1ST byte MIDI data (ノート番号、音階）
				// param1: B16-B23: 2ND byte MIDI data (ベロシティ、「０」だと消音）
				// param2: Timestamp (B0-B15)
					midi_t& t = fifo->put_at();
					t.status = param1;
					t.note = param1 >> 8;
					t.velocity = param1 >> 16;
					t.time = param2;
					fifo->put_go();
//				utils::format("%08X, %08X\n") % static_cast<uint32_t>(param1) % static_cast<uint32_t>(param2);
				}
				break;
			case MIM_LONGDATA:
				break;
			case MIM_ERROR:
				break;
			case MIM_LONGERROR:
				break;
			case MIM_MOREDATA:
				break;
			default:
				break;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		midi_io() : id_(-1), device_(nullptr) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~midi_io() { end(); }


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
			@brief	MIDI デバイス開始
			@param[in]	id	デバイス識別子
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start(int32_t id)
		{
			if(id < 0 || id >= midiInGetNumDevs()) {
				return false;
			}

			auto ret = midiInOpen(&device_, id, (DWORD_PTR)in_task_, (DWORD_PTR)&midi_fifo_,
				CALLBACK_FUNCTION);
			if(ret != MMSYSERR_NOERROR) {
				return false;
			}

			if(midiInStart(device_) != MMSYSERR_NOERROR) {
				midiInClose(device_);
				device_ = nullptr;
				return false;
			}
			id_ = id;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイス開始
			@param[in]	key	デバイス・キーワード
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start(const char* key)
		{
			return start(scan_id(key));
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイスをリセット
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool reset()
		{
			if(id_ < 0 || device_ == nullptr) {
				return false;
			}

			midiInReset(device_);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイスを終了
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool end()
		{
			if(id_ < 0 || device_ == nullptr) {
				return false;
			}

			midiInStop(device_);

			midiInClose(device_);
			id_ = -1;
			device_ = nullptr;

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI 入力バッファへの参照
		*/
		//-----------------------------------------------------------------//
		MIDI_FIFO& at_midi_in()
		{
			return midi_fifo_;
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
