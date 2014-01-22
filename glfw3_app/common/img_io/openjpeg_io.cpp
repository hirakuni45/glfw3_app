//=====================================================================//
/*!	@file
	@brief	OpenJPEG 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdio>
#include <cstdlib>
#define OPJ_STATIC
#include <openjpeg.h>
#include "utils/file_io.hpp"
#include "openjpeg_io.hpp"

namespace img {

	using namespace utils;

	static void error_callback_(const char* msg, void* client_data)
	{
//		FILE* stream = (FILE*)client_data;
		fprintf(stderr, "[ERROR] %s", msg);
	}


	static void warning_callback_(const char* msg, void* client_data)
	{
//		FILE* stream = (FILE*)client_data;
		fprintf(stderr, "[WARNING] %s", msg);
	}


	static void info_callback_(const char* msg, void* client_data)
	{
		(void)client_data;
//		fprintf(stdout, "[INFO] %s", msg);
//		fflush(stdout);
	}


	struct stream_block {
		unsigned char *data;
		int size;
		int pos;
	};


	OPJ_SIZE_T stream_read_(void * p_buffer, OPJ_SIZE_T p_nb_bytes, void * p_user_data)
	{
		stream_block *sb = (stream_block *)p_user_data;
		int len;

		len = sb->size - sb->pos;
		if (len < 0)
			len = 0;
		if (len == 0)
			return (OPJ_SIZE_T)-1;  /* End of file! */
		if ((OPJ_SIZE_T)len > p_nb_bytes)
			len = p_nb_bytes;
		memcpy(p_buffer, sb->data + sb->pos, len);
		sb->pos += len;
		return len;
	}

	OPJ_OFF_T stream_skip_(OPJ_OFF_T skip, void * p_user_data)
	{
		stream_block *sb = (stream_block *)p_user_data;

		if (skip > sb->size - sb->pos)
			skip = sb->size - sb->pos;
		sb->pos += skip;
		return sb->pos;
	}


	OPJ_BOOL stream_seek_(OPJ_OFF_T seek_pos, void * p_user_data)
	{
		stream_block *sb = (stream_block *)p_user_data;

		if (seek_pos > sb->size)
			return OPJ_FALSE;
		sb->pos = seek_pos;
		return OPJ_TRUE;
	}


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

	struct sub_t {
		opj_codec_t*	codec;
		opj_stream_t*	stream;
		opj_image_t*	image;
	};

	bool decode_sub_(utils::file_io& fin, CODEC_FORMAT form, sub_t& t, bool all = true)
	{
		array_uc array;
		if(all) {
			if(!read_array(fin, array)) {
				return false;
			}
		} else { // header のみ
			if(!read_array(fin, array, 1024)) {
				return false;
			}
		}

		opj_dparameters_t parameters;
		opj_set_default_decoder_parameters(&parameters);

		t.codec = opj_create_decompress(form);
		opj_set_info_handler(t.codec, info_callback_, 0);
		opj_set_warning_handler(t.codec, warning_callback_, 0);
		opj_set_error_handler(t.codec, error_callback_, 0);

		if(!opj_setup_decoder(t.codec, &parameters)) {
			return false;
		}

		stream_block sb;
		t.stream = opj_stream_default_create(OPJ_TRUE);
		sb.data = &array[0];
		sb.pos = 0;
		sb.size = array.size();
		opj_stream_set_read_function(t.stream, stream_read_);
		opj_stream_set_skip_function(t.stream, stream_skip_);
		opj_stream_set_seek_function(t.stream, stream_seek_);
		opj_stream_set_user_data(t.stream, &sb);
		// Set the length to avoid an assert
		opj_stream_set_user_data_length(t.stream, array.size());

		opj_image_t *jpx;
		if(!opj_read_header(t.stream, t.codec, &t.image)) {
			opj_stream_destroy(t.stream);
			opj_destroy_codec(t.codec);
			return false;
		}
	}


	static bool decode_header(utils::file_io& fin, CODEC_FORMAT form, img::img_info& fo)
	{
		sub_t t;
		if(!decode_sub_(fin, form, t, false)) {
			return false;
		}

		fo.width  = t.image->x1 - t.image->x0;
		fo.height = t.image->y1 - t.image->y0;
		fo.mipmap_level = 0;
		fo.multi_level = 0;
		fo.i_depth = 0;
		if(t.image->color_space == OPJ_CLRSPC_SRGB) fo.grayscale = false;
		else if(t.image->color_space == OPJ_CLRSPC_GRAY) fo.grayscale = true;

///		printf("Header components: %d\n", image->numcomps);

		if(t.image->numcomps == 1) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 0;
		} else if(t.image->numcomps == 2) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 8;
		} else if(t.image->numcomps == 3) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 0;
		} else if(t.image->numcomps == 4) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 8;
		} else {
			return false;
		}
		opj_image_destroy(t.image);
		opj_stream_destroy(t.stream);
		opj_destroy_codec(t.codec);

		return true;
	}


	static bool decode(utils::file_io& fin, CODEC_FORMAT form, img_rgba8& img)
	{
		sub_t t;
		if(!decode_sub_(fin, form, t)) {
			return false;
		}

		if(!opj_decode(t.codec, t.stream, t.image)) {
			opj_image_destroy(t.image);
			opj_stream_destroy(t.stream);
			opj_destroy_codec(t.codec);
			return false;
		}

		opj_image_to_rgba8(t.image, img);
		opj_image_destroy(t.image);
		opj_stream_destroy(t.stream);
		opj_destroy_codec(t.codec);

		return true;
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
			if(sig[0] == 0xff && sig[1] == 0x4f && sig[2] == 0xff && sig[3] == 0x51) {
				return true;
			} else if(sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0x00 && sig[3] == 0x0c) {
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
	bool openjpeg_io::info(utils::file_io& fin, img::img_info& fo)
	{
		unsigned char sig[4];
		long pos = fin.tell();
		size_t l = fin.read(sig, 1, 4);
		fin.seek(pos, utils::file_io::seek::set);

		CODEC_FORMAT form = OPJ_CODEC_UNKNOWN;
		if(l == 4) {
			if(sig[0] == 0xff && sig[1] == 0x4f && sig[2] == 0xff && sig[3] == 0x51) {
				form = OPJ_CODEC_J2K;
			} else if(sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0x00 && sig[3] == 0x0c) {
				form = OPJ_CODEC_JP2;
			}
		} else {
			return false;
		}

		if(form != OPJ_CODEC_UNKNOWN) {
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
		unsigned char sig[4];
		long pos = fin.tell();
		size_t l = fin.read(sig, 1, 4);
		fin.seek(pos, utils::file_io::seek::set);

		CODEC_FORMAT form = OPJ_CODEC_UNKNOWN;
		if(l == 4) {
			if(sig[0] == 0xff && sig[1] == 0x4f && sig[2] == 0xff && sig[3] == 0x51) {
				form = OPJ_CODEC_J2K;
			} else if(sig[0] == 0x00 && sig[1] == 0x00 && sig[2] == 0x00 && sig[3] == 0x0c) {
				form = OPJ_CODEC_JP2;
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
