#pragma once
//=====================================================================//
/*!	@file
	@brief	DDS 画像を扱うクラス（ヘッダー）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2023 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "img_io/i_img_io.hpp"
#include "img_io/img_idx8.hpp"
#include "img_io/img_rgba8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	DDS 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dds_io : public i_img_io {

		shared_img	img_;

#if 0
		/// DDSurfaceDesc::flags はヘッダ内の有効な情報をあらわす
		static constexpr int DDSD_CAPS        =		0x00000001;	///< dwCaps/dwCpas2 が有効
		static constexpr int DDSD_HEIGHT      =		0x00000002;	///< dwHeight が有効
		static constexpr int DDSD_WIDTH       =		0x00000004;	///< dwWidth が有効
		static constexpr int DDSD_PITCH       =		0x00000008;	///< dwPitchOrLinearSize が Pitch を表す
		static constexpr int DDSD_PIXELFORMAT =		0x00001000;	///< dwPfSize/dwPfFlags/dwRGB〜 等の直接定義が有効
		static constexpr int DDSD_MIPMAPCOUNT =		0x00020000;	///< dwMipMapCount が有効
		static constexpr int DDSD_LINEARSIZE  =		0x00080000;	///< dwPitchOrLinearSize が LinearSize を表す
		static constexpr int DDSD_DEPTH       =		0x00800000;	///< dwDepth が有効 

		/// DDPixelFormat::flags は PixelFormat の有効な情報や形式を表す
		static constexpr int DDPF_ALPHAPIXELS     =	0x00000001;	///< RGB 以外に alpha が含まれている
		static constexpr int DDPF_ALPHA           =	0x00000002;	///< pixel は Alpha 成分のみ
		static constexpr int DDPF_FOURCC          =	0x00000004;	///< dwFourCC が有効
		static constexpr int DDPF_PALETTEINDEXED4 =	0x00000008;	///< Palet 16 colors (DX9 ではたぶん使用されない)
		static constexpr int DDPF_PALETTEINDEXED8 =	0x00000020;	///< Palet 256 colors
		static constexpr int DDPF_RGB             =	0x00000040;	///< dwRGBBitCount/dwRBitMask/dwGBitMask/dwBBitMask/dwRGBAlphaBitMask によってフォーマットが定義されていることを示す
		static constexpr int DDPF_LUMINANCE       =	0x00020000;	///< 1ch のデータが R G B すべてに展開される
		static constexpr int DDPF_BUMPDUDV        =	0x00080000;	///< pixel が符号付であることを示す (本来は bump 用) 

		/// DDSCaps::caps
		static constexpr int DDSCAPS_ALPHA   =	0x00000002;	///< Alpha が含まれている場合 (あまり参照されない)
		static constexpr int DDSCAPS_COMPLEX =	0x00000008;	///< 複数のデータが含まれている場合 Palette/Mipmap/Cube/Volume 等
		static constexpr int DDSCAPS_TEXTURE =	0x00001000;	///< 常に 1
		static constexpr int DDSCAPS_MIPMAP  =	0x00400000;	///< MipMap が存在する場合 
#endif
		static constexpr int DDSD_MIPMAPCOUNT =		0x00020000;	///< dwMipMapCount が有効

		/// DDSCaps::caps2
		static constexpr int DDSCAPS2_CUBEMAP           =	0x00000200;	///< Cubemap が存在する場合
		static constexpr int DDSCAPS2_CUBEMAP_POSITIVEX =	0x00000400;	///< X 軸「正」
		static constexpr int DDSCAPS2_CUBEMAP_NEGATIVEX =	0x00000800;	///< X 軸「負」
		static constexpr int DDSCAPS2_CUBEMAP_POSITIVEY =	0x00001000;	///< Y 軸「正」
		static constexpr int DDSCAPS2_CUBEMAP_NEGATIVEY =	0x00002000;	///< Y 軸「負」
		static constexpr int DDSCAPS2_CUBEMAP_POSITIVEZ =	0x00004000;	///< Z 軸「正」
		static constexpr int DDSCAPS2_CUBEMAP_NEGATIVEZ =	0x00008000;	///< Z 軸「負」
//		static constexpr int DDSCAPS2_VOLUME            =	0x00400000;	///< VolumeTexture の場合 

		static constexpr int CHAN_MAX = 255;

		static inline int8_t UBYTE_TO_CHAN(uint8_t b) { return b; }

		static inline uint32_t EXP5TO8R(uint32_t packedcol) {
   			return ((packedcol >> 8) & 0xf8) | ((packedcol >> 13) & 0x7);
		}

		static inline uint16_t EXP6TO8G(uint16_t packedcol) {
   			return ((packedcol >> 3) & 0xfc) | ((packedcol >>  9) & 0x3);
		}

		static inline uint8_t EXP5TO8B(uint8_t packedcol) {
			return ((packedcol << 3) & 0xf8) | ((packedcol >>  2) & 0x7);
		}

		static inline uint8_t EXP4TO8(uint8_t col) {
   			return col | (col << 4);
		}

		/* inefficient. To be efficient, it would be necessary to decode 16 pixels at once */

		typedef int8_t		DDSbyte;
		typedef uint8_t		DDSubyte;
		typedef int16_t		DDSshort;
		typedef uint16_t	DDSushort;
		typedef int32_t		DDSint;
		typedef uint32_t	DDSuint;

		typedef DDSubyte DDSchan;

		static void dxt135_decode_imageblock ( const DDSubyte *img_block_src, DDSint i, DDSint j, DDSuint dxt_type, img::rgba8& col )
		{
			const DDSushort color0 = img_block_src[0] | (img_block_src[1] << 8);
			const DDSushort color1 = img_block_src[2] | (img_block_src[3] << 8);
			const DDSuint bits = img_block_src[4] | (img_block_src[5] << 8) | (img_block_src[6] << 16) | (img_block_src[7] << 24);
			/* What about big/little endian? */
			DDSubyte bit_pos = 2 * (j * 4 + i) ;
			DDSubyte code = (DDSubyte) ((bits >> bit_pos) & 3);

			col.a = CHAN_MAX;
			switch (code) {
			case 0:
				col.r = UBYTE_TO_CHAN( EXP5TO8R(color0) );
				col.g = UBYTE_TO_CHAN( EXP6TO8G(color0) );
				col.b = UBYTE_TO_CHAN( EXP5TO8B(color0) );
			break;
			case 1:
				col.r = UBYTE_TO_CHAN( EXP5TO8R(color1) );
				col.g = UBYTE_TO_CHAN( EXP6TO8G(color1) );
				col.b = UBYTE_TO_CHAN( EXP5TO8B(color1) );
				break;
			case 2:
				if(color0 > color1) {
					col.r = UBYTE_TO_CHAN( ((EXP5TO8R(color0) * 2 + EXP5TO8R(color1)) / 3) );
					col.g = UBYTE_TO_CHAN( ((EXP6TO8G(color0) * 2 + EXP6TO8G(color1)) / 3) );
					col.b = UBYTE_TO_CHAN( ((EXP5TO8B(color0) * 2 + EXP5TO8B(color1)) / 3) );
				} else {
					col.r = UBYTE_TO_CHAN( ((EXP5TO8R(color0) + EXP5TO8R(color1)) / 2) );
					col.g = UBYTE_TO_CHAN( ((EXP6TO8G(color0) + EXP6TO8G(color1)) / 2) );
					col.b = UBYTE_TO_CHAN( ((EXP5TO8B(color0) + EXP5TO8B(color1)) / 2) );
				}
				break;
			case 3:
				if((dxt_type > 1) || (color0 > color1)) {
					col.r = UBYTE_TO_CHAN( ((EXP5TO8R(color0) + EXP5TO8R(color1) * 2) / 3) );
					col.g = UBYTE_TO_CHAN( ((EXP6TO8G(color0) + EXP6TO8G(color1) * 2) / 3) );
					col.b = UBYTE_TO_CHAN( ((EXP5TO8B(color0) + EXP5TO8B(color1) * 2) / 3) );
				} else {
					col.r = 0;
					col.g = 0;
					col.b = 0;
					if(dxt_type == 1) col.a = UBYTE_TO_CHAN(0);
				}
				break;
			default:
				/* CANNOT happen (I hope) */
				break;
			}
		}

