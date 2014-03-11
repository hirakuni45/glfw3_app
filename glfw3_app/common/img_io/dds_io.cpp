//=====================================================================//
/*!	@file
	@brief	DDS 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "img_io/dds_io.hpp"

namespace img {

	using namespace std;

	struct DDPixelFormat
	{
		unsigned int size;
		unsigned int flgas;
		unsigned int fourCC;
		unsigned int bpp;
		unsigned int redMask;
		unsigned int greenMask;
		unsigned int blueMask;
		unsigned int alphaMask;
	};

	struct DDSCaps
	{
		unsigned int caps;
		unsigned int caps2;
		unsigned int caps3;
		unsigned int caps4;
	};

	struct DDColorKey
	{
		unsigned int lowVal;
		unsigned int hy;
	};

	struct DDSurfaceDesc
	{
		unsigned int size;
		unsigned int flags;
		unsigned int height;
		unsigned int width;
		unsigned int pitch;
		unsigned int depth;
		unsigned int mipMapLevels;
		unsigned int alphaBitDepth;
		unsigned int reserved;
		unsigned int surface;

		DDColorKey ckDestOverlay;
		DDColorKey ckDestBlt;
		DDColorKey ckSrcOverlay;
		DDColorKey ckSrcBlt;

		DDPixelFormat format;
		DDSCaps caps;

		unsigned int textureStage;
	};

	/// DDSurfaceDesc::flags はヘッダ内の有効な情報をあらわす
	static const int DDSD_CAPS        =		0x00000001;	///< dwCaps/dwCpas2 が有効
	static const int DDSD_HEIGHT      =		0x00000002;	///< dwHeight が有効
	static const int DDSD_WIDTH       =		0x00000004;	///< dwWidth が有効
	static const int DDSD_PITCH       =		0x00000008;	///< dwPitchOrLinearSize が Pitch を表す
	static const int DDSD_PIXELFORMAT =		0x00001000;	///< dwPfSize/dwPfFlags/dwRGB〜 等の直接定義が有効
	static const int DDSD_MIPMAPCOUNT =		0x00020000;	///< dwMipMapCount が有効
	static const int DDSD_LINEARSIZE  =		0x00080000;	///< dwPitchOrLinearSize が LinearSize を表す
	static const int DDSD_DEPTH       =		0x00800000;	///< dwDepth が有効 

	/// DDPixelFormat::flags は PixelFormat の有効な情報や形式を表す
	static const int DDPF_ALPHAPIXELS     =	0x00000001;	///< RGB 以外に alpha が含まれている
	static const int DDPF_ALPHA           =	0x00000002;	///< pixel は Alpha 成分のみ
	static const int DDPF_FOURCC          =	0x00000004;	///< dwFourCC が有効
	static const int DDPF_PALETTEINDEXED4 =	0x00000008;	///< Palet 16 colors (DX9 ではたぶん使用されない)
	static const int DDPF_PALETTEINDEXED8 =	0x00000020;	///< Palet 256 colors
	static const int DDPF_RGB             =	0x00000040;	///< dwRGBBitCount/dwRBitMask/dwGBitMask/dwBBitMask/dwRGBAlphaBitMask によってフォーマットが定義されていることを示す
	static const int DDPF_LUMINANCE       =	0x00020000;	///< 1ch のデータが R G B すべてに展開される
	static const int DDPF_BUMPDUDV        =	0x00080000;	///< pixel が符号付であることを示す (本来は bump 用) 

	/// DDSCaps::caps
	static const int DDSCAPS_ALPHA   =	0x00000002;	///< Alpha が含まれている場合 (あまり参照されない)
	static const int DDSCAPS_COMPLEX =	0x00000008;	///< 複数のデータが含まれている場合 Palette/Mipmap/Cube/Volume 等
	static const int DDSCAPS_TEXTURE =	0x00001000;	///< 常に 1
	static const int DDSCAPS_MIPMAP  =	0x00400000;	///< MipMap が存在する場合 

	/// DDSCaps::caps2
	static const int DDSCAPS2_CUBEMAP           =	0x00000200;	///< Cubemap が存在する場合
	static const int DDSCAPS2_CUBEMAP_POSITIVEX =	0x00000400;	///< X 軸「正」
	static const int DDSCAPS2_CUBEMAP_NEGATIVEX =	0x00000800;	///< X 軸「負」
	static const int DDSCAPS2_CUBEMAP_POSITIVEY =	0x00001000;	///< Y 軸「正」
	static const int DDSCAPS2_CUBEMAP_NEGATIVEY =	0x00002000;	///< Y 軸「負」
	static const int DDSCAPS2_CUBEMAP_POSITIVEZ =	0x00004000;	///< Z 軸「正」
	static const int DDSCAPS2_CUBEMAP_NEGATIVEZ =	0x00008000;	///< Z 軸「負」
	static const int DDSCAPS2_VOLUME            =	0x00400000;	///< VolumeTexture の場合 


#define UBYTE_TO_CHAN(b)  (b)
	static const int CHAN_MAX = 255;

#define EXP5TO8R(packedcol)					\
   ((((packedcol) >> 8) & 0xf8) | (((packedcol) >> 13) & 0x7))

#define EXP6TO8G(packedcol)					\
   ((((packedcol) >> 3) & 0xfc) | (((packedcol) >>  9) & 0x3))

#define EXP5TO8B(packedcol)					\
   ((((packedcol) << 3) & 0xf8) | (((packedcol) >>  2) & 0x7))

#define EXP4TO8(col)						\
   ((col) | ((col) << 4))

/* inefficient. To be efficient, it would be necessary to decode 16 pixels at once */

	typedef char			DDSbyte;
	typedef unsigned char	DDSubyte;
	typedef short			DDSshort;
	typedef unsigned short	DDSushort;
	typedef int				DDSint;
	typedef unsigned int	DDSuint;

	typedef DDSubyte DDSchan;

	static void dxt135_decode_imageblock ( const DDSubyte *img_block_src, DDSint i, DDSint j, DDSuint dxt_type, img::rgba8& col )
	{
		const DDSushort color0 = img_block_src[0] | (img_block_src[1] << 8);
		const DDSushort color1 = img_block_src[2] | (img_block_src[3] << 8);
		const DDSuint bits = img_block_src[4] | (img_block_src[5] << 8) |
				(img_block_src[6] << 16) | (img_block_src[7] << 24);
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


	static void fetch_2d_texel_rgb_dxt1(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, img::rgba8& col)
	{
		/* Extract the (i,j) pixel from pixdata and return it
		 * in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
		 */

		const DDSubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 8);
		dxt135_decode_imageblock(blksrc, (i&3), (j&3), 0, col);
	}


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


