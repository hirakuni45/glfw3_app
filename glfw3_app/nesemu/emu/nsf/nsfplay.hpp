#pragma once
//=====================================================================//
/*! @file
	@brief  NSF Player クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/file_io.hpp"
#include "utils/format.hpp"

#include "emu/cpu/nes6502.h"

namespace emu {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	nesemu シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class nsfplay {

		struct info_t {
			char		sig_[5];		///< シグネチュア（"MESM", 0x1A）
			uint8_t		version_;		///< バージョン番号 (現在は 01h 固定)
			uint8_t		music_num_;		///< 全曲数 (1=1 全１曲, 2=2 全２曲, etc)
			uint8_t		start_no_;		///< 開始曲番号 (1=1番目の曲, 2=2番目の曲, etc)
			uint16_t	load_org_;		///< (low/high) ロード開始アドレス (8000 〜 FFFF)
			uint16_t	init_org_;		///< (low/high) 初期化開始アドレス (8000 〜 FFFF)
			uint16_t	play_org_;		///< (low/high) 曲再生開始アドレス (8000 〜 FFFF)
			char		m_name_[32];	///< 曲名（null終端文字列）
			char		m_artist_[32];	///< アーティスト名[分かっている場合のみ]（null終端文字列）
			char		m_cpr_[32];		///< 著作者情報（null終端文字列）
			uint16_t	speed_ntsc_;	///< (low/high) スピード, 1 / 1,000,000回/秒 NTSC用 (曲の再生の項目参照)
			uint8_t		init_bank_[8];	///< バンク切り替え用初期値 (後述の説明参照, FDSの項目)
			uint16_t	speed_pal_;		///< (low/high) スピード, 1 / 1,000,000回/秒 PAL用  (曲の再生の項目参照)
			uint8_t		pal_ntsc_;		///< PAL/NTSCの設定:
										///  bit 0    : 0の時、NTSCモード
										///  bit 0    : 1の時、PALモード
										///  bit 1    : 1の時、デュアルモード（PAL/NTSC）
										///  bit 2〜7 : 未使用（0で固定）
			uint8_t		ext_chip_;		///< 拡張サウンドチップサポート
										///  bit 0    : 1の時、VRCVI
										///  bit 1    : 1の時、VRCVII
										///  bit 2    : 1の時、FDS Sound
										///  bit 3    : 1の時、MMC5 audio
										///  bit 4    : 1の時、Namco 106
										///  bit 5    : 1の時、Sunsoft FME-07
										///  bit 6・7 : 将来の拡張用（0で固定）
			uint8_t		ext_[4];		///< 拡張用バイト列（0で固定）

			bool probe() const {
				if(sig_[0] == 'N' && sig_[1] == 'E' && sig_[2] == 'S' && sig_[3] == 'M' && sig_[4] == 0x1a) {
					return true;
				} else {
					return false;
				}
			}
		};

		info_t		info_;

		nes6502_context* cpu_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		nsfplay() : info_(), cpu_(nullptr) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  オープン
			@param[in]	filename	ファイル名
			@return エラーなら「false」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& filename)
		{
			utils::file_io fi;
			if(!fi.open(filename, "rb")) {
				return false;
			}

			if(fi.read(&info_, 128) != 128) {
				return false;
			}
			if(!info_.probe()) {
				return false;
			}

			utils::format("NSF file:\n");

			cpu_ = nes6502_getcontext();

			return true;
		}




	};
}