#if 0
		static void fetch_2d_texel_rgb_dxt1(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, img::rgba8& col)
		{
			/* Extract the (i,j) pixel from pixdata and return it
			 * in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
			 */

			const DDSubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 8);
			dxt135_decode_imageblock(blksrc, (i&3), (j&3), 0, col);
		}
#endif

		static void fetch_2d_texel_rgba_dxt1(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, img::rgba8& col)
		{
			/* Extract the (i,j) pixel from pixdata and return it
			 * in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
			 */

			const DDSubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 8);
			dxt135_decode_imageblock(blksrc, (i&3), (j&3), 1, col);
		}

		static void fetch_2d_texel_rgba_dxt3(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, img::rgba8& col)
		{
			/* Extract the (i,j) pixel from pixdata and return it
			 * in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
			 */

			const DDSubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 16);
#if 0
			/* Simple 32bit version. */
			/* that's pretty brain-dead for a single pixel, isn't it? */
			const DDSubyte bit_pos = 4 * ((j&3) * 4 + (i&3));
			const DDSuint alpha_low = blksrc[0] | (blksrc[1] << 8) | (blksrc[2] << 16) | (blksrc[3] << 24);
			const DDSuint alpha_high = blksrc[4] | (blksrc[5] << 8) | (blksrc[6] << 16) | (blksrc[7] << 24);

			dxt135_decode_imageblock(blksrc + 8, (i&3), (j&3), 2, col);
			if(bit_pos < 32) {
				col.a = UBYTE_TO_CHAN( (DDSubyte)(EXP4TO8((alpha_low >> bit_pos) & 15)) );
			} else {
				col.a = UBYTE_TO_CHAN( (DDSubyte)(EXP4TO8((alpha_high >> (bit_pos - 32)) & 15)) );
			}
