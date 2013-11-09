//=====================================================================//
/*!	@file
	@brief	TGA 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/tga_io.hpp"
#include <tga.h>

namespace img {

	static int getc_(void* context)
	{
		char ch;
		if(static_cast<utils::file_io*>(context)->get(ch)) {
			return ch;
		} else {
			return -1;
		}
	}

	static size_t read_(void* ptr, size_t size, void* context)
	{
		return static_cast<utils::file_io*>(context)->read(ptr, size);
	}

	static int putc_(char ch, void* context)
	{
		if(static_cast<utils::file_io*>(context)->put(ch)) {
			return 0;
		} else {
			return -1;
		}
	}

	static size_t write_(const void* ptr, size_t size, void* context)
	{
		return static_cast<utils::file_io*>(context)->write(ptr, size);
	}

	static int seek_(size_t offset, int mode, void* context)
	{
		using namespace utils;

		bool f = false;
		switch(mode) {
		case SEEK_SET:
			f = static_cast<file_io*>(context)->seek(offset, file_io::seek::set);
			break;
		case SEEK_CUR:
			f = static_cast<file_io*>(context)->seek(offset, file_io::seek::cur);
			break;
		case SEEK_END:
			f = static_cast<file_io*>(context)->seek(offset, file_io::seek::end);
			break;
		default:
			break;
		}
		if(f) return 0;
		else return -1;
	}

	static size_t tell_(void* context)
	{
		return static_cast<utils::file_io*>(context)->tell();
	}

	static TGA* create_(utils::file_io* fin)
	{
		TGA* tga = TGAInit();

		tga->fd = fin;

		tga->file.getc = getc_;
		tga->file.read = read_;
		tga->file.putc = putc_;
		tga->file.write = write_;
		tga->file.seek = seek_;
		tga->file.tell = tell_;

		return tga;
	}

	static TGA* info_(utils::file_io* fin, img::img_info& fo)
	{
		TGA* tga = create_(fin);
		if(TGAReadHeader(tga) != TGA_OK) {
			TGAFree(tga);
			return 0;
		}

// std::cout << "Map type: " << static_cast<int>(tga->hdr.map_t) << std::endl;
// std::cout << "Img type: " << static_cast<int>(tga->hdr.img_t) << std::endl;
// std::cout << "Map depth: " << static_cast<int>(tga->hdr.map_entry) << std::endl;

		// Indexed color
		if(tga->hdr.map_t == 1 || tga->hdr.map_t == 9) {
			fo.i_depth = tga->hdr.depth;
			fo.clut_num = tga->hdr.map_len;
// std::cout << "Map depth: " << static_cast<int>(tga->hdr.map_entry) << std::endl;
			if(tga->hdr.map_entry == 15) {
				fo.r_depth = 5;
				fo.g_depth = 5;
				fo.b_depth = 5;
				fo.a_depth = 0;
			} else if(tga->hdr.map_entry == 16) {
				fo.r_depth = 5;
				fo.g_depth = 6;
				fo.b_depth = 5;
				fo.a_depth = 0;
			} else if(tga->hdr.map_entry == 24) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 0;
			} else if(tga->hdr.map_entry == 32) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 8;
			}
			fo.grayscale = false;
		}
		if(tga->hdr.img_t == 2 || tga->hdr.img_t == 10) {
// std::cout << static_cast<int>(tga->hdr.depth) << std::endl;
			if(tga->hdr.depth == 24) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 0;
			} else if(tga->hdr.depth == 32) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 8;
			}
			fo.i_depth = 0;
			fo.clut_num = 0;
			fo.grayscale = false;
		} else if(tga->hdr.img_t == 3 || tga->hdr.img_t == 11) {
			fo.r_depth = 0;
			fo.g_depth = 0;
			fo.b_depth = 0;
			fo.a_depth = tga->hdr.alpha;
			fo.i_depth = tga->hdr.depth;
			fo.clut_num = 0;
			fo.grayscale = true;
		}

		fo.width = tga->hdr.width;
		fo.height = tga->hdr.height;
		fo.mipmap_level = 0;
		fo.multi_level = 0;

		return tga;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::probe(utils::file_io& fin)
	{
		size_t pos = fin.tell();

		TGA* tga = create_(&fin);
		bool f = false;
		if(TGAReadHeader(tga) == TGA_OK) {
			f = true;
		}
		TGAFree(tga);

		fin.seek(pos, utils::file_io::seek::set);

		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画像ファイルの情報を取得する
		@param[in]	fin	file_io クラス
		@param[in]	fo	情報を受け取る構造体
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::info(utils::file_io& fin, img::img_info& fo)
	{
		size_t pos = fin.tell();

		TGA* tga = info_(&fin, fo);
		TGAFree(tga);

		fin.seek(pos, utils::file_io::seek::set);

		return tga != 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイル、ロード(utils::file_io)
		@param[in]	fin	ファイル I/O クラス
		@param[in]	ext	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::load(utils::file_io& fin, const std::string& ext)
	{
		img::img_info fo;
		TGA* tga = info_(&fin, fo);

		if(tga == 0) {
			return false;
		}

		bool alpha = false;
		if(fo.a_depth) alpha = true;

		bool f = true;

		if(fo.clut_num) {
			tbyte* cmap = 0;
			int n = TGAReadColorMap(tga, &cmap, TGA_RGB);
// std::cout << "Map: " << n << std::endl;
			if(n != 0) {
				img_.destroy();
				idx_.create(vtx::spos(fo.width, fo.height), alpha);

				for(int i = 0; i < fo.clut_num; ++i) {
					img::rgba8 c;
					if(fo.r_depth == 8 && fo.a_depth == 0) {
						c.r = cmap[i * 3 + 0];
						c.g = cmap[i * 3 + 1];
						c.b = cmap[i * 3 + 2];
						c.a = 255;
					} else if(fo.r_depth == 8 && fo.a_depth == 8) {
						c.r = cmap[i * 4 + 0];
						c.g = cmap[i * 4 + 1];
						c.b = cmap[i * 4 + 2];
						c.a = cmap[i * 4 + 3];
					}
					idx_.put_clut(i, c);
				}
				free(cmap);

				tbyte* tmp = new tbyte[fo.width * fo.height];
				if(TGAReadScanlines(tga, tmp, 0, fo.height, 0) != 0) {
					const uint8_t* p = static_cast<const uint8_t*>(tmp);
					for(int y = 0; y < fo.height; ++y) {
						int ofs;
						if(tga->hdr.vert) ofs = y;
						else ofs = fo.height - y - 1;
						for(int x = 0; x < fo.width; ++x) {
							idx8 i(p[x]);
							idx_.put_pixel(x, ofs, i);
						}
						p += fo.width;
					}
				} else {
					f = false;
				}
				delete[] tmp;
			} else {
				f = false;
			}
		} else {
			idx_.destroy();
			img_.create(vtx::spos(fo.width, fo.height), alpha);
			tbyte* tmp = new tbyte[fo.width * fo.height * 4];
			if(TGAReadScanlines(tga, tmp, 0, fo.height, TGA_BGR) != 0) {
				const uint8_t* p = static_cast<const uint8_t*>(tmp);
				uint32_t n = 3;
				if(alpha) ++n;
				n *= fo.width;
				for(int y = 0; y < fo.height; ++y) {
					int ofs;
					if(tga->hdr.vert) ofs = y;
					else ofs = fo.height - y - 1;
					for(int x = 0; x < fo.width; ++x) {
						rgba8 c;
						if(alpha) {
							c.b = p[x * 4 + 0];
							c.g = p[x * 4 + 1];
							c.r = p[x * 4 + 2];
							c.a = p[x * 4 + 3];
						} else {
							c.b = p[x * 3 + 0];
							c.g = p[x * 3 + 1];
							c.r = p[x * 3 + 2];
						} 
						img_.put_pixel(x, ofs, c);
					}
					p += n;
				}
			} else {
				f = false;
			}
			delete[] tmp;
		}

		TGAFree(tga);

		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイルをセーブする
		@param[in]	fout	ファイル I/O クラス
		@param[in]	ext		フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool tga_io::save(utils::file_io& fout, const std::string& ext)
	{
		if(imf_ == 0) {
			return false;
		}





		return true;
	}
}