#if 0

#define REDWEIGHT 4
#define GREENWEIGHT 16
#define BLUEWEIGHT 1

#define ALPHACUT 127

static void fancybasecolorsearch( GLubyte *blkaddr, GLubyte srccolors[4][4][4], GLubyte *bestcolor[2],
                           GLint numxpixels, GLint numypixels, GLint type, GLboolean haveAlpha)
{
   /* use same luminance-weighted distance metric to determine encoding as for finding the base colors */

   /* TODO could also try to find a better encoding for the 3-color-encoding type, this really should be done
      if it's rgba_dxt1 and we have alpha in the block, currently even values which will be mapped to black
      due to their alpha value will influence the result */
   GLint i, j, colors, z;
   GLuint pixerror, pixerrorred, pixerrorgreen, pixerrorblue, pixerrorbest;
   GLint colordist, blockerrlin[2][3];
   GLubyte nrcolor[2];
   GLint pixerrorcolorbest[3];
   GLubyte enc = 0;
   GLubyte cv[4][4];
   GLubyte testcolor[2][3];

   if (((bestcolor[0][0] & 0xf8) << 8 | (bestcolor[0][1] & 0xfc) << 3 | bestcolor[0][2] >> 3) <
      ((bestcolor[1][0] & 0xf8) << 8 | (bestcolor[1][1] & 0xfc) << 3 | bestcolor[1][2] >> 3)) {
      testcolor[0][0] = bestcolor[0][0];
      testcolor[0][1] = bestcolor[0][1];
      testcolor[0][2] = bestcolor[0][2];
      testcolor[1][0] = bestcolor[1][0];
      testcolor[1][1] = bestcolor[1][1];
      testcolor[1][2] = bestcolor[1][2];
   }
   else {
      testcolor[1][0] = bestcolor[0][0];
      testcolor[1][1] = bestcolor[0][1];
      testcolor[1][2] = bestcolor[0][2];
      testcolor[0][0] = bestcolor[1][0];
      testcolor[0][1] = bestcolor[1][1];
      testcolor[0][2] = bestcolor[1][2];
   }

   for (i = 0; i < 3; i ++) {
      cv[0][i] = testcolor[0][i];
      cv[1][i] = testcolor[1][i];
      cv[2][i] = (testcolor[0][i] * 2 + testcolor[1][i]) / 3;
      cv[3][i] = (testcolor[0][i] + testcolor[1][i] * 2) / 3;
   }

   blockerrlin[0][0] = 0;
   blockerrlin[0][1] = 0;
   blockerrlin[0][2] = 0;
   blockerrlin[1][0] = 0;
   blockerrlin[1][1] = 0;
   blockerrlin[1][2] = 0;

   nrcolor[0] = 0;
   nrcolor[1] = 0;

   for (j = 0; j < numypixels; j++) {
      for (i = 0; i < numxpixels; i++) {
         pixerrorbest = 0xffffffff;
         for (colors = 0; colors < 4; colors++) {
            colordist = srccolors[j][i][0] - (cv[colors][0]);
            pixerror = colordist * colordist * REDWEIGHT;
            pixerrorred = colordist;
            colordist = srccolors[j][i][1] - (cv[colors][1]);
            pixerror += colordist * colordist * GREENWEIGHT;
            pixerrorgreen = colordist;
            colordist = srccolors[j][i][2] - (cv[colors][2]);
            pixerror += colordist * colordist * BLUEWEIGHT;
            pixerrorblue = colordist;
            if (pixerror < pixerrorbest) {
               enc = colors;
               pixerrorbest = pixerror;
               pixerrorcolorbest[0] = pixerrorred;
               pixerrorcolorbest[1] = pixerrorgreen;
               pixerrorcolorbest[2] = pixerrorblue;
            }
         }
         if (enc == 0) {
            for (z = 0; z < 3; z++) {
               blockerrlin[0][z] += 3 * pixerrorcolorbest[z];
            }
            nrcolor[0] += 3;
         }
         else if (enc == 2) {
            for (z = 0; z < 3; z++) {
               blockerrlin[0][z] += 2 * pixerrorcolorbest[z];
            }
            nrcolor[0] += 2;
            for (z = 0; z < 3; z++) {
               blockerrlin[1][z] += 1 * pixerrorcolorbest[z];
            }
            nrcolor[1] += 1;
         }
         else if (enc == 3) {
            for (z = 0; z < 3; z++) {
               blockerrlin[0][z] += 1 * pixerrorcolorbest[z];
            }
            nrcolor[0] += 1;
            for (z = 0; z < 3; z++) {
               blockerrlin[1][z] += 2 * pixerrorcolorbest[z];
            }
            nrcolor[1] += 2;
         }
         else if (enc == 1) {
            for (z = 0; z < 3; z++) {
               blockerrlin[1][z] += 3 * pixerrorcolorbest[z];
            }
            nrcolor[1] += 3;
         }
      }
   }
   if (nrcolor[0] == 0) nrcolor[0] = 1;
   if (nrcolor[1] == 0) nrcolor[1] = 1;
   for (j = 0; j < 2; j++) {
      for (i = 0; i < 3; i++) {
	 GLint newvalue = testcolor[j][i] + blockerrlin[j][i] / nrcolor[j];
	 if (newvalue <= 0)
	    testcolor[j][i] = 0;
	 else if (newvalue >= 255)
	    testcolor[j][i] = 255;
	 else testcolor[j][i] = newvalue;
      }
   }

   if ((abs(testcolor[0][0] - testcolor[1][0]) < 8) &&
       (abs(testcolor[0][1] - testcolor[1][1]) < 4) &&
       (abs(testcolor[0][2] - testcolor[1][2]) < 8)) {
       /* both colors are so close they might get encoded as the same 16bit values */
      GLubyte coldiffred, coldiffgreen, coldiffblue, coldiffmax, factor, ind0, ind1;

      coldiffred = abs(testcolor[0][0] - testcolor[1][0]);
      coldiffgreen = 2 * abs(testcolor[0][1] - testcolor[1][1]);
      coldiffblue = abs(testcolor[0][2] - testcolor[1][2]);
      coldiffmax = coldiffred;
      if (coldiffmax < coldiffgreen) coldiffmax = coldiffgreen;
      if (coldiffmax < coldiffblue) coldiffmax = coldiffblue;
      if (coldiffmax > 0) {
         if (coldiffmax > 4) factor = 2;
         else if (coldiffmax > 2) factor = 3;
         else factor = 4;
         /* Won't do much if the color value is near 255... */
         /* argh so many ifs */
         if (testcolor[1][1] >= testcolor[0][1]) {
            ind1 = 1; ind0 = 0;
         }
         else {
            ind1 = 0; ind0 = 1;
         }
         if ((testcolor[ind1][1] + factor * coldiffgreen) <= 255)
            testcolor[ind1][1] += factor * coldiffgreen;
         else testcolor[ind1][1] = 255;
         if ((testcolor[ind1][0] - testcolor[ind0][1]) > 0) {
            if ((testcolor[ind1][0] + factor * coldiffred) <= 255)
               testcolor[ind1][0] += factor * coldiffred;
            else testcolor[ind1][0] = 255;
         }
         else {
            if ((testcolor[ind0][0] + factor * coldiffred) <= 255)
               testcolor[ind0][0] += factor * coldiffred;
            else testcolor[ind0][0] = 255;
         }
         if ((testcolor[ind1][2] - testcolor[ind0][2]) > 0) {
            if ((testcolor[ind1][2] + factor * coldiffblue) <= 255)
               testcolor[ind1][2] += factor * coldiffblue;
            else testcolor[ind1][2] = 255;
         }
         else {
            if ((testcolor[ind0][2] + factor * coldiffblue) <= 255)
               testcolor[ind0][2] += factor * coldiffblue;
            else testcolor[ind0][2] = 255;
         }
      }
   }

   if (((testcolor[0][0] & 0xf8) << 8 | (testcolor[0][1] & 0xfc) << 3 | testcolor[0][2] >> 3) <
      ((testcolor[1][0] & 0xf8) << 8 | (testcolor[1][1] & 0xfc) << 3 | testcolor[1][2]) >> 3) {
      for (i = 0; i < 3; i++) {
         bestcolor[0][i] = testcolor[0][i];
         bestcolor[1][i] = testcolor[1][i];
      }
   }
   else {
      for (i = 0; i < 3; i++) {
         bestcolor[0][i] = testcolor[1][i];
         bestcolor[1][i] = testcolor[0][i];
      }
   }
}