#endif
#if 1
			/* TODO test this! */
			const DDSubyte anibble = (blksrc[((j&3) * 4 + (i&3)) / 2] >> (4 * (i&1))) & 0xf;
			dxt135_decode_imageblock(blksrc + 8, (i&3), (j&3), 2, col);
			col.a = UBYTE_TO_CHAN( (DDSubyte)(EXP4TO8(anibble)) );
#endif
		}

		static void fetch_2d_texel_rgba_dxt5(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, img::rgba8& col)
		{
			/* Extract the (i,j) pixel from pixdata and return it
			 * in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
			 */

			const DDSubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 16);
			const DDSubyte alpha0 = blksrc[0];
			const DDSubyte alpha1 = blksrc[1];
#if 0

			const DDSubyte bit_pos = 3 * ((j&3) * 4 + (i&3));
			/* simple 32bit version */
			const DDSuint bits_low = blksrc[2] | (blksrc[3] << 8) | (blksrc[4] << 16) | (blksrc[5] << 24);
			const DDSuint bits_high = blksrc[6] | (blksrc[7] << 8);

			DDSubyte code;
			if (bit_pos < 30) {
				code = (DDSubyte) ((bits_low >> bit_pos) & 7);
			} else if (bit_pos == 30) {
				code = (DDSubyte) ((bits_low >> 30) & 3) | ((bits_high << 2) & 4);
			} else {
				code = (DDSubyte) ((bits_high >> (bit_pos - 32)) & 7);
			}
