//=====================================================================//
/*!	@file
	@brief	漢字フォントイメージを freetype2 で扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "kfimg_ft2.hpp"
#include "utils/string_utils.hpp"

using namespace std;

namespace img {

	static kfimg_ft2* g_kanji_font_image = 0;

	//-----------------------------------------------------------------//
	/*!
		@brief	漢字フォントイメージのコンテキストを作成
	 */
	//-----------------------------------------------------------------//
	void create_kfimg()
	{
		if(g_kanji_font_image == 0) {
			g_kanji_font_image = new kfimg_ft2;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	漢字フォントイメージのコンテキストを取得
		@return クラスのポインター
	 */
	//-----------------------------------------------------------------//
	Ikfimg* get_kfimg()
	{
		Ikfimg* p = dynamic_cast<Ikfimg*>(g_kanji_font_image);
		if(p == 0) {
//			printf("Can't create kanji-font-image\n");
		}
		return p;
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	漢字フォントイメージのコンテキストを廃棄
	 */
	//-----------------------------------------------------------------//
	void destroy_kfimg()
	{
		delete g_kanji_font_image;
		g_kanji_font_image = 0;
	}


#if 0
void get_metrics(wchar_t code)
{
	FT_Error error;

	error = FT_Set_Pixel_Sizes(face_, w, h);

	FT_Vector	pen;
	pen.x = pen.y = 0;
	FT_Set_Transform(face_, &matrix_, &pen);

	if(antialias) {
		error = FT_Load_Char(face_, code, FT_LOAD_RENDER);
	} else {
		error = FT_Load_Char(face_, code, FT_LOAD_MONOCHROME);
	}

	FT_GlyphSlot slot = face_->glyph;
	FT_Bitmap* bitmap = &slot->bitmap;

	metrics_.bitmap_w = (float)bitmap->width;
	metrics_.bitmap_h = (float)bitmap->rows;
	metrics_.width    = (float)slot->metrics.width  / 64.0f;
	metrics_.height   = (float)slot->metrics.height / 64.0f;
	metrics_.hori_x   = (float)slot->metrics.horiBearingX / 64.0f;
	metrics_.hori_y   = (float)slot->metrics.horiBearingY / 64.0f;
	metrics_.vert_x   = (float)slot->metrics.vertBearingX / 64.0f;
	metrics_.vert_y   = (float)slot->metrics.vertBearingY / 64.0f;

}
#endif

	//-----------------------------------------------------------------//
	/*!
		@brief	フォントを指定
		@param[in]	fontfile	フォント・ファイル名
		@param[in]	alias		フォントの別名
		@return 成功した場合は「true」
	 */
	//-----------------------------------------------------------------//
	bool kfimg_ft2::install_font_type(const std::string& fontfile, const std::string& alias)
	{
		string name;

		if(alias.empty()) {
			name = utils::get_file_name(fontfile);
		} else {
			name = alias;
		}

		face_map_it it = find_face_(name);
		if(it != face_map_.end()) {
			alias_ = it->first;
			face_  = it->second;
			return true;
		}

		FT_Error error = FT_New_Face(library_, fontfile.c_str(), 0, &face_);
		if(error) {
			face_ = 0;
			alias_.clear();
			return false;
		}

///		printf("kfimg install: (%s) 0x%08x\n", name.c_str(), (int)face_);
///		fflush(stdout);

		alias_ = name;
		install_face_(name, face_);

		FT_Vector pen;
		pen.x = pen.y = 0;
		FT_Set_Transform(face_, &matrix_, &pen);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フォントのビットマップを作成する(FreeType2 版）
		@param[in]	w	フォントの横幅
		@param[in]	h	フォントの高さ（優先）
		@param[in]	code	対応するコード
	*/
	//-----------------------------------------------------------------//
	void kfimg_ft2::create_font_bitmap_(img_gray8& img, const vtx::spos& size, wchar_t code, bool antialias)
	{
		FT_Error error;

		error = FT_Set_Pixel_Sizes(face_, size.x, size.y);

		// 基準点へのオフセットの計算
		if(offset_y_.find(size.y) == offset_y_.end()) {
			error = FT_Load_Char(face_, '~', FT_LOAD_RENDER);
			FT_GlyphSlot slot = face_->glyph;
			std::pair<int, int> v(size.y, (int)(slot->metrics.horiBearingY / 64.0f));
			offset_y_.insert(v);
		}

		if(antialias) {
			error = FT_Load_Char(face_, code, FT_LOAD_RENDER);
		} else {
			error = FT_Load_Char(face_, code, FT_LOAD_MONOCHROME);
		}

		FT_GlyphSlot slot = face_->glyph;
		FT_Bitmap* bitmap = &slot->bitmap;

		metrics_.bitmap_w = (float)bitmap->width;
		metrics_.bitmap_h = (float)bitmap->rows;
		metrics_.width    = (float)slot->metrics.width  / 64.0f;
		metrics_.height   = (float)slot->metrics.height / 64.0f;
		metrics_.hori_x   = (float)slot->metrics.horiBearingX / 64.0f;
		metrics_.hori_y   = (float)slot->metrics.horiBearingY / 64.0f;
		metrics_.vert_x   = (float)slot->metrics.vertBearingX / 64.0f;
		metrics_.vert_y   = (float)slot->metrics.vertBearingY / 64.0f;

		int ox = static_cast<int>(metrics_.hori_x);
		int oy = offset_y_[size.y] - static_cast<int>(metrics_.hori_y);
//	printf("%04X - bitmap num: %d\n", code, bitmap->pixel_mode);
	// グレイスケールでレンダリング出来なかった場合は、モノカラーとなる。
		if(bitmap->pixel_mode != FT_PIXEL_MODE_MONO) {		// gray-scale 0 to 255
			for(int j = 0; j < bitmap->rows; j++) {
				for(int i = 0; i < bitmap->width; i++) {
					img::gray8 c;
					c.g = bitmap->buffer[j * bitmap->width + i];
					img.put_pixel(ox + i, oy + j, c);
				}
			}
		} else {	// monochrome
			int bitpos = 0;
			for(int j = 0; j < bitmap->rows; j++) {
				for(int i = 0; i < bitmap->width; i++) {
					img::gray8 c;
					if(bitmap->buffer[bitpos >> 3] & (1 << (~bitpos & 7))) c.g = 255; else c.g = 0;
					bitpos++;
					img.put_pixel(ox + i, oy + j, c);
				}
				if(bitpos & 7) {
					bitpos |= 7;
					bitpos++;
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	unicode に対応するビットマップを生成する。
		@param[in]	size	生成するビットマップのサイズ
		@param[in]	unicode	生成するビットマップの UNICODE
	 */
	//-----------------------------------------------------------------//
	void kfimg_ft2::create_bitmap(const vtx::spos& size, wchar_t unicode)
	{
		img_.create(size);
		gray8 g(0);
		img_.fill(g);
		create_font_bitmap_(img_, size, unicode, antialias_);
	}

}
