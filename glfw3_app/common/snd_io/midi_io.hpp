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
		@brief	MIDI 入出力クラス @n
				※現在、Windows 専用（winmm library）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class midi_io {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	KEY 型 @n
				x: -1, 0, 1, 2, 3, 4, 5, 6, 7、8、9 (Octave 国際式) @n
				 0: xC  (-1: 8.2, 0:16.4, 1:32.7, 2: 65.4, 3:130.8, 4:261.6 ... Hz) @n
				 1: xC# (-1: 8.7, 0:17.3, 1:34.6, 2: 69.3, 3:138.6, 4:277.2 ... Hz) @n
				 2: xD  (-1: 9.2, 0:18.4, 1:36.7, 2: 73.4, 3:146.8, 4:293.7 ... Hz) @n
				 3: xD# (-1: 9.7, 0:19.4, 1:38.9, 2: 77.8, 3:155.6, 4:311.1 ... Hz) @n
				 4: xE  (-1:10.3, 0:20.6, 1:41.2, 2: 82.4, 3:164.8, 4:329.6 ... Hz) @n
				 5: xF  (-1:10.9, 0:21.8, 1:43.7, 2: 87.3, 3:174.6, 4:349.2 ... Hz) @n
				 6: xF# (-1:11.6, 0:23.1, 1:46.2, 2: 92.5, 3:185.0, 4:370.0 ... Hz) @n
				 7: xG  (-1:12.2, 0:24.5, 1:49.0, 2: 98.0, 3:196.0, 4:392.0 ... Hz) @n
				 8: xG# (-1:13.0, 0:26.0, 1:51.9, 2:103.8, 3:207.7, 4:415.3 ... Hz) @n
				 9: xA  (-1:13.8, 0:27.5, 1:55.0, 2:110.0, 3:220.0, 4:440.0 ... Hz) @n
				10: xA# (-1:14.6, 0:29.1, 1:58.3, 2:116.5, 3:233.1, 4:466.2 ... Hz) @n
				11: xB  (-1:15.4, 0:30.9, 1:61.7, 2:123.5, 3:246.9, 4:493.9 ... Hz)
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class key : uint8_t {
			C,		///< C  ド
			Cs,		///< C# ド＃
			D,		///< D  レ
			Ds,		///< D# レ＃
			E,		///< E  ミ
			F,		///< F  ファ
			Fs,		///< F# ファ＃
			G,		///< G  ソ
			Gs,		///< G# ソ＃
			A,		///< A  ラ
			As,		///< A# ラ＃
			B,		///< B  シ
		};


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

			//-------------------------------------------------------------//
			/*!
				@brief	国際式オクターブを取得（-1 to 9)
				@return 国際式オクターブ
			*/
			//-------------------------------------------------------------//
			int8_t get_octave() const noexcept { return static_cast<int8_t>(note / 12) - 1; }


			//-------------------------------------------------------------//
			/*!
				@brief	KEY を取得（-1 to 9)
				@return KEY
			*/
			//-------------------------------------------------------------//
			key get_key() const noexcept { return static_cast<key>(note % 12); }
		};

		typedef utils::fixed_fifo<midi_t, 4096> MIDI_FIFO;

	private:
		int32_t		id_;
		HMIDIIN		device_;

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
			@brief	MIDI デバイス名を取得
			@return MIDI デバイス名
		*/
		//-----------------------------------------------------------------//
		static std::string get_device_name(int32_t id)
		{
			std::string name;
			if(id < 0 || id >= midiInGetNumDevs()) {
				return name;
			}

			MIDIINCAPS caps;
			midiInGetDevCaps(id, &caps, sizeof(MIDIINCAPS));
			name = caps.szPname;

			return name;
		}


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
			@brief	MIDI デバイスが有効か検査
			@return 有効な場合「true」
		*/
		//-----------------------------------------------------------------//
		bool probe() const {
			if(id_ >= 0 && device_ != nullptr) return true;
			else return false;
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
utils::format("MIDI start: %d\n") % id;

			if(id < 0 || id >= midiInGetNumDevs()) {
				return false;
			}

			end();

			auto ret = midiInOpen(&device_, id, (DWORD_PTR)in_task_, (DWORD_PTR)&midi_fifo_,
				CALLBACK_FUNCTION);
			if(ret != MMSYSERR_NOERROR) {
utils::format("MIDI open err\n");
				return false;
			}

			if(midiInStart(device_) != MMSYSERR_NOERROR) {
				midiInClose(device_);
				device_ = nullptr;
utils::format("MIDI start err\n");
				return false;
			}
			id_ = id;

			utils::format("MIDI start: %s\n") % get_device_name(id_).c_str();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI デバイス開始
			@param[in]	key	デバイス・キーワード
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool start(const std::string& key)
		{
			return start(scan_id(key.c_str()));
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