#endif
#if 1
			/* TODO test this! */
			const DDSubyte bit_pos = ((j&3) * 4 + (i&3)) * 3;
			const DDSubyte acodelow = blksrc[2 + bit_pos / 8];
			const DDSubyte acodehigh = blksrc[3 + bit_pos / 8];
			const DDSubyte code = (acodelow >> (bit_pos & 0x7) |
							   (acodehigh  << (8 - (bit_pos & 0x7)))) & 0x7;
#endif
			dxt135_decode_imageblock(blksrc + 8, (i&3), (j&3), 2, col);
			/* TODO test this */
			if(code == 0) {
				col.a = UBYTE_TO_CHAN( alpha0 );
			} else if (code == 1) {
				col.a = UBYTE_TO_CHAN( alpha1 );
			} else if (alpha0 > alpha1) {
				col.a = UBYTE_TO_CHAN( ((alpha0 * (8 - code) + (alpha1 * (code - 1))) / 7) );
			} else if (code < 6) {
				col.a = UBYTE_TO_CHAN( ((alpha0 * (6 - code) + (alpha1 * (code - 1))) / 5) );
			} else if (code == 6) {
				col.a = 0;
			} else {
				col.a = CHAN_MAX;
			}
		}

		struct DDPixelFormat
		{
			uint32_t size;
			uint32_t flgas;
			uint32_t fourCC;
			uint32_t bpp;
			uint32_t redMask;
			uint32_t greenMask;
			uint32_t blueMask;
			uint32_t alphaMask;
		};

		struct DDSCaps
		{
			uint32_t caps;
			uint32_t caps2;
			uint32_t caps3;
			uint32_t caps4;
		};

		struct DDColorKey
		{
			uint32_t lowVal;
			uint32_t hy;
		};

		struct DDSurfaceDesc
		{
			uint32_t size;
			uint32_t flags;
			uint32_t height;
			uint32_t width;
			uint32_t pitch;
			uint32_t depth;
			uint32_t mipMapLevels;
			uint32_t alphaBitDepth;
			uint32_t reserved;
			uint32_t surface;

			DDColorKey ckDestOverlay;
			DDColorKey ckDestBlt;
			DDColorKey ckSrcOverlay;
			DDColorKey ckSrcBlt;

			DDPixelFormat format;
			DDSCaps caps;

			uint32_t textureStage;
		};

		static void make_info(const DDSurfaceDesc& ddsd, img::img_info& fo)
		{
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = ddsd.alphaBitDepth;
			fo.i_depth = 0;
			fo.clut_num = 0;
			fo.width  = ddsd.width;
			fo.height = ddsd.height;

			int l = 0;
			if(ddsd.flags & DDSD_MIPMAPCOUNT) {
				l = ddsd.mipMapLevels;
			}
			fo.mipmap_level = l;

			l = 0;
			if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP) {
				if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP_POSITIVEX) ++l;
				if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) ++l;
				if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP_POSITIVEY) ++l;
				if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) ++l;
				if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) ++l;
				if(ddsd.caps.caps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ) ++l;
			}
			fo.multi_level = l;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		dds_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~dds_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "dds"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	DDS ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override
		{
			char magic[5];
			magic[4] = 0;

			if(fin.read(magic, 1, 4) != 4) {
				return false;
			}

			if(strncmp(magic, "DDS ", 4) == 0) {
				return true;
			} else {
				return false;
			}
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

			DDSurfaceDesc ddsd;
			if(fin.read(&ddsd, 1, sizeof(DDSurfaceDesc)) != sizeof(DDSurfaceDesc)) {
				return false;
			}

			make_info(ddsd, fo);

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DDS ファイル、ロード(utils::file_io)
			@param[in]	fin	ファイル I/O クラス
			@param[in]	ext	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& ext = "") override
		{
			if(!probe(fin)) {
				return false;
			}

			int multi_level = 0;
			int mipmap_level = 0;
			if(!ext.empty()) {
				utils::strings ss = utils::split_text(ext, ",");
				BOOST_FOREACH(const string& s, ss) {
					int n = -1;
					if(sscanf(s.c_str(), "multi:%d", &n) == 1) {
						if(n > 0 && n < 6) {
							multi_level = n;
						}
					} else if(sscanf(s.c_str(), "mipmap:%d", &n) == 1) {
						if(n > 0 && n <= 12) {
							mipmap_level = n;
						}
					}
				}
			}

			DDSurfaceDesc ddsd;
			if(fin.read(&ddsd, 1, sizeof(DDSurfaceDesc)) != sizeof(DDSurfaceDesc)) {
				return false;
			}

			img::img_info fo;
			make_info(ddsd, fo);
#if 0
			boost::format("load Mipmap: %d\n") % fo.mipmap_level;
			boost::format("load Multi:  %d\n") % fo.multi_level;

			boost::format("sizeof: %d\n") % sizeof(DDSurfaceDesc);
			boost::format("Size: %d\n") % ddsd.size);
			boost::format("Flags: %d\n") % ddsd.flags;
			boost::format("W/H: %d, %d\n") % ddsd.width % ddsd.height;
			boost::format("Mipmap level: %d\n") % ddsd.mipMapLevels;
			boost::format("Pitch: %d\n") % ddsd.pitch;
			boost::format("Depth: %d\n") % ddsd.depth;
			boost::format("Surfcae: %d\n") % ddsd.surface;

			boost::format("Tex-Stage: %d\n") % ddsd.textureStage;

			char fo[5];
			const char* p = (const char *)&ddsd.format.fourCC;
			fo[0] = p[0];
			fo[1] = p[1];
			fo[2] = p[2];
			fo[3] = p[3];
			fo[4] = 0;
			boost::format("format: '%s'\n") % fo;
			boost::format("Caps: %08X\n") % ddsd.caps.caps2;

			boost::format("BPP:   %d\n") % ddsd.format.bpp;
			boost::format("MaskR: %08X\n") % ddsd.format.redMask;
			boost::format("MaskG: %08X\n") % ddsd.format.greenMask;
			boost::format("MaskB: %08X\n") % ddsd.format.blueMask;
			boost::format("MaskA: %08X\n") % ddsd.format.alphaMask;