static void storedxtencodedblock( GLubyte *blkaddr, GLubyte srccolors[4][4][4], GLubyte *bestcolor[2],
                           GLint numxpixels, GLint numypixels, GLuint type, GLboolean haveAlpha)
{
   /* use same luminance-weighted distance metric to determine encoding as for finding the base colors */

   GLint i, j, colors;
   GLuint testerror, testerror2, pixerror, pixerrorbest;
   GLint colordist;
   GLushort color0, color1, tempcolor;
   GLuint bits = 0, bits2 = 0;
   GLubyte *colorptr;
   GLubyte enc = 0;
   GLubyte cv[4][4];

   bestcolor[0][0] = bestcolor[0][0] & 0xf8;
   bestcolor[0][1] = bestcolor[0][1] & 0xfc;
   bestcolor[0][2] = bestcolor[0][2] & 0xf8;
   bestcolor[1][0] = bestcolor[1][0] & 0xf8;
   bestcolor[1][1] = bestcolor[1][1] & 0xfc;
   bestcolor[1][2] = bestcolor[1][2] & 0xf8;

   color0 = bestcolor[0][0] << 8 | bestcolor[0][1] << 3 | bestcolor[0][2] >> 3;
   color1 = bestcolor[1][0] << 8 | bestcolor[1][1] << 3 | bestcolor[1][2] >> 3;
   if (color0 < color1) {
      tempcolor = color0; color0 = color1; color1 = tempcolor;
      colorptr = bestcolor[0]; bestcolor[0] = bestcolor[1]; bestcolor[1] = colorptr;
   }


   for (i = 0; i < 3; i ++) {
      cv[0][i] = bestcolor[0][i];
      cv[1][i] = bestcolor[1][i];
      cv[2][i] = (bestcolor[0][i] * 2 + bestcolor[1][i]) / 3;
      cv[3][i] = (bestcolor[0][i] + bestcolor[1][i] * 2) / 3;
   }

   testerror = 0;
   for (j = 0; j < numypixels; j++) {
      for (i = 0; i < numxpixels; i++) {
         pixerrorbest = 0xffffffff;
         for (colors = 0; colors < 4; colors++) {
            colordist = srccolors[j][i][0] - cv[colors][0];
            pixerror = colordist * colordist * REDWEIGHT;
            colordist = srccolors[j][i][1] - cv[colors][1];
            pixerror += colordist * colordist * GREENWEIGHT;
            colordist = srccolors[j][i][2] - cv[colors][2];
            pixerror += colordist * colordist * BLUEWEIGHT;
            if (pixerror < pixerrorbest) {
               pixerrorbest = pixerror;
               enc = colors;
            }
         }
         testerror += pixerrorbest;
         bits |= enc << (2 * (j * 4 + i));
      }
   }
   for (i = 0; i < 3; i ++) {
      cv[2][i] = (bestcolor[0][i] + bestcolor[1][i]) / 2;
      /* this isn't used. Looks like the black color constant can only be used
         with RGB_DXT1 if I read the spec correctly (note though that the radeon gpu disagrees,
         it will decode 3 to black even with DXT3/5), and due to how the color searching works
         it won't get used even then */
      cv[3][i] = 0;
   }
   testerror2 = 0;
   for (j = 0; j < numypixels; j++) {
      for (i = 0; i < numxpixels; i++) {
         pixerrorbest = 0xffffffff;
         if ((type == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) && (srccolors[j][i][3] <= ALPHACUT)) {
            enc = 3;
            pixerrorbest = 0; /* don't calculate error */
         }
         else {
            /* we're calculating the same what we have done already for colors 0-1 above... */
            for (colors = 0; colors < 3; colors++) {
               colordist = srccolors[j][i][0] - cv[colors][0];
               pixerror = colordist * colordist * REDWEIGHT;
               colordist = srccolors[j][i][1] - cv[colors][1];
               pixerror += colordist * colordist * GREENWEIGHT;
               colordist = srccolors[j][i][2] - cv[colors][2];
               pixerror += colordist * colordist * BLUEWEIGHT;
               if (pixerror < pixerrorbest) {
                  pixerrorbest = pixerror;
                  /* need to exchange colors later */
                  if (colors > 1) enc = colors;
                  else enc = colors ^ 1;
               }
            }
         }
         testerror2 += pixerrorbest;
         bits2 |= enc << (2 * (j * 4 + i));
      }
   }


   /* finally we're finished, write back colors and bits */
   if ((testerror > testerror2) || (haveAlpha)) {
      *blkaddr++ = color1 & 0xff;
      *blkaddr++ = color1 >> 8;
      *blkaddr++ = color0 & 0xff;
      *blkaddr++ = color0 >> 8;
      *blkaddr++ = bits2 & 0xff;
      *blkaddr++ = ( bits2 >> 8) & 0xff;
      *blkaddr++ = ( bits2 >> 16) & 0xff;
      *blkaddr = bits2 >> 24;
   }
   else {
      *blkaddr++ = color0 & 0xff;
      *blkaddr++ = color0 >> 8;
      *blkaddr++ = color1 & 0xff;
      *blkaddr++ = color1 >> 8;
      *blkaddr++ = bits & 0xff;
      *blkaddr++ = ( bits >> 8) & 0xff;
      *blkaddr++ = ( bits >> 16) & 0xff;
      *blkaddr = bits >> 24;
   }
}

