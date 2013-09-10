//=====================================================================//
/*!	@file
	@brief	PNG 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#if defined(IPHONE) || defined(IPAD) || defined(IPHONE_IPAD)
#include "png.h"
#else
#include <png.h>
#endif
#include "png_io.hpp"
#include "img_idx8.hpp"
#include "img_rgba8.hpp"

using namespace std;

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PNG read 関数
		@param[in]	png_ptr	PNG 管理構造体のポインター
		@param[in]	buf		データを格納するポインター
		@param[in]	size	読み込みバイト数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void png_io_read_func(png_structp png_ptr, png_bytep buf, png_size_t size)
	{
		utils::file_io* fin = (utils::file_io*)png_get_io_ptr(png_ptr);

		size_t s = fin->read(buf, 1, size);
		if(s != size) {
			char text[128];
			sprintf(text, "png_io_read_func: error (read size: %d, request: %d)", (int)s, (int)size);
			::png_error(png_ptr, text);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PNG write 関数
		@param[in]	png_ptr	PNG 管理構造体のポインター
		@param[in]	buf		データを格納するポインター
		@param[in]	size	書き込みバイト数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void png_io_write_func(png_structp png_ptr, const png_bytep buf, png_size_t size)
	{
		utils::file_io* fout = (utils::file_io*)png_get_io_ptr(png_ptr);

		size_t s = fout->write(buf, 1, size);
		if(s != size) {
			char text[128];
			sprintf(text, "png_io_write_func: error (%d/%d)", (int)s, (int)size);
			::png_error(png_ptr, text);
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PNG flush 関数
		@param[in]	png_ptr	PNG 管理構造体のポインター
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	static void png_io_flush_func(png_structp png_ptr)
	{
		utils::file_io* fout = (utils::file_io*)png_get_io_ptr(png_ptr);
		fout->flush();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	PNG ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool png_io::probe(utils::file_io& fin)
	{
		static const int PNG_BYTES_TO_CHECK = 4;
		unsigned char	sig[PNG_BYTES_TO_CHECK];

		long pos = fin.tell();
		size_t l = fin.read(sig, 1, PNG_BYTES_TO_CHECK);
		fin.seek(pos, utils::file_io::seek::set);
		if(l == PNG_BYTES_TO_CHECK) {
			if(::png_check_sig(sig, PNG_BYTES_TO_CHECK)) {
				return true;
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画像ファイルの情報を取得する
		@param[in]	fin	file_io クラス
		@param[in]	fo	情報を受け取る構造体
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool png_io::info(utils::file_io& fin, img::img_info& fo)
	{
		if(probe(fin) == false) {
			return false;
		}

		//	png_ptr 構造体を確保・初期化します
		::png_structp png_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(png_ptr == NULL) {
			return false;
		}
		//  info_ptr 構造体を確保・初期化します
		::png_infop info_ptr = ::png_create_info_struct(png_ptr);
		if(info_ptr == NULL) {
			return false;
		}

		long pos = fin.tell();

		::png_set_read_fn(png_ptr, (png_voidp)&fin, png_io_read_func);
		::png_read_info(png_ptr, info_ptr);

//  IHDRチャンク情報を取得します
		::png_uint_32	width, height;
		int bit_depth, color_type, interlace_type;
		::png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
		fo.width  = width;
		fo.height = height;
		fo.mipmap_level = 0;
		fo.multi_level = 0;
		fo.grayscale = false;

		fo.r_depth = bit_depth;
		fo.g_depth = bit_depth;
		fo.b_depth = bit_depth;
		fo.a_depth = 0;
		if(color_type & PNG_COLOR_MASK_PALETTE) {
			fo.i_depth = bit_depth;
			if(info_ptr->num_trans) fo.a_depth = bit_depth;
		} else {
			fo.i_depth = 0;
			if(color_type & PNG_COLOR_MASK_ALPHA) {
				fo.a_depth = bit_depth;
			}
		}

		::png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		fin.seek(pos, utils::file_io::seek::set);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	PNG ファイルをロードする
		@param[in]	ext	フォーマット固有の設定文字列
		@param[in]	fin		file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool png_io::load(utils::file_io& fin, const std::string& ext)
	{
		if(probe(fin) == false) {
			return false;
		}

		//	png_ptr 構造体を確保・初期化します
		::png_structp png_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(png_ptr == NULL) {
			return false;
		}
		//  info_ptr 構造体を確保・初期化します
		::png_infop info_ptr = ::png_create_info_struct(png_ptr);
		if(info_ptr == NULL) {
			return false;
		}

		destroy();

		::png_set_read_fn(png_ptr, (png_voidp)&fin, png_io_read_func);
		::png_read_info(png_ptr, info_ptr);

//  IHDRチャンク情報を取得します
		::png_uint_32	width, height;
		int bit_depth, color_type, interlace_type;
		::png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
		int ch;
		bool alpha;
		bool gray = false;
		i_img* imf;
		if(color_type == PNG_COLOR_TYPE_GRAY) {
			ch = 4;
			gray = true;
			alpha = false;
			imf = dynamic_cast<i_img*>(&img_);
		} else if(color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
			ch = 4;
			gray = true;
			alpha = true;
			imf = dynamic_cast<i_img*>(&img_);
//			printf("PNG gray scale with alpha\n");
		} else if(color_type & PNG_COLOR_MASK_PALETTE) {
			ch = 1;
			if(info_ptr->num_trans) alpha = true; else alpha = false;
			imf = dynamic_cast<i_img*>(&idx_);
		} else if(color_type & PNG_COLOR_MASK_ALPHA) {
			ch = 4;
			alpha = true;
			imf = dynamic_cast<i_img*>(&img_);
//			printf("PNG RGBA\n");
		} else {
			ch = 3;
			alpha = false;
			imf = dynamic_cast<i_img*>(&img_);
///			printf("PNG RGB\n");
		}

		if(bit_depth > 8) {
			ch <<= 1;
		}

		imf->create(vtx::spos(width, height), alpha);

		if(color_type & PNG_COLOR_MASK_PALETTE) {
			for(int i = 0; i < info_ptr->num_palette; ++i) {
				unsigned char a;
				if(info_ptr->num_trans) {
					a = info_ptr->trans[i];
				} else {
					a = 255;
				}
				::png_color* clut = &info_ptr->palette[i];
				rgba8 c(clut->red, clut->green, clut->blue, a);
				imf->put_clut(i, c);
			}
		}

		png_byte* im = new png_byte[width * height * ch];

// ポインターの作成 [png_bytep]
		png_bytep*	pp = new ::png_bytep[height];
		for(int i = 0; i < (int)height; ++i) {
			pp[i] = &im[i * width * ch];
		}
		::png_read_image(png_ptr, pp);

		for(int y = 0; y < (int)height; ++y) {
			png_byte*	p = pp[y];
			if(color_type & PNG_COLOR_MASK_PALETTE) {
				for(int x = 0; x < (int)width; ++x) {
					idx8 idx(*p++);
					idx_.put_pixel(x, y, idx);
				}
			} else {
				for(int x = 0; x < (int)width; ++x) {
					img::rgba8 c;
					if(gray) {
						c.r = c.g = c.b = *p++;
						if(alpha) c.a = *p++;
						else c.a = 255;
					} else {
						c.r = *p++;
						c.g = *p++;
						c.b = *p++;
						if(ch == 4) c.a = *p++;
						else c.a = 255;
					}
					if(color_key_enable_) {
						const png_color_16& tc = info_ptr->trans_values;
						if(static_cast<unsigned short>(c.r) == tc.red
						   && static_cast<unsigned short>(c.g) == tc.green
						   && static_cast<unsigned short>(c.b) == tc.blue) {
							c.a = 0;
						}
					}
					img_.put_pixel(x, y, c);
				}
			}
		}

		delete[] pp;
		delete[] im;

		::png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	PNG ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	ext	フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool png_io::save(utils::file_io& fout, const std::string& ext)
	{
		if(imf_ == 0) return false;

		int w = imf_->get_size().x;
		int h = imf_->get_size().y;
		if(w <= 0 || h <= 0) {
			return false;
		}

		//	png_ptr 構造体を確保・初期化します
		::png_structp png_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(png_ptr == NULL) {
			return false;
		}
		//  info_ptr 構造体を確保・初期化します
		::png_infop info_ptr = ::png_create_info_struct(png_ptr);
		if(info_ptr == NULL) {
			return false;
		}
		::png_set_write_fn(png_ptr, (png_voidp)&fout, png_io_write_func, png_io_flush_func);

		int	type;
		int ch;
		if(imf_->get_type() == IMG::INDEXED8) {
			type = PNG_COLOR_TYPE_PALETTE;
			ch = 1;
		} else {
			if(imf_->test_alpha()) {
				type = PNG_COLOR_TYPE_RGB_ALPHA;
				ch = 4;
			} else {
				type = PNG_COLOR_TYPE_RGB;
				ch = 3;
			}
		}

		int depth = 8;
		::png_set_IHDR(png_ptr, info_ptr, w, h, depth, type,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		unsigned char* clut_trans = 0;
		png_color* clut = 0;
		if(imf_->get_type() == IMG::INDEXED8) {
			int color = 256;
			if(imf_->test_alpha()) clut_trans = new unsigned char[color];
			clut = new png_color[color];
			for(int i = 0; i < color; ++i) {
				rgba8 c;
				imf_->get_clut(i, c);
				clut[i].red   = c.r;
				clut[i].green = c.g;
				clut[i].blue  = c.b;
				if(imf_->test_alpha()) clut_trans[i] = c.a;
			}
			::png_set_PLTE(png_ptr, info_ptr, clut, color);
			if(imf_->test_alpha()) ::png_set_tRNS(png_ptr, info_ptr, clut_trans, color, NULL);
		}

		::png_write_info(png_ptr, info_ptr);

		png_bytep*	pp = new ::png_bytep[h];
		for(int y = 0; y < h; ++y) {
			pp[y] = new png_byte[w * ch];
			png_byte*	p;
			p = pp[y];
			if(ch == 1) {
				for(int x = 0; x < w; ++x) {
					idx8	idx;
					imf_->get_pixel(x, y, idx);
					*p++ = idx.i;
				}
			} else if(imf_->get_type() == IMG::FULL8) {
				for(int x = 0; x < w; ++x) {
					rgba8	c;
					imf_->get_pixel(x, y, c);
					*p++ = c.r;
					*p++ = c.g;
					*p++ = c.b;
					if(ch == 4) *p++ = c.a;
				}
			}
		}
		::png_write_image(png_ptr, pp);

		delete clut_trans;
		delete clut;
		for(int i = 0; i < h; ++i) {
			delete[] pp[i];
		}
		delete[] pp;

		::png_write_end(png_ptr, info_ptr);
		::png_destroy_write_struct(&png_ptr, &info_ptr);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void png_io::destroy()
	{
		idx_.destroy();
		img_.destroy();
	}

}
/* ----- End Of File "png_io.cpp" ----- */