#endif

			enum {
				form_RGBA,
				form_DXT1,
				form_DXT2,
				form_DXT3,
				form_DXT4,
				form_DXT5,
				form_F16,
			};
			int form = -1;
			int multi = 1;
			int divide = 1;
			if(ddsd.format.fourCC == 0) {
				form = form_RGBA;
				multi = 4;
			} else if(ddsd.format.fourCC == 'q') {
				form = form_F16;
			} else if(strncmp((const char *)&ddsd.format.fourCC, "DXT1", 4) == 0) {
				form = form_DXT1;
				divide = 2;
			} else if(strncmp((const char *)&ddsd.format.fourCC, "DXT2", 4) == 0) {
				form = form_DXT2;
				divide = 2;
			} else if(strncmp((const char *)&ddsd.format.fourCC, "DXT3", 4) == 0) {
				form = form_DXT3;
			} else if(strncmp((const char *)&ddsd.format.fourCC, "DXT4", 4) == 0) {
				form = form_DXT4;
			} else if(strncmp((const char *)&ddsd.format.fourCC, "DXT5", 4) == 0) {
				form = form_DXT5;
			}
			int width = ddsd.width;
			int height = ddsd.height;

			size_t size = (width * multi / divide) * height;

			// mipmap のサイズ
			size_t mipmap = 0;
			if(fo.mipmap_level > 1) {
				int w = width;
				int h = height;
				for(int i = 0; i < (fo.mipmap_level - 1); ++i) {
					w >>= 1;
					h >>= 1;
					size_t n = (w * multi / divide) * h;
					if(w > 0 && h > 0 && n < 16) n = 16;	/// 16 バイトでアライメントする！
					mipmap += n;
				}
			}

			size_t skip = 0;

			if(fo.multi_level > 0) {
				skip += (size + mipmap) * multi_level;
			}

			if(mipmap_level > 0) {
				while(mipmap_level > 0) {
					size_t n = (width * multi / divide) * height;
					if(n < 16) n = 16;
					skip += n;
					width >>= 1;
					height >>= 1;
					mipmap_level--;
				}
			}

			if(skip) {
				if(!fin.seek(skip, utils::file_io::SEEK::CUR)) {
					return false;
				}
			}

