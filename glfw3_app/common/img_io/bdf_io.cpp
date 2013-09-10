//=====================================================================//
/*!	@file
	@breif	BDF フォント・ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <string.h>
#include "bdfio.hpp"
#include "fileio.hpp"

// http://homepage1.nifty.com/docs/algo/j/jis2sjis.html
// で収集した変換ソースコード
static unsigned int jis_to_sjis(unsigned int jis)
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
static unsigned short sjis_to_liner(unsigned short sjis)
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


static unsigned int mirror(unsigned int bits, int count)
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


void bdfio::initialize()
{
	size_t loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
	loa *= (0x9f + 1 - 0x81) + (0xef + 1 - 0xe0);
	m_codemap.resize(loa);
	for(size_t i = 0; i < loa; ++i) m_codemap[i] = 0;

	m_bitmaps.resize(loa * 18);
	memset(&m_bitmaps[0], m_bitmaps.size(), 0xff);

	m_jis_code = 0;
	m_bitmap = false;

	m_bbx_width = 0;
	m_bbx_height = 0;
}


bool bdfio::load(const char* filename)
{
	fio::fileio fin;

	if(fin.open(filename, "rb") == false) {
		return false;
	}

	int retcode = true;
	std::string line;
	m_map_max = 0;
	while(fin.get_line(line) == true) {
		fio::strings ss;
		fio::split_text(line.c_str(), " ", ss);
		if(ss.size() == 5) {
			if(ss[0] == "FONTBOUNDINGBOX") {
				sscanf(ss[1].c_str(), "%d", &m_bbx_width);
				sscanf(ss[2].c_str(), "%d", &m_bbx_height);
			}
		} else if(ss.size() == 1) {
			if(m_bitmap) {
				if(ss[0] == "ENDCHAR") {
					unsigned short sjis = jis_to_sjis(m_jis_code);
					unsigned short lin = sjis_to_liner(sjis);
					if(lin == 0xffff) {
						fprintf(stderr, "Error JIS code map: %04X\n", m_jis_code);
						retcode = false;
					} else {
						if(m_codemap.size() > lin) {
							if(m_map_max < lin) m_map_max = lin;
							m_codemap[lin] = 1;
							int len = m_bbx_width * m_bbx_height;
							if(m_bitio.get_limit() == len) {
								if(len & 7) {
									// バイト単位になるように埋める
									for(int i = len; i <= (len | 7); ++i) {
										m_bitio.put_bit(0);
									}
									len |= 7;
									++len;
								}
								len >>= 3;
								memcpy(&m_bitmaps[lin * len], m_bitio.get_array(), len);
							} else {
								fprintf(stderr, "SJIS: %04X ->12 x 12 pixmap fail: %d\n",
										sjis, m_bitio.get_pos());
								retcode = false;
							}
						} else {
							fprintf(stderr, "Linear code area over (Shift-JIS: %04X): %d\n", sjis, lin);
							retcode = false;
						}
					}
					m_bitmap = false;
				} else {
					unsigned int bits;
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
						unsigned int v = mirror(bits, n * 4);
						m_bitio.put_bits(v, m_bbx_width);
					} else {
						fprintf(stderr, "Bitmap hex-decimal decode error (width): '%s'\n", ss[0].c_str());
						retcode = false;
					}
				}
			} else {
				if(ss[0] == "BITMAP") {
					m_bitmap = true;
					m_bitio.clear();
				}
			}
		} else if(ss.size() == 2) {
			if(ss[0] == "ENCODING") {
				int code;
				if(sscanf(ss[1].c_str(), "%d", &code) == 1) {
					m_jis_code = code;
				}
			}
		}
		line.clear();
	}
	fin.close();

//	unsigned short cd = jis_to_sjis(0x5e21);
//	printf("SJIS: %04X\n", cd);

	int use = 0;
	for(int i = 0; i < m_codemap.size(); ++i) {
		if(m_codemap[i]) ++use;
	}
	printf("Used bitmap: %d / %d\n", use, m_codemap.size());
	printf("Linear code max: %d\n", m_map_max);

	return retcode;
}


bool bdfio::save(const char* filename)
{
	fio::fileio fout;

	if(fout.open(filename, "wb") == false) {
		return false;
	}

	fout.write(&m_bitmaps[0], m_map_max * 18);

	fout.close();

	return false;
}
