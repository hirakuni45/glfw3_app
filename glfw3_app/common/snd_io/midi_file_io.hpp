#pragma once
//=====================================================================//
/*!	@file
	@brief	MIDI ファイルを扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2020 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include "utils/format.hpp"
#include "utils/file_io.hpp"

#include <string>

namespace snd {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	MIDI ファイル入出力クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class midi_file_io {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	MIDI ファイル・タグ
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct tag_t {
			typedef std::string STR;

			STR		text_;
			STR		copyright_;
			STR		name_;
			STR		instrument_;
			STR		lylic_;

			void clear()
			{
				text_.clear();
				copyright_.clear();
				name_.clear();
				instrument_.clear();
				lylic_.clear();
			}
		};

	private:
		utils::file_io	fin_;

		uint16_t	format_;
		uint16_t	track_num_;
		uint16_t	time_unit_;

		uint32_t	size_;
		uint32_t	pos_;

		tag_t		tag_;

		uint16_t get16_(const void* p) {
			const uint8_t* vp = static_cast<const uint8_t*>(p);
			uint16_t v = vp[0];
			v <<= 8;
			v |= vp[1];
			return v;		
		}

		uint32_t get32_(const void* p) {
			const uint8_t* vp = static_cast<const uint8_t*>(p);
			uint32_t v = vp[0];
			v <<= 8;
			v |= vp[1];
			v <<= 8;
			v |= vp[2];
			v <<= 8;
			v |= vp[3];
			return v;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		midi_file_io() noexcept : fin_(),
			format_(0), track_num_(0), time_unit_(0),
			size_(0), pos_(0), tag_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	MIDI ファイルをオープンする。
			@param[in]	file	ファイル名
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const char* file) noexcept
		{
			if(!fin_.open(file, "rb")) return false;

			static const char id[4] = { 'M', 'T', 'h', 'd' };
			char tmp[14 + 8];
			if(fin_.read(tmp, sizeof(tmp)) != sizeof(tmp)) {
				return false;
			}
			if(strncmp(tmp, id, 4) != 0) {
				return false;
			}
			if(tmp[4] == 0x00 && tmp[5] == 0x00 && tmp[6] == 0x00 && tmp[7] == 0x06) {
			} else {
				return false;
			}

			format_ = get16_(&tmp[8]);
			track_num_ = get16_(&tmp[10]);
			time_unit_ = get16_(&tmp[12]);

			static const char rk[4] = { 'M', 'T', 'r', 'k' };
			if(strncmp(&tmp[14], rk, 4) != 0) {
				return false;
			}
			size_ = get32_(&tmp[14 + 4]);

			tag_.clear();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フレーム・サービス
		*/
		//-----------------------------------------------------------------//
		void service() noexcept
		{
			if(!fin_.is_open()) return;

			if(fin_.eof()) {
				fin_.close();
				return;
			}

			uint8_t tmp[4];
			if(fin_.read(tmp, sizeof(tmp)) != sizeof(tmp)) {
				fin_.close();
				return;
			}
			pos_ += 4;

			switch(tmp[1]) {
			case 0xf0:  // システムエクスクルーシブメッセージ
				{
					uint8_t msg[256];
					auto l = static_cast<uint8_t>(tmp[2]);
					--l;
					if(fin_.read(msg, l) != l) {
						fin_.close();
						break;
					}
					pos_ += l;
				}
				break;
			case 0xff:
				if(tmp[2] == 0x00) {  // シーケンス番号

				} else if(tmp[2] == 0x2f && tmp[3] == 0x00) {  // End of track
					fin_.close();
				} else if(tmp[2] == 0x51) {  // Set Tempo
					auto l = static_cast<uint8_t>(tmp[3]);
					if(l != 3) {
						fin_.close();
						break;
					}
					uint8_t val[3];
					if(fin_.read(val, l) != l) {
						fin_.close();
						break;
					}
					pos_ += l;
				} else if(tmp[2] == 0x58) {  // Time Signature
					auto l = static_cast<uint8_t>(tmp[3]);
					if(l != 4) {
						fin_.close();
						break;
					}
					uint8_t val[4];
					if(fin_.read(val, l) != l) {
						fin_.close();
						break;
					}
					pos_ += l;
				} else if(tmp[2] == 0x59) {  // Key Signature
					auto l = static_cast<uint8_t>(tmp[3]);
					if(l != 2) {
						fin_.close();
						break;
					}
					uint8_t val[2];
					if(fin_.read(val, l) != l) {
						fin_.close();
						break;
					}
					pos_ += l;
				} else {
					auto l = static_cast<uint8_t>(tmp[3]);
					char str[256];
					if(fin_.read(str, l) != l) {
						fin_.close();
						break;
					}
					pos_ += l;
					str[l] = 0;
					if(tmp[2] == 0x01) {  // テキスト
						tag_.text_ += str;
					} else if(tmp[2] == 0x02) {  // copyright
						tag_.copyright_ += str;
					} else if(tmp[2] == 0x03) {  // シーケンス名・トラック名
						tag_.name_ += str;
					} else if(tmp[2] == 0x04) {  // 楽器名
						tag_.instrument_ += str;
					} else if(tmp[2] == 0x05) {  // 歌詞
						tag_.lylic_ += str;

					} else { 
// utils::format("---------------------\n");
					}
				}
utils::format("%02X %02X %02X %02X\n")
	% static_cast<uint16_t>(tmp[0])
	% static_cast<uint16_t>(tmp[1])
	% static_cast<uint16_t>(tmp[2])
	% static_cast<uint16_t>(tmp[3]);
				break;
			default:
utils::format("%02X %02X %02X %02X\n")
	% static_cast<uint16_t>(tmp[0])
	% static_cast<uint16_t>(tmp[1])
	% static_cast<uint16_t>(tmp[2])
	% static_cast<uint16_t>(tmp[3]);
				break;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	タグの参照
			@return タグ
		*/
		//-----------------------------------------------------------------//
		const auto& get_tag() const noexcept { return tag_; } 
	};
}