//			boost::format("%d, %d\n") % width % height;

			switch(form) {
			case form_RGBA:
				{
					img_ = shared_img(new img_rgba8);
					unsigned char* buff = new unsigned char[width * 4];
					img_->create(vtx::spos(width, height), true);
					vtx::spos pos;
					for(pos.y = 0; pos.y < height; ++pos.y) {
						fin.read(buff, 4, width);
						for(pos.x = 0; pos.x < width; ++pos.x) {
							int i = pos.x * 4;
							img::rgba8 c;
							c.set(buff[i + 2], buff[i + 1], buff[i + 0], 255);
							img_->put_pixel(pos, c);
						}
					}
					delete[] buff;
				}
				break;
			case form_DXT1:
				{
					img_ = shared_img(new img_rgba8);
					img_->create(vtx::spos(width, height), true);
					unsigned char* buff = new unsigned char[width * height];
					fin.read(buff, 1, width * height);
					vtx::spos pos;
					for(pos.y = 0; pos.y < height; ++pos.y) {
						for(pos.x = 0; pos.x < width; ++pos.x) {
							img::rgba8 c;
							fetch_2d_texel_rgba_dxt1(width / 2, buff, pos.x, pos.y, c);
							// fetch_2d_texel_rgba_dxt1(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, DDSvoid *texel);
							img_->put_pixel(pos, c);
						}
					}
					delete[] buff;
				}
				break;
			case form_DXT3:
				{
					img_ = shared_img(new img_rgba8);
					img_->create(vtx::spos(width, height), true);
					unsigned char* buff = new unsigned char[width * height];
					fin.read(buff, 1, width * height);
					vtx::spos pos;
					for(pos.y = 0; pos.y < height; ++pos.y) {
						for(pos.x = 0; pos.x < width; ++pos.x) {
							img::rgba8 c;
							fetch_2d_texel_rgba_dxt3(width, (DDSubyte*)buff, pos.x, pos.y, c);
							img_->put_pixel(pos, c);
						}
					}
					delete[] buff;
				}
				break;
			case form_DXT5:
				{
					img_ = shared_img(new img_rgba8);
					img_->create(vtx::spos(width, height), true);
					unsigned char* buff = new unsigned char[width * height];
					fin.read(buff, 1, width * height);
					vtx::spos pos;
					for(pos.y = 0; pos.y < height; ++pos.y) {
						for(pos.x = 0; pos.x < width; ++pos.x) {
							img::rgba8 c;
							fetch_2d_texel_rgba_dxt5(width, (DDSubyte*)buff, pos.x, pos.y, c);
							img_->put_pixel(pos, c);
						}
					}
					delete[] buff;
				}
				break;
			case form_F16:
				{
				}
				break;
			default:
				std::cout << "Can't decode DDS format..." << std::endl;
				break;
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DDS ファイルをセーブする
			@param[in]	fout	ファイル I/O クラス
			@param[in]	ext	フォーマット固有の設定文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& ext = "") override
		{
			if(!img_) return false;
			int w = img_->get_size().x;
			int h = img_->get_size().y;
			if(w <= 0 || h <= 0) {
				return false;
			}



			return false;
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
			@param[in]	imf	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_image(shared_img img) override { img_ = img; }
	};

}

