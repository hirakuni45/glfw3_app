#pragma once
//=====================================================================//
/*!	@file
	@brief	BDF フォント・ファイルを扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <cstdint>
#include <string>
#include <vector>
#include "utils/bit_array.hpp"
#include "utils/format.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	BDF フォントファイルの入力
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bdf_io {

		uint32_t	lin_code_max_;
		typedef std::vector<uint8_t> PAD;
		PAD			sjis_pad_;

		uint16_t	jis_code_;
		bool		bitmap_;

		int			map_max_;

		uint32_t	jis_count_;
		uint32_t	lin_limit_;

		uint32_t	bbx_width_;
		uint32_t	bbx_height_;

		typedef utils::bit_array BIT_ARRAY;
		BIT_ARRAY	bit_array_;

		bool		ascii_flag_;
		bool		kanji_flag_;

		// http://homepage1.nifty.com/docs/algo/j/jis2sjis.html
		// で収集した変換ソースコード
		static unsigned int jis_to_sjis_(unsigned int jis)
		{
			unsigned int hib, lob;

			hib = (jis >> 8) & 0xff;
			lob = jis & 0xff;
			lob += (hib & 1) ? 0x1f : 0x7d;
			if (lob >= 0x7f) lob++;
			hib = ((hib - 0x21) >> 1) + 0x81;
			if (hib > 0x9f) hib += 0x40;

			return (hib << 8) | lob;
		}

		// sjis コードをリニア表に変換する。
		// 上位バイト： 0x81 to 0x9f, 0xe0 to 0xef
		// 下位バイト： 0x40 to 0x7e, 0x80 to 0xfc
		static uint16_t sjis_to_liner_(uint16_t sjis)
		{
			uint16_t code;
			uint8_t up = sjis >> 8;
			uint8_t lo = sjis & 0xff;
			if(0x81 <= up && up <= 0x9f) {
				code = up - 0x81;
			} else if(0xe0 <= up && up <= 0xef) {
				code = (0x9f + 1 - 0x81) + up - 0xe0;
			} else {
				return 0xffff;
			}
			int loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
			if(0x40 <= lo && lo <= 0x7e) {
				code *= loa;
				code += lo - 0x40;
			} else if(0x80 <= lo && lo <= 0xfc) {
				code *= loa;
				code += 0x7e + 1 - 0x40;
				code += lo - 0x80;
			} else {
				return 0xffff;
			}
			return code;
		}


		static uint32_t mirror_(uint32_t bits, int count)
		{
			uint32_t v = 0;
			uint32_t m = 1 << (count - 1);
			for(int i = 0; i < count; ++i) {
				if(bits & 1) {
					v |= m;
				}
				m >>= 1;
				bits >>= 1;
			}
			return v;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		bdf_io() : lin_code_max_(0), jis_code_(0), bitmap_(false), map_max_(0),
			jis_count_(0), lin_limit_(0),
			bbx_width_(0), bbx_height_(0),
			ascii_flag_(false), kanji_flag_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~bdf_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの横幅の取得
			@return 横幅
		*/
		//-----------------------------------------------------------------//
		uint32_t get_width() const { return bbx_width_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの高さの取得
			@return 高さ
		*/
		//-----------------------------------------------------------------//
		uint32_t get_height() const { return bbx_height_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント辺りのバイト数を取得
			@return バイト数
		*/
		//-----------------------------------------------------------------//
		uint32_t byte_size() const { return (bbx_width_ * bbx_height_ + 7) / 8; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			uint32_t loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
			loa *= (0x9f + 1 - 0x81) + (0xef + 1 - 0xe0);

			lin_code_max_ = loa;

			sjis_pad_.clear();

			jis_code_ = 0;
			bitmap_ = false;

			bbx_width_  = 0;
			bbx_height_ = 0;

			ascii_flag_ = false;
			kanji_flag_ = false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルの読み込み
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& filename)
		{
			utils::file_io fin;
			if(fin.open(filename, "rb") == false) {
				return false;
			}

			bool retcode = true;
			map_max_ = 0;
			while(!fin.eof()) {
				auto line = fin.get_line();
				if(line.empty()) break;
				utils::strings ss = utils::split_text(line, " ");
				if(ss.size() == 5) {
					if(ss[0] == "FONTBOUNDINGBOX") {
						int ww;
						if(!utils::string_to_int(ss[1], ww)) {
							std::cerr << "Error 'FONTBOUNDINGBOX'(width): " << ss[1] << std::endl;
							retcode = false;
						} else {
							bbx_width_ = ww;
						}
						int hh;
						if(!utils::string_to_int(ss[2], hh)) {
							std::cerr << "Error 'FONTBOUNDINGBOX'(height): " << ss[2] << std::endl;
							retcode = false;
						} else {
							bbx_height_ = hh;
						}
						if(sjis_pad_.empty()) {
							sjis_pad_.resize(lin_code_max_ * byte_size());
						}
					}
				} else if(ss.size() == 1) {
					if(bitmap_) {
						if(ss[0] == "ENDCHAR") {
							++jis_count_;
							bool ascii = false;
							if(jis_code_ >= 0 && jis_code_ < 256) ascii = true;
							uint16_t lin = 0;
							uint16_t sjis = 0;
							if(ascii) {
								lin = jis_code_;
								sjis = jis_code_;
								ascii_flag_ = true;
							} else {
								sjis = jis_to_sjis_(jis_code_);
								lin = sjis_to_liner_(sjis);
								kanji_flag_ = true;
							}
							if(!ascii && lin == 0xffff) {
								std::cerr << "Error JIS code map: " << jis_code_
									<< std::endl; 
								retcode = false;
							} else {
								if(lin_limit_ < lin) lin_limit_ = lin;
								if(lin_code_max_ > lin) {
									if(map_max_ < lin) map_max_ = lin;
									int len = bbx_width_ * bbx_height_;
									if(len & 7) {
										// バイト単位になるように埋める
										for(int i = len; i <= (len | 7); ++i) {
											bit_array_.put_bit(0);
										}
									}
									// SJIS 並びのバッファにコピー
									for(uint32_t i = 0; i < byte_size(); ++i) {
										uint32_t ofs_dst = lin * byte_size();
										sjis_pad_[ofs_dst + i] = bit_array_.get_byte(i);
									}
								} else {
									std::cerr << "Out of code area (Shift-JIS: "
										<< sjis << "): " << lin << std::endl;
									retcode = false;
								}
							}
							bitmap_ = false;
						} else {
							uint32_t bits = 0;
							char ch;
							const char *p = ss[0].c_str();
							int n = 0;
							while((ch = *p++) != 0) {
								bits <<= 4;
								if('0' <= ch && ch <= '9') bits |= ch - '0';
								else if('A' <= ch && ch <= 'F') bits |= ch - 'A' + 10;
								else if('a' <= ch && ch <= 'f') bits |= ch - 'a' + 10;
								else {
									n = -1;
									break;
								}
								++n;
							}
							if(n > 0) {
								uint32_t v = mirror_(bits, n * 4);
								bit_array_.put_bits(v, bbx_width_);
							} else {
								std::cerr << "Bitmap hex-decimal decode error (width): "
									<< ss[0] << std::endl;
								retcode = false;
							}
						}
					} else {
						if(ss[0] == "BITMAP") {
							bitmap_ = true;
							bit_array_.clear();
						}
					}
				} else if(ss.size() == 2) {
					if(ss[0] == "ENCODING") {
						int code;
						if(!utils::string_to_int(ss[1], code)) {
							std::cerr << "Error 'ENCODING'(code): " << ss[1] << std::endl;
							retcode = false;
						} else {
							jis_code_ = code;
						}
					}
				}
			}
			fin.close();

			return retcode;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	SJIS 順番によるバイナリー出力
			@param[in]	filename	ファイル名
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& filename)
		{
			utils::file_io fout;

			if(fout.open(filename, "wb") == false) {
				return false;
			}

			if(!sjis_pad_.empty()) {
				if(kanji_flag_) {
					fout.write(&sjis_pad_[0], sjis_pad_.size());
				} else if(ascii_flag_) {
					fout.write(&sjis_pad_[0], 256 * byte_size());
				}
			}

			fout.close();

			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ビットデータ配列の最大サイズを返す
			@return 最大サイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t size() const { return lin_limit_ * byte_size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ビットデータバイトアクセス
			@param[in]	idx	バイト位置
			@return データ
		*/
		//-----------------------------------------------------------------//
		uint8_t get_byte(uint32_t idx) const {
			return sjis_pad_[idx];
		}
	};
}
