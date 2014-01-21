//=====================================================================//
/*!	@file
	@brief	OpenJPEG 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>
#define OPJ_STATIC
#include <openjpeg.h>
#include "utils/file_io.hpp"
#include "openjpeg_io.hpp"

namespace img {

	using namespace utils;

	static void g_error_callback(const char* msg, void* client_data)
	{
		FILE* stream = (FILE*)client_data;
		fprintf(stream, "[ERROR] %s", msg);
	}


	static void g_warning_callback(const char* msg, void* client_data)
	{
		FILE* stream = (FILE*)client_data;
		fprintf(stream, "[WARNING] %s", msg);
	}

#if 0
	static void g_info_callback(const char* msg, void* client_data)
	{
		(void)client_data;
		fprintf(stdout, "[INFO] %s", msg);
		fflush(stdout);
	}
#endif

	static int int_floorlog2(int a)
	{
		int l;
		for (l = 0; a > 1; l++) {
			a >>= 1;
		}
		return l;
	}


	static int int_ceildivpow2(int a, int b)
	{
		return (a + (1 << b) - 1) >> b;
	}


	static int int_ceildiv(int a, int b)
	{
		return (a + b - 1) / b;
	}


	static void opj_image_to_rgba8(opj_image_t* image, img_rgba8& img)
	{
		// image->x1 - image->x0;
		// image->y1 - image->y0;

		// w = int_ceildiv(image->x1 - image->x0, image->comps[0].dx);
		// wr = int_ceildiv(int_ceildivpow2(image->x1 - image->x0,image->factor), image->comps[0].dx);
		int w = image->comps[0].w;
		int wr = int_ceildivpow2(image->comps[0].w, image->comps[0].factor);
		// h = int_ceildiv(image->y1 - image->y0, image->comps[0].dy);
		// hr = int_ceildiv(int_ceildivpow2(image->y1 - image->y0,image->factor), image->comps[0].dy);
		int h = image->comps[0].h;
		int hr = int_ceildivpow2(image->comps[0].h, image->comps[0].factor);

		if(image->numcomps == 1) {
			img.create(vtx::spos(wr, hr), false);
			for(int y = 0; y < hr; ++y) {
				for(int x = 0; x < wr; ++x) {
					rgba8 c;
					c.g = c.b = c.r = image->comps[0].data[y * w + x];
					c.a = 255;
					img.put_pixel(x, y, c);
				}
			}
		} else if(image->numcomps == 2) {
			img.create(vtx::spos(wr, hr), true);
			for(int y = 0; y < hr; ++y) {
				for(int x = 0; x < wr; ++x) {
					rgba8 c;
					c.g = c.b = c.r = image->comps[0].data[y * w + x];
					c.a = image->comps[1].data[y * w + x];
					img.put_pixel(x, y, c);
				}
			}
		} else if(image->numcomps == 3) {
			img.create(vtx::spos(wr, hr), false);
			for(int y = 0; y < hr; ++y) {
				for(int x = 0; x < wr; ++x) {
					rgba8 c;
					c.r = image->comps[0].data[y * w + x];
					c.g = image->comps[1].data[y * w + x];
					c.b = image->comps[2].data[y * w + x];
					c.a = 255;
					img.put_pixel(x, y, c);
				}
			}
		} else if(image->numcomps == 4) {
			img.create(vtx::spos(wr, hr), true);
			for(int y = 0; y < hr; ++y) {
				for(int x = 0; x < wr; ++x) {
					rgba8 c;
					c.r = image->comps[0].data[y * w + x];
					c.g = image->comps[1].data[y * w + x];
					c.b = image->comps[2].data[y * w + x];
					c.a = image->comps[3].data[y * w + x];
					img.put_pixel(x, y, c);
				}
			}
		}
	}


	static bool decode_header(utils::file_io& fin, CODEC_FORMAT form, img::img_info& fo)
	{
		opj_event_mgr_t event_mgr;
		memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
		event_mgr.error_handler = g_error_callback;
		event_mgr.warning_handler = g_warning_callback;
//		event_mgr.info_handler = g_info_callback;
		opj_dparameters_t parameters;
		opj_set_default_decoder_parameters(&parameters);

		// ヘッダーのみのデコード
		parameters.cp_limit_decoding = LIMIT_TO_MAIN_HEADER;

		opj_dinfo_t* dinfo = opj_create_decompress(form);
		opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);
		opj_setup_decoder(dinfo, &parameters);

		long pos = fin.tell();
		array_uc ary;

		// ※ヘッダーとして読み込むサイズは適当なので、問題が起こる可能性あり・・
		// +256 でサンプルは読めたが、念のため +1024 バイト分
		bool f = read_array(fin, ary, 64 + 1024);
		fin.seek(pos, utils::file_io::seek::set);

		if(f) {
			opj_cio_t* cio = opj_cio_open((opj_common_ptr)dinfo, &ary[0], ary.size());
			opj_image_t* image = opj_decode(dinfo, cio);
			if(!image) {
				opj_cio_close(cio);
				opj_destroy_decompress(dinfo);
				return false;
			}

			fo.width  = image->x1 - image->x0;
			fo.height = image->y1 - image->y0;
			fo.mipmap_level = 0;
			fo.multi_level = 0;
			fo.i_depth = 0;
			if(image->color_space == CLRSPC_SRGB) fo.grayscale = false;
			else if(image->color_space == CLRSPC_GRAY) fo.grayscale = true;

///			printf("Header components: %d\n", image->numcomps);

			if(image->numcomps == 1) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 0;
			} else if(image->numcomps == 2) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 8;
			} else if(image->numcomps == 3) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 0;
			} else if(image->numcomps == 4) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 8;
			}
			opj_image_destroy(image);
			opj_cio_close(cio);
			opj_destroy_decompress(dinfo);
			return true;
		} else {
			return false;
		}
	}


	static bool decode(utils::file_io& fin, CODEC_FORMAT form, img_rgba8& img)
	{
		opj_event_mgr_t event_mgr;
		memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
		event_mgr.error_handler = g_error_callback;
		event_mgr.warning_handler = g_warning_callback;
//		event_mgr.info_handler = g_info_callback;

		opj_dparameters_t parameters;
		opj_set_default_decoder_parameters(&parameters);

		opj_dinfo_t* dinfo = opj_create_decompress(form);
		opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);
		opj_setup_decoder(dinfo, &parameters);

		array_uc ary;
		if(read_array(fin, ary)) {
			opj_cio_t* cio = opj_cio_open((opj_common_ptr)dinfo, &ary[0], ary.size());
			if(!cio) {
				opj_destroy_decompress(dinfo);
				return false;				
			}
			opj_image_t* image = opj_decode(dinfo, cio);
			if(!image) {
				opj_cio_close(cio);
				opj_destroy_decompress(dinfo);
				return false;
			}
			opj_image_to_rgba8(image, img);
			opj_image_destroy(image);
			opj_cio_close(cio);
			opj_destroy_decompress(dinfo);
			return true;
		} else {
			opj_destroy_decompress(dinfo);
			return false;
		}
	}


	static bool encode(utils::file_io& fout, CODEC_FORMAT form, const img_rgba8& img)
	{


		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	JPEG ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool openjpeg_io::probe(utils::file_io& fin)
	{
		unsigned char sig[4];
		long pos = fin.tell();
		size_t l = fin.read(sig, 1, 4);
		fin.seek(pos, utils::file_io::seek::set);
		if(l == 4) {
//			printf("OpenJPEG probe: J2K\n");
//			fflush(stdout);
			// J2K: ff, 4f, ff, 51
			if(sig[0] == 0xff && sig[1] == 0x4f && sig[2] == 0xff && sig[3] == 0x51) {
				return true;
			}
			// JP2: 00 00 00 0c
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
	bool openjpeg_io::info(utils::file_io& fin, img::img_info& fo)
	{
		unsigned char sig[4];
		long pos = fin.tell();
		size_t l = fin.read(sig, 1, 4);
		fin.seek(pos, utils::file_io::seek::set);

		CODEC_FORMAT form = CODEC_UNKNOWN;
		if(l == 4) {
			if(sig[0] == 0xff && sig[1] == 0x4f && sig[2] == 0xff && sig[3] == 0x51) {
				form = CODEC_J2K;
			} else if(sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0x00 && sig[3] == 0x0c) {
				form = CODEC_JP2;
			}
		} else {
			return false;
		}

		if(form != CODEC_UNKNOWN) {
			if(decode_header(fin, form, fo)) {
				return true;
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	OpenJPEG ファイル、ロード
		@param[in]	fin	file_io クラス
		@param[in]	ext	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool openjpeg_io::load(utils::file_io& fin, const std::string& ext)
	{
#if 0
		img_info fo;
		if(info(fin, fo)) {
			printf("Info OK!: %d, %d\n", fo.width, fo.height);
			fflush(stdout);
		}
#endif
		unsigned char sig[4];
		long pos = fin.tell();
		size_t l = fin.read(sig, 1, 4);
		fin.seek(pos, utils::file_io::seek::set);

		CODEC_FORMAT form = CODEC_UNKNOWN;
		if(l == 4) {
			if(sig[0] == 0xff && sig[1] == 0x4f && sig[2] == 0xff && sig[3] == 0x51) {
				form = CODEC_J2K;
			} else if(sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0x00 && sig[3] == 0x0c) {
				form = CODEC_JP2;
			}
		} else {
			return false;
		}

		if(decode(fin, form, img_)) {
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	ext	フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool openjpeg_io::save(utils::file_io& fout, const std::string& ext)
	{
		if(imf_ == 0) return false;

#if 0
		int quality = quality_;
		if(ext) {
			int n;
			if(sscanf(ext, "%d", &n) == 1) {
				quality = n;
			}
		}
#endif

		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void openjpeg_io::destroy()
	{
		imf_ = 0;
		img_.destroy();
	}

}