static void encodedxtcolorblockfaster( GLubyte *blkaddr, GLubyte srccolors[4][4][4],
                         GLint numxpixels, GLint numypixels, GLuint type )
{
/* simplistic approach. We need two base colors, simply use the "highest" and the "lowest" color
   present in the picture as base colors */

   /* define lowest and highest color as shortest and longest vector to 0/0/0, though the
      vectors are weighted similar to their importance in rgb-luminance conversion
      doesn't work too well though...
      This seems to be a rather difficult problem */

   GLubyte *bestcolor[2];
   GLubyte basecolors[2][3];
   GLubyte i, j;
   GLuint lowcv, highcv, testcv;
   GLboolean haveAlpha = GL_FALSE;

   lowcv = highcv = srccolors[0][0][0] * srccolors[0][0][0] * REDWEIGHT +
                          srccolors[0][0][1] * srccolors[0][0][1] * GREENWEIGHT +
                          srccolors[0][0][2] * srccolors[0][0][2] * BLUEWEIGHT;
   bestcolor[0] = bestcolor[1] = srccolors[0][0];
   for (j = 0; j < numypixels; j++) {
      for (i = 0; i < numxpixels; i++) {
         /* don't use this as a base color if the pixel will get black/transparent anyway */
         if ((type != GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) || (srccolors[j][i][3] <= ALPHACUT)) {
            testcv = srccolors[j][i][0] * srccolors[j][i][0] * REDWEIGHT +
                     srccolors[j][i][1] * srccolors[j][i][1] * GREENWEIGHT +
                     srccolors[j][i][2] * srccolors[j][i][2] * BLUEWEIGHT;
            if (testcv > highcv) {
               highcv = testcv;
               bestcolor[1] = srccolors[j][i];
            }
            else if (testcv < lowcv) {
               lowcv = testcv;
               bestcolor[0] = srccolors[j][i];
            }
         }
         else haveAlpha = GL_TRUE;
      }
   }
   /* make sure the original color values won't get touched... */
   for (j = 0; j < 2; j++) {
      for (i = 0; i < 3; i++) {
         basecolors[j][i] = bestcolor[j][i];
      }
   }
   bestcolor[0] = basecolors[0];
   bestcolor[1] = basecolors[1];

   /* try to find better base colors */
   fancybasecolorsearch(blkaddr, srccolors, bestcolor, numxpixels, numypixels, type, haveAlpha);
   /* find the best encoding for these colors, and store the result */
   storedxtencodedblock(blkaddr, srccolors, bestcolor, numxpixels, numypixels, type, haveAlpha);
}

static void writedxt5encodedalphablock( GLubyte *blkaddr, GLubyte alphabase1, GLubyte alphabase2,
                         GLubyte alphaenc[16])
{
   *blkaddr++ = alphabase1;
   *blkaddr++ = alphabase2;
   *blkaddr++ = alphaenc[0] | (alphaenc[1] << 3) | ((alphaenc[2] & 3) << 6);
   *blkaddr++ = (alphaenc[2] >> 2) | (alphaenc[3] << 1) | (alphaenc[4] << 4) | ((alphaenc[5] & 1) << 7);
   *blkaddr++ = (alphaenc[5] >> 1) | (alphaenc[6] << 2) | (alphaenc[7] << 5);
   *blkaddr++ = alphaenc[8] | (alphaenc[9] << 3) | ((alphaenc[10] & 3) << 6);
   *blkaddr++ = (alphaenc[10] >> 2) | (alphaenc[11] << 1) | (alphaenc[12] << 4) | ((alphaenc[13] & 1) << 7);
   *blkaddr++ = (alphaenc[13] >> 1) | (alphaenc[14] << 2) | (alphaenc[15] << 5);
}

