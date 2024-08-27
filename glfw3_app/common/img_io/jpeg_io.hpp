#pragma once
//=====================================================================//
/*!	@file
	@brief	JPEG/JFIF 画像を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2024 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "i_img_io.hpp"
#include "img_rgba8.hpp"
#include "utils/file_io.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
/// #include <turbojpeg.h>
};

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	JPEG 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class jpeg_io : public i_img_io {

		shared_img	img_;

		uint32_t	prgl_ref_;
		uint32_t	prgl_pos_;

		static inline int	error_code_ = 0;

		static constexpr size_t INPUT_BUF_SIZE = 4096;

		struct fio_src_mgr {
			struct jpeg_source_mgr	pub;		// public fields

			utils::file_io*	infile;				// source stream

			JOCTET*			buffer;
			bool			start_of_file;
			bool			err_empty;
			bool			wrn_eof;
		};

		typedef fio_src_mgr* fio_src_ptr;

//		static METHODDEF(void) init_source(j_decompress_ptr cinfo)
		static void init_source(j_decompress_ptr cinfo)
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


		static boolean fill_input_buffer(j_decompress_ptr cinfo)
		{
			fio_src_ptr src = (fio_src_ptr)cinfo->src;

			size_t nbytes = src->infile->read(src->buffer, 1, INPUT_BUF_SIZE);
			if(nbytes == 0) {
				if(src->start_of_file) {
					// Treat empty input file as fatal error
//					ERREXIT(cinfo, JERR_INPUT_EMPTY);
					src->err_empty = true;
				}
//				WARNMS(cinfo, JWRN_JPEG_EOF);
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


		static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
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


		static void term_source(j_decompress_ptr cinfo)
		{
		}


		static void fio_jpeg_file_io_src(j_decompress_ptr cinfo, utils::file_io* infile)
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


		static constexpr size_t OUTPUT_BUF_SIZE = 4096;

		struct fio_dst_mgr {
			struct jpeg_destination_mgr	pub;	// public fields

			utils::file_io*	outfile;			// output stream

			JOCTET*			buffer;
			bool			error;
		};

		typedef fio_dst_mgr* fio_dst_ptr;

		static void init_destination(j_compress_ptr cinfo)
		{
			fio_dst_ptr dst = (fio_dst_ptr) cinfo->dest;

			dst->pub.next_output_byte = dst->buffer;
			dst->pub.free_in_buffer = OUTPUT_BUF_SIZE;

			dst->error = false;
		}


		static boolean empty_output_buffer(j_compress_ptr cinfo)
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


		static void term_destination(j_compress_ptr cinfo)
		{
			fio_dst_ptr dst = (fio_dst_ptr)cinfo->dest;

			size_t ws = OUTPUT_BUF_SIZE - dst->pub.free_in_buffer;
			size_t n = dst->outfile->write(dst->buffer, 1, ws);
			if(n != ws) {
				dst->error = true;
			}
		}


		static void fio_jpeg_file_io_dst(j_compress_ptr cinfo, utils::file_io* outfile)
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


		static void error_exit_task(jpeg_common_struct *st)
		{
			error_code_ = 1;
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		jpeg_io() : prgl_ref_(0), prgl_pos_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~jpeg_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "jpg,jpeg,jfif"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	JPEG ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override
		{
			unsigned char sig[2];
			long pos = fin.tell();
			size_t l = fin.read(sig, 1, 2);
			fin.seek(pos, utils::file_io::SEEK::SET);
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
		bool info(utils::file_io& fin, img::img_info& fo) override
		{
			if(!probe(fin)) {
				return false;
			}

			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr errmgr;

			long pos = fin.tell();
			/// std::cout << "info: " << pos << std::endl;
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
			fin.seek(pos, utils::file_io::SEEK::SET);

			if(error_code_ != 0) {
///				std::endl << "JPEG decode error: 'header'(" << error_code_ << ")" << std::endl; 
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
		bool load(utils::file_io& fin, const std::string& opt = "") override
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
			if(error_code_ != 0) {
				std::cout << "JPEG decode error: 'header' (" << error_code_ << ")" << std::endl; 
				jpeg_destroy_decompress(&cinfo);
				return false;
			}

			// 解凍の開始
			error_code_ = 0;
			jpeg_start_decompress(&cinfo);
			if(error_code_ != 0) {
				std::cout << "JPEG decode error: 'decompress' (" << error_code_ << ")" << std::endl;
				jpeg_finish_decompress(&cinfo);
				jpeg_destroy_decompress(&cinfo);
				return false;
			}

			/// cinfo.in_color_space
			if(cinfo.output_components == 1) {
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(cinfo.image_width, cinfo.image_height), true);
			} else if(cinfo.output_components == 3) {
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(cinfo.image_width, cinfo.image_height), true);
			} else if(cinfo.output_components == 4) {
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(cinfo.image_width, cinfo.image_height), true);
			} else {
				img_ = 0;
				std::cout << "JPEG decode error: Can not support components: " << 
					static_cast<int>(cinfo.output_components) << std::endl;
				jpeg_finish_decompress(&cinfo);
				jpeg_destroy_decompress(&cinfo);
				return false;
			}

			prgl_ref_ = cinfo.image_height;
			prgl_pos_ = 0;

			unsigned char* line = new unsigned char[cinfo.output_width * cinfo.output_components];
			unsigned char* lines[1];
			lines[0] = &line[0];
			vtx::spos pos;
			for(pos.y = 0; pos.y < static_cast<short>(cinfo.image_height); ++pos.y) {
				jpeg_read_scanlines(&cinfo, (JSAMPLE**)lines, 1);
				unsigned char* p = &line[0];
				if(cinfo.output_components == 4) {
					for(pos.x = 0; pos.x < static_cast<short>(cinfo.image_width); ++pos.x) {
						img::rgba8 c;
						c.r = *p++;
						c.g = *p++;
						c.b = *p++;
						c.a = *p++;
						img_->put_pixel(pos, c);
					}
				} else if(cinfo.output_components == 3) {
					for(pos.x = 0; pos.x < static_cast<short>(cinfo.image_width); ++pos.x) {
						img::rgba8 c;
						c.r = *p++;
						c.g = *p++;
						c.b = *p++;
						c.a = 255;
						img_->put_pixel(pos, c);
					}
				} else if(cinfo.output_components == 1) {
					for(pos.x = 0; pos.x < static_cast<short>(cinfo.image_width); ++pos.x) {
						img::rgba8 c;
						c.b = c.g = c.r = *p++;
						c.a = 255;
						img_->put_pixel(pos, c);
					}
				}
				prgl_pos_ = pos.y;
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
			@brief	JPEG ファイル、ロード
			@param[in]	filename	ファイル名
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& filename, const std::string& opt = "") {
			utils::file_io fin;
			if(fin.open(filename, "rb")) {
				bool f = load(fin, opt);
				fin.close();
				return f;
			}
			return false;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	JPEG ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	セーブ品質の設定が可能（1～100) 省略すると「75」
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override
		{
			if(!img_) return false;
			if(img_->get_size().x == 0 || img_->get_size().y == 0) return false;

			int q = 75;
			// 0 to 100
			if(!opt.empty()) {
				try {
					q = boost::lexical_cast<int>(opt);
				} catch (boost::bad_lexical_cast&) {
					q = 75;
				}
			}

			struct jpeg_compress_struct cinfo;
			struct jpeg_error_mgr errmgr;

			// エラーのハンドリング
			cinfo.err = jpeg_std_error(&errmgr);
//			errmgr.error_exit = NULL;

			// 構造体の初期設定
			jpeg_create_compress(&cinfo);

			// file_io クラス設定
			fio_jpeg_file_io_dst(&cinfo, &fout);
			fio_dst_ptr dst = (fio_dst_ptr)cinfo.dest;

			int w = img_->get_size().x;
			int h = img_->get_size().y;

			prgl_ref_ = h;
			prgl_pos_ = 0;

			cinfo.image_width  = w;
			cinfo.image_height = h;
#if (defined JCS_ALPHA_EXTENSIONS)
			if(img_->test_alpha()) {
				cinfo.input_components = 4;
				cinfo.in_color_space = JCS_EXT_RGBA;

			} else {
				cinfo.input_components = 3;
				cinfo.in_color_space = JCS_RGB;
			}
#else
			cinfo.input_components = 3;
			cinfo.in_color_space = JCS_RGB;
#endif
			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, q, TRUE);

			jpeg_start_compress(&cinfo, TRUE);

			JSAMPROW row_pointer[1];
			unsigned char* tmp = new unsigned char[w * cinfo.input_components];
			row_pointer[0] = (JSAMPLE *)tmp;
			vtx::spos pos;
			pos.y = 0;
			while(static_cast<short>(cinfo.next_scanline) < h) {
				unsigned char* p = tmp;
				for(pos.x = 0; pos.x < w; ++pos.x) {
					rgba8 c;
					img_->get_pixel(pos, c);
					*p++ = c.r;
					*p++ = c.g;
					*p++ = c.b;
#if (defined JCS_ALPHA_EXTENSIONS)
					if(img_->test_alpha()) *p++ = c.a;
#endif
				}
				jpeg_write_scanlines(&cinfo, row_pointer, 1);
				if(dst->error) break;
				++pos.y;
				++prgl_pos_;
			}
			delete[] tmp;

			// 圧縮後のサイズ
//			int ret = row_stride * height - cinfo.dest->free_in_buffer;

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
			@brief	イメージインターフェースを取得
			@return	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		const shared_img get_image() const override { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースの登録
			@param[in]	img	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_image(shared_img img) override { img_ = img; }


		//-----------------------------------------------------------------//
		/*!
			@brief	decode/encode の進行状態を取得する
			@return 完了の場合 scale を返す
		*/
		//-----------------------------------------------------------------//
		uint32_t get_progless(uint32_t scale) const override {
			if(prgl_ref_ == 0) return 0;
			return prgl_pos_ * scale / prgl_ref_;
		} 
	};
}
