//=====================================================================//
/*!	@file
	@brief	BDF フォント・ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "img_io/bdf_io.hpp"
#include "utils/string_utils.hpp"
#include <boost/lexical_cast.hpp>

namespace img {

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
	static unsigned short sjis_to_liner_(unsigned short sjis)
	{
		unsigned short code;
		unsigned char up = sjis >> 8;
		unsigned char lo = sjis & 0xff;
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


	static unsigned int mirror_(unsigned int bits, int count)
	{
		unsigned int v = 0;
		unsigned int m = 1 << (count - 1);
		for(int i = 0; i < count; ++i) {
			if(bits & 1) {
				v |= m;
			}
			m >>= 1;
			bits >>= 1;
		}
		return v;
	}


	void bdf_io::initialize()
	{
		size_t loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
		loa *= (0x9f + 1 - 0x81) + (0xef + 1 - 0xe0);
		codemap_.reserve(loa);
		codemap_.clear();

		jis_code_ = 0;
		bitmap_ = false;

		bbx_width_  = 0;
		bbx_height_ = 0;
	}


	bool bdf_io::load(const std::string& filename)
	{
		utils::file_io fin;
		if(fin.open(filename, "rb") == false) {
			return false;
		}

		bool retcode = true;
		std::string line;
		map_max_ = 0;
		while(fin.get_line(line) == true) {
			utils::strings ss;
			utils::split_text(line, " ", ss);
			if(ss.size() == 5) {
				if(ss[0] == "FONTBOUNDINGBOX") {
					bbx_width_  = boost::lexical_cast<int>(ss[1]);
					bbx_height_ = boost::lexical_cast<int>(ss[2]);
				}
			} else if(ss.size() == 1) {
				if(bitmap_) {
					if(ss[0] == "ENDCHAR") {
						unsigned short sjis = jis_to_sjis_(jis_code_);
						unsigned short lin = sjis_to_liner_(sjis);
						if(lin == 0xffff) {
							std::cerr << "Error JIS code map: " << jis_code_
								<< std::endl; 
							retcode = false;
						} else {
							if(codemap_.size() > lin) {
								if(map_max_ < lin) map_max_ = lin;
								codemap_[lin] = 1;
								int len = bbx_width_ * bbx_height_;
								if(len & 7) {
									// バイト単位になるように埋める
									for(int i = len; i <= (len | 7); ++i) {
										bit_array_.put_bit(0);
									}
									len |= 7;
									++len;
								}
								// memcpy(&m_bitmaps[lin * len], m_bitio.get_array(), len);
							} else {
								std::cerr << "Linear code area over (Shift-JIS: "
									<< sjis << "): " << lin << std::endl;
								retcode = false;
							}
						}
						bitmap_ = false;
					} else {
						uint32_t bits;
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
					jis_code_ = boost::lexical_cast<int>(ss[1]);
				}
			}
			line.clear();
		}
		fin.close();

		//	unsigned short cd = jis_to_sjis(0x5e21);
		//	printf("SJIS: %04X\n", cd);
		int use = 0;
		for(uint32_t i = 0; i < codemap_.size(); ++i) {
			if(codemap_[i]) ++use;
		}
		std::cout << "Used bitmap: " << use << " / " << codemap_.size() << std::endl;
		std::cout << "Linear code max: " << map_max_ << std::endl;
		return retcode;
	}


	bool bdf_io::save(const std::string& filename)
	{
		utils::file_io fout;

		if(fout.open(filename, "wb") == false) {
			return false;
		}

		fout.write(&bitmaps_[0], bitmaps_.size());

		fout.close();

		return false;
	}
}