static void encodedxt5alpha(GLubyte *blkaddr, GLubyte srccolors[4][4][4],
                            GLint numxpixels, GLint numypixels)
{
   GLubyte alphabase[2], alphause[2];
   GLshort alphatest[2];
   GLuint alphablockerror1, alphablockerror2, alphablockerror3;
   GLubyte i, j, aindex, acutValues[7];
   GLubyte alphaenc1[16], alphaenc2[16], alphaenc3[16];
   GLboolean alphaabsmin = GL_FALSE;
   GLboolean alphaabsmax = GL_FALSE;
   GLshort alphadist;

   /* find lowest and highest alpha value in block, alphabase[0] lowest, alphabase[1] highest */
   alphabase[0] = 0xff; alphabase[1] = 0x0;
   for (j = 0; j < numypixels; j++) {
      for (i = 0; i < numxpixels; i++) {
         if (srccolors[j][i][3] == 0)
            alphaabsmin = GL_TRUE;
         else if (srccolors[j][i][3] == 255)
            alphaabsmax = GL_TRUE;
         else {
            if (srccolors[j][i][3] > alphabase[1])
               alphabase[1] = srccolors[j][i][3];
            if (srccolors[j][i][3] < alphabase[0])
               alphabase[0] = srccolors[j][i][3];
         }
      }
   }


   if ((alphabase[0] > alphabase[1]) && !(alphaabsmin && alphaabsmax)) { /* one color, either max or min */
      /* shortcut here since it is a very common case (and also avoids later problems) */
      /* || (alphabase[0] == alphabase[1] && !alphaabsmin && !alphaabsmax) */
      /* could also thest for alpha0 == alpha1 (and not min/max), but probably not common, so don't bother */

      *blkaddr++ = srccolors[0][0][3];
      blkaddr++;
      *blkaddr++ = 0;
      *blkaddr++ = 0;
      *blkaddr++ = 0;
      *blkaddr++ = 0;
      *blkaddr++ = 0;
      *blkaddr++ = 0;
      return;
   }

   /* find best encoding for alpha0 > alpha1 */
   /* it's possible this encoding is better even if both alphaabsmin and alphaabsmax are true */
   alphablockerror1 = 0x0;
   alphablockerror2 = 0xffffffff;
   alphablockerror3 = 0xffffffff;
   if (alphaabsmin) alphause[0] = 0;
   else alphause[0] = alphabase[0];
   if (alphaabsmax) alphause[1] = 255;
   else alphause[1] = alphabase[1];
   /* calculate the 7 cut values, just the middle between 2 of the computed alpha values */
   for (aindex = 0; aindex < 7; aindex++) {
      /* don't forget here is always rounded down */
      acutValues[aindex] = (alphause[0] * (2*aindex + 1) + alphause[1] * (14 - (2*aindex + 1))) / 14;
   }

   for (j = 0; j < numypixels; j++) {
      for (i = 0; i < numxpixels; i++) {
         /* maybe it's overkill to have the most complicated calculation just for the error
            calculation which we only need to figure out if encoding1 or encoding2 is better... */
         if (srccolors[j][i][3] > acutValues[0]) {
            alphaenc1[4*j + i] = 0;
            alphadist = srccolors[j][i][3] - alphause[1];
         }
         else if (srccolors[j][i][3] > acutValues[1]) {
            alphaenc1[4*j + i] = 2;
            alphadist = srccolors[j][i][3] - (alphause[1] * 6 + alphause[0] * 1) / 7;
         }
         else if (srccolors[j][i][3] > acutValues[2]) {
            alphaenc1[4*j + i] = 3;
            alphadist = srccolors[j][i][3] - (alphause[1] * 5 + alphause[0] * 2) / 7;
         }
         else if (srccolors[j][i][3] > acutValues[3]) {
            alphaenc1[4*j + i] = 4;
            alphadist = srccolors[j][i][3] - (alphause[1] * 4 + alphause[0] * 3) / 7;
         }
         else if (srccolors[j][i][3] > acutValues[4]) {
            alphaenc1[4*j + i] = 5;
            alphadist = srccolors[j][i][3] - (alphause[1] * 3 + alphause[0] * 4) / 7;
         }
         else if (srccolors[j][i][3] > acutValues[5]) {
            alphaenc1[4*j + i] = 6;
            alphadist = srccolors[j][i][3] - (alphause[1] * 2 + alphause[0] * 5) / 7;
         }
         else if (srccolors[j][i][3] > acutValues[6]) {
            alphaenc1[4*j + i] = 7;
            alphadist = srccolors[j][i][3] - (alphause[1] * 1 + alphause[0] * 6) / 7;
         }
         else {
            alphaenc1[4*j + i] = 1;
            alphadist = srccolors[j][i][3] - alphause[0];
         }
         alphablockerror1 += alphadist * alphadist;
      }
   }
   /* it's not very likely this encoding is better if both alphaabsmin and alphaabsmax
      are false but try it anyway */
   if (alphablockerror1 >= 32) {

      /* don't bother if encoding is already very good, this condition should also imply
      we have valid alphabase colors which we absolutely need (alphabase[0] <= alphabase[1]) */
      alphablockerror2 = 0;
      for (aindex = 0; aindex < 5; aindex++) {
         /* don't forget here is always rounded down */
         acutValues[aindex] = (alphabase[0] * (10 - (2*aindex + 1)) + alphabase[1] * (2*aindex + 1)) / 10;
      }
      for (j = 0; j < numypixels; j++) {
         for (i = 0; i < numxpixels; i++) {
             /* maybe it's overkill to have the most complicated calculation just for the error
               calculation which we only need to figure out if encoding1 or encoding2 is better... */
            if (srccolors[j][i][3] == 0) {
               alphaenc2[4*j + i] = 6;
               alphadist = 0;
            }
            else if (srccolors[j][i][3] == 255) {
               alphaenc2[4*j + i] = 7;
               alphadist = 0;
            }
            else if (srccolors[j][i][3] <= acutValues[0]) {
               alphaenc2[4*j + i] = 0;
               alphadist = srccolors[j][i][3] - alphabase[0];
            }
            else if (srccolors[j][i][3] <= acutValues[1]) {
               alphaenc2[4*j + i] = 2;
               alphadist = srccolors[j][i][3] - (alphabase[0] * 4 + alphabase[1] * 1) / 5;
            }
            else if (srccolors[j][i][3] <= acutValues[2]) {
               alphaenc2[4*j + i] = 3;
               alphadist = srccolors[j][i][3] - (alphabase[0] * 3 + alphabase[1] * 2) / 5;
            }
            else if (srccolors[j][i][3] <= acutValues[3]) {
               alphaenc2[4*j + i] = 4;
               alphadist = srccolors[j][i][3] - (alphabase[0] * 2 + alphabase[1] * 3) / 5;
            }
            else if (srccolors[j][i][3] <= acutValues[4]) {
               alphaenc2[4*j + i] = 5;
               alphadist = srccolors[j][i][3] - (alphabase[0] * 1 + alphabase[1] * 4) / 5;
            }
            else {
               alphaenc2[4*j + i] = 1;
               alphadist = srccolors[j][i][3] - alphabase[1];
            }
            alphablockerror2 += alphadist * alphadist;
         }
      }


      /* skip this if the error is already very small
         this encoding is MUCH better on average than #2 though, but expensive! */
      if ((alphablockerror2 > 96) && (alphablockerror1 > 96)) {
         GLshort blockerrlin1 = 0;
         GLshort blockerrlin2 = 0;
         GLubyte nralphainrangelow = 0;
         GLubyte nralphainrangehigh = 0;
         alphatest[0] = 0xff;
         alphatest[1] = 0x0;
         /* if we have large range it's likely there are values close to 0/255, try to map them to 0/255 */
         for (j = 0; j < numypixels; j++) {
            for (i = 0; i < numxpixels; i++) {
               if ((srccolors[j][i][3] > alphatest[1]) && (srccolors[j][i][3] < (255 -(alphabase[1] - alphabase[0]) / 28)))
                  alphatest[1] = srccolors[j][i][3];
               if ((srccolors[j][i][3] < alphatest[0]) && (srccolors[j][i][3] > (alphabase[1] - alphabase[0]) / 28))
                  alphatest[0] = srccolors[j][i][3];
            }
         }
          /* shouldn't happen too often, don't really care about those degenerated cases */
          if (alphatest[1] <= alphatest[0]) {
             alphatest[0] = 1;
             alphatest[1] = 254;
         }
         for (aindex = 0; aindex < 5; aindex++) {
         /* don't forget here is always rounded down */
            acutValues[aindex] = (alphatest[0] * (10 - (2*aindex + 1)) + alphatest[1] * (2*aindex + 1)) / 10;
         }

         /* find the "average" difference between the alpha values and the next encoded value.
            This is then used to calculate new base values.
            Should there be some weighting, i.e. those values closer to alphatest[x] have more weight,
            since they will see more improvement, and also because the values in the middle are somewhat
            likely to get no improvement at all (because the base values might move in different directions)?
            OTOH it would mean the values in the middle are even less likely to get an improvement
         */
         for (j = 0; j < numypixels; j++) {
            for (i = 0; i < numxpixels; i++) {
               if (srccolors[j][i][3] <= alphatest[0] / 2) {
               }
               else if (srccolors[j][i][3] > ((255 + alphatest[1]) / 2)) {
               }
               else if (srccolors[j][i][3] <= acutValues[0]) {
                  blockerrlin1 += (srccolors[j][i][3] - alphatest[0]);
                  nralphainrangelow += 1;
               }
               else if (srccolors[j][i][3] <= acutValues[1]) {
                  blockerrlin1 += (srccolors[j][i][3] - (alphatest[0] * 4 + alphatest[1] * 1) / 5);
                  blockerrlin2 += (srccolors[j][i][3] - (alphatest[0] * 4 + alphatest[1] * 1) / 5);
                  nralphainrangelow += 1;
                  nralphainrangehigh += 1;
               }
               else if (srccolors[j][i][3] <= acutValues[2]) {
                  blockerrlin1 += (srccolors[j][i][3] - (alphatest[0] * 3 + alphatest[1] * 2) / 5);
                  blockerrlin2 += (srccolors[j][i][3] - (alphatest[0] * 3 + alphatest[1] * 2) / 5);
                  nralphainrangelow += 1;
                  nralphainrangehigh += 1;
               }
               else if (srccolors[j][i][3] <= acutValues[3]) {
                  blockerrlin1 += (srccolors[j][i][3] - (alphatest[0] * 2 + alphatest[1] * 3) / 5);
                  blockerrlin2 += (srccolors[j][i][3] - (alphatest[0] * 2 + alphatest[1] * 3) / 5);
                  nralphainrangelow += 1;
                  nralphainrangehigh += 1;
               }
               else if (srccolors[j][i][3] <= acutValues[4]) {
                  blockerrlin1 += (srccolors[j][i][3] - (alphatest[0] * 1 + alphatest[1] * 4) / 5);
                  blockerrlin2 += (srccolors[j][i][3] - (alphatest[0] * 1 + alphatest[1] * 4) / 5);
                  nralphainrangelow += 1;
                  nralphainrangehigh += 1;
                  }
               else {
                  blockerrlin2 += (srccolors[j][i][3] - alphatest[1]);
                  nralphainrangehigh += 1;
               }
            }
         }
         /* shouldn't happen often, needed to avoid div by zero */
         if (nralphainrangelow == 0) nralphainrangelow = 1;
         if (nralphainrangehigh == 0) nralphainrangehigh = 1;
         alphatest[0] = alphatest[0] + (blockerrlin1 / nralphainrangelow);
//         std::cerr << boost::format("block err lin low %d, nr %d\n") % blockerrlin1 % nralphainrangelow;
//         std::cerr << boost::format("block err lin high %d, nr %d\n") % blockerrlin2 % nralphainrangehigh;
         /* again shouldn't really happen often... */
         if (alphatest[0] < 0) {
            alphatest[0] = 0;
//            std::cerr << "adj alpha base val to 0\n";
         }
         alphatest[1] = alphatest[1] + (blockerrlin2 / nralphainrangehigh);
         if (alphatest[1] > 255) {
            alphatest[1] = 255;
//            std::cerr << "adj alpha base val to 255\n";
         }

         alphablockerror3 = 0;
         for (aindex = 0; aindex < 5; aindex++) {
         /* don't forget here is always rounded down */
            acutValues[aindex] = (alphatest[0] * (10 - (2*aindex + 1)) + alphatest[1] * (2*aindex + 1)) / 10;
         }
         for (j = 0; j < numypixels; j++) {
            for (i = 0; i < numxpixels; i++) {
                /* maybe it's overkill to have the most complicated calculation just for the error
                  calculation which we only need to figure out if encoding1 or encoding2 is better... */
               if (srccolors[j][i][3] <= alphatest[0] / 2) {
                  alphaenc3[4*j + i] = 6;
                  alphadist = srccolors[j][i][3];
               }
               else if (srccolors[j][i][3] > ((255 + alphatest[1]) / 2)) {
                  alphaenc3[4*j + i] = 7;
                  alphadist = 255 - srccolors[j][i][3];
               }
               else if (srccolors[j][i][3] <= acutValues[0]) {
                  alphaenc3[4*j + i] = 0;
                  alphadist = srccolors[j][i][3] - alphatest[0];
               }
               else if (srccolors[j][i][3] <= acutValues[1]) {
                 alphaenc3[4*j + i] = 2;
                 alphadist = srccolors[j][i][3] - (alphatest[0] * 4 + alphatest[1] * 1) / 5;
               }
               else if (srccolors[j][i][3] <= acutValues[2]) {
                  alphaenc3[4*j + i] = 3;
                  alphadist = srccolors[j][i][3] - (alphatest[0] * 3 + alphatest[1] * 2) / 5;
               }
               else if (srccolors[j][i][3] <= acutValues[3]) {
                  alphaenc3[4*j + i] = 4;
                  alphadist = srccolors[j][i][3] - (alphatest[0] * 2 + alphatest[1] * 3) / 5;
               }
               else if (srccolors[j][i][3] <= acutValues[4]) {
                  alphaenc3[4*j + i] = 5;
                  alphadist = srccolors[j][i][3] - (alphatest[0] * 1 + alphatest[1] * 4) / 5;
               }
               else {
                  alphaenc3[4*j + i] = 1;
                  alphadist = srccolors[j][i][3] - alphatest[1];
               }
               alphablockerror3 += alphadist * alphadist;
            }
         }
      }
   }
  /* write the alpha values and encoding back. */
   if ((alphablockerror1 <= alphablockerror2) && (alphablockerror1 <= alphablockerror3)) {
//      if (alphablockerror1 > 96) {
//			std::cerr << "enc1 used, error " << static_cast<int>(alphablockerror1) << std::endl;
//		}
      writedxt5encodedalphablock( blkaddr, alphause[1], alphause[0], alphaenc1 );
   }
   else if (alphablockerror2 <= alphablockerror3) {
//      if (alphablockerror2 > 96) {
//			std::cerr << "enc2 used, error " << static_cast<int>(alphablockerror2) << std::endl;
//		}
      writedxt5encodedalphablock( blkaddr, alphabase[0], alphabase[1], alphaenc2 );
   } else {
      std::cerr << "enc3 used, error " << static_cast<int>(alphablockerror3) << std::endl;
      writedxt5encodedalphablock( blkaddr, (GLubyte)alphatest[0], (GLubyte)alphatest[1], alphaenc3 );
   }
}

