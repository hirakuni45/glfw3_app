//=====================================================================//
/*!	@file
	@brief	JPEG 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdio>
#include <cstdlib>
extern "C" {
#include <jpeglib.h>
#include <jerror.h>
};
#include "utils/file_io.hpp"
#include "jpeg_io.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace img {

	static const size_t INPUT_BUF_SIZE = 4096;

	struct fio_src_mgr {
		struct jpeg_source_mgr	pub;		// public fields

		utils::file_io*	infile;				// source stream

		JOCTET*			buffer;
		bool			start_of_file;
		bool			err_empty;
		bool			wrn_eof;
	};

	typedef fio_src_mgr* fio_src_ptr;

	METHODDEF(void) init_source(j_decompress_ptr cinfo)
	{
		fio_src_ptr src = (fio_src_ptr) cinfo->src;

		/* We reset the empty-input-file flag for each image,
		 * but we don't clear the input buffer.
		 * This is correct behavior for reading a series of images from one source.
		 */
		src->start_of_file = true;

		src->err_empty = false;
		src->wrn_eof = false;
	}


	METHODDEF(boolean) fill_input_buffer(j_decompress_ptr cinfo)
	{
		fio_src_ptr src = (fio_src_ptr)cinfo->src;

		size_t nbytes = src->infile->read(src->buffer, 1, INPUT_BUF_SIZE);
		if(nbytes == 0) {
			if(src->start_of_file) {
				// Treat empty input file as fatal error
//				ERREXIT(cinfo, JERR_INPUT_EMPTY);
				src->err_empty = true;
			}
//			WARNMS(cinfo, JWRN_JPEG_EOF);
			src->wrn_eof = true;

			// Insert a fake EOI marker
			src->buffer[0] = (JOCTET) 0xFF;
			src->buffer[1] = (JOCTET) JPEG_EOI;
			nbytes = 2;
		}

		src->pub.next_input_byte = src->buffer;
		src->pub.bytes_in_buffer = nbytes;
		src->start_of_file = false;

		return TRUE;
	}


	METHODDEF(void) skip_input_data(j_decompress_ptr cinfo, long num_bytes)
	{
		fio_src_ptr src = (fio_src_ptr)cinfo->src;

		/* Just a dumb implementation for now.  Could use fseek() except
		 * it doesn't work on pipes.  Not clear that being smart is worth
		 * any trouble anyway --- large skips are infrequent.
		 */
		if(num_bytes > 0) {
			while(num_bytes > (long) src->pub.bytes_in_buffer) {
				num_bytes -= (long) src->pub.bytes_in_buffer;
				(void)fill_input_buffer(cinfo);
				/* note we assume that fill_input_buffer will never return FALSE,
				 * so suspension need not be handled.
				 */
			}
			src->pub.next_input_byte += (size_t) num_bytes;
			src->pub.bytes_in_buffer -= (size_t) num_bytes;
		}
	}


	METHODDEF(void) term_source(j_decompress_ptr cinfo)
	{
	}


	static GLOBAL(void) fio_jpeg_file_io_src(j_decompress_ptr cinfo, utils::file_io* infile)
	{
		if(cinfo->src == NULL) {	/* first time for this JPEG object? */
			cinfo->src = (struct jpeg_source_mgr *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				sizeof(fio_src_mgr));
			fio_src_ptr src = (fio_src_ptr) cinfo->src;
			src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				INPUT_BUF_SIZE * sizeof(JOCTET));
		}

		fio_src_ptr src = (fio_src_ptr) cinfo->src;
		src->pub.init_source		= init_source;
		src->pub.fill_input_buffer	= fill_input_buffer;
		src->pub.skip_input_data	= skip_input_data;
		src->pub.resync_to_restart	= jpeg_resync_to_restart;	// use default method
		src->pub.term_source		= term_source;
		src->infile					= infile;
		src->pub.bytes_in_buffer = 0;		// forces fill_input_buffer on first read
		src->pub.next_input_byte = NULL;	// until buffer loaded
	}


	static const size_t OUTPUT_BUF_SIZE = 4096;

	struct fio_dst_mgr {
		struct jpeg_destination_mgr	pub;	// public fields

		utils::file_io*	outfile;			// output stream

		JOCTET*			buffer;
		bool			error;
	};

	typedef fio_dst_mgr* fio_dst_ptr;

	METHODDEF(void) init_destination(j_compress_ptr cinfo)
	{
		fio_dst_ptr dst = (fio_dst_ptr) cinfo->dest;

		dst->pub.next_output_byte = dst->buffer;
		dst->pub.free_in_buffer = OUTPUT_BUF_SIZE;

		dst->error = false;
	}


	METHODDEF(boolean) empty_output_buffer(j_compress_ptr cinfo)
	{
		fio_dst_ptr dst = (fio_dst_ptr)cinfo->dest;

		size_t n = dst->outfile->write(dst->buffer, 1, OUTPUT_BUF_SIZE);
		if(n != OUTPUT_BUF_SIZE) {
			dst->error = true;
		}
		dst->pub.free_in_buffer = OUTPUT_BUF_SIZE;
		dst->pub.next_output_byte = dst->buffer;

		if(dst->error) return FALSE;

		return TRUE;
	}


	METHODDEF(void) term_destination(j_compress_ptr cinfo)
	{
		fio_dst_ptr dst = (fio_dst_ptr)cinfo->dest;

		size_t ws = OUTPUT_BUF_SIZE - dst->pub.free_in_buffer;
		size_t n = dst->outfile->write(dst->buffer, 1, ws);
		if(n != ws) {
			dst->error = true;
		}
	}


	static GLOBAL(void) fio_jpeg_file_io_dst(j_compress_ptr cinfo, utils::file_io* outfile)
	{
		if(cinfo->dest == NULL) {	/* first time for this JPEG object? */
			cinfo->dest = (struct jpeg_destination_mgr *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				sizeof(fio_dst_mgr));
			fio_dst_ptr dst = (fio_dst_ptr) cinfo->dest;
			dst->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				OUTPUT_BUF_SIZE * sizeof(JOCTET));
		}

		fio_dst_ptr dst = (fio_dst_ptr) cinfo->dest;
		dst->pub.init_destination    = init_destination;
		dst->pub.empty_output_buffer = empty_output_buffer;
		dst->pub.term_destination    = term_destination;
		dst->pub.next_output_byte    = NULL;
		dst->pub.free_in_buffer      = 0;
		dst->outfile                 = outfile;

		dst->error = false;
	}

	static volatile int error_code_;

	METHODDEF(void) error_exit_task(jpeg_common_struct *st)
	{
		error_code_ = 1;
	}


