//=====================================================================//
/*!	@file
	@brief	PVR 画像を扱うクラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include "img_io/pvr_io.hpp"
#include "img_io/img_rgba8.hpp"

// #include <iostream>
// #include <boost/format.hpp>

namespace img {

	struct pvr_info_v2 {
		static const uint32_t PVRTEX_MIPMAP		 = (1<<8);		///< Has mip map levels. DEPRECATED.
		static const uint32_t PVRTEX_TWIDDLE	 = (1<<9);		///< Is twiddled. DEPRECATED.
		static const uint32_t PVRTEX_BUMPMAP	 = (1<<10);		///< Has normals encoded for a bump map. DEPRECATED.
		static const uint32_t PVRTEX_TILING		 = (1<<11);		///< Is bordered for tiled pvr. DEPRECATED.
		static const uint32_t PVRTEX_CUBEMAP	 = (1<<12);		///< Is a cubemap/skybox. DEPRECATED.
		static const uint32_t PVRTEX_FALSEMIPCOL = (1<<13);		///< Are there false coloured MIP levels. DEPRECATED.
		static const uint32_t PVRTEX_VOLUME		 = (1<<14);		///< Is this a volume texture. DEPRECATED.
		static const uint32_t PVRTEX_ALPHA		 = (1<<15);		///< v2.1. Is there transparency info in the texture. DEPRECATED.
		static const uint32_t PVRTEX_VERTICAL_FLIP = (1<<16);	///< v2.1. Is the texture vertically flipped. DEPRECATED.
		static const uint32_t PVRTEX_PIXELTYPE	= 0xff;		///< Pixel type is always in the last 16bits of the flags. DEPRECATED.

		uint32_t	header_size;	///< size of the structure
		uint32_t	height;			///< height of surface to be created
		uint32_t	width;			///< width of input surface
		uint32_t	mipmap_count;	///< number of mip-map levels requested
		uint32_t	flags;			///< pixel format flags
		uint32_t	data_size;		///< Total size in bytes
		uint32_t	bit_count;		///< number of bits per pixel
		uint32_t	r_bit_mask;		///< mask for red bit
		uint32_t	g_bit_mask;		///< mask for green bits
		uint32_t	b_bit_mask;		///< mask for blue bits
		uint32_t	a_bit_mask;		///< mask for alpha channel
		uint8_t		iden[4];		///< magic number identifying pvr file "PVR!"
		uint32_t	num_surface;	///< the number of surfaces present in the pvr

		bool get(utils::file_io& fin) {
			fin.get32(header_size);
			fin.get32(height);
			fin.get32(width);
			fin.get32(mipmap_count);
			fin.get32(flags);
			fin.get32(data_size);
			fin.get32(bit_count);
			fin.get32(r_bit_mask);
			fin.get32(g_bit_mask);
			fin.get32(b_bit_mask);
			fin.get32(a_bit_mask);
			fin.read(iden, 4);
			fin.get32(num_surface);
			if(fin.eof()) return false;
			if(header_size == 0x34 && iden[0] == 'P' && iden[1] == 'V' && iden[2] == 'R' && iden[3] == '!') {
				return true;
			} else {
				return false;
			}
		}


		bool probe(utils::file_io& fin) {
			long pos = fin.tell();
			bool f = get(fin);
			fin.seek(pos, utils::file_io::SEEK::SET);
			return f;
		}
	};

	struct pvr_info_v3 {
		uint8_t		version[4];		///< "PVR",3
		uint32_t	flags;			///< Format flags.
		uint32_t	format[2];		///< The pixel format.
		uint32_t	color_space;	///< The Colour Space of the texture.
		uint32_t	chanel_type;	///< Variable type that the channel is stored in.
		uint32_t	height;			///< Height of the texture.
		uint32_t	width;			///< Width of the texture.
		uint32_t	depth;			///< Depth of the texture. (Z-slices)
		uint32_t	num_surfaces;	///< Number of members in a Texture Array.
		uint32_t	num_faces;		///< Number of faces in a Cube Map. Maybe be a value other than 6.
		uint32_t	mipmap_count;	///< Number of MIP Maps in the texture - NB: Includes top level.
		uint32_t	meta_data_size;	///< Size of the accompanying meta data.

		bool get(utils::file_io& fin) {
			fin.read(version, 4);
			fin.get32(flags);
			fin.get32(format[0]);
			fin.get32(format[1]);
			fin.get32(color_space);
			fin.get32(chanel_type);
			fin.get32(height);
			fin.get32(width);
			fin.get32(depth);
			fin.get32(num_surfaces);
			fin.get32(num_faces);
			fin.get32(mipmap_count);
			fin.get32(meta_data_size);
			return !fin.eof();
		}
	};


	struct Pixel32 {
		uint8_t red,green,blue,alpha;
	};


	struct Pixel128S {
		int32_t red,green,blue,alpha;
	};


	struct PVRTCWordIndices {
		int P[2], Q[2], R[2], S[2];
	};


	struct PVRTCWord {
		uint32_t u32ModulationData;
		uint32_t u32ColourData;
	};

	
	static uint32_t wrapWordIndex(uint32_t numWords, int word)
	{
		return ((word + numWords) % numWords);
	}

	
	static uint32_t TwiddleUV(uint32_t XSize, uint32_t YSize, uint32_t XPos, uint32_t YPos)
	{
		//Initially assume X is the larger size.
		uint32_t MinDimension = XSize;
		uint32_t MaxValue = YPos;
		uint32_t Twiddled = 0;
		uint32_t SrcBitPos = 1;
		uint32_t DstBitPos = 1;
		int ShiftCount = 0;

		//Check the sizes are valid.
		// _ASSERT(YPos < YSize);
		// _ASSERT(XPos < XSize);
		// _ASSERT(isPowerOf2(YSize));
		// _ASSERT(isPowerOf2(XSize));

		//If Y is the larger dimension - switch the min/max values.
		if(YSize < XSize) {
			MinDimension = YSize;
			MaxValue     = XPos;
		}

		// Step through all the bits in the "minimum" dimension
		while(SrcBitPos < MinDimension) {
			if(YPos & SrcBitPos) {
				Twiddled |= DstBitPos;
			}

			if(XPos & SrcBitPos) {
				Twiddled |= (DstBitPos << 1);
			}

			SrcBitPos <<= 1;
			DstBitPos <<= 2;
			ShiftCount += 1;
		}

		// Prepend any unused bits
		MaxValue >>= ShiftCount;
		Twiddled |=  (MaxValue << (2 * ShiftCount));

		return Twiddled;
	}


	static void unpackModulations(const PVRTCWord& word, int offsetX, int offsetY, int32_t i32ModulationValues[16][8], int32_t i32ModulationModes[16][8], uint8_t ui8Bpp)
	{
		uint32_t WordModMode = word.u32ColourData & 0x1;
		uint32_t ModulationBits = word.u32ModulationData;

		// Unpack differently depending on 2bpp or 4bpp modes.
		if(ui8Bpp==2) {
			if(WordModMode) {
				// determine which of the three modes are in use:

				// If this is the either the H-only or V-only interpolation mode...
				if(ModulationBits & 0x1) {
					// look at the "LSB" for the "centre" (V=2,H=4) texel. Its LSB is now
					// actually used to indicate whether it's the H-only mode or the V-only...

					// The centre texel data is the at (y==2, x==4) and so its LSB is at bit 20.
					if(ModulationBits & (0x1 << 20)) {
						// This is the V-only mode
						WordModMode = 3;
					} else {
						// This is the H-only mode
						WordModMode = 2;
					}

					// Create an extra bit for the centre pixel so that it looks like
					// we have 2 actual bits for this texel. It makes later coding much easier.
					if(ModulationBits & (0x1 << 21)) {
						// set it to produce code for 1.0
						ModulationBits |= (0x1 << 20); 
					} else {
						// clear it to produce 0.0 code
						ModulationBits &= ~(0x1 << 20);
					}
				}  // end if H-Only or V-Only interpolation mode was chosen

				if(ModulationBits & 0x2) {
					ModulationBits |= 0x1; /*set it*/
				} else {
					ModulationBits &= ~0x1; /*clear it*/
				}

				// run through all the pixels in the block. Note we can now treat all the
				// "stored" values as if they have 2bits (even when they didn't!)
				for(int y = 0; y < 4; y++) {
					for(int x = 0; x < 8; x++) {
						i32ModulationModes[x+offsetX][y+offsetY] = WordModMode;				

						// if this is a stored value...
						if(((x^y)&1) == 0) {
							i32ModulationValues[x+offsetX][y+offsetY] = ModulationBits & 3;						
							ModulationBits >>= 2;
						}
					}
				} // end for y
			} else {  // else if direct encoded 2bit mode - i.e. 1 mode bit per pixel
				for(int y = 0; y < 4; y++) {
					for(int x = 0; x < 8; x++) {
						i32ModulationModes[x+offsetX][y+offsetY] = WordModMode;					

						/*
						// double the bits so 0=> 00, and 1=>11
						*/
						if(ModulationBits & 1) {
							i32ModulationValues[x+offsetX][y+offsetY] = 0x3;						
						} else {
							i32ModulationValues[x+offsetX][y+offsetY] = 0x0;					
						}
						ModulationBits >>= 1;
					}
				}  // end for y
			}
		} else {
			//Much simpler than the 2bpp decompression, only two modes, so the n/8 values are set directly.
			// run through all the pixels in the word.
			if(WordModMode) {
				for(int y = 0; y < 4; y++) {
					for(int x = 0; x < 4; x++) {
						i32ModulationValues[y+offsetY][x+offsetX] = ModulationBits & 3;
						//if (i32ModulationValues==0) {}; don't need to check 0, 0 = 0/8.
						if (i32ModulationValues[y+offsetY][x+offsetX]==1) { i32ModulationValues[y+offsetY][x+offsetX]=4;}
						else if (i32ModulationValues[y+offsetY][x+offsetX]==2) { i32ModulationValues[y+offsetY][x+offsetX]=14;} //+10 tells the decompressor to punch through alpha.
						else if (i32ModulationValues[y+offsetY][x+offsetX]==3) { i32ModulationValues[y+offsetY][x+offsetX]=8;}
						ModulationBits >>= 2;
					} // end for x
				} // end for y
			} else {
				for(int y = 0; y < 4; y++) {
					for(int x = 0; x < 4; x++) {
						i32ModulationValues[y+offsetY][x+offsetX] = ModulationBits & 3;
						i32ModulationValues[y+offsetY][x+offsetX]*=3;
						if(i32ModulationValues[y+offsetY][x+offsetX]>3) i32ModulationValues[y+offsetY][x+offsetX]-=1;
						ModulationBits >>= 2;
					} // end for x
				} // end for y
			}
		}
	}


	static Pixel32 getColourA(uint32_t u32ColourData)
	{
		Pixel32 colour;

		// Opaque Colour Mode - RGB 554
		if((u32ColourData & 0x8000) != 0) {
			colour.red   = (uint8_t)((u32ColourData & 0x7c00) >> 10); // 5->5 bits
			colour.green = (uint8_t)((u32ColourData & 0x3e0)  >> 5); // 5->5 bits
			colour.blue  = (uint8_t)(u32ColourData  & 0x1e) | ((u32ColourData & 0x1e) >> 4); // 4->5 bits
			colour.alpha = (uint8_t)0xf;// 0->4 bits
		} else {  // Transparent Colour Mode - ARGB 3443
			colour.red   = (uint8_t)((u32ColourData & 0xf00)  >> 7) | ((u32ColourData & 0xf00) >> 11); // 4->5 bits
			colour.green = (uint8_t)((u32ColourData & 0xf0)   >> 3) | ((u32ColourData & 0xf0)  >> 7); // 4->5 bits
			colour.blue  = (uint8_t)((u32ColourData & 0xe)    << 1) | ((u32ColourData & 0xe)   >> 2); // 3->5 bits
			colour.alpha = (uint8_t)((u32ColourData & 0x7000) >> 11);// 3->4 bits - note 0 at right
		}
		return colour;
	}


	static Pixel32 getColourB(uint32_t u32ColourData)
	{
		Pixel32 colour;

		// Opaque Colour Mode - RGB 555
		if(u32ColourData & 0x80000000) {
			colour.red   = (uint8_t)((u32ColourData & 0x7c000000) >> 26); // 5->5 bits 
			colour.green = (uint8_t)((u32ColourData & 0x3e00000)  >> 21); // 5->5 bits
			colour.blue  = (uint8_t)((u32ColourData & 0x1f0000)   >> 16); // 5->5 bits
			colour.alpha = (uint8_t)0xf;// 0 bits
		} else {  // Transparent Colour Mode - ARGB 3444
			colour.red   = (uint8_t)(((u32ColourData & 0xf000000)  >> 23) | ((u32ColourData & 0xf000000) >> 27)); // 4->5 bits
			colour.green = (uint8_t)(((u32ColourData & 0xf00000)   >> 19) | ((u32ColourData & 0xf00000)  >> 23)); // 4->5 bits
			colour.blue  = (uint8_t)(((u32ColourData & 0xf0000)    >> 15) | ((u32ColourData & 0xf0000)   >> 19)); // 4->5 bits
			colour.alpha = (uint8_t)((u32ColourData & 0x70000000) >> 27);// 3->4 bits - note 0 at right
		}
		return colour;
	}


	static void interpolateColours(Pixel32 P, Pixel32 Q, Pixel32 R, Pixel32 S,
								   Pixel128S *pPixel, uint8_t ui8Bpp)
	{
		uint32_t ui32WordWidth = 4;
		uint32_t ui32WordHeight = 4;
		if(ui8Bpp == 2) {
			ui32WordWidth = 8;
		}

		//Convert to int 32.
		Pixel128S hP = {(int32_t)P.red,(int32_t)P.green,(int32_t)P.blue,(int32_t)P.alpha};
		Pixel128S hQ = {(int32_t)Q.red,(int32_t)Q.green,(int32_t)Q.blue,(int32_t)Q.alpha};
		Pixel128S hR = {(int32_t)R.red,(int32_t)R.green,(int32_t)R.blue,(int32_t)R.alpha};
		Pixel128S hS = {(int32_t)S.red,(int32_t)S.green,(int32_t)S.blue,(int32_t)S.alpha};

		//Get vectors.
		Pixel128S QminusP = {hQ.red - hP.red, hQ.green - hP.green, hQ.blue - hP.blue, hQ.alpha - hP.alpha};	
		Pixel128S SminusR = {hS.red - hR.red, hS.green - hR.green, hS.blue - hR.blue, hS.alpha - hR.alpha};	

		//Multiply colours.
		hP.red		*=	ui32WordWidth;
		hP.green	*=	ui32WordWidth;
		hP.blue		*=	ui32WordWidth;
		hP.alpha	*=	ui32WordWidth;
		hR.red		*=	ui32WordWidth;
		hR.green	*=	ui32WordWidth;
		hR.blue		*=	ui32WordWidth;
		hR.alpha	*=	ui32WordWidth;
	
		if(ui8Bpp == 2) {
			//Loop through pixels to achieve results.
			for(unsigned int x=0; x < ui32WordWidth; x++) {			
				Pixel128S Result={4*hP.red, 4*hP.green, 4*hP.blue, 4*hP.alpha};
				Pixel128S dY = {hR.red - hP.red, hR.green - hP.green, hR.blue - hP.blue, hR.alpha - hP.alpha};	

				for(unsigned int y=0; y < ui32WordHeight; y++) {
					pPixel[y*ui32WordWidth+x].red   = (int32_t)((Result.red   >> 7) + (Result.red   >> 2));
					pPixel[y*ui32WordWidth+x].green = (int32_t)((Result.green >> 7) + (Result.green >> 2));
					pPixel[y*ui32WordWidth+x].blue  = (int32_t)((Result.blue  >> 7) + (Result.blue  >> 2));
					pPixel[y*ui32WordWidth+x].alpha = (int32_t)((Result.alpha >> 5) + (Result.alpha >> 1));				

					Result.red		+= dY.red;
					Result.green	+= dY.green;
					Result.blue		+= dY.blue;
					Result.alpha	+= dY.alpha;
				}

				hP.red		+= QminusP.red;
				hP.green	+= QminusP.green;
				hP.blue		+= QminusP.blue;
				hP.alpha	+= QminusP.alpha;

				hR.red		+= SminusR.red;
				hR.green	+= SminusR.green;
				hR.blue		+= SminusR.blue;
				hR.alpha	+= SminusR.alpha;
			}
		} else {
			//Loop through pixels to achieve results.
			for(unsigned int y=0; y < ui32WordHeight; y++) {
				Pixel128S Result={4*hP.red, 4*hP.green, 4*hP.blue, 4*hP.alpha};
				Pixel128S dY = {hR.red - hP.red, hR.green - hP.green, hR.blue - hP.blue, hR.alpha - hP.alpha};	

				for(unsigned int x=0; x < ui32WordWidth; x++) {
					pPixel[y*ui32WordWidth+x].red   = (int32_t)((Result.red   >> 6) + (Result.red   >> 1));
					pPixel[y*ui32WordWidth+x].green = (int32_t)((Result.green >> 6) + (Result.green >> 1));
					pPixel[y*ui32WordWidth+x].blue  = (int32_t)((Result.blue  >> 6) + (Result.blue  >> 1));
					pPixel[y*ui32WordWidth+x].alpha = (int32_t)((Result.alpha >> 4) + (Result.alpha));				

					Result.red += dY.red;
					Result.green += dY.green;
					Result.blue += dY.blue;
					Result.alpha += dY.alpha;
				}

				hP.red += QminusP.red;
				hP.green += QminusP.green;
				hP.blue += QminusP.blue;
				hP.alpha += QminusP.alpha;

				hR.red += SminusR.red;
				hR.green += SminusR.green;
				hR.blue += SminusR.blue;
				hR.alpha += SminusR.alpha;
			}
		}
	}


	static int32_t getModulationValues(int32_t i32ModulationValues[16][8], int32_t i32ModulationModes[16][8],
									   uint32_t xPos, uint32_t yPos, uint8_t ui8Bpp)
	{
		if(ui8Bpp == 2) {
			const int RepVals0[4] = {0, 3, 5, 8};

			// extract the modulation value. If a simple encoding
			if(i32ModulationModes[xPos][yPos]==0) {
				return RepVals0[i32ModulationValues[xPos][yPos]];
			} else {
				// if this is a stored value
				if(((xPos^yPos)&1)==0) {
					return RepVals0[i32ModulationValues[xPos][yPos]];
				} else if(i32ModulationModes[xPos][yPos] == 1) {  // else average from the neighbours
					// if H&V interpolation...
					return (RepVals0[i32ModulationValues[xPos][yPos-1]] +
							RepVals0[i32ModulationValues[xPos][yPos+1]] +
							RepVals0[i32ModulationValues[xPos-1][yPos]] +
							RepVals0[i32ModulationValues[xPos+1][yPos]] + 2) / 4;
				} else if(i32ModulationModes[xPos][yPos] == 2) {  // else if H-Only
				return (RepVals0[i32ModulationValues[xPos-1][yPos]] + 
						RepVals0[i32ModulationValues[xPos+1][yPos]] + 1) / 2;
				} else {  // else it's V-Only
					return (RepVals0[i32ModulationValues[xPos][yPos-1]] + 
							RepVals0[i32ModulationValues[xPos][yPos+1]] + 1) / 2;
				}
			}
		} else if(ui8Bpp == 4) {
			return i32ModulationValues[xPos][yPos];
		}
		return 0;
	}


	static void pvrtcGetDecompressedPixels(const PVRTCWord& P, const PVRTCWord& Q, const PVRTCWord& R, const PVRTCWord& S,
								Pixel32* pColourData, uint8_t ui8Bpp)
	{
		//4bpp only needs 8*8 values, but 2bpp needs 16*8, so rather than wasting processor time we just statically allocate 16*8.
		int32_t i32ModulationValues[16][8];
		//Only 2bpp needs this.
		int32_t i32ModulationModes[16][8];
		//4bpp only needs 16 values, but 2bpp needs 32, so rather than wasting processor time we just statically allocate 32.
		Pixel128S upscaledColourA[32];
		Pixel128S upscaledColourB[32];

		uint32_t ui32WordWidth = 4;
		uint32_t ui32WordHeight = 4;
		if(ui8Bpp == 2) {
			ui32WordWidth = 8;
		}

		//Get the modulations from each word.
		unpackModulations(P, 0, 0, i32ModulationValues, i32ModulationModes, ui8Bpp);
		unpackModulations(Q, ui32WordWidth, 0, i32ModulationValues, i32ModulationModes, ui8Bpp);
		unpackModulations(R, 0, ui32WordHeight, i32ModulationValues, i32ModulationModes, ui8Bpp);
		unpackModulations(S, ui32WordWidth, ui32WordHeight, i32ModulationValues, i32ModulationModes, ui8Bpp);

		// Bilinear upscale image data from 2x2 -> 4x4
		interpolateColours(getColourA(P.u32ColourData), getColourA(Q.u32ColourData),
						   getColourA(R.u32ColourData), getColourA(S.u32ColourData),
						   upscaledColourA, ui8Bpp);
		interpolateColours(getColourB(P.u32ColourData), getColourB(Q.u32ColourData),
						   getColourB(R.u32ColourData), getColourB(S.u32ColourData),
						   upscaledColourB, ui8Bpp);

		for(uint32_t y = 0; y < ui32WordHeight; y++) {
			for(uint32_t x = 0; x < ui32WordWidth; x++) {
				int32_t mod = getModulationValues(i32ModulationValues, i32ModulationModes,
												  x+ui32WordWidth/2, y+ui32WordHeight/2, ui8Bpp);
				bool punchthroughAlpha = false;
				if(mod > 10) {
					punchthroughAlpha = true;
					mod -= 10;
				}

				Pixel128S result;				
				result.red   = (upscaledColourA[y*ui32WordWidth+x].red * (8-mod) + upscaledColourB[y*ui32WordWidth+x].red * mod) / 8;
				result.green = (upscaledColourA[y*ui32WordWidth+x].green * (8-mod) + upscaledColourB[y*ui32WordWidth+x].green * mod) / 8;
				result.blue  = (upscaledColourA[y*ui32WordWidth+x].blue * (8-mod) + upscaledColourB[y*ui32WordWidth+x].blue * mod) / 8;
				if (punchthroughAlpha) result.alpha = 0;
				else result.alpha = (upscaledColourA[y*ui32WordWidth+x].alpha * (8-mod) + upscaledColourB[y*ui32WordWidth+x].alpha * mod) / 8;

				//Convert the 32bit precision result to 8 bit per channel colour.
				if(ui8Bpp == 2) {
					pColourData[y*ui32WordWidth+x].red   = (uint8_t)result.red;
					pColourData[y*ui32WordWidth+x].green = (uint8_t)result.green;
					pColourData[y*ui32WordWidth+x].blue  = (uint8_t)result.blue;
					pColourData[y*ui32WordWidth+x].alpha = (uint8_t)result.alpha;
				} else if(ui8Bpp == 4) {
					pColourData[y+x*ui32WordHeight].red   = (uint8_t)result.red;
					pColourData[y+x*ui32WordHeight].green = (uint8_t)result.green;
					pColourData[y+x*ui32WordHeight].blue  = (uint8_t)result.blue;
					pColourData[y+x*ui32WordHeight].alpha = (uint8_t)result.alpha;				
				}
			}
		}
	}


	static void mapDecompressedData(const Pixel32* pWord, const PVRTCWordIndices& words, uint8_t ui8Bpp,
									shared_img img, bool flip)
	{
		uint32_t ui32WordWidth = 4;
		uint32_t ui32WordHeight = 4;
		if(ui8Bpp == 2) {
			ui32WordWidth = 8;
		}

		for(uint32_t y = 0; y < ui32WordHeight / 2; ++y) {
			for(uint32_t x = 0; x < ui32WordWidth / 2; ++x) {
				{
					vtx::spos pos;
					pos.x = (words.P[0] * ui32WordWidth)  + x + ui32WordWidth  / 2;
					short yy = (words.P[1] * ui32WordHeight) + y + ui32WordHeight / 2;
					if(flip) pos.y = img->get_size().y - yy - 1;
					else pos.y = yy;
					const Pixel32& wc = pWord[y * ui32WordWidth + x];
					rgba8 c(wc.red, wc.green, wc.blue, wc.alpha);
					img->put_pixel(pos, c);
				}
				{
					vtx::spos pos;
					pos.x = (words.Q[0] * ui32WordWidth)  + x;
					short yy = (words.Q[1] * ui32WordHeight) + y + ui32WordHeight / 2;
					if(flip) pos.y = img->get_size().y - yy - 1;
					else pos.y = yy;
					const Pixel32& wc = pWord[y * ui32WordWidth + x + ui32WordWidth / 2];
					rgba8 c(wc.red, wc.green, wc.blue, wc.alpha);
					img->put_pixel(pos, c);
				}
				{
					vtx::spos pos;
					pos.x = (words.R[0] * ui32WordWidth)  + x + ui32WordWidth / 2;
					short yy = (words.R[1] * ui32WordHeight) + y;
					if(flip) pos.y = img->get_size().y - yy - 1;
					else pos.y = yy;
					const Pixel32& wc = pWord[(y + ui32WordHeight / 2) * ui32WordWidth + x];
					rgba8 c(wc.red, wc.green, wc.blue, wc.alpha);
					img->put_pixel(pos, c);
				}
				{
					vtx::spos pos;
					pos.x = (words.S[0] * ui32WordWidth)  + x;
					short yy = (words.S[1] * ui32WordHeight) + y;
					if(flip) pos.y = img->get_size().y - yy - 1;
					else pos.y = yy;
					const Pixel32& wc = pWord[(y + ui32WordHeight / 2) * ui32WordWidth + x + ui32WordWidth / 2];
					rgba8 c(wc.red, wc.green, wc.blue, wc.alpha);
					img->put_pixel(pos, c);
				}
			}
		}
	}


	static uint32_t pvrtc_decompress(const void* pCompressedData, uint32_t ui32Width, uint32_t ui32Height, uint32_t ui8Bpp,
									 shared_img img, bool flip)
	{
		uint32_t ui32WordWidth = 4;
		uint32_t ui32WordHeight = 4;
		if(ui8Bpp == 2) {
			ui32WordWidth = 8;
		}

		const uint32_t* pWordMembers = static_cast<const uint32_t*>(pCompressedData);

		// Calculate number of words
		uint32_t i32NumXWords = ui32Width / ui32WordWidth;
		uint32_t i32NumYWords = ui32Height / ui32WordHeight;

		Pixel32* pPixels = new Pixel32[ui32WordWidth * ui32WordHeight];
		// Structs used for decompression
		PVRTCWordIndices indices;	
		// For each row of words
		for(int32_t wordY = -1; wordY < static_cast<int32_t>(i32NumYWords - 1); ++wordY) {
			// for each column of words
			for(int32_t wordX = -1; wordX < static_cast<int32_t>(i32NumXWords - 1); ++wordX) {
				indices.P[0] = wrapWordIndex(i32NumXWords, wordX);
				indices.P[1] = wrapWordIndex(i32NumYWords, wordY);
				indices.Q[0] = wrapWordIndex(i32NumXWords, wordX + 1);
				indices.Q[1] = wrapWordIndex(i32NumYWords, wordY);
				indices.R[0] = wrapWordIndex(i32NumXWords, wordX);
				indices.R[1] = wrapWordIndex(i32NumYWords, wordY + 1);
				indices.S[0] = wrapWordIndex(i32NumXWords, wordX + 1);
				indices.S[1] = wrapWordIndex(i32NumYWords, wordY + 1);

				//Work out the offsets into the twiddle structs, multiply by two as there are two members per word.
				uint32_t WordOffsets[4] = {
					TwiddleUV(i32NumXWords,i32NumYWords,indices.P[0], indices.P[1]) * 2,
					TwiddleUV(i32NumXWords,i32NumYWords,indices.Q[0], indices.Q[1]) * 2,
					TwiddleUV(i32NumXWords,i32NumYWords,indices.R[0], indices.R[1]) * 2,
					TwiddleUV(i32NumXWords,i32NumYWords,indices.S[0], indices.S[1]) * 2,
				};

				//Access individual elements to fill out PVRTCWord
				PVRTCWord P, Q, R, S;
				P.u32ColourData = pWordMembers[WordOffsets[0]+1];
				P.u32ModulationData = pWordMembers[WordOffsets[0]];
				Q.u32ColourData = pWordMembers[WordOffsets[1]+1];
				Q.u32ModulationData = pWordMembers[WordOffsets[1]];
				R.u32ColourData = pWordMembers[WordOffsets[2]+1];
				R.u32ModulationData = pWordMembers[WordOffsets[2]];
				S.u32ColourData = pWordMembers[WordOffsets[3]+1];
				S.u32ModulationData = pWordMembers[WordOffsets[3]];

				// assemble 4 words into struct to get decompressed pixels from
				pvrtcGetDecompressedPixels(P, Q, R, S, pPixels, ui8Bpp);
				mapDecompressedData(pPixels, indices, ui8Bpp, img, flip);
			}
		}

		delete[] pPixels;
		//Return the data size
		return ui32Width * ui32Height / (ui32WordWidth / 2);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	PVR ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool pvr_io::probe(utils::file_io& fin)
	{
		pvr_info_v2 v2;
		if(v2.probe(fin)) {
			return true;
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
	bool pvr_io::info(utils::file_io& fin, img::img_info& fo)
	{
		pvr_info_v2 v2;
		if(v2.probe(fin)) {
			fo.width = v2.width;
			fo.height = v2.height;
			fo.r_depth = 8;
			fo.g_depth = 8;
			fo.b_depth = 8;
			fo.a_depth = (v2.flags & pvr_info_v2::PVRTEX_ALPHA) != 0 ? 8 : 0;
			fo.i_depth = 0;
			fo.clut_num = 0;
			fo.mipmap_level = 0;
			fo.multi_level = 0;
			fo.grayscale = false;
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	PVR ファイル、ロード
		@param[in]	fin		file_io クラス
		@param[in]	opt	フォーマット固有の拡張文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool pvr_io::load(utils::file_io& fin, const std::string& opt)
	{
		pvr_info_v2 v2;
		if(v2.get(fin)) {

			std::vector<uint8_t> input;
			input.resize(v2.data_size);
			if(fin.read(&input[0], v2.data_size) != v2.data_size) {
				return false;
			}

			bool alpha = (v2.flags & pvr_info_v2::PVRTEX_ALPHA) != 0;
			img_ = shared_img(new img_rgba8);
			img_->create(vtx::spos(v2.width, v2.height), alpha);

			bool flip = (v2.flags & pvr_info_v2::PVRTEX_VERTICAL_FLIP) != 0;
			pvrtc_decompress(&input[0], v2.width, v2.height, v2.bit_count, img_, flip);
			return true;
		} else {

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
	bool pvr_io::save(utils::file_io& fout, const std::string& opt)
	{

		return true;
	}
}
