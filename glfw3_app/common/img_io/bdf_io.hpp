#pragma once
//=====================================================================//
/*!	@file
	@breif	BDF フォント・ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdio>
#include <vector>

#include "bitio.hpp"

class bdfio {

	std::vector<char>	m_codemap;
	std::vector<unsigned char>	m_bitmaps;

	unsigned short	m_jis_code;
	bool	m_bitmap;

	int		m_map_max;

	int		m_bbx_width;
	int		m_bbx_height;

	bitio	m_bitio;

	bool	m_alignment_bits;

public:
	bdfio() : m_jis_code(0), m_bitmap(false), m_map_max(0),
			  m_bbx_width(0), m_bbx_height(0), m_alignment_bits(8) { }

	~bdfio() { }

	void initialize();

	size_t size() const { return m_bitmaps.size(); }

	const unsigned char* get_array() const { return &m_bitmaps[0]; }

	bool load(const char* filename);

	bool save(const char* filename);

};