static void extractsrccolors( GLubyte srcpixels[4][4][4], const GLchan *srcaddr,
                         GLint srcRowStride, GLint numxpixels, GLint numypixels, GLint comps)
{
   GLubyte i, j, c;
   const GLchan *curaddr;
   for (j = 0; j < numypixels; j++) {
      curaddr = srcaddr + j * srcRowStride * comps;
      for (i = 0; i < numxpixels; i++) {
         for (c = 0; c < comps; c++) {
            srcpixels[j][i][c] = *curaddr++ / (CHAN_MAX / 255);
         }
      }
   }
}


void tx_compress_dxtn(GLint srccomps, GLint width, GLint height, const GLubyte *srcPixData,
                     GLenum destFormat, GLubyte *dest, GLint dstRowStride)
{
      GLubyte *blkaddr = dest;
      GLubyte srcpixels[4][4][4];
      const GLchan *srcaddr = srcPixData;
      GLint numxpixels, numypixels;
      GLint i, j;
      GLint dstRowDiff;

   switch (destFormat) {
   case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
   case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
      /* hmm we used to get called without dstRowStride... */
      dstRowDiff = dstRowStride >= (width * 2) ? dstRowStride - (((width + 3) & ~3) * 2) : 0;
      for (j = 0; j < height; j += 4) {
         if (height > j + 3) numypixels = 4;
         else numypixels = height - j;
         srcaddr = srcPixData + j * width * srccomps;
         for (i = 0; i < width; i += 4) {
            if (width > i + 3) numxpixels = 4;
            else numxpixels = width - i;
            extractsrccolors(srcpixels, srcaddr, width, numxpixels, numypixels, srccomps);
            encodedxtcolorblockfaster(blkaddr, srcpixels, numxpixels, numypixels, destFormat);
            srcaddr += srccomps * numxpixels;
            blkaddr += 8;
         }
         blkaddr += dstRowDiff;
      }
      break;
   case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
      dstRowDiff = dstRowStride >= (width * 4) ? dstRowStride - (((width + 3) & ~3) * 4) : 0;
      for (j = 0; j < height; j += 4) {
         if (height > j + 3) numypixels = 4;
         else numypixels = height - j;
         srcaddr = srcPixData + j * width * srccomps;
         for (i = 0; i < width; i += 4) {
            if (width > i + 3) numxpixels = 4;
            else numxpixels = width - i;
            extractsrccolors(srcpixels, srcaddr, width, numxpixels, numypixels, srccomps);
            *blkaddr++ = (srcpixels[0][0][3] >> 4) | (srcpixels[0][1][3] & 0xf0);
            *blkaddr++ = (srcpixels[0][2][3] >> 4) | (srcpixels[0][3][3] & 0xf0);
            *blkaddr++ = (srcpixels[1][0][3] >> 4) | (srcpixels[1][1][3] & 0xf0);
            *blkaddr++ = (srcpixels[1][2][3] >> 4) | (srcpixels[1][3][3] & 0xf0);
            *blkaddr++ = (srcpixels[2][0][3] >> 4) | (srcpixels[2][1][3] & 0xf0);
            *blkaddr++ = (srcpixels[2][2][3] >> 4) | (srcpixels[2][3][3] & 0xf0);
            *blkaddr++ = (srcpixels[3][0][3] >> 4) | (srcpixels[3][1][3] & 0xf0);
            *blkaddr++ = (srcpixels[3][2][3] >> 4) | (srcpixels[3][3][3] & 0xf0);
            encodedxtcolorblockfaster(blkaddr, srcpixels, numxpixels, numypixels, destFormat);
            srcaddr += srccomps * numxpixels;
            blkaddr += 8;
         }
         blkaddr += dstRowDiff;
      }
      break;
   case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
      dstRowDiff = dstRowStride >= (width * 4) ? dstRowStride - (((width + 3) & ~3) * 4) : 0;
      for (j = 0; j < height; j += 4) {
         if (height > j + 3) numypixels = 4;
         else numypixels = height - j;
         srcaddr = srcPixData + j * width * srccomps;
         for (i = 0; i < width; i += 4) {
            if (width > i + 3) numxpixels = 4;
            else numxpixels = width - i;
            extractsrccolors(srcpixels, srcaddr, width, numxpixels, numypixels, srccomps);
            encodedxt5alpha(blkaddr, srcpixels, numxpixels, numypixels);
            encodedxtcolorblockfaster(blkaddr + 8, srcpixels, numxpixels, numypixels, destFormat);
            srcaddr += srccomps * numxpixels;
            blkaddr += 16;
         }
         blkaddr += dstRowDiff;
      }
      break;
   default:
      return;
   }
}
#endif

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


	//-----------------------------------------------------------------//
	/*!
		@brief	DDS ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool dds_io::probe(utils::file_io& fin)
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
	bool dds_io::info(utils::file_io& fin, img::img_info& fo)
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
		@brief	DDS ファイルをロードする
		@param[in]	fin	ファイル I/O クラス
		@param[in]	ext	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool dds_io::load(utils::file_io& fin, const std::string& ext)
	{
		if(!probe(fin)) {
			return false;
		}

		int multi_level = 0;
		int mipmap_level = 0;
		if(!ext.empty()) {
			utils::strings ss;
			utils::split_text(ext, ",", ss);
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
			if(!fin.seek(skip, utils::file_io::seek::cur)) {
				return false;
			}
		}

//		boost::format("%d, %d\n") % width % height;

		switch(form) {
		case form_RGBA:
			{
				unsigned char* buff = new unsigned char[width * 4];
				img_.create(vtx::spos(width, height), true);
				vtx::spos pos;
				for(pos.y = 0; pos.y < height; ++pos.y) {
					fin.read(buff, 4, width);
					for(pos.x = 0; pos.x < width; ++pos.x) {
						int i = pos.x * 4;
						img::rgba8 c;
						c.set(buff[i + 2], buff[i + 1], buff[i + 0], 255);
						img_.put_pixel(pos, c);
					}
				}
				delete[] buff;
			}
			break;
		case form_DXT1:
			{
				img_.create(vtx::spos(width, height), true);
				unsigned char* buff = new unsigned char[width * height];
				fin.read(buff, 1, width * height);
				vtx::spos pos;
				for(pos.y = 0; pos.y < height; ++pos.y) {
					for(pos.x = 0; pos.x < width; ++pos.x) {
						img::rgba8 c;
						fetch_2d_texel_rgba_dxt1(width / 2, buff, pos.x, pos.y, c);
						// fetch_2d_texel_rgba_dxt1(DDSint srcRowStride, const DDSubyte *pixdata, DDSint i, DDSint j, DDSvoid *texel);
						img_.put_pixel(pos, c);
					}
				}
				delete[] buff;
			}
			break;
		case form_DXT3:
			{
				img_.create(vtx::spos(width, height), true);
				unsigned char* buff = new unsigned char[width * height];
				fin.read(buff, 1, width * height);
				vtx::spos pos;
				for(pos.y = 0; pos.y < height; ++pos.y) {
					for(pos.x = 0; pos.x < width; ++pos.x) {
						img::rgba8 c;
						fetch_2d_texel_rgba_dxt3(width, (DDSubyte*)buff, pos.x, pos.y, c);
						img_.put_pixel(pos, c);
					}
				}
				delete[] buff;
			}
			break;
		case form_DXT5:
			{
				img_.create(vtx::spos(width, height), true);
				unsigned char* buff = new unsigned char[width * height];
				fin.read(buff, 1, width * height);
				vtx::spos pos;
				for(pos.y = 0; pos.y < height; ++pos.y) {
					for(pos.x = 0; pos.x < width; ++pos.x) {
						img::rgba8 c;
						fetch_2d_texel_rgba_dxt5(width, (DDSubyte*)buff, pos.x, pos.y, c);
						img_.put_pixel(pos, c);
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
	bool dds_io::save(utils::file_io& fout, const std::string& ext)
	{
		int w = imf_->get_size().x;
		int h = imf_->get_size().y;
		if(w <= 0 || h <= 0) {
			return false;
		}



		return false;
	}

}
