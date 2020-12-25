//=====================================================================//
/*!	@file
	@brief	TGA 画像を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "img_io/tga_io.hpp"

namespace img {

	struct tga {
		static const uint32_t HEADER_SIZE = 18;

		typedef std::vector<rgba8>	clut_type;

		struct tga_info {
			uint8_t		id_length;	// image id length
			uint8_t		clut_t;		// CLUT type
			uint8_t		image_t;	// image type
			uint16_t	clut_first;	// index of first CLUT entry
			uint16_t	clut_num;	// number of entries in CLUT
			uint8_t		clut_depth;	// depth of CLUT
			uint16_t	x;			// x-coordinate
			uint16_t	y;			// y-coordinate
			uint16_t	w;			// width of image
			uint16_t	h;			// height of image
			uint8_t		depth;		// pixel-depth of image
			uint8_t		alpha;		// alpha bits
			bool		h_flip;		// horizontal orientation
			bool		v_flip;		// vertical orientation

			tga_info() : id_length(0),
				clut_t(0), image_t(0),
				clut_first(0), clut_num(0), clut_depth(0),
				x(0), y(0), w(0), h(0), depth(0), alpha(0),
				h_flip(false), v_flip(false) { }

			bool get(utils::file_io& fio) {
				uint8_t t[HEADER_SIZE];
				if(fio.read(t, HEADER_SIZE) != HEADER_SIZE) {
					return false;
				}
				id_length = t[0];
				clut_t    = t[1];
				image_t   = t[2];
				clut_first = t[3] | (t[4] << 8);
				clut_num   = t[5] | (t[6] << 8);
				clut_depth = t[7];
				x = t[ 8] | (t[ 9] << 8);
				y = t[10] | (t[11] << 8);
				w = t[12] | (t[13] << 8);
				h = t[14] | (t[15] << 8);
				depth = t[16];
				alpha = t[17] & 0x0f;
				h_flip = (t[17] & 0x10) ? true : false;
				v_flip = (t[17] & 0x20) ? false : true;

				return true;
			}

			bool put(utils::file_io& fio) {
				uint8_t t[HEADER_SIZE];
				t[0] = id_length;
				t[1] = clut_t;
				t[2] = image_t;
				t[3] = clut_first; t[4] = clut_first >> 8;
				t[5] = clut_num; t[6] = clut_num >> 8;
				t[7] = clut_depth;
				t[8] = x; t[9] = x >> 8;
				t[10] = y; t[11] = y >> 8;
				t[12] = w; t[13] = w >> 8;
				t[14] = h; t[15] = h >> 8;
				t[16] = depth; 
				t[17] = alpha | (h_flip << 4) | (v_flip << 5);

				if(fio.write(t, HEADER_SIZE) != HEADER_SIZE) {
					return false;
				}
				return true;
			}

		};

	private:
		utils::file_io&		fio_;

		tga_info	info_;
		uint32_t	offset_;
		bool		rle_;

		public:

		tga(utils::file_io& fio) : fio_(fio), rle_(false) {
			offset_ = fio.tell();
		}

		const tga_info& get_info() const { return info_; }

		void enable_rle(bool f = true) { rle_ = f; }

		bool read_info(img::img_info& fo)
		{
			if(!fio_.seek(offset_, utils::file_io::SEEK::SET)) {
				return false;
			}

			if(!info_.get(fio_)) {
				return false;
			}

			// Indexed color
			if(info_.clut_t == 0) ;
			else if(info_.clut_t == 1) {
				fo.i_depth  = info_.depth;
				fo.clut_num = info_.clut_num;
				if(info_.clut_depth == 15) {
					fo.r_depth = 5;
					fo.g_depth = 5;
					fo.b_depth = 5;
					fo.a_depth = 0;
				} else if(info_.clut_depth == 16) {
					fo.r_depth = 5;
					fo.g_depth = 6;
					fo.b_depth = 5;
					fo.a_depth = 0;
				} else if(info_.clut_depth == 24) {
					fo.r_depth = 8;
					fo.g_depth = 8;
					fo.b_depth = 8;
					fo.a_depth = 0;
				} else if(info_.clut_depth == 32) {
					fo.r_depth = 8;
					fo.g_depth = 8;
					fo.b_depth = 8;
					fo.a_depth = 8;
				} else {
					return false;
				}
				fo.grayscale = false;
			} else {
				return false;
			}

			if(info_.image_t == 1 || info_.image_t == 9) ;
			else if(info_.image_t == 2 || info_.image_t == 10) {
				if(info_.depth == 24) {
					fo.r_depth = 8;
					fo.g_depth = 8;
					fo.b_depth = 8;
					fo.a_depth = 0;
				} else if(info_.depth == 32) {
					fo.r_depth = 8;
					fo.g_depth = 8;
					fo.b_depth = 8;
					fo.a_depth = 8;
				}
				fo.i_depth = 0;
				fo.clut_num = 0;
				fo.grayscale = false;
			} else if(info_.image_t == 3 || info_.image_t == 11) {
				fo.r_depth = 0;
				fo.g_depth = 0;
				fo.b_depth = 0;
				fo.a_depth = info_.alpha;
				fo.i_depth = info_.depth;
				fo.clut_num = 0;
				fo.grayscale = true;
			} else {
				return false;
			}

			fo.width  = info_.w;
			fo.height = info_.h;
			fo.mipmap_level = 0;
			fo.multi_level  = 0;

			rle_ = (info_.image_t & 0x08) ? true : false;

			return true;
		}

		bool write_info(shared_img img)
		{
	        info_.id_length = 0;
	
			if(img->get_type() == IMG::INDEXED8) {
				info_.clut_t = 1;
				info_.image_t = rle_ ? 9 : 1;
				info_.clut_first = 0;
				info_.clut_num = img->get_clut_max();
				info_.clut_depth = img->test_alpha() ? 32 : 24;
				info_.depth = 8;
			} else if(img->get_type() == IMG::FULL8) {
				info_.clut_t = 0;
				info_.image_t = rle_ ? 10 : 2;
				info_.clut_first = 0;
				info_.clut_num = 0;
				info_.clut_depth = 0;
				info_.depth = img->test_alpha() ? 32 : 24;
			} else {
				return false;
			}
			info_.x = 0;
			info_.y = 0;
			info_.w = img->get_size().x;
			info_.h = img->get_size().y;
			info_.alpha = img->test_alpha() ? 8 : 0;

			if(!info_.put(fio_)) {
				return false;
			}

			return true;
		}

		bool read_clut(clut_type& clut)
		{
			uint32_t n = info_.clut_num * info_.clut_depth / 8;
			if(n == 0) return false;

			if(!fio_.seek(offset_ + HEADER_SIZE + info_.id_length,
				utils::file_io::SEEK::SET)) {
				return false;
			}

			clut.reserve(info_.clut_num);
			clut.clear();
			for(uint32_t i = 0; i < info_.clut_num; ++i) {
				rgba8 c;
				if(!fio_.get(c.b)) return false;
				if(!fio_.get(c.g)) return false;
				if(!fio_.get(c.r)) return false;
				if(info_.clut_depth == 32) {
					if(!fio_.get(c.a)) return false;
				} else {
					c.a = 255;
				}
				clut.push_back(c);
			}
			return true;
		}

		bool write_clut(clut_type& clut)
		{
			for(uint32_t i = 0; i < info_.clut_num; ++i) {
				if(!fio_.put(clut[i].b)) return false;
				if(!fio_.put(clut[i].g)) return false;
				if(!fio_.put(clut[i].r)) return false;
				if(info_.clut_depth == 32) {
					if(!fio_.put(clut[i].a)) return false;
				}
			}
			return true;
		}

		inline void set_pixel_(const uint8_t* p, idx8& pix) {
			pix.i = p[0];
		}

		inline void set_pixel_(const uint8_t* p, rgba8& pix) {
			if(info_.depth == 15 || info_.depth == 16) { // 555
				uint16_t c = p[0] | (p[1] << 8);
				pix.b = c & 0x1f; pix.b |= (pix.b << 3) & 0xe0;
				pix.g = (c >> 5) & 0x1f; pix.g |= (pix.g << 3) & 0xe0;
				pix.r = (c >> 10) & 0x1f; pix.r |= (pix.r << 3) & 0xe0;
				pix.a = 255;
			} else {
				pix.b = p[0];
				pix.g = p[1];
				pix.r = p[2];
				if(info_.alpha) {
					pix.a = p[3];
				} else {
					pix.a = 255;
				}
			}
		}

		inline bool read_pixel_(idx8& pix) {
			return fio_.get(pix.i);
		}

		inline bool read_pixel_(rgba8& pix) {
			if(info_.depth == 15 || info_.depth == 16) { // 555
				uint8_t tmp[2];
				if(fio_.read(tmp, 2) != 2) return false;
				uint16_t c = tmp[0] | (tmp[1] << 8);
				pix.b = c & 0x1f; pix.b |= (pix.b << 3) & 0xe0;
				pix.g = (c >> 5) & 0x1f; pix.g |= (pix.g << 3) & 0xe0;
				pix.r = (c >> 10) & 0x1f; pix.r |= (pix.r << 3) & 0xe0;
				pix.a = 255;
			} else {
				if(!fio_.get(pix.b)) return false;
				if(!fio_.get(pix.g)) return false;
				if(!fio_.get(pix.r)) return false;
				if(info_.alpha) {
					if(!fio_.get(pix.a)) return false;
				} else {
					pix.a = 255;
				}
			}
			return true;
		}


		bool decode_rle_(uint8_t* buf)
		{
			uint8_t repeat = 0;
			uint8_t direct = 0;
			uint8_t bits = info_.depth;
			if(bits == 15) ++bits;
			uint8_t bytes = bits / 8;
			uint8_t pad[4];
    		for(uint16_t x = 0; x < info_.w; ++x) {
				if(repeat == 0 && direct == 0) {
					uint8_t head;
					if(!fio_.get(head)) {
						return false;
					}
					if(head >= 128) {
                		repeat = head - 127;
                		if(fio_.read(pad, bytes) != bytes) {
							return false;
						}
					} else {
						direct = head + 1;
					}
				}
				if(repeat > 0) {
					for(uint8_t k = 0; k < bytes; ++k) { buf[k] = pad[k]; }
					--repeat;
				} else {
					if(fio_.read(buf, bytes) != bytes) {
						return false;
					}
					--direct;
				}
				buf += bytes;
			}
			return true;
		}


		bool decode(shared_img img, uint32_t& prgl_pos)
		{
			uint32_t n = info_.clut_num * info_.clut_depth / 8;
			if(!fio_.seek(offset_ + HEADER_SIZE + info_.id_length + n,
				utils::file_io::SEEK::SET)) {
				return false;
			}

		   uint8_t* buf = 0;
			uint32_t bits = info_.depth;
			if(bits == 15) ++bits;
			if(rle_) {
				buf = new uint8_t[bits / 8 * info_.w];
			}

			vtx::spos pos;
			short dy;
			if(info_.v_flip) {
				pos.y = info_.h - 1;
				dy = -1;
			} else {
				pos.y = 0;
				dy = 1;
			}
			for(uint16_t y = 0; y < info_.h; ++y) {
				if(rle_) {
					if(!decode_rle_(buf)) {
						return false;
					}
					const uint8_t* p = buf;
					short dx;
					if(info_.h_flip) {
						pos.x = info_.w - 1;
						dx = -1;
					} else {
						pos.x = 0;
						dx = 1;
					}
					for(uint16_t x = 0; x < info_.w; ++x) {
						if(img->get_type() == img::IMG::FULL8) {
							rgba8 c;
							set_pixel_(p, c);
							img->put_pixel(pos, c);
						} else {
							idx8 i;
							set_pixel_(p, i);
							img->put_pixel(pos, i);
						}
						p += bits / 8;
						pos.x += dx;
					}
				} else {
//					const uint8_t* p = buf;
					short dx;
					if(info_.h_flip) {
						pos.x = info_.w - 1;
						dx = -1;
					} else {
						pos.x = 0;
						dx = 1;
					}
					for(uint16_t x = 0; x < info_.w; ++x) {
						if(img->get_type() == img::IMG::FULL8) {
							rgba8 c;
							if(!read_pixel_(c)) {
								return false;
							}
							img->put_pixel(pos, c);
						} else {
							idx8 c;
							if(!read_pixel_(c)) {
								return false;
							}
							img->put_pixel(pos, c);
						}
						pos.x += dx;
					}
				}
				pos.y += dy;
				prgl_pos = y;
			}
			delete[] buf;

			return true;
		}


		bool encode_rle_(const uint8_t* buf)
		{
			uint8_t repeat = 0;
			uint8_t direct = 0; 
			uint8_t bytes = info_.depth / 8;
			const uint8_t* from = buf;
			for(int x = 1; x < info_.w; ++x) {
				if(memcmp(buf, buf + bytes, bytes)) {
					if(repeat) {
						if(!fio_.put<uint8_t>(128 + repeat)) return false;
						if(fio_.write(from, bytes) != bytes) return false;
						from = buf + bytes;
						repeat = 0;
						direct = 0;
					} else {
						direct += 1;
					}
				} else {
					if(direct) {
						if(!fio_.put<uint8_t>(direct - 1)) return false;
						uint32_t n = bytes * direct;
						if(fio_.write(from, n) != n) return false;
						from = buf;
						direct = 0;
						repeat = 1;
					} else {
						++repeat;
					}
				}
				if(repeat == 128) {
					if(!fio_.put<uint8_t>(255)) return false;
					if(fio_.write(from, bytes) != bytes) return false;
					from = buf + bytes;
					direct = 0;
					repeat = 0;
				} else if (direct == 128) {
					if(!fio_.put<uint8_t>(127)) return false;
					uint32_t n = bytes * direct;
					if(fio_.write(from, n) != n) return false;
					from = buf + bytes;
					direct = 0;
					repeat = 0;
				}
				buf += bytes;
			}

			if(repeat > 0) {
				if(!fio_.put<uint8_t>(128 + repeat))  return false;
				if(fio_.write(from, bytes) != bytes) return false;
			} else {
				if(!fio_.put<uint8_t>(direct))  return false;
				uint32_t n = bytes * (direct + 1);
				if(fio_.write(from, n) != n) return false;
			}

			return true;
		}

		bool encode(shared_img img, uint32_t& prgl_pos)
		{
// std::cout << static_cast<int>(rle) << std::endl;
			uint8_t* buf = 0;
			if(rle_) {
				uint32_t n = img->get_size().x * info_.depth / 8;
				buf = new uint8_t[n];
			}
			vtx::spos pos;
			pos.y = img->get_size().y - 1;
			for(short y = 0; y < img->get_size().y; ++y) {
				if(rle_) {
					uint8_t* p = buf;
					if(img->get_type() == IMG::INDEXED8) {
						for(pos.x = 0; pos.x < img->get_size().x; ++pos.x) {
							idx8 i;
							img->get_pixel(pos, i);
							*p++ = i.i;
						}
					} else {
						for(pos.x = 0; pos.x < img->get_size().x; ++pos.x) {
							rgba8 c;
							img->get_pixel(pos, c);
							*p++ = c.b;
							*p++ = c.g;
							*p++ = c.r;
							if(info_.alpha) *p++ = c.a;
						}
					}
					if(!encode_rle_(&buf[0])) {
						delete[] buf;
						return false;
					}
				} else {
					if(img->get_type() == IMG::INDEXED8) {
						for(pos.x = 0; pos.x < img->get_size().x; ++pos.x) {
							idx8 i;
							img->get_pixel(pos, i);
							if(!fio_.put(i.i)) return false;
						}
					} else {
						for(pos.x = 0; pos.x < img->get_size().x; ++pos.x) {
							rgba8 c;
							img->get_pixel(pos, c);
							if(!fio_.put(c.b)) return false;
							if(!fio_.put(c.g)) return false;
							if(!fio_.put(c.r)) return false;
							if(img->test_alpha()) {
								if(!fio_.put(c.a)) return false;
							}
						}
					}
				}
				--pos.y;
				prgl_pos = y;
			}
			delete[] buf;
			return true;
		}
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	TGA ファイルか確認する
				※TGA形式はシグネチュアが無い為、不正なファイルでも「true」を返す場合がある
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::probe(utils::file_io& fin)
	{
		size_t pos = fin.tell();

		tga tga_(fin);
		img::img_info fo;
		bool f = tga_.read_info(fo);

		fin.seek(pos, utils::file_io::SEEK::SET);

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

		tga tga_(fin);
		bool f = tga_.read_info(fo);

		fin.seek(pos, utils::file_io::SEEK::SET);
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	TGA ファイル、ロード(utils::file_io)
		@param[in]	fin	ファイル I/O クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::load(utils::file_io& fin, const std::string& opt)
	{
		tga tga_(fin);

		img::img_info fo;
		if(!tga_.read_info(fo)) {
			return false;
		}

		prgl_pos_ = 0;
		prgl_ref_ = fo.height;

		bool alpha = false;
		if(fo.a_depth) alpha = true;

		if(fo.clut_num) {
			tga::clut_type clut;
			if(!tga_.read_clut(clut)) {
				return false;
			}

			img_ = shared_img(new img_idx8);
			img_->create(vtx::spos(fo.width, fo.height), alpha);

			for(int i = 0; i < fo.clut_num; ++i) {
				img_->put_clut(i, clut[i]);
			}

			if(!tga_.decode(img_, prgl_pos_)) {
				return false;
			}

		} else {
			img_ = shared_img(new img_rgba8);
			img_->create(vtx::spos(fo.width, fo.height), alpha);

			if(!tga_.decode(img_, prgl_pos_)) {
				return false;
			}
		}

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	TGA ファイルをセーブする(utils::file_io)
		@param[in]	fout	ファイル I/O クラス
		@param[in]	opt		フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool tga_io::save(utils::file_io& fout, const std::string& opt)
	{
		if(!img_) return false;
		if(img_->get_size().x == 0 || img_->get_size().y == 0) return false;

		prgl_pos_ = 0;
		prgl_ref_ = img_->get_size().y;

		tga tga_(fout);

		if(opt == "rle") tga_.enable_rle(); else tga_.enable_rle(false);

		if(!tga_.write_info(img_)) {
			return false;
		}

		if(img_->get_type() == IMG::INDEXED8) {
			// CLUT の書き込み
			tga::clut_type clut;
			clut.reserve(img_->get_clut_max());
			clut.clear();
			for(int i = 0; i < img_->get_clut_max(); ++i) {
				rgba8 c;
				img_->get_clut(i, c);
				clut.push_back(c);
			}
			if(!tga_.write_clut(clut)) {
				return false;
			}
		}

		// ピクセルの書き込み
		return tga_.encode(img_, prgl_pos_);
	}
}
