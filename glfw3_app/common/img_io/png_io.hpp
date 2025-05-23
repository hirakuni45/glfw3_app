#pragma once
//=====================================================================//
/*!	@file
	@brief	PNG 画像を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "i_img_io.hpp"
#include <png.h>
#include "img_idx8.hpp"
#include "img_rgba8.hpp"
#include <boost/format.hpp>

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	PNG 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class png_io : public i_img_io {

		shared_img	img_;

		bool		color_key_enable_;

		uint32_t	prgl_ref_;
		uint32_t	prgl_pos_;

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
				png_error(png_ptr, boost::io::str(boost::format("png_io_read_func: error (read size: %d, request: %d)")
					% static_cast<int>(s) % static_cast<int>(size)).c_str());
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
				auto str = (boost::format("png_io_write_func: error (%d/%d)")
					% static_cast<int>(s) % static_cast<int>(size)).str();
				png_error(png_ptr, str.c_str());
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


		static void err_func_(png_structp png_ptr, const char* msg)
		{
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		png_io() : color_key_enable_(false), prgl_ref_(0), prgl_pos_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~png_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "png"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・キーを設定
			@param[in]	f	「true」ならカラーキー有効
		*/
		//-----------------------------------------------------------------//
		void color_key_enable(bool f = true) { color_key_enable_ = f; }


		//-----------------------------------------------------------------//
		/*!
			@brief	PNG ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override
		{
			static const int PNG_BYTES_TO_CHECK = 4;
			unsigned char	sig[PNG_BYTES_TO_CHECK];

			long pos = fin.tell();
			size_t l = fin.read(sig, 1, PNG_BYTES_TO_CHECK);
			fin.seek(pos, utils::file_io::SEEK::SET);
			if(l == PNG_BYTES_TO_CHECK) {
				if(png_check_sig(sig, PNG_BYTES_TO_CHECK)) {
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
			long pos = fin.tell();

			if(probe(fin) == false) {
				fin.seek(pos, utils::file_io::SEEK::SET);
				return false;
			}

			//	png_ptr 構造体を確保・初期化します
			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, err_func_);
			if(png_ptr == NULL) {
				fin.seek(pos, utils::file_io::SEEK::SET);
				return false;
			}
			//  info_ptr 構造体を確保・初期化します
			png_infop info_ptr = png_create_info_struct(png_ptr);
			if(info_ptr == NULL) {
				fin.seek(pos, utils::file_io::SEEK::SET);
				return false;
			}

///			long pos = fin.tell();

			png_set_read_fn(png_ptr, (png_voidp)&fin, png_io_read_func);
			png_read_info(png_ptr, info_ptr);

//  IHDRチャンク情報を取得します
			png_uint_32	width, height;
			int bit_depth, color_type, interlace_type;
			png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
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
				png_bytep ta;
				int nt;
				png_color_16p tc;
				png_get_tRNS(png_ptr, info_ptr, &ta, &nt, &tc);
				if(nt) fo.a_depth = bit_depth;
			} else {
				fo.i_depth = 0;
				if(color_type & PNG_COLOR_MASK_ALPHA) {
					fo.a_depth = bit_depth;
				}
			}

			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

			fin.seek(pos, utils::file_io::SEEK::SET);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PNG ファイル、ロード
			@param[in]	fin		file_io クラス
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override
		{
			if(probe(fin) == false) {
				return false;
			}

			//	png_ptr 構造体を確保・初期化します
			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, err_func_);
			if(png_ptr == NULL) {
				return false;
			}
			//  info_ptr 構造体を確保・初期化します
			png_infop info_ptr = png_create_info_struct(png_ptr);
			if(info_ptr == NULL) {
				return false;
			}

			png_set_read_fn(png_ptr, (png_voidp)&fin, png_io_read_func);
			png_read_info(png_ptr, info_ptr);

//  IHDRチャンク情報を取得します
			png_uint_32	width, height;
			int bit_depth, color_type, interlace_type, comp_type, filter_type;
			png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type,
				&comp_type, &filter_type);

// フィルタータイプの取得
//		   png_read_update_info(png_ptr, info_ptr);
//			png_byte ft = png_get_filter_type(png_ptr, info_ptr);
///			std::cout << "Compression: " << comp_type << std::endl;
///			std::cout << "Filter: " << filter_type << std::endl;

			prgl_ref_ = height;
			prgl_pos_ = 0;

			int ch;
			bool alpha;
			bool gray = false;
			bool indexed = false;
   			png_bytep ta = 0;
		   	int nt = 0;
		   	png_color_16p tc = 0;
			if(color_type == PNG_COLOR_TYPE_GRAY) {
				ch = 4;
				gray = true;
				alpha = false;
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(width, height), alpha);
			} else if(color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
				ch = 4;
				gray = true;
				alpha = true;
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(width, height), alpha);
//				std::cout << "PNG gray scale with alpha\n";
			} else if(color_type & PNG_COLOR_MASK_PALETTE) {
				ch = 1;
				png_get_tRNS(png_ptr, info_ptr, &ta, &nt, &tc);
				if(nt) alpha = true; else alpha = false;
				indexed = true;
				img_ = shared_img(new img_idx8);
				img_->create(vtx::spos(width, height), alpha);
			} else if(color_type & PNG_COLOR_MASK_ALPHA) {
				ch = 4;
				alpha = true;
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(width, height), alpha);
//				std::cout << "PNG RGBA\n";
			} else {
				ch = 3;
				alpha = false;
				img_ = shared_img(new img_rgba8);
				img_->create(vtx::spos(width, height), alpha);
///				std::cout << "PNG RGB\n";
			}

			uint32_t skip = 1;
			if(bit_depth > 8) {
				skip = 2;
			}

			if(indexed) {  // カラーパレットの読み込み
				png_colorp pal;
				int num = 0;
				png_get_PLTE(png_ptr, info_ptr, &pal, &num);
				for(int i = 0; i < num; ++i) {
					uint8_t a = 255;
					if(ta && i < nt) { // アルファチャネル
						a = ta[i];
					}
					const png_color* clut = &pal[i];
					rgba8 c(clut->red, clut->green, clut->blue, a);
					img_->put_clut(i, c);
				}
			}

			png_byte* iml = new png_byte[width * ch * skip];
			vtx::spos pos;
			for(pos.y = 0; pos.y < static_cast<short>(height); ++pos.y) {
				png_read_row(png_ptr, iml, nullptr);
				png_byte* p = iml;
				for(pos.x = 0; pos.x < static_cast<short>(width); ++pos.x) {
					img::rgba8 c;
					if(indexed) {
						idx8 i(*p);
						p += skip;
						img_->put_pixel(pos, i);
					} else {
						if(gray) {
							c.r = c.g = c.b = *p;
							p += skip;
							if(alpha) { c.a = *p; p += skip; }
							else c.a = 255;
						} else {
							c.r = *p;
							p += skip;
							c.g = *p;
							p += skip;
							c.b = *p;
							p += skip;
							if(alpha) { c.a = *p; p += skip; }
							else c.a = 255;
						}
						if(color_key_enable_) {
							png_bytep ta;
							int nt;
							png_color_16p tc;
							png_get_tRNS(png_ptr, info_ptr, &ta, &nt, &tc);
							if(static_cast<unsigned short>(c.r) == tc->red
							   && static_cast<unsigned short>(c.g) == tc->green
							   && static_cast<unsigned short>(c.b) == tc->blue) {
								c.a = 0;
							}
						}
						img_->put_pixel(pos, c);
					}
				}
				prgl_pos_ = pos.y;
			}
			delete[] iml;

			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	PNG ファイル、ロード
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
			@brief	ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override
		{
			if(!img_) return false;
		
			int w = img_->get_size().x;
			int h = img_->get_size().y;
			if(w <= 0 || h <= 0) {
				return false;
			}

			//	png_ptr 構造体を確保・初期化します
			png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if(png_ptr == NULL) {
				return false;
			}
			//  info_ptr 構造体を確保・初期化します
			png_infop info_ptr = png_create_info_struct(png_ptr);
			if(info_ptr == NULL) {
				return false;
			}
			png_set_write_fn(png_ptr, (png_voidp)&fout, png_io_write_func, png_io_flush_func);

			prgl_ref_ = h;
			prgl_pos_ = 0;

			int	type;
			int ch;
			if(img_->get_type() == IMG::INDEXED8) {
				type = PNG_COLOR_TYPE_PALETTE;
				ch = 1;
			} else {
				if(img_->test_alpha()) {
					type = PNG_COLOR_TYPE_RGB_ALPHA;
					ch = 4;
				} else {
					type = PNG_COLOR_TYPE_RGB;
					ch = 3;
				}
			}

			int depth = 8;
			png_set_IHDR(png_ptr, info_ptr, w, h, depth, type,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

			if(img_->get_type() == IMG::INDEXED8) {
				int color = img_->get_clut_max();
				png_color* clut = new png_color[color];
				unsigned char* clut_trans = 0;
				if(img_->test_alpha()) clut_trans = new unsigned char[color];
				for(int i = 0; i < color; ++i) {
					rgba8 c;
					img_->get_clut(i, c);
					clut[i].red   = c.r;
					clut[i].green = c.g;
					clut[i].blue  = c.b;
					if(img_->test_alpha()) clut_trans[i] = c.a;
				}
				png_set_PLTE(png_ptr, info_ptr, clut, color);
				if(img_->test_alpha()) png_set_tRNS(png_ptr, info_ptr, clut_trans, color, nullptr);
				png_write_info(png_ptr, info_ptr);
				delete[] clut_trans;
				delete[] clut;
			} else {
				png_write_info(png_ptr, info_ptr);
			}

			vtx::spos pos;
			png_byte* iml = new png_byte[w * ch];
			for(pos.y = 0; pos.y < h; ++pos.y) {
				png_byte* p = iml;
				if(ch == 1) {
					for(pos.x = 0; pos.x < w; ++pos.x) {
						idx8 idx;
						img_->get_pixel(pos, idx);
						*p++ = idx.i;
					}
				} else if(img_->get_type() == IMG::FULL8) {
					for(pos.x = 0; pos.x < w; ++pos.x) {
						rgba8 c;
						img_->get_pixel(pos, c);
						*p++ = c.r;
						*p++ = c.g;
						*p++ = c.b;
						if(ch == 4) *p++ = c.a;
					}
				}
				png_write_row(png_ptr, iml);
				prgl_pos_ = pos.y;
			}
			delete[] iml;

			png_write_end(png_ptr, info_ptr);
			png_destroy_write_struct(&png_ptr, &info_ptr);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルをセーブする
			@param[in]	fn	ファイル名
			@param[in]	opt	フォーマット固有の拡張文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& fn, const std::string& opt = "") {
			utils::file_io out;
			if(out.open(fn, "wb")) {
				bool f = save(out, opt);
				out.close();
				return f;
			} else {
				return false;
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
			@brief	イメージの登録
			@param[in]	img	イメージ
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
