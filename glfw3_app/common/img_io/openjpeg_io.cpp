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
#include "img_io/openjpeg_io.hpp"
#include "utils/string_utils.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace img {

	using namespace utils;

	static void error_callback_(const char* msg, void* context)
	{
		std::cerr << "[ERROR] " << msg;
	}


	static void warning_callback_(const char* msg, void* context)
	{
		std::cerr << "[WARNING] " << msg;
	}


	static void info_callback_(const char* msg, void* context)
	{
//		std::cerr << "[INFO] " << msg;
	}


	struct stream_block {
		unsigned char *data;
		int size;
		int pos;
	};


	OPJ_SIZE_T strm_read_(void* p_buffer, OPJ_SIZE_T p_nb_bytes, void* p_user_data)
	{
		stream_block* sb = static_cast<stream_block*>(p_user_data);
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

	OPJ_OFF_T strm_skip_(OPJ_OFF_T skip, void* p_user_data)
	{
		stream_block* sb = static_cast<stream_block*>(p_user_data);

		if (skip > sb->size - sb->pos)
			skip = sb->size - sb->pos;
		sb->pos += skip;
		return sb->pos;
	}


	OPJ_BOOL strm_seek_(OPJ_OFF_T seek_pos, void* p_user_data)
	{
		stream_block* sb = static_cast<stream_block*>(p_user_data);

		if (seek_pos > sb->size)
			return OPJ_FALSE;
		sb->pos = seek_pos;
		return OPJ_TRUE;
	}

	OPJ_OFF_T wstrm_skip_(OPJ_OFF_T skip, void* p_user_data)
	{
		utils::file_io* fio = static_cast<utils::file_io*>(p_user_data);
		bool f = fio->seek(skip, utils::file_io::seek::cur);
		if(f) return skip;
		else return 0;
	}

	OPJ_BOOL wstrm_seek_(OPJ_OFF_T seek_pos, void* p_user_data)
	{
		utils::file_io* fio = static_cast<utils::file_io*>(p_user_data);
		bool f = fio->seek(seek_pos, utils::file_io::seek::set);
		if(f) return OPJ_TRUE;
		else return OPJ_FALSE;
	}

	OPJ_SIZE_T wstrm_write_(void* p_buffer, OPJ_SIZE_T p_nb_bytes, void* p_user_data)
	{
		utils::file_io* fio = static_cast<utils::file_io*>(p_user_data);
		size_t n = fio->write(p_buffer, p_nb_bytes);
		return n;
	}

#if 0
	static int int_floorlog2_(int a)
	{
		int l;
		for (l = 0; a > 1; l++) {
			a >>= 1;
		}
		return l;
	}
#endif

	static int int_ceildivpow2_(int a, int b)
	{
		return (a + (1 << b) - 1) >> b;
	}

#if 0
	static int int_ceildiv_(int a, int b)
	{
		return (a + b - 1) / b;
	}
#endif

	static void opj_image_to_rgba8_(opj_image_t* image, shared_img img)
	{
		// image->x1 - image->x0;
		// image->y1 - image->y0;

		// w = int_ceildiv(image->x1 - image->x0, image->comps[0].dx);
		// wr = int_ceildiv(int_ceildivpow2(image->x1 - image->x0,image->factor), image->comps[0].dx);
		int w = image->comps[0].w;
		int wr = int_ceildivpow2_(image->comps[0].w, image->comps[0].factor);
		// h = int_ceildiv(image->y1 - image->y0, image->comps[0].dy);
		// hr = int_ceildiv(int_ceildivpow2(image->y1 - image->y0,image->factor), image->comps[0].dy);
//		int h = image->comps[0].h;
		int hr = int_ceildivpow2_(image->comps[0].h, image->comps[0].factor);

		if(image->numcomps == 1) {
			img->create(vtx::spos(wr, hr), false);
			vtx::spos pos;
			for(pos.y = 0; pos.y < hr; ++pos.y) {
				for(pos.x = 0; pos.x < wr; ++pos.x) {
					rgba8 c;
					c.g = c.b = c.r = image->comps[0].data[pos.y * w + pos.x];
					c.a = 255;
					img->put_pixel(pos, c);
				}
			}
		} else if(image->numcomps == 2) {
			img->create(vtx::spos(wr, hr), true);
			vtx::spos pos;
			for(pos.y = 0; pos.y < hr; ++pos.y) {
				for(pos.x = 0; pos.x < wr; ++pos.x) {
					rgba8 c;
					c.g = c.b = c.r = image->comps[0].data[pos.y * w + pos.x];
					c.a = image->comps[1].data[pos.y * w + pos.x];
					img->put_pixel(pos, c);
				}
			}
		} else if(image->numcomps == 3) {
			img->create(vtx::spos(wr, hr), false);
			vtx::spos pos;
			for(pos.y = 0; pos.y < hr; ++pos.y) {
				for(pos.x = 0; pos.x < wr; ++pos.x) {
					rgba8 c;
					c.r = image->comps[0].data[pos.y * w + pos.x];
					c.g = image->comps[1].data[pos.y * w + pos.x];
					c.b = image->comps[2].data[pos.y * w + pos.x];
					c.a = 255;
					img->put_pixel(pos, c);
				}
			}
		} else if(image->numcomps == 4) {
			img->create(vtx::spos(wr, hr), true);
			vtx::spos pos;
			for(pos.y = 0; pos.y < hr; ++pos.y) {
				for(pos.x = 0; pos.x < wr; ++pos.x) {
					rgba8 c;
					c.r = image->comps[0].data[pos.y * w + pos.x];
					c.g = image->comps[1].data[pos.y * w + pos.x];
					c.b = image->comps[2].data[pos.y * w + pos.x];
					c.a = image->comps[3].data[pos.y * w + pos.x];
					img->put_pixel(pos, c);
				}
			}
		}
	}

	struct sub_t {
		array_uc		array;
		opj_codec_t*	codec;
		opj_stream_t*	stream;
		opj_image_t*	image;
		sub_t() : codec(0), stream(0), image(0) { }
	};

	bool decode_sub_(utils::file_io& fin, CODEC_FORMAT form, sub_t& t, bool all = true)
	{
		size_t sz;
		if(all) {
			if(!read_array(fin, t.array)) {
				return false;
			}
			sz = t.array.size();
		} else { // header のみ
			sz = 1024;
			if(!read_array(fin, t.array, sz)) {
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
		sb.data = &t.array[0];
		sb.pos = 0;
		sb.size = sz;
		opj_stream_set_read_function(t.stream, strm_read_);
		opj_stream_set_skip_function(t.stream, strm_skip_);
		opj_stream_set_seek_function(t.stream, strm_seek_);
		opj_stream_set_user_data(t.stream, &sb, nullptr);
		// Set the length to avoid an assert
		opj_stream_set_user_data_length(t.stream, sz);

		if(!opj_read_header(t.stream, t.codec, &t.image)) {
			opj_stream_destroy(t.stream);
			opj_destroy_codec(t.codec);
			return false;
		}

//		int w  = t.image->x1 - t.image->x0;
//		int h = t.image->y1 - t.image->y0;
//		std::cout << w << ", " << h << std::endl;

		return true;
	}


	static bool decode_header_(utils::file_io& fin, CODEC_FORMAT form, img::img_info& fo)
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

///		std::cout << "Header components: " << static_cast<int>(image->numcomps) << std::endl;

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


	static bool decode_(utils::file_io& fin, CODEC_FORMAT form, shared_img img)
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

		opj_image_to_rgba8_(t.image, img);

		opj_image_destroy(t.image);
		opj_stream_destroy(t.stream);
		opj_destroy_codec(t.codec);

		return true;
	}


	static CODEC_FORMAT probe_(utils::file_io& fin)
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
		}
		return form;
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
		return probe_(fin) != OPJ_CODEC_UNKNOWN;
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
		CODEC_FORMAT form = probe_(fin);
		if(form != OPJ_CODEC_UNKNOWN) {
			return decode_header_(fin, form, fo);
		} else {
			return false; 
		}
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
		CODEC_FORMAT form = probe_(fin);
		if(form != OPJ_CODEC_UNKNOWN) {
			img_ = shared_img(new img_rgba8);
			return decode_(fin, form, img_);
		} else {
			img_ = nullptr;
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool openjpeg_io::save(utils::file_io& fout, const std::string& opt)
	{
		if(!img_) return false;
		const vtx::spos& size = img_->get_size();
		if(size.x == 0 || size.y == 0) return false;

		CODEC_FORMAT form;
		const std::string& fp = fout.get_path();
		if(utils::no_capital_strcmp(utils::get_file_ext(fp), "j2k") == 0) {
			form = OPJ_CODEC_J2K;
		} else if(utils::no_capital_strcmp(utils::get_file_ext(fp), "jp2") == 0) {
			form = OPJ_CODEC_JP2;
		} else {
			return false;
		}

		int nc = 3;
		if(img_->test_alpha()) ++nc; 

		// 品質パラメーター
		float q = 0.75f;
		if(!opt.empty()) {
			try {
				q = boost::lexical_cast<float>(opt);
			} catch (boost::bad_lexical_cast&) {
				q = 0.75f;
			}
		}

		opj_cparameters_t parameters;
		opj_set_default_encoder_parameters(&parameters);

		// 各レイヤーの圧縮率
		if(parameters.tcp_numlayers == 0) {
			parameters.tcp_rates[0] = q;	/* MOD antonin : losslessbug */
			parameters.tcp_numlayers++;
			parameters.cp_disto_alloc = 1;
		}

//		parameters.tcp_mct = t.image->numcomps == 3 ? 1 : 0;

		if(parameters.cp_cinema) {

		}

//		char cmt[] = { "openjpeg_io class" };
//		parameters.cp_comment = cmt;

		int dx = parameters.subsampling_dx;
		int dy = parameters.subsampling_dy;
		opj_image_cmptparm_t cmpt[4];
		memset(&cmpt[0], 0, nc * sizeof(opj_image_cmptparm_t));
		for(int i = 0; i < nc; ++i) {
			cmpt[i].prec = 8;
			cmpt[i].bpp  = 8;
			cmpt[i].sgnd = 0;
			cmpt[i].dx = dx;
			cmpt[i].dy = dy;
			cmpt[i].w = size.x;
			cmpt[i].h = size.y;
		}

		sub_t t;
		t.image = opj_image_create(nc, &cmpt[0], OPJ_CLRSPC_SRGB);

		// set image offset and reference grid
		t.image->x0 = parameters.image_offset_x0;
		t.image->y0 = parameters.image_offset_y0;
		t.image->x1 = !t.image->x0 ? (size.x - 1) * dx + 1 : t.image->x0 + (size.x - 1) * dx + 1;
		t.image->y1 = !t.image->y0 ? (size.y - 1) * dy + 1 : t.image->y0 + (size.y - 1) * dy + 1;

// std::cout << (int)t.image->x1 << std::endl;
// std::cout << (int)t.image->y1 << std::endl;

		int idx = 0;
		vtx::spos pos;
		for(pos.y = 0; pos.y < size.y; ++pos.y) {
			for(pos.x = 0; pos.x < size.x; ++pos.x) {
				rgba8 c;
				img_->get_pixel(pos, c);
				t.image->comps[0].data[idx] = c.r;
				t.image->comps[1].data[idx] = c.g;
				t.image->comps[2].data[idx] = c.b;
				if(img_->test_alpha()) t.image->comps[3].data[idx] = c.a;
				++idx;
			}
		}

		t.codec = opj_create_compress(form);
		opj_set_info_handler(   t.codec, info_callback_,    0);
		opj_set_warning_handler(t.codec, warning_callback_, 0);
		opj_set_error_handler(  t.codec, error_callback_,   0);

		opj_setup_encoder(t.codec, &parameters, t.image);

		t.stream = opj_stream_default_create(OPJ_FALSE);
		opj_stream_set_write_function(t.stream, wstrm_write_);
		opj_stream_set_skip_function(t.stream, wstrm_skip_);
		opj_stream_set_seek_function(t.stream, wstrm_seek_);
		opj_stream_set_user_data(t.stream, &fout, nullptr);
		opj_stream_set_user_data_length(t.stream, 0);

		bool f = false;
		if(opj_start_compress(t.codec, t.image, t.stream)) {
			if(opj_encode(t.codec, t.stream)) {
				if(opj_end_compress(t.codec, t.stream)) {
					f = true;
				}
			}
		}

		opj_stream_destroy(t.stream);
		opj_destroy_codec(t.codec);
		opj_image_destroy(t.image);

		return f;
	}

}