#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void jpeg_io::initialize()
	{
#ifdef __PPU__
		int ret = cellSysmoduleLoadModule(CELL_SYSMODULE_JPGDEC);
		if(ret != CELL_OK){
			std::cout << "Can't load jpeg module...." << std::endl;
			return;
		}
#endif
	}
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	JPEG ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool jpeg_io::probe(utils::file_io& fin)
	{
		unsigned char sig[2];
		long pos = fin.tell();
		size_t l = fin.read(sig, 1, 2);
		fin.seek(pos, utils::file_io::seek::set);
		if(l == 2) {
			if(sig[0] == 0xff && sig[1] == 0xd8) {
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
	bool jpeg_io::info(utils::file_io& fin, img::img_info& fo)
	{
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr errmgr;

		long pos = fin.tell();

		// エラーのハンドリング
		cinfo.err = jpeg_std_error(&errmgr);
		errmgr.error_exit = error_exit_task;

	    // 構造体の初期設定
		jpeg_create_decompress(&cinfo);

		// file_io クラス設定
		fio_jpeg_file_io_src(&cinfo, &fin);

		// ファイルの情報ヘッダの読込み
		error_code_ = 0;
		jpeg_read_header(&cinfo, TRUE);
		fin.seek(pos, utils::file_io::seek::set);

		if(error_code_) {
///			std::endl << "JPEG decode error: 'header'(" << static_cast<int>(error_code_) << ")" << std::endl; 
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		fo.width = cinfo.output_width;
		fo.height = cinfo.output_height;
		fo.mipmap_level = 0;
		fo.multi_level = 0;
		fo.i_depth = 0;

		if(cinfo.output_components == 4) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 8;
			fo.grayscale = false;
		} else if(cinfo.output_components == 3) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 0;
			fo.grayscale = false;
		} else if(cinfo.output_components == 2) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 8;
			fo.grayscale = true;
		} else if(cinfo.output_components == 1) {
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = 0;
			fo.grayscale = true;
		} else {
			fo.r_depth = 0;
			fo.g_depth = 0;
			fo.b_depth = 0;
			fo.a_depth = 0;
			fo.grayscale = false;
			return false;
		}

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	JPEG ファイル、ロード
		@param[in]	fin	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool jpeg_io::load(utils::file_io& fin, const std::string& opt)
	{
		// とりあえず、ヘッダーの検査
		if(probe(fin) == false) {
			return false;
		}

		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr errmgr;

		// エラーのハンドリング
		cinfo.err = jpeg_std_error(&errmgr);
		errmgr.error_exit = error_exit_task;

	    // 構造体の初期設定
		jpeg_create_decompress(&cinfo);

		// file_io クラス設定
		fio_jpeg_file_io_src(&cinfo, &fin);

		// ファイルの情報ヘッダの読込み
		error_code_ = 0;
		jpeg_read_header(&cinfo, TRUE);
		if(error_code_) {
			std::cout << "JPEG decode error: 'header'(" << error_code_ << ")" << std::endl; 
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		// 解凍の開始
		error_code_ = 0;
		jpeg_start_decompress(&cinfo);
		if(error_code_) {
			std::cout << "JPEG decode error: 'decompress'(" << error_code_ << ")" << std::endl;
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		img_.destroy();
		/// cinfo.in_color_space
		if(cinfo.output_components == 1) {
			img_.create(vtx::spos(cinfo.image_width, cinfo.image_height), true);
		} else if(cinfo.output_components == 3) {
			img_.create(vtx::spos(cinfo.image_width, cinfo.image_height), true);
		} else if(cinfo.output_components == 4) {
			img_.create(vtx::spos(cinfo.image_width, cinfo.image_height), true);
		} else {
			std::cout << "JPEG decode error: Can not support components: " << 
				static_cast<int>(cinfo.output_components) << std::endl;
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return false;
		}

		unsigned char* line = new unsigned char[cinfo.output_width * cinfo.output_components];
		unsigned char* lines[1];
		lines[0] = &line[0];
		vtx::spos pos;
		for(pos.y = 0; pos.y < cinfo.image_height; ++pos.y) {
			jpeg_read_scanlines(&cinfo, (JSAMPLE**)lines, 1);
			unsigned char* p = &line[0];
			if(cinfo.output_components == 4) {
				for(pos.x = 0; pos.x < cinfo.image_width; ++pos.x) {
					img::rgba8 c;
					c.r = *p++;
					c.g = *p++;
					c.b = *p++;
					c.a = *p++;
					img_.put_pixel(pos, c);
				}
			} else if(cinfo.output_components == 3) {
				for(pos.x = 0; pos.x < cinfo.image_width; ++pos.x) {
					img::rgba8 c;
					c.r = *p++;
					c.g = *p++;
					c.b = *p++;
					c.a = 255;
					img_.put_pixel(pos, c);
				}
			} else if(cinfo.output_components == 1) {
				for(pos.x = 0; pos.x < cinfo.image_width; ++pos.x) {
					img::rgba8 c;
					c.b = c.g = c.r = *p++;
					c.a = 255;
					img_.put_pixel(pos, c);
				}
			}
		}
		delete[] line;

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		fio_src_ptr src = (fio_src_ptr)cinfo.src;
		if(src->err_empty) {
			return false;
		} else {
			return true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	JPEG ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool jpeg_io::save(utils::file_io& fout, const std::string& opt)
	{
		if(imf_ == 0) return false;

		int q = 0;
		if(!opt.empty()) {
			try {
				q = boost::lexical_cast<int>(opt);
			} catch (boost::bad_lexical_cast&) {
				q = 0;
			}
		}

		struct jpeg_compress_struct cinfo;
		struct jpeg_error_mgr errmgr;

		// エラーのハンドリング
		cinfo.err = jpeg_std_error(&errmgr);
//		errmgr.error_exit = NULL;

		// 構造体の初期設定
		jpeg_create_compress(&cinfo);

		// file_io クラス設定
		fio_jpeg_file_io_dst(&cinfo, &fout);
		fio_dst_ptr dst = (fio_dst_ptr)cinfo.dest;

		int w = imf_->get_size().x;
		int h = imf_->get_size().y;
		cinfo.image_width  = w;
		cinfo.image_height = h;
		cinfo.input_components = 3;
		if(imf_->test_alpha()) {
			++cinfo.input_components;
			cinfo.in_color_space = JCS_EXT_RGBA;
		} else {
			cinfo.in_color_space = JCS_RGB;
		}

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, q, TRUE);

		jpeg_start_compress(&cinfo, TRUE);

		JSAMPROW row_pointer[1];
		unsigned char* tmp = new unsigned char[w * cinfo.input_components];
		row_pointer[0] = (JSAMPLE *)tmp;
		vtx::spos pos;
		pos.y = 0;
		while(cinfo.next_scanline < h) {
			unsigned char* p = tmp;
			for(pos.x = 0; pos.x < w; ++pos.x) {
				rgba8 c;
				imf_->get_pixel(pos, c);
				*p++ = c.r;
				*p++ = c.g;
				*p++ = c.b;
				if(imf_->test_alpha()) *p++ = c.a;
			}
			jpeg_write_scanlines(&cinfo, row_pointer, 1);
			if(dst->error) break;
			++pos.y;
		}
		delete[] tmp;

		// 圧縮後のサイズ
//		int ret = row_stride * height - cinfo.dest->free_in_buffer;

		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);

		if(dst->error) {
			return false;
		} else {
			return true;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void jpeg_io::destroy()
	{
		imf_ = 0;
		img_.destroy();
	}

}
